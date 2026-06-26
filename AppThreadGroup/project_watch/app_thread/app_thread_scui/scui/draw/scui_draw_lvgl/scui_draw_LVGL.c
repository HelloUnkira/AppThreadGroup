/*Implementation Goals:
 *    Basic primitive drawing (LVGL lv_draw_sw render pipeline port)
 *
 * Design Philosophy:
 *    1. Mask outline + Blend coloring: generate alpha outline via mask, then fill with blend
 *    2. Region draw mode: no line scanning, compute mask for 2D region directly, blend once
 *    3. Outline surface rendering: mask_to_surface renders mask outline to alpha-format surface
 *    4. Code style: inline as much as possible into one function, avoid unnecessary wrappers
 *
 * File Structure:
 *    - Mask system (line/angle/radius mask + AA + global management)
 *    - Blend system (fill/map × normal/additive)
 *    - Primitive drawing (line/circle/crect/arc)
 *    - Main entry (scui_draw_ctx_graph_LVGL)
 */
#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 2:WARN */

#include "scui.h"

/*
 * ===== 内存越界调试配置 =====
 * SCUI_DRAW_CANARY_CHECK:  启用 mask_buf 释放前的 canary 校验 (默认 ON)
 * SCUI_DRAW_OVERFLOW_CHECK: 启用 draw_w * draw_h 溢出检查 (默认 ON)
 * SCUI_DRAW_MASK_ARRAY_BOUND: 启用 mask list 遍历时的数组边界检查 (默认 ON)
 * SCUI_DRAW_BOUNDS_LOG:    启用边界/参数调试日志 (仅在 DEBUG 级别生效)
 */
#ifndef SCUI_DRAW_CANARY_CHECK
#define SCUI_DRAW_CANARY_CHECK      1   /* 保留 canary 安全检查 */
#endif
#ifndef SCUI_DRAW_OVERFLOW_CHECK
#define SCUI_DRAW_OVERFLOW_CHECK    0   /* 稳定后关闭详细日志 */
#endif
#ifndef SCUI_DRAW_MASK_ARRAY_BOUND
#define SCUI_DRAW_MASK_ARRAY_BOUND  1   /* 保留越界检查 */
#endif
#ifndef SCUI_DRAW_BOUNDS_LOG
#define SCUI_DRAW_BOUNDS_LOG        0   /* 稳定后关闭边界调试 */
#endif
#if defined(LVGL_VERSION_MAJOR) || defined(LVGL_VERSION_MINOR) || defined(LVGL_VERSION_PATCH)
#error "do not import any lvgl header files"
#endif
/* Trig function adapter: LVGL LV_TRIGO_SHIFT=15 -> SCUI scui_sin4096 shift=12 */
#define LV_TRIGO_SHIFT              12
#define LV_TRIGO_SIN_MAX            4095
#define lv_trigo_sin(angle)         scui_sin4096(angle)
#define lv_trigo_cos(angle)         scui_sin4096((angle) + 90)
#define LV_MASK_MAX_ID              16
#define LV_MASK_ID_INV              (-1)
/* Mask buffer element type: always 1 byte (LVGL original uses uint8_t) */
typedef uint8_t lv_mask_alpha_t;
/* mask_buf_set_zero: 将 mask_buf 从 offset 开始的 n 个元素置0
 * mask_buf 使用 lv_mask_alpha_t (uint8_t)，每个元素1字节，直接 memset */
#define mask_buf_set_zero(buf, offset, n)  memset((buf) + (offset), 0, (size_t)(n))
/*****************************************************************************/
/* ======================== Mask Type Definitions (Internal) =============== */
/*****************************************************************************/
/* Mask result codes */
enum {
    LV_DRAW_MASK_RES_TRANSP = 0,
    LV_DRAW_MASK_RES_FULL_COVER,
    LV_DRAW_MASK_RES_CHANGED,
    LV_DRAW_MASK_RES_UNKNOWN
};
typedef uint8_t lv_draw_mask_res_t;
/* Mask types */
enum {
    LV_DRAW_MASK_TYPE_LINE,
    LV_DRAW_MASK_TYPE_ANGLE,
    LV_DRAW_MASK_TYPE_RADIUS,
};
typedef uint8_t lv_draw_mask_type_t;
/* Line mask side */
enum {
    LV_DRAW_MASK_LINE_SIDE_LEFT = 0,
    LV_DRAW_MASK_LINE_SIDE_RIGHT,
    LV_DRAW_MASK_LINE_SIDE_TOP,
    LV_DRAW_MASK_LINE_SIDE_BOTTOM,
};
typedef uint8_t lv_draw_mask_line_side_t;
/* Region Mask callback: compute mask directly for a 2D region */
typedef lv_draw_mask_res_t (*lv_draw_mask_region_cb_t)(
    lv_mask_alpha_t *mask_buf, const scui_area_t *area, scui_coord_t stride, void *p);
/* Mask common descriptor */
typedef struct {
    lv_draw_mask_region_cb_t cb;
    lv_draw_mask_type_t      type;
} lv_draw_mask_common_dsc_t;
/* Line Mask parameters */
typedef struct {
    lv_draw_mask_common_dsc_t dsc;
    struct {
        scui_point_t p1;
        scui_point_t p2;
        lv_draw_mask_line_side_t side:2;
    } cfg;
    scui_point_t origo;
    int32_t xy_steep;
    int32_t yx_steep;
    int32_t steep;
    int32_t spx;
    uint8_t flat:1;
    uint8_t inv:1;
} lv_draw_mask_line_param_t;
/* Angle Mask parameters */
typedef struct {
    lv_draw_mask_common_dsc_t dsc;
    struct {
        scui_point_t vertex_p;
        scui_coord_t start_angle;
        scui_coord_t end_angle;
    } cfg;
    lv_draw_mask_line_param_t start_line;
    lv_draw_mask_line_param_t end_line;
    uint16_t delta_deg;
} lv_draw_mask_angle_param_t;
/* Radius Mask circle data */
typedef struct {
    uint8_t *buf;
    lv_mask_alpha_t *cir_opa; /* uint8_t — 与 LVGL 原设计一致 */
    uint16_t *x_start_on_y;
    uint16_t *opa_start_on_y;
    scui_coord_t radius;
} lv_draw_mask_radius_circle_dsc_t;
/* Radius Mask parameters */
typedef struct {
    lv_draw_mask_common_dsc_t dsc;
    struct {
        scui_area_t rect;
        scui_coord_t radius;
        uint8_t outer:1;
    } cfg;
    lv_draw_mask_radius_circle_dsc_t *circle;
} lv_draw_mask_radius_param_t;
/* Mask global state */
typedef struct {
    lv_draw_mask_common_dsc_t *param;
    void *custom_id;
} lv_draw_mask_saved_t;
/*****************************************************************************/
/* Global draw context */
typedef struct {
    uint8_t           *buf;
    scui_area_t        buf_area;
    const scui_area_t *clip_area;
    scui_surface_t    *dst_surface;  /* 用于 scui_draw_area_blend */
} lv_draw_sw_ctx_t;
/*****************************************************************************/
/* ======================== Internal Constants ============================= */
/*****************************************************************************/
/* Arc split thresholds (ported from lv_draw_sw_arc.c) */
#define SPLIT_RADIUS_LIMIT      10
#define SPLIT_ANGLE_GAP_LIMIT   60
/* Blend opacity constants */
#define LV_OPA_MIN       scui_alpha_trans
#define LV_OPA_MAX       scui_alpha_cover
/* Line width correction table for diagonal lines (ported from lv_draw_sw_line.c) */
static const uint8_t lv_draw_line_wcorr[] = {
    128, 128, 128, 129, 129, 130, 130, 131,
    132, 133, 134, 135, 137, 138, 140, 141,
    143, 145, 147, 149, 151, 153, 155, 158,
    160, 162, 165, 167, 170, 173, 175, 178,
    181,
};
/*****************************************************************************/
/* ======================== Mask Global State ============================== */
/*****************************************************************************/
static lv_draw_mask_saved_t lv_draw_mask_list[LV_MASK_MAX_ID];
/*****************************************************************************/
/* ======================== Mask Forward Declarations ====================== */
/*****************************************************************************/
/* Region mask callbacks (used before full definition) */
static lv_draw_mask_res_t lv_draw_mask_line_region(
    lv_mask_alpha_t *mask_buf, const scui_area_t *area, scui_coord_t stride,
    lv_draw_mask_line_param_t *p);
static lv_draw_mask_res_t lv_draw_mask_angle_region(
    lv_mask_alpha_t *mask_buf, const scui_area_t *area, scui_coord_t stride,
    lv_draw_mask_angle_param_t *p);
static lv_draw_mask_res_t lv_draw_mask_radius_region(
    lv_mask_alpha_t *mask_buf, const scui_area_t *area, scui_coord_t stride,
    lv_draw_mask_radius_param_t *p);
/* Line mask sub-functions (used before full definition) */
static lv_draw_mask_res_t lv_draw_line_mask_flat(lv_draw_mask_line_param_t *p,
    lv_mask_alpha_t *mask_buf, scui_coord_t abs_x, scui_coord_t abs_y, scui_coord_t len);
static lv_draw_mask_res_t lv_draw_line_mask_steep(lv_draw_mask_line_param_t *p,
    lv_mask_alpha_t *mask_buf, scui_coord_t abs_x, scui_coord_t abs_y, scui_coord_t len);
/* Single-line mask calculation (used before full definition) */
static lv_draw_mask_res_t lv_draw_mask_line_row(
    lv_draw_mask_line_param_t *p,
    lv_mask_alpha_t *mask_buf, scui_coord_t abs_x, scui_coord_t abs_y, scui_coord_t len);
/* Circle helpers */
static void lv_draw_circle_init(scui_point_t *c, scui_coord_t *tmp, scui_coord_t radius);
static bool lv_draw_circle_cont(scui_point_t *c);
static void lv_draw_circle_next(scui_point_t *c, scui_coord_t *tmp);
static void lv_draw_circle_calc_aa4(lv_draw_mask_radius_circle_dsc_t *c, scui_coord_t radius);
static lv_mask_alpha_t *lv_draw_mask_radius_get_next_line(lv_draw_mask_radius_circle_dsc_t *c,
    scui_coord_t y, scui_coord_t *len, scui_coord_t *x_start);
static inline scui_alpha_t lv_draw_mask_mix(scui_alpha_t mask_act, scui_alpha_t mask_new);
/*****************************************************************************/
/* ======================== Mask Global Management ========================= */
/*****************************************************************************/
static int16_t lv_draw_mask_add(void *param, void *custom_id)
{
    uint8_t i;
    for (i = 0; i < LV_MASK_MAX_ID; i++) {
        if (lv_draw_mask_list[i].param == NULL) break;
    }
    if (i >= LV_MASK_MAX_ID) {
        SCUI_LOG_WARN("mask add: no place (all %d slots used)", LV_MASK_MAX_ID);
        return LV_MASK_ID_INV;
    }
    lv_draw_mask_list[i].param = param;
    lv_draw_mask_list[i].custom_id = custom_id;
#if SCUI_DRAW_BOUNDS_LOG
    {
        int mask_count = 0;
        for (int mi = 0; mi < LV_MASK_MAX_ID; mi++)
            if (lv_draw_mask_list[mi].param) mask_count++;
        SCUI_LOG_DEBUG("mask add: slot=%d type=%d total=%d",
            (int)i,
            (int)((lv_draw_mask_common_dsc_t*)param)->type,
            mask_count);
    }
#endif
#if SCUI_MEM_SENTRY_CHECK
    SCUI_LOG_DEBUG("mask add: sentry after add slot=%d", (int)i);
    scui_mem_sentry_check();
    SCUI_LOG_DEBUG("mask add: sentry after add slot=%d OK", (int)i);
#endif
    return i;
}
/* Region mode: apply all masks to a 2D region at once */
static lv_draw_mask_res_t lv_draw_mask_apply_region(
    lv_mask_alpha_t *mask_buf, const scui_area_t *area, scui_coord_t stride)
{
    bool changed = false;
#if SCUI_DRAW_BOUNDS_LOG
    {
        int mask_count = 0;
        for (int mi = 0; mi < LV_MASK_MAX_ID; mi++)
            if (lv_draw_mask_list[mi].param) mask_count++;
        SCUI_LOG_DEBUG("mask_apply_region: area=(%d,%d,%d,%d) stride=%d active_masks=%d",
            (int)area->x, (int)area->y, (int)area->w, (int)area->h,
            (int)stride, mask_count);
    }
#endif
    lv_draw_mask_saved_t *m = lv_draw_mask_list;
#if SCUI_DRAW_MASK_ARRAY_BOUND
    lv_draw_mask_saved_t *m_end = m + LV_MASK_MAX_ID;
    while (m < m_end && m->param) {
        lv_draw_mask_common_dsc_t *dsc = m->param;
        /* 校验 dsc 指针在合理范围（防御已损坏的 param） */
        if ((uintptr_t)dsc < 0x1000 || (dsc->type > LV_DRAW_MASK_TYPE_RADIUS)) {
            SCUI_LOG_ERROR("mask list corruption at idx=%d, param=%p, type=%d",
                (int)(m - lv_draw_mask_list), (void*)dsc,
                (dsc->type <= LV_DRAW_MASK_TYPE_RADIUS) ? dsc->type : -1);
            return LV_DRAW_MASK_RES_TRANSP;
        }
#if SCUI_DRAW_BOUNDS_LOG
        SCUI_LOG_DEBUG("mask_apply_region: executing mask[%d] type=%d param=%p",
            (int)(m - lv_draw_mask_list), (int)dsc->type, (void*)m->param);
#endif
        lv_draw_mask_res_t res = dsc->cb(mask_buf, area, stride, m->param);
#if SCUI_DRAW_BOUNDS_LOG
        /* 统计 mask_buf 内容（执行 mask 后） */
        {
            uint32_t size = (uint32_t)area->w * (uint32_t)area->h;
            uint32_t n_zeros = 0, n_255 = 0, n_other = 0;
            uint32_t i;
            for (i = 0; i < size; i++) {
                if (mask_buf[i] == 0) n_zeros++;
                else if (mask_buf[i] == 255) n_255++;
                else n_other++;
            }
            SCUI_LOG_DEBUG("mask_apply_region: mask[%d] type=%d result=%d stats: zeros=%u ones=%u other=%u",
                (int)(m - lv_draw_mask_list), (int)dsc->type, (int)res,
                (unsigned)n_zeros, (unsigned)n_255, (unsigned)n_other);
        }
#endif
        if (res == LV_DRAW_MASK_RES_TRANSP) return LV_DRAW_MASK_RES_TRANSP;
        else if (res == LV_DRAW_MASK_RES_CHANGED) changed = true;
        m++;
    }
    /* 如果遍历完 16 个槽位后 param 仍非 NULL，说明列表损坏 */
    if (m >= m_end && m->param) {
        SCUI_LOG_ERROR("mask list overflow: %d entries, trailing param=%p",
            LV_MASK_MAX_ID, (void*)m->param);
        return LV_DRAW_MASK_RES_TRANSP;
    }
#else
    while (m->param) {
        lv_draw_mask_common_dsc_t *dsc = m->param;
        lv_draw_mask_res_t res = dsc->cb(mask_buf, area, stride, m->param);
        if (res == LV_DRAW_MASK_RES_TRANSP) return LV_DRAW_MASK_RES_TRANSP;
        else if (res == LV_DRAW_MASK_RES_CHANGED) changed = true;
        m++;
    }
#endif
    return changed ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
}
static void *lv_draw_mask_remove_id(int16_t id)
{
    lv_draw_mask_common_dsc_t *p = NULL;
    if (id != LV_MASK_ID_INV) {
        if (id >= LV_MASK_MAX_ID) {
            SCUI_LOG_ERROR("mask remove: invalid id=%d (max=%d)", (int)id, LV_MASK_MAX_ID);
            return NULL;
        }
        p = lv_draw_mask_list[id].param;
#if SCUI_DRAW_BOUNDS_LOG
        SCUI_LOG_DEBUG("mask remove: slot=%d type=%d param=%p",
            (int)id, p ? (int)p->type : -1, (void*)p);
#endif
        lv_draw_mask_list[id].param = NULL;
        lv_draw_mask_list[id].custom_id = NULL;
#if SCUI_MEM_SENTRY_CHECK
        SCUI_LOG_DEBUG("mask remove: sentry after remove slot=%d", (int)id);
        scui_mem_sentry_check();
        SCUI_LOG_DEBUG("mask remove: sentry after remove slot=%d OK", (int)id);
#endif
    }
    return p;
}
static void lv_draw_mask_free_param(void *p)
{
    lv_draw_mask_common_dsc_t *pdsc = p;
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("mask free_param: type=%d param=%p",
        (int)pdsc->type, (void*)p);
#endif
    if (pdsc->type == LV_DRAW_MASK_TYPE_RADIUS) {
        lv_draw_mask_radius_param_t *radius_p = (lv_draw_mask_radius_param_t *)p;
        if (radius_p->circle) {
#if SCUI_DRAW_BOUNDS_LOG
            SCUI_LOG_DEBUG("mask free_param radius: circle=%p buf=%p radius=%d",
                (void*)radius_p->circle, (void*)radius_p->circle->buf,
                (int)radius_p->circle->radius);
#endif
#if SCUI_MEM_SENTRY_CHECK
            SCUI_LOG_DEBUG("mask free_param: sentry before circle free");
            scui_mem_sentry_check();
            SCUI_LOG_DEBUG("mask free_param: sentry before circle free OK");
#endif
            SCUI_MEM_FREE(radius_p->circle->buf);
#if SCUI_MEM_SENTRY_CHECK
            SCUI_LOG_DEBUG("mask free_param: sentry after circle->buf free, before circle free");
            scui_mem_sentry_check();
            SCUI_LOG_DEBUG("mask free_param: sentry after circle->buf free OK");
#endif
            SCUI_MEM_FREE(radius_p->circle);
        }
    }
}
/*****************************************************************************/
/* ======================== Line Mask ====================================== */
/*****************************************************************************/
static void lv_draw_mask_line_points_init(lv_draw_mask_line_param_t *param,
    scui_coord_t p1x, scui_coord_t p1y, scui_coord_t p2x, scui_coord_t p2y,
    lv_draw_mask_line_side_t side)
{
    memset(param, 0, sizeof(lv_draw_mask_line_param_t));
    if (p1y == p2y && side == LV_DRAW_MASK_LINE_SIDE_BOTTOM) {
        p1y--; p2y--;
    }
    if (p1y > p2y) {
        scui_coord_t t;
        t = p2x; p2x = p1x; p1x = t;
        t = p2y; p2y = p1y; p1y = t;
    }
    param->cfg.p1.x = p1x; param->cfg.p1.y = p1y;
    param->cfg.p2.x = p2x; param->cfg.p2.y = p2y;
    param->cfg.side = side;
    param->origo.x = p1x; param->origo.y = p1y;
    param->flat = (scui_abs(p2x - p1x) > scui_abs(p2y - p1y)) ? 1 : 0;
    param->yx_steep = 0; param->xy_steep = 0;
    param->dsc.cb = (lv_draw_mask_region_cb_t)lv_draw_mask_line_region;
    param->dsc.type = LV_DRAW_MASK_TYPE_LINE;
    int32_t dx = p2x - p1x;
    int32_t dy = p2y - p1y;
    if (param->flat) {
        int32_t m;
        if (dx) { m = (1L << 20) / dx; param->yx_steep = (m * dy) >> 10; }
        if (dy) { m = (1L << 20) / dy; param->xy_steep = (m * dx) >> 10; }
        param->steep = param->yx_steep;
    } else {
        int32_t m;
        if (dy) { m = (1L << 20) / dy; param->xy_steep = (m * dx) >> 10; }
        if (dx) { m = (1L << 20) / dx; param->yx_steep = (m * dy) >> 10; }
        param->steep = param->xy_steep;
    }
    if (param->cfg.side == LV_DRAW_MASK_LINE_SIDE_LEFT) param->inv = 0;
    else if (param->cfg.side == LV_DRAW_MASK_LINE_SIDE_RIGHT) param->inv = 1;
    else if (param->cfg.side == LV_DRAW_MASK_LINE_SIDE_TOP) {
        param->inv = (param->steep > 0) ? 1 : 0;
    } else if (param->cfg.side == LV_DRAW_MASK_LINE_SIDE_BOTTOM) {
        param->inv = (param->steep > 0) ? 0 : 1;
    }
    param->spx = param->steep >> 2;
    if (param->steep < 0) param->spx = -param->spx;
}
static void lv_draw_mask_line_angle_init(lv_draw_mask_line_param_t *param,
    scui_coord_t p1x, scui_coord_t py, int16_t angle,
    lv_draw_mask_line_side_t side)
{
    if (angle > 180) angle -= 180;
    int32_t p2x = (lv_trigo_sin(angle + 90) >> 5) + p1x;
    int32_t p2y = (lv_trigo_sin(angle) >> 5) + py;
    lv_draw_mask_line_points_init(param, p1x, py, p2x, p2y, side);
}
/* Region mode entry: iterate rows over 2D area, calling line mask */
static lv_draw_mask_res_t lv_draw_mask_line_region(
    lv_mask_alpha_t *mask_buf, const scui_area_t *area, scui_coord_t stride,
    lv_draw_mask_line_param_t *p)
{
    bool changed = false;
    scui_coord_t y;
    for (y = area->y; y < area->y + area->h; y++) {
        lv_mask_alpha_t *line = mask_buf + (y - area->y) * stride;
        lv_draw_mask_res_t res = lv_draw_mask_line_row(p, line, area->x, y, area->w);
        if (res == LV_DRAW_MASK_RES_TRANSP) {
            mask_buf_set_zero(line, 0, area->w);
            changed = true;
        } else if (res == LV_DRAW_MASK_RES_CHANGED) changed = true;
    }
    /* mask_buf 现在是 uint8_t 类型，值自然在 0-255 范围内 */
    return changed ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
}
/* Single-line mask calculation (preserving original logic) */
static lv_draw_mask_res_t lv_draw_mask_line_row(
    lv_draw_mask_line_param_t *p,
    lv_mask_alpha_t *mask_buf, scui_coord_t abs_x, scui_coord_t abs_y, scui_coord_t len)
{
    abs_y -= p->origo.y;
    abs_x -= p->origo.x;
    if (p->steep == 0) {
        /* Horizontal line */
        if (p->flat) {
            if (p->cfg.side == LV_DRAW_MASK_LINE_SIDE_LEFT ||
                p->cfg.side == LV_DRAW_MASK_LINE_SIDE_RIGHT)
                return LV_DRAW_MASK_RES_FULL_COVER;
            else if (p->cfg.side == LV_DRAW_MASK_LINE_SIDE_TOP && abs_y + 1 < 0)
                return LV_DRAW_MASK_RES_FULL_COVER;
            else if (p->cfg.side == LV_DRAW_MASK_LINE_SIDE_BOTTOM && abs_y > 0)
                return LV_DRAW_MASK_RES_FULL_COVER;
            else
                return LV_DRAW_MASK_RES_TRANSP;
        }
        /* Vertical line */
        else {
            if (p->cfg.side == LV_DRAW_MASK_LINE_SIDE_TOP ||
                p->cfg.side == LV_DRAW_MASK_LINE_SIDE_BOTTOM)
                return LV_DRAW_MASK_RES_FULL_COVER;
            else if (p->cfg.side == LV_DRAW_MASK_LINE_SIDE_RIGHT && abs_x > 0)
                return LV_DRAW_MASK_RES_FULL_COVER;
            else if (p->cfg.side == LV_DRAW_MASK_LINE_SIDE_LEFT) {
                if (abs_x + len < 0) return LV_DRAW_MASK_RES_FULL_COVER;
                else {
                    int32_t k = -abs_x;
                    if (k < 0) return LV_DRAW_MASK_RES_TRANSP;
                    if (k >= 0 && k < len) {
                        int32_t n = len - k;
                        if (n > len) n = len;
                        if (k + n > len) n = len - k;
                        if (n > 0) mask_buf_set_zero(mask_buf, k, n);
                    }
                    return LV_DRAW_MASK_RES_CHANGED;
                }
            } else {
                if (abs_x + len < 0) return LV_DRAW_MASK_RES_TRANSP;
                else {
                    int32_t k = -abs_x;
                    if (k < 0) k = 0;
                    if (k >= len) return LV_DRAW_MASK_RES_TRANSP;
                    else if (k > 0 && k <= len) mask_buf_set_zero(mask_buf, 0, k);
                    return LV_DRAW_MASK_RES_CHANGED;
                }
            }
        }
    }
    return p->flat ? lv_draw_line_mask_flat(p, mask_buf, abs_x, abs_y, len)
                   : lv_draw_line_mask_steep(p, mask_buf, abs_x, abs_y, len);
}
static lv_draw_mask_res_t lv_draw_line_mask_flat(lv_draw_mask_line_param_t *p,
    lv_mask_alpha_t *mask_buf, scui_coord_t abs_x, scui_coord_t abs_y, scui_coord_t len)
{
    int32_t y_at_x = (int32_t)((int32_t)p->yx_steep * abs_x) >> 10;
    if (p->yx_steep > 0) {
        if (y_at_x > abs_y)
            return p->inv ? LV_DRAW_MASK_RES_FULL_COVER : LV_DRAW_MASK_RES_TRANSP;
    } else {
        if (y_at_x < abs_y)
            return p->inv ? LV_DRAW_MASK_RES_FULL_COVER : LV_DRAW_MASK_RES_TRANSP;
    }
    y_at_x = (int32_t)((int32_t)p->yx_steep * (abs_x + len)) >> 10;
    if (p->yx_steep > 0) {
        if (y_at_x < abs_y)
            return p->inv ? LV_DRAW_MASK_RES_TRANSP : LV_DRAW_MASK_RES_FULL_COVER;
    } else {
        if (y_at_x > abs_y)
            return p->inv ? LV_DRAW_MASK_RES_TRANSP : LV_DRAW_MASK_RES_FULL_COVER;
    }
    int32_t xe;
    if (p->yx_steep > 0) xe = ((abs_y * 256) * p->xy_steep) >> 10;
    else xe = (((abs_y + 1) * 256) * p->xy_steep) >> 10;
    int32_t xei = xe >> 8;
    int32_t xef = xe & 0xFF;
    int32_t px_h;
    if (xef == 0) px_h = 255;
    else px_h = 255 - (((255 - xef) * p->spx) >> 8);
    int32_t k = xei - abs_x;
    scui_alpha_t m;
    if (xef) {
        if (k >= 0 && k < len) {
            m = 255 - (((255 - xef) * (255 - px_h)) >> 9);
            if (p->inv) m = 255 - m;
            mask_buf[k] = lv_draw_mask_mix(mask_buf[k], m);
        }
        k++;
    }
    while (px_h > p->spx) {
        if (k >= 0 && k < len) {
            m = px_h - (p->spx >> 1);
            if (p->inv) m = 255 - m;
            mask_buf[k] = lv_draw_mask_mix(mask_buf[k], m);
        }
        px_h -= p->spx;
        k++;
        if (k >= len) break;
    }
    if (k < len && k >= 0) {
        int32_t x_inters = (px_h * p->xy_steep) >> 10;
        m = (x_inters * px_h) >> 9;
        if (p->yx_steep < 0) m = 255 - m;
        if (p->inv) m = 255 - m;
        mask_buf[k] = lv_draw_mask_mix(mask_buf[k], m);
    }
    if (p->inv) {
        k = xei - abs_x;
        /* 边界保护: k 必须在 [0, len] 范围内 */
        if (k <= 0) return LV_DRAW_MASK_RES_CHANGED;
        if (k > len) k = len;
        mask_buf_set_zero(mask_buf, 0, k);
    } else {
        k++;
        if (k >= len) return LV_DRAW_MASK_RES_CHANGED;
        if (k < 0) k = 0;
        mask_buf_set_zero(mask_buf, k, len - k);
    }
    return LV_DRAW_MASK_RES_CHANGED;
}
static lv_draw_mask_res_t lv_draw_line_mask_steep(lv_draw_mask_line_param_t *p,
    lv_mask_alpha_t *mask_buf, scui_coord_t abs_x, scui_coord_t abs_y, scui_coord_t len)
{
    int32_t k, x_at_y;
    x_at_y = (int32_t)((int32_t)p->xy_steep * abs_y) >> 10;
    if (p->xy_steep > 0) x_at_y++;
    if (x_at_y < abs_x)
        return p->inv ? LV_DRAW_MASK_RES_FULL_COVER : LV_DRAW_MASK_RES_TRANSP;
    x_at_y = (int32_t)((int32_t)p->xy_steep * (abs_y)) >> 10;
    if (x_at_y > abs_x + len)
        return p->inv ? LV_DRAW_MASK_RES_TRANSP : LV_DRAW_MASK_RES_FULL_COVER;
    int32_t xs = ((abs_y * 256) * p->xy_steep) >> 10;
    int32_t xsi = xs >> 8;
    int32_t xsf = xs & 0xFF;
    int32_t xe = (((abs_y + 1) * 256) * p->xy_steep) >> 10;
    int32_t xei = xe >> 8;
    int32_t xef = xe & 0xFF;
    scui_alpha_t m;
    k = xsi - abs_x;
    if (xsi != xei && (p->xy_steep < 0 && xsf == 0)) {
        xsf = 0xFF; xsi = xei; k--;
    }
    if (xsi == xei) {
        if (k >= 0 && k < len) {
            m = (xsf + xef) >> 1;
            if (p->inv) m = 255 - m;
            mask_buf[k] = lv_draw_mask_mix(mask_buf[k], m);
        }
        k++;
        if (p->inv) {
            k = xsi - abs_x;
            /* 边界保护 */
            if (k <= 0) return LV_DRAW_MASK_RES_CHANGED;
            if (k > len) k = len;
            mask_buf_set_zero(mask_buf, 0, k);
        } else {
            if (k >= len) return LV_DRAW_MASK_RES_CHANGED;
            if (k < 0) k = 0;
            mask_buf_set_zero(mask_buf, k, len - k);
        }
    } else {
        int32_t y_inters;
        if (p->xy_steep < 0) {
            y_inters = (xsf * (-p->yx_steep)) >> 10;
            if (k >= 0 && k < len) {
                m = (y_inters * xsf) >> 9;
                if (p->inv) m = 255 - m;
                mask_buf[k] = lv_draw_mask_mix(mask_buf[k], m);
            }
            k--;
            int32_t x_inters = ((255 - y_inters) * (-p->xy_steep)) >> 10;
            if (k >= 0 && k < len) {
                m = 255 - (((255 - y_inters) * x_inters) >> 9);
                if (p->inv) m = 255 - m;
                mask_buf[k] = lv_draw_mask_mix(mask_buf[k], m);
            }
            k += 2;
            if (p->inv) {
                k = xsi - abs_x - 1;
                /* 边界保护 */
                if (k <= 0) return LV_DRAW_MASK_RES_CHANGED;
                if (k > len) k = len;
                mask_buf_set_zero(mask_buf, 0, k);
            } else {
                if (k >= len) return LV_DRAW_MASK_RES_CHANGED;
                if (k < 0) k = 0;
                mask_buf_set_zero(mask_buf, k, len - k);
            }
        } else {
            y_inters = ((255 - xsf) * p->yx_steep) >> 10;
            if (k >= 0 && k < len) {
                m = 255 - ((y_inters * (255 - xsf)) >> 9);
                if (p->inv) m = 255 - m;
                mask_buf[k] = lv_draw_mask_mix(mask_buf[k], m);
            }
            k++;
            int32_t x_inters = ((255 - y_inters) * p->xy_steep) >> 10;
            if (k >= 0 && k < len) {
                m = ((255 - y_inters) * x_inters) >> 9;
                if (p->inv) m = 255 - m;
                mask_buf[k] = lv_draw_mask_mix(mask_buf[k], m);
            }
            k++;
            if (p->inv) {
                k = xsi - abs_x;
                /* 边界保护 */
                if (k <= 0) return LV_DRAW_MASK_RES_CHANGED;
                if (k > len) k = len;
                mask_buf_set_zero(mask_buf, 0, k);
            } else {
                if (k >= len) return LV_DRAW_MASK_RES_CHANGED;
                if (k < 0) k = 0;
                mask_buf_set_zero(mask_buf, k, len - k);
            }
        }
    }
    return LV_DRAW_MASK_RES_CHANGED;
}
/*****************************************************************************/
/* ======================== Angle Mask ===================================== */
/*****************************************************************************/
static void lv_draw_mask_angle_init(lv_draw_mask_angle_param_t *param,
    scui_coord_t vertex_x, scui_coord_t vertex_y,
    scui_coord_t start_angle, scui_coord_t end_angle)
{
    lv_draw_mask_line_side_t start_side, end_side;
    if (start_angle < 0) start_angle = 0;
    else if (start_angle > 359) start_angle = 359;
    if (end_angle < 0) end_angle = 0;
    else if (end_angle > 359) end_angle = 359;
    param->delta_deg = (end_angle < start_angle)
        ? (360 - start_angle + end_angle)
        : scui_abs(end_angle - start_angle);
    param->cfg.start_angle = start_angle;
    param->cfg.end_angle = end_angle;
    param->cfg.vertex_p.x = vertex_x;
    param->cfg.vertex_p.y = vertex_y;
    param->dsc.cb = (lv_draw_mask_region_cb_t)lv_draw_mask_angle_region;
    param->dsc.type = LV_DRAW_MASK_TYPE_ANGLE;
    start_side = (start_angle >= 0 && start_angle < 180)
        ? LV_DRAW_MASK_LINE_SIDE_LEFT : LV_DRAW_MASK_LINE_SIDE_RIGHT;
    end_side = (end_angle >= 0 && end_angle < 180)
        ? LV_DRAW_MASK_LINE_SIDE_RIGHT : LV_DRAW_MASK_LINE_SIDE_LEFT;
    lv_draw_mask_line_angle_init(&param->start_line, vertex_x, vertex_y, start_angle, start_side);
    lv_draw_mask_line_angle_init(&param->end_line, vertex_x, vertex_y, end_angle, end_side);
}
/* Single-line angle mask calculation (preserving original logic) */
static lv_draw_mask_res_t lv_draw_mask_angle_row(
    lv_draw_mask_angle_param_t *p,
    lv_mask_alpha_t *mask_buf, scui_coord_t abs_x, scui_coord_t abs_y, scui_coord_t len)
{
    int32_t rel_y = abs_y - p->cfg.vertex_p.y;
    int32_t rel_x = abs_x - p->cfg.vertex_p.x;
    if (p->cfg.start_angle < 180 && p->cfg.end_angle < 180 &&
        p->cfg.start_angle != 0 && p->cfg.end_angle != 0 &&
        p->cfg.start_angle > p->cfg.end_angle) {
        if (abs_y < p->cfg.vertex_p.y) return LV_DRAW_MASK_RES_FULL_COVER;
        int32_t end_angle_first = (rel_y * p->end_line.xy_steep) >> 10;
        int32_t start_angle_last = ((rel_y + 1) * p->start_line.xy_steep) >> 10;
        if (p->cfg.start_angle > 270 && p->cfg.start_angle <= 359 && start_angle_last < 0) start_angle_last = 0;
        else if (p->cfg.start_angle > 0 && p->cfg.start_angle <= 90 && start_angle_last < 0) start_angle_last = 0;
        else if (p->cfg.start_angle > 90 && p->cfg.start_angle < 270 && start_angle_last > 0) start_angle_last = 0;
        if (p->cfg.end_angle > 270 && p->cfg.end_angle <= 359 && start_angle_last < 0) start_angle_last = 0;
        else if (p->cfg.end_angle > 0 && p->cfg.end_angle <= 90 && start_angle_last < 0) start_angle_last = 0;
        else if (p->cfg.end_angle > 90 && p->cfg.end_angle < 270 && start_angle_last > 0) start_angle_last = 0;
        int32_t dist = (end_angle_first - start_angle_last) >> 1;
        lv_draw_mask_res_t res1 = LV_DRAW_MASK_RES_FULL_COVER;
        lv_draw_mask_res_t res2 = LV_DRAW_MASK_RES_FULL_COVER;
        int32_t tmp = start_angle_last + dist - rel_x;
        if (tmp > len) tmp = len;
        if (tmp > 0) {
            res1 = lv_draw_mask_line_row(&p->start_line, &mask_buf[0], abs_x, abs_y, tmp);
            if (res1 == LV_DRAW_MASK_RES_TRANSP) mask_buf_set_zero(mask_buf, 0, tmp);
        }
        if (tmp > len) tmp = len;
        if (tmp < 0) tmp = 0;
        res2 = lv_draw_mask_line_row(&p->end_line, &mask_buf[tmp], abs_x + tmp, abs_y, len - tmp);
        if (res2 == LV_DRAW_MASK_RES_TRANSP) mask_buf_set_zero(mask_buf, tmp, len - tmp);
        return (res1 == res2) ? res1 : LV_DRAW_MASK_RES_CHANGED;
    } else if (p->cfg.start_angle > 180 && p->cfg.end_angle > 180 && p->cfg.start_angle > p->cfg.end_angle) {
        if (abs_y > p->cfg.vertex_p.y) return LV_DRAW_MASK_RES_FULL_COVER;
        int32_t end_angle_first = (rel_y * p->end_line.xy_steep) >> 10;
        int32_t start_angle_last = ((rel_y + 1) * p->start_line.xy_steep) >> 10;
        if (p->cfg.start_angle > 270 && p->cfg.start_angle <= 359 && start_angle_last < 0) start_angle_last = 0;
        else if (p->cfg.start_angle > 0 && p->cfg.start_angle <= 90 && start_angle_last < 0) start_angle_last = 0;
        else if (p->cfg.start_angle > 90 && p->cfg.start_angle < 270 && start_angle_last > 0) start_angle_last = 0;
        if (p->cfg.end_angle > 270 && p->cfg.end_angle <= 359 && start_angle_last < 0) start_angle_last = 0;
        else if (p->cfg.end_angle > 0 && p->cfg.end_angle <= 90 && start_angle_last < 0) start_angle_last = 0;
        else if (p->cfg.end_angle > 90 && p->cfg.end_angle < 270 && start_angle_last > 0) start_angle_last = 0;
        int32_t dist = (end_angle_first - start_angle_last) >> 1;
        lv_draw_mask_res_t res1 = LV_DRAW_MASK_RES_FULL_COVER;
        lv_draw_mask_res_t res2 = LV_DRAW_MASK_RES_FULL_COVER;
        int32_t tmp = start_angle_last + dist - rel_x;
        if (tmp > len) tmp = len;
        if (tmp > 0) {
            res1 = lv_draw_mask_line_row((lv_draw_mask_line_param_t *)&p->end_line, &mask_buf[0], abs_x, abs_y, tmp);
            if (res1 == LV_DRAW_MASK_RES_TRANSP) mask_buf_set_zero(mask_buf, 0, tmp);
        }
        if (tmp > len) tmp = len;
        if (tmp < 0) tmp = 0;
        res2 = lv_draw_mask_line_row((lv_draw_mask_line_param_t *)&p->start_line, &mask_buf[tmp], abs_x + tmp, abs_y, len - tmp);
        if (res2 == LV_DRAW_MASK_RES_TRANSP) mask_buf_set_zero(mask_buf, tmp, len - tmp);
        return (res1 == res2) ? res1 : LV_DRAW_MASK_RES_CHANGED;
    } else {
        lv_draw_mask_res_t res1 = LV_DRAW_MASK_RES_FULL_COVER;
        lv_draw_mask_res_t res2 = LV_DRAW_MASK_RES_FULL_COVER;
        if (p->cfg.start_angle == 180) {
            res1 = (abs_y < p->cfg.vertex_p.y) ? LV_DRAW_MASK_RES_FULL_COVER : LV_DRAW_MASK_RES_UNKNOWN;
        } else if (p->cfg.start_angle == 0) {
            res1 = (abs_y < p->cfg.vertex_p.y) ? LV_DRAW_MASK_RES_UNKNOWN : LV_DRAW_MASK_RES_FULL_COVER;
        } else if ((p->cfg.start_angle < 180 && abs_y < p->cfg.vertex_p.y) ||
                   (p->cfg.start_angle > 180 && abs_y >= p->cfg.vertex_p.y)) {
            res1 = LV_DRAW_MASK_RES_UNKNOWN;
        } else {
            res1 = lv_draw_mask_line_row(&p->start_line, mask_buf, abs_x, abs_y, len);
        }
        if (p->cfg.end_angle == 180) {
            res2 = (abs_y < p->cfg.vertex_p.y) ? LV_DRAW_MASK_RES_UNKNOWN : LV_DRAW_MASK_RES_FULL_COVER;
        } else if (p->cfg.end_angle == 0) {
            res2 = (abs_y < p->cfg.vertex_p.y) ? LV_DRAW_MASK_RES_FULL_COVER : LV_DRAW_MASK_RES_UNKNOWN;
        } else if ((p->cfg.end_angle < 180 && abs_y < p->cfg.vertex_p.y) ||
                   (p->cfg.end_angle > 180 && abs_y >= p->cfg.vertex_p.y)) {
            res2 = LV_DRAW_MASK_RES_UNKNOWN;
        } else {
            res2 = lv_draw_mask_line_row(&p->end_line, mask_buf, abs_x, abs_y, len);
        }
        if (res1 == LV_DRAW_MASK_RES_TRANSP || res2 == LV_DRAW_MASK_RES_TRANSP) return LV_DRAW_MASK_RES_TRANSP;
        else if (res1 == LV_DRAW_MASK_RES_UNKNOWN && res2 == LV_DRAW_MASK_RES_UNKNOWN) return LV_DRAW_MASK_RES_TRANSP;
        else if (res1 == LV_DRAW_MASK_RES_FULL_COVER && res2 == LV_DRAW_MASK_RES_FULL_COVER) return LV_DRAW_MASK_RES_FULL_COVER;
        else return LV_DRAW_MASK_RES_CHANGED;
    }
}
/* Region mode entry: iterate rows over 2D area */
static lv_draw_mask_res_t lv_draw_mask_angle_region(
    lv_mask_alpha_t *mask_buf, const scui_area_t *area, scui_coord_t stride,
    lv_draw_mask_angle_param_t *p)
{
    bool changed = false;
    scui_coord_t y;
    for (y = area->y; y < area->y + area->h; y++) {
        lv_mask_alpha_t *line = mask_buf + (y - area->y) * stride;
        lv_draw_mask_res_t res = lv_draw_mask_angle_row(p, line, area->x, y, area->w);
        if (res == LV_DRAW_MASK_RES_TRANSP) {
            mask_buf_set_zero(line, 0, area->w);
            changed = true;
        } else if (res == LV_DRAW_MASK_RES_CHANGED) changed = true;
    }
    return changed ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
}
/*****************************************************************************/
/* ======================== Radius Mask ==================================== */
/*****************************************************************************/
static void lv_draw_mask_radius_init(lv_draw_mask_radius_param_t *param,
    const scui_area_t *rect, scui_coord_t radius, bool inv)
{
    scui_coord_t w = rect->w, h = rect->h;
    int32_t short_side = scui_min(w, h);
    if (radius > short_side >> 1) radius = short_side >> 1;
    if (radius < 0) radius = 0;
    memcpy(&param->cfg.rect, rect, sizeof(scui_area_t));
    param->cfg.radius = radius;
    param->cfg.outer = inv ? 1 : 0;
    param->dsc.cb = (lv_draw_mask_region_cb_t)lv_draw_mask_radius_region;
    param->dsc.type = LV_DRAW_MASK_TYPE_RADIUS;
    if (radius == 0) { param->circle = NULL; return; }
#if SCUI_MEM_SENTRY_CHECK
    scui_mem_sentry_check();
    SCUI_LOG_DEBUG("mask_radius_init: sentry before alloc entry, radius=%d,sizeof_entry=%d", (int)radius, (int)sizeof(lv_draw_mask_radius_circle_dsc_t));
#endif
    lv_draw_mask_radius_circle_dsc_t *entry = SCUI_MEM_ALLOC(scui_mem_type_graph, sizeof(lv_draw_mask_radius_circle_dsc_t));
#if SCUI_MEM_SENTRY_CHECK
    scui_mem_sentry_check();
    SCUI_LOG_DEBUG("mask_radius_init: sentry after alloc entry, radius=%d entry=%p", (int)radius, (void*)entry);
#endif
    memset(entry, 0, sizeof(lv_draw_mask_radius_circle_dsc_t));
    /* 验证 memset 确实清零了 c->buf 指针 */
    SCUI_ASSERT(entry->buf == NULL);
    SCUI_ASSERT(entry->cir_opa == NULL);
    param->circle = entry;
#if SCUI_MEM_SENTRY_CHECK
    scui_mem_sentry_check();
    SCUI_LOG_DEBUG("mask_radius_init: sentry before calc_aa4, radius=%d entry=%p", (int)radius, (void*)entry);
#endif
    lv_draw_circle_calc_aa4(param->circle, radius);
#if SCUI_MEM_SENTRY_CHECK
    SCUI_LOG_DEBUG("mask_radius_init: sentry after calc_aa4, radius=%d", (int)radius);
    scui_mem_sentry_check();
    SCUI_LOG_DEBUG("mask_radius_init: sentry after calc_aa4 OK");
#endif
}
/* Single-line radius mask calculation (preserving original logic) */
static lv_draw_mask_res_t lv_draw_mask_radius_row(
    lv_draw_mask_radius_param_t *p,
    lv_mask_alpha_t *mask_buf, scui_coord_t abs_x, scui_coord_t abs_y, scui_coord_t len)
{
    bool outer = p->cfg.outer;
    int32_t radius = p->cfg.radius;
    scui_area_t rect;
    memcpy(&rect, &p->cfg.rect, sizeof(scui_area_t));
    if (outer == false) {
        if ((abs_y < rect.y || abs_y > rect.y + rect.h - 1))
            return LV_DRAW_MASK_RES_TRANSP;
    } else {
        if (abs_y < rect.y || abs_y > rect.y + rect.h - 1)
            return LV_DRAW_MASK_RES_FULL_COVER;
    }
    if ((abs_x >= rect.x + radius && abs_x + len <= rect.x + rect.w - radius) ||
        (abs_y >= rect.y + radius && abs_y <= rect.y + rect.h - radius)) {
        if (outer == false) {
            int32_t last = rect.x - abs_x;
            if (last > len) return LV_DRAW_MASK_RES_TRANSP;
            if (last >= 0) mask_buf_set_zero(mask_buf, 0, last);
            int32_t first = rect.x + rect.w - abs_x;
            if (first <= 0) return LV_DRAW_MASK_RES_TRANSP;
            else if (first < len) mask_buf_set_zero(mask_buf, first, len - first);
            if (last == 0 && first == len) return LV_DRAW_MASK_RES_FULL_COVER;
            else return LV_DRAW_MASK_RES_CHANGED;
        } else {
            int32_t first = rect.x - abs_x;
            if (first < 0) first = 0;
            if (first <= len) {
                int32_t last = rect.x + rect.w - abs_x - first;
                if (first + last > len) last = len - first;
                if (last >= 0) mask_buf_set_zero(mask_buf, first, last);
            }
        }
        return LV_DRAW_MASK_RES_CHANGED;
    }
    int32_t k = rect.x - abs_x;
    int32_t w = rect.w, h = rect.h;
    abs_x -= rect.x;
    abs_y -= rect.y;
    scui_coord_t aa_len, x_start;
    scui_coord_t cir_y = (abs_y < radius) ? (radius - abs_y - 1) : (abs_y - (h - radius));
#if SCUI_DRAW_BOUNDS_LOG
    if (cir_y < 0 || cir_y >= radius) {
        SCUI_LOG_WARN("radius_row: cir_y=%d out of range [0,%d), abs_y=%d radius=%d h=%d rect=(%d,%d,%d,%d)",
            (int)cir_y, (int)radius, (int)abs_y, (int)radius, (int)h,
            (int)rect.x, (int)rect.y, (int)rect.w, (int)rect.h);
        return outer ? LV_DRAW_MASK_RES_FULL_COVER : LV_DRAW_MASK_RES_TRANSP;
    }
#endif
    lv_mask_alpha_t *aa_opa = lv_draw_mask_radius_get_next_line(p->circle, cir_y, &aa_len, &x_start);
    scui_coord_t cir_x_right = k + w - radius + x_start;
    scui_coord_t cir_x_left = k + radius - x_start - 1;
    scui_coord_t i;
    if (outer == false) {
        for (i = 0; i < aa_len; i++) {
            scui_alpha_t opa = aa_opa[aa_len - i - 1];
            if (cir_x_right + i >= 0 && cir_x_right + i < len)
                mask_buf[cir_x_right + i] = lv_draw_mask_mix(opa, mask_buf[cir_x_right + i]);
            if (cir_x_left - i >= 0 && cir_x_left - i < len)
                mask_buf[cir_x_left - i] = lv_draw_mask_mix(opa, mask_buf[cir_x_left - i]);
        }
        cir_x_right = scui_clamp(0, cir_x_right + i, len);
#if SCUI_DRAW_BOUNDS_LOG
        if (len - cir_x_right > 0)
            SCUI_LOG_DEBUG("radius_row memset_right: off=%d len=%d", (int)cir_x_right, (int)(len - cir_x_right));
#endif
        if (cir_x_right < len)
            mask_buf_set_zero(mask_buf, cir_x_right, len - cir_x_right);
        cir_x_left = scui_clamp(0, cir_x_left - aa_len + 1, len);
#if SCUI_DRAW_BOUNDS_LOG
        if (cir_x_left > 0)
            SCUI_LOG_DEBUG("radius_row memset_left: len=%d", (int)cir_x_left);
#endif
        if (cir_x_left > 0)
            mask_buf_set_zero(mask_buf, 0, cir_x_left);
    } else {
        for (i = 0; i < aa_len; i++) {
            scui_alpha_t opa = 255 - (aa_opa[aa_len - 1 - i]);
            if (cir_x_right + i >= 0 && cir_x_right + i < len)
                mask_buf[cir_x_right + i] = lv_draw_mask_mix(opa, mask_buf[cir_x_right + i]);
            if (cir_x_left - i >= 0 && cir_x_left - i < len)
                mask_buf[cir_x_left - i] = lv_draw_mask_mix(opa, mask_buf[cir_x_left - i]);
        }
        scui_coord_t clr_start = scui_clamp(0, cir_x_left + 1, len);
        scui_coord_t clr_len = scui_clamp(0, cir_x_right - clr_start, len - clr_start);
        mask_buf_set_zero(mask_buf, clr_start, clr_len);
    }
    return LV_DRAW_MASK_RES_CHANGED;
}
/* Region mode entry: iterate rows over 2D area */
static lv_draw_mask_res_t lv_draw_mask_radius_region(
    lv_mask_alpha_t *mask_buf, const scui_area_t *area, scui_coord_t stride,
    lv_draw_mask_radius_param_t *p)
{
    bool changed = false;
    scui_coord_t y;
    for (y = area->y; y < area->y + area->h; y++) {
        lv_mask_alpha_t *line = mask_buf + (y - area->y) * stride;
        lv_draw_mask_res_t res = lv_draw_mask_radius_row(p, line, area->x, y, area->w);
        if (res == LV_DRAW_MASK_RES_TRANSP) {
            mask_buf_set_zero(line, 0, area->w);  /* zero 该行，继续处理其他行 */
            changed = true;
        }
        else if (res == LV_DRAW_MASK_RES_CHANGED) changed = true;
    }
    return changed ? LV_DRAW_MASK_RES_CHANGED : LV_DRAW_MASK_RES_FULL_COVER;
}
/*****************************************************************************/
/* ======================== Circle AA Cache Computation ==================== */
/*****************************************************************************/
static void lv_draw_circle_init(scui_point_t *c, scui_coord_t *tmp, scui_coord_t radius)
{
    c->x = radius; c->y = 0; *tmp = 1 - radius;
}
static bool lv_draw_circle_cont(scui_point_t *c) { return c->y <= c->x; }
static void lv_draw_circle_next(scui_point_t *c, scui_coord_t *tmp)
{
    if (*tmp <= 0) { (*tmp) += 2 * c->y + 3; }
    else { (*tmp) += 2 * (c->y - c->x) + 5; c->x--; }
    c->y++;
}
static void lv_draw_circle_calc_aa4(lv_draw_mask_radius_circle_dsc_t *c, scui_coord_t radius)
{
    if (radius == 0) return;
    c->radius = radius;
#if SCUI_MEM_SENTRY_CHECK
    scui_mem_sentry_check();
#endif
    if (c->buf) SCUI_MEM_FREE(c->buf);
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("circle_calc_aa4: radius=%d alloc=%d + %d = %d",
        (int)radius,
        (int)(radius * 6 + 8),
        (int)((radius + 1) * 4 * 2 * (int)sizeof(scui_coord_t)),
        (int)(radius * 6 + 8 + (radius + 1) * 4 * 2 * (int)sizeof(scui_coord_t)));
#endif
    /* 合并分配：c->buf + cir_x 一次分配，避免多次SCUI_MEM_ALLOC之间的canary检查问题 */
    {
        uint32_t buf_size = (uint32_t)(radius * 6 + 8);
        uint32_t cirx_size = (uint32_t)((radius + 1) * 4 * 2 * sizeof(scui_coord_t));
        c->buf = SCUI_MEM_ALLOC(scui_mem_type_graph, buf_size + cirx_size);
    }
    c->cir_opa = c->buf;
    c->opa_start_on_y = (uint16_t *)(c->buf + 2 * radius + 4);
    c->x_start_on_y = (uint16_t *)(c->buf + 4 * radius + 6);
    if (radius == 1) {
        c->cir_opa[0] = 180;
        c->opa_start_on_y[0] = 0;
        c->opa_start_on_y[1] = 1;
        c->x_start_on_y[0] = 0;
        return;
    }
    /* cir_x/cir_y 复用 c->buf 尾部空间：cir_x 紧接 c->buf 数据 */
    scui_coord_t *cir_x = (scui_coord_t *)(c->buf + radius * 6 + 8);
    scui_coord_t *cir_y = &cir_x[(radius + 1) * 4];
    uint32_t y_8th_cnt = 0;
    scui_point_t cp;
    scui_coord_t tmp;
    lv_draw_circle_init(&cp, &tmp, radius * 4);
    int32_t i;
    uint32_t x_int[4], x_fract[4];
    scui_coord_t cir_size = 0;
    x_int[0] = cp.x >> 2; x_fract[0] = 0;
    while (lv_draw_circle_cont(&cp)) {
        for (i = 0; i < 4; i++) {
            lv_draw_circle_next(&cp, &tmp);
            if (!lv_draw_circle_cont(&cp)) break;
            x_int[i] = cp.x >> 2;
            x_fract[i] = cp.x & 0x3;
        }
        if (i != 4) break;
        if (x_int[0] == x_int[3]) {
            cir_x[cir_size] = x_int[0]; cir_y[cir_size] = y_8th_cnt;
            c->cir_opa[cir_size] = (x_fract[0] + x_fract[1] + x_fract[2] + x_fract[3]) * 16;
            cir_size++;
        } else if (x_int[0] != x_int[1]) {
            cir_x[cir_size] = x_int[0]; cir_y[cir_size] = y_8th_cnt;
            c->cir_opa[cir_size] = x_fract[0] * 16; cir_size++;
            cir_x[cir_size] = x_int[0] - 1; cir_y[cir_size] = y_8th_cnt;
            c->cir_opa[cir_size] = (1 * 4 + x_fract[1] + x_fract[2] + x_fract[3]) * 16;
            cir_size++;
        } else if (x_int[0] != x_int[2]) {
            cir_x[cir_size] = x_int[0]; cir_y[cir_size] = y_8th_cnt;
            c->cir_opa[cir_size] = (x_fract[0] + x_fract[1]) * 16; cir_size++;
            cir_x[cir_size] = x_int[0] - 1; cir_y[cir_size] = y_8th_cnt;
            c->cir_opa[cir_size] = (2 * 4 + x_fract[2] + x_fract[3]) * 16;
            cir_size++;
        } else {
            cir_x[cir_size] = x_int[0]; cir_y[cir_size] = y_8th_cnt;
            c->cir_opa[cir_size] = (x_fract[0] + x_fract[1] + x_fract[2]) * 16; cir_size++;
            cir_x[cir_size] = x_int[0] - 1; cir_y[cir_size] = y_8th_cnt;
            c->cir_opa[cir_size] = (3 * 4 + x_fract[3]) * 16;
            cir_size++;
        }
        y_8th_cnt++;
    }
    /* 1/8 circle special point */
    int32_t mid = radius * 723;
    int32_t mid_int = mid >> 10;
    if (cir_x[cir_size - 1] != mid_int || cir_y[cir_size - 1] != mid_int) {
        int32_t tmp_val = mid - (mid_int << 10);
        if (tmp_val <= 512) {
            tmp_val = tmp_val * tmp_val * 2;
            tmp_val = tmp_val >> (10 + 6);
        } else {
            tmp_val = 1024 - tmp_val;
            tmp_val = tmp_val * tmp_val * 2;
            tmp_val = tmp_val >> (10 + 6);
            tmp_val = 15 - tmp_val;
        }
        cir_x[cir_size] = mid_int; cir_y[cir_size] = mid_int;
        c->cir_opa[cir_size] = tmp_val * 16;
        cir_size++;
    }
    /* Second octant mirror */
    for (i = cir_size - 2; i >= 0; i--, cir_size++) {
        cir_x[cir_size] = cir_y[i]; cir_y[cir_size] = cir_x[i];
        c->cir_opa[cir_size] = c->cir_opa[i];
    }
    scui_coord_t y = 0;
    i = 0;
    c->opa_start_on_y[0] = 0;
    while (i < cir_size) {
        if (y > radius) {
            SCUI_LOG_ERROR("circle_calc_aa4: y=%d exceeds radius=%d, cir_size=%d, i=%d",
                (int)y, (int)radius, (int)cir_size, (int)i);
            break;
        }
        c->opa_start_on_y[y] = i;
        c->x_start_on_y[y] = cir_x[i];
        for (; cir_y[i] == y && i < (int32_t)cir_size; i++)
            c->x_start_on_y[y] = scui_min(c->x_start_on_y[y], cir_x[i]);
        y++;
    }
    /* cir_x 内嵌在 c->buf 中，无需单独释放 */
}
static lv_mask_alpha_t *lv_draw_mask_radius_get_next_line(lv_draw_mask_radius_circle_dsc_t *c,
    scui_coord_t y, scui_coord_t *len, scui_coord_t *x_start)
{
    *len = c->opa_start_on_y[y + 1] - c->opa_start_on_y[y];
    *x_start = c->x_start_on_y[y];
    return &c->cir_opa[c->opa_start_on_y[y]];
}
static inline scui_alpha_t lv_draw_mask_mix(scui_alpha_t mask_act, scui_alpha_t mask_new)
{
    if (mask_new >= scui_alpha_cover) return mask_act;
    if (mask_new <= scui_alpha_trans) return scui_alpha_trans;
    return SCUI_DIV_0xFF(mask_act * mask_new);
}
/*****************************************************************************/
/* ======================== Diagnostic Helpers ============================= */
/*****************************************************************************/
/* 安全分配 mask_buf，带溢出检查 (使用 lv_mask_alpha_t = uint8_t) */
static lv_mask_alpha_t *draw_mask_alloc(scui_coord_t w, scui_coord_t h, const char *caller)
{
#if SCUI_DRAW_OVERFLOW_CHECK
    if (w <= 0 || h <= 0) {
        SCUI_LOG_ERROR("%s: invalid size w=%d h=%d", caller, (int)w, (int)h);
        return NULL;
    }
    uint32_t u_w = (uint32_t)w, u_h = (uint32_t)h;
    if (u_w > UINT32_MAX / u_h) {
        SCUI_LOG_ERROR("%s: overflow w=%d h=%d", caller,
            (int)w, (int)h);
        return NULL;
    }
#endif
    /* 分配 w*h 字节 + 安全垫片，防止 mask 操作微越界破坏 allocator */
    uint32_t nbytes = (uint32_t)w * (uint32_t)h + 16;
    lv_mask_alpha_t *buf = SCUI_MEM_ALLOC(scui_mem_type_graph, nbytes);
    SCUI_ASSERT(buf != NULL);
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("%s: alloc %d x %d = %u bytes at %p",
        caller, (int)w, (int)h, (unsigned)nbytes, (void*)buf);
#endif

    return buf;
}
/* 释放 mask_buf */
static void draw_mask_free(lv_mask_alpha_t *buf, scui_coord_t w, scui_coord_t h, const char *caller)
{
    (void)caller;
    if (buf) SCUI_MEM_FREE(buf);
}
/*****************************************************************************/
/* ======================== SCUI Blend Helper ============================== */
/* 将 mask_buf (alpha8 uint8_t*) 通过框架的 scui_draw_area_blend 渲染到 surface */
/*****************************************************************************/
static void lv_draw_sw_blend(lv_draw_sw_ctx_t *ctx,
    const lv_mask_alpha_t *mask_buf, scui_coord_t mask_w, scui_coord_t mask_h,
    const scui_area_t *mask_area, const scui_area_t *dst_clip,
    scui_color32_t color, scui_alpha_t opa, bool is_additive)
{
    scui_coord_t w = mask_area->w, h = mask_area->h;
    if (w <= 0 || h <= 0) return;
    (void)is_additive;

    /* 取交集 */
    scui_area_t blend_clip;
    if (!scui_area_inter(&blend_clip, dst_clip, mask_area))
        return;

    if (mask_buf == NULL) {
        /* 无 mask: 直接逐行填充（避免 SCUI 任务系统重入） */
        scui_surface_t *dst_surf = ctx->dst_surface;
        scui_coord_t y;
        for (y = 0; y < blend_clip.h; y++) {
            uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surf, blend_clip.y + y, blend_clip.x);
            scui_coord_t x;
            for (x = 0; x < blend_clip.w; x++) {
                scui_pixel_mix_with(dst_surf->format, dst_ofs,
                    scui_pixel_cf_bmp8888, &color, opa);
                dst_ofs += dst_surf->pbyte;
            }
        }
    } else {
        /* 有 mask: 逐行 blend — 每行只算一次 surface_pixel_ofs */
        scui_surface_t *dst_surf = ctx->dst_surface;
        scui_coord_t off_x = blend_clip.x - mask_area->x;
        scui_coord_t off_y = blend_clip.y - mask_area->y;
        scui_coord_t y;
        for (y = 0; y < blend_clip.h; y++) {
            const lv_mask_alpha_t *mask_row = mask_buf + (off_y + y) * mask_w + off_x;
            uint8_t *dst_ofs = scui_surface_pixel_ofs(dst_surf, blend_clip.y + y, blend_clip.x);
            scui_coord_t x;
            for (x = 0; x < blend_clip.w; x++) {
                lv_mask_alpha_t m = mask_row[x];
                if (m == 0) { dst_ofs += dst_surf->pbyte; continue; }
                scui_alpha_t mix_a = (m >= LV_OPA_MAX) ? opa
                    : (scui_alpha_t)(((uint16_t)m * (uint16_t)opa) >> 8);
                scui_pixel_mix_with(dst_surf->format, dst_ofs,
                    scui_pixel_cf_bmp8888, &color, mix_a);
                dst_ofs += dst_surf->pbyte;
            }
        }
    }
}
/*****************************************************************************/
/* ======================== Primitive Drawing ============================== */
/*****************************************************************************/
/*@brief Outline rendering: mask->alpha surface (region mode)
 *@param dst_surface  target surface (alpha format, 1 byte per pixel)
 *@param dst_area     target surface area
 *@param dst_clip     clip area
 *@param mask_area    mask evaluation area
 */
static void mask_to_surface(
    scui_surface_t *dst_surface,
    const scui_area_t *dst_area,
    const scui_area_t *dst_clip,
    const scui_area_t *mask_area)
{
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL);
    SCUI_ASSERT(dst_surface->format == scui_pixel_cf_alpha8);
    scui_area_t draw_area;
    if (!scui_area_inter(&draw_area, dst_area, dst_clip))
        return;
    scui_coord_t w = draw_area.w;
    scui_coord_t h = draw_area.h;
    if (w <= 0 || h <= 0) return;
    uint8_t *dst_pixel = dst_surface->pixel;
    scui_coord_t stride = dst_surface->hor_res;
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("mask_to_surface: draw_area=(%d,%d,%d,%d) stride=%d",
        (int)draw_area.x, (int)draw_area.y, (int)w, (int)h, (int)stride);
#endif
    /* Allocate temporary mask buffer */
    lv_mask_alpha_t *mask_buf = draw_mask_alloc(w, h, "mask_to_surface");
    if (mask_buf == NULL) return;
    /* Initialize to full cover */
    memset(mask_buf, scui_alpha_cover, (size_t)w * (size_t)h);
    /* One-shot region mask calculation */
    lv_draw_mask_apply_region(mask_buf, &draw_area, w);
#if SCUI_MEM_SENTRY_CHECK
    SCUI_LOG_DEBUG("mask_to_surface: sentry after mask_apply, before writeback");
    scui_mem_sentry_check();
    SCUI_LOG_DEBUG("mask_to_surface: sentry after mask_apply OK");
#endif
    /* Write back to target surface */
    scui_coord_t y;
    for (y = 0; y < h; y++) {
        scui_coord_t dst_y = draw_area.y + y;
        scui_coord_t dst_x = draw_area.x;
#if SCUI_DRAW_BOUNDS_LOG
        /* 验证目标表面写入边界 */
        if (dst_y < 0 || dst_y >= dst_surface->ver_res ||
            dst_x < 0 || dst_x + w > dst_surface->hor_res) {
            SCUI_LOG_ERROR("mask_to_surface: dst OOB! dst_y=%d dst_x=%d w=%d dst_res=(%d,%d)",
                (int)dst_y, (int)dst_x, (int)w,
                (int)dst_surface->hor_res, (int)dst_surface->ver_res);
            return;
        }
#endif
        /* mask_buf 是 uint8_t，dst 也是 uint8_t，直接 memcpy */
        uint8_t *dst = dst_pixel + dst_y * stride + dst_x;
        lv_mask_alpha_t *src = mask_buf + y * w;
        memcpy(dst, src, (size_t)w);
    }
#if SCUI_MEM_SENTRY_CHECK
    SCUI_LOG_DEBUG("mask_to_surface: sentry after writeback, before free");
    scui_mem_sentry_check();
    SCUI_LOG_DEBUG("mask_to_surface: sentry after writeback OK");
#endif
    draw_mask_free(mask_buf, w, h, "mask_to_surface");
}
/* Common region helper: allocate 2D mask within clip_area, apply masks, then blend */
static void draw_scan_and_blend(
    lv_draw_sw_ctx_t *ctx,
    scui_color32_t color, scui_alpha_t opa,
    const scui_area_t *clip_area)
{
    scui_coord_t draw_w = clip_area->w;
    scui_coord_t draw_h = clip_area->h;
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("scan_blend entry: area=(%d,%d,%d,%d) color=#%06X opa=%d ctx_buf=(%d,%d,%d,%d) clip=(%d,%d,%d,%d)",
        (int)clip_area->x, (int)clip_area->y, (int)draw_w, (int)draw_h,
        (unsigned)color.full, (int)opa,
        (int)ctx->buf_area.x, (int)ctx->buf_area.y, (int)ctx->buf_area.w, (int)ctx->buf_area.h,
        (int)ctx->clip_area->x, (int)ctx->clip_area->y, (int)ctx->clip_area->w, (int)ctx->clip_area->h);
#endif
    if (draw_w <= 0 || draw_h <= 0) return;
    lv_mask_alpha_t *mask_buf = draw_mask_alloc(draw_w, draw_h, "draw_scan_and_blend");
    if (mask_buf == NULL) return;
    /* Initialize to specified opacity (lv_mask_alpha_t = uint8_t) */
    memset(mask_buf, opa, (size_t)draw_w * (size_t)draw_h);
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("scan_blend before mask_apply: opa_init=%d", (int)opa);
#endif
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("scan_blend before mask_apply: opa_init=%d", (int)opa);
#endif
    /* One-shot region mask calculation */
    lv_draw_mask_res_t mask_result = lv_draw_mask_apply_region(mask_buf, clip_area, draw_w);
#if SCUI_DRAW_BOUNDS_LOG
    /* mask_apply 后统计 mask_buf 内容 */
    {
        uint32_t size = (uint32_t)draw_w * (uint32_t)draw_h;
        uint32_t n_zeros = 0, n_255 = 0, n_other = 0;
        uint32_t i;
        for (i = 0; i < size; i++) {
            if (mask_buf[i] == 0) n_zeros++;
            else if (mask_buf[i] == 255) n_255++;
            else n_other++;
        }
        SCUI_LOG_DEBUG("scan_blend after mask_apply: result=%d stats: zeros=%u ones=%u other=%u",
            (int)mask_result, (unsigned)n_zeros, (unsigned)n_255, (unsigned)n_other);
        if (mask_result == LV_DRAW_MASK_RES_TRANSP)
            SCUI_LOG_WARN("scan_blend: mask_result=TRANSP, skipping blend");
    }
#endif
    if (mask_result == LV_DRAW_MASK_RES_TRANSP) {
        draw_mask_free(mask_buf, draw_w, draw_h, "draw_scan_and_blend");
        return;
    }
    /* Blend via SCUI framework (alpha8 surface) */
    lv_draw_sw_blend(ctx, mask_buf, draw_w, draw_h,
        clip_area, ctx->clip_area, color, opa, false);
    draw_mask_free(mask_buf, draw_w, draw_h, "draw_scan_and_blend");
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("scan_blend done");
#endif
}
/*****************************************************************************/
/* Line Drawing (Anti-aliased) */
/* Ported from lv_draw_sw_line.c */
/*****************************************************************************/
static void lv_draw_sw_line(lv_draw_sw_ctx_t *ctx,
    scui_color32_t color, scui_alpha_t opa, scui_coord_t width,
    const scui_point_t *p1, const scui_point_t *p2)
{
    if (opa <= scui_alpha_trans) return;
    if (width <= 0) width = 1;
    if (p1->x == p2->x && p1->y == p2->y) return;
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("line entry: p1=(%d,%d) p2=(%d,%d) width=%d opa=%d color=#%06X",
        (int)p1->x, (int)p1->y, (int)p2->x, (int)p2->y,
        (int)width, (int)opa, (unsigned)color.full);
#endif
    /* Horizontal line: direct blend */
    if (p1->y == p2->y) {
        scui_area_t blend_area;
        blend_area.x = scui_min(p1->x, p2->x);
        blend_area.y = p1->y - (width >> 1);
        blend_area.w = scui_abs(p2->x - p1->x) + 1;
        blend_area.h = width;
        lv_draw_sw_blend(ctx, NULL, 0, 0, &blend_area, ctx->clip_area, color, opa, false);
        return;
    }
    /* Vertical line: direct blend */
    if (p1->x == p2->x) {
        scui_area_t blend_area;
        blend_area.x = p1->x - (width >> 1);
        blend_area.y = scui_min(p1->y, p2->y);
        blend_area.w = width;
        blend_area.h = scui_abs(p2->y - p1->y) + 1;
        lv_draw_sw_blend(ctx, NULL, 0, 0, &blend_area, ctx->clip_area, color, opa, false);
        return;
    }
    /* Diagonal line: ensure p1.y <= p2.y */
    scui_point_t pt1 = *p1, pt2 = *p2;
    if (pt1.y > pt2.y) { scui_point_t t = pt1; pt1 = pt2; pt2 = t; }
    int32_t xdiff = pt2.x - pt1.x, ydiff = pt2.y - pt1.y;
    bool flat = scui_abs(xdiff) > scui_abs(ydiff);
    /* Width correction */
    int32_t w = width;
    int32_t wcorr_i;
    if (flat) wcorr_i = (scui_abs(ydiff) << 5) / scui_abs(xdiff);
    else      wcorr_i = (scui_abs(xdiff) << 5) / scui_abs(ydiff);
    w = (w * lv_draw_line_wcorr[wcorr_i] + 63) >> 7;
    int32_t w_half0 = w >> 1;
    int32_t w_half1 = w_half0 + (w & 0x1);
    /* 上行 flat 线：直接逐行渲染（mask 精度不足导致锯齿） */
    if (flat && xdiff < 0) {
        int32_t step = scui_abs(xdiff) / ydiff;  /* 像素/行 中心位移 */
        int32_t rw = scui_max(w, step + 2);      /* +2确保step=3时仍有重叠 */
        scui_coord_t y;
        for (y = pt1.y; y <= pt2.y; y++) {
            if (y < ctx->clip_area->y || y > ctx->clip_area->y + ctx->clip_area->h) continue;
            int32_t cx = pt1.x + (xdiff * (y - pt1.y)) / ydiff;
            int32_t x0 = cx - (rw >> 1);
            int32_t x1 = x0 + rw - 1;
            /* 端点裁剪（不超过 pt1.x / pt2.x） */
            if (y == pt1.y) x0 = scui_max(x0, pt1.x);
            if (y == pt2.y) x1 = scui_min(x1, pt2.x);
            scui_area_t seg = {.x = x0, .y = y, .w = x1 - x0 + 1, .h = 1};
            scui_area_t clip_seg;
            if (seg.w > 0 && scui_area_inter(&clip_seg, &seg, ctx->clip_area))
                lv_draw_sw_blend(ctx, NULL, 0, 0, &clip_seg, ctx->clip_area, color, opa, false);
        }
        return;
    }
    /* Calculate blend area */
    scui_area_t blend_area;
    blend_area.x = scui_min(pt1.x, pt2.x) - w;
    blend_area.y = scui_min(pt1.y, pt2.y) - w;
    blend_area.w = scui_abs(pt2.x - pt1.x) + (w << 1);
    blend_area.h = scui_abs(pt2.y - pt1.y) + (w << 1);
    /* Create 4 line masks */
    lv_draw_mask_line_param_t mask_left, mask_right, mask_top, mask_bottom;
    if (flat) {
        if (xdiff > 0) {
            lv_draw_mask_line_points_init(&mask_left,  pt1.x, pt1.y - w_half0, pt2.x, pt2.y - w_half0, LV_DRAW_MASK_LINE_SIDE_LEFT);
            lv_draw_mask_line_points_init(&mask_right, pt1.x, pt1.y + w_half1, pt2.x, pt2.y + w_half1, LV_DRAW_MASK_LINE_SIDE_RIGHT);
        } else {
            lv_draw_mask_line_points_init(&mask_left,  pt1.x, pt1.y + w_half1, pt2.x, pt2.y + w_half1, LV_DRAW_MASK_LINE_SIDE_LEFT);
            lv_draw_mask_line_points_init(&mask_right, pt1.x, pt1.y - w_half0, pt2.x, pt2.y - w_half0, LV_DRAW_MASK_LINE_SIDE_RIGHT);
        }
    } else {
        lv_draw_mask_line_points_init(&mask_left,  pt1.x + w_half1, pt1.y, pt2.x + w_half1, pt2.y, LV_DRAW_MASK_LINE_SIDE_LEFT);
        lv_draw_mask_line_points_init(&mask_right, pt1.x - w_half0, pt1.y, pt2.x - w_half0, pt2.y, LV_DRAW_MASK_LINE_SIDE_RIGHT);
    }
    int16_t mask_left_id  = lv_draw_mask_add(&mask_left, NULL);
    int16_t mask_right_id = lv_draw_mask_add(&mask_right, NULL);
    lv_draw_mask_line_points_init(&mask_top, pt1.x, pt1.y, pt1.x - ydiff, pt1.y + xdiff, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
    lv_draw_mask_line_points_init(&mask_bottom, pt2.x, pt2.y, pt2.x - ydiff, pt2.y + xdiff, LV_DRAW_MASK_LINE_SIDE_TOP);
    int16_t mask_top_id    = lv_draw_mask_add(&mask_top, NULL);
    int16_t mask_bottom_id = lv_draw_mask_add(&mask_bottom, NULL);
    scui_area_t clip_area;
    if (!scui_area_inter(&clip_area, &blend_area, ctx->clip_area)) {
        lv_draw_mask_remove_id(mask_left_id); lv_draw_mask_remove_id(mask_right_id);
        lv_draw_mask_remove_id(mask_top_id);  lv_draw_mask_remove_id(mask_bottom_id);
        return;
    }
    draw_scan_and_blend(ctx, color, opa, &clip_area);
    lv_draw_mask_remove_id(mask_left_id);  lv_draw_mask_remove_id(mask_right_id);
    lv_draw_mask_remove_id(mask_top_id);   lv_draw_mask_remove_id(mask_bottom_id);
    lv_draw_mask_free_param(&mask_left);   lv_draw_mask_free_param(&mask_right);
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("line done");
#endif
}
/*****************************************************************************/
/* Circle Drawing (Anti-aliased, Filled) */
/* Ported from lv_draw_sw_rect.c draw_bg + radius mask */
/*****************************************************************************/
static void lv_draw_sw_circle(lv_draw_sw_ctx_t *ctx,
    scui_color32_t color, scui_alpha_t opa,
    const scui_point_t *center, scui_coord_t radius)
{
    if (opa <= scui_alpha_trans || radius <= 0) return;
    scui_area_t circle_area;
    circle_area.x = center->x - radius;
    circle_area.y = center->y - radius;
    circle_area.w = (radius << 1) + 1;
    circle_area.h = (radius << 1) + 1;
    lv_draw_mask_radius_param_t mask_param;
    lv_draw_mask_radius_init(&mask_param, &circle_area, radius, false);
    int16_t mask_id = lv_draw_mask_add(&mask_param, NULL);
    scui_area_t clip_area;
    if (!scui_area_inter(&clip_area, &circle_area, ctx->clip_area)) {
        lv_draw_mask_remove_id(mask_id);
        lv_draw_mask_free_param(&mask_param);
        return;
    }
    draw_scan_and_blend(ctx, color, opa, &clip_area);
    lv_draw_mask_remove_id(mask_id);
    lv_draw_mask_free_param(&mask_param);
}
/*****************************************************************************/
/* Rounded Rectangle Drawing (Anti-aliased) */
/* Ported from lv_draw_sw_rect.c draw_bg */
/*****************************************************************************/
static void lv_draw_sw_crect(lv_draw_sw_ctx_t *ctx,
    scui_color32_t color, scui_alpha_t opa, scui_coord_t width,
    const scui_area_t *coords, scui_coord_t radius)
{
    if (opa <= scui_alpha_trans) return;
    scui_coord_t short_side = scui_min(coords->w, coords->h);
    scui_coord_t rout = scui_min(radius, short_side >> 1);
    bool is_outline = (width > 0) && (width < rout);  /* EGUI: 0<width<radius 为 outline */
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("crect entry: coords=(%d,%d,%d,%d) radius=%d rout=%d width=%d outline=%d opa=%d",
        (int)coords->x, (int)coords->y, (int)coords->w, (int)coords->h,
        (int)radius, (int)rout, (int)width, (int)is_outline, (int)opa);
#endif
    if (rout <= 0) {
        lv_draw_sw_blend(ctx, NULL, 0, 0, coords, ctx->clip_area, color, opa, false);
        return;
    }
    scui_area_t clip_area;
    if (!scui_area_inter(&clip_area, coords, ctx->clip_area))
        return;
    scui_coord_t draw_w = clip_area.w;
    scui_coord_t draw_h = clip_area.h;
    if (draw_w <= 0 || draw_h <= 0) return;
    lv_mask_alpha_t *mask_buf = draw_mask_alloc(draw_w, draw_h, "crect");
    if (mask_buf == NULL) return;
    /* 外圈 mask (fill) */
    lv_draw_mask_radius_param_t mask_out_param;
    lv_draw_mask_radius_init(&mask_out_param, coords, rout, false);
    int16_t mask_out_id = lv_draw_mask_add(&mask_out_param, NULL);
    /* Initialize to specified opacity */
    memset(mask_buf, opa, (size_t)draw_w * (size_t)draw_h);
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("crect: init OK, size=%u opa=%d", (unsigned)(draw_w * draw_h), (int)opa);
#endif
    lv_draw_mask_res_t mask_result = lv_draw_mask_apply_region(mask_buf, &clip_area, draw_w);
#if SCUI_DRAW_BOUNDS_LOG
    /* mask_apply 后统计 */
    {
        uint32_t size = (uint32_t)draw_w * (uint32_t)draw_h;
        uint32_t n_zeros = 0, n_255 = 0, n_other = 0;
        uint32_t i;
        for (i = 0; i < size; i++) {
            if (mask_buf[i] == 0) n_zeros++;
            else if (mask_buf[i] == 255) n_255++;
            else n_other++;
        }
        SCUI_LOG_DEBUG("crect mask_apply result=%d stats: zeros=%u ones=%u other=%u",
            (int)mask_result, (unsigned)n_zeros, (unsigned)n_255, (unsigned)n_other);
    }
    if (mask_result == LV_DRAW_MASK_RES_TRANSP)
        SCUI_LOG_WARN("crect: mask_result=TRANSP, skipping blend");
#endif
    if (mask_result == LV_DRAW_MASK_RES_TRANSP) {
        lv_draw_mask_remove_id(mask_out_id);
        lv_draw_mask_free_param(&mask_out_param);
        draw_mask_free(mask_buf, draw_w, draw_h, "crect");
        return;
    }
    /* 内圈挖空（ring/outline）：几何距离法清零 inner circle */
    if (is_outline) {
        scui_coord_t inner_r = rout - width + 1;  /* +1 补偿外圈AA占据的像素 */
        if (inner_r > 0) {
            scui_coord_t cx = coords->x + coords->w / 2;
            scui_coord_t cy = coords->y + coords->h / 2;
            int32_t ir2 = (int32_t)inner_r * (int32_t)inner_r;
            int32_t ir_a2 = ((int32_t)inner_r - 1) * ((int32_t)inner_r - 1); /* AA内边界 */
            scui_coord_t y, x;
            for (y = 0; y < draw_h; y++) {
                int32_t dy = (clip_area.y + y) - cy;
                int32_t dy2 = dy * dy;
                for (x = 0; x < draw_w; x++) {
                    int32_t dx = (clip_area.x + x) - cx;
                    int32_t d2 = dx * dx + dy2;
                    if (d2 <= ir_a2) {
                        mask_buf[y * draw_w + x] = 0;   /* 完全在内圈内 */
                    } else if (d2 < ir2) {
                        /* AA：距内圈边缘 1 像素软过渡 */
                        int32_t diff  = d2 - ir_a2;
                        int32_t range = ir2 - ir_a2;
                        lv_mask_alpha_t a = mask_buf[y * draw_w + x];
                        lv_mask_alpha_t factor = (lv_mask_alpha_t)((uint32_t)diff * 255u / (uint32_t)range);
                        mask_buf[y * draw_w + x] = (lv_mask_alpha_t)((uint32_t)a * (uint32_t)factor / 255u);
                    }
                }
            }
        }
    }
    /* Blend via SCUI framework */
    lv_draw_sw_blend(ctx, mask_buf, draw_w, draw_h,
        &clip_area, ctx->clip_area, color, opa, false);
    draw_mask_free(mask_buf, draw_w, draw_h, "crect");
    lv_draw_mask_remove_id(mask_out_id);
    lv_draw_mask_free_param(&mask_out_param);
#if SCUI_DRAW_BOUNDS_LOG
    SCUI_LOG_DEBUG("crect done");
#endif
}
/*****************************************************************************/
/* Arc Drawing (Anti-aliased) */
/* Ported from lv_draw_sw_arc.c */
/*****************************************************************************/
static void lv_draw_sw_arc(lv_draw_sw_ctx_t *ctx,
    scui_color32_t color, scui_alpha_t opa, scui_coord_t width,
    const scui_point_t *center, scui_coord_t radius,
    scui_coord_t start_angle, scui_coord_t end_angle,
    bool rounded)
{
    if (opa <= scui_alpha_trans || width <= 0 || start_angle == end_angle) return;
    if (width > radius) width = radius;
    /* Normalize angles to [0, 360) */
    while (start_angle >= 360) start_angle -= 360;
    while (end_angle >= 360)   end_angle   -= 360;
    while (start_angle < 0)    start_angle += 360;
    while (end_angle < 0)      end_angle   += 360;
    /* Full ring: draw circle directly */
    if (start_angle == 0 && end_angle == 0) {
        lv_draw_sw_circle(ctx, color, opa, center, radius);
        if (width < radius) {
            scui_coord_t inner_r = radius - width;
            if (inner_r > 0) {
                scui_area_t inner_area;
                inner_area.x = center->x - inner_r;
                inner_area.y = center->y - inner_r;
                inner_area.w = (inner_r << 1) + 1;
                inner_area.h = (inner_r << 1) + 1;
                lv_draw_mask_radius_param_t inner_mask;
                lv_draw_mask_radius_init(&inner_mask, &inner_area, inner_r, true);
                int16_t inner_id = lv_draw_mask_add(&inner_mask, NULL);
                scui_area_t clip;
                if (scui_area_inter(&clip, &inner_area, ctx->clip_area)) {
                    draw_scan_and_blend(ctx, color, opa, &clip);
                }
                lv_draw_mask_remove_id(inner_id);
                lv_draw_mask_free_param(&inner_mask);
            }
        }
        return;
    }
    /* Construct outer circle area (odd size for center pixel) */
    scui_area_t area_out;
    area_out.x = center->x - radius;
    area_out.y = center->y - radius;
    area_out.w = (radius << 1) + 1;
    area_out.h = (radius << 1) + 1;
    /* Inner circle area (shrunk by width on all sides) */
    scui_area_t area_in = area_out;
    area_in.x += width;
    area_in.y += width;
    area_in.w -= (width << 1);
    area_in.h -= (width << 1);
    /* Inner circle mask (punch out interior) — 仅 ring/outline */
    int16_t mask_in_id = LV_MASK_ID_INV;
    lv_draw_mask_radius_param_t mask_in_param;
    if ((width > 0) && (width < radius) && area_in.w > 0 && area_in.h > 0) {
        lv_draw_mask_radius_init(&mask_in_param, &area_in, area_in.w >> 1, true);
        mask_in_id = lv_draw_mask_add(&mask_in_param, NULL);
    }
    /* Outer circle mask */
    lv_draw_mask_radius_param_t mask_out_param;
    lv_draw_mask_radius_init(&mask_out_param, &area_out, radius, false);
    int16_t mask_out_id = lv_draw_mask_add(&mask_out_param, NULL);
    /* Angle mask */
    lv_draw_mask_angle_param_t mask_angle_param;
    lv_draw_mask_angle_init(&mask_angle_param, center->x, center->y, start_angle, end_angle);
    int16_t mask_angle_id = lv_draw_mask_add(&mask_angle_param, NULL);
    /* Calculate actual draw area */
    scui_area_t clip_area;
    if (!scui_area_inter(&clip_area, &area_out, ctx->clip_area)) {
        lv_draw_mask_remove_id(mask_angle_id);
        lv_draw_mask_remove_id(mask_out_id);
        if (mask_in_id != LV_MASK_ID_INV) lv_draw_mask_remove_id(mask_in_id);
        lv_draw_mask_free_param(&mask_angle_param);
        lv_draw_mask_free_param(&mask_out_param);
        if (mask_in_id != LV_MASK_ID_INV) lv_draw_mask_free_param(&mask_in_param);
        return;
    }
    /* One-shot region draw with all masks applied */
    draw_scan_and_blend(ctx, color, opa, &clip_area);
    /* Rounded cap handling */
    if (rounded) {
        scui_coord_t r = width >> 1;
        scui_point_t pt;
        pt.x = center->x + ((lv_trigo_sin(start_angle + 90) * (radius - r)) >> LV_TRIGO_SHIFT);
        pt.y = center->y + ((lv_trigo_sin(start_angle) * (radius - r)) >> LV_TRIGO_SHIFT);
        lv_draw_sw_circle(ctx, color, opa, &pt, r);
        pt.x = center->x + ((lv_trigo_sin(end_angle + 90) * (radius - r)) >> LV_TRIGO_SHIFT);
        pt.y = center->y + ((lv_trigo_sin(end_angle) * (radius - r)) >> LV_TRIGO_SHIFT);
        lv_draw_sw_circle(ctx, color, opa, &pt, r);
    }
    lv_draw_mask_remove_id(mask_angle_id);
    lv_draw_mask_remove_id(mask_out_id);
    if (mask_in_id != LV_MASK_ID_INV) lv_draw_mask_remove_id(mask_in_id);
    lv_draw_mask_free_param(&mask_angle_param);
    lv_draw_mask_free_param(&mask_out_param);
    if (mask_in_id != LV_MASK_ID_INV) lv_draw_mask_free_param(&mask_in_param);
}
/*****************************************************************************/
/* Main entry: extract params from scui_draw_dsc_t, build ctx, call draw function */
/*****************************************************************************/
static void ctx_from_dsc(lv_draw_sw_ctx_t *ctx, scui_draw_dsc_t *draw_dsc)
{
    scui_surface_t *surface = draw_dsc->graph.dst_surface;
    SCUI_ASSERT(surface != NULL);
    ctx->buf = surface->pixel;
    ctx->buf_area = scui_surface_area(surface);
    ctx->clip_area = &draw_dsc->graph.dst_clip;
    ctx->dst_surface = surface;
}
bool scui_draw_ctx_graph_LVGL(scui_draw_dsc_t *draw_dsc)
{
    scui_surface_t *dst_surface = draw_dsc->graph.dst_surface;
    scui_color32_t  src_color   = draw_dsc->graph.src_color.color;
    scui_alpha_t    src_alpha   = draw_dsc->graph.src_alpha;
    if (src_alpha == scui_alpha_trans) return false;
    if (dst_surface == NULL || dst_surface->pixel == NULL) return false;
    lv_draw_sw_ctx_t ctx;
    ctx_from_dsc(&ctx, draw_dsc);
    switch (draw_dsc->type) {
    case scui_draw_type_pixel_line: {
        scui_coord_t width = draw_dsc->graph.src_width;
        if (width <= 0) width = 1;
        lv_draw_sw_line(&ctx, src_color, src_alpha, width,
            &draw_dsc->graph.src_pos_1, &draw_dsc->graph.src_pos_2);
#if SCUI_DRAW_BOUNDS_LOG
        SCUI_LOG_DEBUG("draw exit: line done");
#endif
#if SCUI_MEM_SENTRY_CHECK
        scui_mem_sentry_check();
        SCUI_LOG_DEBUG("draw exit: line sentry OK");
#endif
        return true;
    }
    case scui_draw_type_pixel_circle: {
        scui_coord_t width  = draw_dsc->graph.src_width;
        scui_coord_t radius = draw_dsc->graph.src_radius;
        if (width > 0 && width < radius) {  /* EGUI: 0<width<radius → 空心圆环 */
            /* 圆环：用 crect 的 ring 模式 */
            scui_area_t circle_rect = {
                .x = draw_dsc->graph.src_center.x - radius,
                .y = draw_dsc->graph.src_center.y - radius,
                .w = (radius << 1) + 1,
                .h = (radius << 1) + 1,
            };
            lv_draw_sw_crect(&ctx, src_color, src_alpha, width,
                &circle_rect, radius);
        } else {
            lv_draw_sw_circle(&ctx, src_color, src_alpha,
                &draw_dsc->graph.src_center, radius);
        }
#if SCUI_DRAW_BOUNDS_LOG
        SCUI_LOG_DEBUG("draw exit: circle done");
#endif
#if SCUI_MEM_SENTRY_CHECK
        scui_mem_sentry_check();
        SCUI_LOG_DEBUG("draw exit: circle sentry OK");
#endif
        return true;
    }
    case scui_draw_type_pixel_crect: {
        scui_coord_t width = draw_dsc->graph.src_width;
        lv_draw_sw_crect(&ctx, src_color, src_alpha, width,
            &draw_dsc->graph.dst_part, draw_dsc->graph.src_radius);
#if SCUI_DRAW_BOUNDS_LOG
        SCUI_LOG_DEBUG("draw exit: crect done");
#endif
#if SCUI_MEM_SENTRY_CHECK
        scui_mem_sentry_check();
        SCUI_LOG_DEBUG("draw exit: crect sentry OK");
#endif
        return true;
    }
    case scui_draw_type_pixel_arc: {
        scui_coord_t width  = draw_dsc->graph.src_width;
        scui_coord_t radius = draw_dsc->graph.src_radius;
        bool rounded = draw_dsc->graph.src_round != 0;
        lv_draw_sw_arc(&ctx, src_color, src_alpha, width,
            &draw_dsc->graph.src_center, radius,
            draw_dsc->graph.src_angle_s, draw_dsc->graph.src_angle_e,
            rounded);
#if SCUI_DRAW_BOUNDS_LOG
        SCUI_LOG_DEBUG("draw exit: arc done");
#endif
#if SCUI_MEM_SENTRY_CHECK
        scui_mem_sentry_check();
        SCUI_LOG_DEBUG("draw exit: arc sentry OK");
#endif
        return true;
    }
    default:
#if SCUI_DRAW_BOUNDS_LOG
        SCUI_LOG_WARN("draw exit: unknown type=%d", (int)draw_dsc->type);
#endif
        return false;
    }
}

