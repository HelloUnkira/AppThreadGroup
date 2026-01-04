#ifndef SCUI_DRAW_RING_H
#define SCUI_DRAW_RING_H

/* scui_draw_type_ring */
#define scui_draw_ring(dst_surface_v, dst_clip_v,                               \
    dst_center_v, src_image_e_v, src_image_v, src_clip_v,                       \
    src_angle_s_v, src_alpha_v, src_angle_e_v, src_color_v)                     \
do {                                                                            \
    scui_draw_dsc_t draw_dsc = {                                                \
        .type = scui_draw_type_ring,                                            \
        .dst_surface = dst_surface_v,                                           \
        .dst_clip    = dst_clip_v,                                              \
        .dst_center  = dst_center_v,                                            \
        .src_image_e = src_image_e_v,                                           \
        .src_image   = src_image_v,                                             \
        .src_clip    = src_clip_v,                                              \
        .src_angle_s = src_angle_s_v,                                           \
        .src_alpha   = src_alpha_v,                                             \
        .src_angle_e = src_angle_e_v,                                           \
        .src_color   = src_color_v,                                             \
    };                                                                          \
    scui_draw_ctx(&draw_dsc);                                                   \
} while (0)                                                                     \

#endif
