#ifndef SCUI_CUSTOM_DRAW_H
#define SCUI_CUSTOM_DRAW_H

// custom插件:
#include "scui_plug_coupler.h"

typedef struct {
    /*************************************************************************/
    scui_event_t *event;            // 绘制事件
    scui_area_t  *clip;             // 绘制区域
    union {
    /*************************************************************************/
    struct {
        scui_color_t color;         // 颜色(.color_l,.color_d,)
        uint8_t     *data;          // url网址链接字符串
        uint32_t     size:31;       // 字符串长度
        uint32_t     cover:1;       // 背景覆盖
    } qrcode;
    struct {
        scui_color_t color;         // 颜色(.color_l,.color_d,)
        uint8_t     *data;          // url网址链接字符串
        uint32_t     size:31;       // 字符串长度
        uint32_t     cover:1;       // 背景覆盖
    } barcode;
    struct {
        scui_handle_t spinner;      // 图像句柄(alpha图:环)
        scui_handle_t edge;         // 图像句柄(alpha图:边界点)
        scui_color_t  color;        // 图像源色调(.color_l,.color_d,.color_f, .filter,)
        scui_coord_t  percent;      // 旋转百分比(0~100)
        scui_coord_t  angle_s;      // 旋转参考点(参考值270度)
        scui_coord_t  angle_l;      // 旋转参考点(参考值270度)
        scui_coord_t  way;          // 旋转方向(顺时针:+1;逆时针:-1;)
    } spinner;
    struct {
        scui_handle_t bar;          // 图像句柄(背景图)
        scui_handle_t edge;         // 图像句柄(边界点)
        scui_color_t  color_bar;    // 图像源色调(alpha图使用)
        scui_color_t  color_edge;   // 图像源色调(alpha图使用)
        scui_coord_t  vmin;         // 最小值(默认可为百分比:0)
        scui_coord_t  vmax;         // 最大值(默认可为百分比:100)
        scui_coord_t  cmin;         // 当前最小值
        scui_coord_t  cmax;         // 当前最大值
        scui_handle_t dist;         // 宽度或高度
        bool          way;          // 方向(0:水平方向;1:垂直方向)
    } slider;
    struct {
        scui_handle_t wait;         // 图像句柄(未选中)
        scui_handle_t focus;        // 图像句柄(选中)
        scui_color_t  color_wait;   // 图像源色调(alpha图使用)
        scui_color_t  color_focus;  // 图像源色调(alpha图使用)
        scui_handle_t count;        // 导航点数量
        scui_handle_t index;        // 目标索引(选中)
        scui_handle_t span;         // 间隙
        bool          way;          // 方向(0:水平方向;1:垂直方向)
    } indicator;
    struct {
        scui_handle_t image;        // 图像句柄
        scui_color_t  color;        // 图像源色调(alpha图使用)
        scui_point_t *center;       // 旋转中心
        scui_coord_t  radius;       // 旋转半径
        scui_coord_t  angle;        // 旋转角度(顺时针旋转:+,逆时针旋转:-)
    } ring_edge;
    struct {
        scui_handle_t *image;       // 图像句柄
        scui_color_t   color;       // 图像源色调(alpha图使用)
        scui_coord_t   span;        // 图像间隙
        scui_coord_t   num;         // 图像数量
        bool           way;         // 方向(0:水平方向;1:垂直方向)
    } image_text;
    struct {
        scui_handle_t image[4];     // 图像句柄(左上角,右上角,左下角,右下角)
        scui_color_t  color;        // 图像源色调(alpha图使用)
        scui_coord_t  delta;        // 边界填充线:
                                    //   0:忽略(复杂图像集成)
                                    //  -1:完全填充(全填充圆角矩形)
                                    //  其他:边界填充(空心圆角矩形)
    } image_crect4;
    
    /*************************************************************************/
    };
} scui_custom_draw_dsc_t;

/*@brief 自定义控件:插件:二维码生成器
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_qrcode(scui_custom_draw_dsc_t *draw_dsc);

/*@brief 自定义控件:插件:条形码生成器
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_barcode(scui_custom_draw_dsc_t *draw_dsc);

/*@brief 自定义控件:插件:加载圆环
 *@param draw_graph 绘制参数实例
 */
void scui_custom_draw_spinner(scui_custom_draw_dsc_t *draw_dsc);

/*@brief 自定义控件:插件:进度条,滚动条
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_slider(scui_custom_draw_dsc_t *draw_dsc);

/*@brief 自定义控件:插件:导航点
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_indicator(scui_custom_draw_dsc_t *draw_dsc);

/*@brief 自定义控件:插件:绕圆旋转图像
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_ring_edge(scui_custom_draw_dsc_t *draw_dsc);

/*@brief 自定义控件:插件:图像连续绘制
 *       一般主要用于绘制连续数字符号图片
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_image_text(scui_custom_draw_dsc_t *draw_dsc);

/*@brief 按钮控件绘制(四个角使用图像绘制)
 *@param draw_dsc 绘制参数实例
 */
void scui_custom_draw_image_crect4(scui_custom_draw_dsc_t *draw_dsc);

#endif
