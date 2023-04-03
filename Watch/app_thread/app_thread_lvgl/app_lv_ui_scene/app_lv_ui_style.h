#ifndef APP_LV_UI_STYLE_H
#define APP_LV_UI_STYLE_H

/*@brief     水平像素转换(未使用)
 *           将x/100像素点放大到指定显示器像素
 *@param[in] 水平像素
 *@retval    转换后的像素
 */
static inline lv_coord_t app_lv_ui_zoom_x(lv_coord_t x)
{
    return (lv_coord_t)((double)LV_HOR_RES / (double)100 * (double)x);
}

/*@brief     垂直像素转换(未使用)
 *           将y/100像素点放大到指定显示器像素
 *@param[in] 垂直像素
 *@retval    转换后的像素
 */
static inline lv_coord_t app_lv_ui_zoom_y(lv_coord_t y)
{
    return (lv_coord_t)((double)LV_VER_RES / (double)100 * (double)y);
}

/*@brief    默认控件风格
 *          黑色背景,无边框,无阴影,无间隙
 *param[in] obj 控件
 */
void app_lv_ui_style_object(lv_obj_t *obj);

/*@brief    默认全局刷新动画
 *          立即初始化并启动,用于数据资源常态刷新
 *param[in] scene  界面
 *param[in] anim   动画
 *param[in] exec   动画刷新回调
 *param[in] repeat 更新次数
 *param[in] start  起始值
 *param[in] end    结束值
 *param[in] period 周期
 */
void app_lv_ui_style_object_anim(lv_obj_t *obj, lv_anim_t *anim, lv_anim_exec_xcb_t exec, uint16_t repeat, int32_t start, int32_t end, uint32_t period);

/*@brief 默认界面风格
 *       无主题风格,全屏幕,黑色背景全覆盖,无边界,无内部间隙
 *retval 返回场景(窗口,界面)
 */
lv_obj_t * app_lv_ui_style_scene(void);

/*@brief     文本标签
 *           黑色背景,无边框,白色滚动长文
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_ui_style_label(lv_obj_t *parent);

/*@brief     大号文本标签
 *           黑色背景,无边框,白色滚动长文
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_ui_style_label_large(lv_obj_t * parent);

/*@brief     默认按钮
 *           蓝色背景,无边框,标准弧度45
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_ui_style_btn(lv_obj_t *parent);

/*@brief     特效按钮矩阵
 *           蓝色背景,无边框,默认全响应
 *param[in]  parent  父控件
 *param[in]  map     文本矩阵
 */
lv_obj_t * app_lv_ui_style_btnmatrix(lv_obj_t *parent, const char *map[]);

/*@brief     特效文本框
 *           边框蓝色,大小3,光标红色,1秒闪烁,默认文本灰色,文本蓝色,指针选定
 *param[in]  parent  父控件
 */
lv_obj_t * app_lv_ui_style_textarea(lv_obj_t *parent);

/*@brief     特效滚轮
 *           默认遮罩,被选值(白色,内部字体28),非选值(灰色,内部字体22),轮滚
 *param[in]  parent  父控件
 *param[in]  width   滚轮宽度
 *param[in]  options 滚轮显示值
 *param[in]  vis_row 滚轮默认显示个数
 */
lv_obj_t * app_lv_ui_style_roller(lv_obj_t *parent, lv_coord_t width, const char *options, uint8_t vis_row);

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
lv_obj_t * app_lv_ui_style_arc(lv_obj_t *parent, lv_coord_t size, lv_coord_t width, lv_palette_t color, int16_t min, int16_t max, uint16_t start, uint16_t end, uint16_t rotation);

/*@brief     顶部居中标题标签
 *           黑色背景,无边框,白色滚动长文
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_ui_style_label_title(lv_obj_t *parent);

/*@brief     顶部风格
 *           左部按钮,左中部标题文本,右部时间文本,按钮返回上一层
 *param[in]  parent 父控件
 *param[out] time  左中部时钟实例
 *param[out] title 右上部文本实例
 *retval     顶部风格控件
 */
lv_obj_t * app_lv_ui_style_title(lv_obj_t *parent, lv_obj_t **time, lv_obj_t **title);

/*@brief     底部单按钮
 *           无阴影,默认扩散,蓝色背景(按钮,内部字体22)
 *param[in]  parent 父控件
 *param[out] btn 下部按钮实例
 *param[out] lbl 下部文本实例
 */
lv_obj_t * app_lv_ui_style_one_btn(lv_obj_t *parent, lv_obj_t **btn_c, lv_obj_t **lbl_c);

/*@brief     底部双按钮组
 *           左部按钮,右部按钮,无阴影,默认扩散,蓝色背景(按钮,内部字体22)
 *param[in]  parent 父控件
 *param[out] btn_l 左下部按钮实例
 *param[out] lbl_l 左下部文本实例
 *param[out] btn_r 右下部按钮实例
 *param[out] lbl_r 右下部文本实例
 */
lv_obj_t * app_lv_ui_style_two_btns(lv_obj_t *parent, lv_obj_t **btn_l, lv_obj_t **lbl_l, lv_obj_t **btn_r, lv_obj_t **lbl_r);

/*@brief     特效加载圆环
 *           速度300ms,加载条覆盖1/4,匀速,底色绿色,加载色蓝色
 *param[in]  parent  父控件
 *param[in]  size    圆环直径
 *param[in]  side    圆环宽度
 */
lv_obj_t * app_lv_ui_style_loading_spinner(lv_obj_t *parent, lv_coord_t size, lv_coord_t side);

/*@brief     特效加载进度条
 *           默认加载范围0~100,边界色蓝色,指针渐变(蓝色,绿色),内部间隙4,边界间隙1,弧度45
 *param[in]  parent  父控件
 *param[in]  width   进度条宽度
 *param[in]  height  进度条高度
 *param[in]  grad    加载方向
 */
lv_obj_t * app_lv_ui_style_loading_bar(lv_obj_t *parent, lv_coord_t width, lv_coord_t height, lv_grad_dir_t grad);

/*@brief     渐变折线图表
 *           无点折线图,水平线5等分,0~100百分比数据点,无刻度标签,固定宽高,24条数据
 *param[in]  parent  父控件
 */
lv_obj_t * app_lv_ui_style_fade_chart(lv_obj_t *parent);

#endif
