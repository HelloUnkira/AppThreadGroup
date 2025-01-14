#ifndef SCUI_DRAW_STRING_H
#define SCUI_DRAW_STRING_H

/* scui_draw_type_string */
#define scui_draw_string(dst_surface_v, dst_clip_v,                             \
    src_args_v, src_clip_v, src_alpha_v)                                        \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_string,                                          \
        .string.dst_surface = dst_surface_v,                                    \
        .string.dst_clip    = dst_clip_v,                                       \
        .string.src_args    = src_args_v,                                       \
        .string.src_clip    = src_clip_v,                                       \
        .string.src_alpha   = src_alpha_v,                                      \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

#endif
