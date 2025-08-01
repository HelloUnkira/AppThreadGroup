/*实现目标:
 *    复合剪切域运算
 *    单个剪切域及其运算可以直接用区域及其运算描述
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 剪切域为空
 *@param clip_set 剪切域集合
 *@retval 是否为空
 */
bool scui_clip_empty(scui_clip_set_t *clip_set)
{
    if (scui_list_dll_head(&clip_set->dl_list) == NULL &&
        scui_list_dll_tail(&clip_set->dl_list) == NULL)
        return true;
    
    SCUI_ASSERT(scui_list_dll_head(&clip_set->dl_list) != NULL);
    SCUI_ASSERT(scui_list_dll_tail(&clip_set->dl_list) != NULL);
    
    return false;
}

/*@brief 就绪剪切域
 *@param clip_set 剪切域集合
 */
void scui_clip_ready(scui_clip_set_t *clip_set)
{
    clip_set->clip = (scui_area_t){0};
    scui_list_dll_reset(&clip_set->dl_list);
}

/*@brief 清空剪切域
 *@param clip_set 剪切域集合
 */
void scui_clip_clear(scui_clip_set_t *clip_set)
{
    for (scui_clip_unit_t *unit = NULL; true; true) {
        scui_list_dll_btra(&clip_set->dl_list, node) {
            unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
            break;
        }
        if (unit == NULL)
            break;
        if (unit != NULL) {
            scui_list_dll_remove(&clip_set->dl_list, &unit->dl_node);
            SCUI_MEM_FREE(unit);
            unit  = NULL;
        }
    }
}

/*@brief 添加剪切域
 *@param clip_set 剪切域集合
 *@param clip     剪切域
 */
bool scui_clip_add(scui_clip_set_t *clip_set, scui_area_t *clip)
{
    scui_clip_unit_t *unit = NULL;
    
    if (scui_area_empty(&clip_set->clip))
        return false;
    if (scui_area_empty(clip))
        return false;
    
    SCUI_LOG_DEBUG("<%d, %d, %d, %d>", clip->x, clip->y, clip->w, clip->h);
    
    scui_area_t clip_valid = {0};
    if (!scui_area_inter(&clip_valid, &clip_set->clip, clip))
         return false;
    
    while (true) {
        /* 如果剪切域可以合并, 合并后的剪切域要继续检查 */
        scui_list_dll_btra(&clip_set->dl_list, node) {
            unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
            /* 剪切域是否真包含 */
            if (scui_area_inside(&clip_valid, &unit->clip))
                break;
            if (scui_area_inside(&unit->clip, &clip_valid)) {
                clip_valid = unit->clip;
                break;
            }
            /* 剪切域是否可以并列 */
            scui_area_t clip_union = {0};
            if (scui_area_union(&clip_union, &unit->clip, &clip_valid)) {
                clip_valid = clip_union;
                break;
            }
            unit = NULL;
        }
        if (unit == NULL)
            break;
        if (unit != NULL) {
            scui_list_dll_remove(&clip_set->dl_list, &unit->dl_node);
            SCUI_MEM_FREE(unit);
            unit = NULL;
        }
    }
    
    /* 将最终的合并结果加入 */
    unit = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_clip_unit_t));
    unit->clip = clip_valid;
    scui_list_dln_reset(&unit->dl_node);
    scui_list_dll_ainsert(&clip_set->dl_list, NULL, &unit->dl_node);
    return true;
}

/*@brief 移除剪切域
 *@param clip_set 剪切域集合
 *@param clip     剪切域
 */
bool scui_clip_del(scui_clip_set_t *clip_set, scui_area_t *clip)
{
    scui_area_t  clip4[4]  = {0};
    scui_coord_t clip4_num = 0;
    
    if (scui_area_empty(clip))
        return false;
    if (scui_area_empty(&clip_set->clip))
        return false;
    
    SCUI_LOG_DEBUG("<%d, %d, %d, %d>", clip->x, clip->y, clip->w, clip->h);
    
    scui_area_t clip_valid = {0};
    if (!scui_area_inter(&clip_valid, &clip_set->clip, clip))
         return false;
    
    for (scui_clip_unit_t *unit = NULL; true; true) {
        /* 如果剪切域可以合并, 合并后的剪切域要继续检查 */
        scui_list_dll_btra(&clip_set->dl_list, node) {
            unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
            /* 饶过不相交的剪切域,计算剪切域差集 */
            scui_area_t clip_inter = {0};
            if (scui_area_inter(&clip_inter, &unit->clip, &clip_valid)) {
                scui_area_differ(clip4, &clip4_num, &unit->clip, &clip_inter);
                /* 如果求不到余集,那就是全子集 */
                SCUI_ASSERT(clip4_num != 0 || (clip4_num == 0 &&
                            scui_area_equal(&unit->clip, &clip_inter)));
                break;
            }
            unit = NULL;
        }
        if (unit == NULL)
            break;
        if (unit != NULL) {
            scui_list_dll_remove(&clip_set->dl_list, &unit->dl_node);
            SCUI_MEM_FREE(unit);
            unit  = NULL;
            
            for (scui_handle_t idx = 0; idx < clip4_num; idx++) {
                scui_clip_add(clip_set, &clip4[idx]);
                clip4[idx] = (scui_area_t){0};
            }
            clip4_num = 0;
        }
    }
}
