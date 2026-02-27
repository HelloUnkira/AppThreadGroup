#ifndef SCUI_DRAW_BASIC_H
#define SCUI_DRAW_BASIC_H



/* scui_draw_type_byte_copy */
#define scui_draw_byte_copy(dst_addr_v, src_addr_v, len_v)                      \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_byte_copy;                                  \
    draw_dsc->dst_addr = dst_addr_v;                                            \
    draw_dsc->src_addr = src_addr_v;                                            \
    draw_dsc->len      = len_v;                                                 \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_area_blur */
#define scui_draw_area_blur(dst_surface_v, dst_clip_v)                          \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_area_blur;                                  \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_area_fill */
#define scui_draw_area_fill(dst_surface_v, dst_clip_v,                          \
    src_alpha_v, src_color_v)                                                   \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_area_fill;                                  \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_alpha   = src_alpha_v;                                        \
    draw_dsc->src_color   = src_color_v;                                        \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_area_fill_grad */
#define scui_draw_area_fill_grad(dst_surface_v, dst_clip_v,                     \
    src_color_v, src_clip_v, src_alpha_v, src_way_v)                            \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_area_fill_grad;                             \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_color   = src_color_v;                                        \
    draw_dsc->src_clip    = src_clip_v;                                         \
    draw_dsc->src_alpha   = src_alpha_v;                                        \
    draw_dsc->src_way     = src_way_v;                                          \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_area_fill_grads */
#define scui_draw_area_fill_grads(dst_surface_v, dst_clip_v,                    \
    src_grad_s_v, src_grad_n_v, src_way_v, src_filter_v, src_alpha_v)           \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_area_fill_grads;                            \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_grad_s  = src_grad_s_v;                                       \
    draw_dsc->src_grad_n  = src_grad_n_v;                                       \
    draw_dsc->src_filter  = src_filter_v;                                       \
    draw_dsc->src_alpha   = src_alpha_v;                                        \
    draw_dsc->src_way     = src_way_v;                                          \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_area_copy */
#define scui_draw_area_copy(dst_surface_v, dst_clip_v,                          \
    src_surface_v, src_clip_v)                                                  \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_area_copy;                                  \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_surface = src_surface_v;                                      \
    draw_dsc->src_clip    = src_clip_v;                                         \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_area_blend */
#define scui_draw_area_blend(dst_surface_v, dst_clip_v,                         \
    src_surface_v, src_clip_v, src_color_v)                                     \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_area_blend;                                 \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_surface = src_surface_v;                                      \
    draw_dsc->src_clip    = src_clip_v;                                         \
    draw_dsc->src_color   = src_color_v;                                        \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_area_alpha_filter */
#define scui_draw_area_alpha_filter(dst_surface_v, dst_clip_v,                  \
    src_surface_v, src_clip_v)                                                  \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_area_alpha_filter;                          \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_surface = src_surface_v;                                      \
    draw_dsc->src_clip    = src_clip_v;                                         \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_area_matrix_fill */
#define scui_draw_area_matrix_fill(dst_surface_v, dst_clip_v,                   \
    src_clip_v, src_alpha_v, src_color_v, inv_matrix_v, src_matrix_v)           \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_area_matrix_fill;                           \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_clip    = src_clip_v;                                         \
    draw_dsc->src_alpha   = src_alpha_v;                                        \
    draw_dsc->src_color   = src_color_v;                                        \
    draw_dsc->inv_matrix  = inv_matrix_v;                                       \
    draw_dsc->src_matrix  = src_matrix_v;                                       \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

/* scui_draw_type_area_matrix_blend */
#define scui_draw_area_matrix_blend(dst_surface_v, dst_clip_v,                  \
    src_surface_v, src_clip_v, src_color_v, inv_matrix_v, src_matrix_v)         \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc);                                             \
    draw_dsc->type = scui_draw_type_area_matrix_blend;                          \
    draw_dsc->dst_surface = dst_surface_v;                                      \
    draw_dsc->dst_clip    = dst_clip_v;                                         \
    draw_dsc->src_surface = src_surface_v;                                      \
    draw_dsc->src_clip    = src_clip_v;                                         \
    draw_dsc->src_color   = src_color_v;                                        \
    draw_dsc->inv_matrix  = inv_matrix_v;                                       \
    draw_dsc->src_matrix  = src_matrix_v;                                       \
    scui_draw_ctx_sched(draw_dsc);                                                    \
} while (0)                                                                     \

#endif
