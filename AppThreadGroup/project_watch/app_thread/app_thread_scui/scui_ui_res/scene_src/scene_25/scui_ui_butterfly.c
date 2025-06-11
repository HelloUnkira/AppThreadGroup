/*实现目标:
 *    场景:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#define SCUI_UI_RES_LOCAL_NUM_IMAGE               4
#define SCUI_UI_RES_LOCAL_NUM_ELEMENT             10

static struct {
    scui_handle_t image_wing_type;      // 翅膀风格(四种0123)
    
    /* 图片组 */
    union {
    struct {
    scui_handle_t image_antenna;        // 触角
    scui_handle_t image_trunk;          // 躯干
    scui_handle_t image_wing[2];        // 翅膀(左右镜像对称,前后共俩对)
    };
    scui_handle_t image_list[SCUI_UI_RES_LOCAL_NUM_IMAGE];
    };
    
    /* 元素组 */
    union {
    struct {
    scui_matrix_t matrix_antenna[2];    // 触角矩阵
    scui_matrix_t matrix_trunk[4];      // 躯干矩阵
    scui_matrix_t matrix_wing[4];       // 翅膀矩阵(左前;左后;右前;右后;)
    };
    scui_matrix_t matrix_list[SCUI_UI_RES_LOCAL_NUM_ELEMENT];
    };
    scui_coord3_t center_z_list[SCUI_UI_RES_LOCAL_NUM_ELEMENT];
    
    /* 蝴蝶的跳动节拍 */
    scui_coord_t heart_peroid;
    scui_coord_t heart_count;
    scui_coord_t heart_way;
    
    /* 蝴蝶翅膀煽动 */
    scui_coord_t wing_limit;
    scui_coord_t wing_angle;
    
    /* 蝴蝶整体上下摆动 */
    scui_coord_t sway_limit;
    scui_coord_t sway_dist;
    
    /* 蝴蝶整体缩放 */
    scui_coord_t scale_max;     // 中心参考值: 1024
    scui_coord_t scale_min;     // 中心参考值: 1024
    scui_coord_t scale_var;
    scui_coord_t scale_way;
    
    /* 蝴蝶整体旋转 */
    scui_coord_t rotate_angle;
    
} * scui_ui_res_local = NULL;

/*@brief 更新图片使用参数
 */
static void scui_ui_scene_butterfly_image_update(void)
{
    scui_ui_res_local->image_antenna = scui_image_prj_image_src_00_3d_Butterfly_Antennapng;
    scui_ui_res_local->image_trunk   = scui_image_prj_image_src_00_3d_Butterfly_BodyHalfpng;

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
}

/*@brief 更新空间面布局
 */
static void scui_ui_scene_butterfly_face_ready(scui_face3_t face3_list[4])
{
    scui_size2_t size2 = {0};
    
    scui_handle_t image_antenna = scui_ui_res_local->image_antenna;
    scui_handle_t image_trunk   = scui_ui_res_local->image_trunk;
    scui_handle_t image_wing0   = scui_ui_res_local->image_wing[0];
    scui_handle_t image_wing1   = scui_ui_res_local->image_wing[1];
    
    #if 1
    // 空间建模为原点xy平面
    
    size2 = scui_image_s2(image_antenna);
    scui_face3_t face3_antenna = {
        .point3[0] = {.x = +0,       .y = -size2.h - 1,},
        .point3[1] = {.x = +size2.w, .y = -size2.h - 1,},
        .point3[2] = {.x = +size2.w, .y = +0 - 1,},
        .point3[3] = {.x = +0,       .y = +0 - 1,},
    };
    
    size2 = scui_image_s2(image_trunk);
    scui_face3_t face3_trunk = {
        .point3[0] = {.x = -size2.w + 1, .y = -size2.h / 2,},
        .point3[1] = {.x = +0 + 1,       .y = -size2.h / 2,},
        .point3[2] = {.x = +0 + 1,       .y = +size2.h / 2,},
        .point3[3] = {.x = -size2.w + 1, .y = +size2.h / 2,},
    };
    
    size2 = scui_image_s2(image_wing0);
    scui_face3_t face3_wing0 = {
        .point3[0] = {.x = +0 - 2,       .y = -size2.h + 2,},
        .point3[1] = {.x = +size2.w - 2, .y = -size2.h + 2,},
        .point3[2] = {.x = +size2.w - 2, .y = +0 + 2,},
        .point3[3] = {.x = +0 - 2,       .y = +0 + 2,},
    };
    
    size2 = scui_image_s2(image_wing1);
    scui_face3_t face3_wing1 = {
        .point3[0] = {.x = +0 - 2,       .y = -0 + 2,},
        .point3[1] = {.x = +size2.w - 2, .y = -0 + 2,},
        .point3[2] = {.x = +size2.w - 2, .y = +size2.h + 2,},
        .point3[3] = {.x = +0 - 2,       .y = +size2.h + 2,},
    };
    #endif
    
    for (uint8_t idx = 0; idx < 4; idx++) {
        face3_antenna.point3[idx].y += -scui_image_h(image_trunk) / 2;
        
        face3_wing0.point3[idx].x += scui_image_w(image_trunk);
        face3_wing1.point3[idx].x += scui_image_w(image_trunk);
        
        face3_wing0.point3[idx].y += -30.0f;
        face3_wing1.point3[idx].y += -30.0f;
    }
    
    /* 总共四个面, 为 antenna trunk wing0 wing1 */
    face3_list[0] = face3_antenna;
    face3_list[1] = face3_trunk;
    face3_list[2] = face3_wing0;
    face3_list[3] = face3_wing1;
}

/*@brief 更新空间面布局
 */
static void scui_ui_scene_butterfly_face_update(scui_face3_t *face3, uint8_t idx)
{
#if 1
    // 蝴蝶整体上下摆动
    scui_point3_t offset = {.y = scui_ui_res_local->sway_dist};
    scui_area3_offset(face3, &offset);
#endif
}

/*@brief 更新空间面布局
 */
static void scui_ui_scene_butterfly_face_matrix_update(void)
{
    /* 触角 *//* 躯干 *//* 翅膀 */
    scui_matrix_t *matrix_antenna = scui_ui_res_local->matrix_antenna;
    scui_matrix_t *matrix_trunk   = scui_ui_res_local->matrix_trunk;
    scui_matrix_t *matrix_wing    = scui_ui_res_local->matrix_wing;
    scui_matrix_t *matrix_list    = scui_ui_res_local->matrix_list;
    scui_handle_t  matrix_num     = SCUI_UI_RES_LOCAL_NUM_ELEMENT;
    
    #if 1   // 整体缩放和旋转
    for (uint8_t idx = 0; idx < matrix_num; idx++) {
        
        scui_point2_t scale = {
            .x = 1.0 + scui_ui_res_local->scale_var / 1024.0f,
            .y = 1.0 + scui_ui_res_local->scale_var / 1024.0f,
        };
        scui_matrix_scale(&matrix_list[idx], &scale);
        scui_matrix_rotate_a(&matrix_list[idx], scui_ui_res_local->rotate_angle, 0x02);
    }
    #endif
    
    #if 1
    // 变成左侧视图 <head, tail>
    scui_matrix_rotate_a(&matrix_trunk[0], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_trunk[1], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_trunk[2], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_trunk[3], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_trunk[0], -90.0f, 0x00);
    scui_matrix_rotate_a(&matrix_trunk[1], -90.0f, 0x00);
    scui_matrix_rotate_a(&matrix_trunk[2], -90.0f, 0x00);
    scui_matrix_rotate_a(&matrix_trunk[3], -90.0f, 0x00);
    
    scui_matrix_rotate_a(&matrix_antenna[0], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_antenna[1], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_antenna[0], -90.0f, 0x00);
    scui_matrix_rotate_a(&matrix_antenna[1], -90.0f, 0x00);
    
    scui_matrix_rotate_a(&matrix_antenna[0], -60.0f, 0x02);
    scui_matrix_rotate_a(&matrix_antenna[1], +60.0f, 0x02);
    
    scui_matrix_rotate_a(&matrix_wing[0], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_wing[1], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_wing[2], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_wing[3], 90.0f, 0x01);
    scui_matrix_rotate_a(&matrix_wing[0], -90.0f, 0x00);
    scui_matrix_rotate_a(&matrix_wing[1], -90.0f, 0x00);
    scui_matrix_rotate_a(&matrix_wing[2], -90.0f, 0x00);
    scui_matrix_rotate_a(&matrix_wing[3], -90.0f, 0x00);
    #endif
    
    // 蝴蝶煽动翅膀
    scui_matrix_rotate_a(&matrix_wing[0], +scui_ui_res_local->wing_angle, 0x02);
    scui_matrix_rotate_a(&matrix_wing[1], -scui_ui_res_local->wing_angle, 0x02);
    scui_matrix_rotate_a(&matrix_wing[2], +scui_ui_res_local->wing_angle, 0x02);
    scui_matrix_rotate_a(&matrix_wing[3], -scui_ui_res_local->wing_angle, 0x02);
    
    // 有一半矩阵是对称的
    scui_matrix_rotate_a(&matrix_antenna[1], 180.0f, 0x02);
    scui_matrix_rotate_a(&matrix_trunk[0], 180.0f, 0x02);
    scui_matrix_rotate_a(&matrix_trunk[2], 270.0f, 0x02);
    scui_matrix_rotate_a(&matrix_trunk[3], 90.0f, 0x02);
    scui_matrix_rotate_a(&matrix_wing[1], 180.0f, 0x02);
    scui_matrix_rotate_a(&matrix_wing[3], 180.0f, 0x02);
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_butterfly_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_local_res:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        break;
    case scui_event_anima_elapse:
        break;
    case scui_event_show:
        SCUI_LOG_INFO("scui_event_show");
        
        /* 界面数据加载准备 */
        if (scui_event_check_prepare(event)) {
            
            scui_ui_res_local->image_wing_type = 1;
            scui_ui_scene_butterfly_image_update();
            
            /* 蝴蝶的跳动节拍 */
            scui_ui_res_local->heart_peroid = 10;
            scui_ui_res_local->heart_way    = +1;
            
            /* 蝴蝶翅膀煽动 */
            scui_ui_res_local->wing_limit = 45;
            /* 蝴蝶整体上下摆动 */
            scui_ui_res_local->sway_limit = 45;
            /* 蝴蝶整体缩放 */
            scui_ui_res_local->scale_max = +256;
            scui_ui_res_local->scale_min = -768;
            scui_ui_res_local->scale_way = -1;
        }
        break;
    case scui_event_hide:
        SCUI_LOG_INFO("scui_event_hide");
        break;
    case scui_event_focus_get:
        SCUI_LOG_INFO("scui_event_focus_get");
        scui_ui_scene_link_cfg(event);
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
        
        /* 蝴蝶的跳动节拍 */
        if (scui_ui_res_local->heart_count < -scui_ui_res_local->heart_peroid ||
            scui_ui_res_local->heart_count > +scui_ui_res_local->heart_peroid)
            scui_ui_res_local->heart_way   = -scui_ui_res_local->heart_way;
            scui_ui_res_local->heart_count += scui_ui_res_local->heart_way;
        
        /* 蝴蝶翅膀煽动 */
        scui_ui_res_local->wing_angle = scui_map_ease_in_out(
             scui_ui_res_local->heart_count,
            -scui_ui_res_local->heart_peroid,
            +scui_ui_res_local->heart_peroid,
            -scui_ui_res_local->wing_limit,
            +scui_ui_res_local->wing_limit);
        
        /* 蝴蝶整体上下摆动 */
        scui_ui_res_local->sway_dist = scui_map_ease_in_out(
             scui_ui_res_local->heart_count,
            -scui_ui_res_local->heart_peroid,
            +scui_ui_res_local->heart_peroid,
            -scui_ui_res_local->sway_limit,
            +scui_ui_res_local->sway_limit);
        
        
        
        /* 蝴蝶整体缩放 */
        if (scui_ui_res_local->scale_var <  scui_ui_res_local->scale_min ||
            scui_ui_res_local->scale_var >  scui_ui_res_local->scale_max)
            scui_ui_res_local->scale_way = -scui_ui_res_local->scale_way;
            scui_ui_res_local->scale_var += scui_ui_res_local->scale_way;
        
        /* 蝴蝶整体旋转 */
        scui_ui_res_local->rotate_angle++;
        
        scui_widget_draw(event->object, NULL, false);
        break;
    case scui_event_draw:
        
        /* 绘制流程准备 */
        if (scui_event_check_prepare(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            scui_handle_t *image_list    = scui_ui_res_local->image_list;
            scui_matrix_t *matrix_list   = scui_ui_res_local->matrix_list;
            scui_handle_t  matrix_num    = SCUI_UI_RES_LOCAL_NUM_ELEMENT;
            scui_coord3_t *center_z_list = scui_ui_res_local->center_z_list;
            
            // 测试时，背景填白
            // scui_widget_color_set(event->object, SCUI_COLOR_MAKE32(false, 0x0, 0xFFFFFFFF));
            
            /* 居中偏移 */
            scui_area_t clip = scui_widget_clip(event->object);
            scui_point3_t offset = {
                .x = clip.x + clip.w / 2,
                .y = clip.y + clip.h / 2,
            };
            
            /* 视点在屏幕中间 */
            scui_view3_t view = {
                .x = +offset.x,
                .y = +offset.y,
                .z = -offset.x,
            };
            
            /* 面初始化 */
            scui_face3_t face3_list4[4] = {0};
            scui_ui_scene_butterfly_face_ready(face3_list4);
            
            /* 面列表 */
            scui_face3_t list_face3[] = {
                face3_list4[0], face3_list4[0],
                face3_list4[1], face3_list4[1], face3_list4[1], face3_list4[1],
                face3_list4[2], face3_list4[2], face3_list4[3], face3_list4[3],
            };
            /* 图列表 */
            scui_handle_t list_image[] = {
                image_list[0], image_list[0],
                image_list[1], image_list[1], image_list[1], image_list[1],
                image_list[2], image_list[2], image_list[3], image_list[3],
            };
            
            // 矩阵重置
            for (uint8_t idx = 0; idx < matrix_num; idx++)
                scui_matrix_identity(&matrix_list[idx]);
            
            // 矩阵变换元素
            scui_ui_scene_butterfly_face_matrix_update();
            
            // 矩阵变换流程
            for (uint8_t idx = 0; idx < matrix_num; idx++) {
                
                scui_face3_t face3 = list_face3[idx];
                scui_size2_t size2 = scui_image_s2(list_image[idx]);
                scui_area3_transform_by_matrix(&face3, &matrix_list[idx]);
                scui_area3_offset(&face3, &offset);
                // 面处理最后: 对面进行最后一次更新
                scui_ui_scene_butterfly_face_update(&face3, idx);
                
                scui_area3_center_z(&face3, &center_z_list[idx]);
                scui_matrix_perspective_view_blit(&matrix_list[idx], &size2, &face3, &view);
                scui_matrix_inverse(&matrix_list[idx]);
            }
        }
        
        /* 绘制流程进行 */
        if (scui_event_check_execute(event)) {
            SCUI_ASSERT(scui_ui_res_local != NULL);
            
            scui_handle_t *image_list    = scui_ui_res_local->image_list;
            scui_matrix_t *matrix_list   = scui_ui_res_local->matrix_list;
            scui_handle_t  matrix_num    = SCUI_UI_RES_LOCAL_NUM_ELEMENT;
            scui_coord3_t *center_z_list = scui_ui_res_local->center_z_list;
            
            /* 图列表 */
            scui_handle_t list_image[] = {
                image_list[0], image_list[0],
                image_list[1], image_list[1], image_list[1], image_list[1],
                image_list[2], image_list[2], image_list[3], image_list[3],
            };
            
            /* 根据center_z的深度信息进行排序决定绘制顺序 */
            scui_coord3_t draw_z[SCUI_UI_RES_LOCAL_NUM_ELEMENT] = {0};
            scui_coord_t  draw_i[SCUI_UI_RES_LOCAL_NUM_ELEMENT] = {0};
            
            for (uint8_t idx = 0; idx < matrix_num; idx++) {
                draw_i[idx] = idx; draw_z[idx] = center_z_list[idx];
            }
            
            for (uint8_t idx_i = 0; idx_i < matrix_num; idx_i++)
            for (uint8_t idx_j = 0; idx_j < matrix_num; idx_j++)
                if (draw_z[idx_i] > draw_z[idx_j] && idx_i != idx_j) {
                    
                    scui_coord_t draw_i_t = draw_i[idx_i];
                    draw_i[idx_i] = draw_i[idx_j]; draw_i[idx_j] = draw_i_t;
                    scui_coord3_t draw_z_t = draw_z[idx_i];
                    draw_z[idx_i] = draw_z[idx_j]; draw_z[idx_j] = draw_z_t;
                }
            
            for (uint8_t idx = 0; idx < matrix_num; idx++)
                scui_widget_draw_image_matrix(event->object, NULL, list_image[draw_i[idx]],
                    NULL, &matrix_list[draw_i[idx]]);
            
        }
        break;
    case scui_event_ptr_move:
        scui_event_mask_over(event);
        
        break;
    case scui_event_ptr_click:
        scui_event_mask_over(event);
        
        scui_ui_res_local->image_wing_type += 1;
        if (scui_ui_res_local->image_wing_type > 3)
            scui_ui_res_local->image_wing_type = 0;
        
        scui_ui_scene_butterfly_image_update();
        
        scui_widget_draw(event->object, NULL, false);
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
