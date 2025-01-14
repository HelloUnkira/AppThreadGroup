#ifndef SCUI_DRAW_IMAGE_H
#define SCUI_DRAW_IMAGE_H

/* scui_draw_type_image */
#define scui_draw_image(dst_surface_v, dst_clip_v,                              \
    src_image_v, src_clip_v, src_alpha_v, src_color_v)                          \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_image,                                           \
        .image.dst_surface = dst_surface_v,                                     \
        .image.dst_clip    = dst_clip_v,                                        \
        .image.src_image   = src_image_v,                                       \
        .image.src_clip    = src_clip_v,                                        \
        .image.src_alpha   = src_alpha_v,                                       \
        .image.src_color   = src_color_v,                                       \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_image_scale */
#define scui_draw_image_scale(dst_surface_v, dst_clip_v,                        \
    src_image_v, src_clip_v, src_alpha_v, src_scale_v,                          \
    dst_offset_v, src_offset_v)                                                 \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_image_scale,                                     \
        .image_scale.dst_surface = dst_surface_v,                               \
        .image_scale.dst_clip    = dst_clip_v,                                  \
        .image_scale.src_image   = src_image_v,                                 \
        .image_scale.src_clip    = src_clip_v,                                  \
        .image_scale.src_alpha   = src_alpha_v,                                 \
        .image_scale.src_scale   = src_scale_v,                                 \
        .image_scale.dst_offset  = dst_offset_v,                                \
        .image_scale.src_offset  = src_offset_v,                                \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_image_rotate */
#define scui_draw_image_rotate(dst_surface_v, dst_clip_v,                       \
    src_image_v, src_clip_v, src_alpha_v, src_angle_v,                          \
    src_anchor_v, src_center_v)                                                 \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_image_rotate,                                    \
        .image_rotate.dst_surface = dst_surface_v,                              \
        .image_rotate.dst_clip    = dst_clip_v,                                 \
        .image_rotate.src_image   = src_image_v,                                \
        .image_rotate.src_clip    = src_clip_v,                                 \
        .image_rotate.src_alpha   = src_alpha_v,                                \
        .image_rotate.src_angle   = src_angle_v,                                \
        .image_rotate.src_anchor  = src_anchor_v,                               \
        .image_rotate.src_center  = src_center_v,                               \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

/* scui_draw_type_image_matrix_blend */
#define scui_draw_image_matrix_blend(dst_surface_v, dst_clip_v,                 \
    src_image_v, src_clip_v, src_alpha_v, src_color_v,                          \
    inv_matrix_v, src_matrix_v)                                                 \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_image_matrix_blend,                              \
        .image_matrix_blend.dst_surface = dst_surface_v,                        \
        .image_matrix_blend.dst_clip    = dst_clip_v,                           \
        .image_matrix_blend.src_image   = src_image_v,                          \
        .image_matrix_blend.src_clip    = src_clip_v,                           \
        .image_matrix_blend.src_alpha   = src_alpha_v,                          \
        .image_matrix_blend.src_color   = src_color_v,                          \
        .image_matrix_blend.inv_matrix  = inv_matrix_v,                         \
        .image_matrix_blend.src_matrix  = src_matrix_v,                         \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

#endif
