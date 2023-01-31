#ifdef APP_MODULE_PROTOCOL_H
#ifdef APP_MODULE_PROTOCOL_CB_H

/*实现目标:
 *一个代替动态注册的,静态本地导入的协议传输回调集
 *它会被内置到protocol线程中作为一个辅助功能使用
 *回调会在到点时执行下面注册的回调,按顺序执行
 *因为这些回调都是编码时注册,本地对应回调的实际空间占用
 *最终编译器都会将其转化为函数代码段函数表,不占用内存
 *注意:这个文件不可以以任何方式被其他地方引用
 *     它的唯一使用者为app_module_protocol.c
 */

/* 函数声明及其引用(注意:不要导入头文件,直接提供函数名即可): */
extern bool app_module_protocol_rx_empty(app_module_protocol_pkg_t *ptl_pkg, uint8_t *ptl_dat);
extern bool app_module_protocol_tx_empty(app_module_protocol_pkg_t *ptl_pkg, uint8_t **ptl_dat);

/* 协议传输回调 */
static const app_module_protocol_callback app_module_protocol_cb[] = {
    {app_module_protocol_rx_empty,
     app_module_protocol_tx_empty,},
};

#endif
#endif
