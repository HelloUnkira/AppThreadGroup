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
    scui_list_dll_reset(&clip_set->dl_list);
    clip_set->clip = (scui_area_t){0};
    clip_set->num  = 0;
    clip_set->size = 0;
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
            clip_set->size -= scui_area_size(&unit->clip);
            clip_set->num--;
            SCUI_MEM_FREE(unit);
            
            unit  = NULL;
        }
    }
    
    SCUI_ASSERT(clip_set->size == 0 && clip_set->num == 0);
}

/*@brief 添加剪切域
 *       调用者:检查剪切域有效性
 *@param clip_set 剪切域集合
 *@param clip     剪切域
 */
bool scui_clip_add(scui_clip_set_t *clip_set, scui_area_t *clip)
{
    scui_area_t clip_valid = *clip;
    SCUI_ASSERT(scui_area_inter(&clip_valid, &clip_set->clip, clip));
    
    for (scui_clip_unit_t *unit = NULL; !scui_clip_empty(clip_set); true) {
        /* 如果剪切域可以合并,合并后的剪切域要继续检查 */
        scui_list_dll_btra(&clip_set->dl_list, node) {
            unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
            /* 剪切域是否真包含(保留旧有的,直接退出) */
            if (scui_area_inside(&unit->clip, &clip_valid))
                return true;
            /* 剪切域是否真包含(去除旧有的,重新检查) */
            if (scui_area_inside(&clip_valid, &unit->clip)) {
                // 将旧有的节点回收, 总和剪切域更新到clip_valid
                scui_list_dll_remove(&clip_set->dl_list, &unit->dl_node);
                clip_set->size -= scui_area_size(&unit->clip);
                clip_set->num--;
                SCUI_MEM_FREE(unit);
                break;
            }
            /* 剪切域是否并列(去除旧有的,重新检查) */
            scui_area_t clip_union = {0};
            if (scui_area_union(&clip_union, &unit->clip, &clip_valid)) {
                clip_valid = clip_union;
                // 将旧有的节点回收,总和剪切域更新到clip_valid
                scui_list_dll_remove(&clip_set->dl_list, &unit->dl_node);
                clip_set->size -= scui_area_size(&unit->clip);
                clip_set->num--;
                SCUI_MEM_FREE(unit);
                break;
            }
            /* 剪切域是否相交(去除重合的,重走流程) */
            scui_area_t  clip2[3] = {0};
            scui_coord_t clip2_num = 0; bool result = true;
            if (scui_area_differ2(clip2, &clip2_num, &unit->clip, &clip_valid)) {
                // 现在clip_valid被拆成更小的了(重走流程)
                for (scui_coord_t idx = 0; idx < clip2_num; idx++)
                    result |= scui_clip_add(clip_set, &clip2[idx]);
                return result;
            }
            
            unit = NULL;
        }
        
        // 循环一次未命中
        if (unit == NULL)
            break;
    }
    
    scui_clip_unit_t *unit = NULL;
    /* 流程已经终结,将最终的合并结果加入 */
    unit = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_clip_unit_t));
    unit->clip = clip_valid;
    scui_list_dln_reset(&unit->dl_node);
    scui_list_dll_ainsert(&clip_set->dl_list, NULL, &unit->dl_node);
    clip_set->size += scui_area_size(&unit->clip);
    clip_set->num++;
    
    return true;
}

/*@brief 移除剪切域
 *       调用者:检查剪切域有效性
 *@param clip_set 剪切域集合
 *@param clip     剪切域
 */
bool scui_clip_del(scui_clip_set_t *clip_set, scui_area_t *clip)
{
    scui_area_t clip_valid = *clip;
    SCUI_ASSERT(scui_area_inter(&clip_valid, &clip_set->clip, clip));
    
    for (scui_clip_unit_t *unit = NULL; !scui_clip_empty(clip_set); true) {
        /* 如果剪切域可以合并,合并后的剪切域要继续检查 */
        scui_list_dll_btra(&clip_set->dl_list, node) {
            unit = scui_own_ofs(scui_clip_unit_t, dl_node, node);
            /* 饶过不相交的剪切域,计算剪切域差集 */
            scui_area_t clip_inter = {0};
            if (scui_area_inter(&clip_inter, &unit->clip, &clip_valid)) {
                scui_list_dll_remove(&clip_set->dl_list, &unit->dl_node);
                clip_set->size -= scui_area_size(&unit->clip);
                clip_set->num--;
                /* 如果求不到余集,那就是全子集(clip4_num == 0) */
                
                scui_area_t  clip4[4] = {0};
                scui_coord_t clip4_num = 0;
                if (scui_area_differ(clip4, &clip4_num, &unit->clip, &clip_inter)) {
                    // 将多余的重新加回去, 当前循环就此终止
                    for (scui_coord_t idx = 0; idx < clip4_num; idx++)
                        scui_clip_add(clip_set, &clip4[idx]);
                }
                
                SCUI_MEM_FREE(unit);
                break;
            }
            
            unit = NULL;
        }
        
        // 循环一次未命中
        if (unit == NULL)
            break;
    }
    
    return true;
}
