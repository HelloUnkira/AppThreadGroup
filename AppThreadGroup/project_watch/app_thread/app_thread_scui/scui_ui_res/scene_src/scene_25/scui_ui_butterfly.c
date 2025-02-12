/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t image_antenna;        // 触角
    scui_handle_t image_trunk;          // 躯干
    scui_handle_t image_wing[2];        // 翅膀(左右镜像对称,前后共俩对)
    scui_handle_t image_wing_type;      // 翅膀风格(四种0123)
    scui_matrix_t matrix_antenna[2];    // 触角矩阵
    scui_matrix_t matrix_trunk[2];      // 躯干矩阵
    scui_matrix_t matrix_wing[4];       // 翅膀矩阵(左前;左后;右前;右后;)
    
    scui_coord_t  angle_limit;
    scui_coord_t  angle_wing;
    scui_coord_t  angle_way;
    
    scui_view3_t  view;                 // 视点移动
    
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_butterfly_event_proc(scui_event_t *event)
{
    scui_ui_scene_link_cfg(event);
    
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local == NULL);
            scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(*scui_ui_res_local));
            memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
        }
        
        /* 界面数据加载准备 */
        if (scui_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            scui_area_t clip = scui_widget_clip(event->object);
            /* 居中偏移 */
            scui_point3_t offset = {
                .x = clip.x + clip.w / 2,
                .y = clip.y + clip.h / 2,
            };
            
            /* 初始视点在屏幕中间 */
            scui_ui_res_local->view.x = +offset.x;
            scui_ui_res_local->view.y = +offset.y;
            scui_ui_res_local->view.z = -offset.x;
            
            scui_ui_res_local->angle_limit = 45;
            scui_ui_res_local->angle_way   = +5;
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        
        /* 界面数据转存回收 */
        if (scui_event_check_finish(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
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
void scui_ui_scene_butterfly_custom_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse:
        /* 这个事件可以视为本控件的全局刷新帧动画 */
        if (!scui_event_check_execute(event))
             break;
        
        if (scui_ui_res_local->angle_wing < -scui_ui_res_local->angle_limit ||
            scui_ui_res_local->angle_wing > +scui_ui_res_local->angle_limit)
            scui_ui_res_local->angle_way  = -scui_ui_res_local->angle_way;
        
        scui_ui_res_local->angle_wing += scui_ui_res_local->angle_way;
        
        // scui_ui_res_local->view.x += 25;
        // scui_ui_res_local->view.y += 25;
        // scui_ui_res_local->view.z += 25;
        
        scui_widget_draw(event->object, NULL, false);
        
        break;
    case scui_event_draw:
        
        /* 绘制流程准备 */
        if (scui_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            scui_ui_res_local->image_antenna = scui_image_prj_image_src_00_3d_Butterfly_Antennapng;
            scui_ui_res_local->image_trunk   = scui_image_prj_image_src_00_3d_Butterfly_BodyHalfpng;
            
            scui_ui_res_local->image_wing_type = 1;
            
            if (scui_ui_res_local->image_wing_type == 0) {
                scui_ui_res_local->image_wing[0] = scui_image_prj_image_src_00_3d_Butterfly_SwingUp0png;
                scui_ui_res_local->image_wing[1] = scui_image_prj_image_src_00_3d_Butterfly_SwingDown0png;
            }
            if (scui_ui_res_local->image_wing_type == 1) {
                scui_ui_res_local->image_wing[0] = scui_image_prj_image_src_00_3d_Butterfly_SwingUp1png;
                scui_ui_res_local->image_wing[1] = scui_image_prj_image_src_00_3d_Butterfly_SwingDown1png;
            }
            if (scui_ui_res_local->image_wing_type == 2) {
                scui_ui_res_local->image_wing[0] = scui_image_prj_image_src_00_3d_Butterfly_SwingUp2png;
                scui_ui_res_local->image_wing[1] = scui_image_prj_image_src_00_3d_Butterfly_SwingDown2png;
            }
            if (scui_ui_res_local->image_wing_type == 3) {
                scui_ui_res_local->image_wing[0] = scui_image_prj_image_src_00_3d_Butterfly_SwingUp3png;
                scui_ui_res_local->image_wing[1] = scui_image_prj_image_src_00_3d_Butterfly_SwingDown3png;
            }
            
            scui_view3_t *view = &scui_ui_res_local->view;
            
            scui_handle_t image_antenna = scui_ui_res_local->image_antenna;
            scui_handle_t image_trunk   = scui_ui_res_local->image_trunk;
            scui_handle_t image_wing0   = scui_ui_res_local->image_wing[0];
            scui_handle_t image_wing1   = scui_ui_res_local->image_wing[1];
            
            scui_face3_t face3_antenna = {
                .point3[0] = {.x = +0,                           .y = -scui_image_h(image_antenna),},
                .point3[1] = {.x = +scui_image_w(image_antenna), .y = -scui_image_h(image_antenna),},
                .point3[2] = {.x = +scui_image_w(image_antenna), .y = +0,},
                .point3[3] = {.x = +0,                           .y = +0,},
            };
            
            scui_face3_t face3_trunk = {
                .point3[0] = {.x = -scui_image_w(image_trunk), .y = -scui_image_h(image_trunk) / 2,},
                .point3[1] = {.x = +0,                         .y = -scui_image_h(image_trunk) / 2,},
                .point3[2] = {.x = +0,                         .y = +scui_image_h(image_trunk) / 2,},
                .point3[3] = {.x = -scui_image_w(image_trunk), .y = +scui_image_h(image_trunk) / 2,},
            };
            
            scui_face3_t face3_wing0 = {
                .point3[0] = {.x = +0,                         .y = -scui_image_h(image_wing0),},
                .point3[1] = {.x = +scui_image_w(image_wing0), .y = -scui_image_h(image_wing0),},
                .point3[2] = {.x = +scui_image_w(image_wing0), .y = +0,},
                .point3[3] = {.x = +0,                         .y = +0,},
            };
            
            scui_face3_t face3_wing1 = {
                .point3[0] = {.x = +0,                         .y = -0,},
                .point3[1] = {.x = +scui_image_w(image_wing1), .y = -0,},
                .point3[2] = {.x = +scui_image_w(image_wing1), .y = +scui_image_h(image_wing1),},
                .point3[3] = {.x = +0,                         .y = +scui_image_h(image_wing1),},
            };
            
            for (uint8_t idx = 0; idx < 4; idx++) {
                face3_antenna.point3[idx].y += -scui_image_h(image_trunk) / 2;
                
                face3_wing0.point3[idx].x += scui_image_w(image_trunk);
                face3_wing1.point3[idx].x += scui_image_w(image_trunk);
                
                face3_wing0.point3[idx].y += -30.0f;
                face3_wing1.point3[idx].y += -30.0f;
            }
            
            scui_face3_t face3_antenna_0 = face3_antenna;
            scui_face3_t face3_antenna_1 = face3_antenna;
            scui_face3_t face3_trunk_0 = face3_trunk;
            scui_face3_t face3_trunk_1 = face3_trunk;
            scui_face3_t face3_wing_0 = face3_wing0;
            scui_face3_t face3_wing_1 = face3_wing0;
            scui_face3_t face3_wing_2 = face3_wing1;
            scui_face3_t face3_wing_3 = face3_wing1;
            
            scui_matrix_t *matrix_antenna = scui_ui_res_local->matrix_antenna;
            scui_matrix_t *matrix_trunk   = scui_ui_res_local->matrix_trunk;
            scui_matrix_t *matrix_wing    = scui_ui_res_local->matrix_wing;
            
            scui_area_t clip = scui_widget_clip(event->object);
            /* 居中偏移 */
            scui_point3_t offset = {
                .x = clip.x + clip.w / 2,
                .y = clip.y + clip.h / 2,
            };
            
            /* 触角矩阵 */
            scui_matrix_identity(&matrix_antenna[0]);
            scui_area3_transform_by_matrix(&face3_antenna_0, &matrix_antenna[0]);
            scui_area3_offset(&face3_antenna_0, &offset);
            scui_size2_t size2_antenna_0 = {.w = scui_image_w(image_antenna),.h = scui_image_h(image_antenna),};
            scui_matrix_perspective_view_blit(&matrix_antenna[0], &size2_antenna_0, &face3_antenna_0, view);
            scui_matrix_inverse(&matrix_antenna[0]);
            
            scui_matrix_identity(&matrix_antenna[1]);
            scui_matrix_rotate(&matrix_antenna[1], 180.0f, 0x02);
            scui_area3_transform_by_matrix(&face3_antenna_1, &matrix_antenna[1]);
            scui_area3_offset(&face3_antenna_1, &offset);
            scui_size2_t size2_antenna_1 = {.w = scui_image_w(image_antenna),.h = scui_image_h(image_antenna),};
            scui_matrix_perspective_view_blit(&matrix_antenna[1], &size2_antenna_1, &face3_antenna_1, view);
            scui_matrix_inverse(&matrix_antenna[1]);
            
            /* 躯干矩阵 */
            scui_matrix_identity(&matrix_trunk[0]);
            scui_matrix_rotate(&matrix_trunk[0], 180.0f, 0x02);
            scui_area3_transform_by_matrix(&face3_trunk_0, &matrix_trunk[0]);
            scui_area3_offset(&face3_trunk_0, &offset);
            scui_size2_t size2_trunk_0 = {.w = scui_image_w(image_trunk),.h = scui_image_h(image_trunk),};
            scui_matrix_perspective_view_blit(&matrix_trunk[0], &size2_trunk_0, &face3_trunk_0, view);
            scui_matrix_inverse(&matrix_trunk[0]);
            
            scui_matrix_identity(&matrix_trunk[1]);
            scui_area3_transform_by_matrix(&face3_trunk_1, &matrix_trunk[1]);
            scui_area3_offset(&face3_trunk_1, &offset);
            scui_size2_t size2_trunk_1 = {.w = scui_image_w(image_trunk),.h = scui_image_h(image_trunk),};
            scui_matrix_perspective_view_blit(&matrix_trunk[1], &size2_trunk_1, &face3_trunk_1, view);
            scui_matrix_inverse(&matrix_trunk[1]);
            
            /* 翅膀矩阵(左上右上) */
            scui_matrix_identity(&matrix_wing[0]);
            scui_matrix_rotate(&matrix_wing[0], +scui_ui_res_local->angle_wing, 0x02);
            scui_area3_transform_by_matrix(&face3_wing_0, &matrix_wing[0]);
            scui_area3_offset(&face3_wing_0, &offset);
            scui_size2_t size2_wing_0 = {.w = scui_image_w(image_wing0),.h = scui_image_h(image_wing0),};
            scui_matrix_perspective_view_blit(&matrix_wing[0], &size2_wing_0, &face3_wing_0, view);
            scui_matrix_inverse(&matrix_wing[0]);
            
            scui_matrix_identity(&matrix_wing[1]);
            scui_matrix_rotate(&matrix_wing[1], 180.0f, 0x02);
            scui_matrix_rotate(&matrix_wing[1], -scui_ui_res_local->angle_wing, 0x02);
            scui_area3_transform_by_matrix(&face3_wing_1, &matrix_wing[1]);
            scui_area3_offset(&face3_wing_1, &offset);
            scui_size2_t size2_wing_1 = {.w = scui_image_w(image_wing0),.h = scui_image_h(image_wing0),};
            scui_matrix_perspective_view_blit(&matrix_wing[1], &size2_wing_1, &face3_wing_1, view);
            scui_matrix_inverse(&matrix_wing[1]);
            
            /* 翅膀矩阵(左下右下) */
            scui_matrix_identity(&matrix_wing[2]);
            scui_matrix_rotate(&matrix_wing[2], +scui_ui_res_local->angle_wing, 0x02);
            scui_area3_transform_by_matrix(&face3_wing_2, &matrix_wing[2]);
            scui_area3_offset(&face3_wing_2, &offset);
            scui_size2_t size2_wing_2 = {.w = scui_image_w(image_wing1),.h = scui_image_h(image_wing1),};
            scui_matrix_perspective_view_blit(&matrix_wing[2], &size2_wing_2, &face3_wing_2, view);
            scui_matrix_inverse(&matrix_wing[2]);
            
            scui_matrix_identity(&matrix_wing[3]);
            scui_matrix_rotate(&matrix_wing[3], 180.0f, 0x02);
            scui_matrix_rotate(&matrix_wing[3], -scui_ui_res_local->angle_wing, 0x02);
            scui_area3_transform_by_matrix(&face3_wing_3, &matrix_wing[3]);
            scui_area3_offset(&face3_wing_3, &offset);
            scui_size2_t size2_wing_3 = {.w = scui_image_w(image_wing1),.h = scui_image_h(image_wing1),};
            scui_matrix_perspective_view_blit(&matrix_wing[3], &size2_wing_3, &face3_wing_3, view);
            scui_matrix_inverse(&matrix_wing[3]);
        }
        
        /* 绘制流程进行 */
        if (scui_event_check_execute(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            /* 翅膀 */
            scui_handle_t *image_wing  = scui_ui_res_local->image_wing;
            scui_matrix_t *matrix_wing = scui_ui_res_local->matrix_wing;
            scui_widget_draw_image_matrix(event->object, NULL, image_wing[1], NULL, &matrix_wing[2]);
            scui_widget_draw_image_matrix(event->object, NULL, image_wing[1], NULL, &matrix_wing[3]);
            scui_widget_draw_image_matrix(event->object, NULL, image_wing[0], NULL, &matrix_wing[0]);
            scui_widget_draw_image_matrix(event->object, NULL, image_wing[0], NULL, &matrix_wing[1]);
            
            /* 触角 */
            scui_handle_t  image_antenna  = scui_ui_res_local->image_antenna;
            scui_matrix_t *matrix_antenna = scui_ui_res_local->matrix_antenna;
            scui_widget_draw_image_matrix(event->object, NULL, image_antenna, NULL, &matrix_antenna[0]);
            scui_widget_draw_image_matrix(event->object, NULL, image_antenna, NULL, &matrix_antenna[1]);
            
            /* 躯干 */
            scui_handle_t  image_trunk  = scui_ui_res_local->image_trunk;
            scui_matrix_t *matrix_trunk = scui_ui_res_local->matrix_trunk;
            scui_widget_draw_image_matrix(event->object, NULL, image_trunk, NULL, &matrix_trunk[0]);
            scui_widget_draw_image_matrix(event->object, NULL, image_trunk, NULL, &matrix_trunk[1]);
        }
        break;
    case scui_event_ptr_move:
        scui_event_mask_over(event);
        
        break;
    case scui_event_ptr_down:
    case scui_event_ptr_up:
        break;
    break;
    default:
        SCUI_LOG_DEBUG("event %u event->object %u", event->type, event->object);
        break;
    }
}
