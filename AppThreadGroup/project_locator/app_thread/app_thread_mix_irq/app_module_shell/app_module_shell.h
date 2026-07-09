#ifndef APP_MODULE_SHELL_H
#define APP_MODULE_SHELL_H

/*@brief shell 模组就绪
 *       初始化设备,配置轮询定时器
 */
void app_module_shell_ready(void);

/*@brief shell 模组轮询
 *       由 mix_irq 心跳驱动,读取设备输入后拆字段派发
 */
void app_module_shell_poll(void);

#endif
