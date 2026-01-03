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
void scui_custom_make(void *inst, void *inst_maker, scui_handle_t *handle)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_custom_t *custom = widget;
    scui_custom_maker_t *custom_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_custom));
    // 自定义控件既可以是根控件(子画布控件树),也可以是非根控件
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_custom_burn(scui_handle_t handle)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    // 回收绘制描述符(全局)
    SCUI_MEM_FREE(custom->draw_dsc);
    
    /* 析构基础控件实例 */
    scui_widget_burn(widget);
}

/*@brief 自定义控件子控件(可选)
 *@param handle   自定义控件句柄
 *@param handle_s 控件句柄地址
 */
void scui_custom_handle_m(scui_handle_t handle, scui_handle_t **handle_m)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    *handle_m = &custom->handle_m;
}

/*@brief 自定义控件子控件树(可选)
 *@param handle   自定义控件句柄
 *@param handle_s 控件句柄地址
 */
void scui_custom_handle_s(scui_handle_t handle, scui_handle_t **handle_s)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    *handle_s = &custom->handle_s;
}

/*@brief 自定义控件绘制描述符(全局,唯一)
 *@param handle   自定义控件句柄
 *@param draw_dsc 绘制描述符
 */
void scui_custom_draw_dsc(scui_handle_t handle, void **draw_dsc)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    SCUI_ASSERT(draw_dsc != NULL);
    if (custom->draw_dsc == NULL)
        custom->draw_dsc  = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_custom_draw_dsc_t));
    
    SCUI_ASSERT(custom->draw_dsc != NULL);
    *draw_dsc = custom->draw_dsc;
}

/*************************************************************************************************/
/*************************************************************************************************/
/*************************************************************************************************/

/* basic draw type */
void scui_custom_draw_ctx_text(scui_custom_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_handle_t     handle = draw_dsc->text.handle;
    scui_area_t      *target = draw_dsc->text.target;
    scui_string_args_t *args = draw_dsc->text.args;
    scui_handle_t       text = draw_dsc->text.text;
    /* draw dsc args<e> */
    SCUI_LOG_DEBUG("");
    
    if (text != SCUI_HANDLE_INVALID)
        args->utf8 = scui_multi_lang_str(text, args->lang);
    
    args->clip   = target != NULL ? *target : scui_widget_clip(handle);
    args->update = true;
    scui_string_args_process(args);
    scui_widget_draw_string(handle, target, args);
    
    // 绘制完就地回收
    args->update = true;
    args->name = SCUI_HANDLE_INVALID;
    args->utf8 = NULL;
    scui_string_args_process(args);
}

/*@brief 自定义控件:插件:上下文绘制(帧动画调度)
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_ctx_text(scui_custom_draw_dsc_t *draw_dsc);
void scui_custom_draw_ctx_dial_ptr(scui_custom_draw_dsc_t *draw_dsc);
void scui_custom_draw_anim_ctx_dial_ptr(scui_custom_draw_dsc_t *draw_dsc);
void scui_custom_draw_ctx_spinner(scui_custom_draw_dsc_t *draw_dsc);
void scui_custom_draw_ctx_slider(scui_custom_draw_dsc_t *draw_dsc);
void scui_custom_draw_ctx_indicator(scui_custom_draw_dsc_t *draw_dsc);
void scui_custom_draw_ctx_ring_edge(scui_custom_draw_dsc_t *draw_dsc);
void scui_custom_draw_ctx_image_text(scui_custom_draw_dsc_t *draw_dsc);
void scui_custom_draw_ctx_image_crect4(scui_custom_draw_dsc_t *draw_dsc);

/*@brief 自定义控件:插件:上下文绘制
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_ctx(scui_custom_draw_dsc_t *draw_dsc)
{
    typedef void (*scui_custom_draw_ctx_cb_t)(scui_custom_draw_dsc_t *draw_dsc);
    static const scui_custom_draw_ctx_cb_t scui_custom_draw_ctx_cb[scui_custom_draw_type_num] = {
        /* basic draw type */
        [scui_custom_draw_type_text] =              scui_custom_draw_ctx_text,
        /* custom draw type */
        [scui_custom_draw_type_dial_ptr] =          scui_custom_draw_ctx_dial_ptr,
        [scui_custom_draw_type_spinner] =           scui_custom_draw_ctx_spinner,
        [scui_custom_draw_type_slider] =            scui_custom_draw_ctx_slider,
        [scui_custom_draw_type_indicator] =         scui_custom_draw_ctx_indicator,
        [scui_custom_draw_type_ring_edge] =         scui_custom_draw_ctx_ring_edge,
        [scui_custom_draw_type_image_text] =        scui_custom_draw_ctx_image_text,
        [scui_custom_draw_type_image_crect4] =      scui_custom_draw_ctx_image_crect4,
    };
    
    SCUI_ASSERT(draw_dsc->type > scui_custom_draw_type_none);
    SCUI_ASSERT(draw_dsc->type < scui_custom_draw_type_num);
    scui_custom_draw_ctx_cb[draw_dsc->type](draw_dsc);
}

/*@brief 自定义控件:插件:上下文绘制(帧动画调度)
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_anim_ctx(scui_custom_draw_dsc_t *draw_dsc)
{
    typedef void (*scui_custom_draw_anim_ctx_cb_t)(scui_custom_draw_dsc_t *draw_dsc);
    static const scui_custom_draw_anim_ctx_cb_t scui_custom_draw_anim_ctx_cb[scui_custom_draw_type_num] = {
        [scui_custom_draw_type_dial_ptr] =          scui_custom_draw_anim_ctx_dial_ptr,
    };
    
    SCUI_ASSERT(draw_dsc->type > scui_custom_draw_type_none);
    SCUI_ASSERT(draw_dsc->type < scui_custom_draw_type_num);
    scui_custom_draw_anim_ctx_cb[draw_dsc->type](draw_dsc);
}
