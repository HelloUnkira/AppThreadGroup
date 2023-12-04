/*实现目标:
 *    胜者树
 *    败者树
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 每修改tree->leave[idx]的值都需要执行下面的动作
 *@param tree  树实例
 *@param idx 索引
 */
void app_sys_tree_fl_adjust(app_sys_tree_fl_t *tree, int32_t idx)
{
    int32_t init_val = (idx + tree->len - 1) >> 1;
    for (int32_t parent = init_val; parent > 0; parent >>= 1)
        if (tree->leave[idx] > tree->leave[tree->node[parent]]) {
            int32_t tmp = idx;
            idx = tree->node[parent];
            tree->node[parent] = tmp;
        }
    tree->node[0] = idx;
}

/*@brief 当tree->leave[X]被批量更新时执行该动作刷新
 *@param tree 树实例
 */
void app_sys_tree_fl_reflush(app_sys_tree_fl_t *tree)
{
    for (int32_t idx = 0; idx < tree->len; idx++)
        tree->node[idx] = 0;
    for (int32_t idx = tree->len; idx > 0; idx--)
        app_sys_tree_fl_adjust(tree, idx);
}

/*@brief 每修改tree->leave[idx]的值都需要执行下面的动作
 *@param tree  树实例
 *@param idx 索引
 */
static void app_sys_tree_fw_adjust_one(app_sys_tree_fw_t *tree, int32_t idx)
{
    /* 左孩子:(2 * idx + 0), 右孩子:(2 * idx + 1) */
    uint32_t child_l = 2 * idx + 0;
    uint32_t child_r = 2 * idx + 1;
    /* 获取左右孩子的值存储位点 */
    child_l = (child_l < tree->len) ? (tree->node[child_l]) : (child_l - tree->len + 1);
    child_r = (child_r < tree->len) ? (tree->node[child_r]) : (child_r - tree->len + 1);
    /* 进行一次比赛 */
    tree->node[idx] = tree->leave[child_l] <= tree->leave[child_r] ? child_l : child_r;
}

/*@brief 每修改tree->leave[idx]的值都需要执行下面的动作
 *@param tree  树实例
 *@param idx 索引
 */
void app_sys_tree_fw_adjust(app_sys_tree_fw_t *tree, int32_t idx)
{
    int32_t init_val = (idx + tree->len - 1) >> 1;
    for (int32_t parent = init_val; parent > 0; parent >>= 1)
        app_sys_tree_fw_adjust_one(tree, parent);
}

/*@brief 当tree->leave[X]被批量更新时执行该动作刷新
 *@param tree 树实例
 */
void app_sys_tree_fw_reflush(app_sys_tree_fw_t *tree)
{
    for (uint32_t idx = tree->len - 1; idx > 0; idx--)
        app_sys_tree_fw_adjust_one(tree, idx);
}
