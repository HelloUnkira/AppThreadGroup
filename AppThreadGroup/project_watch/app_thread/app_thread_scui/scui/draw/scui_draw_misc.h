#ifndef SCUI_DRAW_MISC_H
#define SCUI_DRAW_MISC_H

/* scui_draw_type_qrcode */
#define scui_draw_qrcode(dst_surface_v, dst_clip_v,                             \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)               \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_qrcode,                                          \
        .qrcode.dst_surface = dst_surface_v,                                    \
        .qrcode.dst_clip    = dst_clip_v,                                       \
        .qrcode.src_clip    = src_clip_v,                                       \
        .qrcode.src_alpha   = src_alpha_v,                                      \
        .qrcode.src_color   = src_color_v,                                      \
        .qrcode.src_size    = src_size_v,                                       \
        .qrcode.src_data    = src_data_v,                                       \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_barcode */
#define scui_draw_barcode(dst_surface_v, dst_clip_v,                            \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)               \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_barcode,                                         \
        .qrcode.dst_surface = dst_surface_v,                                    \
        .qrcode.dst_clip    = dst_clip_v,                                       \
        .qrcode.src_clip    = src_clip_v,                                       \
        .qrcode.src_alpha   = src_alpha_v,                                      \
        .qrcode.src_color   = src_color_v,                                      \
        .qrcode.src_size    = src_size_v,                                       \
        .qrcode.src_data    = src_data_v,                                       \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

#endif
