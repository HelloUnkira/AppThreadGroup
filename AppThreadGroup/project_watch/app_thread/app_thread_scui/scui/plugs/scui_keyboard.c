/*实现目标:
 *    键盘布局与辅助信息
 *    本模组不含控件, 布局可由layout与btn组装
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 数字键盘 */
static const scui_keyboard_item_t scui_keyboard_item_num[] = {
    {'1', 1, 0}, {'2', 1, 0}, {'3', 1, 0},
    {'4', 1, 1}, {'5', 1, 1}, {'6', 1, 1},
    {'7', 1, 2}, {'8', 1, 2}, {'9', 1, 2},
    {scui_keyboard_key_sym, 1, 3},
    {'0', 1, 3},
    {scui_keyboard_key_del, 1, 3},
};

/* 字母键盘(小写) */
static const scui_keyboard_item_t scui_keyboard_item_word_l[] = {
    {'q', 1, 0}, {'w', 1, 0}, {'e', 1, 0}, {'r', 1, 0}, {'t', 1, 0},
    {'y', 1, 0}, {'u', 1, 0}, {'i', 1, 0}, {'o', 1, 0}, {'p', 1, 0},
    
    {'a', 1, 1}, {'s', 1, 1}, {'d', 1, 1}, {'f', 1, 1}, {'g', 1, 1},
    {'h', 1, 1}, {'j', 1, 1}, {'k', 1, 1}, {'l', 1, 1},
    
    {scui_keyboard_key_case, 1, 2},
    {'z', 1, 2}, {'x', 1, 2}, {'c', 1, 2}, {'v', 1, 2},
    {'b', 1, 2}, {'n', 1, 2}, {'m', 1, 2},
    {scui_keyboard_key_del, 1, 2},
    
    {scui_keyboard_key_sym,   1, 3},
    {scui_keyboard_key_lang,  1, 3},
    {scui_keyboard_key_space, 4, 3},
    {scui_keyboard_key_enter, 1, 3},
};

/* 字母键盘(大写) */
static const scui_keyboard_item_t scui_keyboard_item_word_u[] = {
    {'Q', 1, 0}, {'W', 1, 0}, {'E', 1, 0}, {'R', 1, 0}, {'T', 1, 0},
    {'Y', 1, 0}, {'U', 1, 0}, {'I', 1, 0}, {'O', 1, 0}, {'P', 1, 0},
    
    {'A', 1, 1}, {'S', 1, 1}, {'D', 1, 1}, {'F', 1, 1}, {'G', 1, 1},
    {'H', 1, 1}, {'J', 1, 1}, {'K', 1, 1}, {'L', 1, 1},
    
    {scui_keyboard_key_case, 1, 2},
    {'Z', 1, 2}, {'X', 1, 2}, {'C', 1, 2}, {'V', 1, 2},
    {'B', 1, 2}, {'N', 1, 2}, {'M', 1, 2},
    {scui_keyboard_key_del, 1, 2},
    
    {scui_keyboard_key_sym,   1, 3},
    {scui_keyboard_key_lang,  1, 3},
    {scui_keyboard_key_space, 4, 3},
    {scui_keyboard_key_enter, 1, 3},
};

/* 符号键盘 */
static const scui_keyboard_item_t scui_keyboard_item_sym[] = {
    {'1', 1, 0}, {'2', 1, 0}, {'3', 1, 0}, {'4', 1, 0}, {'5', 1, 0},
    {'6', 1, 0}, {'7', 1, 0}, {'8', 1, 0}, {'9', 1, 0}, {'0', 1, 0},
    
    {'-', 1, 1}, {'/', 1, 1}, {':', 1, 1}, {';', 1, 1}, {'(', 1, 1},
    {')', 1, 1}, {'&', 1, 1}, {'@', 1, 1}, {'~', 1, 1},
    
    {scui_keyboard_key_sym, 1, 2},
    {'.', 1, 2}, {',', 1, 2}, {'?', 1, 2}, {'!', 1, 2},
    {'\'', 1, 2}, {'#', 1, 2}, {'*', 1, 2},
    {scui_keyboard_key_del, 1, 2},
    
    {scui_keyboard_key_sym,   1, 3},
    {scui_keyboard_key_lang,  1, 3},
    {scui_keyboard_key_space, 4, 3},
    {scui_keyboard_key_enter, 1, 3},
};

/*@brief 按键表取值
 *@param type  布局类型
 *@param upper 字母大写
 *@param item  按键表输出
 *@retval 按键数量
 */
static scui_coord_t scui_keyboard_table(scui_keyboard_type_t type, bool upper,
    const scui_keyboard_item_t **item)
{
    if (type == scui_keyboard_type_num) {
        *item = scui_keyboard_item_num;
        return scui_arr_len(scui_keyboard_item_num);
    }
    if (type == scui_keyboard_type_word) {
        *item = upper ? scui_keyboard_item_word_u : scui_keyboard_item_word_l;
        return scui_arr_len(scui_keyboard_item_word_u);
    }
    if (type == scui_keyboard_type_sym) {
        *item = scui_keyboard_item_sym;
        return scui_arr_len(scui_keyboard_item_sym);
    }
    
    *item = NULL;
    return 0;
}

/*@brief 键盘布局查询
 *@param type   布局类型
 *@param upper  字母大写(word类型有效)
 *@param layout 布局输出
 */
void scui_keyboard_layout(scui_keyboard_type_t type, bool upper, scui_keyboard_layout_t *layout)
{
    SCUI_ASSERT(layout != NULL);
    
    const scui_keyboard_item_t *item = NULL;
    scui_coord_t num = scui_keyboard_table(type, upper, &item);
    
    layout->num     = num;
    layout->row_num = 0;
    layout->item    = item;
    
    /* 轨道数量由表推算, 避免与按键表脱节 */
    for (scui_coord_t idx = 0; idx < num; idx++)
    if (layout->row_num < item[idx].row + 1)
        layout->row_num  = item[idx].row + 1;
}

/*@brief 键盘按键查询
 *@param type 布局类型
 *@param idx  按键号
 *@retval 键值(0:越界)
 */
uint32_t scui_keyboard_code(scui_keyboard_type_t type, scui_coord_t idx)
{
    const scui_keyboard_item_t *item = NULL;
    scui_coord_t num = scui_keyboard_table(type, false, &item);
    if (idx < 0 || idx >= num)
        return scui_keyboard_key_none;
    
    return item[idx].code;
}

/*@brief 键盘单位总数查询(单轨)
 *@param type 布局类型
 *@param row  轨道号
 *@retval 单位总数(0:轨道为空)
 */
scui_coord_t scui_keyboard_unit(scui_keyboard_type_t type, scui_coord_t row)
{
    const scui_keyboard_item_t *item = NULL;
    scui_coord_t num = scui_keyboard_table(type, false, &item);
    
    scui_coord_t unit_cnt = 0;
    for (scui_coord_t idx = 0; idx < num; idx++)
    if (item[idx].row == row)
        unit_cnt += item[idx].unit > 0 ? item[idx].unit : 1;
    
    return unit_cnt;
}
