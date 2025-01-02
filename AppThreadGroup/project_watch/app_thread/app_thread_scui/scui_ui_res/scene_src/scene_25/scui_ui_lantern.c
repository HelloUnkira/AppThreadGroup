/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static const char * cwf_json_bin[] = {
    "D10597001.bin",
    "D10598001.bin",
    "D10599001.bin",
    "D10600001.bin",
    "D10601001.bin",
    "D10602001.bin",
    "D10603001.bin",
    "D10604001.bin",
};

static struct {
    scui_handle_t  num;
    scui_handle_t *image;           // 面图标
    scui_matrix_t *matrix;          // 投影矩阵
    scui_matrix_t *matrix_inv;      // 投影矩阵(倒影)
    scui_matrix_t *matrix_s;        // 投影矩阵
    scui_matrix_t *matrix_inv_s;    // 投影矩阵
    scui_handle_t *image_s;         // 面图标
    scui_coord_t   w_res;           // 水平宽度
    scui_coord_t   h_res;           // 垂直宽度
    scui_coord_t   x_span;          // 水平间隙
    scui_coord_t   scale_c;         // 整体缩放
    scui_coord_t   angle_a;         // 单位角度
    scui_coord_t   rotate_y;        // 旋转角度
    bool           move_lock;       // 移动锁
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_lantern_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
        }
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            scui_ui_res_local->num = scui_arr_len(cwf_json_bin);
            scui_handle_t num = scui_ui_res_local->num;
            scui_ui_res_local->image        = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_handle_t) * num);
            scui_ui_res_local->matrix       = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_matrix_t) * num);
            scui_ui_res_local->matrix_inv   = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_matrix_t) * num);
            scui_ui_res_local->matrix_s     = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_matrix_t) * num);
            scui_ui_res_local->matrix_inv_s = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_matrix_t) * num);
            scui_ui_res_local->image_s      = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_handle_t) * num);
            scui_ui_res_local->angle_a      = 360 / scui_ui_res_local->num;
            SCUI_ASSERT(360 % scui_ui_res_local->num == 0);
            
            
            
            for (scui_handle_t idx = 0; idx < scui_ui_res_local->num; idx++) {
                scui_ui_res_local->image[idx] = SCUI_HANDLE_INVALID;
                scui_cwf_json_make_pv(&scui_ui_res_local->image[idx], cwf_json_bin[idx]);
            }
            
            
            
            scui_ui_res_local->w_res   = scui_image_w(scui_ui_res_local->image[0]);
            scui_ui_res_local->h_res   = scui_image_h(scui_ui_res_local->image[0]);
            scui_ui_res_local->x_span  = 0;
            scui_ui_res_local->scale_c = 1024;
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        /* 界面数据转存回收 */
        if (scui_widget_event_check_finish(event)) {
            
            
            
            for (scui_handle_t idx = 0; idx < scui_ui_res_local->num; idx++)
                scui_cwf_json_burn_pv(&scui_ui_res_local->image[idx]);
            
            
            
            SCUI_ASSERT(scui_ui_res_local != NULL);
            SCUI_MEM_FREE(scui_ui_res_local->image_s);
            SCUI_MEM_FREE(scui_ui_res_local->matrix_inv_s);
            SCUI_MEM_FREE(scui_ui_res_local->matrix_s);
            SCUI_MEM_FREE(scui_ui_res_local->matrix_inv);
            SCUI_MEM_FREE(scui_ui_res_local->matrix);
            SCUI_MEM_FREE(scui_ui_res_local->image);
            SCUI_MEM_FREE(scui_ui_res_local);
            scui_ui_res_local = NULL;
        }
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        break;
    case scui_event_key_click:
        break;
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_lantern_custom_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_widget_event_check_execute(event))
             break;
        
        if (scui_ui_res_local->move_lock)
            break;
        
        static int32_t resize_step = 0;
        static int16_t resize_way  = 1;
        resize_step += 1 * resize_way;
        if (!scui_betw_lr(resize_step, -256, 256))
             resize_way = -resize_way;
        
        scui_ui_res_local->scale_c  += resize_way;
        scui_ui_res_local->rotate_y += 1.0f;
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw:
        
        /* 绘制流程准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            float tan_a = scui_tan(scui_radian_by_angle(scui_ui_res_local->angle_a / 2.0f));
            scui_coord3_t scale  = scui_ui_res_local->scale_c / 1024.0f;
            scui_coord3_t x_span = scui_ui_res_local->x_span;
            scui_coord3_t w_res  = scui_ui_res_local->w_res;
            scui_coord3_t h_res  = scui_ui_res_local->h_res;
            scui_coord3_t z_res  = (w_res + x_span * 2) / 2 / tan_a;
            
            scui_area_t clip = scui_widget_clip(event->object);
            scui_point3_t offset = {
                .x = clip.x + clip.w / 2,
                .y = clip.y + clip.h / 2 + clip.h / 3,
                .z = -8.0f * z_res,
            };
            
            scui_view3_t view3 = {
                .x = +offset.x,
                .z = -offset.z,
            };
            
            for (uint8_t idx = 0; idx < scui_ui_res_local->num; idx++) {
                
                scui_coord3_t rotate_y = scui_ui_res_local->rotate_y + idx * scui_ui_res_local->angle_a;
                scui_matrix_t r_matrix = {0};
                scui_matrix_identity(&r_matrix);
                scui_matrix_rotate(&r_matrix, rotate_y, 0x02);
                // scui_matrix_check(&r_matrix);
                
                scui_face3_t face3 = {
                    .point3[0] = {(-w_res / 2) * scale, (-h_res / 2) * scale, z_res * scale,},
                    .point3[1] = {(+w_res / 2) * scale, (-h_res / 2) * scale, z_res * scale,},
                    .point3[2] = {(+w_res / 2) * scale, (+h_res / 2) * scale, z_res * scale,},
                    .point3[3] = {(-w_res / 2) * scale, (+h_res / 2) * scale, z_res * scale,},
                };
                // 交换0-3,1-2以倒位面, 后y移动h
                scui_face3_t face3_inv = {
                    .point3[0] = face3.point3[3],
                    .point3[1] = face3.point3[2],
                    .point3[2] = face3.point3[1],
                    .point3[3] = face3.point3[0],
                };
                face3_inv.point3[0].y += h_res * scale;
                face3_inv.point3[1].y += h_res * scale;
                face3_inv.point3[2].y += h_res * scale;
                face3_inv.point3[3].y += h_res * scale;
                
                scui_point3_transform_by_matrix(&face3.point3[0], &r_matrix);
                scui_point3_transform_by_matrix(&face3.point3[1], &r_matrix);
                scui_point3_transform_by_matrix(&face3.point3[2], &r_matrix);
                scui_point3_transform_by_matrix(&face3.point3[3], &r_matrix);
                scui_point3_offset(&face3.point3[0], &offset);
                scui_point3_offset(&face3.point3[1], &offset);
                scui_point3_offset(&face3.point3[2], &offset);
                scui_point3_offset(&face3.point3[3], &offset);
                
                scui_point3_transform_by_matrix(&face3_inv.point3[0], &r_matrix);
                scui_point3_transform_by_matrix(&face3_inv.point3[1], &r_matrix);
                scui_point3_transform_by_matrix(&face3_inv.point3[2], &r_matrix);
                scui_point3_transform_by_matrix(&face3_inv.point3[3], &r_matrix);
                scui_point3_offset(&face3_inv.point3[0], &offset);
                scui_point3_offset(&face3_inv.point3[1], &offset);
                scui_point3_offset(&face3_inv.point3[2], &offset);
                scui_point3_offset(&face3_inv.point3[3], &offset);
                
                /* 透视变换矩阵 */
                scui_matrix_t *matrix = scui_ui_res_local->matrix;
                scui_matrix_t *matrix_inv = scui_ui_res_local->matrix_inv;
                scui_image_t *image = scui_handle_get(scui_ui_res_local->image[idx]);
                SCUI_ASSERT(image != NULL);
                
                scui_size2_t size2 = {.w = image->pixel.width,.h = image->pixel.height,};
                scui_matrix_perspective_view_blit(&matrix[idx], &size2, &face3, &view3);
                scui_matrix_perspective_view_blit(&matrix_inv[idx], &size2, &face3_inv, &view3);
            }
            
            /* 通过矩阵的y平移参数进行排序 */
            scui_handle_t *image        = scui_ui_res_local->image;
            scui_matrix_t *matrix       = scui_ui_res_local->matrix;
            scui_matrix_t *matrix_inv   = scui_ui_res_local->matrix_inv;
            scui_handle_t *image_s      = scui_ui_res_local->image_s;
            scui_matrix_t *matrix_s     = scui_ui_res_local->matrix_s;
            scui_matrix_t *matrix_inv_s = scui_ui_res_local->matrix_inv_s;
            
            for (uint8_t idx = 0; idx < scui_ui_res_local->num; idx++) {
                image_s[idx]      = image[idx];
                matrix_s[idx]     = matrix[idx];
                matrix_inv_s[idx] = matrix_inv[idx];
            }
            
            for (uint8_t idx_i = 0; idx_i < scui_ui_res_local->num; idx_i++)
            for (uint8_t idx_j = 0; idx_j < scui_ui_res_local->num; idx_j++)
                if (matrix_s[idx_i].meta[1][2] < matrix_s[idx_j].meta[1][2] && idx_i != idx_j) {
                    scui_handle_t image_i = image_s[idx_i];
                    scui_handle_t image_j = image_s[idx_j];
                    image_s[idx_i] = image_j;
                    image_s[idx_j] = image_i;
                    scui_matrix_t matrix_i = matrix_s[idx_i];
                    scui_matrix_t matrix_j = matrix_s[idx_j];
                    matrix_s[idx_i] = matrix_j;
                    matrix_s[idx_j] = matrix_i;
                    scui_matrix_t matrix_inv_i = matrix_inv_s[idx_i];
                    scui_matrix_t matrix_inv_j = matrix_inv_s[idx_j];
                    matrix_inv_s[idx_i] = matrix_inv_j;
                    matrix_inv_s[idx_j] = matrix_inv_i;
                }
            
            for (uint8_t idx = 0; idx < scui_ui_res_local->num; idx++) {
                // scui_matrix_check(&matrix_s[idx]);
                scui_matrix_inverse(&matrix_s[idx]);
                scui_matrix_inverse(&matrix_inv_s[idx]);
            }
        }
        
        /* 绘制流程进行 */
        if (scui_widget_event_check_execute(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            for (uint8_t idx = 0; idx < scui_ui_res_local->num; idx++) {
                
                scui_handle_t *image      = scui_ui_res_local->image_s;
                scui_matrix_t *matrix     = scui_ui_res_local->matrix_s;
                scui_matrix_t *matrix_inv = scui_ui_res_local->matrix_inv_s;
                scui_widget_draw_image_matrix(event->object, NULL, image[idx], NULL, &matrix[idx]);
                
                scui_alpha_t alpha = scui_widget_alpha_get(event->object);
                scui_widget_alpha_set(event->object, scui_alpha_pct50, false);
                scui_widget_draw_image_matrix(event->object, NULL, image[idx], NULL, &matrix_inv[idx]);
                scui_widget_alpha_set(event->object, alpha, false);
            }
        }
        break;
    case scui_event_ptr_move:
        scui_widget_event_mask_over(event);
        scui_ui_res_local->move_lock = true;
        scui_ui_res_local->rotate_y += event->ptr_e.x - event->ptr_s.x;
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_ptr_down:
    case scui_event_ptr_up:
        scui_ui_res_local->move_lock = false;
        break;
    case scui_event_key_click: {
        if (event->key_id != scui_event_key_val_enter)
            break;
        
        scui_window_switch_type_t *cfg_type = NULL;
        scui_window_switch_cfg_type(&cfg_type);
        scui_window_switch_type_t type = *cfg_type;
        *cfg_type = scui_window_switch_circle;
        scui_window_stack_del();
        *cfg_type = type;
        
        break;
    }
    break;
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}
