#ifndef SCUI_CLIP_H
#define SCUI_CLIP_H

typedef struct {
    scui_list_dln_t dl_node;
    scui_area_t     clip;       /* 局部剪切域 */
} scui_clip_unit_t;

typedef struct {
    scui_list_dll_t dl_list;
    scui_area_t     clip;       /* 全局剪切域 */
} scui_clip_set_t;

/*@brief 剪切域为空
 *@param clip_set 剪切域集合
 *@retval 是否为空
 */
bool scui_clip_empty(scui_clip_set_t *clip_set);

/*@brief 清空剪切域
 *@param clip_set 剪切域集合
 */
void scui_clip_clear(scui_clip_set_t *clip_set);

/*@brief 添加剪切域
 *@param clip_set 剪切域集合
 *@param clip     剪切域
 */
bool scui_clip_add(scui_clip_set_t *clip_set, scui_area_t *clip);

/*@brief 移除剪切域
 *@param clip_set 剪切域集合
 *@param clip     剪切域
 */
bool scui_clip_del(scui_clip_set_t *clip_set, scui_area_t *clip);

#endif
