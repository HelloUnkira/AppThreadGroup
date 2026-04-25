#ifndef SCUI_DRAW_INF_H
#define SCUI_DRAW_INF_H

/*@brief 暂时放于此处, 后续清理
 */



/* scui_draw_type_byte_copy */
#define scui_draw_byte_copy(sync_v, dst_addr_v, src_addr_v, src_len_v)              \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_byte_copy;                                     \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->byte_copy.dst_addr = dst_addr_v,                                     \
    scui_dd_i->byte_copy.src_addr = src_addr_v,                                     \
    scui_dd_i->byte_copy.src_len  = src_len_v,                                      \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_fill */
#define scui_draw_area_fill(sync_v, dst_surface_v, dst_clip_v,                      \
    src_alpha_v, src_color_v)                                                       \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_fill;                                     \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_fill.dst_surface = dst_surface_v,                               \
    scui_dd_i->area_fill.dst_clip    = dst_clip_v,                                  \
    scui_dd_i->area_fill.src_alpha   = src_alpha_v,                                 \
    scui_dd_i->area_fill.src_color   = src_color_v,                                 \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_copy */
#define scui_draw_area_copy(sync_v, dst_surface_v, dst_clip_v,                      \
    src_surface_v, src_clip_v)                                                      \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_copy;                                     \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_copy.dst_surface = dst_surface_v,                               \
    scui_dd_i->area_copy.dst_clip    = dst_clip_v,                                  \
    scui_dd_i->area_copy.src_surface = src_surface_v,                               \
    scui_dd_i->area_copy.src_clip    = src_clip_v,                                  \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_blend */
#define scui_draw_area_blend(sync_v, dst_surface_v, dst_clip_v,                     \
    src_surface_v, src_clip_v, src_color_v)                                         \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_blend;                                    \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_blend.dst_surface = dst_surface_v,                              \
    scui_dd_i->area_blend.dst_clip    = dst_clip_v,                                 \
    scui_dd_i->area_blend.src_surface = src_surface_v,                              \
    scui_dd_i->area_blend.src_clip    = src_clip_v,                                 \
    scui_dd_i->area_blend.src_color   = src_color_v,                                \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_scale_blend */
#define scui_draw_area_scale_blend(sync_v, dst_surface_v, dst_clip_v,               \
    src_surface_v, src_clip_v, src_color_v, src_scale_v,                            \
    dst_offset_v, src_offset_v)                                                     \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_scale_blend;                              \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_scale_blend.dst_surface = dst_surface_v,                        \
    scui_dd_i->area_scale_blend.dst_clip    = dst_clip_v,                           \
    scui_dd_i->area_scale_blend.src_surface = src_surface_v,                        \
    scui_dd_i->area_scale_blend.src_clip    = src_clip_v,                           \
    scui_dd_i->area_scale_blend.src_color   = src_color_v,                          \
    scui_dd_i->area_scale_blend.src_scale   = src_scale_v,                          \
    scui_dd_i->area_scale_blend.dst_offset  = dst_offset_v,                         \
    scui_dd_i->area_scale_blend.src_offset  = src_offset_v,                         \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_rotate_blend */
#define scui_draw_area_rotate_blend(sync_v, dst_surface_v, dst_clip_v,              \
    src_surface_v, src_clip_v, src_color_v, src_angle_v,                            \
    src_anchor_v, src_center_v)                                                     \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_rotate_blend;                             \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_rotate_blend.dst_surface = dst_surface_v,                       \
    scui_dd_i->area_rotate_blend.dst_clip    = dst_clip_v,                          \
    scui_dd_i->area_rotate_blend.src_surface = src_surface_v,                       \
    scui_dd_i->area_rotate_blend.src_clip    = src_clip_v,                          \
    scui_dd_i->area_rotate_blend.src_color   = src_color_v,                         \
    scui_dd_i->area_rotate_blend.src_angle   = src_angle_v,                         \
    scui_dd_i->area_rotate_blend.src_anchor  = src_anchor_v,                        \
    scui_dd_i->area_rotate_blend.src_center  = src_center_v,                        \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_matrix_blend */
#define scui_draw_area_matrix_blend(sync_v, dst_surface_v, dst_clip_v,              \
    src_surface_v, src_clip_v, src_color_v, inv_matrix_v, src_matrix_v)             \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_matrix_blend;                             \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_matrix_blend.dst_surface = dst_surface_v,                       \
    scui_dd_i->area_matrix_blend.dst_clip    = dst_clip_v,                          \
    scui_dd_i->area_matrix_blend.src_surface = src_surface_v,                       \
    scui_dd_i->area_matrix_blend.src_clip    = src_clip_v,                          \
    scui_dd_i->area_matrix_blend.src_color   = src_color_v,                         \
    scui_dd_i->area_matrix_blend.inv_matrix  = inv_matrix_v,                        \
    scui_dd_i->area_matrix_blend.src_matrix  = src_matrix_v,                        \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_matrix_fill */
#define scui_draw_area_matrix_fill(sync_v, dst_surface_v, dst_clip_v,               \
    src_clip_v, src_alpha_v, src_color_v, inv_matrix_v, src_matrix_v)               \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_matrix_fill;                              \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_matrix_fill.dst_surface = dst_surface_v,                        \
    scui_dd_i->area_matrix_fill.dst_clip    = dst_clip_v,                           \
    scui_dd_i->area_matrix_fill.src_clip    = src_clip_v,                           \
    scui_dd_i->area_matrix_fill.src_alpha   = src_alpha_v,                          \
    scui_dd_i->area_matrix_fill.src_color   = src_color_v,                          \
    scui_dd_i->area_matrix_fill.inv_matrix  = inv_matrix_v,                         \
    scui_dd_i->area_matrix_fill.src_matrix  = src_matrix_v,                         \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \



/* scui_draw_type_area_dither */
#define scui_draw_area_dither(sync_v, dst_surface_v, dst_clip_v)                    \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_dither;                                   \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_dither.dst_surface = dst_surface_v,                             \
    scui_dd_i->area_dither.dst_clip    = dst_clip_v,                                \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_blur */
#define scui_draw_area_blur(sync_v, dst_surface_v, dst_clip_v)                      \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_blur;                                     \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_blur.dst_surface = dst_surface_v,                               \
    scui_dd_i->area_blur.dst_clip    = dst_clip_v,                                  \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_grad */
#define scui_draw_area_grad(sync_v, dst_surface_v, dst_clip_v,                      \
    src_clip_v, src_color_v, src_alpha_v, src_way_v)                                \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_grad;                                     \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_grad.dst_surface = dst_surface_v,                               \
    scui_dd_i->area_grad.dst_clip    = dst_clip_v,                                  \
    scui_dd_i->area_grad.src_clip    = src_clip_v,                                  \
    scui_dd_i->area_grad.src_color   = src_color_v,                                 \
    scui_dd_i->area_grad.src_alpha   = src_alpha_v,                                 \
    scui_dd_i->area_grad.src_way     = src_way_v,                                   \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_grads */
#define scui_draw_area_grads(sync_v, dst_surface_v, dst_clip_v,                     \
    src_grad_s_v, src_grad_n_v, src_way_v, src_filter_v, src_alpha_v)               \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_grads;                                    \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_grads.dst_surface = dst_surface_v,                              \
    scui_dd_i->area_grads.dst_clip    = dst_clip_v,                                 \
    scui_dd_i->area_grads.src_grad_s  = src_grad_s_v,                               \
    scui_dd_i->area_grads.src_grad_n  = src_grad_n_v,                               \
    scui_dd_i->area_grads.src_filter  = src_filter_v,                               \
    scui_dd_i->area_grads.src_alpha   = src_alpha_v,                                \
    scui_dd_i->area_grads.src_way     = src_way_v,                                  \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_area_afilter */
#define scui_draw_area_afilter(sync_v, dst_surface_v, dst_clip_v,                   \
    src_surface_v, src_clip_v)                                                      \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_area_afilter;                                  \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->area_afilter.dst_surface = dst_surface_v,                            \
    scui_dd_i->area_afilter.dst_clip    = dst_clip_v,                               \
    scui_dd_i->area_afilter.src_surface = src_surface_v,                            \
    scui_dd_i->area_afilter.src_clip    = src_clip_v,                               \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \



/* scui_draw_type_image */
#define scui_draw_image(sync_v, dst_surface_v, dst_clip_v,                          \
    src_image_v, src_clip_v, src_alpha_v, src_color_v)                              \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_image;                                         \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->image.dst_surface = dst_surface_v,                                   \
    scui_dd_i->image.dst_clip    = dst_clip_v,                                      \
    scui_dd_i->image.src_image   = src_image_v,                                     \
    scui_dd_i->image.src_clip    = src_clip_v,                                      \
    scui_dd_i->image.src_alpha   = src_alpha_v,                                     \
    scui_dd_i->image.src_color   = src_color_v,                                     \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_image_scale */
#define scui_draw_image_scale(sync_v, dst_surface_v, dst_clip_v,                    \
    src_image_v, src_clip_v, src_alpha_v, src_scale_v,                              \
    dst_offset_v, src_offset_v)                                                     \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_image_scale;                                   \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->image_scale.dst_surface = dst_surface_v,                             \
    scui_dd_i->image_scale.dst_clip    = dst_clip_v,                                \
    scui_dd_i->image_scale.src_image   = src_image_v,                               \
    scui_dd_i->image_scale.src_clip    = src_clip_v,                                \
    scui_dd_i->image_scale.src_alpha   = src_alpha_v,                               \
    scui_dd_i->image_scale.src_scale   = src_scale_v,                               \
    scui_dd_i->image_scale.dst_offset  = dst_offset_v,                              \
    scui_dd_i->image_scale.src_offset  = src_offset_v,                              \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_image_rotate */
#define scui_draw_image_rotate(sync_v, dst_surface_v, dst_clip_v,                   \
    src_image_v, src_clip_v, src_alpha_v, src_angle_v,                              \
    src_anchor_v, src_center_v)                                                     \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_image_rotate;                                  \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->image_rotate.dst_surface = dst_surface_v,                            \
    scui_dd_i->image_rotate.dst_clip    = dst_clip_v,                               \
    scui_dd_i->image_rotate.src_image   = src_image_v,                              \
    scui_dd_i->image_rotate.src_clip    = src_clip_v,                               \
    scui_dd_i->image_rotate.src_alpha   = src_alpha_v,                              \
    scui_dd_i->image_rotate.src_angle   = src_angle_v,                              \
    scui_dd_i->image_rotate.src_anchor  = src_anchor_v,                             \
    scui_dd_i->image_rotate.src_center  = src_center_v,                             \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_image_matrix_blend */
#define scui_draw_image_matrix_blend(sync_v, dst_surface_v, dst_clip_v,             \
    src_image_v, src_clip_v, src_alpha_v, src_color_v,                              \
    inv_matrix_v, src_matrix_v)                                                     \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_image_matrix_blend;                            \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->image_matrix_blend.dst_surface = dst_surface_v,                      \
    scui_dd_i->image_matrix_blend.dst_clip    = dst_clip_v,                         \
    scui_dd_i->image_matrix_blend.src_image   = src_image_v,                        \
    scui_dd_i->image_matrix_blend.src_clip    = src_clip_v,                         \
    scui_dd_i->image_matrix_blend.src_alpha   = src_alpha_v,                        \
    scui_dd_i->image_matrix_blend.src_color   = src_color_v,                        \
    scui_dd_i->image_matrix_blend.inv_matrix  = inv_matrix_v,                       \
    scui_dd_i->image_matrix_blend.src_matrix  = src_matrix_v,                       \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \



/* scui_draw_type_ring */
#define scui_draw_ring(sync_v, dst_surface_v, dst_clip_v,                           \
    dst_center_v, src_image_e_v, src_image_v, src_clip_v,                           \
    src_angle_s_v, src_alpha_v, src_angle_e_v, src_color_v)                         \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_ring;                                          \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->ring.dst_surface = dst_surface_v,                                    \
    scui_dd_i->ring.dst_clip    = dst_clip_v,                                       \
    scui_dd_i->ring.dst_center  = dst_center_v,                                     \
    scui_dd_i->ring.src_image_e = src_image_e_v,                                    \
    scui_dd_i->ring.src_image   = src_image_v,                                      \
    scui_dd_i->ring.src_clip    = src_clip_v,                                       \
    scui_dd_i->ring.src_angle_s = src_angle_s_v,                                    \
    scui_dd_i->ring.src_alpha   = src_alpha_v,                                      \
    scui_dd_i->ring.src_angle_e = src_angle_e_v,                                    \
    scui_dd_i->ring.src_color   = src_color_v,                                      \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \



/* scui_draw_type_letter */
#define scui_draw_letter(sync_v, dst_surface_v, dst_clip_v,                         \
    src_glyph_v, src_clip_v, src_alpha_v, src_color_v)                              \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_letter;                                        \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->letter.dst_surface = dst_surface_v,                                  \
    scui_dd_i->letter.dst_clip    = dst_clip_v,                                     \
    scui_dd_i->letter.src_glyph   = src_glyph_v,                                    \
    scui_dd_i->letter.src_clip    = src_clip_v,                                     \
    scui_dd_i->letter.src_alpha   = src_alpha_v,                                    \
    scui_dd_i->letter.src_color   = src_color_v,                                    \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_string */
#define scui_draw_string(sync_v, dst_surface_v, dst_clip_v,                         \
    src_clip_v, src_alpha_v, src_args_v)                                            \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_string;                                        \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->string.dst_surface = dst_surface_v,                                  \
    scui_dd_i->string.dst_clip    = dst_clip_v,                                     \
    scui_dd_i->string.src_clip    = src_clip_v,                                     \
    scui_dd_i->string.src_alpha   = src_alpha_v,                                    \
    scui_dd_i->string.src_args    = src_args_v,                                     \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \



/* scui_draw_type_qrcode */
#define scui_draw_qrcode(sync_v, dst_surface_v, dst_clip_v,                         \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)                   \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_qrcode;                                        \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->qrcode.dst_surface = dst_surface_v,                                  \
    scui_dd_i->qrcode.dst_clip    = dst_clip_v,                                     \
    scui_dd_i->qrcode.src_clip    = src_clip_v,                                     \
    scui_dd_i->qrcode.src_alpha   = src_alpha_v,                                    \
    scui_dd_i->qrcode.src_color   = src_color_v,                                    \
    scui_dd_i->qrcode.src_size    = src_size_v,                                     \
    scui_dd_i->qrcode.src_data    = src_data_v,                                     \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_barcode */
#define scui_draw_barcode(sync_v, dst_surface_v, dst_clip_v,                        \
    src_clip_v, src_alpha_v, src_color_v, src_size_v, src_data_v)                   \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = scui_draw_type_barcode;                                       \
    scui_dd_i->sync = sync_v;                                                       \
    scui_dd_i->qrcode.dst_surface = dst_surface_v,                                  \
    scui_dd_i->qrcode.dst_clip    = dst_clip_v,                                     \
    scui_dd_i->qrcode.src_clip    = src_clip_v,                                     \
    scui_dd_i->qrcode.src_alpha   = src_alpha_v,                                    \
    scui_dd_i->qrcode.src_color   = src_color_v,                                    \
    scui_dd_i->qrcode.src_size    = src_size_v,                                     \
    scui_dd_i->qrcode.src_data    = src_data_v,                                     \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

/* scui_draw_type_graph */
#define scui_draw_graph(sync_v, dst_surface_v, dst_clip_v,                          \
    src_alpha_v, src_color_v, draw_dsc_v)                                           \
do {                                                                                \
    scui_draw_dsc_t *scui_dd_i = NULL;                                              \
    scui_draw_dsc_ready(&scui_dd_i);                                                \
    scui_dd_i->type = (draw_dsc_v)->type;                                           \
    scui_dd_i->sync =  sync_v;                                                      \
    scui_dd_i->graph.dst_surface = dst_surface_v;                                   \
    scui_dd_i->graph.dst_clip    = dst_clip_v;                                      \
    scui_dd_i->graph.src_alpha   = src_alpha_v;                                     \
    scui_dd_i->graph.src_color   = src_color_v;                                     \
                                                                                    \
    scui_dd_i->graph.src_area    = (draw_dsc_v)->graph.src_area;                    \
    scui_dd_i->graph.src_center  = (draw_dsc_v)->graph.src_center;                  \
    scui_dd_i->graph.src_angle_s = (draw_dsc_v)->graph.src_angle_s;                 \
    scui_dd_i->graph.src_angle_e = (draw_dsc_v)->graph.src_angle_e;                 \
    scui_dd_i->graph.src_width   = (draw_dsc_v)->graph.src_width;                   \
    scui_dd_i->graph.src_pos_1   = (draw_dsc_v)->graph.src_pos_1;                   \
    scui_dd_i->graph.src_pos_2   = (draw_dsc_v)->graph.src_pos_2;                   \
    scui_dd_i->graph.src_radius  = (draw_dsc_v)->graph.src_radius;                  \
    scui_dd_i->graph.src_shadow  = (draw_dsc_v)->graph.src_shadow;                  \
    /* ThorVG扩展字段: */                                                               \
    scui_dd_i->graph.src_tvg_cb     = (draw_dsc_v)->graph.src_tvg_cb;               \
    scui_dd_i->graph.src_tvg_round  = (draw_dsc_v)->graph.src_tvg_round;            \
    scui_dd_i->graph.src_tvg_gradw  = (draw_dsc_v)->graph.src_tvg_gradw;            \
    scui_dd_i->graph.src_tvg_grad   = (draw_dsc_v)->graph.src_tvg_grad;             \
    scui_draw_dsc_task(scui_dd_i);                                                  \
} while (0)                                                                         \

#endif
