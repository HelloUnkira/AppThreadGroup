#ifndef APP_SYS_TREE_FD_H
#define APP_SYS_TREE_FD_H

/* 当前决策树只提供胜者树与败者树 */

typedef struct {
    int32_t *node;     /* 长度:len, 0号元素为最小值索引 */
    int32_t *leave;    /* 长度:len + 1, 0号元素填充假值, 为数据下界 */
    int32_t  len;
} app_sys_tree_fl_t;

typedef struct {
    uint32_t *node;     /* 长度:len, 1号元素为最大值索引 */
    uint32_t *leave;    /* 长度:len + 1 */
    uint32_t  len;
} app_sys_tree_fw_t;

/*@brief 每修改tree->leave[idx]的值都需要执行下面的动作
 *@param tree  树实例
 *@param idx 索引
 */
void app_sys_tree_fl_adjust(app_sys_tree_fl_t *tree, int32_t idx);

/*@brief 当tree->leave[X]被批量更新时执行该动作刷新
 *@param tree 树实例
 */
void app_sys_tree_fl_reflush(app_sys_tree_fl_t *tree);

/*@brief 每修改tree->leave[idx]的值都需要执行下面的动作
 *@param tree  树实例
 *@param idx 索引
 */
void app_sys_tree_fw_adjust(app_sys_tree_fw_t *tree, int32_t idx);

/*@brief 当tree->leave[X]被批量更新时执行该动作刷新
 *@param tree 树实例
 */
void app_sys_tree_fw_reflush(app_sys_tree_fw_t *tree);

#endif
