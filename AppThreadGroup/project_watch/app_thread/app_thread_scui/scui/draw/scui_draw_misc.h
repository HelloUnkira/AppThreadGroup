#ifndef SCUI_DRAW_MISC_H
#define SCUI_DRAW_MISC_H

/* scui_draw_type_qrcode */
#define scui_draw_qrcode(dst_surface_v, dst_clip_v,                             \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)               \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_qrcode;                                     \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_clip    = src_clip_v;                                         \
    draw_dsc->src_alpha   = src_alpha_v;                                        \
    draw_dsc->src_color   = src_color_v;                                        \
    draw_dsc->src_size    = src_size_v;                                         \
    draw_dsc->src_data    = src_data_v;                                         \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_barcode */
#define scui_draw_barcode(dst_surface_v, dst_clip_v,                            \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)               \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_barcode;                                    \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_clip    = src_clip_v;                                         \
    draw_dsc->src_alpha   = src_alpha_v;                                        \
    draw_dsc->src_color   = src_color_v;                                        \
    draw_dsc->src_size    = src_size_v;                                         \
    draw_dsc->src_data    = src_data_v;                                         \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

#endif
