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
    scui_coord_t meta[2];
} scui_point_t;

/*@brief 坐标区域
 *       总计使用俩种表达方式
 *       主表达为:<x,y,w,h>
 *       副表达为:<x1,y1,x2,y2>
 *       主要考虑不同使用窗口下的定制化逻辑
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

/*@brief 点/区域对齐
 */
typedef enum {
    /* 0x yy xx */
    scui_align_mask_ixl = 0x0001,
    scui_align_mask_ixm = 0x0002,
    scui_align_mask_ixr = 0x0004,
    scui_align_mask_oxl = 0x0010,
    scui_align_mask_oxr = 0x0020,
    
    scui_align_mask_iyt = 0x0100,
    scui_align_mask_iym = 0x0200,
    scui_align_mask_iyb = 0x0400,
    scui_align_mask_oyt = 0x1000,
    scui_align_mask_oyb = 0x2000,
    
    scui_align_itl = 0x0101,    /* in top left */
    scui_align_itm = 0x0102,    /* in top middle */
    scui_align_itr = 0x0104,    /* in top right */
    scui_align_ibl = 0x0401,    /* in bottom left */
    scui_align_ibm = 0x0402,    /* in bottom middle */
    scui_align_ibr = 0x0404,    /* in bottom right */
    scui_align_ilm = 0x0201,    /* in left middle */
    scui_align_irm = 0x0204,    /* in right middle */
    scui_align_icc = 0x0202,    /* in center center */
    
    scui_align_otl = 0x1001,    /* out top left */
    scui_align_otm = 0x1002,    /* out top middle */
    scui_align_otr = 0x1004,    /* out top right */
    scui_align_obl = 0x2001,    /* out bottom left */
    scui_align_obm = 0x2002,    /* out bottom middle */
    scui_align_obr = 0x2004,    /* out bottom right */
    
    scui_align_olt = 0x0110,    /* out left top */
    scui_align_olm = 0x0210,    /* out left middle */
    scui_align_olb = 0x0410,    /* out left bottom */
    scui_align_ort = 0x0120,    /* out right top */
    scui_align_orm = 0x0220,    /* out right middle */
    scui_align_orb = 0x0420,    /* out right bottom */
    
} scui_align_t;

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

/*@brief 求区域求差(area = area1 - area2)
 *       求差要求:必须存在交集,area2是area1的子集
 *@param area  区域
 *@param num   区域数量
 *@param area1 区域
 *@param area2 区域
 *@retval 不为空
 */
bool scui_area_differ(scui_area_t area[4], scui_coord_t *num, scui_area_t *area1, scui_area_t *area2);

/*@brief 求区域交差(area = area2 - (area1 & area2))
 *       求差要求:必须存在交集,一个或俩个相邻顶点
 *@param area  区域
 *@param num   区域数量
 *@param area1 区域
 *@param area2 区域
 *@retval 失败或者非相交
 */
bool scui_area_differ2(scui_area_t area[2], scui_coord_t *num, scui_area_t *area1, scui_area_t *area2);

/*@brief 剪切域偏移调整
 *       先调整剪切域偏移
 *       后与原剪切域交集运算
 *@param clip   剪切域
 *@param offset 偏移量
 *@retval 为空false,不为空true
 */
bool scui_area_limit_offset(scui_area_t *clip, scui_point_t *offset);

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

/*@brief 区域大小
 *@param area 区域
 *@retval 区域大小
 */
scui_multi_t scui_area_size(scui_area_t *area);

/*@brief 区域相对区域对齐偏移(area_t参考area对齐)
 *@param area   区域
 *@param area_t 区域
 *@param align  对齐方向
 *@retval 对齐偏移值
 */
scui_point_t scui_area_align(scui_area_t *area, scui_area_t *area_t, scui_align_t align);

#endif
