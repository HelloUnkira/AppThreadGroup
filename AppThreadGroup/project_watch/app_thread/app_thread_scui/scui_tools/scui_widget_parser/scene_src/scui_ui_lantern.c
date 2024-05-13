/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static inline void scui_ui_scene_window_float_cfg(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    /* 窗口属性参数配置 */
    scui_window_float_t float_cfg = {0};
    scui_window_float_cfg_get(&float_cfg);
    float_cfg.main = SCUI_UI_SCENE_LANTERN;
    float_cfg.list[0] = SCUI_HANDLE_INVALID;
    float_cfg.list[1] = SCUI_HANDLE_INVALID;
    float_cfg.list[2] = SCUI_HANDLE_INVALID;
    float_cfg.list[3] = SCUI_HANDLE_INVALID;
    scui_window_float_cfg_set(&float_cfg);
}

static inline void scui_ui_scene_window_list_cfg(scui_event_t *event)
{
    if (!scui_widget_event_check_prepare(event))
         return;
    
    /* 窗口属性参数配置 */
    scui_window_cfg_t window_cfg = {0};
    scui_window_cfg_get(SCUI_UI_SCENE_LANTERN, &window_cfg);
    window_cfg.sibling[0] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[1] = SCUI_UI_SCENE_2;
    window_cfg.sibling[2] = SCUI_HANDLE_INVALID;
    window_cfg.sibling[3] = SCUI_HANDLE_INVALID;
    scui_window_cfg_set(SCUI_UI_SCENE_LANTERN, &window_cfg);
}

static struct {
    scui_handle_t  num;
    scui_handle_t *image;       // 面图标
    scui_matrix_t *matrix;      // 投影矩阵
    scui_handle_t *image_s;     // 面图标
    scui_matrix_t *matrix_s;    // 投影矩阵
    scui_coord_t   w_res;       // 水平宽度
    scui_coord_t   h_res;       // 垂直宽度
    scui_coord_t   y_ofs;       // 垂直偏转
    scui_coord_t   x_span;      // 水平间隙
    scui_coord_t   scale_c;     // 整体缩放
    scui_coord_t   angle_a;     // 单位角度
    scui_coord_t   rotate_y;    // 旋转角度
    bool           move_lock;   // 移动锁
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_lantern_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        scui_widget_event_mask_keep(event);
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
        }
        
        /* 界面数据加载准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            scui_ui_res_local->num = 6;
            scui_handle_t num = scui_ui_res_local->num;
            scui_ui_res_local->image    = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_handle_t) * num);
            scui_ui_res_local->matrix   = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_matrix_t) * num);
            scui_ui_res_local->image_s  = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_handle_t) * num);
            scui_ui_res_local->matrix_s = SCUI_MEM_ALLOC(scui_mem_type_def, sizeof(scui_matrix_t) * num);
            scui_ui_res_local->angle_a  = 360 / scui_ui_res_local->num;
            SCUI_ASSERT(360 % scui_ui_res_local->num == 0);
            
            scui_ui_res_local->image[0] = scui_image_prj_image_src_home_watch_D10604001_preview_01_1bmp;
            scui_ui_res_local->image[1] = scui_image_prj_image_src_home_watch_D10597001_preview_01_1bmp;
            scui_ui_res_local->image[2] = scui_image_prj_image_src_home_watch_D10598001_preview_01_1bmp;
            scui_ui_res_local->image[3] = scui_image_prj_image_src_home_watch_D10599001_preview_01_1bmp;
            scui_ui_res_local->image[4] = scui_image_prj_image_src_home_watch_D10602001_preview_01_1bmp;
            scui_ui_res_local->image[5] = scui_image_prj_image_src_home_watch_D10603001_preview_01_1bmp;
            
            scui_ui_res_local->w_res = 382;
            scui_ui_res_local->h_res = 298;
            scui_ui_res_local->y_ofs = 100;
            scui_ui_res_local->x_span = 0;
            scui_ui_res_local->scale_c = 1024;
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        scui_widget_event_mask_keep(event);
        
        /* 界面数据转存回收 */
        if (scui_widget_event_check_finish(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            SCUI_MEM_FREE(scui_ui_res_local->matrix_s);
            SCUI_MEM_FREE(scui_ui_res_local->image_s);
            SCUI_MEM_FREE(scui_ui_res_local->matrix);
            SCUI_MEM_FREE(scui_ui_res_local->image);
            SCUI_MEM_FREE(scui_ui_res_local);
            scui_ui_res_local = NULL;
        }
        break;
    case scui_event_focus_get:
        scui_ui_scene_window_float_cfg(event);
        scui_ui_scene_window_list_cfg(event);
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_focus_lost:
        SCUI_LOG_INFO("scui_event_focus_lost");
        scui_widget_event_mask_keep(event);
        break;
    case scui_event_key_click:
        scui_widget_event_mask_keep(event);
        break;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_lantern_custom_event_proc(scui_event_t *event)
{
    scui_handle_t  handle = event->object;
    scui_widget_t *widget = scui_handle_get(handle);
    SCUI_ASSERT(widget != NULL);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        scui_widget_event_mask_keep(event);
        
        if (scui_ui_res_local->move_lock)
            break;
        
        static int32_t resize_step = 0;
        static int16_t resize_way  = 1;
        resize_step += 1 * resize_way;
        if (!scui_betw_lr(resize_step, -512, 512))
             resize_way = -resize_way;
        
        scui_ui_res_local->scale_c  += resize_way;
        scui_ui_res_local->rotate_y += 1.0f;
        scui_widget_draw(handle, NULL, false);
        break;
    case scui_event_draw:
        scui_widget_event_mask_keep(event);
        
        /* 绘制流程准备 */
        if (scui_widget_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            scui_coord3_t w_res   = scui_ui_res_local->w_res;
            scui_coord3_t h_res   = scui_ui_res_local->h_res;
            scui_coord3_t z_res   = w_res * scui_sin4096(scui_ui_res_local->angle_a) / 4096.0f;
            scui_coord3_t x_span  = scui_ui_res_local->x_span;
            scui_coord3_t scale_c = scui_ui_res_local->scale_c / 1024.0f;
            
            scui_point3_t offset = {
                .x = widget->clip.x + widget->clip.w / 2,
                .y = widget->clip.y + widget->clip.h / 2 + scui_ui_res_local->y_ofs,
                .z = -2.0f * z_res,
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
                    .point3[0] = {(-w_res / 2 + x_span) * scale_c, (-h_res / 2) * scale_c, z_res * scale_c,},
                    .point3[1] = {(+w_res / 2 - x_span) * scale_c, (-h_res / 2) * scale_c, z_res * scale_c,},
                    .point3[2] = {(+w_res / 2 - x_span) * scale_c, (+h_res / 2) * scale_c, z_res * scale_c,},
                    .point3[3] = {(-w_res / 2 + x_span) * scale_c, (+h_res / 2) * scale_c, z_res * scale_c,},
                };
                
                scui_point3_transform_by_matrix(&face3.point3[0], &r_matrix);
                scui_point3_transform_by_matrix(&face3.point3[1], &r_matrix);
                scui_point3_transform_by_matrix(&face3.point3[2], &r_matrix);
                scui_point3_transform_by_matrix(&face3.point3[3], &r_matrix);
                
                scui_point3_offset(&face3.point3[0], &offset);
                scui_point3_offset(&face3.point3[1], &offset);
                scui_point3_offset(&face3.point3[2], &offset);
                scui_point3_offset(&face3.point3[3], &offset);
                
                /* 透视变换矩阵 */
                scui_matrix_t *matrix = scui_ui_res_local->matrix;
                scui_image_t *image = scui_handle_get(scui_ui_res_local->image[idx]);
                SCUI_ASSERT(image != NULL);
                
                scui_size2_t size2 = {.w = image->pixel.width,.h = image->pixel.height,};
                scui_matrix_perspective_view_blit(&matrix[idx], &size2, &face3, &view3);
            }
            
            /* 通过矩阵的y平移参数进行排序 */
            scui_handle_t *image    = scui_ui_res_local->image;
            scui_matrix_t *matrix   = scui_ui_res_local->matrix;
            scui_handle_t *image_s  = scui_ui_res_local->image_s;
            scui_matrix_t *matrix_s = scui_ui_res_local->matrix_s;
            
            for (uint8_t idx = 0; idx < scui_ui_res_local->num; idx++) {
                image_s[idx]  = image[idx];
                matrix_s[idx] = matrix[idx];
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
                }
            
            for (uint8_t idx = 0; idx < scui_ui_res_local->num; idx++) {
                // scui_matrix_check(&matrix_s[idx]);
                scui_matrix_inverse(&matrix_s[idx]);
            }
        }
        
        /* 绘制流程进行 */
        if (scui_widget_event_check_execute(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            for (uint8_t idx = 0; idx < 6; idx++) {
                
                scui_handle_t *image  = scui_ui_res_local->image_s;
                scui_matrix_t *matrix = scui_ui_res_local->matrix_s;
                scui_widget_surface_draw_image_by_matrix(widget, image[idx], NULL, &matrix[idx]);
            }
        }
        break;
    case scui_event_ptr_move:
        scui_widget_event_mask_over(event);
        scui_ui_res_local->move_lock = true;
        scui_ui_res_local->rotate_y += event->ptr_e.x - event->ptr_s.x;
        scui_widget_draw(handle, NULL, false);
        break;
    case scui_event_ptr_down:
    case scui_event_ptr_up:
        scui_widget_event_mask_keep(event);
        scui_ui_res_local->move_lock = false;
        break;
    break;
    default:
        SCUI_LOG_DEBUG("event %u widget %u", event->type, event->object);
        break;
    }
}
