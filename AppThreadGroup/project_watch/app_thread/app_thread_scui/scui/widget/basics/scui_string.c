/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 */
void scui_string_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_string_t *string = widget;
    scui_string_maker_t *string_maker = widget_maker;
    
    /* 必须标记anima事件 */
    widget_maker->style.sched_anima = true;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_string));
    SCUI_ASSERT(widget_maker->parent != SCUI_HANDLE_INVALID);
    
    string->font_idx    = string_maker->font_idx;
    string->args        = string_maker->args;
    string->unit_ms     = string_maker->unit_ms;
    string->unit_dx     = string_maker->unit_dx;
    string->unit_s      = string_maker->unit_s;
    string->draw_cache  = string_maker->draw_cache;
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
    
    if (string->args.lang == SCUI_HANDLE_INVALID)
        scui_lang_get(&string->args.lang);
    
    if (string->unit_ms == 0) string->unit_ms = SCUI_WIDGET_STRING_UNIT_MS;
    if (string->unit_dx == 0) string->unit_dx = SCUI_WIDGET_STRING_UNIT_DX;
    if (string->args.name == SCUI_HANDLE_INVALID) {
        string->args.name  = scui_font_name_match(string->font_idx, string->args.lang);
        string->args.size  = scui_font_size_match(string->font_idx, string->args.size);
    }
    
    /* 滚动方向: RTL内容右移(offset正向), LTR内容左移(offset负向) */
    string->unit_way = scui_lang_RTL() && !string->args.line_multi ? -1 : 1;
    
    /* RTL语言: 左对齐自动调整为右对齐 */
    if (string->args.align_hor == 0 && scui_lang_RTL()) {
        string->args.align_hor  = 1;
        string->align_rtl       = true;
    }
    
    /* 尝试初始更新字符串文本信息 */
    scui_string_update_text(*handle, string_maker->text);
    
    #if SCUI_MEM_FEAT_MINI
    /* 禁用绘制缓存块 */
    /* 禁用级联色彩渐变 */
    string->draw_cache  = false;
    string->args.regrad = false;
    #endif
    
    /* 让内存画布绑定到句柄 */
    if (string->draw_cache) {
        string->draw_image = scui_handle_find();
        string->draw_image_src = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_image_t));
        scui_handle_linker(string->draw_image, string->draw_image_src);
    }
    
    /* 更新一次字符串绘制参数 */
    string->args.update = true;
    string->args.utf8   = string->str_utf8;
    string->args.clip   = scui_widget_area(widget->myself);
    scui_string_args_proc(&string->args);
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_string_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    string->args.update = true;
    string->args.name   = SCUI_HANDLE_INVALID;
    string->args.utf8   = NULL;
    scui_string_args_proc(&string->args);
    scui_string_update_str(handle, NULL);
    
    /* 回收旧颜色值表 */
    if (string->args.colors != NULL) {
        SCUI_MEM_FREE(string->args.colors->index_ls);
        SCUI_MEM_FREE(string->args.colors->index_le);
        SCUI_MEM_FREE(string->args.colors->color_ll);
        SCUI_MEM_FREE(string->args.colors);
        string->args.colors  = NULL;
    }
    
    /* 回收渐变序列表 */
    if (string->args.grads != NULL) {
        SCUI_MEM_FREE(string->args.grads->grad_s);
        SCUI_MEM_FREE(string->args.grads);
        string->args.grads  = NULL;
    }
    
    /* 回收绘制缓存块 */
    scui_image_burn(string->draw_image_src);
    
    /* 资源及句柄回收 */
    SCUI_MEM_FREE(string->draw_image_src);
    scui_handle_clear(string->draw_image);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
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
    
    if (string->text == text)
        return;
    
    string->text = text;
    if (string->str_utf8 != NULL) {
        SCUI_MEM_FREE(string->str_utf8);
        string->str_utf8 = NULL;
    }
    
    if (string->text != SCUI_HANDLE_INVALID) {
        if (string->args.lang == SCUI_HANDLE_INVALID)
            scui_lang_get(&string->args.lang);
        uint8_t *str_utf8 = scui_lang_str(string->text, string->args.lang);
        scui_coord_t str_bytes = scui_utf8_str_bytes(str_utf8);
        string->str_utf8 = SCUI_MEM_ALLOC(scui_mem_type_mix, str_bytes + 7);
        memcpy(string->str_utf8, str_utf8, str_bytes);
        string->str_utf8[str_bytes] = '\0';
    }
    
    string->args.utf8   = string->str_utf8;
    string->args.update = true;
    scui_widget_layout_refr(handle);
    scui_widget_draw(handle, NULL, false, 0);
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
    
    /* 重复的字符串, 跳过绘制 */
    if (string->str_utf8 != NULL && str_utf8 != NULL &&
        strcmp(string->str_utf8, str_utf8) == 0)
        return;
    
    string->text = SCUI_HANDLE_INVALID;
    if (string->str_utf8 != NULL) {
        SCUI_MEM_FREE(string->str_utf8);
        string->str_utf8 = NULL;
    }
    
    if (str_utf8 != NULL) {
        scui_coord_t str_bytes = scui_utf8_str_bytes(str_utf8);
        string->str_utf8 = SCUI_MEM_ALLOC(scui_mem_type_mix, str_bytes + 7);
        memcpy(string->str_utf8, str_utf8, str_bytes);
        string->str_utf8[str_bytes] = '\0';
    }
    
    string->args.utf8   = string->str_utf8;
    string->args.update = true;
    scui_widget_layout_refr(handle);
    scui_widget_draw(handle, NULL, false, 0);
}

/*@brief 字符串控件更新字符串(重上色)
 *@brief 使用#- -#包裹的内容为重上色区域
 *@brief 不可再使用其他接口更新字符串
 *@param handle    字符串控件句柄
 *@param str_utf8  字符串(utf8)
 *@param color_num 重上色颜色数量
 *@param color_ll  重上色颜色列表
 */
void scui_string_update_str_rec(scui_handle_t handle, uint8_t *str_utf8, scui_coord_t color_num, scui_color_t *color_ll)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    /* 重复的字符串, 跳过绘制 */
    if (string->str_utf8 != NULL && str_utf8 != NULL &&
        strcmp(string->str_utf8, str_utf8) == 0)
        return;
    
    /* 回收旧颜色值表 */
    SCUI_ASSERT(string->args.recolor);
    if (string->args.colors != NULL) {
        SCUI_MEM_FREE(string->args.colors->index_ls);
        SCUI_MEM_FREE(string->args.colors->index_le);
        SCUI_MEM_FREE(string->args.colors->color_ll);
        SCUI_MEM_FREE(string->args.colors);
        string->args.colors  = NULL;
    }
    
    /* 如果仅回收资源 */
    if (str_utf8 == NULL || color_num == 0 || color_ll == NULL)
        return;
    
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
    
    scui_string_update_str(handle, str_utf8);
}

/*@brief 字符串控件渐变序列更新
 *@param handle 字符串控件句柄
 *@param grad_s 渐变序列
 *@param grad_n 渐变序列数量
 *@param grad_w 渐变序列方向(0:hor;1:ver;)
 */
void scui_string_upgrade_grads(scui_handle_t handle, scui_color_t *grad_s, scui_coord_t grad_n, bool grad_w)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    #if SCUI_MEM_FEAT_MINI
    /* 禁用级联色彩渐变 */
    return;
    #endif
    
    /* 必须要配置绘制缓存块 */
    SCUI_ASSERT(string->args.regrad);
    SCUI_ASSERT(string->draw_cache);
    SCUI_ASSERT(grad_n >= 2);
    
    /* 回收渐变序列表 */
    if (string->args.grads != NULL) {
        SCUI_MEM_FREE(string->args.grads->grad_s);
        SCUI_MEM_FREE(string->args.grads);
        string->args.grads  = NULL;
    }
    
    /* 新建渐变序列表 */
    string->args.grads = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_string_grad_t));
    string->args.grads->grad_s = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_color_t) * grad_n);
    string->args.grads->grad_n = grad_n;
    string->args.grads->grad_w = grad_w;
    
    for (scui_coord_t idx = 0; idx < grad_n; idx++)
        string->args.grads->grad_s[idx] = grad_s[idx];
    
    string->args.update = true;
    scui_widget_draw(handle, NULL, false, 0);
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
void scui_string_adjust_size(scui_handle_t handle, scui_coord_t size)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_string));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_string_t *string = (void *)widget;
    
    if (string->args.size == 0) {
        SCUI_LOG_ERROR("only support vector font");
        return;
    }
    if (string->args.size == size)
        return;
    
    string->args.size = scui_font_size_match(string->font_idx, size);
    scui_widget_layout_refr(handle);
    
    /* 清扫一遍cache以让旧资源快速回收 */
    scui_cache_font_rectify();
    scui_cache_glyph_rectify();
    
    /* 这里是需要重刷新 */
    string->args.update = true;
    scui_widget_draw(widget->myself, NULL, false, 0);
}

/*@brief 字符串滚动映射(通用迭代+镜像)
 *@param offset 内容位移(有符号; LTR负向, RTL正向)
 *@param mirror RTL镜像(单行水平)
 *@param size_t 内容长(单行=文本宽/多行=文本高)
 *@param size_w 轨道宽(单行=控件宽/多行=控件高)
 *@param gap    循环间距(单行=字间距/多行=行间距; 0=不循环补片)
 *@param img_1  段1源裁剪(x为起点,w为宽)
 *@param dst_1  段1目标偏移(窗口内)
 *@param have_1 段1有效
 *@param img_2  段2源裁剪(轮转补片)
 *@param dst_2  段2目标偏移
 *@param have_2 段2有效
 */
static void scui_string_scroll_map(scui_coord_t offset, bool mirror,
    scui_coord_t size_t, scui_coord_t size_w, scui_coord_t gap,
    scui_area_t *img_1, scui_area_t *dst_1, bool *have_1,
    scui_area_t *img_2, scui_area_t *dst_2, bool *have_2)
{
    /* 窗口起始缘对应缓存源(段1起点) */
    scui_coord_t s1 = mirror ? (size_t - size_w - offset) : -offset;
    
    /* 段1: 窗口与内容区 [0, size_t] 交集 */
    scui_coord_t vis_s = s1 < 0 ? 0 : s1;
    scui_coord_t vis_e = s1 + size_w > size_t ? size_t : s1 + size_w;
    if (vis_e > vis_s) {
        *have_1 = true;
        img_1->x = vis_s;
        img_1->w = vis_e - vis_s;
        dst_1->x = vis_s - s1;
    } else {
        *have_1 = false;
    }
    
    /* 段2: 轮转补片(跨环后从进入侧补开头) */
    *have_2 = false;
    if (mirror) {
        /* RTL: 窗口起始缘跨间隙, 开头从窗口起始侧进入 */
        scui_coord_t w2 = -(s1 + gap);
        if (w2 > 0) {
            w2 = w2 > size_w ? size_w : w2;
            *have_2 = true;
            img_2->x = size_t - w2;
            img_2->w = w2;
            dst_2->x = 0;
        }
    } else {
        /* LTR: 窗口终止缘跨间隙, 开头从窗口终止侧进入 */
        scui_coord_t w2 = s1 + size_w - (size_t + gap);
        if (w2 > 0) {
            w2 = w2 > size_w ? size_w : w2;
            *have_2 = true;
            img_2->x = 0;
            img_2->w = w2;
            dst_2->x = size_w - w2;
        }
    }
}

/*@brief 事件处理回调
 *@param event 事件
 */
void scui_string_invoke(scui_event_t *event)
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
        
        string->rcd_ms += event->tick;
        if (string->rcd_ms < string->unit_ms) break;
        else string->rcd_ms -= string->unit_ms;
        
        scui_string_args_proc(&string->args);
        /* 滚动迭代: 内容位移双向推进(方向由unit_way决定) */
        /* LTR内容左移(offset负向), RTL内容右移(offset正向) */
        string->args.offset -= string->unit_dx * string->unit_way;
        
        if (string->args.mode_scroll == 0) {
            /* 左右来回: 端点反弹(双向迭代) */
            if (scui_abs(string->args.offset) > string->args.limit) {
                string->args.offset = string->args.offset > 0 ?
                    string->args.limit : -string->args.limit;
                string->unit_way = -string->unit_way;
                /* 单次滚动结束标记 */
                if (string->unit_s)
                    string->unit_over = true;
            }
        }
        if (string->args.mode_scroll == 1) {
            /* 轮转滚动: 周期循环(内容长+轨道宽+间距; RTL单行与LTR同周期) */
            scui_coord_t limit_all = string->args.line_multi ? widget->clip.h : widget->clip.w;
            scui_coord_t gap       = string->args.line_multi ?
                SCUI_WIDGET_STRING_SCROLL_LINE : SCUI_WIDGET_STRING_SCROLL_ITEM;
            if (scui_abs(string->args.offset) > string->args.limit + limit_all + gap) {
                
                string->args.offset = 0;
                
                string->unit_way = scui_lang_RTL() && !string->args.line_multi ? -1 : 1;
                /* 单次滚动结束标记 */
                if (string->unit_s)
                    string->unit_over = true;
            }
        }
        
        string->unit_anima = true;
        scui_widget_draw(widget->myself, NULL, false, 0);
        break;
    }
    case scui_event_draw_graph: {
        
        /* 无绘制目标 */
        if (string->str_utf8 == NULL)
            break;
        
        if (string->args.update)
        if (string->draw_cache) {
            /* 回收绘制缓存块 */
            scui_image_burn(string->draw_image_src);
        }
        
        string->args.utf8 = string->str_utf8;
        string->args.clip = widget->clip;
        string->args.clip.x = 0;
        string->args.clip.y = 0;
        scui_string_args_proc(&string->args);
        
        if (string->draw_cache) {
            /* 绘制缓存块,在第一次全局重绘时进行 */
            if (string->draw_image_src->pixel.data_bin == NULL) {
                string->draw_image_src->format = SCUI_PIXEL_CF_DEF_A;
                
                scui_coord_t hor_res = widget->clip.w;
                scui_coord_t ver_res = widget->clip.h;
                hor_res = string->args.line_multi ? hor_res : scui_max(hor_res, string->args.width);
                ver_res = string->args.line_multi ? scui_max(ver_res, string->args.height) : ver_res;
                /* 问题:上面用的剪切域生成的排版在这里重调, 会导致剪切域更变 */
                /* 影响:只能统一到控件剪切域, 会导致非峰值画布变大(问题不大) */
                
                scui_surface_t draw_surface = {0};
                scui_area_t area_image = {.w = hor_res,.h = ver_res};
                scui_image_make(string->draw_image_src, &area_image);
                scui_image_to_surface(string->draw_image_src, &draw_surface);
                scui_area_t draw_clip = scui_surface_area(&draw_surface);
                
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
                    string->args.color = SCUI_COLOR_MAKE32_SE(true, 0x0, 0xFFFFFFFF, 0xFFFFFFFF);
                }
                /* 后将文字绘制到这个画布中 */
                string->args.offset = 0;
                scui_draw_string(true, &draw_surface, draw_clip,
                    draw_clip, scui_alpha_cover, &string->args);
                
                if (string->args.regrad) {
                    /* 如果需要全局渐变,对绘制画布进行渐变 */
                    scui_draw_area_grads(true, &draw_surface, draw_clip,
                        string->args.grads->grad_s, string->args.grads->grad_n,
                        string->args.grads->grad_w, SCUI_COLOR_FILTER_TRANS,
                        scui_alpha_cover);
                }
            }
            
            if (string->args.limit <= 0)
                scui_widget_draw_image(widget->myself, NULL, string->draw_image, NULL, SCUI_COLOR_UNUSED);
            else {
                if (string->args.line_multi)
                    SCUI_LOG_DEBUG("offset y:%d", string->args.offset);
                else
                    SCUI_LOG_DEBUG("offset x:%d", string->args.offset);
                
                /* 滚动映射: 通用迭代+镜像(段1主显示/段2轮转补片) */
                /* mirror按控件自身RTL状态(align_rtl): 语言切换瞬间按旧语言渲染, 无过渡空帧 */
                scui_coord_t offset = string->args.offset;
                bool mirror = string->align_rtl && !string->args.line_multi;
                
                scui_coord_t limit_all = string->args.line_multi ? widget->clip.h : widget->clip.w;
                scui_coord_t gap = string->args.line_multi ?
                    SCUI_WIDGET_STRING_SCROLL_LINE : SCUI_WIDGET_STRING_SCROLL_ITEM;
                scui_coord_t size_t = string->args.line_multi ? string->args.height : string->args.width;
                scui_area_t  img_1 = {0}, img_2 = {0};
                scui_area_t  dst_1 = {0}, dst_2 = {0};
                bool have_1 = false, have_2 = false;
                scui_string_scroll_map(offset, mirror, size_t, limit_all,
                    string->args.mode_scroll == 1 ? gap : 0,
                    &img_1, &dst_1, &have_1, &img_2, &dst_2, &have_2);
                
                
                if (have_1) {
                    scui_area_t  draw_clip = widget->clip;
                    scui_area_t image_clip = scui_image_area(string->draw_image);
                    draw_clip.x = draw_clip.y = 0;
                    if (string->args.line_multi) {
                        image_clip.y = img_1.x;
                        image_clip.h = img_1.w;
                        draw_clip.y = dst_1.x;
                    } else {
                        image_clip.x = img_1.x;
                        image_clip.w = img_1.w;
                        draw_clip.x = dst_1.x;
                    }
                    scui_widget_draw_image(widget->myself, &draw_clip, string->draw_image, &image_clip, SCUI_COLOR_UNUSED);
                }
                if (have_2) {
                    scui_area_t  draw_clip = widget->clip;
                    scui_area_t image_clip = scui_image_area(string->draw_image);
                    draw_clip.x = draw_clip.y = 0;
                    if (string->args.line_multi) {
                        image_clip.y = img_2.x;
                        image_clip.h = img_2.w;
                        draw_clip.y = dst_2.x;
                    } else {
                        image_clip.x = img_2.x;
                        image_clip.w = img_2.w;
                        draw_clip.x = dst_2.x;
                    }
                    scui_widget_draw_image(widget->myself, &draw_clip, string->draw_image, &image_clip, SCUI_COLOR_UNUSED);
                }
            }
            
            break;
        }
        
        /* 无缓存块的绘制下 */
        /* 滚动映射(无缓存用绘制偏移表达) */
        /* mirror按控件自身RTL状态(align_rtl): 语言切换瞬间按旧语言渲染, 无过渡空帧 */
        scui_coord_t offset_bak = string->args.offset;
        bool mirror = string->align_rtl && !string->args.line_multi;
        /* 段1: 绘制偏移(窗口显示缓存区间 [s1, s1+W], off=-s1) */
        string->args.offset = mirror ? offset_bak - string->args.limit : offset_bak;
        scui_widget_draw_string(widget->myself, NULL, &string->args);
        /* */
        if (string->args.mode_scroll == 1 && string->args.limit > 0) {
            scui_coord_t limit_all = string->args.line_multi ? widget->clip.h : widget->clip.w;
            scui_coord_t gap = string->args.line_multi ?
                SCUI_WIDGET_STRING_SCROLL_LINE : SCUI_WIDGET_STRING_SCROLL_ITEM;
            scui_coord_t size_t = string->args.line_multi ? string->args.height : string->args.width;
            /* 段2: 轮转补片(与缓存路径同映射, 用绘制偏移表达) */
            scui_area_t  img_1 = {0}, img_2 = {0};
            scui_area_t  dst_1 = {0}, dst_2 = {0};
            bool have_1 = false, have_2 = false;
            scui_string_scroll_map(offset_bak, mirror, size_t, limit_all, gap,
                &img_1, &dst_1, &have_1, &img_2, &dst_2, &have_2);
            if (have_2) {
                /* 补片绘制偏移: 开头对齐进入侧 */
                if (mirror)
                    string->args.offset = -img_2.x;
                else
                    string->args.offset = limit_all - img_2.w;
                
                scui_widget_draw_string(widget->myself, NULL, &string->args);
            }
        }
        string->args.offset = offset_bak;
        break;
    }
    case scui_event_layout: {
        /* 自动尺寸或单行至少可显示 */
        scui_cache_font_unit_t font_unit = {0};
        font_unit.name = string->args.name;
        font_unit.size = string->args.size;
        scui_cache_font_load(&font_unit);
        scui_coord_t line_height = scui_font_line_height(font_unit.font);
        scui_cache_font_unload(&font_unit);
        
        scui_coord_t width  = widget->clip.w;
        scui_coord_t height = widget->clip.h;
        
        /* 自动布局:自动宽高(单行),自动高(多行) */
        if (string->args.line_multi) {
            SCUI_ASSERT(widget->clip.w > 0);
            SCUI_ASSERT(!widget->state.layout_w);
            
            if (widget->state.layout_h) {
                /* 尚未排版(首帧)或内容/尺寸有更新时, 先排版取内容高度 */
                if (string->args.update || string->args.height <= 0) {
                    string->args.utf8 = string->str_utf8;
                    string->args.clip = widget->clip;
                    string->args.clip.x = 0;
                    string->args.clip.y = 0;
                    string->args.update = true;
                    scui_string_args_proc(&string->args);
                }
                
                /* 文本无效时不处理(保持自动标记, 交由绘制断言捕获) */
                if (string->args.height > 0) height = string->args.height;
            }
        } else {
            if (widget->state.layout_w) {
                /* 尚未排版(首帧)或内容/尺寸有更新时, 先排版取内容宽度 */
                if (string->args.update || string->args.width <= 0) {
                    string->args.utf8 = string->str_utf8;
                    string->args.clip = widget->clip;
                    string->args.clip.x = 0;
                    string->args.clip.y = 0;
                    string->args.update = true;
                    scui_string_args_proc(&string->args);
                }
                
                /* 文本无效时不处理(保持自动标记, 交由绘制断言捕获) */
                if (string->args.width > 0) width = string->args.width;
            }
            
            /* 单行模式行高即高度 */
            /* 不足行高时, 高度取行高(保证最小一行) */
            if (widget->state.layout_h) height = line_height;
            if (line_height > height) height = line_height;
        }
        
        scui_widget_adjust_size(event->object, width, height);
        break;
    }
    case scui_event_self_size: {
        string->args.update = true;
        string->args.clip   = scui_widget_area(widget->myself);
        scui_widget_draw(widget->myself, NULL, false, 0);
        
        /* 回收绘制缓存块 */
        scui_image_burn(string->draw_image_src);
        break;
    }
    case scui_event_lang_change: {
        
        if (string->text != SCUI_HANDLE_INVALID) {
            string->unit_over = false;
            /* 滚动方向: RTL内容右移(offset正向), LTR内容左移(offset负向) */
            string->unit_way  = scui_lang_RTL() && !string->args.line_multi ? -1 : 1;
            /* 立即归位滚动位移(切换瞬间不再沿用旧语言滚动值) */
            string->args.offset = 0;
            scui_handle_t text = string->text;
            scui_string_update_text(widget->myself, SCUI_HANDLE_INVALID);
            scui_lang_get(&string->args.lang);
            string->args.size = scui_font_size_match(string->font_idx, string->args.size);
            string->args.name = scui_font_name_match(string->font_idx, string->args.lang);
            
            /* RTL语言: 左对齐自动调整为右对齐(切回常规语言时还原) */
            if (string->align_rtl) {
                string->args.align_hor = 0;
                string->align_rtl      = false;
            }
            if (string->args.align_hor == 0 && scui_lang_RTL()) {
                string->args.align_hor  = 1;
                string->align_rtl       = true;
            }
            scui_string_update_text(widget->myself, text);
            scui_widget_draw(widget->myself, NULL, true, 0);
        }
        break;
    }
    default:
        break;
    }
}
