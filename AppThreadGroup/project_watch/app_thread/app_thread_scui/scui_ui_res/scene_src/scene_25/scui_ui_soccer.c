/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 光子所在面(分布在不同层/列, 随球体旋转自然呈现/隐没) */
static const scui_coord_t soccer_photon_face[5][2] = {
    {0, 0}, {1, 1}, {2, 2}, {3, 3}, {0, 4},
};

/* 点击动画状态机 */
typedef enum {
    soccer_way_idle = 0,    /* 正常旋转 + 光子行进 */
    soccer_way_rotate,      /* 选中面旋转到屏幕中心(正对) */
    soccer_way_scale,       /* 整体放大到面铺满屏幕 */
} soccer_way_t;

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
    scui_matrix_t inv_matrix_bg[4][5];  // 变换矩阵(从上至下:4层;每层5个)
    scui_matrix_t inv_matrix_fg[4][5];  // 变换矩阵(从上至下:4层;每层5个)
    scui_coord3_t normal_z_bg[4][5];    // 面法线
    scui_coord3_t normal_z_fg[4][5];    // 面法线
    bool          move_lock;            // 移动锁
    
    /* 光子参数(create配置) */
    scui_coord_t  photon_num;          // 光子数量(<=5)
    scui_coord_t  photon_size;         // 光子绘制尺寸(star.png 原尺寸64x64)
    scui_coord3_t photon_speed;        // 行进速度(每tick相位推进, 0~1绕六边形一圈)
    /* 光子: 边上相位(0~1 绕六边形一圈) */
    scui_coord3_t photon_phase[5];
    
    /* 面几何缓存(变换链复用) */
    scui_coord3_t angle_x[4];           // 每层x轴倾斜角
    scui_point3_t face3_bg_ofs[4];      // 每层y/z偏移
    scui_point3_t offset;               // 居中偏移(draw_ready更新)
    
    /* 面投影与深度(draw_ready更新, 点击命中检测) */
    scui_coord3_t face2_bg[4][5][4][2]; // 面投影四边形(屏幕坐标)
    scui_coord3_t face_z_bg[4][5];      // 面中心深度
    
    /* 点击动画参数(create配置) */
    scui_coord_t  pick_rotate;          // 旋转动画时长(ms)
    scui_coord_t  pick_scale;           // 放大动画时长(ms)
    scui_coord3_t pick_scale_max;       // 放大目标倍数(面铺满屏幕)
    /* 点击动画 */
    scui_coord3_t scale_cur;            // 整体缩放(1.0为基准)
    scui_point3_t rotate_dst;           // 旋转目标(x,y)
    scui_point3_t rotate_src;           // 旋转起点(x,y)
    scui_coord_t  pick_way;             // 状态机
    scui_coord_t  pick_anima;           // 动画计时
    scui_coord_t  hover_time;           // 悬停时长配置(ms)
    scui_coord_t  click_slop;           // 点击位移阈值(px)
    scui_coord_t  hover_tick;           // 悬停计时(>0暂停自动旋转)
    scui_coord_t  pick_j, pick_i;       // 选中面
    scui_point_t  ptr_down_pt;          // 按下落点(点击判定)
    bool          ptr_dragged;          // 是否已拖动(拖动不触发跳转)
} * scui_ui_res_local = NULL;

/*@brief 面变换链: 面本地坐标 -> 屏幕坐标(3D)
 *       与draw_ready的矩阵链完全一致, 供光子/命中检测复用
 *@param j    层
 *@param i    列
 *@param x    面本地x
 *@param y    面本地y
 *@param z    面本地z
 *@param p    输出屏幕坐标(3D)
 */
static void soccer_face_point3(scui_coord_t j, scui_coord_t i,
    scui_coord3_t x, scui_coord3_t y, scui_coord3_t z, scui_point3_t *p)
{
    /* 1. x轴向旋转(面倾斜) */
    scui_matrix_t x_matrix = {0};
    scui_matrix_identity(&x_matrix);
    scui_matrix_rotate_a(&x_matrix, -(scui_ui_res_local->angle_x[j]), 0x01);
    scui_point3_t point3 = {.x = x,.y = y,.z = z,};
    scui_point3_transform_by_matrix(&point3, &x_matrix);
    /* 2. y/z轴向偏移 */
    point3.y += scui_ui_res_local->face3_bg_ofs[j].y;
    point3.z += scui_ui_res_local->face3_bg_ofs[j].z;
    /* 3. y轴向旋转(列角 + 全局旋转) */
    scui_matrix_t y_matrix = {0};
    scui_matrix_identity(&y_matrix);
    scui_matrix_rotate_a(&y_matrix, +(i * 72.0f + (j < 2 ? 0.0f : 36.0f) + scui_ui_res_local->rotate.y), 0x02);
    scui_point3_transform_by_matrix(&point3, &y_matrix);
    /* 4. x轴向旋转(全局) */
    scui_matrix_t rx_matrix = {0};
    scui_matrix_identity(&rx_matrix);
    scui_matrix_rotate_a(&rx_matrix, +(scui_ui_res_local->rotate.x), 0x01);
    scui_point3_transform_by_matrix(&point3, &rx_matrix);
    /* 5. 移动到中心 + 整体缩放(与draw_ready一致: 先平移再绕中心缩放) */
    scui_point3_t offset = scui_ui_res_local->offset;
    point3.x += offset.x;
    point3.y += offset.y;
    if (scui_ui_res_local->scale_cur != 1.0f) {
        point3.x = offset.x + (point3.x - offset.x) * scui_ui_res_local->scale_cur;
        point3.y = offset.y + (point3.y - offset.y) * scui_ui_res_local->scale_cur;
    }
    
    *p = point3;
}

/*@brief 光子位置: 六边形边上一点(相位0~1绕六边形一圈)
 *@param idx 光子索引
 *@param p   输出屏幕坐标(3D)
 */
static void soccer_photon_point(scui_coord_t idx, scui_point3_t *p)
{
    /* 正六边形(flat-top): 外接圆半径=边长, 顶点角度30+k*60 */
    static const scui_coord3_t hex[6][2] = {
        { 1.0f,  0.0f},
        { 0.5f,  0.8660254f},
        {-0.5f,  0.8660254f},
        {-1.0f,  0.0f},
        {-0.5f, -0.8660254f},
        { 0.5f, -0.8660254f},
    };
    scui_coord3_t R = scui_ui_res_local->sub_size;
    scui_coord3_t t = scui_ui_res_local->photon_phase[idx] * 6.0f;
    scui_coord_t  k = (scui_coord_t)t;
    if (k > 5) k = 5;
    scui_coord3_t f = t - k;
    scui_coord_t  k2 = (k + 1) % 6;
    scui_coord3_t lx = (hex[k][0] * (1.0f - f) + hex[k2][0] * f) * R;
    scui_coord3_t ly = (hex[k][1] * (1.0f - f) + hex[k2][1] * f) * R;
    
    /* 面本地坐标: 六边形中心在矩形中心y=sub_ver_res/2处, 需补偏移 */
    scui_coord_t j = soccer_photon_face[idx][0];
    scui_coord_t i = soccer_photon_face[idx][1];
    soccer_face_point3(j, i, lx, scui_ui_res_local->sub_ver_res / 2 + ly, 0.0f, p);
}

/*@brief 点在凸四边形内判定(叉积同号)
 *@param pt    点
 *@param quad  四边形(4点, 逆时针)
 *@retval 是否在内
 */
static bool soccer_point_in_quad(scui_point_t *pt, scui_coord3_t (*quad)[2])
{
    for (scui_coord_t k = 0; k < 4; k++) {
        scui_coord3_t ax = quad[k][0] - pt->x;
        scui_coord3_t ay = quad[k][1] - pt->y;
        scui_coord3_t bx = quad[(k + 1) % 4][0] - pt->x;
        scui_coord3_t by = quad[(k + 1) % 4][1] - pt->y;
        scui_coord3_t cross = ax * by - ay * bx;
        /* 4条边叉积同号(允许接近0) */
        if (cross < 0.0f)
            return false;
    }
    return true;
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_soccer_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        break;
    case scui_event_destroy:
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
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
    case scui_event_create: {
        
        // 相邻两个六边形之间的夹角约为 138.190f
        // 五边形和六边形之间的夹角约为 142.623f
        scui_ui_res_local->angle_56 = 142.623f - 90.0f;
        scui_ui_res_local->angle_66 = 138.190f - 90.0f - (180.0f - 142.623f);

        /* 光子与点击动画参数(可配置) */
        scui_ui_res_local->photon_num     = 5;
        scui_ui_res_local->photon_size    = 8;
        scui_ui_res_local->photon_speed   = 0.0025f;
        scui_ui_res_local->pick_rotate    = 900;
        scui_ui_res_local->pick_scale     = 800;
        scui_ui_res_local->pick_scale_max = 6.0f;
        scui_ui_res_local->hover_time     = 3000;
        scui_ui_res_local->click_slop     = 12;
        
        scui_ui_res_local->image_bg = scui_image_prj_3d_Soccer_board;
        for (uint8_t idx_j = 0; idx_j < 4; idx_j++)
        for (uint8_t idx_i = 0; idx_i < 5; idx_i++) {
            scui_handle_t image_fg_0 = scui_ui_res_local->image_bg - 3;
            scui_ui_res_local->image_fg[idx_j][idx_i] = image_fg_0 + idx_j * 5 + idx_i;
            if ((idx_j * 5 + idx_i) >= scui_image_prj_3d_Soccer_star - 2 - image_fg_0)
                scui_ui_res_local->image_fg[idx_j][idx_i] += 1;
            if ((idx_j * 5 + idx_i) >= scui_image_prj_3d_Soccer_bg - image_fg_0)
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
        
        /* 面几何缓存(变换链复用) */
        scui_coord3_t dist_5 = scui_tan(SCUI_RAD_BY_A(36.0f)) * scui_ui_res_local->sub_size;
        scui_coord3_t sin_56 = scui_sin(SCUI_RAD_BY_A(scui_ui_res_local->angle_56));
        scui_coord3_t cos_56 = scui_cos(SCUI_RAD_BY_A(scui_ui_res_local->angle_56));
        scui_coord3_t sin_66 = scui_sin(SCUI_RAD_BY_A(scui_ui_res_local->angle_66));
        scui_coord3_t cos_66 = scui_cos(SCUI_RAD_BY_A(scui_ui_res_local->angle_66));
        scui_ui_res_local->angle_x[0] = scui_ui_res_local->angle_56;
        scui_ui_res_local->angle_x[1] = scui_ui_res_local->angle_66;
        scui_ui_res_local->angle_x[2] = -scui_ui_res_local->angle_66;
        scui_ui_res_local->angle_x[3] = -scui_ui_res_local->angle_56;
        scui_point3_t face3_bg_ofs[4] = {
            {.y = -cos_66 * scui_ui_res_local->sub_ver_res * 3 / 4 - cos_56 * scui_ui_res_local->sub_ver_res, .z = -dist_5},
            {.y = -cos_66 * scui_ui_res_local->sub_ver_res * 3 / 4, .z = -dist_5 - sin_56 * scui_ui_res_local->sub_ver_res,},
            {.y = -cos_66 * scui_ui_res_local->sub_ver_res * 1 / 4, .z = -dist_5 - sin_56 * scui_ui_res_local->sub_ver_res - sin_66 * scui_ui_res_local->sub_ver_res,},
            {.y = +cos_66 * scui_ui_res_local->sub_ver_res * 3 / 4, .z = -dist_5 - sin_56 * scui_ui_res_local->sub_ver_res,},
        };
        for (scui_coord_t idx_k = 0; idx_k < 4; idx_k++)
            scui_ui_res_local->face3_bg_ofs[idx_k] = face3_bg_ofs[idx_k];
        
        /* 光子初始化: 相位均匀错开 */
        for (scui_coord_t idx = 0; idx < scui_ui_res_local->photon_num; idx++)
            scui_ui_res_local->photon_phase[idx] = (scui_coord3_t)idx / scui_ui_res_local->photon_num;
        
        /* 点击动画初始态 */
        scui_ui_res_local->scale_cur = 1.0f;
        scui_ui_res_local->pick_way  = soccer_way_idle;
        scui_ui_res_local->pick_anima = 0;
        scui_ui_res_local->hover_tick = 0;
        scui_ui_res_local->ptr_dragged = false;
        break;
    }
    case scui_event_anima_elapse: {
        
        /* 点击动画: 旋转/缩放阶段锁定(不响应指针) */
        if (scui_ui_res_local->pick_way == soccer_way_rotate) {
            
            scui_ui_res_local->pick_anima += event->tick;
            scui_coord_t t = scui_map_linear(scui_ui_res_local->pick_anima,
                0, scui_ui_res_local->pick_rotate, 0, 100);
            scui_coord_t pct = scui_map_ease_out(t, 0, 100, 0, 100);
            scui_ui_res_local->rotate.x = scui_ui_res_local->rotate_src.x +
                (scui_ui_res_local->rotate_dst.x - scui_ui_res_local->rotate_src.x) * pct / 100;
            scui_ui_res_local->rotate.y = scui_ui_res_local->rotate_src.y +
                (scui_ui_res_local->rotate_dst.y - scui_ui_res_local->rotate_src.y) * pct / 100;
            
            if (scui_ui_res_local->pick_anima >= scui_ui_res_local->pick_rotate) {
                scui_ui_res_local->rotate.x = scui_ui_res_local->rotate_dst.x;
                scui_ui_res_local->rotate.y = scui_ui_res_local->rotate_dst.y;
                /* 放大目标: 选中面铺满屏幕(不超出), 按当前正对投影计算 */
                scui_coord3_t (*face2)[2] = scui_ui_res_local->face2_bg[scui_ui_res_local->pick_j][scui_ui_res_local->pick_i];
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
                scui_ui_res_local->pick_way   = soccer_way_scale;
                scui_ui_res_local->pick_anima = 0;
            }
            scui_widget_draw(event->object, NULL, false, 0);
            break;
        }
        if (scui_ui_res_local->pick_way == soccer_way_scale) {
            
            scui_ui_res_local->pick_anima += event->tick;
            scui_coord_t t = scui_map_linear(scui_ui_res_local->pick_anima,
                0, scui_ui_res_local->pick_scale, 0, 100);
            scui_coord_t pct = scui_map_ease_out(t, 0, 100, 0, 100);
            scui_ui_res_local->scale_cur = 1.0f +
                (scui_ui_res_local->pick_scale_max - 1.0f) * pct / 100;
            
            if (scui_ui_res_local->pick_anima >= scui_ui_res_local->pick_scale) {
                /* 放大完成: 跳转主界面 */
                scui_window_stack_reset(SCUI_UI_SCENE_HOME, false);
                break;
            }
            scui_widget_draw(event->object, NULL, false, 0);
            break;
        }
        
        /* idle: 光子行进 + 自动旋转(跟手悬停时暂停) */
        for (scui_coord_t idx = 0; idx < scui_ui_res_local->photon_num; idx++) {
            scui_ui_res_local->photon_phase[idx] += scui_ui_res_local->photon_speed;
            if (scui_ui_res_local->photon_phase[idx] >= 1.0f)
                scui_ui_res_local->photon_phase[idx] -= 1.0f;
        }
        if (scui_ui_res_local->move_lock)
            break;
        
        if (scui_ui_res_local->hover_tick > 0) {
            scui_ui_res_local->hover_tick -= event->tick;
            if (scui_ui_res_local->hover_tick < 0)
                scui_ui_res_local->hover_tick = 0;
        } else {
            scui_ui_res_local->rotate.y += 1.5f;
        }
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    case scui_event_draw_ready: {
        
        /* 绘制就绪: 几何/矩阵预计算 */
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
        scui_ui_res_local->offset = offset;
        
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
            /* 6.整体缩放(点击放大) */
            if (scui_ui_res_local->scale_cur != 1.0f)
            for (uint8_t idx_k = 0; idx_k < 4; idx_k++) {
                face3_bg.point3[idx_k].x = offset.x + (face3_bg.point3[idx_k].x - offset.x) * scui_ui_res_local->scale_cur;
                face3_bg.point3[idx_k].y = offset.y + (face3_bg.point3[idx_k].y - offset.y) * scui_ui_res_local->scale_cur;
                face3_fg.point3[idx_k].x = offset.x + (face3_fg.point3[idx_k].x - offset.x) * scui_ui_res_local->scale_cur;
                face3_fg.point3[idx_k].y = offset.y + (face3_fg.point3[idx_k].y - offset.y) * scui_ui_res_local->scale_cur;
            }
            
            /* 记录面投影(点击命中检测)与深度 */
            for (uint8_t idx_k = 0; idx_k < 4; idx_k++) {
                scui_ui_res_local->face2_bg[idx_j][idx_i][idx_k][0] = face3_bg.point3[idx_k].x;
                scui_ui_res_local->face2_bg[idx_j][idx_i][idx_k][1] = face3_bg.point3[idx_k].y;
            }
            scui_ui_res_local->face_z_bg[idx_j][idx_i] =
                (face3_bg.point3[0].z + face3_bg.point3[1].z +
                 face3_bg.point3[2].z + face3_bg.point3[3].z) / 4;
            
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
                scui_matrix_t (*inv_matrix_bg)[5] = scui_ui_res_local->inv_matrix_bg;
                scui_size2_t size2_bg = {.w = image_bg_w,.h = image_bg_h,};
                scui_matrix_affine_blit(&matrix_bg[idx_j][idx_i], &size2_bg, &face3_bg);
                inv_matrix_bg[idx_j][idx_i] = matrix_bg[idx_j][idx_i];
                scui_matrix_inverse(&inv_matrix_bg[idx_j][idx_i]);
            }
            if (normal_z_fg[idx_j][idx_i] < -0.0f) {
                scui_matrix_t (*matrix_fg)[5] = scui_ui_res_local->matrix_fg;
                scui_matrix_t (*inv_matrix_fg)[5] = scui_ui_res_local->inv_matrix_fg;
                scui_size2_t size2_fg = {.w = image_fg_w,.h = image_fg_h,};
                scui_matrix_affine_blit(&matrix_fg[idx_j][idx_i], &size2_fg, &face3_fg);
                inv_matrix_fg[idx_j][idx_i] = matrix_fg[idx_j][idx_i];
                scui_matrix_inverse(&inv_matrix_fg[idx_j][idx_i]);
            }
        }
        break;
    }
    case scui_event_draw_graph: {
        
        /* 绘制图形: 执行3D渲染 */
        SCUI_ASSERT(scui_ui_res_local != NULL);
        
        for (uint8_t idx_j = 0; idx_j < 4; idx_j++)
        for (uint8_t idx_i = 0; idx_i < 5; idx_i++) {
            
            scui_coord3_t (*normal_z_bg)[5] = scui_ui_res_local->normal_z_bg;
            scui_coord3_t (*normal_z_fg)[5] = scui_ui_res_local->normal_z_fg;
            if (normal_z_bg[idx_j][idx_i] < -0.0f) {
                scui_handle_t image_bg = scui_ui_res_local->image_bg;
                scui_matrix_t (*matrix_bg)[5] = scui_ui_res_local->matrix_bg;
                scui_matrix_t (*inv_matrix_bg)[5] = scui_ui_res_local->inv_matrix_bg;
                scui_widget_draw_image_3d(event->object, NULL, image_bg, NULL,
                    SCUI_COLOR_UNUSED, &matrix_bg[idx_j][idx_i], &inv_matrix_bg[idx_j][idx_i]);
            }
            if (normal_z_fg[idx_j][idx_i] < -0.0f) {
                scui_handle_t (*image_fg)[5] = scui_ui_res_local->image_fg;
                scui_matrix_t (*matrix_fg)[5] = scui_ui_res_local->matrix_fg;
                scui_matrix_t (*inv_matrix_fg)[5] = scui_ui_res_local->inv_matrix_fg;
                scui_widget_draw_image_3d(event->object, NULL, image_fg[idx_j][idx_i], NULL,
                     SCUI_COLOR_UNUSED, &matrix_fg[idx_j][idx_i], &inv_matrix_fg[idx_j][idx_i]);
            }
        }
        
        /* 光子特效: 随所在面行进, 面不可见则隐藏 */
        scui_handle_t image_star = scui_image_prj_3d_Soccer_star;
        for (scui_coord_t idx = 0; idx < scui_ui_res_local->photon_num; idx++) {
            
            scui_coord_t j = soccer_photon_face[idx][0];
            scui_coord_t i = soccer_photon_face[idx][1];
            if (scui_ui_res_local->normal_z_bg[j][i] >= -0.0f)
                continue;
            
            scui_point3_t p = {0};
            soccer_photon_point(idx, &p);
            
            /* star原图64x64, 缩放到光子尺寸 */
            scui_point_t scale = {
                .x = scui_ui_res_local->photon_size * SCUI_SCALE_COF / scui_image_w(image_star),
                .y = scui_ui_res_local->photon_size * SCUI_SCALE_COF / scui_image_h(image_star),
            };
            scui_area_t target = {
                .x = (scui_coord_t)p.x - scui_ui_res_local->photon_size / 2,
                .y = (scui_coord_t)p.y - scui_ui_res_local->photon_size / 2,
                .w = scui_ui_res_local->photon_size,
                .h = scui_ui_res_local->photon_size,
            };
            scui_area_t  tg_img  = scui_image_area(image_star);
            scui_point_t anchor = scui_area_center(&target);
            scui_point_t center = scui_area_center(&tg_img);
            scui_widget_draw_image_scale(event->object, &target, image_star, NULL,
                SCUI_COLOR_MAKE32(true, 0xFF000000, 0xFFFFFFFF),
                anchor, center, scale);
        }
        break;
    }
    case scui_event_ptr_down:
        /* 只记录落点(点击跳转在ptr_up判定, 拖动不跳转) */
        if (scui_ui_res_local->pick_way != soccer_way_idle)
            break;
        if (scui_ui_res_local->move_lock)
            break;
        scui_ui_res_local->ptr_down_pt = event->ptr_c;
        scui_ui_res_local->ptr_dragged = false;
        break;
    case scui_event_ptr_move:
        scui_event_mask_over(event);
        
        /* 点击动画期间锁定(不再响应跟手) */
        if (scui_ui_res_local->pick_way != soccer_way_idle)
            break;
        
        /* 位移超阈值视为拖动(仅点击触发跳转) */
        {
            scui_coord_t dx = event->ptr_e.x - scui_ui_res_local->ptr_down_pt.x;
            scui_coord_t dy = event->ptr_e.y - scui_ui_res_local->ptr_down_pt.y;
            if (dx * dx + dy * dy > scui_ui_res_local->click_slop * scui_ui_res_local->click_slop)
                scui_ui_res_local->ptr_dragged = true;
        }
        
        /* y轴旋转是x的移动偏移量(方向与手指反向) */
        /* x轴旋转是y的移动偏移量(方向与手指反向) */
        scui_ui_res_local->move_lock = true;
        scui_ui_res_local->hover_tick = scui_ui_res_local->hover_time;
        scui_ui_res_local->rotate.y += event->ptr_s.x - event->ptr_e.x;
        scui_ui_res_local->rotate.x += event->ptr_e.y - event->ptr_s.y;
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    case scui_event_ptr_up: {
        scui_ui_res_local->move_lock = false;
        /* 抬手后悬停(暂停自动旋转) */
        scui_ui_res_local->hover_tick = scui_ui_res_local->hover_time;
        
        /* 拖动不触发点击跳转 */
        if (scui_ui_res_local->ptr_dragged)
            break;
        if (scui_ui_res_local->pick_way != soccer_way_idle)
            break;
        
        /* 点击命中检测: 最上层可见面 */
        scui_point_t point = event->ptr_c;
        scui_coord3_t best_z = 1e9f;
        bool hit = false;
        for (uint8_t idx_j = 0; idx_j < 4; idx_j++)
        for (uint8_t idx_i = 0; idx_i < 5; idx_i++) {
            
            if (scui_ui_res_local->normal_z_bg[idx_j][idx_i] >= -0.0f)
                continue;
            if (scui_ui_res_local->face_z_bg[idx_j][idx_i] >= best_z)
                continue;
            if (!soccer_point_in_quad(&point, scui_ui_res_local->face2_bg[idx_j][idx_i]))
                continue;
            
            best_z = scui_ui_res_local->face_z_bg[idx_j][idx_i];
            scui_ui_res_local->pick_j = idx_j;
            scui_ui_res_local->pick_i = idx_i;
            hit = true;
        }
        
        if (hit) {
            /* 目标旋转: 选中面旋转到正对屏幕(抵消面朝向) */
            scui_coord_t j = scui_ui_res_local->pick_j;
            scui_coord_t i = scui_ui_res_local->pick_i;
            scui_ui_res_local->rotate_src.x = scui_ui_res_local->rotate.x;
            scui_ui_res_local->rotate_src.y = scui_ui_res_local->rotate.y;
            scui_ui_res_local->rotate_dst.x = scui_ui_res_local->angle_x[j];
            scui_coord3_t dst_y = -(i * 72.0f + (j < 2 ? 0.0f : 36.0f));
            /* 规范化: 取与当前旋转角最近的等价角(避免绕大圈) */
            while (dst_y - scui_ui_res_local->rotate_src.y >  180.0f) dst_y -= 360.0f;
            while (dst_y - scui_ui_res_local->rotate_src.y < -180.0f) dst_y += 360.0f;
            scui_ui_res_local->rotate_dst.y = dst_y;
            
            scui_ui_res_local->pick_way   = soccer_way_rotate;
            scui_ui_res_local->pick_anima = 0;
            scui_ui_res_local->move_lock  = true;
            scui_event_mask_over(event);
            scui_widget_draw(event->object, NULL, false, 0);
        }
        break;
    }
    default:
        break;
    }
}
