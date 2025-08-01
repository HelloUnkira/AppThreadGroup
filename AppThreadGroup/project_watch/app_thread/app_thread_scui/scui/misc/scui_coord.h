#ifndef SCUI_COORD_H
#define SCUI_COORD_H

/*@brief 坐标点,范围:[-32768, +32767]
 *       通常来说不需要支持更大范围的坐标点
 */
typedef int16_t scui_coord_t;
#define scui_coord_max  (0x7fff)
#define scui_coord_min  (0x8000)

/*@brief 坐标点运算量
 *       坐标点运算可能产生溢出
 */
typedef int32_t scui_multi_t;
#define scui_multi_max  (0x7fffffff)
#define scui_multi_min  (0x80000000)

/*@brief 坐标
 */
typedef union {
    struct {
        scui_coord_t x;
        scui_coord_t y;
    };
    struct {
        scui_coord_t w;
        scui_coord_t h;
    };
    scui_coord_t meta[2];
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
    struct {
        scui_point_t p1;
        scui_point_t p2;
    };
    struct {
        scui_point_t pos;
        scui_point_t size;
    };
    scui_coord_t meta[4];
} scui_area_t;

/*@brief 线段相交判断
 *@param line1 线段
 *@param line2 线段
 *@retval 是否相交
 */
bool scui_line_cross(scui_area_t *line1, scui_area_t *line2);

/*@breif 转化区域描述方式(主->从)
 *@param area_m 区域(主)
 *@param area_s 区域(从)
 */
void scui_area_m_to_s(scui_area_t *area_m, scui_area_t *area_s);

/*@breif 转化区域描述方式(从->主)
 *@param area_m 区域(主)
 *@param area_s 区域(从)
 */
void scui_area_m_by_s(scui_area_t *area_m, scui_area_t *area_s);

/*@brief 区域为空检查
 *@param area 区域
 *@retval 为空true,不为空false
 */
bool scui_area_empty(scui_area_t *area);

/*@brief 区域相等检查(area1 == area2)
 *@param area1 区域
 *@param area2 区域
 *@retval 是否相等
 */
bool scui_area_equal(scui_area_t *area1, scui_area_t *area2);

/*@brief 求区域交集(area = area1 & area2)
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 *@retval 为空false,不为空true
 */
bool scui_area_inter(scui_area_t *area, scui_area_t *area1, scui_area_t *area2);

/*@brief 求区域交集(area = area & area1)
 *@param area  区域
 *@param area1 区域
 *@retval 为空false,不为空true
 */
bool scui_area_inter2(scui_area_t *area, scui_area_t *area1);

/*@brief 求区域并集(area = area1 | area2)
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 */
void scui_area_merge(scui_area_t *area, scui_area_t *area1, scui_area_t *area2);

/*@brief 求区域并集(area = area | area1)
 *@param area  区域
 *@param area1 区域
 */
void scui_area_merge2(scui_area_t *area, scui_area_t *area1);

/*@brief 求区域联合(area = area1 U area2)
 *       联合要求:区域互相平行或垂直
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 *@retval 失败或者非相交
 */
bool scui_area_union(scui_area_t *area, scui_area_t *area1, scui_area_t *area2);

/*@brief 剪切域偏移调整
 *       先调整剪切域偏移
 *       后与原剪切域交集运算
 *@param clip   剪切域
 *@param offset 偏移量
 *@retval 为空false,不为空true
 */
bool scui_area_limit_offset(scui_area_t *clip, scui_point_t *offset);

/*@brief 求区域求差(area = area1 - area2)
 *       求差要求:必须存在交集,area2是area1的子集
 *@param area  区域
 *@param num   区域数量
 *@param area1 区域
 *@param area2 区域
 *@retval 不为空
 */
bool scui_area_differ(scui_area_t area[4], scui_coord_t *num, scui_area_t *area1, scui_area_t *area2);

/*@brief 检查区域包含区域(area1包含area2)
 *@param area1 区域
 *@param area2 区域
 *@retval 包含true,不包含false
 */
bool scui_area_inside(scui_area_t *area1, scui_area_t *area2);

/*@brief 检查区域包含区域(area1包含area2, 限制圆角)
 *@param area1  区域
 *@param area2  区域
 *@param radius 半径
 *@retval 包含true,不包含false
 */
bool scui_area_inside_cric(scui_area_t *area1, scui_area_t *area2, scui_coord_t radius);

/*@brief 检查区域包含区域(area1不包含area2, 限制圆角)
 *@param area1  区域
 *@param area2  区域
 *@param radius 半径
 *@retval 包含true,不包含false
 */
bool scui_area_outside_cric(scui_area_t *area1, scui_area_t *area2, scui_coord_t radius);

/*@brief 检查区域包含坐标(area包含point)
 *@param area  区域
 *@param point 坐标
 *@retval 包含true,不包含false
 */
bool scui_area_point(scui_area_t *area, scui_point_t *point);

/*@brief 检查区域包含坐标(水平圆)
 *@param area  区域
 *@param point 坐标
 *@retval 包含true,不包含false
 */
bool scui_area_point_circ_in(scui_area_t *area, scui_point_t *point);

/*@brief 检查区域包含坐标(圆角区域)
 *@param area   区域
 *@param point  坐标
 *@param radius 半径
 *@retval 包含true,不包含false
 */
bool scui_area_point_circ(scui_area_t *area, scui_point_t *point, scui_coord_t radius);

/*@brief 检查区域包含线(area包含line)
 *@param area   区域
 *@param p1 坐标端点
 *@param p2 坐标端点
 *@retval 包含true,不包含false
 */
bool scui_area_line(scui_area_t *area, scui_area_t *line);

/*@brief 区域中心坐标
 *@param area 区域
 *@retval 中心坐标
 */
scui_point_t scui_area_center(scui_area_t *area);

#endif
