#ifndef SCUI_DRAW_INF_H
#define SCUI_DRAW_INF_H

/*@brief 暂时放于此处, 后续清理
 */



/* scui_draw_type_byte_copy */
#define scui_draw_byte_copy(sync_v, dst_addr_v, src_addr_v, src_len_v)              \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_byte_copy;                                 \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->byte_copy.dst_addr = dst_addr_v,                                 \
    draw_dsc_inst->byte_copy.src_addr = src_addr_v,                                 \
    draw_dsc_inst->byte_copy.src_len  = src_len_v,                                  \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_fill */
#define scui_draw_area_fill(sync_v, dst_surface_v, dst_clip_v,                      \
    src_alpha_v, src_color_v)                                                       \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_fill;                                 \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_fill.dst_surface = dst_surface_v,                           \
    draw_dsc_inst->area_fill.dst_clip    = dst_clip_v,                              \
    draw_dsc_inst->area_fill.src_alpha   = src_alpha_v,                             \
    draw_dsc_inst->area_fill.src_color   = src_color_v,                             \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_copy */
#define scui_draw_area_copy(sync_v, dst_surface_v, dst_clip_v,                      \
    src_surface_v, src_clip_v)                                                      \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_copy;                                 \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_copy.dst_surface = dst_surface_v,                           \
    draw_dsc_inst->area_copy.dst_clip    = dst_clip_v,                              \
    draw_dsc_inst->area_copy.src_surface = src_surface_v,                           \
    draw_dsc_inst->area_copy.src_clip    = src_clip_v,                              \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_blend */
#define scui_draw_area_blend(sync_v, dst_surface_v, dst_clip_v,                     \
    src_surface_v, src_clip_v, src_color_v)                                         \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_blend;                                \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_blend.dst_surface = dst_surface_v,                          \
    draw_dsc_inst->area_blend.dst_clip    = dst_clip_v,                             \
    draw_dsc_inst->area_blend.src_surface = src_surface_v,                          \
    draw_dsc_inst->area_blend.src_clip    = src_clip_v,                             \
    draw_dsc_inst->area_blend.src_color   = src_color_v,                            \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_matrix_fill */
#define scui_draw_area_matrix_fill(sync_v, dst_surface_v, dst_clip_v,               \
    src_clip_v, src_alpha_v, src_color_v, inv_matrix_v, src_matrix_v)               \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_matrix_fill;                          \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_matrix_fill.dst_surface = dst_surface_v,                    \
    draw_dsc_inst->area_matrix_fill.dst_clip    = dst_clip_v,                       \
    draw_dsc_inst->area_matrix_fill.src_clip    = src_clip_v,                       \
    draw_dsc_inst->area_matrix_fill.src_alpha   = src_alpha_v,                      \
    draw_dsc_inst->area_matrix_fill.src_color   = src_color_v,                      \
    draw_dsc_inst->area_matrix_fill.inv_matrix  = inv_matrix_v,                     \
    draw_dsc_inst->area_matrix_fill.src_matrix  = src_matrix_v,                     \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_matrix_blend */
#define scui_draw_area_matrix_blend(sync_v, dst_surface_v, dst_clip_v,              \
    src_surface_v, src_clip_v, src_color_v, inv_matrix_v, src_matrix_v)             \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_matrix_blend;                         \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_matrix_blend.dst_surface = dst_surface_v,                   \
    draw_dsc_inst->area_matrix_blend.dst_clip    = dst_clip_v,                      \
    draw_dsc_inst->area_matrix_blend.src_surface = src_surface_v,                   \
    draw_dsc_inst->area_matrix_blend.src_clip    = src_clip_v,                      \
    draw_dsc_inst->area_matrix_blend.src_color   = src_color_v,                     \
    draw_dsc_inst->area_matrix_blend.inv_matrix  = inv_matrix_v,                    \
    draw_dsc_inst->area_matrix_blend.src_matrix  = src_matrix_v,                    \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \



/* scui_draw_type_area_convolution */
#define scui_draw_area_convolution(sync_v, dst_surface_v, dst_clip_v,               \
    kernel_v, scale_v)                                                              \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_convolution;                          \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_convolution.dst_surface = dst_surface_v,                    \
    draw_dsc_inst->area_convolution.dst_clip    = dst_clip_v,                       \
    draw_dsc_inst->area_convolution.kernel      = kernel_v,                         \
    draw_dsc_inst->area_convolution.scale       = scale_v,                          \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_dither */
#define scui_draw_area_dither(sync_v, dst_surface_v, dst_clip_v)                    \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_dither;                               \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_dither.dst_surface = dst_surface_v,                         \
    draw_dsc_inst->area_dither.dst_clip    = dst_clip_v,                            \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_blur */
#define scui_draw_area_blur(sync_v, dst_surface_v, dst_clip_v)                      \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_blur;                                 \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_blur.dst_surface = dst_surface_v,                           \
    draw_dsc_inst->area_blur.dst_clip    = dst_clip_v,                              \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_grad */
#define scui_draw_area_grad(sync_v, dst_surface_v, dst_clip_v,                      \
    src_clip_v, src_color_v, src_alpha_v, src_way_v)                                \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_grad;                                 \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_grad.dst_surface = dst_surface_v,                           \
    draw_dsc_inst->area_grad.dst_clip    = dst_clip_v,                              \
    draw_dsc_inst->area_grad.src_clip    = src_clip_v,                              \
    draw_dsc_inst->area_grad.src_color   = src_color_v,                             \
    draw_dsc_inst->area_grad.src_alpha   = src_alpha_v,                             \
    draw_dsc_inst->area_grad.src_way     = src_way_v,                               \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_grads */
#define scui_draw_area_grads(sync_v, dst_surface_v, dst_clip_v,                     \
    src_grad_s_v, src_grad_n_v, src_way_v, src_filter_v, src_alpha_v)               \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_grads;                                \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_grads.dst_surface = dst_surface_v,                          \
    draw_dsc_inst->area_grads.dst_clip    = dst_clip_v,                             \
    draw_dsc_inst->area_grads.src_grad_s  = src_grad_s_v,                           \
    draw_dsc_inst->area_grads.src_grad_n  = src_grad_n_v,                           \
    draw_dsc_inst->area_grads.src_filter  = src_filter_v,                           \
    draw_dsc_inst->area_grads.src_alpha   = src_alpha_v,                            \
    draw_dsc_inst->area_grads.src_way     = src_way_v,                              \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_area_afilter */
#define scui_draw_area_afilter(sync_v, dst_surface_v, dst_clip_v,                   \
    src_surface_v, src_clip_v)                                                      \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_area_afilter;                              \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->area_afilter.dst_surface = dst_surface_v,                        \
    draw_dsc_inst->area_afilter.dst_clip    = dst_clip_v,                           \
    draw_dsc_inst->area_afilter.src_surface = src_surface_v,                        \
    draw_dsc_inst->area_afilter.src_clip    = src_clip_v,                           \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \



/* scui_draw_type_image */
#define scui_draw_image(sync_v, dst_surface_v, dst_clip_v,                          \
    src_image_v, src_clip_v, src_alpha_v, src_color_v)                              \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_image;                                     \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->image.dst_surface = dst_surface_v,                               \
    draw_dsc_inst->image.dst_clip    = dst_clip_v,                                  \
    draw_dsc_inst->image.src_image   = src_image_v,                                 \
    draw_dsc_inst->image.src_clip    = src_clip_v,                                  \
    draw_dsc_inst->image.src_alpha   = src_alpha_v,                                 \
    draw_dsc_inst->image.src_color   = src_color_v,                                 \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_image_scale */
#define scui_draw_image_scale(sync_v, dst_surface_v, dst_clip_v,                    \
    src_image_v, src_clip_v, src_alpha_v, src_scale_v,                              \
    dst_offset_v, src_offset_v)                                                     \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_image_scale;                               \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->image_scale.dst_surface = dst_surface_v,                         \
    draw_dsc_inst->image_scale.dst_clip    = dst_clip_v,                            \
    draw_dsc_inst->image_scale.src_image   = src_image_v,                           \
    draw_dsc_inst->image_scale.src_clip    = src_clip_v,                            \
    draw_dsc_inst->image_scale.src_alpha   = src_alpha_v,                           \
    draw_dsc_inst->image_scale.src_scale   = src_scale_v,                           \
    draw_dsc_inst->image_scale.dst_offset  = dst_offset_v,                          \
    draw_dsc_inst->image_scale.src_offset  = src_offset_v,                          \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_image_rotate */
#define scui_draw_image_rotate(sync_v, dst_surface_v, dst_clip_v,                   \
    src_image_v, src_clip_v, src_alpha_v, src_angle_v,                              \
    src_anchor_v, src_center_v)                                                     \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_image_rotate;                              \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->image_rotate.dst_surface = dst_surface_v,                        \
    draw_dsc_inst->image_rotate.dst_clip    = dst_clip_v,                           \
    draw_dsc_inst->image_rotate.src_image   = src_image_v,                          \
    draw_dsc_inst->image_rotate.src_clip    = src_clip_v,                           \
    draw_dsc_inst->image_rotate.src_alpha   = src_alpha_v,                          \
    draw_dsc_inst->image_rotate.src_angle   = src_angle_v,                          \
    draw_dsc_inst->image_rotate.src_anchor  = src_anchor_v,                         \
    draw_dsc_inst->image_rotate.src_center  = src_center_v,                         \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_image_matrix_blend */
#define scui_draw_image_matrix_blend(sync_v, dst_surface_v, dst_clip_v,             \
    src_image_v, src_clip_v, src_alpha_v, src_color_v,                              \
    inv_matrix_v, src_matrix_v)                                                     \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_image_matrix_blend;                        \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->image_matrix_blend.dst_surface = dst_surface_v,                  \
    draw_dsc_inst->image_matrix_blend.dst_clip    = dst_clip_v,                     \
    draw_dsc_inst->image_matrix_blend.src_image   = src_image_v,                    \
    draw_dsc_inst->image_matrix_blend.src_clip    = src_clip_v,                     \
    draw_dsc_inst->image_matrix_blend.src_alpha   = src_alpha_v,                    \
    draw_dsc_inst->image_matrix_blend.src_color   = src_color_v,                    \
    draw_dsc_inst->image_matrix_blend.inv_matrix  = inv_matrix_v,                   \
    draw_dsc_inst->image_matrix_blend.src_matrix  = src_matrix_v,                   \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \



/* scui_draw_type_ring */
#define scui_draw_ring(sync_v, dst_surface_v, dst_clip_v,                           \
    dst_center_v, src_image_e_v, src_image_v, src_clip_v,                           \
    src_angle_s_v, src_alpha_v, src_angle_e_v, src_color_v)                         \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_ring;                                      \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->ring.dst_surface = dst_surface_v,                                \
    draw_dsc_inst->ring.dst_clip    = dst_clip_v,                                   \
    draw_dsc_inst->ring.dst_center  = dst_center_v,                                 \
    draw_dsc_inst->ring.src_image_e = src_image_e_v,                                \
    draw_dsc_inst->ring.src_image   = src_image_v,                                  \
    draw_dsc_inst->ring.src_clip    = src_clip_v,                                   \
    draw_dsc_inst->ring.src_angle_s = src_angle_s_v,                                \
    draw_dsc_inst->ring.src_alpha   = src_alpha_v,                                  \
    draw_dsc_inst->ring.src_angle_e = src_angle_e_v,                                \
    draw_dsc_inst->ring.src_color   = src_color_v,                                  \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \



/* scui_draw_type_letter */
#define scui_draw_letter(sync_v, dst_surface_v, dst_clip_v,                         \
    src_glyph_v, src_clip_v, src_alpha_v, src_color_v)                              \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_letter;                                    \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->letter.dst_surface = dst_surface_v,                              \
    draw_dsc_inst->letter.dst_clip    = dst_clip_v,                                 \
    draw_dsc_inst->letter.src_glyph   = src_glyph_v,                                \
    draw_dsc_inst->letter.src_clip    = src_clip_v,                                 \
    draw_dsc_inst->letter.src_alpha   = src_alpha_v,                                \
    draw_dsc_inst->letter.src_color   = src_color_v,                                \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_string */
#define scui_draw_string(sync_v, dst_surface_v, dst_clip_v,                         \
    src_clip_v, src_alpha_v, src_args_v)                                            \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_string;                                    \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->string.dst_surface = dst_surface_v,                              \
    draw_dsc_inst->string.dst_clip    = dst_clip_v,                                 \
    draw_dsc_inst->string.src_clip    = src_clip_v,                                 \
    draw_dsc_inst->string.src_alpha   = src_alpha_v,                                \
    draw_dsc_inst->string.src_args    = src_args_v,                                 \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \



/* scui_draw_type_qrcode */
#define scui_draw_qrcode(sync_v, dst_surface_v, dst_clip_v,                         \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)                   \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_qrcode;                                    \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->qrcode.dst_surface = dst_surface_v,                              \
    draw_dsc_inst->qrcode.dst_clip    = dst_clip_v,                                 \
    draw_dsc_inst->qrcode.src_clip    = src_clip_v,                                 \
    draw_dsc_inst->qrcode.src_alpha   = src_alpha_v,                                \
    draw_dsc_inst->qrcode.src_color   = src_color_v,                                \
    draw_dsc_inst->qrcode.src_size    = src_size_v,                                 \
    draw_dsc_inst->qrcode.src_data    = src_data_v,                                 \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_barcode */
#define scui_draw_barcode(sync_v, dst_surface_v, dst_clip_v,                        \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)                   \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = scui_draw_type_barcode;                                   \
    draw_dsc_inst->sync = sync_v;                                                   \
    draw_dsc_inst->qrcode.dst_surface = dst_surface_v,                              \
    draw_dsc_inst->qrcode.dst_clip    = dst_clip_v,                                 \
    draw_dsc_inst->qrcode.src_clip    = src_clip_v,                                 \
    draw_dsc_inst->qrcode.src_alpha   = src_alpha_v,                                \
    draw_dsc_inst->qrcode.src_color   = src_color_v,                                \
    draw_dsc_inst->qrcode.src_size    = src_size_v,                                 \
    draw_dsc_inst->qrcode.src_data    = src_data_v,                                 \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

/* scui_draw_type_graph */
#define scui_draw_graph(sync_v, dst_surface_v, dst_clip_v,                          \
    src_alpha_v, src_color_v, draw_dsc_v)                                           \
do {                                                                                \
    scui_draw_dsc_t *draw_dsc_inst = NULL;                                          \
    scui_draw_dsc_ready(&draw_dsc_inst);                                            \
    draw_dsc_inst->type = (draw_dsc_v)->type;                                       \
    draw_dsc_inst->sync =  sync_v;                                                  \
    draw_dsc_inst->graph.dst_surface = dst_surface_v;                               \
    draw_dsc_inst->graph.dst_clip    = dst_clip_v;                                  \
    draw_dsc_inst->graph.src_alpha   = src_alpha_v;                                 \
    draw_dsc_inst->graph.src_color   = src_color_v;                                 \
                                                                                    \
    draw_dsc_inst->graph.src_area    = (draw_dsc_v)->graph.src_area;                \
    draw_dsc_inst->graph.src_center  = (draw_dsc_v)->graph.src_center;              \
    draw_dsc_inst->graph.src_angle_s = (draw_dsc_v)->graph.src_angle_s;             \
    draw_dsc_inst->graph.src_angle_e = (draw_dsc_v)->graph.src_angle_e;             \
    draw_dsc_inst->graph.src_width   = (draw_dsc_v)->graph.src_width;               \
    draw_dsc_inst->graph.src_pos_1   = (draw_dsc_v)->graph.src_pos_1;               \
    draw_dsc_inst->graph.src_pos_2   = (draw_dsc_v)->graph.src_pos_2;               \
    draw_dsc_inst->graph.src_radius  = (draw_dsc_v)->graph.src_radius;              \
    draw_dsc_inst->graph.src_shadow  = (draw_dsc_v)->graph.src_shadow;              \
    scui_draw_dsc_task(draw_dsc_inst);                                              \
} while (0)                                                                         \

#endif
