/*实现目标:
 *    lvgl控件默认风格或者常规风格
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_lv_lib.h"

/*@brief 水平像素转换(未使用)
 *       将x/100像素点放大到指定显示器像素
 *@param x 水平像素
 *@retval 转换后的像素
 */
lv_coord_t app_lv_style_zoom_x(lv_coord_t x)
{
    return (lv_coord_t)((double)LV_HOR_RES / (double)100 * (double)x);
}

/*@brief 垂直像素转换(未使用)
 *       将y/100像素点放大到指定显示器像素
 *@param y 垂直像素
 *@retval 转换后的像素
 */
lv_coord_t app_lv_style_zoom_y(lv_coord_t y)
{
    return (lv_coord_t)((double)LV_VER_RES / (double)100 * (double)y);
}

/*@brief 水平宽度百分比
 *@param pct 水平像素
 *@retval 转换后的像素
 */
lv_coord_t app_lv_style_hor_pct(uint8_t pct)
{
    return (lv_coord_t)((double)LV_HOR_RES * pct / 100.0);
}

/*@brief 垂直宽度百分比
 *@param pct 垂直像素
 *@retval 转换后的像素
 */
lv_coord_t app_lv_style_ver_pct(uint8_t pct)
{
    return (lv_coord_t)((double)LV_VER_RES * pct / 100.0);
}

/*@brief 透明度更新
 *@param obj 对象
 *@param opa 透明度
 */
void app_lv_style_opa_update(lv_obj_t *obj, uint8_t opa)
{
    lv_obj_set_style_opa(obj, opa, 0);
    /* 需要递归??? */
    // return;
    
    for (uint32_t idx = 0; idx < lv_obj_get_child_cnt(obj); idx++)
         app_lv_style_opa_update(lv_obj_get_child(obj, idx), opa);
}

/*@brief 默认控件全局刷新动画
 *@param obj    对象
 *@param anim   动画
 *@param exec   动画刷新回调
 *@param repeat 更新次数
 *@param start  起始值
 *@param end    结束值
 *@param period 周期
 */
void app_lv_style_object_anim(lv_obj_t *obj, lv_anim_t *anim, lv_anim_exec_xcb_t exec, uint16_t repeat, int32_t start, int32_t end, uint32_t period)
{
    /* 初始化显示动画 */
    lv_anim_init(anim);
    lv_anim_set_var(anim, obj);
    lv_anim_set_exec_cb(anim, exec);
    lv_anim_set_repeat_count(anim, repeat);
    lv_anim_set_values(anim, start, end);
    lv_anim_set_time(anim, period);
    lv_anim_start(anim);
}

/*@brief 对齐中心内容域
 *@param obj   对象
 *@param align 部分对齐
 */
void app_lv_style_object_content_align(lv_obj_t *obj, lv_align_t align)
{
    lv_coord_t width  = lv_obj_get_width(obj);
    lv_coord_t height = lv_obj_get_height(obj);
    lv_coord_t content_width  = lv_obj_get_content_width(obj);
    lv_coord_t content_height = lv_obj_get_content_height(obj);
    /* 先清空pad */
    lv_obj_set_style_pad_all(obj, 0, 0);
    /* 这个默认是左上角对齐,所以只需要对齐俩个方向 */
    switch (align) {
    case LV_ALIGN_CENTER:
        lv_obj_set_style_pad_top(obj,  (width - content_width) / 2, 0);
        lv_obj_set_style_pad_left(obj, (width - content_width) / 2, 0);
        break;
    case LV_ALIGN_LEFT_MID:
        lv_obj_set_style_pad_top(obj,  (width - content_width) / 2, 0);
        lv_obj_set_style_pad_left(obj,  0, 0);
        break;
    case LV_ALIGN_RIGHT_MID:
        lv_obj_set_style_pad_top(obj,  (width - content_width) / 2, 0);
        lv_obj_set_style_pad_left(obj,  width - content_width, 0);
        break;
    case LV_ALIGN_TOP_MID:
        lv_obj_set_style_pad_top(obj,   0, 0);
        lv_obj_set_style_pad_left(obj, (width - content_width) / 2, 0);
        break;
    case LV_ALIGN_BOTTOM_MID:
        lv_obj_set_style_pad_top(obj,   width - content_width, 0);
        lv_obj_set_style_pad_left(obj, (width - content_width) / 2, 0);
        break;
    default:
        break;
    }
}
