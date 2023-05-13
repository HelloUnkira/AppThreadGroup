/* 实现目标:
 *     APP需要对系统进行的封装体
 *     APP会在一个专门的地点和时刻准备这些资源
 *     对所有平台做一个全通配的最低配置
 */

#include "app_ext_lib.h"

#if APP_ARCH_IS_PC

/*@brief     变参函数式LOG输出接口
 *@param[in] format 格式化字符串
 */
void app_ext_arch_log_msg1(const char *format, ...)
{
    va_list  list;
    va_start(list, format);
    vprintf(format, list);
    va_end(list);
}

/*@brief     变参列表式LOG输出接口
 *@param[in] format 格式化字符串
 *@param[in] list   变参列表
 */
void app_ext_arch_log_msg2(const char *format, va_list list)
{
    vprintf(format, list);
}

#endif
