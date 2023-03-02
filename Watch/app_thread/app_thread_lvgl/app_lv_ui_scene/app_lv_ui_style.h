#ifndef APP_LV_UI_STYLE_H
#define APP_LV_UI_STYLE_H

/*@brief    默认控件风格
 *          黑色背景,无边框
 *param[in] obj 控件
 */
void app_lv_ui_style_object(lv_obj_t *obj);

/*@brief 默认界面风格
 *       无主题风格,全屏幕,黑色背景全覆盖,无边界,无内部间隙
 *retval 返回场景(窗口,界面)
 */
lv_obj_t * app_lv_ui_style_scene(void);

/*@brief     顶部居中标题标签
 *           黑色背景,无边框,白色滚动长文
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_ui_style_title_label(lv_obj_t *parent);

/*@brief     默认按钮
 *           蓝色背景,无边框
 *param[in]  parent 父控件
 */
lv_obj_t * app_lv_ui_style_btn(lv_obj_t *parent);

/*@brief     顶部风格
 *           左部按钮,右部时间,按钮返回上一层
 *param[in]  parent 父控件
 *param[out] btn   左上部按钮实例
 *param[out] title 左上部文本实例
 *retval     顶部风格控件
 */
lv_obj_t * app_lv_ui_style_title(lv_obj_t *parent, lv_obj_t **btn, lv_obj_t **title, lv_obj_t **time);

/*@brief     底部双按钮组
 *           左部按钮,右部按钮,无阴影,默认扩散,蓝色背景(按钮,内部字体22)
 */
lv_obj_t * app_lv_ui_style_two_btns(lv_obj_t *parent, lv_obj_t **btn_l, lv_obj_t **lbl_l, lv_obj_t **btn_r, lv_obj_t **lbl_r);

/*@brief     特效滚轮
 *           默认遮罩,被选值(白色,内部字体28),非选值(灰色,内部字体22),轮滚
 *param[in]  parent  父控件
 *param[in]  width   滚轮宽度
 *param[in]  options 滚轮显示值
 *param[in]  vis_row 滚轮默认显示个数
 */
lv_obj_t * app_lv_ui_style_roller(lv_obj_t *parent, lv_coord_t width, const char *options, uint8_t vis_row);

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

#endif
