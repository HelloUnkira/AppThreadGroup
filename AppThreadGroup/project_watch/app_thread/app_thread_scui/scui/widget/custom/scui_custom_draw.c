/*实现目标:
 *    自定义插件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 自定义控件:插件:上下文绘制
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_ctx(scui_custom_draw_dsc_t *draw_dsc)
{
    void scui_custom_draw_ctx_qrcode(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_barcode(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_spinner(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_slider(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_indicator(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_ring_edge(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_image_text(scui_custom_draw_dsc_t *draw_dsc);
    void scui_custom_draw_ctx_image_crect4(scui_custom_draw_dsc_t *draw_dsc);
    
    switch (draw_dsc->type) {
    case scui_custom_draw_type_qrcode:
        scui_custom_draw_ctx_qrcode(draw_dsc);
        break;
    case scui_custom_draw_type_barcode:
        scui_custom_draw_ctx_barcode(draw_dsc);
        break;
    case scui_custom_draw_type_spinner:
        scui_custom_draw_ctx_spinner(draw_dsc);
        break;
    case scui_custom_draw_type_slider:
        scui_custom_draw_ctx_slider(draw_dsc);
        break;
    case scui_custom_draw_type_indicator:
        scui_custom_draw_ctx_indicator(draw_dsc);
        break;
    case scui_custom_draw_type_ring_edge:
        scui_custom_draw_ctx_ring_edge(draw_dsc);
        break;
    case scui_custom_draw_type_image_text:
        scui_custom_draw_ctx_image_text(draw_dsc);
        break;
    case scui_custom_draw_type_image_crect4:
        scui_custom_draw_ctx_image_crect4(draw_dsc);
        break;
    default:
        SCUI_LOG_ERROR("unknown type :%d", draw_dsc->type);
        SCUI_ASSERT(false);
        break;
    }
}
