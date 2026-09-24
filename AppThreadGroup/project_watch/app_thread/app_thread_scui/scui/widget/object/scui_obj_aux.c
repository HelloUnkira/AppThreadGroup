/*实现目标:
 *    对象控件辅助推演
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 辅助推演矩形样式参数
 *@param res    样式资源(base/edge/box/sha)
 *@param size   最大宽高
 *@param stroke 各层描边(base/edge/box/sha; 0=跳过(alpha=trans))
 *@param radius 最大圆角
 */
void scui_obj_aux_res_rect(scui_obj_btn_res_t res[4], scui_point_t size,
    scui_coord_t stroke[4], scui_point_t radius)
{
    /* 层级表: base[0]/edge[1]/box[2]/sha[3] */
    static const scui_object_type_t form_table[] = {
        scui_object_form_rect_base,
        scui_object_form_rect_edge,
        scui_object_form_rect_box,
        scui_object_form_rect_sha,
    };
    
    scui_coord_t outer = 0;   /* 外圈描边累计(sha->base) */
    scui_coord_t size_min = scui_min(size.x, size.y);
    
    /* 包围由外向内: sha最大, 可绘制则向内推, 中间缺层跳过 */
    for (scui_coord_t idx = 3; idx >= 0; idx--) {
        scui_obj_btn_res_t *res_i = &res[idx];
        
        res_i->part  = scui_object_part_rect_bg;
        res_i->form  = form_table[idx];
        res_i->align = scui_opt_pos_c;
        res_i->alpha = scui_alpha_cover;
        
        /* 该层区域: 最大尺寸 - 外圈厚度(两侧) */
        scui_coord_t area_w = size.x - outer * 2;
        scui_coord_t area_h = size.y - outer * 2;
        if (area_w < 0) area_w = 0;
        if (area_h < 0) area_h = 0;
        res_i->area.w = area_w;
        res_i->area.h = area_h;
        res_i->radius = radius.x - outer;
        
        scui_coord_t stroke_i = stroke[idx];
        if (stroke_i == 0) {
            /* 不需要绘制: 跳过(alpha=trans) */
            res_i->alpha = scui_alpha_trans;
            res_i->width = 0;
        } else if (stroke_i < 0 || stroke_i >= size_min / 2) {
            /* 完全填充(实心) */
            res_i->width = 0;
        } else {
            /* 空心描边: 向内推进该层厚度 */
            res_i->width = stroke_i;
            outer += stroke_i;
        }
    }
}
