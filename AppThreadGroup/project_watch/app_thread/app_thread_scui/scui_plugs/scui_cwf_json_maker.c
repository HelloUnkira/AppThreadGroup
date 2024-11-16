/*实现目标:
 *    cwf 构造流程
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

// JSON解析库
#include "cJSON.h"

/*@brief 内部函数:将val转为idx_ofs数组
 */
static void scui_cwf_json_val_to_idx_ofs(scui_cwf_json_parser_t *parser, uint32_t idx, uint32_t val, const char *format, uint8_t fix)
{
    scui_csf_json_item__res_t *res = parser->list_src[idx];
    scui_handle_t handle = parser->list_child[res->list_idx];
    uint16_t list_idx = res->list_idx;
    
    uint8_t bit = fix == 0 ? scui_dec_bits(val): fix;
    scui_area_t area = scui_widget_clip(handle);
    area.w = bit * res->img_w + (bit - 1) * res->img_span;
    scui_widget_adjust_size(handle, area.w, area.h);
    SCUI_ASSERT(bit != 0);
    
    SCUI_MEM_FREE(res->idx_ofs);
    res->idx_num = bit;
    res->idx_ofs = SCUI_MEM_ALLOC(scui_mem_type_mix, (res->idx_num + 1) * sizeof(uint8_t));
    snprintf(res->idx_ofs, res->idx_num + 1, format, val);
    for (uint16_t idx = 0; idx < res->idx_num; idx++)
        res->idx_ofs[idx] -= '0';
}

/*@brief 事件处理回调
 *@param event 事件
 */
static void scui_cwf_json_custom_event_cb(scui_event_t *event)
{
    // 固定资源绑定到res, 构建时将widget与固定的res绑定
    void *user_data = scui_widget_user_data_get(event->object);
    scui_csf_json_item__res_t *res = user_data;
    SCUI_ASSERT(res != NULL);
    
    // 从res中逆向获取解析器目标
    scui_cwf_json_parser_t *parser = res->parser;
    uint16_t list_idx = res->list_idx;
    
    switch (event->type) {
    case scui_event_draw: {
        scui_widget_event_mask_keep(event);
        if (!scui_widget_event_check_execute(event))
             return;
        
        // 没有信息则不绘制
        if (res->idx_ofs == NULL)
            break;
        
        scui_area_t clip = scui_widget_clip(event->object);
        for (int32_t idx = 0; idx < res->idx_num; idx++) {
            uint16_t img_ofs = res->img_ofs[res->idx_ofs[idx]];
            scui_handle_t image = parser->image_hit[img_ofs];
            SCUI_ASSERT(img_ofs < parser->image_num);
            
            
            // 要么全部绘制图片, 要么只绘制指定位置的图片
            if (res->img_bits == 0 || res->img_bits - 1 == idx) {
                clip.w = scui_image_w(image);
                clip.h = scui_image_h(image);
                scui_widget_draw_image(event->object, &clip, image, NULL, (scui_color_t){0});
            }
            // 迭代到下一个绘制目标
            clip.x += clip.w + res->img_span;
        }
        break;
    }
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 解析器指定项更新
 *@param parser 解析器
 *@param idx    指定项
 */
void scui_cwf_json_anim_item(scui_cwf_json_parser_t *parser, uint32_t idx)
{
    if (parser->list_child[idx] == SCUI_HANDLE_INVALID)
        return;
    
    scui_csf_json_item__res_t *res = parser->list_src[idx];
    scui_handle_t handle = parser->list_child[res->list_idx];
    uint16_t list_idx = res->list_idx;
    
    // 目前还未为type开发太多效果
    switch (parser->list_type[idx]) {
    case scui_cwf_json_type_img: {
        
        
        
        // 为type_sub进行构建
        switch (parser->list_type_sub[idx]) {
        case scui_cwf_json_type_img_preview:
        case scui_cwf_json_type_img_simple:
        case scui_cwf_json_type_img_watch:
            /* skip */
            break;
        case scui_cwf_json_type_img_day: {
            uint8_t val = scui_ui_presenter.get_day();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%u", 0);
            break;
        }
        case scui_cwf_json_type_img_hour: {
            uint8_t val = scui_ui_presenter.get_hour();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%02u", 2);
            break;
        }
        case scui_cwf_json_type_img_minute: {
            uint8_t val = scui_ui_presenter.get_minute();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%02u", 2);
            break;
        }
        case scui_cwf_json_type_img_second: {
            uint8_t val = scui_ui_presenter.get_second();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%02u", 2);
            break;
        }
        case scui_cwf_json_type_img_hr: {
            uint32_t val = scui_ui_presenter.get_hr_cur();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%03u", 3);
            break;
        }
        case scui_cwf_json_type_img_kcal: {
            uint32_t val = scui_ui_presenter.get_kcal_cur();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%04u", 4);
            break;
        }
        case scui_cwf_json_type_img_step: {
            uint32_t val = scui_ui_presenter.get_step_cur();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%05u", 5);
            break;
        }
        case scui_cwf_json_type_img_batt: {
            uint32_t val = scui_ui_presenter.get_batt_cur();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%03u", 3);
            break;
        }
        case scui_cwf_json_type_img_dist: {
            uint32_t val = scui_ui_presenter.get_dist_cur();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%04u", 4);
            break;
        }
        case scui_cwf_json_type_img_temp: {
            uint32_t val = scui_ui_presenter.get_temp_cur();
            scui_cwf_json_val_to_idx_ofs(parser, idx, val, "%04u", 4);
            break;
        }
        case scui_cwf_json_type_img_month: {
            uint8_t val = scui_ui_presenter.get_month();
            scui_handle_t image = parser->image_hit[res->img_ofs[val]];
            scui_widget_image_set(handle, image);
            break;
        }
        case scui_cwf_json_type_img_week: {
            uint8_t val = scui_ui_presenter.get_week();
            scui_handle_t image = parser->image_hit[res->img_ofs[val]];
            scui_widget_image_set(handle, image);
            break;
        }
        case scui_cwf_json_type_img_ampm: {
            if (scui_ui_presenter.is_24()) {
                scui_widget_hide(handle, true);
            } else {
                scui_widget_show(handle, true);
                uint8_t val = scui_ui_presenter.get_hour();
                val = val >= 12 ? 1 : 0;
                scui_handle_t image = parser->image_hit[res->img_ofs[val]];
                scui_widget_image_set(handle, image);
            }
            break;
        }
        case scui_cwf_json_type_img_temp_unit: {
            uint8_t val = scui_ui_presenter.get_temp_unit() ? 0 : 1;
            scui_handle_t image = parser->image_hit[res->img_ofs[val]];
            scui_widget_image_set(handle, image);
            break;
        }
        case scui_cwf_json_type_img_hr_prog : {
            int64_t val_min = scui_ui_presenter.get_hr_min();
            int64_t val_max = scui_ui_presenter.get_hr_max();
            int64_t val_cur = scui_ui_presenter.get_hr_cur();
            int64_t val = scui_map(val_cur, val_min, val_max, 0, res->img_num - 1);
            scui_handle_t image = parser->image_hit[res->img_ofs[val]];
            scui_widget_image_set(handle, image);
            break;
        }
        case scui_cwf_json_type_img_kcal_prog : {
            int64_t val_min = scui_ui_presenter.get_kcal_min();
            int64_t val_max = scui_ui_presenter.get_kcal_max();
            int64_t val_cur = scui_ui_presenter.get_kcal_cur();
            int64_t val = scui_map(val_cur, val_min, val_max, 0, res->img_num - 1);
            scui_handle_t image = parser->image_hit[res->img_ofs[val]];
            scui_widget_image_set(handle, image);
            break;
        }
        case scui_cwf_json_type_img_step_prog : {
            int64_t val_min = scui_ui_presenter.get_step_min();
            int64_t val_max = scui_ui_presenter.get_step_max();
            int64_t val_cur = scui_ui_presenter.get_step_cur();
            int64_t val = scui_map(val_cur, val_min, val_max, 0, res->img_num - 1);
            scui_handle_t image = parser->image_hit[res->img_ofs[val]];
            scui_widget_image_set(handle, image);
            break;
        }
        case scui_cwf_json_type_img_batt_prog : {
            int64_t val_min = scui_ui_presenter.get_batt_min();
            int64_t val_max = scui_ui_presenter.get_batt_max();
            int64_t val_cur = scui_ui_presenter.get_batt_cur();
            int64_t val = scui_map(val_cur, val_min, val_max, 0, res->img_num - 1);
            scui_handle_t image = parser->image_hit[res->img_ofs[val]];
            scui_widget_image_set(handle, image);
            break;
        }
        case scui_cwf_json_type_img_dist_prog : {
            int64_t val_min = scui_ui_presenter.get_dist_min();
            int64_t val_max = scui_ui_presenter.get_dist_max();
            int64_t val_cur = scui_ui_presenter.get_dist_cur();
            int64_t val = scui_map(val_cur, val_min, val_max, 0, res->img_num - 1);
            scui_handle_t image = parser->image_hit[res->img_ofs[val]];
            scui_widget_image_set(handle, image);
            break;
        }
        case scui_cwf_json_type_img_anim: {
            res->idx_anim += 1;
            if (res->idx_anim >= res->img_num)
                res->idx_anim  = -1;
            
            uint8_t val = scui_clamp(res->idx_anim, 0, res->img_num - 1);
            scui_handle_t image = parser->image_hit[res->img_ofs[val]];
            scui_widget_image_set(handle, image);
            break;
        }
        default:
            SCUI_LOG_ERROR("unknown type_sub:%d", parser->list_type_sub[idx]);
            break;
        }
        
        
        
        break;
    }
    case scui_cwf_json_type_txt: {
        
        
        
        // 为type_sub进行构建
        switch (parser->list_type_sub[idx]) {
        
        default:
            SCUI_LOG_ERROR("unknown type_sub:%d", parser->list_type_sub[idx]);
            break;
        }
        
        
        
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown type:%d", parser->list_type[idx]);
        break;
    }
}

/*@brief 解析器指定项销毁
 *@param parser 解析器
 *@param idx    指定项
 */
void scui_cwf_json_burn_item(scui_cwf_json_parser_t *parser, uint32_t idx)
{
    scui_csf_json_item__res_t *res = parser->list_src[idx];
    parser->list_src[idx] = NULL;
    
    // 目前还未为type开发太多效果
    switch (parser->list_type[idx]) {
    case scui_cwf_json_type_img: {
        
        // 回收动态生成的子资源
        SCUI_MEM_FREE(res->idx_ofs);
        
        // 回收动态生成的子资源
        SCUI_MEM_FREE(res->img_ofs);
        
        break;
    }
    case scui_cwf_json_type_txt: {
        
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown type:%d", parser->list_type[idx]);
        break;
    }
    
    SCUI_MEM_FREE(res);
}

/*@brief 解析器指定项构造
 *@param parser 解析器
 *@param idx    指定项
 *@param dict   构造参数
 */
void scui_cwf_json_make_item(scui_cwf_json_parser_t *parser, uint32_t idx, cJSON *dict)
{
    scui_csf_json_item__res_t *res = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_csf_json_item__res_t));
    memset(res, 0, sizeof(scui_csf_json_item__res_t));
    parser->list_src[idx] = res;
    
    // 按协议解析字段 (进行一级解析)
    // 按协议解析字段 (进行二级解析)
    parser->list_type[idx]     = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "type")) + 0.1;
    parser->list_type_sub[idx] = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "type_sub")) + 0.1;
    
    // 目前还未为type开发太多效果
    switch (parser->list_type[idx]) {
    case scui_cwf_json_type_img: {
        // 每一个该type都有类似的资源表, 直接构建即可
        cJSON *json_src = cJSON_GetObjectItem(dict, "image_src");
        cJSON *json_num = cJSON_GetObjectItem(dict, "image_num");
        res->img_num = cJSON_GetNumberValue(json_num) + 0.1;
        res->img_ofs = SCUI_MEM_ALLOC(scui_mem_type_mix, res->img_num * sizeof(uint16_t));
        SCUI_ASSERT(res->img_num != 0);
        // 继续构建资源索引, 以便将来快速访问image_hit
        for (uint32_t idx = 0; idx < res->img_num; idx++) {
             uint32_t ofs = cJSON_GetNumberValue(cJSON_GetArrayItem(json_src, idx)) + 0.1;
             SCUI_ASSERT(ofs < parser->image_num);
             res->img_ofs[idx] = ofs;
        }
        res->img_w = scui_image_w(parser->image_hit[res->img_ofs[0]]);
        res->img_h = scui_image_h(parser->image_hit[res->img_ofs[0]]);
        
        
        
        // 为type_sub进行构建
        switch (parser->list_type_sub[idx]) {
        case scui_cwf_json_type_img_preview:
            /* skip... */
            break;
        case scui_cwf_json_type_img_simple: {
            SCUI_ASSERT(res->img_num == 1);
            scui_custom_maker_t custom_maker = {0};
            custom_maker.widget.type   = scui_widget_type_custom;
            custom_maker.widget.clip.x = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "x")) + 0.1;
            custom_maker.widget.clip.y = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "y")) + 0.1;
            custom_maker.widget.clip.w = res->img_w;
            custom_maker.widget.clip.h = res->img_h;
            custom_maker.widget.parent = parser->parent;
            
            custom_maker.widget.image  = parser->image_hit[res->img_ofs[0]];
            scui_custom_create(&custom_maker, &parser->list_child[idx], false);
            break;
        }
        case scui_cwf_json_type_img_watch: {
            SCUI_ASSERT(res->img_num == 3);
            scui_watch_maker_t watch_maker = {0};
            watch_maker.widget.type   = scui_widget_type_watch;
            watch_maker.widget.clip   = scui_widget_clip(parser->parent);
            watch_maker.widget.parent = parser->parent;
            watch_maker.image_h = parser->image_hit[res->img_ofs[0]];
            watch_maker.image_m = parser->image_hit[res->img_ofs[1]];
            watch_maker.image_s = parser->image_hit[res->img_ofs[2]];
            watch_maker.anchor_h.x = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "hx")) + 0.1;
            watch_maker.anchor_h.y = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "hy")) + 0.1;
            watch_maker.anchor_m.x = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "mx")) + 0.1;
            watch_maker.anchor_m.y = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "my")) + 0.1;
            watch_maker.anchor_s.x = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "sx")) + 0.1;
            watch_maker.anchor_s.y = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "sy")) + 0.1;
            watch_maker.center_h.y = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "h_cy")) + 0.1;
            watch_maker.center_m.y = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "m_cy")) + 0.1;
            watch_maker.center_s.y = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "s_cy")) + 0.1;
            watch_maker.center_h.x = scui_image_w(parser->image_hit[res->img_ofs[0]]) / 2;
            watch_maker.center_m.x = scui_image_w(parser->image_hit[res->img_ofs[1]]) / 2;
            watch_maker.center_s.x = scui_image_w(parser->image_hit[res->img_ofs[2]]) / 2;
            // watch_maker.tick_mode = true;
            
            watch_maker.widget.style.trans = true;
            scui_watch_create(&watch_maker, &parser->list_child[idx], false);
            break;
        }
        case scui_cwf_json_type_img_day:
        case scui_cwf_json_type_img_hour:
        case scui_cwf_json_type_img_minute:
        case scui_cwf_json_type_img_second:
        case scui_cwf_json_type_img_hr:
        case scui_cwf_json_type_img_kcal:
        case scui_cwf_json_type_img_step:
        case scui_cwf_json_type_img_batt:
        case scui_cwf_json_type_img_dist:
        case scui_cwf_json_type_img_temp: {
            // 数字类固定都是10个
            SCUI_ASSERT(res->img_num == 10);
            scui_custom_maker_t custom_maker = {0};
            custom_maker.widget.type   = scui_widget_type_custom;
            custom_maker.widget.clip.x = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "x")) + 0.1;
            custom_maker.widget.clip.y = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "y")) + 0.1;
            custom_maker.widget.clip.w = res->img_w;
            custom_maker.widget.clip.h = res->img_h;
            custom_maker.widget.parent = parser->parent;
            
            custom_maker.widget.style.trans = true;
            custom_maker.widget.event_cb = scui_cwf_json_custom_event_cb;
            scui_custom_create(&custom_maker, &parser->list_child[idx], false);
            break;
        }
        case scui_cwf_json_type_img_month:
        case scui_cwf_json_type_img_week:
        case scui_cwf_json_type_img_ampm:
        case scui_cwf_json_type_img_temp_unit:
        case scui_cwf_json_type_img_hr_prog :
        case scui_cwf_json_type_img_kcal_prog :
        case scui_cwf_json_type_img_step_prog :
        case scui_cwf_json_type_img_batt_prog :
        case scui_cwf_json_type_img_dist_prog :
        case scui_cwf_json_type_img_anim: {
            if (parser->list_type_sub[idx] == scui_cwf_json_type_img_month)
                SCUI_ASSERT(res->img_num == 12);
            if (parser->list_type_sub[idx] == scui_cwf_json_type_img_week)
                SCUI_ASSERT(res->img_num == 7);
            if (parser->list_type_sub[idx] == scui_cwf_json_type_img_ampm)
                SCUI_ASSERT(res->img_num == 2);
            
            scui_custom_maker_t custom_maker = {0};
            custom_maker.widget.type   = scui_widget_type_custom;
            custom_maker.widget.clip.x = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "x")) + 0.1;
            custom_maker.widget.clip.y = cJSON_GetNumberValue(cJSON_GetObjectItem(dict, "y")) + 0.1;
            custom_maker.widget.clip.w = scui_image_w(parser->image_hit[res->img_ofs[0]]);
            custom_maker.widget.clip.h = scui_image_h(parser->image_hit[res->img_ofs[0]]);
            custom_maker.widget.parent = parser->parent;
            
            custom_maker.widget.image  = parser->image_hit[res->img_ofs[0]];
            scui_custom_create(&custom_maker, &parser->list_child[idx], false);
            break;
        }
        default:
            SCUI_LOG_ERROR("unknown type_sub:%d", parser->list_type_sub[idx]);
            break;
        }
        
        
        
        break;
    }
    case scui_cwf_json_type_txt: {
        
        
        
        // 为type_sub进行构建
        switch (parser->list_type_sub[idx]) {
        
        default:
            SCUI_LOG_ERROR("unknown type_sub:%d", parser->list_type_sub[idx]);
            break;
        }
        
        
        
        break;
    }
    default:
        SCUI_LOG_ERROR("unknown type:%d", parser->list_type[idx]);
        break;
    }
    
    // 关联逆向索引, 资源绑定到目标
    if (parser->list_child[idx] != SCUI_HANDLE_INVALID) {
        scui_widget_user_data_set(parser->list_child[idx], res);
        res->parser = parser;
        res->list_idx = idx;
    }
}
