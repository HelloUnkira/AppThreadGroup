/*实现目标:
 *    帧缓冲管理
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

static scui_surface_t scui_surface_draw = {0};

/*@brief 画布为空检查
 *@param surface 画布实例
 *@retval 是否为空
 */
bool scui_surface_empty(scui_surface_t *surface)
{
    if (surface->pixel == NULL)
        return true;
    return false;
}

/*@brief draw画布设置
 *@param surface 画布实例
 */
void scui_surface_draw_set(scui_surface_t *surface)
{
    scui_surface_draw = *surface;
}

/*@brief draw画布获取
 *@param surface 画布实例
 */
void scui_surface_draw_get(scui_surface_t *surface)
{
    *surface = scui_surface_draw;
}
