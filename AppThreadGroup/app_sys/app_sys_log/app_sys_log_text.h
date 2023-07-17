#ifndef APP_SYS_LOG_TEXT_H
#define APP_SYS_LOG_TEXT_H

/* 单次存储日志文本最大长度(包括字符串结尾'\0') */
#define APP_SYS_LOG_TEXT_MAX  128

typedef union {
    uint8_t buffer[0];
    struct {
        struct {
            uintptr_t head;
            uintptr_t tail;
            uintptr_t peek;
            uintptr_t zone;
        } info;
        uint32_t crc32;
    };
} app_sys_log_text_t;

typedef union {
    uint8_t buffer[0];
    struct {
        uintptr_t length;
        uint8_t   text[APP_SYS_LOG_TEXT_MAX + 1];
    };
} app_sys_log_item_t;

/*@brief 日志追踪队列重置
 */
void app_sys_log_text_reset(void);

/*@brief 日志追踪队列初始化
 */
void app_sys_log_text_ready(void);

/*@brief      日志追踪队列尾转入一段日志
 *@param[in]  text       日志文本
 *@param[in]  need_cover 覆盖式加入日志
 *                       如果空间不足则会不断丢弃最旧的数据
 *                       直到空间完全释放也不能加入
 *                       或者可以加入新条目为止
 *@retval     成功或者失败
 */
bool app_sys_log_text_dump(char text[APP_SYS_LOG_TEXT_MAX], bool need_cover);

/*@brief      日志追踪队列头转出一段日志
 *@param[out] text 日志文本
 *@retval     成功或者失败
 */
bool app_sys_log_text_load(char text[APP_SYS_LOG_TEXT_MAX]);

/*@brief      日志追踪队列窥探一段日志(自动迭代到下一段日志)
 *@param[out] text 日志文本
 *@retval     成功或者失败
 */
bool app_sys_log_text_peek(char text[APP_SYS_LOG_TEXT_MAX]);

/*@brief 日志追踪队列窥探指针重置
 */
void app_sys_log_text_peek_reset(void);

/*@brief 日志追踪队列转储和加载测试
 *       此测试模式只能单独使用
 */
void app_sys_log_text_test(void);

#endif
