/*实现目标:
 *    cwf 构造流程
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* JSON解析库 */
#include "cJSON.h"

/*@brief cJSON数值项转整数
 *@param item cJSON项
 *@retval 数值(非数值项为0)
 */
static int32_t scui_cwf_json_number(cJSON *item)
{
    if (!cJSON_IsNumber(item)) return 0;
    double val = cJSON_GetNumberValue(item);
    return (int32_t)(val < 0 ? val - 0.5 : val + 0.5);
}

/*@brief 固件align配对
 *@param 协议align
 *@retval 固件align
 */
static scui_align_t scui_cwf_json_align(scui_cwf_json_align_t align)
{
    switch (align) {
    default:                      return scui_align_itl;
    case scui_cwf_json_align_itl: return scui_align_itl;
    case scui_cwf_json_align_itm: return scui_align_itm;
    case scui_cwf_json_align_itr: return scui_align_itr;
    case scui_cwf_json_align_ibl: return scui_align_ibl;
    case scui_cwf_json_align_ibm: return scui_align_ibm;
    case scui_cwf_json_align_ibr: return scui_align_ibr;
    case scui_cwf_json_align_ilm: return scui_align_ilm;
    case scui_cwf_json_align_irm: return scui_align_irm;
    case scui_cwf_json_align_icc: return scui_align_icc;
    case scui_cwf_json_align_otl: return scui_align_otl;
    case scui_cwf_json_align_otm: return scui_align_otm;
    case scui_cwf_json_align_otr: return scui_align_otr;
    case scui_cwf_json_align_obl: return scui_align_obl;
    case scui_cwf_json_align_obm: return scui_align_obm;
    case scui_cwf_json_align_obr: return scui_align_obr;
    case scui_cwf_json_align_olt: return scui_align_olt;
    case scui_cwf_json_align_olm: return scui_align_olm;
    case scui_cwf_json_align_olb: return scui_align_olb;
    case scui_cwf_json_align_ort: return scui_align_ort;
    case scui_cwf_json_align_orm: return scui_align_orm;
    case scui_cwf_json_align_orb: return scui_align_orb;
    }
}

/*@brief 当前语言协议
 *@retval 语言协议
 */
static scui_cwf_json_lang_t scui_cwf_json_lang(void)
{
    scui_lang_type_t lang = 0;
    scui_lang_get(&lang);
    
    switch (lang) {
    default:                 return scui_cwf_json_lang_en;
    case scui_lang_type_en:  return scui_cwf_json_lang_en;
    case scui_lang_type_zh:  return scui_cwf_json_lang_zh;
    case scui_lang_type_de:  return scui_cwf_json_lang_de;
    case scui_lang_type_fra: return scui_cwf_json_lang_fra;
    case scui_lang_type_nl:  return scui_cwf_json_lang_nl;
    case scui_lang_type_pt:  return scui_cwf_json_lang_pt;
    case scui_lang_type_jp:  return scui_cwf_json_lang_jp;
    case scui_lang_type_ar:  return scui_cwf_json_lang_ar;
    case scui_lang_type_fa:  return scui_cwf_json_lang_fa;
    }
}

/*@brief 元素控件尺寸(区域为0时按内容算)
 *@param res 元素资源
 *@param hit seq图集句柄序列
 *@param num seq图集序列长度
 */
static void scui_cwf_json_item_size(scui_cwf_json_item_res_t *res, scui_handle_t *hit, uint16_t num)
{
    if (res->area_h == 0)
        res->area_h  = res->img_h;
    
    /* seq 未给场宽(nums=0): 盒恒贴内容, 宽度随位数变, 每次都重算 */
    if (res->type == scui_cwf_json_type_seq && res->nums == 0 && num != 0) {
        res->area_w = scui_image_list_calc(hit, num, false) + (num - 1) * res->span;
        return;
    }
    
    if (res->area_w == 0) {
        res->area_w  = res->img_w;
        
        if (res->type == scui_cwf_json_type_seq) {
            
            if (num != 0) {
                /* 有句柄序列按实测内容累加, 构建期还没有序列则按场宽推演 */
                res->area_w = scui_image_list_calc(hit, num, false) + (num - 1) * res->span;
                return;
            }
            
            uint16_t width = res->nums ? res->nums : 1;
            res->area_w = width * res->img_w + (width - 1) * res->span;
        }
    }
}

/*@brief 元素落位(线对齐 / 参照前序元素的外部对齐)
 *@param parser 解析器
 *@param idx    指定项
 *@param res    元素资源
 */
static void scui_cwf_json_item_place(scui_cwf_json_parser_t *parser, uint32_t idx, scui_cwf_json_item_res_t *res)
{
    scui_handle_t handle = parser->list_child[idx];
    scui_align_t  align  = scui_cwf_json_align(res->align);
    scui_point_t  offset = {0};
    
    if (handle == SCUI_HANDLE_INVALID) return;
    /* 外部对齐: 参照目标(align_ofs指定前序元素; 无目标则相对父控件) */
    if (scui_opt_bits_check(align, scui_align_mask_oxl | scui_align_mask_oxr) ||
        scui_opt_bits_check(align, scui_align_mask_oyt | scui_align_mask_oyb)) {
        
        scui_handle_t target = SCUI_HANDLE_INVALID;
        int32_t idx_t = (int32_t)idx + res->align_ofs;
        if (res->align_ofs != 0 && idx_t >= 0 && idx_t < (int32_t)parser->list_num)
            target = parser->list_child[idx_t];
        
        if (scui_opt_bits_equal(align, scui_align_mask_oxl)) offset.x = -(scui_coord_t)res->span;
        if (scui_opt_bits_equal(align, scui_align_mask_oxr)) offset.x = +(scui_coord_t)res->span;
        if (scui_opt_bits_equal(align, scui_align_mask_oyt)) offset.y = -(scui_coord_t)res->span;
        if (scui_opt_bits_equal(align, scui_align_mask_oyb)) offset.y = +(scui_coord_t)res->span;
        
        scui_widget_align_pos(handle, target, align, &offset);
        return;
    }
    
    offset.x = res->area_x;
    offset.y = res->area_y;
    /* 线对齐: offset 取父相对坐标, align_pos 内部会自行叠加父控件坐标 */
    if (scui_opt_bits_equal(align, scui_align_mask_ixm)) offset.x -= res->area_w / 2;
    if (scui_opt_bits_equal(align, scui_align_mask_ixr)) offset.x -= res->area_w;
    if (scui_opt_bits_equal(align, scui_align_mask_iym)) offset.y -= res->area_h / 2;
    if (scui_opt_bits_equal(align, scui_align_mask_iyb)) offset.y -= res->area_h;
    scui_widget_align_pos(handle,  SCUI_HANDLE_INVALID, scui_align_itl, &offset);
}

/*@brief 解析器指定项更新
 *@param parser 解析器
 *@param idx    指定项
 *@param tick   流失ms
 */
void scui_cwf_json_anim_item(scui_cwf_json_parser_t *parser, uint32_t idx, uint32_t tick)
{
    scui_handle_t handle = parser->list_child[idx];
    scui_cwf_json_item_res_t *res = parser->list_src[idx];
    if (handle == SCUI_HANDLE_INVALID || res == NULL) return;
    
    /* 表针不参与值刷新 */
    if (res->key == scui_cwf_json_key_watch)
        return;
    
    /* 帧动画: 按元素自己的间隔推进 */
    if (res->type == scui_cwf_json_type_anim) {
        res->anim_tick += tick;
        
        if (res->anim_tick < res->anima_ms) return;
        else res->anim_tick -= res->anima_ms;
        
        if (res->idx_anim >= res->img_num) res->idx_anim = 0;
        else res->idx_anim += 1;
        
        scui_widget_image_set(handle, res->parser->image_hit[res->img_res[res->idx_anim]]);
        scui_widget_draw(handle, NULL, false, 0);
        return;
    }
    
    /* 跳过间隔门限 */
    if (parser->refr_tick < SCUI_CWF_JSON_ANIMA_REFR)
        return;
    
    scui_coord_t val = 0;
    /* 值刷新: 按类型更新(seq/map/group...) */
    if (scui_cwf_json_source_value(res->source, &val)) {
        switch (res->type) {
        case scui_cwf_json_type_seq: {
            scui_handle_t hit[SCUI_CWF_JSON_SEQ_MAX] = {0};
            
            /* 值转化为seq图集句柄序列(自然数字) */
            char text[SCUI_CWF_JSON_SEQ_MAX + 2] = {0};
            snprintf(text, sizeof(text), "%u", (uint32_t)val);
            
            /* 协议字符位 -> 图集句柄 */
            scui_handle_t map[SCUI_CWF_JSON_SEQ_NUM] = {0};
            uint16_t map_num = scui_min(res->img_num, SCUI_CWF_JSON_SEQ_NUM);
            for (uint16_t idx = 0; idx < map_num; idx++) {
                map[idx] = res->parser->image_hit[res->img_res[idx]];
            }
            
            res->idx_num = (uint16_t)scui_image_list_remap(hit, SCUI_CWF_JSON_SEQ_MAX,
                map, map_num, SCUI_CWF_JSON_SEQ_SET, SCUI_CWF_JSON_SEQ_NUM, text);
            
            scui_cwf_json_item_size(res, hit, res->idx_num);
            scui_widget_adjust_size(handle, res->area_w, res->area_h);
            
            /* 盒内右对齐: 逐位留白由align承担 */
            scui_ximage_sequence(handle, hit, res->idx_num, SCUI_COLOR_UNUSED,
                scui_align_itr, res->span, false);
            break;
        }
        case scui_cwf_json_type_map: {
            /* 值映射到图集下标 */
            scui_coord_t val_min = scui_cwf_json_source_info[res->source].val_min;
            scui_coord_t val_max = scui_cwf_json_source_info[res->source].val_max;
            scui_cwf_json_source_range(res->source, &val_min, &val_max);
            
            uint16_t idx = (uint16_t)scui_map(val, val_min, val_max, 0, res->img_num - 1);
            scui_widget_image_set(handle, res->parser->image_hit[res->img_res[idx]]);
            scui_widget_draw(handle, NULL, false, 0);
            break;
        }
        case scui_cwf_json_type_group: {
            /* 组下标(值 x 语言挡位) */
            scui_coord_t val_min = scui_cwf_json_source_info[res->source].val_min;
            scui_coord_t val_max = scui_cwf_json_source_info[res->source].val_max;
            uint16_t idx = 0, base = (uint16_t)(val_max - val_min + 1);
            
            if (base != 0) {
                uint16_t slot_num = res->img_num / base;
                uint16_t slot_cur = scui_cwf_json_lang();
                
                if (slot_num == 0 || slot_cur >= slot_num) slot_cur = 0;
                uint16_t index = slot_cur * base + (uint16_t)(val - val_min);
                idx = index < res->img_num ? index : 0;
            }
            
            scui_widget_image_set(handle, res->parser->image_hit[res->img_res[idx]]);
            scui_widget_draw(handle, NULL, false, 0);
            break;
        }
        default:
            break;
        }
    }
}

/*@brief 解析器指定项销毁
 *@param parser 解析器
 *@param idx    指定项
 */
void scui_cwf_json_burn_item(scui_cwf_json_parser_t *parser, uint32_t idx)
{
    scui_cwf_json_item_res_t *res = parser->list_src[idx];
    parser->list_src[idx] = NULL;
    
    if (res == NULL)
        return;
    
    SCUI_MEM_FREE(res->img_res);
    SCUI_MEM_FREE(res);
}

/*@brief 解析器指定项构造(字段读取与控件构造按类型原地展开)
 *@param parser 解析器
 *@param idx    指定项
 *@param dict   构造参数
 *@param parent 父控件
 */
void scui_cwf_json_make_item(scui_cwf_json_parser_t *parser, uint32_t idx, cJSON *dict,
                             scui_handle_t parent)
{
    scui_cwf_json_item_res_t *res = SCUI_MEM_ZALLOC(scui_mem_type_user, sizeof(scui_cwf_json_item_res_t));
    parser->list_src[idx] = res; res->parser = parser; res->list_idx = idx;
    
    /* 必读: 渲染类别与元素角色(决定后续分支) */
    res->type = (uint8_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "type"));
    res->key  = (uint8_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "key"));
    parser->list_type[idx] = res->type;
    
    /* 预览图不参与绘制(scui_cwf_json_make_pv单独提取) */
    if (res->key == scui_cwf_json_key_preview) return;
    
    /* 通用定位: 所有构造元素共用 */
    res->span       = (uint16_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "span"));
    res->align      = ( uint8_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "align"));
    res->align_ofs  = (  int8_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "align_ofs"));
    res->area_x = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "area_x"));
    res->area_y = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "area_y"));
    
    /* 资源装载 */
    switch (res->type) {
    case scui_cwf_json_type_img:
    case scui_cwf_json_type_map:
    case scui_cwf_json_type_seq:
    case scui_cwf_json_type_anim:
    case scui_cwf_json_type_group: {
        cJSON *json_res = cJSON_GetObjectItem(dict, "img_res");
        
        if (cJSON_IsArray(json_res)) {
            res->img_num = (uint16_t)cJSON_GetArraySize(json_res);
            res->img_res = SCUI_MEM_ALLOC(scui_mem_type_user, res->img_num * sizeof(uint16_t));
            
            for (uint16_t img_idx = 0; img_idx < res->img_num; img_idx++) {
                res->img_res[img_idx] = (uint16_t)scui_cwf_json_number(cJSON_GetArrayItem(json_res, img_idx));
                SCUI_ASSERT(res->img_res[img_idx] < parser->image_num);
            }
            
            res->img_w = scui_image_w(parser->image_hit[res->img_res[0]]);
            res->img_h = scui_image_h(parser->image_hit[res->img_res[0]]);
        }
        break;
    }
    case scui_cwf_json_type_font:
        /* 将来: 字体资源装载 */
        break;
    default:
        break;
    }
    
    /* 特殊类型匹配 */
    switch (res->key) {
    case scui_cwf_json_key_layout:
        /* key:layout 容器: 子元素数量(层级栈用) */
        res->child = (uint16_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "child"));
        break;

    case scui_cwf_json_key_watch:{
        scui_point_t watch_anchor[3] = {0};
        scui_point_t watch_center[3] = {0};
        SCUI_ASSERT(res->img_num == 3);
        
        /* key:watch 表针: 锚点/旋转中心只在本构造用一次, 不落结构 */
        watch_anchor[0].x = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "hx"));
        watch_anchor[0].y = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "hy"));
        watch_anchor[1].x = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "mx"));
        watch_anchor[1].y = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "my"));
        watch_anchor[2].x = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "sx"));
        watch_anchor[2].y = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "sy"));
        watch_center[0].x = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "h_cx"));
        watch_center[0].y = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "h_cy"));
        watch_center[1].x = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "m_cx"));
        watch_center[1].y = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "m_cy"));
        watch_center[2].x = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "s_cx"));
        watch_center[2].y = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "s_cy"));
        
        scui_xwatch_maker_define(xwatch_maker);
        xwatch_maker.widget.clip   = scui_widget_area(parent);
        xwatch_maker.widget.parent = parent;
        xwatch_maker.image[0]  = parser->image_hit[res->img_res[0]];
        xwatch_maker.image[1]  = parser->image_hit[res->img_res[1]];
        xwatch_maker.image[2]  = parser->image_hit[res->img_res[2]];
        xwatch_maker.anchor[0] = watch_anchor[0];
        xwatch_maker.anchor[1] = watch_anchor[1];
        xwatch_maker.anchor[2] = watch_anchor[2];
        xwatch_maker.center[0] = watch_center[0];
        xwatch_maker.center[1] = watch_center[1];
        xwatch_maker.center[2] = watch_center[2];
        xwatch_maker.get_h  = scui_cwf_json_source_tick_h;
        xwatch_maker.get_m  = scui_cwf_json_source_tick_m;
        xwatch_maker.get_s  = scui_cwf_json_source_tick_s;
        xwatch_maker.get_ms = scui_cwf_json_source_tick_ms;
        xwatch_maker.tick_mode = 2;
        scui_widget_create(&xwatch_maker, &parser->list_child[idx]);
        goto place;
    }
    }
    
    /* 按渲染类别读取专属字段并原地构造 */
    switch (res->type) {
    case scui_cwf_json_type_img:
    case scui_cwf_json_type_map:
    case scui_cwf_json_type_group:
    case scui_cwf_json_type_anim: {
        scui_custom_maker_define(custom_maker);
        res->area_w = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "area_w"));
        res->area_h = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "area_h"));
        if (res->type == scui_cwf_json_type_map || res->type == scui_cwf_json_type_group)
            res->source = (uint8_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "source"));
        if (res->type == scui_cwf_json_type_anim) {
            res->anima_ms = (uint16_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "anima_ms"));
            if (res->anima_ms == 0) res->anima_ms = SCUI_CWF_JSON_ANIMA_DEF;
        }
        
        SCUI_ASSERT(res->img_num != 0);
        scui_cwf_json_item_size(res, NULL, 0);
        
        custom_maker.widget.clip.x = res->area_x;
        custom_maker.widget.clip.y = res->area_y;
        custom_maker.widget.clip.w = res->area_w;
        custom_maker.widget.clip.h = res->area_h;
        custom_maker.widget.parent = parent;
        custom_maker.widget.style.fully_bg = true;
        custom_maker.widget.image = parser->image_hit[res->img_res[0]];
        scui_widget_create(&custom_maker, &parser->list_child[idx]);
        break;
    }
    case scui_cwf_json_type_seq: {
        scui_ximage_maker_define(ximage_maker);
        res->nums   = (uint16_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "nums"));
        res->source = ( uint8_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "source"));
        res->area_w = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "area_w"));
        res->area_h = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "area_h"));
        
        SCUI_ASSERT(res->img_num != 0);
        scui_cwf_json_item_size(res, NULL, 0);
        
        ximage_maker.widget.clip.x = res->area_x;
        ximage_maker.widget.clip.y = res->area_y;
        ximage_maker.widget.clip.w = res->area_w;
        ximage_maker.widget.clip.h = res->area_h;
        ximage_maker.widget.parent = parent;
        scui_widget_create(&ximage_maker, &parser->list_child[idx]);
        break;
    }
    case scui_cwf_json_type_none: {
        scui_custom_maker_define(custom_maker);
        scui_area_t clip_p = scui_widget_area(parent);
        res->area_w = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "area_w"));
        res->area_h = (scui_coord_t)scui_cwf_json_number(cJSON_GetObjectItem(dict, "area_h"));
        
        custom_maker.widget.clip.x = res->area_x;
        custom_maker.widget.clip.y = res->area_y;
        custom_maker.widget.clip.w = res->area_w ? res->area_w : clip_p.w;
        custom_maker.widget.clip.h = res->area_h ? res->area_h : clip_p.h;
        custom_maker.widget.parent = parent;
        scui_widget_create(&custom_maker, &parser->list_child[idx]);
        break;
    }
    case scui_cwf_json_type_font:
        SCUI_LOG_WARN("cwf json font unsupported");
        break;
    default:
        SCUI_LOG_WARN("cwf json type unknown:%d", res->type);
        break;
    }
    
    place:
    if (parser->list_child[idx] != SCUI_HANDLE_INVALID) {
        scui_widget_user_data_set(parser->list_child[idx], res);
        scui_cwf_json_item_place(parser, idx, res);
    }
}
