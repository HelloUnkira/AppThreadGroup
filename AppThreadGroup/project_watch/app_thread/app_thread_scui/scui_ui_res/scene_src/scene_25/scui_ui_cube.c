/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_coord_t  size;         // 边长度
    scui_handle_t image[6];     // 面图标
    scui_matrix_t matrix[6];    // 仿射矩阵
    scui_point3_t rotate;       // 三轴偏移旋转
    scui_coord3_t normal_z[6];  // 面法线(0321,4567,1265,0473,2376,0154)
    bool          move_lock;    // 移动锁
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_cube_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        /* 界面数据加载准备 */
        
        scui_ui_res_local->image[0] = scui_image_prj_image_src_00_theme_icon_00_heart_09_08png;
        scui_ui_res_local->image[1] = scui_image_prj_image_src_00_theme_icon_01_spo2_09_08png;
        scui_ui_res_local->image[2] = scui_image_prj_image_src_00_theme_icon_02_message_09_08png;
        scui_ui_res_local->image[3] = scui_image_prj_image_src_00_theme_icon_04_call_09_08png;
        scui_ui_res_local->image[4] = scui_image_prj_image_src_00_theme_icon_05_sport_record_09_08png;
        scui_ui_res_local->image[5] = scui_image_prj_image_src_00_theme_icon_06_activity_09_08png;
        scui_ui_res_local->size = 102;
        
        scui_ui_res_local->rotate.x = 45.0f;
        scui_ui_res_local->rotate.y = 45.0f;
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
void scui_ui_scene_cube_custom_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        if (scui_ui_res_local->move_lock)
            break;
        
        static int32_t rotate_step = 0;
        static int16_t rotate_way  = 1;
        rotate_step += 1 * rotate_way;
        if (!scui_betw_lr(rotate_step, -360, +360))
             rotate_way = -rotate_way;
        
        static int32_t resize_step = 0;
        static int16_t resize_way  = 1;
        resize_step += 1 * resize_way;
        if (!scui_betw_lr(resize_step, 0, scui_ui_res_local->size / 2))
             resize_way = -resize_way;
        
        scui_ui_res_local->rotate.y += 1.0f * rotate_way;
        scui_ui_res_local->rotate.x -= 1.0f * rotate_way;
        scui_ui_res_local->size += 1 * resize_way;
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw:
        
        /* 绘制流程准备 */
        if (scui_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            /* 8个顶点 */
            scui_coord3_t size = scui_ui_res_local->size / 2;
            scui_vertex3_t vertex3_0 = {-1.0f * size, -1.0f * size, -1.0f * size,};
            scui_vertex3_t vertex3_1 = {+1.0f * size, -1.0f * size, -1.0f * size,};
            scui_vertex3_t vertex3_2 = {+1.0f * size, +1.0f * size, -1.0f * size,};
            scui_vertex3_t vertex3_3 = {-1.0f * size, +1.0f * size, -1.0f * size,};
            scui_vertex3_t vertex3_4 = {-1.0f * size, -1.0f * size, +1.0f * size,};
            scui_vertex3_t vertex3_5 = {+1.0f * size, -1.0f * size, +1.0f * size,};
            scui_vertex3_t vertex3_6 = {+1.0f * size, +1.0f * size, +1.0f * size,};
            scui_vertex3_t vertex3_7 = {-1.0f * size, +1.0f * size, +1.0f * size,};
            
            scui_face3_t face3[6] = {
                {.point3 = {vertex3_0, vertex3_3, vertex3_2, vertex3_1,},},
                {.point3 = {vertex3_4, vertex3_5, vertex3_6, vertex3_7,},},
                {.point3 = {vertex3_1, vertex3_2, vertex3_6, vertex3_5,},},
                {.point3 = {vertex3_0, vertex3_4, vertex3_7, vertex3_3,},},
                {.point3 = {vertex3_2, vertex3_3, vertex3_7, vertex3_6,},},
                {.point3 = {vertex3_0, vertex3_1, vertex3_5, vertex3_4,},},
            };
            
            scui_normal3_t normal3[6] = {
                {+0.0f, +0.0f, -1.0f},  // 0321
                {+0.0f, +0.0f, +1.0f},  // 4567
                {+1.0f, +0.0f, +0.0f},  // 1265
                {-1.0f, +0.0f, +0.0f},  // 0473
                {+0.0f, +1.0f, +0.0f},  // 2376
                {+0.0f, -1.0f, +0.0f},  // 0154
            };
            
            scui_area_t clip = scui_widget_clip(event->object);
            /* 居中偏移 */
            scui_point3_t offset = {
                .x = clip.x + clip.w / 2,
                .y = clip.y + clip.h / 2,
            };
            
            /* 三轴旋转矩阵 */
            scui_matrix_t r_matrix = {0};
            scui_point3_t angle = scui_ui_res_local->rotate;
            scui_matrix_rotate_a3(&r_matrix, &angle, 0x00);
            
            for (uint8_t idx = 0; idx < 6; idx++) {
                /* 旋转面,移动面 */
                scui_area3_transform_by_matrix(&face3[idx], &r_matrix);
                scui_area3_offset_xy(&face3[idx], &offset);
                /* 计算法线z轴 */
                scui_coord3_t *normal_z = scui_ui_res_local->normal_z;
                scui_mormal3_z_by_matrix(&normal3[idx], &normal_z[idx], &r_matrix);
                if (normal_z[idx] <= 0.0f)
                    continue;
                /* 仿射变换矩阵 */
                scui_matrix_t *matrix = scui_ui_res_local->matrix;
                
                scui_size2_t size2 = {
                    .w = scui_image_w(scui_ui_res_local->image[idx]),
                    .h = scui_image_h(scui_ui_res_local->image[idx]),
                };
                scui_matrix_affine_blit(&matrix[idx], &size2, &face3[idx]);
                scui_matrix_inverse(&matrix[idx]);
            }
        }
        
        /* 绘制流程进行 */
        if (scui_event_check_execute(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            for (uint8_t idx = 0; idx < 6; idx++) {
                
                scui_coord3_t *normal_z = scui_ui_res_local->normal_z;
                if (normal_z[idx] <= 0.0f)
                    continue;
                
                scui_handle_t *image  = scui_ui_res_local->image;
                scui_matrix_t *matrix = scui_ui_res_local->matrix;
                scui_widget_draw_image_matrix(event->object, NULL, image[idx], NULL, &matrix[idx]);
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
