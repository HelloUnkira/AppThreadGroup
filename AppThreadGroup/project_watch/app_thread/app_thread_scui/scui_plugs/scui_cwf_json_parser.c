/*实现目标:
 *    cwf 解析流程
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

// JSON解析库
#include "cJSON.h"

#define SCUI_CWF_JSON_PARSER_HOOK   1
#if     SCUI_CWF_JSON_PARSER_HOOK
void * scui_cwf_json_parser_alloc(size_t size)
{
    return SCUI_MEM_ALLOC(scui_mem_type_user, size);
}
void scui_cwf_json_parser_free(void *ptr)
{
    SCUI_MEM_FREE(ptr);
}
#endif

/*@brief 解析器指定项更新
 *@param parser 解析器
 *@param idx    指定项
 */
void scui_cwf_json_anim_item(scui_cwf_json_parser_t *parser, uint32_t idx);

/*@brief 解析器指定项销毁
 *@param parser 解析器
 *@param idx    指定项
 */
void scui_cwf_json_burn_item(scui_cwf_json_parser_t *parser, uint32_t idx);

/*@brief 解析器指定项构造
 *@param parser 解析器
 *@param idx    指定项
 *@param dict   构造参数
 */
void scui_cwf_json_make_item(scui_cwf_json_parser_t *parser, uint32_t idx, cJSON *dict);

/*@brief 生成u32字段
 *@param byte4 小端序4字节
 *@retval u32字段
 */
static uint32_t scui_cwf_json_u32(uint8_t byte4[4])
{
    uint32_t data32 = 0;
    for (uint32_t idx = 0; idx < 4; idx++)
        data32 += (uint32_t)byte4[idx] << (idx * 8);
    
    return data32;
}

/*@brief 更新cwf
 *@param inst 实例
 */
void scui_cwf_json_anim(void **inst)
{
    SCUI_ASSERT(inst != NULL);
    scui_cwf_json_parser_t *parser = *inst;
    
    /* 做个缓速 */
    static uint32_t span_fps = 7;
    static uint32_t tick_cnt = 0;
    tick_cnt += SCUI_ANIMA_TICK;
    if (tick_cnt >= 1000 / span_fps)
        tick_cnt -= 1000 / span_fps;
    else
        return;
    
    for (uint32_t idx = 0; idx < parser->list_num; idx++)
        scui_cwf_json_anim_item(parser, idx);
    
    // 控件自行更新, 不做全局刷新
    // scui_widget_draw(parser->parent, NULL, false);
}

/*@brief 销毁cwf
 *@param inst 实例地址
 */
void scui_cwf_json_burn(void **inst)
{
    SCUI_ASSERT(inst != NULL);
    scui_cwf_json_parser_t *parser = *inst;
    *inst = NULL;
    
    if (parser == NULL)
        return;
    
    // 批量销毁所有条目资源
    for (uint32_t idx = 0; idx < parser->list_num; idx++)
        scui_cwf_json_burn_item(parser, idx);
    // 销毁整个子控件树
    scui_widget_destroy(parser->parent);
    // 回收内部维护资源
    SCUI_MEM_FREE(parser->list_child);
    SCUI_MEM_FREE(parser->list_type);
    SCUI_MEM_FREE(parser->list_src);
    // 图片资源缓存无效化
    for (uint32_t idx = 0; idx < parser->image_num; idx++) {
        scui_image_unit_t image_unit = {0};
        image_unit.image = &parser->image_src[idx];
        scui_image_cache_invalidate(&image_unit);
    }
    // 批量回收所有图片句柄
    for (uint32_t idx = 0; idx < parser->image_num; idx++)
        if (parser->image_hit[idx] != SCUI_HANDLE_INVALID)
            scui_handle_clear(parser->image_hit[idx]);
    // 回收内部维护资源
    SCUI_MEM_FREE(parser->image_hit);
    SCUI_MEM_FREE(parser->image_src);
    // 回收名字和对应句柄
    const char *name = scui_handle_source(parser->name);
    scui_handle_clear(parser->name);
    // 回收内部维护资源
    SCUI_MEM_FREE(name);
    
    // 销毁解析器
    SCUI_MEM_FREE(parser);
}

/*@brief 构建cwf
 *@param inst   实例地址
 *@param file   cwf名字
 *@param parent 父控件
 */
void scui_cwf_json_make(void **inst, const char *file, scui_handle_t parent)
{
    SCUI_ASSERT(inst != NULL);
    // 在构建时尝试销毁上一次cwf
    scui_cwf_json_burn(inst);
    
    uint8_t head[28] = {0};
    scui_image_bin_read(file, 0, sizeof(head), head);
    // 先提取头部信息
    uint8_t  fw_ver[4]   = SCUI_CWF_JSON_VER;
    uint8_t  cwf_ver[4]  = {head[0],head[1],head[2],head[3],};
    uint32_t json_ofs    = scui_cwf_json_u32(&head[4 * 1]);
    uint32_t json_size   = scui_cwf_json_u32(&head[4 * 2]);
    uint32_t image_iofs  = scui_cwf_json_u32(&head[4 * 3]);
    uint32_t image_isize = scui_cwf_json_u32(&head[4 * 4]);
    uint32_t image_dofs  = scui_cwf_json_u32(&head[4 * 5]);
    uint32_t image_dsize = scui_cwf_json_u32(&head[4 * 6]);
    // 进行版本校验
    if (memcmp(fw_ver, cwf_ver, 4) != 0)
        SCUI_LOG_ERROR("unmatched cwf version");
    
    scui_cwf_json_parser_t *parser = NULL;
    parser = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_cwf_json_parser_t));
    memset(parser, 0, sizeof(scui_cwf_json_parser_t));
    *inst = parser;
    
    uint8_t *json_file = SCUI_MEM_ALLOC(scui_mem_type_graph, json_size + 1);
    scui_image_bin_read(file, json_ofs, json_size, json_file);
    json_file[json_size] = '\0';
    
    // 构建parser资源
    uint8_t *name = SCUI_MEM_ALLOC(scui_mem_type_user, strlen(file) + 1);
    strncpy(name, file, strlen(file));
    name[strlen(file)] = '\0';
    parser->name = scui_handle_find();
    scui_handle_linker(parser->name, name);
    
    SCUI_ASSERT(image_isize % 18 == 0);
    parser->image_num = image_isize / 18;
    parser->image_src = SCUI_MEM_ALLOC(scui_mem_type_user, parser->image_num * sizeof(scui_image_t));
    parser->image_hit = SCUI_MEM_ALLOC(scui_mem_type_user, parser->image_num * sizeof(scui_handle_t));
    
    uint8_t *image_info = SCUI_MEM_ALLOC(scui_mem_type_user, parser->image_num * 18);
    scui_image_bin_read(file, image_iofs, parser->image_num * 18, image_info);
    for (uint32_t idx = 0; idx < parser->image_num; idx++) {
        uint8_t  format = image_info[18 * idx + 0];
        uint8_t  type   = image_info[18 * idx + 1];
        uint32_t height = scui_cwf_json_u32(&image_info[18 * idx + 2 + 4 * 0]);
        uint32_t width  = scui_cwf_json_u32(&image_info[18 * idx + 2 + 4 * 1]);
        uint32_t size   = scui_cwf_json_u32(&image_info[18 * idx + 2 + 4 * 2]);
        uint32_t data   = scui_cwf_json_u32(&image_info[18 * idx + 2 + 4 * 3]);
        
        switch (format) {
        case scui_cwf_json_image_cf_alpha4: parser->image_src[idx].format = scui_pixel_cf_alpha4; break;
        case scui_cwf_json_image_cf_alpha8: parser->image_src[idx].format = scui_pixel_cf_alpha8; break;
        case scui_cwf_json_image_cf_bmp565:   parser->image_src[idx].format = scui_pixel_cf_bmp565;   break;
        case scui_cwf_json_image_cf_bmp888:   parser->image_src[idx].format = scui_pixel_cf_bmp888;   break;
        case scui_cwf_json_image_cf_bmp8565:  parser->image_src[idx].format = scui_pixel_cf_bmp8565;  break;
        case scui_cwf_json_image_cf_bmp8888:  parser->image_src[idx].format = scui_pixel_cf_bmp8888;  break;
        default: SCUI_LOG_ERROR("unknown cwf json image format:%d", format);
        }
        parser->image_src[idx].type = type;
        parser->image_src[idx].pixel.height   = (uintptr_t)height;
        parser->image_src[idx].pixel.width    = (uintptr_t)width;
        parser->image_src[idx].pixel.size_bin = (uintptr_t)size;
        parser->image_src[idx].pixel.data_bin = (uintptr_t)data;
        parser->image_src[idx].from = parser->name;
        
        // 资源关联绑定 handle <---> image
        parser->image_hit[idx] = scui_handle_find();
        scui_handle_linker(parser->image_hit[idx], &parser->image_src[idx]);
    }
    
    /* cJSON组件初始化(选择性使用) */
    #if SCUI_CWF_JSON_PARSER_HOOK
    cJSON_Hooks cjson_hooks = {
        .malloc_fn = scui_cwf_json_parser_alloc,
        .free_fn   = scui_cwf_json_parser_free,
    };
    cJSON_InitHooks(&cjson_hooks);
    #endif
    
    // 提取json文件,对顶层解析,继续构建资源
    cJSON *json_object = cJSON_Parse(json_file);
    cJSON *json_layout = cJSON_GetObjectItem(json_object, "layout");
    parser->list_num   = cJSON_GetArraySize(json_layout);
    parser->list_child = SCUI_MEM_ALLOC(scui_mem_type_user, parser->list_num * sizeof(scui_handle_t));
    parser->list_type  = SCUI_MEM_ALLOC(scui_mem_type_user, parser->list_num * sizeof(uint8_t));
    parser->list_src   = SCUI_MEM_ALLOC(scui_mem_type_user, parser->list_num * sizeof(void *));
    
    #if 0   /* 检查JSON */
    char *json_format = cJSON_Print(json_object);
    SCUI_LOG_INFO("\njson:%d", json_size);
    SCUI_LOG_INFO("\n%s\n", json_format);
    cJSON_free(json_format);
    #endif
    
    memset(parser->list_child, 0, parser->list_num * sizeof(scui_handle_t));
    memset(parser->list_type,  0, parser->list_num * sizeof(uint8_t));
    memset(parser->list_src,   0, parser->list_num * sizeof(void *));
    
    // 构建一个父容器,用于承载cwf
    scui_custom_maker_t custom_maker = {0};
    custom_maker.widget.type        = scui_widget_type_custom;
    custom_maker.widget.style.cover = true;
    custom_maker.widget.clip        = scui_widget_clip(parent);
    custom_maker.widget.parent      = parent;
    custom_maker.widget.child_num   = parser->list_num;
    scui_widget_create(&custom_maker, &parser->parent, false);
    
    // 按索引顺序一个个解析, 然后添加到parser中去
    for (uint32_t idx = 0; idx < parser->list_num; idx++) {
        cJSON *json_dict = cJSON_GetArrayItem(json_layout, idx);
        scui_cwf_json_make_item(parser, idx, json_dict);
    }
    
    SCUI_MEM_FREE(image_info);
    cJSON_Delete(json_object);
    SCUI_MEM_FREE(json_file);
    
    // 在结束的时候,进行一次anim更新
    for (uint32_t idx = 0; idx < parser->list_num; idx++)
        scui_cwf_json_anim_item(parser, idx);
}

/*@brief 销毁cwf preview
 *@param preview 预览图句柄地址
 */
void scui_cwf_json_burn_pv(scui_handle_t *preview)
{
    SCUI_ASSERT(preview != NULL);
    scui_handle_t image_hit = *preview;
    *preview = SCUI_HANDLE_INVALID;
    
    if (image_hit == SCUI_HANDLE_INVALID)
        return;
    
    scui_image_t *image_src = scui_handle_source_check(image_hit);
    
    // 记得要清理缓存
    scui_image_unit_t image_unit = {.image = image_src,};
    scui_image_cache_invalidate(&image_unit);
    
    char *name = scui_handle_source(image_src->from);
    scui_handle_clear(image_src->from);
    SCUI_MEM_FREE(name);
    
    scui_handle_clear(image_hit);
    SCUI_MEM_FREE(image_src);
}

/*@brief 构建cwf preview
 *@param inst 预览图实例地址
 *@param file cwf名字
 */
void scui_cwf_json_make_pv(scui_handle_t *preview, const char *file)
{
    SCUI_ASSERT(preview != NULL);
    // 在构建时尝试销毁上一次cwf
    scui_cwf_json_burn_pv(preview);
    
    uint8_t head[28] = {0};
    scui_image_bin_read(file, 0, sizeof(head), head);
    // 先提取头部信息
    uint8_t  fw_ver[4]   = SCUI_CWF_JSON_VER;
    uint8_t  cwf_ver[4]  = {head[0],head[1],head[2],head[3],};
    uint32_t json_ofs    = scui_cwf_json_u32(&head[4 * 1]);
    uint32_t json_size   = scui_cwf_json_u32(&head[4 * 2]);
    uint32_t image_iofs  = scui_cwf_json_u32(&head[4 * 3]);
    uint32_t image_isize = scui_cwf_json_u32(&head[4 * 4]);
    uint32_t image_dofs  = scui_cwf_json_u32(&head[4 * 5]);
    uint32_t image_dsize = scui_cwf_json_u32(&head[4 * 6]);
    // 进行版本校验
    if (memcmp(fw_ver, cwf_ver, 4) != 0)
        SCUI_LOG_ERROR("unmatched cwf version");
    
    uint8_t *json_file = SCUI_MEM_ALLOC(scui_mem_type_graph, json_size + 1);
    scui_image_bin_read(file, json_ofs, json_size, json_file);
    json_file[json_size] = '\0';
    
    // 提取json文件,对顶层解析,继续构建资源
    cJSON *json_object = cJSON_Parse(json_file);
    cJSON *json_layout = cJSON_GetObjectItem(json_object, "layout");
    
    for (uint32_t idx = 0; idx < cJSON_GetArraySize(json_layout); idx++) {
        cJSON *json_dict = cJSON_GetArrayItem(json_layout, idx);
        
        uint8_t type = cJSON_GetNumberValue(cJSON_GetObjectItem(json_dict, "type")) + 0.1;
        
        // 预览图被记录在:
        // scui_cwf_json_type_img_preview
        if (type == scui_cwf_json_type_img_preview) {
            cJSON *json_src = cJSON_GetObjectItem(json_dict, "image_src");
            cJSON *json_num = cJSON_GetObjectItem(json_dict, "image_num");
            uint32_t img_ofs = cJSON_GetNumberValue(cJSON_GetArrayItem(json_src, 0)) + 0.1;
            uint16_t img_num = cJSON_GetNumberValue(json_num) + 0.1;
            SCUI_ASSERT(img_num == 1);
            
            uint8_t image_info[18] = {0};
            scui_image_bin_read(file, image_iofs + img_ofs * 18, 18, image_info);
            scui_image_t *image_src = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_image_t));
            memset(image_src, 0, sizeof(scui_image_t));
            
            uint8_t  format = image_info[18 * 0 + 0];
            uint8_t  type   = image_info[18 * 0 + 1];
            uint32_t height = scui_cwf_json_u32(&image_info[18 * 0 + 2 + 4 * 0]);
            uint32_t width  = scui_cwf_json_u32(&image_info[18 * 0 + 2 + 4 * 1]);
            uint32_t size   = scui_cwf_json_u32(&image_info[18 * 0 + 2 + 4 * 2]);
            uint32_t data   = scui_cwf_json_u32(&image_info[18 * 0 + 2 + 4 * 3]);
            
            switch (format) {
            case scui_cwf_json_image_cf_alpha4: image_src->format = scui_pixel_cf_alpha4; break;
            case scui_cwf_json_image_cf_alpha8: image_src->format = scui_pixel_cf_alpha8; break;
            case scui_cwf_json_image_cf_bmp565:   image_src->format = scui_pixel_cf_bmp565;   break;
            case scui_cwf_json_image_cf_bmp888:   image_src->format = scui_pixel_cf_bmp888;   break;
            case scui_cwf_json_image_cf_bmp8565:  image_src->format = scui_pixel_cf_bmp8565;  break;
            case scui_cwf_json_image_cf_bmp8888:  image_src->format = scui_pixel_cf_bmp8888;  break;
            default: SCUI_LOG_ERROR("unknown cwf json image format:%d", format);
            }
            image_src->type = type;
            image_src->pixel.height   = (uintptr_t)height;
            image_src->pixel.width    = (uintptr_t)width;
            image_src->pixel.size_bin = (uintptr_t)size;
            image_src->pixel.data_bin = (uintptr_t)data;
            
            uint8_t *name = SCUI_MEM_ALLOC(scui_mem_type_user, strlen(file) + 1);
            strncpy(name, file, strlen(file) + 1);
            name[strlen(file)] = '\0';
            image_src->from = scui_handle_find();
            scui_handle_linker(image_src->from, name);
            
            // 资源关联绑定 handle <---> image
            scui_handle_t image_hit = scui_handle_find();
            scui_handle_linker(image_hit, image_src);
            *preview = image_hit;
            break;
        }
    }
    
    cJSON_Delete(json_object);
    SCUI_MEM_FREE(json_file);
}
