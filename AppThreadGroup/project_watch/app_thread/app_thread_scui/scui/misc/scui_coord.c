/*实现目标:
 *    坐标点运算
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 线段相交判断
 *@param line1 线段(s)
 *@param line2 线段(s)
 *@retval 是否相交
 */
bool scui_line_cross(scui_area_t *line1, scui_area_t *line2)
{
    /* 排斥检查 */
    
    if (scui_min(line1->x1, line1->x2) > scui_max(line2->x1, line2->x2) ||
        scui_min(line2->x1, line2->x2) > scui_max(line1->x1, line1->x2) ||
        scui_min(line1->y1, line1->y2) > scui_max(line2->y1, line2->y2) ||
        scui_min(line2->y1, line2->y2) > scui_max(line1->y1, line1->y2))
        return false;
    
    /* 跨立检查 */
    
    scui_multi_t dis_11_21_x = line1->x1 - line2->x1;
    scui_multi_t dis_12_21_x = line1->x2 - line2->x1;
    scui_multi_t dis_22_21_y = line2->y2 - line2->y1;
    
    scui_multi_t dis_11_21_y = line1->y1 - line2->y1;
    scui_multi_t dis_12_21_y = line1->y2 - line2->y1;
    scui_multi_t dis_22_21_x = line2->x2 - line2->x1;
    
    if ((dis_11_21_x * dis_22_21_y - dis_11_21_y * dis_22_21_x) *
        (dis_12_21_x * dis_22_21_y - dis_12_21_y * dis_22_21_x) > 0)
        return false;
    
    scui_multi_t dis_21_11_x = -dis_11_21_x;
    scui_multi_t dis_22_11_x = line2->x2 - line1->x1;
    scui_multi_t dis_12_11_y = line1->y2 - line1->y1;
    
    scui_multi_t dis_21_11_y = -dis_11_21_y;
    scui_multi_t dis_22_11_y = line2->y2 - line1->y1;
    scui_multi_t dis_12_11_x = line1->x2 - line1->x1;
    
    if ((dis_21_11_x * dis_12_11_y - dis_21_11_y * dis_12_11_x) *
        (dis_22_11_x * dis_12_11_y - dis_22_11_y * dis_12_11_x) > 0)
        return false;
    
    return true;
}

/*@breif 转化区域描述方式(主->从)
 *@param area_m 区域(主)
 *@param area_s 区域(从)
 */
void scui_area_m_to_s(scui_area_t *area_m, scui_area_t *area_s)
{
    scui_coord_t x1 = area_m->x, x2 = area_m->x + area_m->w - 1;
    scui_coord_t y1 = area_m->y, y2 = area_m->y + area_m->h - 1;
    
    area_s->x1 = x1;
    area_s->x2 = x2;
    area_s->y1 = y1;
    area_s->y2 = y2;
}

/*@breif 转化区域描述方式(从->主)
 *@param area_m 区域(主)
 *@param area_s 区域(从)
 */
void scui_area_m_by_s(scui_area_t *area_m, scui_area_t *area_s)
{
    scui_coord_t x = area_s->x1, w = area_s->x2 - area_s->x1 + 1;
    scui_coord_t y = area_s->y1, h = area_s->y2 - area_s->y1 + 1;
    
    area_m->x = x;
    area_m->y = y;
    area_m->w = w;
    area_m->h = h;
}

/*@brief 区域为空检查
 *@param area 区域
 *@retval 为空true,不为空false
 */
bool scui_area_empty(scui_area_t *area)
{
    return (area->w <= 0 || area->h <= 0);
}

/*@brief 区域相等检查(area1 == area2)
 *@param area1 区域
 *@param area2 区域
 *@retval 是否相等
 */
bool scui_area_equal(scui_area_t *area1, scui_area_t *area2)
{
    if (area1->x == area2->x &&
        area1->y == area2->y &&
        area1->w == area2->w &&
        area1->h == area2->h)
        return true;
    
    return false;
}

/*@brief 求区域交集(area = area1 & area2)
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 *@retval 为空false,不为空true
 */
bool scui_area_inter(scui_area_t *area, scui_area_t *area1, scui_area_t *area2)
{
    if (area1 == area2) {
        *area = *area1;
    } else {
        scui_area_t area1_s = {0};
        scui_area_t area2_s = {0};
        scui_area_m_to_s(area1, &area1_s);
        scui_area_m_to_s(area2, &area2_s);
        area->x1 = scui_max(area1_s.x1, area2_s.x1);
        area->y1 = scui_max(area1_s.y1, area2_s.y1);
        area->x2 = scui_min(area1_s.x2, area2_s.x2);
        area->y2 = scui_min(area1_s.y2, area2_s.y2);
        scui_area_m_by_s(area, area);
    }
    
    return !scui_area_empty(area);
}

/*@brief 求区域交集(area = area & area1)
 *@param area  区域
 *@param area1 区域
 *@retval 为空false,不为空true
 */
bool scui_area_inter2(scui_area_t *area, scui_area_t *area1)
{
    scui_area_t area_r = {0};
    if (scui_area_inter(&area_r, area, area1)) {
       *area = area_r;
        return true;
    }
    return false;
}

/*@brief 求区域并集(area = area1 | area2)
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 */
void scui_area_merge(scui_area_t *area, scui_area_t *area1, scui_area_t *area2)
{
    if (area1 == area2) {
        *area = *area1;
    } else {
        scui_area_t area1_s = {0};
        scui_area_t area2_s = {0};
        scui_area_m_to_s(area1, &area1_s);
        scui_area_m_to_s(area2, &area2_s);
        area->x1 = scui_min(area1_s.x1, area2_s.x1);
        area->y1 = scui_min(area1_s.y1, area2_s.y1);
        area->x2 = scui_max(area1_s.x2, area2_s.x2);
        area->y2 = scui_max(area1_s.y2, area2_s.y2);
        scui_area_m_by_s(area, area);
    }
}

/*@brief 求区域并集(area = area | area1)
 *@param area  区域
 *@param area1 区域
 */
void scui_area_merge2(scui_area_t *area, scui_area_t *area1)
{
    scui_area_t area_r = {0};
    scui_area_merge(&area_r, area, area1);
    *area = area_r;
}

/*@brief 求区域联合(area = area1 U area2)
 *       联合要求:区域互相平行或垂直
 *@param area  区域
 *@param area1 区域
 *@param area2 区域
 *@retval 失败或者非相交
 */
bool scui_area_union(scui_area_t *area, scui_area_t *area1, scui_area_t *area2)
{
    bool result = false;
    
    scui_area_t area1_s = {0};
    scui_area_t area2_s = {0};
    scui_area_m_to_s(area1, &area1_s);
    scui_area_m_to_s(area2, &area2_s);
    /* 区域求和要求:区域互相平行或垂直 */
    
    /* 区域垂直: */
    if (area1_s.x1 == area2_s.x1 && area1_s.x2 == area2_s.x2) {
        /*  */
        if (!(area1_s.y1 > area2_s.y2 || area1_s.y2 < area2_s.y1)) {
            /*  */
            area->x1 = (area1_s.x1, area2_s.x1);
            area->x2 = (area1_s.x2, area2_s.x2);
            area->y1 = scui_min(area1_s.y1, area2_s.y1);
            area->y2 = scui_max(area1_s.y2, area2_s.y2);
            result = true;
        }
    }
    
    /* 区域平行: */
    if (area1_s.y1 == area2_s.y1 && area1_s.y2 == area2_s.y2) {
        /*  */
        if (!(area1_s.x1 > area2_s.x2 || area1_s.x2 < area2_s.x1)) {
            /*  */
            area->y1 = (area1_s.y1, area2_s.y1);
            area->y2 = (area1_s.y2, area2_s.y2);
            area->x1 = scui_min(area1_s.x1, area2_s.x1);
            area->x2 = scui_max(area1_s.x2, area2_s.x2);
            result = true;
        }
    }
    
    scui_area_m_by_s(area, area);
    return result;
}


/*@brief 剪切域偏移调整
 *       先调整剪切域偏移
 *       后与原剪切域交集运算
 *@param clip   剪切域
 *@param offset 偏移量
 *@retval 为空false,不为空true
 */
bool scui_area_limit_offset(scui_area_t *clip, scui_point_t *offset)
{
    if (offset->x == 0 && offset->y == 0)
        return true;
    
    scui_area_t clip_offset = *clip;
    clip_offset.x += offset->x;
    clip_offset.y += offset->y;
    
    scui_area_t clip_inter = {0};
    if (scui_area_inter(&clip_inter, clip, &clip_offset)) {
       *clip = clip_inter;
        return true;
    }
    return false;
}

/*@brief 求区域求差(area = area1 - area2)
 *       求差要求:必须存在交集,area2是area1的子集
 *@param area  区域
 *@param num   区域数量
 *@param area1 区域
 *@param area2 区域
 *@retval 不为空
 */
bool scui_area_differ(scui_area_t area[4], scui_coord_t *num, scui_area_t *area1, scui_area_t *area2)
{
    *num = 0;
    scui_area_t area1_s = {0};
    scui_area_t area2_s = {0};
    scui_area_m_to_s(area1, &area1_s);
    scui_area_m_to_s(area2, &area2_s);
    
    /* 裁剪多余上部 */
    if (area1_s.y1 < area2_s.y1) {
        area[*num].x1 = area1_s.x1;
        area[*num].x2 = area1_s.x2;
        area[*num].y1 = area1_s.y1;
        area[*num].y2 = area2_s.y1 - 1;
        (*num)++;
    }
    /* 裁剪多余下部 */
    if (area1_s.y2 > area2_s.y2) {
        area[*num].x1 = area1_s.x1;
        area[*num].x2 = area1_s.x2;
        area[*num].y1 = area2_s.y2 + 1;
        area[*num].y2 = area1_s.y2;
        (*num)++;
    }
    /* 裁剪多余左部 */
    if (area1_s.x1 < area2_s.x1) {
        area[*num].y1 = area2_s.y1;
        area[*num].y2 = area2_s.y2;
        area[*num].x1 = area1_s.x1;
        area[*num].x2 = area2_s.x1 - 1;
        (*num)++;
    }
    /* 裁剪主域多余右部 */
    if (area1_s.x2 > area2_s.x2) {
        area[*num].y1 = area2_s.y1;
        area[*num].y2 = area2_s.y2;
        area[*num].x1 = area2_s.x2 + 1;
        area[*num].x2 = area1_s.x2;
        (*num)++;
    }
    
    for (scui_coord_t idx = 0; idx < *num; idx++)
        scui_area_m_by_s(&area[idx], &area[idx]);
    
    return *num != 0;
}

/*@brief 检查区域包含区域(area1包含area2)
 *@param area1 区域
 *@param area2 区域
 *@retval 包含true,不包含false
 */
bool scui_area_inside(scui_area_t *area1, scui_area_t *area2)
{
    if (area1 == area2)
        return true;
    if (area1->x <= area2->x && area1->x + area1->w >= area2->x + area2->w &&
        area1->y <= area2->y && area1->y + area1->h >= area2->y + area2->h)
        return true;
    
    return false;
}

/*@brief 检查区域包含坐标(area包含point)
 *@param area  区域
 *@param point 坐标
 *@retval 包含true,不包含false
 */
bool scui_area_point(scui_area_t *area, scui_point_t *point)
{
    if (point->x >= area->x && point->x <= area->x + area->w - 1 &&
        point->y >= area->y && point->y <= area->y + area->h - 1)
        return true;
    
    return false;
}

/*@brief 检查区域包含线(area包含line)
 *@param area 区域
 *@param line 线段
 *@retval 包含true,不包含false
 */
bool scui_area_line(scui_area_t *area, scui_area_t *line)
{
    /* 1.端点是否在域内 */
    if (scui_area_point(area, &line->p1))
        return true;
    if (scui_area_point(area, &line->p2))
        return true;
    /* 2.线段与区域俩条对角线是否相交 */
    scui_area_t line1 = {
        .x1 = area->x1, .y1 = area->y1,
        .x2 = area->x2, .y2 = area->y2,
    };
    scui_area_t line2 = {
        .x1 = area->x1, .y1 = area->y2,
        .x2 = area->x2, .y2 = area->y1,
    };
    
    if (scui_line_cross(line, &line1))
        return true;
    if (scui_line_cross(line, &line2))
        return true;
    
    return false;
}
