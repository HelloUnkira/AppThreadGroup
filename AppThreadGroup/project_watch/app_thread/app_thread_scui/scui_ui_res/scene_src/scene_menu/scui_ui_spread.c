/*实现目标:
 *    主题:扩散(无限环轴平移)
 *    每圈8个, 奇偶圈错位半角(13579/2468), 圈间距x_gap固定
 *    圈k半径 r=k*x_gap-offset (k可负, 无0圈), 永续循环
 *    整数轴: AXIS=0x80000000, 圈轴位置=AXIS+k*x_gap, 屏幕r=(圈轴-中心轴)
 *    放大(offset+): 负圈从中心死区外冒出, 正圈出屏
 *    缩小(offset-): 外圈从屏幕边缘进入
 *    径向跟手拖动=offset; 松手吸附最近整圈; 编码器一次跳两环(一圈)
 *    中心死区r<dead无条件隐藏; 图标随径向远大小近小
 *    轻点图标: 居中放大动画后跳转
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 整数轴中心: 圈轴位置=AXIS+k*x_gap, k为int32可负, AXIS大使其永不下溢 */
#define SPREAD_AXIS                 0x80000000u

static struct {
    int32_t       offset;       /* 环轴偏移(px, 可负) */
    int           x_gap;        /* 圈间距 */
    int           dead;         /* 中心死区半径 */
    int           rad;          /* 屏半径(短边一半) */

    bool          touch;        /* 跟手拖动中 */
    int           down_r;       /* 按下起点径向 */
    int32_t       down_offset;  /* 按下起点offset */
    scui_coord_t  down_x, down_y;

    /* offset 吸附/编码器动画 */
    bool          snap;
    scui_coord_t  snap_t;
    int32_t       snap_from, snap_to;
    scui_coord_t  snap_dur;

    /* 点击居中动画 */
    bool          focus;
    scui_coord_t  focus_t;
    scui_coord_t  focus_k, focus_a;
    scui_coord_t  focus_dur;
    bool          move_lock;
} * scui_ui_res_local = NULL;

/*@brief 解算圈k角a的图标(控件内坐标)
 */
static void scui_ui_spread_solve(int k, int a, int cx, int cy,
    scui_coord3_t *out_x, scui_coord3_t *out_y, scui_coord3_t *out_wh,
    int *out_r, scui_coord_t *out_idx)
{
    /* 整数轴: 中心轴=AXIS-offset, 圈轴=AXIS+k*x_gap, 屏幕r=圈轴-中心轴=k*x_gap+offset */
    int r = k * scui_ui_res_local->x_gap + scui_ui_res_local->offset;
    int abs_k = k < 0 ? -k : k;
    /* 奇偶错位半角: 奇圈0, 偶圈22.5 (浮点, 像素级平滑) */
    scui_coord3_t ang = (scui_coord3_t)a * 45.0f + ((abs_k & 1) ? 0.0f : 22.5f);
    scui_coord3_t rad = SCUI_RAD_BY_A(ang);
    *out_x = (scui_coord3_t)cx + scui_cos(rad) * (scui_coord3_t)r;
    *out_y = (scui_coord3_t)cy + scui_sin(rad) * (scui_coord3_t)r;
    *out_r = r;
    /* 越远越大, 越近越小 */
    *out_wh = 34.0f + (scui_coord3_t)r * 0.14f;
    *out_idx = scui_mabs(k * 8 + a, (int64_t)scui_ui_scene_list_num);
}

/*@brief 窗口控件事件响应
 */
void scui_ui_scene_spread_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create:
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);

        scui_ui_res_local->offset      = 0;
        scui_ui_res_local->touch       = false;
        scui_ui_res_local->snap        = false;
        scui_ui_res_local->focus       = false;
        scui_ui_res_local->move_lock   = false;
        scui_ui_res_local->snap_dur    = 200;
        scui_ui_res_local->focus_dur   = 300;

        scui_area_t widget_clip = scui_widget_area(event->object);
        scui_ui_res_local->rad   = scui_min(widget_clip.w, widget_clip.h) / 2;
        scui_ui_res_local->x_gap = scui_ui_res_local->rad * 27 / 100;
        scui_ui_res_local->dead  = scui_ui_res_local->rad / 12;
        scui_ui_scene_list_cfg(scui_ui_scene_list_type_spread);
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

/*@brief 控件事件响应
 */
void scui_ui_scene_spread_custom_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_anima_elapse: {

        /* offset 吸附/编码器动画 */
        if (scui_ui_res_local->snap) {
            scui_ui_res_local->snap_t += event->tick;
            scui_coord3_t k = (scui_coord3_t)scui_ui_res_local->snap_t / scui_ui_res_local->snap_dur;
            if (k > 1.0f) k = 1.0f;
            k = k * k * (3.0f - 2.0f * k);
            scui_ui_res_local->offset = scui_ui_res_local->snap_from +
                (int32_t)((scui_coord3_t)(scui_ui_res_local->snap_to - scui_ui_res_local->snap_from) * k);
            if (scui_ui_res_local->snap_t >= scui_ui_res_local->snap_dur)
                scui_ui_res_local->snap = false;
            scui_widget_draw(event->object, NULL, false, 0);
        }
        /* 点击居中动画 */
        if (scui_ui_res_local->focus) {
            scui_ui_res_local->focus_t += event->tick;
            scui_coord3_t p = (scui_coord3_t)scui_ui_res_local->focus_t / scui_ui_res_local->focus_dur;
            if (p > 1.0f) p = 1.0f;
            p = p * p * (3.0f - 2.0f * p);
            scui_widget_draw(event->object, NULL, false, 0);
            if (scui_ui_res_local->focus_t >= scui_ui_res_local->focus_dur) {
                scui_ui_res_local->focus = false;
                scui_ui_res_local->move_lock = false;
                scui_coord_t idx = scui_mabs(scui_ui_res_local->focus_k * 8 + scui_ui_res_local->focus_a,
                    (int64_t)scui_ui_scene_list_num);
                scui_event_mask_over(event);
                scui_window_stack_jump(scui_ui_scene_list[idx].jump, 1,
                    scui_window_switch_center_out, scui_opt_dir_none, false, NULL);
            }
        }
        break;
    }
    case scui_event_ptr_down: {

        scui_event_mask_over(event);
        scui_ui_res_local->touch = false;
        scui_ui_res_local->snap = false;
        scui_area_t  widget_clip = scui_widget_area(event->object);
        int cx = widget_clip.w / 2;
        int cy = widget_clip.h / 2;
        scui_point_t p = event->ptr_c;
        scui_widget_switch_point(event->object, &p);
        scui_ui_res_local->down_x = p.x;
        scui_ui_res_local->down_y = p.y;
        scui_ui_res_local->down_r = (int)hypotf((scui_coord3_t)(p.x - cx), (scui_coord3_t)(p.y - cy));
        scui_ui_res_local->down_offset = scui_ui_res_local->offset;
        break;
    }
    case scui_event_ptr_move: {

        scui_event_mask_over(event);
        if (scui_ui_res_local->move_lock)
            break;
        scui_area_t  widget_clip = scui_widget_area(event->object);
        int cx = widget_clip.w / 2;
        int cy = widget_clip.h / 2;
        scui_point_t p = event->ptr_e;
        scui_widget_switch_point(event->object, &p);

        /* 未过阈值不启动拖动 */
        if (!scui_ui_res_local->touch) {
            scui_coord_t dx = p.x - scui_ui_res_local->down_x;
            scui_coord_t dy = p.y - scui_ui_res_local->down_y;
            if (dx * dx + dy * dy < 16 * 16)
                break;
            scui_ui_res_local->touch = true;
        }
        int r = (int)hypotf((scui_coord3_t)(p.x - cx), (scui_coord3_t)(p.y - cy));
        /* 径向跟手: 往外(r增)=offset+ */
        scui_ui_res_local->offset = scui_ui_res_local->down_offset + (r - scui_ui_res_local->down_r);
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    case scui_event_ptr_up: {

        if (scui_ui_res_local->move_lock)
            break;
        scui_event_mask_over(event);

        if (scui_ui_res_local->touch) {
            /* 拖动松手: 吸附到最近整圈 */
            scui_ui_res_local->touch = false;
            int32_t snap_near = (int32_t)((scui_ui_res_local->offset + scui_ui_res_local->x_gap / 2) / scui_ui_res_local->x_gap)
                * scui_ui_res_local->x_gap;
            scui_ui_res_local->snap = true;
            scui_ui_res_local->snap_t = 0;
            scui_ui_res_local->snap_from = scui_ui_res_local->offset;
            scui_ui_res_local->snap_to = snap_near;
        } else {
            /* 轻点: 命中最上层图标 -> 居中 */
            scui_area_t  widget_clip = scui_widget_area(event->object);
            int cx = widget_clip.w / 2;
            int cy = widget_clip.h / 2;
            scui_point_t p = event->ptr_c;
            scui_widget_switch_point(event->object, &p);
            scui_coord_t hit = -1, hit_k = 0, hit_a = 0;
            scui_coord3_t best = 1e9f;
            int max_r = scui_ui_res_local->rad;
            /* r=k*x_gap+offset in [0,max_r] -> k in [-offset/x_gap, (max_r-offset)/x_gap] */
            int k_min = -scui_ui_res_local->offset / scui_ui_res_local->x_gap;
            int k_max = (max_r - scui_ui_res_local->offset) / scui_ui_res_local->x_gap;
            for (int k = k_min; k <= k_max; k++) {
                for (int a = 0; a < 8; a++) {
                    scui_coord3_t sx, sy, sw; int sr; scui_coord_t si;
                    scui_ui_spread_solve(k, a, cx, cy, &sx, &sy, &sw, &sr, &si);
                    /* 圆监管: 整图标在死区内或屏圆外则不参与命中 */
                    if (sr + sw/2 < scui_ui_res_local->dead) continue;
                    if (sr - sw/2 > scui_ui_res_local->rad) continue;
                    scui_coord3_t ddx = p.x - sx, ddy = p.y - sy;
                    scui_coord3_t dd = hypotf(ddx, ddy);
                    if (dd < sw / 2 && dd < best) {
                        best = dd; hit = si; hit_k = (scui_coord_t)k; hit_a = (scui_coord_t)a;
                    }
                }
            }
            if (hit >= 0) {
                scui_ui_res_local->focus = true;
                scui_ui_res_local->focus_t = 0;
                scui_ui_res_local->focus_k = hit_k;
                scui_ui_res_local->focus_a = hit_a;
                scui_ui_res_local->move_lock = true;
            }
        }
        scui_widget_draw(event->object, NULL, false, 0);
        break;
    }
    case scui_event_enc_tick: {

        scui_event_mask_over(event);
        /* 编码器: 一次跳两环(一圈), 平滑动画; 动画中累加到同一目标 */
        int32_t step = ((event->enc_way == 0) ? 1 : -1) * 2 * scui_ui_res_local->x_gap;
        if (scui_ui_res_local->snap) {
            scui_ui_res_local->snap_from = scui_ui_res_local->offset;
            scui_ui_res_local->snap_to += step;
        } else {
            scui_ui_res_local->snap = true;
            scui_ui_res_local->snap_t = 0;
            scui_ui_res_local->snap_from = scui_ui_res_local->offset;
            scui_ui_res_local->snap_to = scui_ui_res_local->offset + step;
        }
        break;
    }
    case scui_event_draw_graph: {

        scui_area_t  widget_clip = scui_widget_area(event->object);
        /* 与 honeycomb 同基准: 屏幕绝对中心算坐标, draw_clip 再减回控件原点 */
        int cx = widget_clip.x + widget_clip.w / 2;
        int cy = widget_clip.y + widget_clip.h / 2;
        int max_r = scui_ui_res_local->rad;
        scui_handle_t icon0 = scui_ui_scene_list[0].image;
        scui_coord_t  img_w = scui_image_w(icon0);
        scui_coord_t  img_h = scui_image_h(icon0);

        /* 点击居中动画进度 */
        scui_coord3_t fp = 0.0f;
        if (scui_ui_res_local->focus) {
            fp = (scui_coord3_t)scui_ui_res_local->focus_t / scui_ui_res_local->focus_dur;
            if (fp > 1.0f) fp = 1.0f;
            fp = fp * fp * (3.0f - 2.0f * fp);
        }

        int k_min = (-scui_ui_res_local->offset - 120) / scui_ui_res_local->x_gap;
        int k_max = (max_r - scui_ui_res_local->offset + 120) / scui_ui_res_local->x_gap;
        for (int k = k_min; k <= k_max; k++) {
            for (int a = 0; a < 8; a++) {
                scui_coord3_t sx, sy, sw; int sr; scui_coord_t si;
                scui_ui_spread_solve(k, a, cx, cy, &sx, &sy, &sw, &sr, &si);
                /* 圆监管: 整图标在死区内 或 完全在屏圆外则不画 */
                if (sr + sw/2 < scui_ui_res_local->dead) continue;
                if (sr - sw/2 > scui_ui_res_local->rad) continue;

                /* 居中动画: 被点图标滑到中心放大 */
                if (scui_ui_res_local->focus && k == scui_ui_res_local->focus_k
                    && a == scui_ui_res_local->focus_a) {
                    scui_coord3_t target = scui_ui_res_local->rad * 0.7f;
                    scui_coord3_t fx = sx + (cx - sx) * fp;
                    scui_coord3_t fy = sy + (cy - sy) * fp;
                    scui_coord3_t fw = sw + (target - sw) * fp;
                    /* clip 向外取整: 左上floor 右上ceil, 保证放大区域完整 */
                    /* target 比图像实际占据区多扩余量(draw内部角变换带±1.5px), 否则求交裁边 */
                    scui_coord_t dw1 = (scui_coord_t)ceilf(fw) + 6;
                    scui_area_t dc = {
                        .x = (scui_coord_t)floorf(fx - fw / 2) - 3 - widget_clip.x,
                        .y = (scui_coord_t)floorf(fy - fw / 2) - 3 - widget_clip.y,
                        .w = dw1, .h = dw1,
                    };
                    scui_point_t sc = {
                        .x = fw * SCUI_SCALE_COF / img_w,
                        .y = fw * SCUI_SCALE_COF / img_h,
                    };
                    scui_area_t  area_i  = scui_image_area(scui_ui_scene_list[si].image);
                    scui_area_t  area_w  = dc;
                    scui_point_t anchor = scui_area_center(&area_w);
                    scui_point_t center = scui_area_center(&area_i);
                    scui_widget_draw_image_scale(event->object, &dc,
                        scui_ui_scene_list[si].image, NULL, SCUI_COLOR_UNUSED, anchor, center, sc);
                    continue;
                }

                /* clip 向外取整: 左上floor 右上ceil, 保证放大区域完整 */
                /* target 比图像实际占据区多扩余量(draw内部角变换带±1.5px), 否则求交裁边 */
                scui_coord_t dw2 = (scui_coord_t)ceilf(sw) + 6;
                scui_area_t dc = {
                    .x = (scui_coord_t)floorf(sx - sw / 2) - 3 - widget_clip.x,
                    .y = (scui_coord_t)floorf(sy - sw / 2) - 3 - widget_clip.y,
                    .w = dw2, .h = dw2,
                };
                scui_point_t sc = {
                    .x = sw * SCUI_SCALE_COF / img_w,
                    .y = sw * SCUI_SCALE_COF / img_h,
                };
                scui_area_t  area_i  = scui_image_area(scui_ui_scene_list[si].image);
                scui_area_t  area_w  = dc;
                scui_point_t anchor = scui_area_center(&area_w);
                scui_point_t center = scui_area_center(&area_i);
                scui_widget_draw_image_scale(event->object, &dc,
                    scui_ui_scene_list[si].image, NULL, SCUI_COLOR_UNUSED, anchor, center, sc);
            }
        }
        break;
    }
    default:
        break;
    }
}
