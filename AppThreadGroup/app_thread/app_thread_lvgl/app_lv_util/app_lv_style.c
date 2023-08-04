/*实现目标:
 *    lvgl控件默认风格或者常规风格
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"

#include "lvgl.h"
#include "app_lv_event.h"
#include "app_lv_scene.h"
#include "app_lv_style.h"
#include "app_lv_multi_font.h"
#include "app_lv_event_ui.h"

/*@brief    默认控件风格
 *          黑色背景,无边框,无阴影,无间隙,无外部线
 *param[in] obj 控件
 */
void app_lv_style_object(lv_obj_t *obj)
{
    lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_set_style_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(obj, lv_color_black(), 0);
    lv_obj_set_style_border_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_side(obj, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_border_color(obj, lv_color_black(), 0);
    lv_obj_set_style_shadow_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_shadow_width(obj, 0, 0);
    lv_obj_set_style_shadow_color(obj, lv_color_black(), 0);
    lv_obj_set_style_outline_pad(obj, 0, 0);
    lv_obj_set_style_outline_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_outline_width(obj, 0, 0);
    lv_obj_set_style_outline_color(obj, lv_color_black(), 0);
    lv_obj_set_style_outline_color_filtered(obj, lv_color_black(), 0);
}

/*@brief 默认界面风格
 *       无主题风格,全屏幕,黑色背景全覆盖,无边界,无内部间隙
 *retval 返回场景(窗口,界面)
 */
lv_obj_t * app_lv_style_scene(void)
{
    lv_obj_t *scene = lv_obj_create(lv_scr_act());
    lv_obj_remove_style_all(scene);
    app_lv_style_object(scene);
    lv_obj_clear_flag(scene, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_size(scene, LV_HOR_RES, LV_VER_RES);
    return scene;
}

/*@brief    默认控件全局刷新动画
 *param[in] scene  界面
 *param[in] anim   动画
 *param[in] exec   动画刷新回调
 *param[in] repeat 更新次数
 *param[in] start  起始值
 *param[in] end    结束值
 *param[in] period 周期
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

/*@brief     对齐中心内容域
 *param[in]  parent 父控件
 *param[in]  align  部分对齐
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

/*@brief     文本标签
 *           黑色背景,无边框,白色滚动长文
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_label(lv_obj_t *parent)
{
    lv_obj_t *label = lv_label_create(parent);
    app_lv_style_object(label);
    lv_obj_set_style_bg_opa(label, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_label_set_text(label, "Null");
    return label;
}

/*@brief     默认组合文本
 *           自动扩展,左对齐,无缩进
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_spangroup(lv_obj_t *parent)
{
    lv_obj_t *spangroup = lv_spangroup_create(parent);
    app_lv_style_object(spangroup);
    lv_obj_set_style_bg_opa(spangroup, LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_opa(spangroup, LV_OPA_COVER, 0);
    lv_spangroup_set_align(spangroup, LV_TEXT_ALIGN_LEFT);
    lv_spangroup_set_mode(spangroup, LV_SPAN_MODE_EXPAND);
    lv_spangroup_set_overflow(spangroup, LV_SPAN_OVERFLOW_ELLIPSIS);
    lv_spangroup_set_indent(spangroup, 0);
    return spangroup;
}

/*@brief     默认按钮
 *           蓝色背景,无边框,标准弧度45
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_btn(lv_obj_t *parent)
{
    lv_obj_t *btn = lv_btn_create(parent);
    app_lv_style_object(btn);
    lv_obj_set_style_radius(btn, 45, 0);
    lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_BLUE), 0);
    return btn;
}

/*@brief 显示标签块
 *       无圆角,外部线白色,边界线黑色
 */
lv_obj_t * app_lv_style_label_block(lv_obj_t *parent, lv_coord_t pad_with, lv_coord_t border_width, lv_coord_t outline_width)
{
    lv_obj_t *label = app_lv_style_label(parent);
    lv_label_set_text(label, "");
    lv_obj_set_style_radius(label, 0, 0);
    lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(label, pad_with, 0);
    lv_obj_set_style_border_side(label, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_width(label, border_width, 0);
    lv_obj_set_style_border_color(label, lv_color_black(), 0);
    lv_obj_set_style_outline_width(label, outline_width, 0);
    lv_obj_set_style_outline_color(label, lv_color_white(), 0);
    lv_obj_set_style_outline_color_filtered(label, lv_color_white(), 0);
    return label;
}

/*@brief 显示标签块
 *       无圆角,外部线白色,边界线黑色
 */
lv_obj_t * app_lv_style_label_block2(lv_obj_t *parent, lv_obj_t **container, lv_coord_t pad_with, lv_coord_t border_width, lv_coord_t outline_width)
{
    *container = app_lv_style_label(parent);
    lv_label_set_text(*container, "");
    lv_obj_set_style_radius(*container, 0, 0);
    lv_obj_set_style_pad_all(*container, pad_with, 0);
    lv_obj_set_style_border_side(*container, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_width(*container, border_width, 0);
    lv_obj_set_style_border_color(*container, lv_color_black(), 0);
    lv_obj_set_style_outline_width(*container, outline_width, 0);
    lv_obj_set_style_outline_color(*container, lv_color_white(), 0);
    lv_obj_set_style_outline_color_filtered(*container, lv_color_white(), 0);
    lv_obj_t *obj = lv_obj_create(*container);
    app_lv_style_object(obj);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    return obj;
}

/*@brief 显示按钮块
 *       无圆角,外部线白色,边界线黑色
 */
lv_obj_t * app_lv_style_btn_block(lv_obj_t *parent, lv_coord_t pad_with, lv_coord_t border_width, lv_coord_t outline_width)
{
    lv_obj_t *btn = app_lv_style_btn(parent);
    lv_obj_set_style_radius(btn, 0, 0);
    lv_obj_set_style_pad_all(btn, pad_with, 0);
    lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_width(btn, border_width, 0);
    lv_obj_set_style_border_color(btn, lv_color_black(), 0);
    lv_obj_set_style_outline_width(btn, outline_width, 0);
    lv_obj_set_style_outline_color(btn, lv_color_white(), 0);
    lv_obj_set_style_outline_color_filtered(btn, lv_color_white(), 0);
    return btn;
}

/*@brief 显示按钮块
 *       无圆角,外部线白色,边界线黑色
 */
lv_obj_t * app_lv_style_btn_block2(lv_obj_t *parent, lv_obj_t **container, lv_coord_t pad_with, lv_coord_t border_width, lv_coord_t outline_width)
{
    *container = app_lv_style_btn(parent);
    lv_obj_remove_style_all(*container);
    lv_obj_set_style_radius(*container, 0, 0);
    lv_obj_set_style_pad_all(*container, pad_with, 0);
    lv_obj_set_style_bg_opa(*container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_side(*container, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_width(*container, border_width, 0);
    lv_obj_set_style_border_color(*container, lv_color_black(), 0);
    lv_obj_set_style_outline_width(*container, outline_width, 0);
    lv_obj_set_style_outline_color(*container, lv_color_white(), 0);
    lv_obj_set_style_outline_color_filtered(*container, lv_color_white(), 0);
    lv_obj_t *obj = lv_obj_create(*container);
    app_lv_style_object(obj);
    lv_obj_set_size(obj, lv_pct(100), lv_pct(100));
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_align(obj, LV_ALIGN_CENTER, 0, 0);
    return obj;
}

/*@brief     按钮框
 *           黑色背景,指定色边框,标准弧度45
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_btn_box(lv_obj_t *parent, lv_coord_t width, lv_color_t color)
{
    lv_obj_t *btn = lv_btn_create(parent);
    app_lv_style_object(btn);
    lv_obj_set_style_radius(btn, 45, 0);
    lv_obj_set_style_border_opa(btn, LV_OPA_COVER, 0);
    lv_obj_set_style_border_side(btn, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_width(btn, width, 0);
    lv_obj_set_style_border_color(btn, color, 0);
    return btn;
}

/*@brief     特效按钮矩阵
 *           蓝色背景,无边框,默认全响应
 *param[in]  parent  父控件
 *param[in]  map     文本矩阵
 */
lv_obj_t * app_lv_style_btnmatrix(lv_obj_t *parent, const char *map[])
{
    lv_obj_t *btnmatrix = lv_btnmatrix_create(parent);
    app_lv_style_object(btnmatrix);
    lv_obj_set_style_pad_all(btnmatrix, 20, 0);
    lv_obj_set_style_pad_row(btnmatrix, app_lv_style_ver_pct(2), 0);
    lv_obj_set_style_pad_column(btnmatrix, app_lv_style_hor_pct(2), 0);
    lv_obj_set_style_height(btnmatrix, app_lv_style_ver_pct(10), LV_PART_ITEMS);
    lv_obj_set_style_radius(btnmatrix, 45, LV_PART_ITEMS);
    lv_obj_set_style_width(btnmatrix, app_lv_style_hor_pct(30), LV_PART_ITEMS);
    lv_obj_set_style_bg_color(btnmatrix, lv_palette_main(LV_PALETTE_BLUE), LV_PART_ITEMS);
    lv_btnmatrix_set_map(btnmatrix, map);
    lv_btnmatrix_set_btn_ctrl_all(btnmatrix, LV_BTNMATRIX_CTRL_CHECKABLE);
    return btnmatrix;
}

/*@brief     特效文本框
 *           边框蓝色,大小3,光标红色,1秒闪烁,默认文本灰色,文本蓝色,指针选定
 *param[in]  parent  父控件
 */

lv_obj_t * app_lv_style_textarea(lv_obj_t *parent)
{
    lv_obj_t *textarea = lv_textarea_create(parent);
    app_lv_style_object(textarea);
    lv_obj_set_style_anim_time(textarea, 500, LV_PART_CURSOR);
    lv_obj_set_style_border_side(textarea,LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_width(textarea, 3, 0);
    lv_obj_set_style_border_color(textarea, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_text_font(textarea,  app_lv_multi_font(app_lv_multi_font_24), 0);
    lv_obj_set_style_text_color(textarea, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_text_color(textarea, lv_palette_main(LV_PALETTE_GREY), LV_PART_TEXTAREA_PLACEHOLDER);
    lv_obj_set_style_text_color(textarea, lv_palette_main(LV_PALETTE_RED), LV_PART_CURSOR);
    lv_obj_set_style_text_align(textarea, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_scrollbar_mode(textarea, LV_SCROLLBAR_MODE_OFF);
    lv_textarea_set_one_line(textarea, true);
    lv_textarea_set_cursor_click_pos(textarea, true);
    lv_textarea_set_placeholder_text(textarea, "......");
    lv_textarea_set_text(textarea, "");
    return textarea;
}

/*@brief     特效滚轮
 *           默认遮罩,被选值(白色,内部字体28),非选值(灰色,内部字体22),轮滚
 *param[in]  parent  父控件
 *param[in]  width   滚轮宽度
 *param[in]  options 滚轮显示值
 *param[in]  vis_row 滚轮默认显示个数
 */
lv_obj_t * app_lv_style_roller(lv_obj_t *parent, lv_coord_t width, const char *options, uint8_t vis_row)
{
    lv_obj_t *roller = lv_roller_create(parent);
    app_lv_style_object(roller);
    lv_obj_add_event_cb(roller, app_lv_event_ui_roller_mask_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_width(roller, width);
    lv_obj_set_style_bg_opa(roller, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(roller, LV_OPA_TRANSP, LV_PART_SELECTED);
    lv_obj_set_style_text_font(roller, app_lv_ui_font(32), 0);
    lv_obj_set_style_text_font(roller, app_lv_ui_font(48), LV_PART_SELECTED);
    lv_obj_set_style_text_align(roller, LV_TEXT_ALIGN_CENTER, 0);
    lv_roller_set_options(roller, options, LV_ROLLER_MODE_NORMAL);//LV_ROLLER_MODE_INFINITE);///* 滚动模式显示异常 */
    lv_roller_set_visible_row_count(roller, vis_row);
    lv_obj_align(roller, LV_ALIGN_CENTER, 0, 0);
    return roller;
}

/*@brief     特效圆环
 *           无指示点,禁用点击,4级暗色,常规旋转
 *param[in]  parent   父控件
 *param[in]  size     周长
 *param[in]  width    环长
 *param[in]  color    变色
 *param[in]  min      最小值
 *param[in]  max      最大值
 *param[in]  start    起点值
 *param[in]  end      结束值
 *param[in]  rotation 旋转点
 */
lv_obj_t * app_lv_style_arc(lv_obj_t *parent, lv_coord_t size, lv_coord_t width, lv_palette_t color, int16_t min, int16_t max, uint16_t start, uint16_t end, uint16_t rotation)
{
    lv_obj_t *arc = lv_arc_create(parent);
    app_lv_style_object(arc);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(arc, size, size);
    lv_obj_set_style_bg_opa(arc, LV_OPA_TRANSP, 0);
    lv_obj_set_style_arc_width(arc, width, 0);
    lv_obj_set_style_arc_width(arc, width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, lv_palette_darken(color, 4), 0);
    lv_obj_set_style_arc_color(arc, lv_palette_lighten(color, 1), LV_PART_INDICATOR);
    lv_arc_set_mode(arc, LV_ARC_MODE_NORMAL);
    lv_arc_set_range(arc, min, max);
    lv_arc_set_bg_angles(arc, start, end);
    lv_arc_set_rotation(arc, rotation);
    return arc;
}

/*@brief     顶部居中标题标签
 *           黑色背景,无边框,白色滚动长文
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_label_title(lv_obj_t *parent)
{
    lv_obj_t *label = app_lv_style_label(parent);
    lv_obj_set_size(label, app_lv_style_hor_pct(90), app_lv_style_ver_pct(8));
    lv_obj_set_style_text_font(label, app_lv_multi_font(app_lv_multi_font_24), 0);
    return label;
}

/*@brief     顶部风格
 *           左部按钮,左中部标题文本,右部时间文本,按钮返回上一层
 *param[in]  parent 父控件
 *param[out] time  左中部时钟实例
 *param[out] title 右上部文本实例
 *retval     顶部风格控件
 */
lv_obj_t * app_lv_style_title(lv_obj_t *parent, lv_obj_t **time, lv_obj_t **title)
{
    APP_SYS_ASSERT(time  != NULL);
    APP_SYS_ASSERT(title != NULL);
    /* 组合控件 */
    lv_obj_t *obj_box = lv_obj_create(parent);
    app_lv_style_object(obj_box);
    lv_obj_set_size(obj_box, LV_HOR_RES, app_lv_style_ver_pct(10));
    lv_obj_set_style_bg_opa(obj_box, LV_OPA_TRANSP, 0);
    lv_obj_align(obj_box, LV_ALIGN_TOP_MID, 0, 0);
    /* 左上角按钮 */
    lv_obj_t *btn = app_lv_style_btn(obj_box);
    lv_obj_set_size(btn, app_lv_style_hor_pct(15), app_lv_style_ver_pct(8));
    lv_obj_add_event_cb(btn, app_lv_event_ui_backtrack_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_align(btn, LV_ALIGN_LEFT_MID, app_lv_style_hor_pct(5), 0);
    lv_obj_t *label = app_lv_style_label(btn);
    lv_label_set_text_static(label, LV_SYMBOL_BACKSPACE);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    /* 中部文本 */
    *title = app_lv_style_label(obj_box);
    lv_obj_set_size(*title, app_lv_style_hor_pct(40), app_lv_style_ver_pct(8));
    lv_obj_set_style_pad_top(*title, app_lv_style_ver_pct(2), 0);
    lv_obj_set_style_text_align(*title, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align_to(*title, btn, LV_ALIGN_OUT_RIGHT_MID, app_lv_style_hor_pct(1), 0);
    lv_label_set_text(*title, "Null");
    /* 右上角时间 */
    *time = app_lv_style_label(obj_box);
    lv_obj_set_size(*time, app_lv_style_hor_pct(40), app_lv_style_ver_pct(8));
    lv_obj_set_style_pad_top(*time, app_lv_style_ver_pct(2), 0);
    lv_obj_set_style_text_align(*time, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(*time, LV_ALIGN_RIGHT_MID, -app_lv_style_hor_pct(5), 0);
    lv_label_set_text(*time, "00:00");
    /* 组合控件 */
    return obj_box;
}

/*@brief     底部单按钮
 *           无阴影,默认扩散,蓝色背景(按钮,内部字体22)
 *param[in]  parent 父控件
 *param[out] btn 下部按钮实例
 *param[out] lbl 下部文本实例
 */
lv_obj_t * app_lv_style_one_btn(lv_obj_t *parent, lv_obj_t **btn_c, lv_obj_t **lbl_c)
{
    APP_SYS_ASSERT(btn_c != NULL);
    APP_SYS_ASSERT(lbl_c != NULL);
    /* 下部按钮 */
    lv_obj_t *btn = lv_obj_create(parent);
    app_lv_style_object(btn);
    lv_obj_set_size(btn, LV_HOR_RES, app_lv_style_ver_pct(25));
    lv_obj_set_style_pad_all(btn, app_lv_style_hor_pct(1), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    /* 中心按键 */
    *btn_c = app_lv_style_btn(btn);
    lv_obj_set_size(*btn_c, app_lv_style_hor_pct(90), app_lv_style_ver_pct(20));
    lv_obj_align(*btn_c, LV_ALIGN_CENTER, 0, 0);
    *lbl_c = app_lv_style_label_title(*btn_c);
    lv_obj_align(*lbl_c, LV_ALIGN_CENTER, 0, 0);
    return btn;
}

/*@brief     底部双按钮组
 *           左部按钮,右部按钮,无阴影,默认扩散,蓝色背景(按钮,内部字体22)
 *param[in]  parent 父控件
 *param[out] btn_l 左下部按钮实例
 *param[out] lbl_l 左下部文本实例
 *param[out] btn_r 右下部按钮实例
 *param[out] lbl_r 右下部文本实例
 */
lv_obj_t * app_lv_style_two_btns(lv_obj_t *parent, lv_obj_t **btn_l, lv_obj_t **lbl_l, lv_obj_t **btn_r, lv_obj_t **lbl_r)
{
    APP_SYS_ASSERT(btn_l != NULL);
    APP_SYS_ASSERT(lbl_l != NULL);
    APP_SYS_ASSERT(btn_r != NULL);
    APP_SYS_ASSERT(lbl_r != NULL);
    /* 下部按钮 */
    lv_obj_t *btn = lv_obj_create(parent);
    app_lv_style_object(btn);
    lv_obj_set_size(btn, LV_HOR_RES, app_lv_style_ver_pct(25));
    lv_obj_set_style_pad_all(btn, app_lv_style_hor_pct(1), 0);
    lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, 0);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    /* 左下角按钮 */
    *btn_l = app_lv_style_btn(btn);
    lv_obj_set_size(*btn_l, app_lv_style_hor_pct(45), app_lv_style_ver_pct(20));
    lv_obj_align(*btn_l, LV_ALIGN_LEFT_MID, app_lv_style_hor_pct(1), 0);
    *lbl_l = app_lv_style_label_title(*btn_l);
    lv_obj_align(*lbl_l, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(*lbl_l, "L_Btn");
    /* 右下角按钮 */
    *btn_r = app_lv_style_btn(btn);
    lv_obj_set_size(*btn_r, app_lv_style_hor_pct(45), app_lv_style_ver_pct(20));
    lv_obj_align(*btn_r, LV_ALIGN_RIGHT_MID, -app_lv_style_hor_pct(1), 0);
    *lbl_r = app_lv_style_label_title(*btn_r);
    lv_obj_align(*lbl_r, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text(*lbl_r, "R_Btn");
    /* 下部按钮 */
    return btn;
}

/*@brief     特效加载圆环
 *           速度300ms,加载条覆盖1/4,匀速,底色绿色,加载色蓝色
 *param[in]  parent  父控件
 *param[in]  size    圆环直径
 *param[in]  side    圆环宽度
 */
lv_obj_t * app_lv_style_loading_spinner(lv_obj_t *parent, lv_coord_t size, lv_coord_t side)
{
    lv_obj_t *spinner = lv_spinner_create(parent, 300, 90);
    app_lv_style_object(spinner);
    lv_obj_remove_style(spinner, NULL, LV_PART_KNOB);
    lv_obj_set_size(spinner, size, size);
    lv_arc_set_bg_angles(spinner, 0, 360);
    lv_obj_set_style_arc_width(spinner, side, 0);
    lv_obj_set_style_arc_width(spinner, side, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(spinner, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_arc_color(spinner, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_arc_set_mode(spinner, LV_ARC_MODE_NORMAL);
}

/*@brief     特效加载进度条
 *           默认加载范围0~100,边界色蓝色,指针渐变(蓝色,绿色),内部间隙4,边界间隙1,弧度45
 *param[in]  parent  父控件
 *param[in]  width   进度条宽度
 *param[in]  height  进度条高度
 *param[in]  grad    加载方向
 */
lv_obj_t * app_lv_style_loading_bar(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, lv_grad_dir_t grad)
{
    lv_obj_t *bar = lv_bar_create(parent);
    app_lv_style_object(bar);
    lv_obj_set_size(bar, width, height);
    lv_obj_set_style_pad_all(bar, 4, 0);
    lv_obj_set_style_radius(bar, 45, 0);
    lv_obj_set_style_radius(bar, 45, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(bar, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(bar, lv_color_black(), 0);
    lv_obj_set_style_bg_color(bar, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(bar, lv_palette_main(LV_PALETTE_GREEN), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(bar, grad, LV_PART_INDICATOR);
    lv_obj_set_style_border_side(bar, LV_BORDER_SIDE_FULL, 0);
    lv_obj_set_style_border_width(bar, 1, 0);
    lv_obj_set_style_border_color(bar, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_bar_set_range(bar, 0, 100);
    return bar;
}

/*@brief     渐变折线图表
 *           无点折线图,水平线5等分,0~100百分比数据点,无刻度标签,固定宽高,24条数据
 *param[in]  parent  父控件
 */
lv_obj_t * app_lv_style_fade_chart(lv_obj_t *parent)
{
    lv_obj_t *chart = lv_chart_create(parent);
    app_lv_style_object(chart);
    lv_obj_set_size(chart, app_lv_style_hor_pct(90), app_lv_style_ver_pct(30));
    lv_obj_add_event_cb(chart, app_lv_event_ui_chart_fade_cb, LV_EVENT_ALL, NULL);
    //lv_obj_set_style_bg_color(chart, lv_palette_darken(LV_PALETTE_GREY, 4), 0);
    lv_obj_set_style_size(chart, 1, LV_PART_INDICATOR);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 24);
    lv_chart_set_div_line_count(chart, 5, 0);
    lv_chart_set_update_mode(chart, LV_CHART_UPDATE_MODE_CIRCULAR);
    return chart;
}
