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
    scui_handle_t num;
    scui_handle_t image[20];        // 面图标
    scui_matrix_t matrix[20];       // 投影矩阵
    scui_matrix_t matrix_inv[20];   // 投影矩阵
    scui_coord3_t center_z[20];     // 中心点z
    scui_coord_t  w_res;            // 水平宽度
    scui_coord_t  h_res;            // 垂直宽度
    scui_coord_t  x_span;           // 水平间隙
    scui_coord_t  scale_c;          // 整体缩放
    scui_coord_t  angle_a;          // 单位角度
    scui_coord_t  rotate_x;         // 旋转角度
    scui_coord_t  rotate_y;         // 旋转角度
    bool          move_lock;        // 移动锁
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_lantern_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        break;
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        
        /* 界面数据加载准备 */
        
        scui_ui_res_local->num = scui_arr_len(cwf_json_bin);
        scui_ui_res_local->angle_a = 360 / scui_ui_res_local->num;
        SCUI_ASSERT(360 % scui_ui_res_local->num == 0);
        
        //cwf json 测试:  读取云表盘预览图
        for (scui_handle_t idx = 0; idx < scui_ui_res_local->num; idx++) {
            scui_ui_res_local->image[idx] = SCUI_HANDLE_INVALID;
            scui_cwf_json_make_pv(&scui_ui_res_local->image[idx], cwf_json_bin[idx]);
        }
        
        scui_ui_res_local->w_res   = scui_image_w(scui_ui_res_local->image[0]);
        scui_ui_res_local->h_res   = scui_image_h(scui_ui_res_local->image[0]);
        scui_ui_res_local->x_span  = 0;
        scui_ui_res_local->scale_c = 1024;
        
        break;
    }
    case scui_event_destroy: {
        
        //cwf json 测试: 释放云表盘预览图
        for (scui_handle_t idx = 0; idx < scui_ui_res_local->num; idx++)
            scui_cwf_json_burn_pv(&scui_ui_res_local->image[idx]);
        
        break;
    }
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
void scui_ui_scene_lantern_custom_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        
        if (scui_ui_res_local->move_lock)
            break;
        
        static int32_t resize_step = 0;
        static int16_t resize_way  = 1;
        resize_step += 1 * resize_way;
        if (!scui_betw_lr(resize_step, -256, 256))
             resize_way = -resize_way;
        
        scui_ui_res_local->scale_c  += resize_way;
        scui_ui_res_local->rotate_x += 1.0f;
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw:
        
        /* 绘制流程准备 */
        if (scui_event_check_prepare(event)) {
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
                
                scui_matrix_t r_matrix = {0};
                scui_matrix_identity(&r_matrix);
                scui_point3_t rotate_3 = {
                    .y = scui_ui_res_local->rotate_x + idx * scui_ui_res_local->angle_a,
                    .x = scui_ui_res_local->rotate_y,
                };
                scui_matrix_rotate_a3(&r_matrix, &rotate_3, 0x05);
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
                
                scui_area3_transform_by_matrix(&face3, &r_matrix);
                scui_area3_offset(&face3, &offset);
                
                scui_area3_transform_by_matrix(&face3_inv, &r_matrix);
                scui_area3_offset(&face3_inv, &offset);
                
                /* 透视变换矩阵 */
                scui_matrix_t *matrix     = scui_ui_res_local->matrix;
                scui_matrix_t *matrix_inv = scui_ui_res_local->matrix_inv;
                scui_coord3_t *center_z   = scui_ui_res_local->center_z;
                
                /* 深度信息计算,后面排序处理 */
                scui_area3_center_z(&face3, &center_z[idx]);
                
                scui_handle_t image = scui_ui_res_local->image[idx];
                scui_size2_t size2 = {.w = scui_image_w(image),.h = scui_image_h(image),};
                scui_matrix_perspective_view_blit(&matrix[idx], &size2, &face3, &view3);
                scui_matrix_perspective_view_blit(&matrix_inv[idx], &size2, &face3_inv, &view3);
                
                scui_matrix_inverse(&matrix[idx]);
                scui_matrix_inverse(&matrix_inv[idx]);
            }
        }
        
        /* 绘制流程进行 */
        if (scui_event_check_execute(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            scui_widget_alpha_set(event->object, scui_alpha_cover, false);
            
            /* 根据center_z的深度信息进行排序决定绘制顺序 */
            scui_coord3_t *center_z = scui_ui_res_local->center_z;
            scui_coord3_t *draw_z   = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_coord3_t) * scui_ui_res_local->num);
            scui_coord_t  *draw_i   = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_coord_t)  * scui_ui_res_local->num);
            
            for (uint8_t idx = 0; idx < scui_ui_res_local->num; idx++) {
                draw_i[idx] = idx; draw_z[idx] = center_z[idx];
            }
            
            for (uint8_t idx_i = 0; idx_i < scui_ui_res_local->num; idx_i++)
            for (uint8_t idx_j = 0; idx_j < scui_ui_res_local->num; idx_j++)
                if (draw_z[idx_i] < draw_z[idx_j] && idx_i != idx_j) {
                    
                    scui_coord_t draw_i_t = draw_i[idx_i];
                    draw_i[idx_i] = draw_i[idx_j]; draw_i[idx_j] = draw_i_t;
                    scui_coord3_t draw_z_t = draw_z[idx_i];
                    draw_z[idx_i] = draw_z[idx_j]; draw_z[idx_j] = draw_z_t;
                }
            
            for (uint8_t idx = 0; idx < scui_ui_res_local->num; idx++) {
                
                scui_handle_t *image      = scui_ui_res_local->image;
                scui_matrix_t *matrix     = scui_ui_res_local->matrix;
                scui_matrix_t *matrix_inv = scui_ui_res_local->matrix_inv;
                
                scui_alpha_t alpha = scui_alpha_trans;
                scui_widget_alpha_get(event->object, &alpha);
                scui_widget_alpha_set(event->object, scui_alpha_pct50, false);
                scui_widget_draw_image_matrix(event->object, NULL, image[draw_i[idx]], NULL, &matrix_inv[draw_i[idx]]);
                scui_widget_alpha_set(event->object, alpha, false);
                
                scui_widget_draw_image_matrix(event->object, NULL, image[draw_i[idx]], NULL, &matrix[draw_i[idx]]);
            }
            
            SCUI_MEM_FREE(draw_z);
            SCUI_MEM_FREE(draw_i);
        }
        break;
    case scui_event_ptr_move:
        scui_event_mask_over(event);
        scui_coord_t ptr_dx = event->ptr_e.x - event->ptr_s.x;
        scui_coord_t ptr_dy = event->ptr_e.y - event->ptr_s.y;
        scui_ui_res_local->move_lock = true;
        scui_ui_res_local->rotate_x += ptr_dx;
        
        if (scui_betw_lr(scui_ui_res_local->rotate_y + ptr_dy, -15, 15))
            scui_ui_res_local->rotate_y += ptr_dy;
        
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
        break;
    }
}
