/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

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


/*@brief 自定义控件:插件:上下文绘制
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_ctx(scui_custom_draw_dsc_t *draw_dsc)
{
    /* basic draw type */
    void scui_custom_draw_ctx_text(scui_custom_draw_dsc_t *draw_dsc);
    /* custom draw type */
    void scui_custom_draw_ctx_dial_ptr(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_spinner(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_slider(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_indicator(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_ring_edge(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_image_text(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_image_crect4(scui_custom_draw_dsc_t *draw_dsc);
    
    
    
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
    void scui_custom_draw_anim_ctx_dial_ptr(scui_custom_draw_dsc_t *draw_dsc);
    
    
    
    typedef void (*scui_custom_draw_anim_ctx_cb_t)(scui_custom_draw_dsc_t *draw_dsc);
    static const scui_custom_draw_anim_ctx_cb_t scui_custom_draw_anim_ctx_cb[scui_custom_draw_type_num] = {
        [scui_custom_draw_type_dial_ptr] =          scui_custom_draw_anim_ctx_dial_ptr,
    };
    
    SCUI_ASSERT(draw_dsc->type > scui_custom_draw_type_none);
    SCUI_ASSERT(draw_dsc->type < scui_custom_draw_type_num);
    scui_custom_draw_anim_ctx_cb[draw_dsc->type](draw_dsc);
}
