#ifndef APP_LV_UI_STYLE_OLD_H
#define APP_LV_UI_STYLE_OLD_H

/*@brief     文本标签
 *           黑色背景,无边框,白色滚动长文
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_label(lv_obj_t *parent);

/*@brief     默认组合文本
 *           自动扩展,左对齐,无缩进
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_spangroup(lv_obj_t *parent);

/*@brief     默认按钮
 *           蓝色背景,无边框,标准弧度45
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_btn(lv_obj_t *parent);

/*@brief 显示标签块
 *       无圆角,外部线白色,边界线黑色
 */
lv_obj_t * app_lv_style_label_block(lv_obj_t *parent, lv_coord_t pad_with, lv_coord_t border_width, lv_coord_t outline_width);

/*@brief 显示标签块
 *       无圆角,外部线白色,边界线黑色
 */
lv_obj_t * app_lv_style_label_block2(lv_obj_t *parent, lv_obj_t **container, lv_coord_t pad_with, lv_coord_t border_width, lv_coord_t outline_width);

/*@brief 显示按钮块(带点击特效)
 *       无圆角,外部线白色,边界线黑色
 */
lv_obj_t * app_lv_style_btn_block(lv_obj_t *parent, lv_coord_t pad_with, lv_coord_t border_width, lv_coord_t outline_width);

/*@brief 显示按钮块
 *       无圆角,外部线白色,边界线黑色
 */
lv_obj_t * app_lv_style_btn_block2(lv_obj_t *parent, lv_obj_t **container, lv_coord_t pad_with, lv_coord_t border_width, lv_coord_t outline_width);

/*@brief     按钮框
 *           黑色背景,指定色边框,标准弧度45
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_btn_box(lv_obj_t *parent, lv_coord_t width, lv_color_t color);

/*@brief     特效按钮矩阵
 *           蓝色背景,无边框,默认全响应
 *param[in]  parent  父控件
 *param[in]  map     文本矩阵
 */
lv_obj_t * app_lv_style_btnmatrix(lv_obj_t *parent, const char *map[]);

/*@brief     特效文本框
 *           边框蓝色,大小3,光标红色,1秒闪烁,默认文本灰色,文本蓝色,指针选定
 *param[in]  parent  父控件
 */
lv_obj_t * app_lv_style_textarea(lv_obj_t *parent);

/*@brief     特效滚轮
 *           默认遮罩,被选值(白色,内部字体28),非选值(灰色,内部字体22),轮滚
 *param[in]  parent  父控件
 *param[in]  width   滚轮宽度
 *param[in]  options 滚轮显示值
 *param[in]  vis_row 滚轮默认显示个数
 */
lv_obj_t * app_lv_style_roller(lv_obj_t *parent, lv_coord_t width, const char *options, uint8_t vis_row);

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
lv_obj_t * app_lv_style_arc(lv_obj_t *parent, lv_coord_t size, lv_coord_t width, lv_palette_t color, int16_t min, int16_t max, uint16_t start, uint16_t end, uint16_t rotation);

/*@brief     顶部居中标题标签
 *           黑色背景,无边框,白色滚动长文
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_style_label_title(lv_obj_t *parent);

/*@brief     顶部风格
 *           左部按钮,左中部标题文本,右部时间文本,按钮返回上一层
 *param[in]  parent 父控件
 *param[out] time  左中部时钟实例
 *param[out] title 右上部文本实例
 *retval     顶部风格控件
 */
lv_obj_t * app_lv_style_title(lv_obj_t *parent, lv_obj_t **time, lv_obj_t **title);

/*@brief     底部单按钮
 *           无阴影,默认扩散,蓝色背景(按钮,内部字体22)
 *param[in]  parent 父控件
 *param[out] btn 下部按钮实例
 *param[out] lbl 下部文本实例
 */
lv_obj_t * app_lv_style_one_btn(lv_obj_t *parent, lv_obj_t **btn_c, lv_obj_t **lbl_c);

/*@brief     底部双按钮组
 *           左部按钮,右部按钮,无阴影,默认扩散,蓝色背景(按钮,内部字体22)
 *param[in]  parent 父控件
 *param[out] btn_l 左下部按钮实例
 *param[out] lbl_l 左下部文本实例
 *param[out] btn_r 右下部按钮实例
 *param[out] lbl_r 右下部文本实例
 */
lv_obj_t * app_lv_style_two_btns(lv_obj_t *parent, lv_obj_t **btn_l, lv_obj_t **lbl_l, lv_obj_t **btn_r, lv_obj_t **lbl_r);

/*@brief     特效加载圆环
 *           速度300ms,加载条覆盖1/4,匀速,底色绿色,加载色蓝色
 *param[in]  parent  父控件
 *param[in]  size    圆环直径
 *param[in]  side    圆环宽度
 */
lv_obj_t * app_lv_style_loading_spinner(lv_obj_t *parent, lv_coord_t size, lv_coord_t side);

/*@brief     特效加载进度条
 *           默认加载范围0~100,边界色蓝色,指针渐变(蓝色,绿色),内部间隙4,边界间隙1,弧度45
 *param[in]  parent  父控件
 *param[in]  width   进度条宽度
 *param[in]  height  进度条高度
 *param[in]  grad    加载方向
 */
lv_obj_t * app_lv_style_loading_bar(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, lv_grad_dir_t grad);

/*@brief     渐变折线图表
 *           无点折线图,水平线5等分,0~100百分比数据点,无刻度标签,固定宽高,24条数据
 *param[in]  parent  父控件
 */
lv_obj_t * app_lv_style_fade_chart(lv_obj_t *parent);

#endif
