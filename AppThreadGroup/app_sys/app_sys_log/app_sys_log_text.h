#ifndef APP_SYS_LOG_TEXT_H
#define APP_SYS_LOG_TEXT_H

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
        uint8_t   text[APP_SYS_LOG_TEXT_LIMIT + 1];
    };
} app_sys_log_item_t;

/*@brief 日志追踪队列重置
 */
void app_sys_log_text_reset(void);

/*@brief 日志追踪队列初始化
 */
void app_sys_log_text_ready(void);

/*@brief 日志追踪队列尾转入一段日志
 *@param text       日志文本
 *@param need_cover 覆盖式加入日志
 *                       如果空间不足则会不断丢弃最旧的数据
 *                       直到空间完全释放也不能加入
 *                       或者可以加入新条目为止
 *@retval 成功或者失败
 */
bool app_sys_log_text_dump(char text[APP_SYS_LOG_TEXT_LIMIT], bool need_cover);

/*@brief 日志追踪队列头转出一段日志
 *@param text 日志文本
 *@retval 成功或者失败
 */
bool app_sys_log_text_load(char text[APP_SYS_LOG_TEXT_LIMIT]);

/*@brief 日志追踪队列窥探一段日志(自动迭代到下一段日志)
 *@param text 日志文本
 *@retval 成功或者失败
 */
bool app_sys_log_text_peek(char text[APP_SYS_LOG_TEXT_LIMIT]);

/*@brief 日志追踪队列窥探指针重置
 */
void app_sys_log_text_peek_reset(void);

/*@brief 日志持久化转接口
 *           这只是一个转接口,外界不可使用
 *@param text 日志文本
 */
void app_sys_log_text_persistent(char *text);

#endif
