#ifndef SCUI_DRAW_LETTER_H
#define SCUI_DRAW_LETTER_H

/* scui_draw_type_letter */
#define scui_draw_letter(dst_surface_v, dst_clip_v,                             \
    src_glyph_v, src_clip_v, src_alpha_v, src_color_v)                          \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_letter,                                          \
        .letter.dst_surface = dst_surface_v,                                    \
        .letter.dst_clip    = dst_clip_v,                                       \
        .letter.src_glyph   = src_glyph_v,                                      \
        .letter.src_clip    = src_clip_v,                                       \
        .letter.src_alpha   = src_alpha_v,                                      \
        .letter.src_color   = src_color_v,                                      \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

#endif
