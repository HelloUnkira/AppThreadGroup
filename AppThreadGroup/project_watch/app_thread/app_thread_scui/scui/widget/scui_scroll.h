#ifndef SCUI_SCROLL_H
#define SCUI_SCROLL_H

/*@brief 滚动控件布局风格
 *       水平自动布局会调整所有子控件的(x,y,h)
 *       垂直自动布局会调整所有子控件的(x,y,w)
 */
typedef enum {
    scui_scroll_style_none,         /* 自由布局 */
    scui_scroll_style_magnetic,     /* 自由布局,自动靠边 */
    scui_scroll_style_ver,          /* 自动布局,水平方向,回弹效果 */
    scui_scroll_style_hor,          /* 自动布局,垂直方向,回弹效果 */
} scui_scroll_style_t;

typedef struct {
    scui_widget_t widget;
} scui_scroll_t;

typedef struct {
    scui_widget_maker_t widget;
    scui_scroll_style_t style;          /* 布局风格 */
    scui_coord_t        springback;     /* 回弹效果系数 */
    scui_coord_t        space;          /* 控件间隙(自动布局) */
    uint8_t             loop:1;         /* 滚动循环(自动布局,与回弹效果互斥) */
} scui_scroll_maker_t;

/*@brief 可滚动控件创建
 *@param maker  可滚动控件创建参数
 *@param handle 可滚动控件句柄
 *@param layout 通过布局创建
 */
void scui_scroll_create(scui_scroll_maker_t *maker, scui_handle_t *handle, bool layout);

/*@brief 可滚动控件销毁
 *@param handle 可滚动控件句柄
 */
void scui_scroll_destroy(scui_handle_t handle);

#endif
