/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_handle_t image[6];             // 面图标(固定显示6个, 末尾自动迭代)
    scui_matrix_t matrix[6];            // 投影矩阵
    scui_matrix_t inv_matrix[6];        // 投影矩阵
    scui_matrix_t matrix_fake[6];       // 投影矩阵
    scui_matrix_t inv_matrix_fake[6];   // 投影矩阵
    scui_coord3_t center_z[6];          // 中心点z
    scui_coord3_t face2[6][4][2];       // 面投影四边形(屏幕坐标, 点击命中)
    scui_coord_t  w_res;                // 水平宽度
    scui_coord_t  h_res;                // 垂直宽度
    scui_coord_t  x_span;               // 水平间隙
    scui_coord_t  scale_c;              // 整体缩放
    scui_coord_t  scale_breath;        // 呼吸缩放步进
    scui_coord_t  scale_way;           // 呼吸缩放方向(±1)
    scui_coord_t  angle_a;              // 单位角度
    scui_coord_t  rotate_x;             // 旋转角度
    scui_coord_t  rotate_y;             // 旋转角度
    bool          move_lock;            // 移动锁
    
    /* 表盘预览窗口(性能: 固定显示6个, 每面转到背面时迭代到下一个表盘) */
    scui_coord_t  dial_num;             // 表盘总数
    scui_coord_t  dial_show;            // 显示数量(固定6)
    scui_coord_t  dial_ofs[6];          // 各面表盘索引(背面迭代+6)
    scui_coord_t  dial_cross[6];        // 各面转过背面次数(换图节流)
    
    /* 点击动画参数(create配置, 参考soccer) */
    scui_coord_t  pick_rotate;          // 旋转动画时长(ms)
    scui_coord_t  pick_scale;           // 放大动画时长(ms)
    scui_coord3_t pick_scale_max;       // 放大目标倍数(面铺满屏幕)
    scui_coord_t  click_slop;           // 点击位移阈值(px)
    
    /* 点击动画状态 */
    scui_coord_t  pick_way;             // 状态机(0=idle,1=rotate,2=scale)
    scui_coord_t  pick_anima;           // 动画计时
    scui_coord3_t scale_cur;            // 整体缩放(1.0为基准)
    scui_coord_t  pick_idx;             // 选中面
    scui_point_t  ptr_down_pt;          // 按下落点
    bool          ptr_dragged;          // 是否拖动
} * scui_ui_res_local = NULL;

/*@brief 加载指定面的表盘预览图(make_pv内部先释放旧图)
 *@param idx 面序号
 */
static void lantern_pv_load(scui_coord_t idx)
{
    scui_ui_res_local->image[idx] = SCUI_HANDLE_INVALID;
    scui_cwf_json_make_pv(&scui_ui_res_local->image[idx],
        scui_presenter.cwf_dial_name(scui_ui_res_local->dial_ofs[idx]));
}

/*@brief 点在凸四边形内判定(叉积同号)
 *@param pt    点
 *@param quad  四边形(4点)
 *@retval 是否在内
 */
static bool lantern_point_in_quad(scui_point_t *pt, scui_coord3_t (*quad)[2])
{
    for (scui_coord_t k = 0; k < 4; k++) {
        scui_coord3_t ax = quad[k][0] - pt->x;
        scui_coord3_t ay = quad[k][1] - pt->y;
        scui_coord3_t bx = quad[(k + 1) % 4][0] - pt->x;
        scui_coord3_t by = quad[(k + 1) % 4][1] - pt->y;
        scui_coord3_t cross = ax * by - ay * bx;
        if (cross < 0.0f)
            return false;
    }
    return true;
}

/*@brief 窗口推进: 各面转到背面(完全朝后)时迭代到下一个表盘
 *       内容绑定面, 仅背面换图 => 无同帧跳变
 */
static void lantern_window_advance(void)
{
    for (scui_coord_t idx = 0; idx < scui_ui_res_local->dial_show; idx++) {
        scui_coord3_t ang = scui_ui_res_local->rotate_x + idx * scui_ui_res_local->angle_a;
        scui_coord3_t cross = (ang - 180.0f) / 360.0f;
        scui_coord_t cross_i = (scui_coord_t)floorf(cross);
        if (cross_i > scui_ui_res_local->dial_cross[idx]) {
            scui_ui_res_local->dial_cross[idx] = cross_i;
            scui_ui_res_local->dial_ofs[idx] =
                (scui_ui_res_local->dial_ofs[idx] + scui_ui_res_local->dial_show) % scui_ui_res_local->dial_num;
            lantern_pv_load(idx);
        }
    }
}

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
        
        /* 表盘总数与显示数量(性能: 固定显示6个, 末尾自动迭代) */
        scui_ui_res_local->dial_num  = scui_presenter.cwf_dial_num();
        scui_ui_res_local->dial_show = scui_ui_res_local->dial_num < 6 ? scui_ui_res_local->dial_num : 6;
        SCUI_ASSERT(360 % scui_ui_res_local->dial_show == 0);
        scui_ui_res_local->angle_a = 360 / scui_ui_res_local->dial_show;
        /* 各面表盘索引初始为0..5; 背面迭代计数按当前角度预置(避免开局即换) */
        for (scui_coord_t idx = 0; idx < scui_ui_res_local->dial_show; idx++) {
            scui_ui_res_local->dial_ofs[idx] = idx;
            scui_coord3_t ang = idx * scui_ui_res_local->angle_a;
            scui_ui_res_local->dial_cross[idx] = (scui_coord_t)floorf((ang - 180.0f) / 360.0f);
        }
        
        /* 点击动画参数(可配置) */
        scui_ui_res_local->pick_rotate    = 900;
        scui_ui_res_local->pick_scale     = 800;
        scui_ui_res_local->pick_scale_max = 6.0f;
        scui_ui_res_local->click_slop     = 12;
        
        /* 初始状态 */
        scui_ui_res_local->scale_cur  = 1.0f;
        scui_ui_res_local->pick_way   = 0;
        scui_ui_res_local->pick_anima = 0;
        scui_ui_res_local->ptr_dragged = false;
        
        //cwf json 测试:  读取云表盘预览图(窗口起始6个)
        for (scui_multi_t idx = 0; idx < scui_ui_res_local->dial_show; idx++)
            lantern_pv_load(idx);
        
        scui_ui_res_local->w_res   = scui_image_w(scui_ui_res_local->image[0]);
        scui_ui_res_local->h_res   = scui_image_h(scui_ui_res_local->image[0]);
        scui_ui_res_local->x_span  = 0;
        scui_ui_res_local->scale_c = 1024;
        scui_ui_res_local->scale_breath = 0;
        scui_ui_res_local->scale_way    = 1;
        
        break;
    }
    case scui_event_destroy: {
        
        //cwf json 测试: 释放云表盘预览图
        for (scui_multi_t idx = 0; idx < scui_ui_res_local->dial_show; idx++)
            scui_cwf_json_burn_pv(&scui_ui_res_local->image[idx]);
        
        break;
    }
    case scui_event_focus_get:
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
        
        /* 点击动画: 选中面旋转到中心正对 */
        if (scui_ui_res_local->pick_way == 1) {
            
            scui_ui_res_local->pick_anima += event->tick;
            scui_coord_t t = scui_map_linear(scui_ui_res_local->pick_anima,
                0, scui_ui_res_local->pick_rotate, 0, 100);
            scui_coord_t pct = scui_map_ease_out(t, 0, 100, 0, 100);
            scui_coord3_t dst_x = -(scui_ui_res_local->pick_idx * scui_ui_res_local->angle_a);
            /* 规范化: 取与当前旋转角最近的等价角(避免绕大圈) */
            scui_coord3_t src_x = scui_ui_res_local->rotate_x;
            while (dst_x - src_x >  180.0f) dst_x -= 360.0f;
            while (dst_x - src_x < -180.0f) dst_x += 360.0f;
            scui_ui_res_local->rotate_x = src_x + (dst_x - src_x) * pct / 100;
            scui_ui_res_local->rotate_y = 0.0f + (0.0f - scui_ui_res_local->rotate_y) * pct / 100;
            
            if (scui_ui_res_local->pick_anima >= scui_ui_res_local->pick_rotate) {
                scui_ui_res_local->rotate_x = dst_x;
                scui_ui_res_local->rotate_y = 0;
                /* 放大目标: 选中面铺满屏幕(不超出), 按当前正对投影计算 */
                scui_coord3_t (*face2)[2] = scui_ui_res_local->face2[scui_ui_res_local->pick_idx];
                scui_coord3_t fw = face2[1][0] - face2[0][0];
                scui_coord3_t fh = face2[3][1] - face2[0][1];
                if (fw < 1.0f) fw = 1.0f;
                if (fh < 1.0f) fh = 1.0f;
                scui_area_t clip = scui_widget_clip(event->object);
                scui_coord3_t sx = clip.w / fw;
                scui_coord3_t sy = clip.h / fh;
                scui_coord3_t smax = (sx < sy ? sx : sy) * 0.92f;
                if (smax < 1.0f) smax = 1.0f;
                scui_ui_res_local->pick_scale_max = smax;
                /* 进入放大阶段 */
                scui_ui_res_local->pick_way   = 2;
                scui_ui_res_local->pick_anima = 0;
            }
            scui_widget_draw(event->object, NULL, false, 0);
            break;
        }
        /* 点击动画: 整体放大到面铺满屏幕 */
        if (scui_ui_res_local->pick_way == 2) {
            
            scui_ui_res_local->pick_anima += event->tick;
            scui_coord_t t = scui_map_linear(scui_ui_res_local->pick_anima,
                0, scui_ui_res_local->pick_scale, 0, 100);
            scui_coord_t pct = scui_map_ease_out(t, 0, 100, 0, 100);
            scui_ui_res_local->scale_cur = 1.0f +
                (scui_ui_res_local->pick_scale_max - 1.0f) * pct / 100;
            
            if (scui_ui_res_local->pick_anima >= scui_ui_res_local->pick_scale) {
                /* 放大完成: 切换表盘并回主界面 */
                scui_coord_t dial_idx = scui_ui_res_local->dial_ofs[scui_ui_res_local->pick_idx];
                scui_presenter.cwf_idx_set(dial_idx);
                SCUI_LOG_INFO("lantern-pick: idx:%u/%u", dial_idx, scui_ui_res_local->dial_num);
                scui_window_stack_reset(SCUI_UI_SCENE_HOME, false);
                break;
            }
            scui_widget_draw(event->object, NULL, false, 0);
            break;
        }
        
        /* idle: 自动旋转 + 窗口推进(末尾自动迭代下一个表盘) */
        if (scui_ui_res_local->move_lock)
            break;
        
        scui_ui_res_local->scale_breath += scui_ui_res_local->scale_way;
        if (!scui_betw_lr(scui_ui_res_local->scale_breath, -256, 256))
            scui_ui_res_local->scale_way = -scui_ui_res_local->scale_way;
        
        scui_ui_res_local->scale_c  += scui_ui_res_local->scale_way;
        scui_ui_res_local->rotate_x += 1.0f;
        lantern_window_advance();
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    case scui_event_draw_ready: {
        
        /* 绘制就绪: 几何/透视预计算 */
        SCUI_ASSERT(scui_ui_res_local != NULL);
        
        float tan_a = scui_tan(SCUI_RAD_BY_A(scui_ui_res_local->angle_a / 2.0f));
        scui_coord3_t scale  = scui_ui_res_local->scale_c / 1024.0f * scui_ui_res_local->scale_cur;
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
        
        for (uint8_t idx = 0; idx < scui_ui_res_local->dial_show; idx++) {
            
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
            scui_matrix_t *matrix = scui_ui_res_local->matrix;
            scui_matrix_t *matrix_fake = scui_ui_res_local->matrix_fake;
            
            /* 深度信息计算,后面排序处理 */
            scui_coord3_t *center_z = scui_ui_res_local->center_z;
            scui_area3_center_z(&face3, &center_z[idx]);
            
            scui_handle_t image = scui_ui_res_local->image[idx];
            scui_size2_t size2 = {.w = scui_image_w(image),.h = scui_image_h(image),};
            scui_matrix_perspective_view_blit(&matrix[idx], &size2, &face3, &view3);
            scui_matrix_perspective_view_blit(&matrix_fake[idx], &size2, &face3_inv, &view3);
            
            scui_matrix_t *inv_matrix = scui_ui_res_local->inv_matrix;
            scui_matrix_t *inv_matrix_fake = scui_ui_res_local->inv_matrix_fake;
            inv_matrix[idx] = matrix[idx]; inv_matrix_fake[idx] = matrix_fake[idx];
            scui_matrix_inverse(&inv_matrix[idx]);
            scui_matrix_inverse(&inv_matrix_fake[idx]);
            
            /* 面投影四边形(点击命中检测): 正面透视到屏幕 */
            scui_face3_t face3_view = face3;
            scui_area3_perspective(&face3_view, &view3);
            for (uint8_t idx_k = 0; idx_k < 4; idx_k++) {
                scui_ui_res_local->face2[idx][idx_k][0] = face3_view.point3[idx_k].x;
                scui_ui_res_local->face2[idx][idx_k][1] = face3_view.point3[idx_k].y;
            }
        }
        break;
    }
    case scui_event_draw_graph: {
        
        /* 绘制图形: 深度排序渲染 */
        SCUI_ASSERT(scui_ui_res_local != NULL);
        scui_widget_alpha_set(event->object, scui_alpha_cover, false);
        
        /* 根据center_z的深度信息进行排序决定绘制顺序 */
        scui_coord3_t *center_z = scui_ui_res_local->center_z;
        scui_coord3_t *draw_z   = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_coord3_t) * scui_ui_res_local->dial_show);
        scui_coord_t  *draw_i   = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(scui_coord_t)  * scui_ui_res_local->dial_show);
        
        for (uint8_t idx = 0; idx < scui_ui_res_local->dial_show; idx++) {
            draw_i[idx] = idx; draw_z[idx] = center_z[idx];
        }
        
        for (uint8_t idx_i = 0; idx_i < scui_ui_res_local->dial_show; idx_i++)
        for (uint8_t idx_j = 0; idx_j < scui_ui_res_local->dial_show; idx_j++)
            if (draw_z[idx_i] < draw_z[idx_j] && idx_i != idx_j) {
                
                scui_coord_t draw_i_t = draw_i[idx_i];
                draw_i[idx_i] = draw_i[idx_j]; draw_i[idx_j] = draw_i_t;
                scui_coord3_t draw_z_t = draw_z[idx_i];
                draw_z[idx_i] = draw_z[idx_j]; draw_z[idx_j] = draw_z_t;
            }
        
        for (uint8_t idx = 0; idx < scui_ui_res_local->dial_show; idx++) {
            
            scui_handle_t *image = scui_ui_res_local->image;
            scui_matrix_t *matrix = scui_ui_res_local->matrix;
            scui_matrix_t *inv_matrix = scui_ui_res_local->inv_matrix;
            scui_matrix_t *matrix_fake = scui_ui_res_local->matrix_fake;
            scui_matrix_t *inv_matrix_fake = scui_ui_res_local->inv_matrix_fake;
            
            scui_alpha_t alpha = scui_alpha_trans;
            scui_widget_alpha_get(event->object, &alpha);
            scui_widget_alpha_set(event->object, scui_alpha_pct50, false);
            scui_widget_draw_image_3d(event->object, NULL, image[draw_i[idx]], NULL,
                SCUI_COLOR_UNUSED, &matrix_fake[draw_i[idx]], &inv_matrix_fake[draw_i[idx]]);
            scui_widget_alpha_set(event->object, alpha, false);
            
            scui_widget_draw_image_3d(event->object, NULL, image[draw_i[idx]], NULL,
                SCUI_COLOR_UNUSED, &matrix[draw_i[idx]], &inv_matrix[draw_i[idx]]);
        }
        
        SCUI_MEM_FREE(draw_z);
        SCUI_MEM_FREE(draw_i);
        break;
    }
    case scui_event_ptr_down:
        /* 只记录落点(点击跳转在ptr_up判定, 拖动不跳转) */
        if (scui_ui_res_local->pick_way != 0)
            break;
        if (scui_ui_res_local->move_lock)
            break;
        scui_ui_res_local->ptr_down_pt = event->ptr_c;
        scui_ui_res_local->ptr_dragged = false;
        break;
    case scui_event_ptr_move:
        scui_event_mask_over(event);
        
        /* 点击动画期间锁定(不再响应跟手) */
        if (scui_ui_res_local->pick_way != 0)
            break;
        
        /* 位移超阈值视为拖动(仅点击触发跳转) */
        {
            scui_coord_t dx = event->ptr_e.x - scui_ui_res_local->ptr_down_pt.x;
            scui_coord_t dy = event->ptr_e.y - scui_ui_res_local->ptr_down_pt.y;
            if (dx * dx + dy * dy > scui_ui_res_local->click_slop * scui_ui_res_local->click_slop)
                scui_ui_res_local->ptr_dragged = true;
        }
        
        scui_coord_t ptr_dx = event->ptr_e.x - event->ptr_s.x;
        scui_coord_t ptr_dy = event->ptr_e.y - event->ptr_s.y;
        scui_ui_res_local->move_lock = true;
        scui_ui_res_local->rotate_x += ptr_dx;
        lantern_window_advance();
        
        if (scui_betw_lr(scui_ui_res_local->rotate_y + ptr_dy, -15, 15))
            scui_ui_res_local->rotate_y += ptr_dy;
        
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    case scui_event_ptr_up: {
        scui_ui_res_local->move_lock = false;
        
        /* 拖动不触发点击跳转 */
        if (scui_ui_res_local->ptr_dragged)
            break;
        if (scui_ui_res_local->pick_way != 0)
            break;
        
        /* 点击命中检测: 最上层可见面 */
        scui_point_t point = event->ptr_c;
        scui_coord3_t best_z = 1e9f;
        bool hit = false;
        for (uint8_t idx = 0; idx < scui_ui_res_local->dial_show; idx++) {
            
            if (scui_ui_res_local->center_z[idx] >= best_z)
                continue;
            if (!lantern_point_in_quad(&point, scui_ui_res_local->face2[idx]))
                continue;
            
            best_z = scui_ui_res_local->center_z[idx];
            scui_ui_res_local->pick_idx = idx;
            hit = true;
        }
        
        if (hit) {
            scui_ui_res_local->pick_way   = 1;
            scui_ui_res_local->pick_anima = 0;
            scui_ui_res_local->move_lock  = true;
            scui_event_mask_over(event);
            scui_widget_draw(event->object, NULL, false, 0);
        }
        break;
    }
    case scui_event_key_click: {
        
        scui_window_stack_del_by(SCUI_UI_SCENE_LANTERN,
            scui_window_switch_circle, scui_opt_dir_none);
        break;
    }
    default:
        break;
    }
}
