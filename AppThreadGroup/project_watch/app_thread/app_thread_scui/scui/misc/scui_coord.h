#ifndef SCUI_COORD_H
#define SCUI_COORD_H

/*@brief 坐标点,范围:[-32768, +32767]
 *       通常来说不需要支持更大范围的坐标点
 */
typedef int16_t scui_coord_t;

/*@brief 坐标点运算量
 *       坐标点运算可能产生溢出
 */
typedef int32_t scui_multi_t;

/*@brief 坐标
 */
typedef struct {
    scui_coord_t x;
    scui_coord_t y;
} scui_point_t;

/*@brief 坐标区域
 *       总计使用俩种表达方式
 *       主表达为:<x,y,w,h>
 *       副表达为:<x1,y1,x2,y2>
 *       主要考虑不同使用场景下的定制化逻辑
 */
typedef union {
    struct {    /* 主描述 */
        scui_coord_t x;
        scui_coord_t y;
        scui_coord_t w;
        scui_coord_t h;
    };
    struct {    /* 从描述 */
        scui_coord_t x1;
        scui_coord_t y1;
        scui_coord_t x2;
        scui_coord_t y2;
    };
} scui_area_t;

/*@breif 转化区域描述方式(主->从)
 */
inline void scui_area_m_to_s(scui_area_t *area);

/*@breif 转化区域描述方式(从->主)
 */
inline void scui_area_s_to_m(scui_area_t *area);

/*@brief 区域为空检查
 *@param area 区域
 *@retval 为空true,不为空false
 */
bool scui_area_empty(scui_area_t *area);

/*@brief 求区域交集(area = area1 & area2)
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 *@retval 为空false,不为空true
 */
bool scui_area_inter(scui_area_t *area, scui_area_t *area1, scui_area_t *area2);

/*@brief 求区域并集(area = area1 | area2)
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 */
void scui_area_merge(scui_area_t *area, scui_area_t *area1, scui_area_t *area2);

/*@brief 检查区域包含区域(area2包含area1)
 *@param area1 区域
 *@param area2 区域
 *@retval 包含true,不包含false
 */
bool scui_area_inside(scui_area_t *area1, scui_area_t *area2);

/*@brief 检查区域包含坐标(area包含point)
 *@param area  区域
 *@param point 坐标
 *@retval 包含true,不包含false
 */
bool scui_area_inside_point(scui_area_t *area, scui_point_t *point);

#endif
