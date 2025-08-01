/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_coord3_t sub_size;             // 足球六边形边长
    scui_coord3_t sub_hor_res;          // 足球六边形水平距离
    scui_coord3_t sub_ver_res;          // 足球六边形垂直距离
    scui_coord3_t angle_56;             // 正五边形与正六边形空间夹角
    scui_coord3_t angle_66;             // 正六边形与正六边形空间夹角
    scui_point2_t rotate;               // 移动量
    scui_point2_t offset_bg;            // 相对变换矩阵偏移量
    scui_point2_t offset_fg;            // 相对变换矩阵偏移量
    scui_handle_t image_bg;             // 图片
    scui_handle_t image_fg[4][5];       // 图片(从上至下:4层;每层5个)
    scui_matrix_t matrix_bg[4][5];      // 变换矩阵(从上至下:4层;每层5个)
    scui_matrix_t matrix_fg[4][5];      // 变换矩阵(从上至下:4层;每层5个)
    scui_coord3_t normal_z_bg[4][5];    // 面法线
    scui_coord3_t normal_z_fg[4][5];    // 面法线
    bool          move_lock;            // 移动锁
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_soccer_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        // 相邻两个六边形之间的夹角约为 138.190f
        // 五边形和六边形之间的夹角约为 142.623f
        scui_ui_res_local->angle_56 = 142.623f - 90.0f;
        scui_ui_res_local->angle_66 = 138.190f - 90.0f - (180.0f - 142.623f);
        
        scui_ui_res_local->image_bg = scui_image_prj_image_src_00_3d_Soccer_boardpng;
        for (uint8_t idx_j = 0; idx_j < 4; idx_j++)
        for (uint8_t idx_i = 0; idx_i < 5; idx_i++) {
            scui_handle_t image_fg_0 = scui_ui_res_local->image_bg - 3;
            scui_ui_res_local->image_fg[idx_j][idx_i] = image_fg_0 + idx_j * 5 + idx_i;
            if ((idx_j * 5 + idx_i) >= scui_image_prj_image_src_00_3d_Soccer_starpng - 2 - image_fg_0)
                scui_ui_res_local->image_fg[idx_j][idx_i] += 1;
            if ((idx_j * 5 + idx_i) >= scui_image_prj_image_src_00_3d_Soccer_backgroundpng - image_fg_0)
                scui_ui_res_local->image_fg[idx_j][idx_i] += 2;
        }
        
        scui_handle_t image_bg = scui_ui_res_local->image_bg;
        scui_handle_t image_fg = scui_ui_res_local->image_fg[0][0];
        scui_coord3_t image_bg_w = scui_image_w(image_bg);
        scui_coord3_t image_bg_h = scui_image_h(image_bg);
        scui_coord3_t image_fg_w = scui_image_w(image_fg);
        scui_coord3_t image_fg_h = scui_image_h(image_fg);
        SCUI_ASSERT(image_bg_h == image_bg_w);
        SCUI_ASSERT(image_fg_h == image_fg_w);
        
        /* 要求:背景图是的水平距离是正六边形的水平宽度 */
        scui_ui_res_local->sub_size = image_bg_w / 2;
        scui_ui_res_local->sub_hor_res = image_bg_w;
        scui_ui_res_local->sub_ver_res = image_bg_w * scui_cos(SCUI_RAD_BY_A(30));
        scui_ui_res_local->offset_bg.x = (0) / 2;
        scui_ui_res_local->offset_bg.y = (image_bg_h - scui_cos(SCUI_RAD_BY_A(30)) * image_bg_w) / 2;
        scui_ui_res_local->offset_fg.x = (image_bg_w - image_fg_w) / 2;
        scui_ui_res_local->offset_fg.y = (image_bg_h - image_fg_h) / 2;
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        scui_ui_scene_link_cfg(event);
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_key_click:
        break;
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_soccer_custom_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        if (scui_ui_res_local->move_lock)
            break;
        
        scui_ui_res_local->rotate.y += 1.5f;
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw:
        
        /* 绘制流程准备 */
        if (scui_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            scui_coord3_t angle_56 = scui_ui_res_local->angle_56;
            scui_coord3_t angle_66 = scui_ui_res_local->angle_66;
            
            scui_coord3_t sub_size = scui_ui_res_local->sub_size;
            scui_coord3_t sub_hor_res = scui_ui_res_local->sub_hor_res;
            scui_coord3_t sub_ver_res = scui_ui_res_local->sub_ver_res;
            
            scui_handle_t image_bg = scui_ui_res_local->image_bg;
            scui_handle_t image_fg = scui_ui_res_local->image_fg[0][0];
            scui_coord3_t image_bg_w = scui_image_w(image_bg);
            scui_coord3_t image_bg_h = scui_image_h(image_bg);
            scui_coord3_t image_fg_w = scui_image_w(image_fg);
            scui_coord3_t image_fg_h = scui_image_h(image_fg);
            scui_point2_t offset_bg  = scui_ui_res_local->offset_bg;
            scui_point2_t offset_fg  = scui_ui_res_local->offset_fg;
            
            scui_coord3_t dist_5 = scui_tan(SCUI_RAD_BY_A(36.0f)) * sub_size;
            scui_coord3_t sin_56 = scui_sin(SCUI_RAD_BY_A(angle_56));
            scui_coord3_t cos_56 = scui_cos(SCUI_RAD_BY_A(angle_56));
            scui_coord3_t sin_66 = scui_sin(SCUI_RAD_BY_A(angle_66));
            scui_coord3_t cos_66 = scui_cos(SCUI_RAD_BY_A(angle_66));
            
            scui_area_t clip = scui_widget_clip(event->object);
            /* 居中偏移 */
            scui_point3_t offset = {
                .x = clip.x + clip.w / 2,
                .y = clip.y + clip.h / 2,
            };
            
            scui_coord3_t angle_x[4] = {angle_56, angle_66, - angle_66, - angle_56,};
            scui_point3_t face3_bg_ofs[4] = {
                {.y = -cos_66 * sub_ver_res * 3 / 4 - cos_56 * sub_ver_res, .z = -dist_5},
                {.y = -cos_66 * sub_ver_res * 3 / 4,                        .z = -dist_5 - sin_56 * sub_ver_res,},
                {.y = -cos_66 * sub_ver_res * 1 / 4,                        .z = -dist_5 - sin_56 * sub_ver_res - sin_66 * sub_ver_res,},
                {.y = +cos_66 * sub_ver_res * 3 / 4,                        .z = -dist_5 - sin_56 * sub_ver_res,},
            };
            scui_point3_t face3_fg_ofs[4] = {0};
            for (uint8_t idx_k = 0; idx_k < 4; idx_k++) {
                face3_fg_ofs[idx_k] = face3_bg_ofs[idx_k];
                #if 0
                if (idx_k == 0 || idx_k == 3) {
                    face3_fg_ofs[idx_k].y += cos_56 * offset_fg.y;
                    face3_fg_ofs[idx_k].z += sin_56 * offset_fg.y;
                }
                if (idx_k == 1 || idx_k == 2) {
                    face3_fg_ofs[idx_k].y += cos_66 * offset_fg.y;
                    face3_fg_ofs[idx_k].z += sin_66 * offset_fg.y;
                }
                #endif
            }
            
            
            for (uint8_t idx_j = 0; idx_j < 4; idx_j++)
            for (uint8_t idx_i = 0; idx_i < 5; idx_i++) {
                
                /* 1.移动x轴向 */
                scui_face3_t face3_bg = {
                    .point3[0] = {.x = -image_bg_w / 2 - offset_bg.x, .y = -offset_bg.y,},
                    .point3[1] = {.x = +image_bg_w / 2 - offset_bg.x, .y = -offset_bg.y,},
                    .point3[2] = {.x = +image_bg_w / 2 - offset_bg.x, .y = -offset_bg.y + image_bg_h,},
                    .point3[3] = {.x = -image_bg_w / 2 - offset_bg.x, .y = -offset_bg.y + image_bg_h,},
                };
                scui_face3_t face3_fg = {
                    .point3[0] = {.x = -image_fg_w / 2, .y = -offset_bg.y + offset_fg.y,},
                    .point3[1] = {.x = +image_fg_w / 2, .y = -offset_bg.y + offset_fg.y,},
                    .point3[2] = {.x = +image_fg_w / 2, .y = -offset_bg.y + offset_fg.y + image_fg_h,},
                    .point3[3] = {.x = -image_fg_w / 2, .y = -offset_bg.y + offset_fg.y + image_fg_h,},
                };
                
                /* 2.进行x轴向旋转 */
                scui_matrix_t x_matrix = {0};
                scui_matrix_identity(&x_matrix);
                scui_matrix_rotate_a(&x_matrix, -(angle_x[idx_j]), 0x01);
                scui_area3_transform_by_matrix(&face3_bg, &x_matrix);
                scui_area3_transform_by_matrix(&face3_fg, &x_matrix);
                /* 3.移动y轴向和z轴向 */
                for (uint8_t idx_k = 0; idx_k < 4; idx_k++) {
                    face3_bg.point3[idx_k].y += face3_bg_ofs[idx_j].y;
                    face3_bg.point3[idx_k].z += face3_bg_ofs[idx_j].z;
                    face3_fg.point3[idx_k].y += face3_fg_ofs[idx_j].y;
                    face3_fg.point3[idx_k].z += face3_fg_ofs[idx_j].z;
                }
                /* 3.进行y轴向旋转 */
                scui_matrix_t y_matrix = {0};
                scui_matrix_identity(&y_matrix);
                scui_coord3_t rotate_y = scui_ui_res_local->rotate.y;
                scui_matrix_rotate_a(&y_matrix, +(idx_i * 72.0f + (idx_j < 2 ? 0.0f : 36.0f) + rotate_y), 0x02);
                scui_area3_transform_by_matrix(&face3_bg, &y_matrix);
                scui_area3_transform_by_matrix(&face3_fg, &y_matrix);
                /* 4.进行x轴向旋转 */
                scui_matrix_t rx_matrix = {0};
                scui_matrix_identity(&rx_matrix);
                scui_coord3_t rotate_x = scui_ui_res_local->rotate.x;
                scui_matrix_rotate_a(&rx_matrix, +(rotate_x), 0x01);
                scui_area3_transform_by_matrix(&face3_bg, &rx_matrix);
                scui_area3_transform_by_matrix(&face3_fg, &rx_matrix);
                /* 5.移动到中心点 */
                scui_area3_offset_xy(&face3_bg, &offset);
                scui_area3_offset_xy(&face3_fg, &offset);
                
                /* 计算法线z轴 */
                scui_matrix_t t_matrix = {0};
                scui_matrix_identity(&t_matrix);
                scui_matrix_multiply(&t_matrix, &x_matrix);
                scui_matrix_multiply(&t_matrix, &y_matrix);
                scui_matrix_multiply(&t_matrix, &rx_matrix);
                scui_normal3_t  normal3 = {0.0f, 0.0f, -1.0f};
                scui_coord3_t (*normal_z_bg)[5] = scui_ui_res_local->normal_z_bg;
                scui_coord3_t (*normal_z_fg)[5] = scui_ui_res_local->normal_z_fg;
                scui_mormal3_z_by_matrix(&normal3, &normal_z_bg[idx_j][idx_i], &t_matrix);
                scui_mormal3_z_by_matrix(&normal3, &normal_z_fg[idx_j][idx_i], &t_matrix);
                if (normal_z_bg[idx_j][idx_i] < -0.0f) {
                    /* 仿射变换矩阵 */
                    scui_matrix_t (*matrix_bg)[5] = scui_ui_res_local->matrix_bg;
                    scui_size2_t size2_bg = {.w = image_bg_w,.h = image_bg_h,};
                    scui_matrix_affine_blit(&matrix_bg[idx_j][idx_i], &size2_bg, &face3_bg);
                    scui_matrix_inverse(&matrix_bg[idx_j][idx_i]);
                }
                if (normal_z_fg[idx_j][idx_i] < -0.0f) {
                    scui_matrix_t (*matrix_fg)[5] = scui_ui_res_local->matrix_fg;
                    scui_size2_t size2_fg = {.w = image_fg_w,.h = image_fg_h,};
                    scui_matrix_affine_blit(&matrix_fg[idx_j][idx_i], &size2_fg, &face3_fg);
                    scui_matrix_inverse(&matrix_fg[idx_j][idx_i]);
                }
            }
        }
        
        /* 绘制流程进行 */
        if (scui_event_check_execute(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            for (uint8_t idx_j = 0; idx_j < 4; idx_j++)
            for (uint8_t idx_i = 0; idx_i < 5; idx_i++) {
                
                scui_coord3_t (*normal_z_bg)[5] = scui_ui_res_local->normal_z_bg;
                scui_coord3_t (*normal_z_fg)[5] = scui_ui_res_local->normal_z_fg;
                if (normal_z_bg[idx_j][idx_i] < -0.0f) {
                    scui_handle_t   image_bg = scui_ui_res_local->image_bg;
                    scui_matrix_t (*matrix_bg)[5] = scui_ui_res_local->matrix_bg;
                    scui_widget_draw_image_matrix(event->object, NULL, image_bg, NULL, &matrix_bg[idx_j][idx_i]);
                }
                if (normal_z_fg[idx_j][idx_i] < -0.0f) {
                    scui_handle_t (*image_fg)[5] = scui_ui_res_local->image_fg;
                    scui_matrix_t (*matrix_fg)[5] = scui_ui_res_local->matrix_fg;
                    scui_widget_draw_image_matrix(event->object, NULL, image_fg[idx_j][idx_i], NULL, &matrix_fg[idx_j][idx_i]);
                }
            }
        }
        break;
    case scui_event_ptr_move:
        scui_event_mask_over(event);
        
        /* y轴旋转是x的移动偏移量 */
        /* x轴旋转是y的移动偏移量 */
        scui_ui_res_local->move_lock = true;
        scui_ui_res_local->rotate.y += event->ptr_e.x - event->ptr_s.x;
        scui_ui_res_local->rotate.x += event->ptr_s.y - event->ptr_e.y;     // y轴方向是反的
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_ptr_down:
    case scui_event_ptr_up:
        scui_ui_res_local->move_lock = false;
        
        break;
    break;
    default:
        break;
    }
}
