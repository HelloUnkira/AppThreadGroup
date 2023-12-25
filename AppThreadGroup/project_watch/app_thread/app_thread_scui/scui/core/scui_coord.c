/*实现目标:
 *    坐标点运算
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@breif 转化区域描述方式(主->从)
 */
inline void scui_area_m_to_s(scui_area_t *area)
{
    scui_coord_t x1 = area->x, x2 = area->x + area->w - 1;
    scui_coord_t y1 = area->y, y2 = area->y + area->h - 1;
    
    area->x1 = x1;
    area->x2 = x2;
    area->y1 = y1;
    area->y2 = y2;
}

/*@breif 转化区域描述方式(从->主)
 */
inline void scui_area_s_to_m(scui_area_t *area)
{
    scui_coord_t x = area->x1, w = area->x2 - area->x1 + 1;
    scui_coord_t y = area->y1, h = area->y2 - area->y1 + 1;
    
    area->x = x;
    area->y = y;
    area->w = w;
    area->h = h;
}

/*@brief 区域为空检查
 *@param area 区域
 *@retval 为空true,不为空false
 */
inline bool scui_area_empty(scui_area_t *area)
{
    return (area->w <= 0 || area->h <= 0);
}

/*@brief 求区域交集(area = area1 & area2)
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 *@retval 为空false,不为空true
 */
inline bool scui_area_inter(scui_area_t *area, scui_area_t *area1, scui_area_t *area2)
{
    scui_area_m_to_s(area1);
    scui_area_m_to_s(area2);
    area->x1 = app_sys_max(area1->x1, area2->x1);
    area->y1 = app_sys_max(area1->y1, area2->y1);
    area->x2 = app_sys_min(area1->x2, area2->x2);
    area->y2 = app_sys_min(area1->y2, area2->y2);
    scui_area_s_to_m(area2);
    scui_area_s_to_m(area1);
    scui_area_s_to_m(area);
    
    return !scui_area_empty(area);
}

/*@brief 求区域并集(area = area1 | area2)
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 */
inline void scui_area_merge(scui_area_t *area, scui_area_t *area1, scui_area_t *area2)
{
    scui_area_m_to_s(area1);
    scui_area_m_to_s(area2);
    area->x1 = app_sys_min(area1->x1, area2->x1);
    area->y1 = app_sys_min(area1->y1, area2->y1);
    area->x2 = app_sys_max(area1->x2, area2->x2);
    area->y2 = app_sys_max(area1->y2, area2->y2);
    scui_area_s_to_m(area2);
    scui_area_s_to_m(area1);
    scui_area_s_to_m(area);
}

/*@brief 检查区域包含区域(area2包含area1)
 *@param area1 区域
 *@param area2 区域
 *@retval 包含true,不包含false
 */
inline bool scui_area_inside(scui_area_t *area1, scui_area_t *area2)
{
    if (area1->x < area2->x || area1->x + area1->w > area2->x + area2->w ||
        area1->y < area2->y || area1->y + area1->h > area2->y + area2->h)
        return false;
    return true;
}

/*@brief 检查区域包含坐标(area包含point)
 *@param area  区域
 *@param point 坐标
 *@retval 包含true,不包含false
 */
inline bool scui_area_inside_point(scui_area_t *area, scui_point_t *point)
{
    if (point->x >= area->x && point->x <= area->x + area->w &&
        point->y >= area->y && point->y <= area->y + area->h)
        return true;
    return false;
}
