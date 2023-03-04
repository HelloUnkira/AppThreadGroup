#ifndef APP_LV_UI_DEVELOPER_MODE_H
#define APP_LV_UI_DEVELOPER_MODE_H

/* 测试时使用 */
/* 绕过开关机动画部分,禁用超时机制 */
#define APP_LV_UI_DEVELOPER_MODE    0

/*@brief 开发者模式
 *       在该函数内部添加自定义开发界面
 *       绕过一部分调度机制更方便UI的开发
 */
#if APP_LV_UI_DEVELOPER_MODE
void app_lv_ui_developer_mode(void);
#endif

#endif
