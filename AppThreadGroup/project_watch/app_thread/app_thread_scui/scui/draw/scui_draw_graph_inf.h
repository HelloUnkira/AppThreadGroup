#ifndef SCUI_DRAW_INF_H
#define SCUI_DRAW_INF_H

/*@brief 暂时放于此处, 后续清理
 */



/* scui_draw_type_byte_copy */
#define scui_draw_byte_copy(sync_v, dst_addr_v, src_addr_v, src_len_v)                  \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_byte_copy;                                  \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_addr = dst_addr_v;                                            \
    draw_dsc_inst->src_addr = src_addr_v;                                            \
    draw_dsc_inst->src_len  = src_len_v;                                             \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_area_blur */
#define scui_draw_area_blur(sync_v, dst_surface_v, dst_clip_v)                          \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_area_blur;                                  \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_area_fill */
#define scui_draw_area_fill(sync_v, dst_surface_v, dst_clip_v,                          \
    src_alpha_v, src_color_v)                                                   \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_area_fill;                                  \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_area_fill_grad */
#define scui_draw_area_fill_grad(sync_v, dst_surface_v, dst_clip_v,                     \
    src_color_v, src_clip_v, src_alpha_v, src_way_v)                            \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_area_fill_grad;                             \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_way     = src_way_v;                                          \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_area_fill_grads */
#define scui_draw_area_fill_grads(sync_v, dst_surface_v, dst_clip_v,                    \
    src_grad_s_v, src_grad_n_v, src_way_v, src_filter_v, src_alpha_v)           \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_area_fill_grads;                            \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_grad_s  = src_grad_s_v;                                       \
    draw_dsc_inst->src_grad_n  = src_grad_n_v;                                       \
    draw_dsc_inst->src_filter  = src_filter_v;                                       \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_way     = src_way_v;                                          \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_area_copy */
#define scui_draw_area_copy(sync_v, dst_surface_v, dst_clip_v,                          \
    src_surface_v, src_clip_v)                                                  \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_area_copy;                                  \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_surface = src_surface_v;                                      \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_area_blend */
#define scui_draw_area_blend(sync_v, dst_surface_v, dst_clip_v,                         \
    src_surface_v, src_clip_v, src_color_v)                                     \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_area_blend;                                 \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_surface = src_surface_v;                                      \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_area_alpha_filter */
#define scui_draw_area_alpha_filter(sync_v, dst_surface_v, dst_clip_v,                  \
    src_surface_v, src_clip_v)                                                  \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_area_alpha_filter;                          \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_surface = src_surface_v;                                      \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_area_matrix_fill */
#define scui_draw_area_matrix_fill(sync_v, dst_surface_v, dst_clip_v,                   \
    src_clip_v, src_alpha_v, src_color_v, inv_matrix_v, src_matrix_v)           \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_area_matrix_fill;                           \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    draw_dsc_inst->inv_matrix  = inv_matrix_v;                                       \
    draw_dsc_inst->src_matrix  = src_matrix_v;                                       \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_area_matrix_blend */
#define scui_draw_area_matrix_blend(sync_v, dst_surface_v, dst_clip_v,                  \
    src_surface_v, src_clip_v, src_color_v, inv_matrix_v, src_matrix_v)         \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_area_matrix_blend;                          \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_surface = src_surface_v;                                      \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    draw_dsc_inst->inv_matrix  = inv_matrix_v;                                       \
    draw_dsc_inst->src_matrix  = src_matrix_v;                                       \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \




/* scui_draw_type_image */
#define scui_draw_image(sync_v, dst_surface_v, dst_clip_v,                              \
    src_image_v, src_clip_v, src_alpha_v, src_color_v)                          \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_image;                                      \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_image   = src_image_v;                                        \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_image_scale */
#define scui_draw_image_scale(sync_v, dst_surface_v, dst_clip_v,                        \
    src_image_v, src_clip_v, src_alpha_v, src_scale_v,                          \
    dst_offset_v, src_offset_v)                                                 \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_image_scale;                                \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_image   = src_image_v;                                        \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_scale   = src_scale_v;                                        \
    draw_dsc_inst->dst_offset  = dst_offset_v;                                       \
    draw_dsc_inst->src_offset  = src_offset_v;                                       \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_image_rotate */
#define scui_draw_image_rotate(sync_v, dst_surface_v, dst_clip_v,                       \
    src_image_v, src_clip_v, src_alpha_v, src_angle_v,                          \
    src_anchor_v, src_center_v)                                                 \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_image_rotate;                               \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_image   = src_image_v;                                        \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_angle   = src_angle_v;                                        \
    draw_dsc_inst->src_anchor  = src_anchor_v;                                       \
    draw_dsc_inst->src_center  = src_center_v;                                       \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_image_matrix_blend */
#define scui_draw_image_matrix_blend(sync_v, dst_surface_v, dst_clip_v,                 \
    src_image_v, src_clip_v, src_alpha_v, src_color_v,                          \
    inv_matrix_v, src_matrix_v)                                                 \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_image_matrix_blend;                         \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_image   = src_image_v;                                        \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    draw_dsc_inst->inv_matrix  = inv_matrix_v;                                       \
    draw_dsc_inst->src_matrix  = src_matrix_v;                                       \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \



/* scui_draw_type_ring */
#define scui_draw_ring(sync_v, dst_surface_v, dst_clip_v,                               \
    dst_center_v, src_image_e_v, src_image_v, src_clip_v,                       \
    src_angle_s_v, src_alpha_v, src_angle_e_v, src_color_v)                     \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_ring;                                       \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->dst_center  = dst_center_v;                                       \
    draw_dsc_inst->src_image_e = src_image_e_v;                                      \
    draw_dsc_inst->src_image   = src_image_v;                                        \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_angle_s = src_angle_s_v;                                      \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_angle_e = src_angle_e_v;                                      \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \



/* scui_draw_type_letter */
#define scui_draw_letter(sync_v, dst_surface_v, dst_clip_v,                             \
    src_glyph_v, src_clip_v, src_alpha_v, src_color_v)                          \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_letter;                                     \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_glyph   = src_glyph_v;                                        \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_string */
#define scui_draw_string(sync_v, dst_surface_v, dst_clip_v,                             \
    src_args_v, src_clip_v, src_alpha_v)                                        \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_string;                                     \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_args    = src_args_v;                                         \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \



/* scui_draw_type_qrcode */
#define scui_draw_qrcode(sync_v, dst_surface_v, dst_clip_v,                             \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)               \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_qrcode;                                     \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    draw_dsc_inst->src_size    = src_size_v;                                         \
    draw_dsc_inst->src_data    = src_data_v;                                         \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_barcode */
#define scui_draw_barcode(sync_v, dst_surface_v, dst_clip_v,                            \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)               \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = scui_draw_type_barcode;                                    \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_clip    = src_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_color   = src_color_v;                                        \
    draw_dsc_inst->src_size    = src_size_v;                                         \
    draw_dsc_inst->src_data    = src_data_v;                                         \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

/* scui_draw_type_graph */
#define scui_draw_graph(sync_v, dst_surface_v, dst_clip_v,                              \
    src_alpha_v, src_color_v, draw_dsc_v)                                           \
do {                                                                            \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                           \
    scui_draw_dsc_ready(&draw_dsc_inst);                                             \
    draw_dsc_inst->type = (draw_dsc_v)->type;                                        \
    draw_dsc_inst->sync = sync_v;                                                    \
    draw_dsc_inst->dst_surface = dst_surface_v;                                      \
    draw_dsc_inst->dst_clip    = dst_clip_v;                                         \
    draw_dsc_inst->src_alpha   = src_alpha_v;                                        \
    draw_dsc_inst->src_color   = src_color_v;                                        \
                                                                                     \
    draw_dsc_inst->src_center  = (draw_dsc_v)->src_center;                           \
    draw_dsc_inst->src_angle_s = (draw_dsc_v)->src_angle_s;                          \
    draw_dsc_inst->src_angle_e = (draw_dsc_v)->src_angle_e;                          \
    draw_dsc_inst->src_width   = (draw_dsc_v)->src_width;                            \
    draw_dsc_inst->src_pos_1   = (draw_dsc_v)->src_pos_1;                            \
    draw_dsc_inst->src_pos_2   = (draw_dsc_v)->src_pos_2;                            \
    draw_dsc_inst->src_radius  = (draw_dsc_v)->src_radius;                           \
    scui_draw_ctx_sched(draw_dsc_inst);                                              \
} while (0)                                                                     \

#endif
