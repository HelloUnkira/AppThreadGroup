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
             res->img_ofs[idx] = ofs;
        }
        
        
        
        // 为type_sub进行构建
        switch (parser->list_type_sub[idx]) {
        case scui_cwf_json_type_img_preview: {
            break;
        }
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
