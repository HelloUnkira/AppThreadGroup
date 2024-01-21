/*实现目标:
 *    显示设备事件
 */

#define SCUI_LOG_LOCAL_STATUS        1
#define SCUI_LOG_LOCAL_LEVEL         0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_disp_t scui_disp = {0};

/*@brief 设置显示器水平距离
 *@param hor_res 水平距离
 */
void scui_disp_set_hor_res(scui_coord_t hor_res)
{
    scui_disp.hor_res = hor_res;
}

/*@brief 设置显示器垂直距离
 *@param ver_res 垂直距离
 */
void scui_disp_set_ver_res(scui_coord_t ver_res)
{
    scui_disp.ver_res = ver_res;
}

/*@brief 获取显示器水平距离
 *@retval 水平距离
 */
scui_coord_t scui_disp_get_hor_res(void)
{
    return scui_disp.hor_res;
}

/*@brief 获取显示器垂直距离
 *@retval 垂直距离
 */
scui_coord_t scui_disp_get_ver_res(void)
{
    return scui_disp.ver_res;
}

/*@brief 显示器初始化
 */
void scui_disp_ready(void)
{
}
