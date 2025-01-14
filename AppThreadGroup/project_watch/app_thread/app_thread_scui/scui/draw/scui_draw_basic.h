#ifndef SCUI_DRAW_BASIC_H
#define SCUI_DRAW_BASIC_H

/* scui_draw_type_byte_copy */
#define scui_draw_byte_copy(dst_addr_v, src_addr_v, len_v)                      \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_byte_copy,                                       \
        .byte_copy.dst_addr = dst_addr_v,                                       \
        .byte_copy.src_addr = src_addr_v,                                       \
        .byte_copy.len      = len_v,                                            \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_area_blur */
#define scui_draw_area_blur(dst_surface_v, dst_clip_v)                          \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_area_blur,                                       \
        .area_blur.dst_surface = dst_surface_v,                                 \
        .area_blur.dst_clip    = dst_clip_v,                                    \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_area_fill */
#define scui_draw_area_fill(dst_surface_v, dst_clip_v,                          \
    src_alpha_v, src_color_v)                                                   \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_area_fill,                                       \
        .area_fill.dst_surface = dst_surface_v,                                 \
        .area_fill.dst_clip    = dst_clip_v,                                    \
        .area_fill.src_alpha   = src_alpha_v,                                   \
        .area_fill.src_color   = src_color_v,                                   \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_area_fill_grad */
#define scui_draw_area_fill_grad(dst_surface_v, dst_clip_v,                     \
    src_color_v, src_clip_v, src_alpha_v, src_way_v)                            \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_area_fill_grad,                                  \
        .area_fill_grad.dst_surface = dst_surface_v,                            \
        .area_fill_grad.dst_clip    = dst_clip_v,                               \
        .area_fill_grad.src_color   = src_color_v,                              \
        .area_fill_grad.src_clip    = src_clip_v,                               \
        .area_fill_grad.src_alpha   = src_alpha_v,                              \
        .area_fill_grad.src_way     = src_way_v,                                \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_area_fill_grads */
#define scui_draw_area_fill_grads(dst_surface_v, dst_clip_v,                    \
    src_grad_s_v, src_grad_n_v, src_way_v, src_filter_v, src_alpha_v)           \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_area_fill_grads,                                 \
        .area_fill_grads.dst_surface = dst_surface_v,                           \
        .area_fill_grads.dst_clip    = dst_clip_v,                              \
        .area_fill_grads.src_grad_s  = src_grad_s_v,                            \
        .area_fill_grads.src_grad_n  = src_grad_n_v,                            \
        .area_fill_grads.src_filter  = src_filter_v,                            \
        .area_fill_grads.src_alpha   = src_alpha_v,                             \
        .area_fill_grads.src_way     = src_way_v,                               \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_area_copy */
#define scui_draw_area_copy(dst_surface_v, dst_clip_v,                          \
    src_surface_v, src_clip_v)                                                  \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_area_copy,                                       \
        .area_copy.dst_surface = dst_surface_v,                                 \
        .area_copy.dst_clip    = dst_clip_v,                                    \
        .area_copy.src_surface = src_surface_v,                                 \
        .area_copy.src_clip    = src_clip_v,                                    \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_area_blend */
#define scui_draw_area_blend(dst_surface_v, dst_clip_v,                         \
    src_surface_v, src_clip_v, src_color_v)                                     \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_area_blend,                                      \
        .area_blend.dst_surface = dst_surface_v,                                \
        .area_blend.dst_clip    = dst_clip_v,                                   \
        .area_blend.src_surface = src_surface_v,                                \
        .area_blend.src_clip    = src_clip_v,                                   \
        .area_blend.src_color   = src_color_v,                                  \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_area_alpha_filter */
#define scui_draw_area_alpha_filter(dst_surface_v, dst_clip_v,                  \
    src_surface_v, src_clip_v)                                                  \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_area_alpha_filter,                               \
        .area_alpha_filter.dst_surface = dst_surface_v,                         \
        .area_alpha_filter.dst_clip    = dst_clip_v,                            \
        .area_alpha_filter.src_surface = src_surface_v,                         \
        .area_alpha_filter.src_clip    = src_clip_v,                            \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_area_matrix_fill */
#define scui_draw_area_matrix_fill(dst_surface_v, dst_clip_v,                   \
    src_clip_v, src_alpha_v, src_color_v, inv_matrix_v, src_matrix_v)           \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_area_matrix_fill,                                \
        .area_matrix_fill.dst_surface = dst_surface_v,                          \
        .area_matrix_fill.dst_clip    = dst_clip_v,                             \
        .area_matrix_fill.src_clip    = src_clip_v,                             \
        .area_matrix_fill.src_alpha   = src_alpha_v,                            \
        .area_matrix_fill.src_color   = src_color_v,                            \
        .area_matrix_fill.inv_matrix  = inv_matrix_v,                           \
        .area_matrix_fill.src_matrix  = src_matrix_v,                           \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_area_matrix_blend */
#define scui_draw_area_matrix_blend(dst_surface_v, dst_clip_v,                  \
    src_surface_v, src_clip_v, src_color_v, inv_matrix_v, src_matrix_v)         \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_area_matrix_blend,                               \
        .area_matrix_blend.dst_surface = dst_surface_v,                         \
        .area_matrix_blend.dst_clip    = dst_clip_v,                            \
        .area_matrix_blend.src_surface = src_surface_v,                         \
        .area_matrix_blend.src_clip    = src_clip_v,                            \
        .area_matrix_blend.src_color   = src_color_v,                           \
        .area_matrix_blend.inv_matrix  = inv_matrix_v,                          \
        .area_matrix_blend.src_matrix  = src_matrix_v,                          \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

#endif
