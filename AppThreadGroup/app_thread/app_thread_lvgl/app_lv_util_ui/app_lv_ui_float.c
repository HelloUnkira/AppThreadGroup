/*实现目标:
 *    lvgl子窗口浮动特效
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_ui_float.h"

/* 四个浮动子窗口私有数据 */
static app_lv_ui_float_t  app_lv_ui_float_pl = {0};
static app_lv_ui_float_t  app_lv_ui_float_pr = {0};
static app_lv_ui_float_t  app_lv_ui_float_pt = {0};
static app_lv_ui_float_t  app_lv_ui_float_pb = {0};
static app_lv_ui_float_t *app_lv_ui_float_act = NULL;

/*@brief 浮动子窗口透明度递归改变回调
 */
static void app_lv_ui_float_set_opa_recursion(void *var, int32_t val)
{
    uint8_t   opa = val;
    lv_obj_t *obj = var;
    
    lv_obj_set_style_opa(obj, opa, 0);

    uint32_t child_cnt = obj->spec_attr->child_cnt;
    lv_obj_t **children = obj->spec_attr->children;
    for (uint32_t idx = 0; idx < child_cnt; idx++)
        app_lv_ui_float_set_opa_recursion(children[idx], opa);
}

/*@brief  浮动子窗口滚动处理
 */
static void app_lv_ui_float_scroll(app_lv_ui_float_t *float_var, lv_indev_t *act)
{
    const lv_coord_t scroll_threshold = -10;
    /* 统一先锁定滚动 */
    float_var->scroll = true;
    /* 按下时,记录按下点 */
    lv_point_t point = {0};
    lv_indev_get_point(act, &point);
    lv_obj_t *obj = lv_indev_get_scroll_obj(act);
    if (obj == NULL)
        return;
    /* 水平方向锁定(对象在起始点时,对象在结束点时) */
    if (float_var->resume_x > 0 && float_var->resume_y == 0)
    if (scroll_threshold > lv_obj_get_scroll_left(obj))
        float_var->scroll = false;
    if (float_var->resume_x < 0 && float_var->resume_y == 0)
    if (scroll_threshold > lv_obj_get_scroll_right(obj))
        float_var->scroll = false;
    /* 垂直方向锁定(对象在起始点时,对象在结束点时)  */
    if (float_var->resume_y > 0 && float_var->resume_x == 0)
    if (scroll_threshold > lv_obj_get_scroll_top(obj))
        float_var->scroll = false;
    if (float_var->resume_y < 0 && float_var->resume_x == 0)
    if (scroll_threshold > lv_obj_get_scroll_bottom(obj))
        float_var->scroll = false;
    /* ... */
    APP_SYS_LOG_INFO("scroll:%d",           float_var->scroll);
    APP_SYS_LOG_INFO("scroll_x:%d",         lv_obj_get_scroll_x(obj));
    APP_SYS_LOG_INFO("scroll_y:%d",         lv_obj_get_scroll_y(obj));
    APP_SYS_LOG_INFO("scroll_top:%d",       lv_obj_get_scroll_top(obj));
    APP_SYS_LOG_INFO("scroll_bottom:%d",    lv_obj_get_scroll_bottom(obj));
    APP_SYS_LOG_INFO("scroll_left:%d",      lv_obj_get_scroll_left(obj));
    APP_SYS_LOG_INFO("scroll_right:%d",     lv_obj_get_scroll_right(obj));
}

/*@brief  浮动子窗口位移门限检查
 *@retval 是否有足够的滑动位移
 */
static bool app_lv_ui_float_threshold(app_lv_ui_float_t *float_var, lv_point_t point)
{
    const lv_coord_t click_threshold = 30;
    lv_coord_t obj_x = lv_obj_get_x(float_var->scene);
    lv_coord_t obj_y = lv_obj_get_y(float_var->scene);
    /* 水平方向锁定(对象在起始点时,对象在结束点时) */
   if (float_var->resume_x > 0 && float_var->resume_y == 0) {
        /* 如果没有达到位移门限 */
        if (obj_x == float_var->resume_x && obj_y == float_var->resume_y)
        if (point.x >= float_var->click_x - click_threshold)
            return false;
        if (obj_x == 0 && obj_y == 0)
        if (point.x <= float_var->click_x + click_threshold)
            return false;
    }
    if (float_var->resume_x < 0 && float_var->resume_y == 0) {
        /* 如果没有达到位移门限 */
        if (obj_x == float_var->resume_x && obj_y == float_var->resume_y)
        if (point.x <= float_var->click_x + click_threshold)
            return false;
        if (obj_x == 0 && obj_y == 0)
        if (point.x >= float_var->click_x - click_threshold)
            return false;
    }
    /* 垂直方向锁定(对象在起始点时,对象在结束点时)  */
    if (float_var->resume_y > 0 && float_var->resume_x == 0) {
        /* 如果没有达到位移门限 */
        if (obj_x == float_var->resume_x && obj_y == float_var->resume_y)
        if (point.y >= float_var->click_y - click_threshold)
            return false;
        if (obj_x == 0 && obj_y == 0)
        if (point.y <= float_var->click_y + click_threshold)
            return false;
    }
    if (float_var->resume_y < 0 && float_var->resume_x == 0) {
        /* 如果没有达到位移门限 */
        if (obj_x == float_var->resume_x && obj_y == float_var->resume_y)
        if (point.y <= float_var->click_y + click_threshold)
            return false;
        if (obj_x == 0 && obj_y == 0)
        if (point.y >= float_var->click_y - click_threshold)
            return false;
    }
    return true;
}

/*@brief  浮动子窗口复位(如果未复位的话)
 *@retval 是否进行复位
 */
bool app_lv_ui_float_reset(void)
{
    app_lv_ui_float_t *float_var = app_lv_ui_float_act;
    /* 需要有活动浮窗存在 */
    if (float_var == NULL)
        return false;
    /* 不存在未复位的浮窗 */
    if (!float_var->cover)
        return false;
    /* 滚动事件未结束,暂时不处理 */
    if (float_var->scroll_w != LV_DIR_NONE)
        return false;
    /* 滚动方向为水平滚动 */
    if (float_var == &app_lv_ui_float_pl || float_var == &app_lv_ui_float_pr) {
        /* 不管什么情况,回弹它 */
        lv_anim_set_values(&float_var->anim_pos, lv_obj_get_x(float_var->scene), float_var->resume_x);
        lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_width(float_var->scene), 0);
        float_var->cover = false;
        lv_anim_set_exec_cb(&float_var->anim_pos, (lv_anim_exec_xcb_t)lv_obj_set_x);
    }
    /* 滚动方向为垂直滚动 */
    if (float_var == &app_lv_ui_float_pt || float_var == &app_lv_ui_float_pb) {
        /* 不管什么情况,回弹它 */
        lv_anim_set_values(&float_var->anim_pos, lv_obj_get_y(float_var->scene), float_var->resume_y);
        lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_height(float_var->scene), 0);
        float_var->cover = false;
        lv_anim_set_exec_cb(&float_var->anim_pos, (lv_anim_exec_xcb_t)lv_obj_set_y);
    }
    lv_anim_set_exec_cb(&float_var->anim_opa, app_lv_ui_float_set_opa_recursion);
    lv_anim_start(&float_var->anim_pos);
    lv_anim_start(&float_var->anim_opa);
    /* 清除滑动记录 */
    float_var->scroll_w = LV_DIR_NONE;
    /* 没有活动浮窗 */
    if (!float_var->cover)
        app_lv_ui_float_act = NULL;
    return true;
}

/*@brief 浮动子窗口特效回调
 */
static void app_lv_ui_float_event_cb(lv_event_t * e)
{
    app_lv_ui_float_t *float_var = lv_event_get_user_data(e);
    
    lv_obj_t   *obj = float_var->scene;
    lv_indev_t *act = lv_indev_get_act();
    
    /* 有活动浮窗,阻止其他浮窗的响应 */
    if (app_lv_ui_float_act != NULL &&
        app_lv_ui_float_act != float_var)
        return;
    
    switch(lv_event_get_code(e)) {
    case LV_EVENT_SCROLL_BEGIN: {
        /* 子界面能够滚动时,锁定尝试 */
        app_lv_ui_float_scroll(float_var, act);
        break;
    }
    case LV_EVENT_SCROLL: {
        /* 子界面能够滚动时,锁定尝试 */
        app_lv_ui_float_scroll(float_var, act);
        break;
    }
    case LV_EVENT_SCROLL_END: {
        float_var->scroll = false;
        break;
    }
    case LV_EVENT_PRESSED: {
        /* 按下时,记录按下点 */
        lv_point_t point = {0};
        lv_indev_get_point(act, &point);
        float_var->click_x = point.x;
        float_var->click_y = point.y;
        break;
    }
    case LV_EVENT_PRESSING: {
        /* 按下时,记录按下点 */
        lv_point_t point = {0};
        lv_indev_get_point(act, &point);
        /* 有效滚动中锁定门限 */
        if (float_var->scroll)
            break;
        /* 滚动方向需要满足滚动条件 */
        if (!app_lv_ui_float_threshold(float_var, point))
            break;
        /* 滚动开始时,锁定滚动方向,滚动只可以在这个方向上 */
        if (float_var->scroll_w == LV_DIR_NONE) {
            /* 水平方向锁定(对象在起始点时,对象在结束点时) */
           if (float_var->resume_x > 0 && float_var->resume_y == 0) {
                float_var->scroll_w = LV_DIR_HOR;
                float_var->scroll_x = point.x;
                float_var->percent  = lv_obj_get_width(obj) - point.x;
            }
            if (float_var->resume_x < 0 && float_var->resume_y == 0) {
                float_var->scroll_w = LV_DIR_HOR;
                float_var->scroll_x = point.x - lv_obj_get_width(obj);
                float_var->percent  = point.x;
            }
            /* 垂直方向锁定(对象在起始点时,对象在结束点时)  */
            if (float_var->resume_y > 0 && float_var->resume_x == 0) {
                float_var->scroll_w = LV_DIR_VER;
                float_var->scroll_y = point.y;
                float_var->percent  = lv_obj_get_height(obj) - point.y;
            }
            if (float_var->resume_y < 0 && float_var->resume_x == 0) {
                float_var->scroll_w = LV_DIR_VER;
                float_var->scroll_y = point.y - lv_obj_get_height(obj);
                float_var->percent  = point.y;
            }
            /* 未抓获滚动方向 */
            if (float_var->scroll_w == LV_DIR_NONE)
                break;
            /* 记录当前活动的浮窗,以锁定其他浮窗 */
            app_lv_ui_float_act = float_var;
            /* 根据滚动方向,将坐标动画偏移到指针点 */
            /* 滚动方向为水平滚动 */
            if (float_var->scroll_w == LV_DIR_HOR) {
                lv_anim_set_values(&float_var->anim_pos, lv_obj_get_x(obj), float_var->scroll_x);
                lv_anim_set_values(&float_var->anim_opa, 0, float_var->percent * 255 / lv_obj_get_width(obj));
                lv_anim_set_exec_cb(&float_var->anim_pos, (lv_anim_exec_xcb_t)lv_obj_set_x);
            }
            /* 滚动方向为垂直滚动 */
            if (float_var->scroll_w == LV_DIR_VER) {
                lv_anim_set_values(&float_var->anim_pos, lv_obj_get_y(obj), float_var->scroll_y);
                lv_anim_set_values(&float_var->anim_opa, 0, float_var->percent * 255 / lv_obj_get_height(obj));
                lv_anim_set_exec_cb(&float_var->anim_pos, (lv_anim_exec_xcb_t)lv_obj_set_y);
            }
            lv_anim_set_exec_cb(&float_var->anim_opa, (lv_anim_exec_xcb_t)app_lv_ui_float_set_opa_recursion);
            lv_anim_start(&float_var->anim_pos);
            lv_anim_start(&float_var->anim_opa);
        } else {
            /* 滚动方向为水平滚动 */
            if (float_var->scroll_w == LV_DIR_HOR) {
                /* 水平方向更新 */
                if (float_var->resume_x > 0 && float_var->resume_y == 0) {
                    float_var->scroll_x = point.x;
                    float_var->percent  = lv_obj_get_width(obj) - point.x;
                }
                if (float_var->resume_x < 0 && float_var->resume_y == 0) {
                    float_var->scroll_x = point.x - lv_obj_get_width(obj);
                    float_var->percent  = point.x;
                }
                lv_anim_set_values(&float_var->anim_pos, lv_obj_get_x(obj), float_var->scroll_x);
                lv_anim_set_values(&float_var->anim_opa, lv_obj_get_style_opa(obj, 0), float_var->percent * 255 / lv_obj_get_width(obj));
                /* 动画只会播放一次 */
                if (lv_anim_get_playtime(&float_var->anim_pos) >= float_var->anim_period - 1 &&
                    lv_anim_get_playtime(&float_var->anim_opa) >= float_var->anim_period - 1) {
                    lv_obj_set_x(obj, float_var->scroll_x);
                    app_lv_ui_float_set_opa_recursion(obj, float_var->percent * 255 / lv_obj_get_width(obj));
                }
            }
            /* 滚动方向为垂直滚动 */
            if (float_var->scroll_w == LV_DIR_VER) {
                /* 垂直方向更新 */
                if (float_var->resume_y > 0 && float_var->resume_x == 0) {
                    float_var->scroll_y = point.y;
                    float_var->percent  = lv_obj_get_height(obj) - point.y;
                }
                if (float_var->resume_y < 0 && float_var->resume_x == 0) {
                    float_var->scroll_y = point.y - lv_obj_get_height(obj);
                    float_var->percent  = point.y;
                }
                lv_anim_set_values(&float_var->anim_pos, lv_obj_get_y(obj), float_var->scroll_y);
                lv_anim_set_values(&float_var->anim_opa, lv_obj_get_style_opa(obj, 0), float_var->percent * 255 / lv_obj_get_height(obj));
                /* 动画只会播放一次 */
                if (lv_anim_get_playtime(&float_var->anim_pos) >= float_var->anim_period - 1 &&
                    lv_anim_get_playtime(&float_var->anim_opa) >= float_var->anim_period - 1) {
                    lv_obj_set_y(obj, float_var->scroll_y);
                    app_lv_ui_float_set_opa_recursion(obj, float_var->percent * 255 / lv_obj_get_height(obj));
                }
            }
        }
        break;
    }
    case LV_EVENT_RELEASED: {
        /* 未抓获滚动方向 */
        if (float_var->scroll_w == LV_DIR_NONE)
            break;
        /* 按下时,记录按下点 */
        lv_point_t point = {0};
        lv_indev_get_point(act, &point);
        /* 根据滚动方向,将坐标动画偏移到指针点 */
        uint32_t rollback_x = 70; //lv_obj_get_width(float_var->scene) / 3
        uint32_t rollback_y = 80; //lv_obj_get_height(float_var->scene) / 3
        /* 滚动方向为水平滚动 */
        if (float_var->scroll_w == LV_DIR_HOR) {
            /* 计算偏移量 */
            uint32_t delta_x = LV_ABS(point.x - float_var->click_x);
            /* 是回弹还是前进 */
            if (delta_x < rollback_x) {
                if (float_var->cover) {
                    lv_anim_set_values(&float_var->anim_pos, lv_obj_get_x(obj), 0);
                    lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_width(obj), 255);
                    float_var->cover = true;
                } else {
                    lv_anim_set_values(&float_var->anim_pos, lv_obj_get_x(obj), float_var->resume_x);
                    lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_width(obj), 0);
                    float_var->cover = false;
                }
            } else {
                if (float_var->cover) {
                    lv_anim_set_values(&float_var->anim_pos, lv_obj_get_x(obj), float_var->resume_x);
                    lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_width(obj), 0);
                    float_var->cover = false;
                } else {
                    lv_anim_set_values(&float_var->anim_pos, lv_obj_get_x(obj), 0);
                    lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_width(obj), 255);
                    float_var->cover = true;
                }
            }
            lv_anim_set_exec_cb(&float_var->anim_pos, (lv_anim_exec_xcb_t)lv_obj_set_x);
        }
        /* 滚动方向为垂直滚动 */
        if (float_var->scroll_w == LV_DIR_VER) {
            /* 计算偏移量 */
            uint32_t delta_y = LV_ABS(point.y - float_var->click_y);
            /* 是回弹还是前进 */
            if (delta_y < rollback_y) {
                if (float_var->cover) {
                    lv_anim_set_values(&float_var->anim_pos, lv_obj_get_y(obj), 0);
                    lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_height(obj), 255);
                    float_var->cover = true;
                } else {
                    lv_anim_set_values(&float_var->anim_pos, lv_obj_get_y(obj), float_var->resume_y);
                    lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_height(obj), 0);
                    float_var->cover = false;
                }
            } else {
                if (float_var->cover) {
                    lv_anim_set_values(&float_var->anim_pos, lv_obj_get_y(obj), float_var->resume_y);
                    lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_height(obj), 0);
                    float_var->cover = false;
                } else {
                    lv_anim_set_values(&float_var->anim_pos, lv_obj_get_y(obj), 0);
                    lv_anim_set_values(&float_var->anim_opa, float_var->percent * 255 / lv_obj_get_height(obj), 255);
                    float_var->cover = true;
                }
            }
            lv_anim_set_exec_cb(&float_var->anim_pos, (lv_anim_exec_xcb_t)lv_obj_set_y);
        }
        lv_anim_set_exec_cb(&float_var->anim_opa, app_lv_ui_float_set_opa_recursion);
        lv_anim_start(&float_var->anim_pos);
        lv_anim_start(&float_var->anim_opa);
        /* 清除滑动记录 */
        float_var->scroll_w = LV_DIR_NONE;
        /* 没有活动浮窗 */
        if (!float_var->cover)
            app_lv_ui_float_act = NULL;
        break;
    }
    default:
        break;
    }
}

/*@brief     浮动子窗口
 *@param[in] parent 父窗口
 *@param[in] child  子窗口
 *@param[in] direct 相对初始方向
 */
static void app_lv_ui_float_link(lv_obj_t *parent, lv_obj_t *child, lv_dir_t direct)
{
    APP_SYS_ASSERT(parent != NULL);
    APP_SYS_ASSERT(child  != NULL);
    /* 重置浮窗参数 */
    app_lv_ui_float_t *float_var = NULL;
    /* 子窗口漂浮,成为父窗口的子窗口,样式固定 */
    lv_obj_add_flag(child, LV_OBJ_FLAG_FLOATING);
    lv_obj_set_parent(child, parent);
    lv_obj_set_style_opa(child, 0, 0);
    /* 设置初始位置 */
    switch (direct) {
    case LV_DIR_LEFT:
        float_var = &app_lv_ui_float_pl;
        float_var->resume_x = -lv_obj_get_width(child);
        float_var->resume_y = 0;
        break;
    case LV_DIR_RIGHT:
        float_var = &app_lv_ui_float_pr;
        float_var->resume_x = +lv_obj_get_width(child);
        float_var->resume_y = 0;
        break;
    case LV_DIR_TOP:
        float_var = &app_lv_ui_float_pt;
        float_var->resume_x = 0;
        float_var->resume_y = -lv_obj_get_height(child);
        break;
    case LV_DIR_BOTTOM:
        float_var = &app_lv_ui_float_pb;
        float_var->resume_x = 0;
        float_var->resume_y = +lv_obj_get_height(child);
        break;
    default:
        APP_SYS_ASSERT(true == false);
        break;
    }
    /* 俩个重叠的浮动子窗口断言 */
    APP_SYS_ASSERT(float_var->scene == NULL);
    const uint32_t period = 500;
    float_var->valid = true;
    float_var->scene = child;
    float_var->anim_period = period;
    lv_obj_set_x(float_var->scene, float_var->resume_x);
    lv_obj_set_y(float_var->scene, float_var->resume_y);
    /* 浮动子窗口动画重置 */
    lv_anim_init(&float_var->anim_pos);
    lv_anim_init(&float_var->anim_opa);
    lv_anim_set_var(&float_var->anim_pos,   float_var->scene);
    lv_anim_set_var(&float_var->anim_opa,   float_var->scene);
    lv_anim_set_time(&float_var->anim_pos,  float_var->anim_period);
    lv_anim_set_time(&float_var->anim_opa,  float_var->anim_period);
    /* 悬挂浮动子窗口事件响应 */
    lv_obj_add_event_cb(parent, app_lv_ui_float_event_cb, LV_EVENT_ALL, float_var);
    lv_obj_add_event_cb(child,  app_lv_ui_float_event_cb, LV_EVENT_ALL, float_var);
}

/*@brief  浮动子窗口禁用手势
 *@retval 浮动子窗口需要禁用指定手势
 */
bool app_lv_ui_float_cannot_gestrue(lv_dir_t direct)
{
    app_lv_ui_float_t *float_var = app_lv_ui_float_act;
    
    if ((direct & LV_DIR_LEFT) || (direct & LV_DIR_RIGHT))
    if ((float_var == &app_lv_ui_float_pl || float_var == &app_lv_ui_float_pr))
        return true;
    if ((direct & LV_DIR_TOP) || (direct & LV_DIR_BOTTOM))
    if ((float_var == &app_lv_ui_float_pt || float_var == &app_lv_ui_float_pb))
        return true;
    return false;
}

/*@brief     浮窗激活
 *@param[in] scene 场景
 */
void app_lv_ui_float_active(app_lv_scene_t *scene)
{
    if (scene->float_t != NULL) {
        /* 获得浮窗 */
        app_lv_scene_t* app_lv_ui_float = scene->float_t;
        /* 激活一个窗口 */
        app_lv_scene_active(scene->float_t, true);
        /* 父子窗口 */
        memset(&app_lv_ui_float_pt, 0, sizeof(app_lv_ui_float_t));
        app_lv_ui_float_link(scene->self, app_lv_ui_float->self, LV_DIR_TOP);
    }
    if (scene->float_b != NULL) {
        /* 获得浮窗 */
        app_lv_scene_t* app_lv_ui_float = scene->float_b;
        /* 激活一个窗口 */
        app_lv_scene_active(scene->float_b, true);
        /* 父子窗口 */
        memset(&app_lv_ui_float_pb, 0, sizeof(app_lv_ui_float_t));
        app_lv_ui_float_link(scene->self, app_lv_ui_float->self, LV_DIR_BOTTOM);
    }
    if (scene->float_l != NULL) {
        /* 获得浮窗 */
        app_lv_scene_t* app_lv_ui_float = scene->float_l;
        /* 激活一个窗口 */
        app_lv_scene_active(scene->float_l, true);
        /* 父子窗口 */
        memset(&app_lv_ui_float_pl, 0, sizeof(app_lv_ui_float_t));
        app_lv_ui_float_link(scene->self, app_lv_ui_float->self, LV_DIR_LEFT);
    }
    if (scene->float_r != NULL) {
        /* 获得浮窗 */
        app_lv_scene_t* app_lv_ui_float = scene->float_r;
        /* 激活一个窗口 */
        app_lv_scene_active(scene->float_r, true);
        /* 父子窗口 */
        memset(&app_lv_ui_float_pr, 0, sizeof(app_lv_ui_float_t));
        app_lv_ui_float_link(scene->self, app_lv_ui_float->self, LV_DIR_RIGHT);
    }
}

/*@brief     浮窗失活
 *@param[in] scene 场景
 */
void app_lv_ui_float_hidden(app_lv_scene_t *scene)
{
    if (scene->float_t != NULL) {
        /* 获得浮窗 */
        app_lv_scene_t* app_lv_ui_float = scene->float_t;
        /* 失活一个窗口 */
        app_lv_scene_hidden(scene->float_t, true);
        memset(&app_lv_ui_float_pt, 0, sizeof(app_lv_ui_float_t));
    }
    if (scene->float_b != NULL) {
        /* 获得浮窗 */
        app_lv_scene_t* app_lv_ui_float = scene->float_b;
        /* 失活一个窗口 */
        app_lv_scene_hidden(scene->float_b, true);
        memset(&app_lv_ui_float_pb, 0, sizeof(app_lv_ui_float_t));
    }
    if (scene->float_l != NULL) {
        /* 获得浮窗 */
        app_lv_scene_t* app_lv_ui_float = scene->float_l;
        /* 失活一个窗口 */
        app_lv_scene_hidden(scene->float_l, true);
        memset(&app_lv_ui_float_pl, 0, sizeof(app_lv_ui_float_t));
    }
    if (scene->float_r != NULL) {
        /* 获得浮窗 */
        app_lv_scene_t* app_lv_ui_float = scene->float_r;
        /* 失活一个窗口 */
        app_lv_scene_hidden(scene->float_r, true);
        memset(&app_lv_ui_float_pr, 0, sizeof(app_lv_ui_float_t));
    }
}
