/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 字符串控件创建
 *@param maker  字符串控件创建参数
 *@param handle 字符串控件句柄
 *@param layout 通过布局创建
 */
void scui_string_make(scui_string_maker_t *maker, scui_handle_t *handle, bool layout)
{
    /* 创建字符串控件实例 */
    scui_string_t *string = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_string_t));
    memset(string, 0, sizeof(scui_string_t));
    
    /* 创建基础控件实例 */
    scui_widget_maker_t widget_maker = maker->widget;
    
    /* 必须标记anima事件 */
    widget_maker.style.sched_anima = true;
    
    /* 创建基础控件实例 */
    scui_widget_make(&string->widget, &widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_string));
    SCUI_ASSERT(widget_maker.parent != SCUI_HANDLE_INVALID);
    
    string->font_idx    = maker->font_idx;
    string->lang_type   = maker->lang_type;
    string->args        = maker->args;
    string->unit_ms     = maker->unit_ms != 0 ? maker->unit_ms : SCUI_WIDGET_STRING_UNIT_MS;
    string->unit_dx     = maker->unit_dx != 0 ? maker->unit_dx : SCUI_WIDGET_STRING_UNIT_DX;
    string->unit_s      = maker->unit_s;
    string->draw_cache  = maker->draw_cache;
    string->unit_anima  = true;
    string->unit_abort  = false;
    string->unit_over   = false;
    string->unit_way    = 1;
    
    if (string->args.gap_line == 0)
        string->args.gap_line  = SCUI_WIDGET_STRING_GAP_LINE;
    if (string->args.gap_item == 0)
        string->args.gap_item  = SCUI_WIDGET_STRING_GAP_ITEM;
    if (string->args.gap_none == 0)
        string->args.gap_none  = SCUI_WIDGET_STRING_GAP_NONE;
    
    string->args.lang = string->lang_type;
    if (string->args.lang == SCUI_HANDLE_INVALID)
        scui_multi_lang_get(&string->args.lang);
    
    string->args.size = scui_font_size_match(string->font_idx, string->args.size);
    string->args.name = scui_font_name_match(string->font_idx, string->args.lang);
    
    /* 尝试初始更新字符串文本信息 */
    scui_string_update_text(*handle, maker->text);
    
    /* 更新一次字符串绘制参数 */
    string->args.update = true;
    string->args.utf8   = string->str_utf8;
    string->args.clip   = string->widget.clip;
    scui_string_args_process(&string->args);
}

/*@brief 字符串控件销毁
 *@param handle 字符串控件句柄
 *@param parent_way 来自父控件的销毁
 */
void scui_string_burn(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    string->args.update = true;
    string->args.name   = SCUI_HANDLE_INVALID;
    string->args.utf8   = NULL;
    scui_string_args_process(&string->args);
    scui_string_update_str(handle, NULL);
    
    /* 回收渐变序列 */
    if (string->args.grad_s != NULL) {
        SCUI_MEM_FREE(string->args.grad_s);
        string->args.grad_s  = NULL;
    }
    
    /* 回收旧颜色值表 */
    if (string->args.colors != NULL) {
        SCUI_MEM_FREE(string->args.colors->index_ls);
        SCUI_MEM_FREE(string->args.colors->index_le);
        SCUI_MEM_FREE(string->args.colors->color_ll);
        SCUI_MEM_FREE(string->args.colors);
        string->args.colors  = NULL;
    }
    
    /* 回收绘制缓存块 */
    if (string->draw_surface != NULL) {
        SCUI_MEM_FREE(string->draw_surface->pixel);
        SCUI_MEM_FREE(string->draw_surface);
        string->draw_surface = NULL;
    }
    
    /* 销毁基础控件实例 */
    SCUI_ASSERT(widget->type == scui_widget_type_string);
    scui_widget_burn(&string->widget);
    
    /* 销毁字符串控件实例 */
    SCUI_MEM_FREE(string);
}

/*@brief 字符串控件文本
 *@param handle 字符串控件句柄
 *@param text   字符串句柄
 */
void scui_string_text(scui_handle_t handle, scui_handle_t *text)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    *text = string->text;
}

/*@brief 字符串控件字符串
 *@param handle   字符串控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_string_str_utf8(scui_handle_t handle, uint8_t **str_utf8)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    *str_utf8 = string->str_utf8;
}

/*@brief 字符串控件更新文本
 *@param handle 字符串控件句柄
 *@param text   字符串句柄
 */
void scui_string_update_text(scui_handle_t handle, scui_handle_t text)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    /* 回收旧颜色值表 */
    if (string->args.colors != NULL) {
        SCUI_MEM_FREE(string->args.colors->index_ls);
        SCUI_MEM_FREE(string->args.colors->index_le);
        SCUI_MEM_FREE(string->args.colors->color_ll);
        SCUI_MEM_FREE(string->args.colors);
        string->args.colors  = NULL;
    }
    
    if (string->text == text)
        return;
    string->text = text;
    
    if (string->str_utf8 != NULL) {
        SCUI_MEM_FREE(string->str_utf8);
        string->str_utf8 = NULL;
    }
    
    if (string->text != SCUI_HANDLE_INVALID) {
        uint8_t *str_utf8  = scui_multi_lang_str(string->text, string->args.lang);
        uint32_t str_bytes = scui_utf8_str_bytes(str_utf8);
        string->str_utf8 = SCUI_MEM_ALLOC(scui_mem_type_mix, str_bytes + 7);
        memcpy(string->str_utf8, str_utf8, str_bytes);
        string->str_utf8[str_bytes] = '\0';
        
        #if 1 // 这个要不要???
        /* 从字库中提取一些信息 */
        scui_font_unit_t font_unit = {0};
        font_unit.name = string->args.name;
        font_unit.size = string->args.size;
        scui_font_cache_load(&font_unit);
        scui_font_cache_unload(&font_unit);
        scui_coord_t base_line   = scui_font_base_line(font_unit.font);
        scui_coord_t line_height = scui_font_line_height(font_unit.font);
        
        if (line_height > widget->clip.h)
            scui_widget_adjust_size(handle, widget->clip.w, line_height);
        #endif
    }
    
    string->args.update = true;
    scui_widget_draw(handle, NULL, false);
}

/*@brief 字符串控件更新字符串
 *@param handle   字符串控件句柄
 *@param str_utf8 字符串(utf8)
 */
void scui_string_update_str(scui_handle_t handle, uint8_t *str_utf8)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    // 重复的字符串, 跳过绘制
    if (string->str_utf8 != NULL && str_utf8 != NULL &&
        strcmp(string->str_utf8, str_utf8) == 0)
        return;
    
    /* 回收旧颜色值表 */
    if (!string->args.recolor)
    if (string->args.colors != NULL) {
        SCUI_MEM_FREE(string->args.colors->index_ls);
        SCUI_MEM_FREE(string->args.colors->index_le);
        SCUI_MEM_FREE(string->args.colors->color_ll);
        SCUI_MEM_FREE(string->args.colors);
        string->args.colors  = NULL;
    }
    
    string->text = SCUI_HANDLE_INVALID;
    
    if (string->str_utf8 != NULL) {
        SCUI_MEM_FREE(string->str_utf8);
        string->str_utf8 = NULL;
    }
    
    if (str_utf8 != NULL) {
        uint32_t str_bytes = scui_utf8_str_bytes(str_utf8);
        string->str_utf8 = SCUI_MEM_ALLOC(scui_mem_type_mix, str_bytes + 7);
        memcpy(string->str_utf8, str_utf8, str_bytes);
        string->str_utf8[str_bytes] = '\0';
    }
    
    string->args.update = true;
    scui_widget_draw(handle, NULL, false);
}

/*@brief 字符串控件更新字符串(重上色)
 *@param 使用#- -#包裹的内容为重上色区域
 *@param handle    字符串控件句柄
 *@param str_utf8  字符串(utf8)
 *@param color_num 重上色颜色数量
 *@param color_ll  重上色颜色列表
 */
void scui_string_update_str_rec(scui_handle_t handle, uint8_t *str_utf8, uint32_t color_num, scui_color_t *color_ll)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    SCUI_ASSERT(string->args.recolor);
    /* 回收旧颜色值表 */
    if (string->args.colors != NULL) {
        SCUI_MEM_FREE(string->args.colors->index_ls);
        SCUI_MEM_FREE(string->args.colors->index_le);
        SCUI_MEM_FREE(string->args.colors->color_ll);
        SCUI_MEM_FREE(string->args.colors);
        string->args.colors  = NULL;
    }
    
    /* 新建颜色值表 */
    string->args.colors = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_string_rec_t));
    string->args.colors->color_num = color_num;
    string->args.colors->index_ls  = SCUI_MEM_ALLOC(scui_mem_type_mix, color_num * sizeof(scui_coord_t));
    string->args.colors->index_le  = SCUI_MEM_ALLOC(scui_mem_type_mix, color_num * sizeof(scui_coord_t));
    string->args.colors->color_ll  = SCUI_MEM_ALLOC(scui_mem_type_mix, color_num * sizeof(scui_color_t));
    for (uint32_t idx = 0; idx < color_num; idx++) {
        string->args.colors->index_ls[idx] = -1;
        string->args.colors->index_le[idx] = -1;
        string->args.colors->color_ll[idx] = color_ll[idx];
    }
    
    /* 进行utf-8字符串转为unicode编码, 以此确认unicode编码下的index */
    uint32_t  num_unicode = scui_utf8_str_num(str_utf8);
    uint32_t *str_unicode = SCUI_MEM_ALLOC(scui_mem_type_font, 4 * (num_unicode + 1));
    memset(str_unicode, 0, 4 * (num_unicode + 1));
    scui_utf8_str_to_unicode(str_utf8, num_unicode, str_unicode);
    
    uint32_t idx_colors = 0, key_chars = 0;
    /* 匹配颜色值的起始点和结束点, 记录范围索引值, 同时去除目标点 */
    for (uint32_t idx = 0; idx + 1 < num_unicode; idx++) {
        /* 匹配到颜色起始点:#- */
        /* 匹配到颜色结束点:-# */
        if (str_unicode[idx] == '#' && str_unicode[idx + 1] == '-') {
            string->args.colors->index_ls[idx_colors] = idx - key_chars;
            key_chars += 2;
            idx++;
            continue;
        }
        if (str_unicode[idx] == '-' && str_unicode[idx + 1] == '#') {
            string->args.colors->index_le[idx_colors] = idx - key_chars - 1;
            key_chars += 2;
            idx++;
            
            idx_colors += 1;
            SCUI_ASSERT(idx_colors <= color_num);
            continue;
        }
    }
    SCUI_MEM_FREE(str_unicode);
    /* 规则匹配不上, 断言中止 */
    SCUI_ASSERT(idx_colors == color_num);
    
    uint32_t idx_utf8_raw = 0;
    uint32_t num_utf8_raw = strlen(str_utf8);
    uint8_t *str_utf8_raw = SCUI_MEM_ALLOC(scui_mem_type_font, num_utf8_raw + 1);
    memset(str_utf8_raw, 0, num_utf8_raw + 1);
    for (uint32_t idx = 0; idx + 1 < num_utf8_raw; idx++) {
        /* 匹配到颜色起始点:#- */
        /* 匹配到颜色结束点:-# */
        if ((str_utf8[idx] == '#' && str_utf8[idx + 1] == '-') ||
            (str_utf8[idx] == '-' && str_utf8[idx + 1] == '#')) {
             idx++;
             continue;
        }
        /* 拷贝其他字符 */
        str_utf8_raw[idx_utf8_raw] = str_utf8[idx];
        idx_utf8_raw++;
    }
    
    scui_string_update_str(handle, str_utf8_raw);
    SCUI_MEM_FREE(str_utf8_raw);
}

/*@brief 字符串控件渐变序列更新
 *@param handle 字符串控件句柄
 *@param grad_s 渐变序列
 *@param grad_n 渐变序列数量
 */
void scui_string_upgrade_grads(scui_handle_t handle, scui_color_t *grad_s, uint32_t grad_n)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    /* 必须要配置绘制缓存块 */
    SCUI_ASSERT(string->args.regrad);
    SCUI_ASSERT(string->draw_cache);
    SCUI_ASSERT(grad_n >= 2);
    
    /* 回收渐变序列 */
    if (string->args.grad_s != NULL) {
        SCUI_MEM_FREE(string->args.grad_s);
        string->args.grad_s  = NULL;
    }
    
    string->args.grad_n = grad_n;
    string->args.grad_s = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_color_t) * grad_n);
    for (uint32_t idx = 0; idx < grad_n; idx++)
        string->args.grad_s[idx] = grad_s[idx];
    
    string->args.update = true;
    scui_widget_draw(handle, NULL, false);
}

/*@brief 字符串控件滚动中止
 *@param handle 字符串控件句柄
 *@param abort  中止标记
 */
void scui_string_scroll_abort(scui_handle_t handle, bool abort)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    string->unit_abort = abort;
}

/*@brief 字符串控件滚动结束(单次滚动)
 *@param handle 字符串控件句柄
 *@retval 是否结束
 */
bool scui_string_scroll_over(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    return string->unit_over;
}

/*@brief 字符串控件修改文字尺寸(仅矢量字库有效)
 *@param handle 字符串控件句柄
 *@param size   新尺寸
 */
void scui_string_adjust_size(scui_handle_t handle, uint16_t size)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    if (string->args.size == 0)
        return;
    if (string->args.size == size)
        return;
    
    string->args.size = scui_font_size_match(string->font_idx, size);
    
    // 这里是需要重刷新
    string->args.update = true;
    scui_widget_draw(widget->myself, NULL, false);
}

/*@brief 字符串控件事件处理回调
 *@param event 事件
 */
void scui_string_event(scui_event_t *event)
{
    SCUI_LOG_INFO("event %u widget %u", event->type, event->object);
    scui_widget_t *widget = scui_handle_source_check(event->object);
    scui_string_t *string = (void *)widget;
    
    switch (event->type) {
    case scui_event_anima_elapse: {
        
        if (string->unit_abort)
            break;
        if (string->unit_over)
            break;
        
        if (string->args.limit <= 0) {
            string->args.offset = 0;
            /* 单次滚动结束标记 */
            if (string->unit_s)
                string->unit_over = true;
            break;
        }
        
        string->rcd_ms += SCUI_ANIMA_TICK;
        if (string->rcd_ms < string->unit_ms)
            break;
        string->rcd_ms -= string->unit_ms;
        
        scui_string_args_process(&string->args);
        string->args.offset -= string->unit_dx * string->unit_way;
        
        if (string->args.mode_scroll == 0) {
            
            if (string->args.offset > 0 ||
                string->args.offset < -string->args.limit) {
                
                if (string->args.offset > 0)
                    string->args.offset = 0;
                else
                    string->args.offset = -string->args.limit;
                
                string->unit_way = -string->unit_way;
                /* 单次滚动结束标记 */
                if (string->unit_s)
                    string->unit_over = true;
            }
        }
        if (string->args.mode_scroll == 1) {
            
            scui_coord_t limit_all = string->args.line_multi ? widget->clip.h : widget->clip.w;
            if (string->args.offset < -(string->args.limit + limit_all)) {
                
                if (string->args.line_multi)
                    string->args.offset = SCUI_WIDGET_STRING_SCROLL_LINE;
                else
                    string->args.offset = SCUI_WIDGET_STRING_SCROLL_ITEM;
                
                string->unit_way = scui_abs(string->unit_way);
                /* 单次滚动结束标记 */
                if (string->unit_s)
                    string->unit_over = true;
            }
        }
        
        string->unit_anima = true;
        scui_widget_draw(widget->myself, NULL, false);
        break;
    }
    case scui_event_draw: {
        if (!scui_event_check_execute(event))
             break;
        
        // 无绘制目标
        if (string->str_utf8 == NULL)
            break;
        
        if (string->args.update)
        if (string->draw_cache) {
            /* 回收绘制缓存块 */
            if (string->draw_surface != NULL) {
                SCUI_MEM_FREE(string->draw_surface->pixel);
                SCUI_MEM_FREE(string->draw_surface);
                string->draw_surface = NULL;
            }
        }
        
        string->args.utf8 = string->str_utf8;
        string->args.clip = string->widget.clip;
        scui_string_args_process(&string->args);
        
        if (string->draw_cache) {
            /* 绘制缓存块,在第一次全局重绘时进行 */
            if (string->draw_surface == NULL) {
                string->draw_surface  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_surface_t));
                string->draw_surface->format = SCUI_PIXEL_CF_DEF_A;
                string->draw_surface->alpha  = scui_alpha_cover;
                
                scui_coord_t hor_res = string->widget.clip.w;
                scui_coord_t ver_res = string->widget.clip.h;
                hor_res = string->args.line_multi ? hor_res : scui_max(hor_res, string->args.width);
                ver_res = string->args.line_multi ? scui_max(ver_res, string->args.height) : ver_res;
                
                scui_coord_t surface_byte     = scui_pixel_bits(string->draw_surface->format) / 8;
                scui_coord_t surface_remain   = sizeof(scui_color_wt_t) - surface_byte;
                scui_multi_t surface_size     = hor_res * ver_res * surface_byte + surface_remain;
                string->draw_surface->pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, surface_size);
                string->draw_surface->hor_res = hor_res;
                string->draw_surface->ver_res = ver_res;
                
                /* 先填充透明背景 */
                scui_area_t draw_clip = scui_surface_area(string->draw_surface);
                scui_draw_area_fill(string->draw_surface, &draw_clip, scui_alpha_cover, SCUI_COLOR_ZEROED);
                /* 如果全局渐变 */
                if (string->args.regrad) {
                    /* 回收旧颜色值表 */
                    if (string->args.colors != NULL) {
                        SCUI_MEM_FREE(string->args.colors->index_ls);
                        SCUI_MEM_FREE(string->args.colors->index_le);
                        SCUI_MEM_FREE(string->args.colors->color_ll);
                        SCUI_MEM_FREE(string->args.colors);
                        string->args.colors  = NULL;
                    }
                    /* 所有颜色统一绘制成白色 */
                    string->args.color = (scui_color_t){0};
                    string->args.color.color_s.full = 0xFFFFFFFF;
                    string->args.color.color_e.full = 0xFFFFFFFF;
                    string->args.color.filter = true;
                }
                /* 后将文字绘制到这个画布中 */
                string->args.offset = 0;
                string->args.clip = draw_clip;
                scui_area_t string_clip = draw_clip;
                
                scui_draw_string(string->draw_surface, &draw_clip,
                    &string->args, &string_clip, scui_alpha_cover);
                
                if (string->args.regrad) {
                    /* 如果需要全局渐变,对绘制画布进行渐变 */
                    scui_draw_area_fill_grads(string->draw_surface, &draw_clip,
                        string->args.grad_s, string->args.grad_n, string->args.grad_w,
                        SCUI_COLOR_FILTER_TRANS, scui_alpha_cover);
                }
            }
            
            scui_image_t image_inst = {
                .type           = scui_image_type_mem,
                .format         = string->draw_surface->format,
                .pixel.width    = string->draw_surface->hor_res,
                .pixel.height   = string->draw_surface->ver_res,
                .pixel.data_bin = string->draw_surface->pixel,
            };
            scui_handle_t image = scui_handle_find();
            scui_handle_linker(image, &image_inst);
            
            if (string->args.limit <= 0)
                scui_widget_draw_image(widget->myself, NULL, image, NULL, SCUI_COLOR_UNUSED);
            else {
                if (string->args.line_multi)
                    SCUI_LOG_DEBUG("offset y:%d", string->args.offset);
                else
                    SCUI_LOG_DEBUG("offset x:%d", string->args.offset);
                
                if (string->args.mode_scroll == 0) {
                    scui_area_t image_clip = scui_surface_area(string->draw_surface);
                    image_clip.x = string->args.line_multi ? 0 : -string->args.offset;
                    image_clip.y = string->args.line_multi ? -string->args.offset : 0;
                    scui_widget_draw_image(widget->myself, NULL, image, &image_clip, SCUI_COLOR_UNUSED);
                }
                
                if (string->args.mode_scroll == 1) {
                    scui_area_t draw_clip  = widget->clip;
                    scui_area_t image_clip = scui_surface_area(string->draw_surface);
                    if (string->args.offset < 0) {
                        image_clip.x = string->args.line_multi ? 0 : -string->args.offset;
                        image_clip.y = string->args.line_multi ? -string->args.offset : 0;
                    } else {
                        draw_clip.x += string->args.line_multi ? 0 : string->args.offset;
                        draw_clip.y += string->args.line_multi ? string->args.offset : 0;
                    }
                    scui_widget_draw_image(widget->myself, &draw_clip, image, &image_clip, SCUI_COLOR_UNUSED);
                }
                if (string->args.mode_scroll == 1 && string->args.limit > 0) {
                    scui_coord_t offset    = string->args.offset;
                    scui_coord_t limit_all = string->args.line_multi ? widget->clip.h : widget->clip.w;
                    offset += string->args.limit + limit_all;
                    offset += string->args.line_multi ? SCUI_WIDGET_STRING_SCROLL_LINE : 0;
                    offset += string->args.line_multi ? 0 : SCUI_WIDGET_STRING_SCROLL_ITEM;
                    
                    scui_area_t draw_clip = widget->clip;
                    draw_clip.x += string->args.line_multi ? 0 : offset;
                    draw_clip.y += string->args.line_multi ? offset : 0;
                    scui_widget_draw_image(widget->myself, &draw_clip, image, NULL, SCUI_COLOR_UNUSED);
                }
            }
            
            scui_handle_clear(image);
            break;
        }
        
        // 无缓存块的绘制下
        scui_widget_draw_string(widget->myself, NULL, &string->args);
        //
        if (string->args.mode_scroll == 1 && string->args.limit > 0) {
            scui_coord_t offset = string->args.offset;
            scui_coord_t limit_all = string->args.line_multi ? widget->clip.h : widget->clip.w;
            string->args.offset += string->args.limit + limit_all;
            string->args.offset += string->args.line_multi ? SCUI_WIDGET_STRING_SCROLL_LINE : 0;
            string->args.offset += string->args.line_multi ? 0 : SCUI_WIDGET_STRING_SCROLL_ITEM;
            
            scui_widget_draw_string(widget->myself, NULL, &string->args);
            string->args.offset = offset;
        }
        break;
    }
    case scui_event_adjust_size:{
        string->args.update = true;
        scui_widget_draw(widget->myself, NULL, false);
        break;
    }
    case scui_event_change_lang: {
        
        if (string->text != SCUI_HANDLE_INVALID) {
            string->unit_over = false;
            string->unit_way  = 1;
            scui_handle_t text = string->text;
            scui_string_update_text(widget->myself, SCUI_HANDLE_INVALID);
            scui_multi_lang_get(&string->args.lang);
            string->args.size = scui_font_size_match(string->font_idx, string->args.size);
            string->args.name = scui_font_name_match(string->font_idx, string->args.lang);
            scui_string_update_text(widget->myself, text);
            scui_widget_draw(widget->myself, NULL, true);
        }
        break;
    }
    default:
        break;
    }
}
