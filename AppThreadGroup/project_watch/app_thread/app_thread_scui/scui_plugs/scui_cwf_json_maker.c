/*实现目标:
 *    cwf 构造流程
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

// JSON解析库
#include "cJSON.h"

/*@brief 解析器指定项更新
 *@param parser 解析器
 *@param idx    指定项
 */
void scui_cwf_json_anim_item(scui_cwf_json_parser_t *parser, uint32_t idx)
{
    scui_csf_json_item__res_t *res = parser->list_src[idx];
    
    // 目前还未为type开发太多效果
    switch (parser->list_type[idx]) {
    case scui_cwf_json_type_img: {
        
        
        
        // 为type_sub进行构建
        switch (parser->list_type_sub[idx]) {
        case scui_cwf_json_type_img_preview:
            break;
        case scui_cwf_json_type_img_bg: {
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
        
        
        
        // 为type_sub进行构建
        switch (parser->list_type_sub[idx]) {
        case scui_cwf_json_type_img_preview: {
            break;
        }
        case scui_cwf_json_type_img_bg: {
            SCUI_ASSERT(res->img_num == 1);
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
        case scui_cwf_json_type_img_watch: {
            SCUI_ASSERT(res->img_num == 3);
            scui_watch_maker_t watch_maker = {0};
            watch_maker.widget.type        = scui_widget_type_watch;
            watch_maker.widget.style.trans = true;
            watch_maker.widget.clip        = scui_widget_clip(parser->parent);
            watch_maker.widget.parent      = parser->parent;
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
            scui_watch_create(&watch_maker, &parser->list_child[idx], false);
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
