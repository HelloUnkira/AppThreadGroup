/*实现目标:
 *    主题:蜂窝(球面)
 *    图标均匀铺在虚拟球面上(geodesic二十面体细分, 真球面均匀, 旋转对称)
 *    观察者正对球心orthographic投影: 正对点最大, 边缘透视缩小
 *    上下左右拖动=球旋转(跟手), 背面内容从球轮廓边界转进来
 *    边缘圆形挤压: 图标中心在圆内, 缩到贴圆边
 *    松手: 最近焦点回正中心; 点击: 先居中再跳转
 *    单全屏custom原始解算贴图
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* geodesic球面均匀点(二十面体细分1级, 42个单位球坐标, 北极{0,0,1}起) */
static const struct {
    scui_coord3_t x, y, z;
} scui_ui_honeycomb_geo[] = {
    { 0.0000f,  0.0000f,  1.0000f},
    { 0.0000f, -0.5257f,  0.8507f},
    { 0.0000f,  0.5257f,  0.8507f},
    { 0.5000f, -0.3090f,  0.8090f},
    { 0.5000f,  0.3090f,  0.8090f},
    {-0.5000f, -0.3090f,  0.8090f},
    {-0.5000f,  0.3090f,  0.8090f},
    { 0.8507f,  0.0000f,  0.5257f},
    {-0.8507f,  0.0000f,  0.5257f},
    { 0.3090f, -0.8090f,  0.5000f},
    {-0.3090f,  0.8090f,  0.5000f},
    {-0.3090f, -0.8090f,  0.5000f},
    { 0.3090f,  0.8090f,  0.5000f},
    {-0.8090f, -0.5000f,  0.3090f},
    { 0.8090f,  0.5000f,  0.3090f},
    {-0.8090f,  0.5000f,  0.3090f},
    { 0.8090f, -0.5000f,  0.3090f},
    {-0.5257f,  0.8507f,  0.0000f},
    { 0.5257f,  0.8507f,  0.0000f},
    {-0.5257f, -0.8507f,  0.0000f},
    { 0.5257f, -0.8507f,  0.0000f},
    { 1.0000f,  0.0000f,  0.0000f},
    { 0.0000f,  1.0000f,  0.0000f},
    {-1.0000f,  0.0000f,  0.0000f},
    { 0.0000f, -1.0000f,  0.0000f},
    {-0.8090f,  0.5000f, -0.3090f},
    {-0.8090f, -0.5000f, -0.3090f},
    { 0.8090f,  0.5000f, -0.3090f},
    { 0.8090f, -0.5000f, -0.3090f},
    { 0.3090f, -0.8090f, -0.5000f},
    {-0.3090f,  0.8090f, -0.5000f},
    { 0.3090f,  0.8090f, -0.5000f},
    {-0.3090f, -0.8090f, -0.5000f},
    { 0.8507f,  0.0000f, -0.5257f},
    {-0.8507f,  0.0000f, -0.5257f},
    {-0.5000f,  0.3090f, -0.8090f},
    { 0.5000f, -0.3090f, -0.8090f},
    {-0.5000f, -0.3090f, -0.8090f},
    { 0.5000f,  0.3090f, -0.8090f},
    { 0.0000f, -0.5257f, -0.8507f},
    { 0.0000f,  0.5257f, -0.8507f},
    { 0.0000f,  0.0000f, -1.0000f}
};
#define SCUI_UI_HONEYCOMB_GEO_NUM   scui_arr_len(scui_ui_honeycomb_geo)

static struct {
    scui_coord3_t pitch;        /* 球旋转: 俯仰(弧度) */
    scui_coord3_t yaw;          /* 球旋转: 偏航(弧度) */
    scui_coord3_t rad;          /* 球半径(短边一半) */
    scui_coord3_t icon_max;     /* 正对图标wh */
    scui_coord3_t icon_min;     /* 边缘图标wh */
    scui_coord_t  geo_num;      /* 实际布点数(<=42) */

    bool          touch;        /* 拖动中 */
    scui_coord_t  down_x, down_y;  /* 按下起点 */
    scui_coord_t  last_x, last_y;   /* 上次拖动点 */
    scui_coord_t  move_lock;    /* 动画/跟手锁定 */

    /* 回正/居中snap动画 */
    bool          snap_on;
    scui_coord_t  snap_jump;    /* >=0: 居中走完后放大icon再跳转该app idx */
    scui_coord3_t snap_p0, snap_y0;
    scui_coord3_t snap_p1, snap_y1;
    scui_coord_t  snap_t;
    scui_coord_t  snap_dur;
    scui_coord_t  drawn;

    /* 居中后放大icon动画 */
    bool          zoom_on;
    scui_coord_t  zoom_idx;
    scui_coord_t  zoom_t;
    scui_coord_t  zoom_dur;
    scui_coord3_t zoom_size;    /* 放大目标wh */

    /* 编码器焦点idx */
    scui_coord_t  focus_idx;
} * scui_ui_res_local = NULL;

/*@brief 解算单个图标(球面3D坐标+旋转+ortho投影+透视+圆形挤压)
 *@param gx/gy/gz 球面单位坐标
 *@param widget_cx/cy 屏幕中心
 *@param rad 球半径
 *@param out_x/out_y 输出图标中心
 *@param out_size 输出图标wh
 *@param out_z2 输出旋转后z分量(透视/可见)
 */
static void scui_ui_honeycomb_solve(scui_coord3_t gx, scui_coord3_t gy, scui_coord3_t gz,
    scui_coord3_t widget_cx, scui_coord3_t widget_cy, scui_coord3_t rad,
    scui_coord3_t *out_x, scui_coord3_t *out_y, scui_coord3_t *out_size,
    scui_coord3_t *out_z2)
{
    scui_coord3_t cp = scui_cos(scui_ui_res_local->pitch);
    scui_coord3_t sp = scui_sin(scui_ui_res_local->pitch);
    scui_coord3_t y1 = gy * cp - gz * sp;
    scui_coord3_t z1 = gy * sp + gz * cp;
    scui_coord3_t cy = scui_cos(scui_ui_res_local->yaw);
    scui_coord3_t sy = scui_sin(scui_ui_res_local->yaw);
    scui_coord3_t x1 = gx * cy + z1 * sy;
    scui_coord3_t z2 = -gx * sy + z1 * cy;

    *out_z2 = z2;
    *out_x = widget_cx + x1 * rad;
    *out_y = widget_cy + y1 * rad;

    /* 透视大小: 正对(z2=1)最大, 边缘(z2=0)最小 */
    scui_coord3_t persp = z2;
    if (persp < 0.0f)
        persp = 0.0f;
    scui_coord3_t size = scui_ui_res_local->icon_min +
        (scui_ui_res_local->icon_max - scui_ui_res_local->icon_min) * persp;

    /* 圆形挤压: 中心在圆内, 缩到贴圆边 */
    scui_coord3_t half = size / 2;
    scui_coord3_t ddx = *out_x - widget_cx;
    scui_coord3_t ddy = *out_y - widget_cy;
    scui_coord3_t avail = rad - sqrtf(ddx * ddx + ddy * ddy);
    if (avail < half)
        size = avail * 2;
    if (size < 1.0f)
        size = 1.0f;
    *out_size = size;
}

/*@brief 启动snap动画(把目标点转到中心)
 *@param gx/gy/gz 目标球面坐标
 *@param jump  >=0: 走完后跳转该app idx
 *@param limit  >0: 限幅(回正);  0: 不限幅(点击精确居中)
 */
static void scui_ui_honeycomb_snap(scui_coord3_t gx, scui_coord3_t gy, scui_coord3_t gz,
    scui_coord_t jump, scui_coord3_t limit)
{
    scui_coord3_t pt = atan2f(gy, gz);
    scui_coord3_t yw = atan2f(-gx, hypotf(gy, gz));
    /* 最短路径: 起点取主值[-π,π], 差值也取主值(避免累计角度绕圈) */
    scui_coord3_t p0 = atan2f(sinf(scui_ui_res_local->pitch), cosf(scui_ui_res_local->pitch));
    scui_coord3_t y0 = atan2f(sinf(scui_ui_res_local->yaw),   cosf(scui_ui_res_local->yaw));
    scui_coord3_t dp = atan2f(sinf(pt - p0), cosf(pt - p0));
    scui_coord3_t dy = atan2f(sinf(yw - y0), cosf(yw - y0));
    if (limit > 0.0f) {
        scui_coord3_t dd = hypotf(dp, dy);
        if (dd > limit) {
            dp *= limit / dd;
            dy *= limit / dd;
        }
    }
    scui_ui_res_local->snap_on   = true;
    scui_ui_res_local->snap_jump = jump;
    scui_ui_res_local->snap_p0   = p0;
    scui_ui_res_local->snap_y0   = y0;
    scui_ui_res_local->snap_p1   = p0 + dp;
    scui_ui_res_local->snap_y1   = y0 + dy;
    scui_ui_res_local->snap_t    = 0;
    scui_ui_res_local->snap_dur  = 280;
}

/*@brief 窗口控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_honeycomb_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);

        scui_ui_res_local->pitch      = 0.0f;
        scui_ui_res_local->yaw       = 0.0f;
        scui_ui_res_local->touch      = false;
        scui_ui_res_local->move_lock  = 0;
        scui_ui_res_local->snap_on    = false;
        scui_ui_res_local->snap_jump  = -1;
        scui_ui_res_local->drawn      = false;
        scui_ui_res_local->zoom_on    = false;
        scui_ui_res_local->focus_idx  = 0;

        scui_area_t widget_clip = scui_widget_area(event->object);
        scui_ui_res_local->rad = scui_min(widget_clip.w, widget_clip.h) / 2;
        scui_ui_res_local->icon_max = scui_ui_res_local->rad * 0.50f;
        scui_ui_res_local->icon_min = scui_ui_res_local->rad * 0.09f;

        scui_ui_scene_list_cfg(scui_ui_scene_list_type_honeycomb);
        scui_ui_res_local->geo_num = scui_min((scui_coord_t)scui_ui_scene_list_num,
            (scui_coord_t)SCUI_UI_HONEYCOMB_GEO_NUM);
        SCUI_LOG_INFO("[hb] create rad=%d max=%d min=%d num=%d geo=%d",
            (int)scui_ui_res_local->rad, (int)scui_ui_res_local->icon_max,
            (int)scui_ui_res_local->icon_min, (int)scui_ui_scene_list_num,
            scui_ui_res_local->geo_num);
        break;
    case scui_event_destroy:
        break;
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
void scui_ui_scene_honeycomb_custom_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {

        if (scui_ui_res_local->snap_on) {
            scui_ui_res_local->snap_t += event->tick;
            scui_coord3_t k = (scui_coord3_t)scui_ui_res_local->snap_t /
                scui_ui_res_local->snap_dur;
            if (k > 1.0f)
                k = 1.0f;
            k = k * k * (3.0f - 2.0f * k);   /* smoothstep */
            scui_ui_res_local->pitch = scui_ui_res_local->snap_p0 +
                (scui_ui_res_local->snap_p1 - scui_ui_res_local->snap_p0) * k;
            scui_ui_res_local->yaw = scui_ui_res_local->snap_y0 +
                (scui_ui_res_local->snap_y1 - scui_ui_res_local->snap_y0) * k;
            if (scui_ui_res_local->snap_t >= scui_ui_res_local->snap_dur) {
                scui_ui_res_local->snap_on = false;
                /* 居中走完: 点击/编码器 -> 放大icon再跳转(保持锁定); 回正 -> 解锁 */
                if (scui_ui_res_local->snap_jump >= 0) {
                    scui_coord_t idx = scui_ui_res_local->snap_jump;
                    scui_ui_res_local->snap_jump = -1;
                    scui_area_t  widget_clip = scui_widget_area(event->object);
                    scui_ui_res_local->zoom_on   = true;
                    scui_ui_res_local->zoom_idx  = idx;
                    scui_ui_res_local->zoom_t    = 0;
                    scui_ui_res_local->zoom_dur  = 220;
                    scui_ui_res_local->zoom_size = scui_max(widget_clip.w, widget_clip.h) * 1.2f;
                    SCUI_LOG_INFO("[hb] zoom idx=%d", idx);
                } else {
                    scui_ui_res_local->move_lock = 0;
                }
            }
            scui_widget_draw(event->object, NULL, false, 0);
        }
        if (scui_ui_res_local->zoom_on) {
            scui_ui_res_local->zoom_t += event->tick;
            if (scui_ui_res_local->zoom_t >= scui_ui_res_local->zoom_dur) {
                scui_coord_t idx = scui_ui_res_local->zoom_idx;
                scui_ui_res_local->zoom_on = false;
                SCUI_LOG_INFO("[hb] jump app idx=%d", idx);
                scui_event_mask_over(event);
                scui_window_stack_jump(scui_ui_scene_list[idx].jump, 1,
                    scui_window_switch_center_out, scui_opt_dir_none, false, NULL);
            }
            scui_widget_draw(event->object, NULL, false, 0);
        }
        break;
    }
    case scui_event_enc_tick: {
        /* 编码器: 一格移动一个idx(跟方向), 对应图标动画居中 */
        if (scui_ui_res_local->touch || scui_ui_res_local->zoom_on)
            break;
        scui_coord_t way = (event->enc_way == 0) ? +1 : -1;
        scui_coord_t idx = scui_ui_res_local->focus_idx +
            way * event->enc_diff;
        idx = (idx % scui_ui_res_local->geo_num +
            scui_ui_res_local->geo_num) % scui_ui_res_local->geo_num;
        scui_ui_res_local->focus_idx = idx;
        scui_event_mask_over(event);
        scui_ui_res_local->move_lock = 1;
        scui_ui_honeycomb_snap(scui_ui_honeycomb_geo[idx].x,
            scui_ui_honeycomb_geo[idx].y, scui_ui_honeycomb_geo[idx].z,
            -1, 0.0f);
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    case scui_event_ptr_down: {

        scui_ui_res_local->down_x = event->ptr_c.x;
        scui_ui_res_local->down_y = event->ptr_c.y;
        scui_ui_res_local->last_x = event->ptr_c.x;
        scui_ui_res_local->last_y = event->ptr_c.y;
        scui_ui_res_local->touch = false;
        break;
    }
    case scui_event_ptr_move: {

        scui_event_mask_over(event);
        if (scui_ui_res_local->move_lock)
            break;

        /* 位移超阈值才视为拖动(区分点击) */
        if (!scui_ui_res_local->touch) {
            scui_coord_t dx = event->ptr_e.x - scui_ui_res_local->down_x;
            scui_coord_t dy = event->ptr_e.y - scui_ui_res_local->down_y;
            if (dx * dx + dy * dy < 16 * 16)
                break;
            scui_ui_res_local->touch = true;
            scui_ui_res_local->snap_on = false;
        }

        /* 跟手: 拖下球向下转, 拖右球向右转 */
        scui_coord3_t dx = event->ptr_e.x - scui_ui_res_local->last_x;
        scui_coord3_t dy = event->ptr_e.y - scui_ui_res_local->last_y;
        scui_ui_res_local->last_x = event->ptr_e.x;
        scui_ui_res_local->last_y = event->ptr_e.y;
        scui_ui_res_local->pitch -= dy / scui_ui_res_local->rad;
        scui_ui_res_local->yaw   += dx / scui_ui_res_local->rad;
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    case scui_event_ptr_up: {

        if (scui_ui_res_local->move_lock)
            break;

        if (scui_ui_res_local->touch) {
            /* 拖动松手: 最近焦点回正(限幅) */
            scui_ui_res_local->touch = false;
            scui_area_t  widget_clip = scui_widget_area(event->object);
            scui_coord_t widget_cx = widget_clip.x + widget_clip.w / 2;
            scui_coord_t widget_cy = widget_clip.y + widget_clip.h / 2;
            scui_coord3_t best_z = -1.0f;
            scui_coord_t best = 0;
            scui_coord3_t ox, oy, os, oz;
            for (scui_coord_t i = 0; i < scui_ui_res_local->geo_num; i++) {
                scui_ui_honeycomb_solve(scui_ui_honeycomb_geo[i].x,
                    scui_ui_honeycomb_geo[i].y, scui_ui_honeycomb_geo[i].z,
                    widget_cx, widget_cy, scui_ui_res_local->rad,
                    &ox, &oy, &os, &oz);
                if (oz > best_z) {
                    best_z = oz;
                    best = i;
                }
            }
            scui_ui_honeycomb_snap(scui_ui_honeycomb_geo[best].x,
                scui_ui_honeycomb_geo[best].y, scui_ui_honeycomb_geo[best].z,
                -1, 0.35f);
        } else {
            /* 点击: 命中最上层图标 -> 精确居中 -> 跳转 */
            scui_area_t  widget_clip = scui_widget_area(event->object);
            scui_coord_t widget_cx = widget_clip.x + widget_clip.w / 2;
            scui_coord_t widget_cy = widget_clip.y + widget_clip.h / 2;
            scui_coord_t mx = event->ptr_c.x;
            scui_coord_t my = event->ptr_c.y;
            scui_coord3_t best_z = 0.0f;
            scui_coord_t hit = -1;
            scui_coord3_t ox, oy, os, oz;
            for (scui_coord_t i = 0; i < scui_ui_res_local->geo_num; i++) {
                scui_ui_honeycomb_solve(scui_ui_honeycomb_geo[i].x,
                    scui_ui_honeycomb_geo[i].y, scui_ui_honeycomb_geo[i].z,
                    widget_cx, widget_cy, scui_ui_res_local->rad,
                    &ox, &oy, &os, &oz);
                if (oz <= 0.0f)
                    continue;
                scui_coord_t ddx = mx - ox;
                scui_coord_t ddy = my - oy;
                if (ddx >= -os / 2 && ddx <= os / 2 && ddy >= -os / 2 && ddy <= os / 2) {
                    if (oz > best_z) {
                        best_z = oz;
                        hit = i;
                    }
                }
            }
            if (hit >= 0) {
                scui_event_mask_over(event);
                scui_ui_res_local->move_lock = 1;
                scui_ui_res_local->focus_idx = hit;
                scui_ui_honeycomb_snap(scui_ui_honeycomb_geo[hit].x,
                    scui_ui_honeycomb_geo[hit].y, scui_ui_honeycomb_geo[hit].z,
                    hit, 0.0f);
            }
        }
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    case scui_event_draw_graph: {

        scui_area_t  widget_clip = scui_widget_area(event->object);
        scui_coord_t widget_cx = widget_clip.x + widget_clip.w / 2;
        scui_coord_t widget_cy = widget_clip.y + widget_clip.h / 2;

        scui_coord3_t zoom_k = 0.0f;
        if (scui_ui_res_local->zoom_on) {
            zoom_k = (scui_coord3_t)scui_ui_res_local->zoom_t /
                scui_ui_res_local->zoom_dur;
            if (zoom_k > 1.0f)
                zoom_k = 1.0f;
            zoom_k = zoom_k * zoom_k * (3.0f - 2.0f * zoom_k);
        }
        for (scui_coord_t i = 0; i < scui_ui_res_local->geo_num; i++) {
            scui_coord3_t ox, oy, os, oz;
            scui_ui_honeycomb_solve(scui_ui_honeycomb_geo[i].x,
                scui_ui_honeycomb_geo[i].y, scui_ui_honeycomb_geo[i].z,
                widget_cx, widget_cy, scui_ui_res_local->rad,
                &ox, &oy, &os, &oz);
            /* 放大图标最后单独绘制, 避免被其他图标覆盖 */
            if (scui_ui_res_local->zoom_on && i == scui_ui_res_local->zoom_idx)
                continue;
            if (oz <= 0.0f || os <= 2.0f)
                continue;
            scui_handle_t image = scui_ui_scene_list[i].image;
            scui_area_t   draw_clip = {
                .x = (scui_coord_t)floorf(ox - os / 2) - widget_clip.x,
                .y = (scui_coord_t)floorf(oy - os / 2) - widget_clip.y,
                .w = (scui_coord_t)ceilf(os),
                .h = (scui_coord_t)ceilf(os),
            };
            scui_point_t scale = {
                .x = os * SCUI_SCALE_COF / scui_image_w(image),
                .y = os * SCUI_SCALE_COF / scui_image_h(image),
            };
            scui_area_t  area_i  = scui_image_area(image);
            scui_area_t  area_w  = draw_clip;
            scui_point_t anchor = scui_area_center(&area_w);
            scui_point_t center = scui_area_center(&area_i);
            scui_widget_draw_image_scale(event->object, &draw_clip, image, NULL,
                SCUI_COLOR_UNUSED, anchor, center, scale);
        }
        /* 放大icon最后单独绘制(最上层, 不被其他图标覆盖) */
        if (scui_ui_res_local->zoom_on) {
            scui_coord_t i = scui_ui_res_local->zoom_idx;
            scui_coord3_t ox, oy, os, oz;
            scui_ui_honeycomb_solve(scui_ui_honeycomb_geo[i].x,
                scui_ui_honeycomb_geo[i].y, scui_ui_honeycomb_geo[i].z,
                widget_cx, widget_cy, scui_ui_res_local->rad,
                &ox, &oy, &os, &oz);
            os = os + (scui_ui_res_local->zoom_size - os) * zoom_k;
            if (oz > 0.0f && os > 2.0f) {
                scui_handle_t image = scui_ui_scene_list[i].image;
                scui_area_t   draw_clip = {
                    .x = (scui_coord_t)floorf(ox - os / 2) - widget_clip.x,
                    .y = (scui_coord_t)floorf(oy - os / 2) - widget_clip.y,
                    .w = (scui_coord_t)ceilf(os),
                    .h = (scui_coord_t)ceilf(os),
                };
                scui_point_t scale = {
                    .x = os * SCUI_SCALE_COF / scui_image_w(image),
                    .y = os * SCUI_SCALE_COF / scui_image_h(image),
                };
                scui_area_t  area_i  = scui_image_area(image);
                scui_area_t  area_w  = draw_clip;
                scui_point_t anchor = scui_area_center(&area_w);
                scui_point_t center = scui_area_center(&area_i);
                scui_widget_draw_image_scale(event->object, &draw_clip, image, NULL,
                    SCUI_COLOR_UNUSED, anchor, center, scale);
            }
        }
        if (!scui_ui_res_local->drawn) {
            scui_ui_res_local->drawn = true;
            SCUI_LOG_INFO("[hb] draw p=%.2f y=%.2f num=%d",
                scui_ui_res_local->pitch, scui_ui_res_local->yaw,
                scui_ui_res_local->geo_num);
        }
        break;
    }
    default:
        break;
    }
}
