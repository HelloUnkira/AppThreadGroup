#ifndef SCUI_DRAW_LETTER_H
#define SCUI_DRAW_LETTER_H

/* scui_draw_type_letter */
#define scui_draw_letter(dst_surface_v, dst_clip_v,                             \
    src_glyph_v, src_clip_v, src_alpha_v, src_color_v)                          \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_letter;                                     \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_glyph   = src_glyph_v;                                        \
    draw_dsc->src_clip    = src_clip_v;                                         \
    draw_dsc->src_alpha   = src_alpha_v;                                        \
    draw_dsc->src_color   = src_color_v;                                        \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_string */
#define scui_draw_string(dst_surface_v, dst_clip_v,                             \
    src_args_v, src_clip_v, src_alpha_v)                                        \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_string;                                     \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_args    = src_args_v;                                         \
    draw_dsc->src_clip    = src_clip_v;                                         \
    draw_dsc->src_alpha   = src_alpha_v;                                        \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

#endif
