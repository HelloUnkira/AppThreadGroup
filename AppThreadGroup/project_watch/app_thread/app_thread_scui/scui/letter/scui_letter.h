#ifndef SCUI_LETTER_H
#define SCUI_LETTER_H

/*@brief 计算调色板值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 调色板值
 */
uint8_t scui_letter_bpp_palette(uint8_t bitmap, uint8_t bpp, uint8_t bpp_x);

/*****************************************************************************/
/* utf-8 tools<part>:******************************************************* */
/*****************************************************************************/

/*@brief utf8字符转unicode字符
 *@param utf8    utf8字符
 *@param unicode unicode字符
 *@retval utf8字符字节数
 */
uint32_t scui_utf8_to_unicode(uint8_t *utf8, uint32_t *unicode);

/*@brief utf8字符字节数
 *@param utf8 字符(首字符)
 *@retval 字符长度
 */
uint32_t scui_utf8_bytes(uint8_t utf8);

/*@brief utf8字符数量
 *@param utf8 字符串
 *@retval 字符数量
 */
uint32_t scui_utf8_str_num(uint8_t *utf8);

/*@brief utf8字节数量
 *@param utf8 字符串
 *@retval 字节数量
 */
uint32_t scui_utf8_str_bytes(uint8_t *utf8);

/*@brief 后缀匹配
 *@param str    utf8字符串
 *@param suffix utf8字符串
 */
bool scui_utf8_str_match_suffix(uint8_t *str, uint8_t *suffix);

/*****************************************************************************/
/* unicode tools<part>:***************************************************** */
/*****************************************************************************/

/*@brief utf8字符串构造生成unicode字符串
 *@param unicode unicode字符串
 *@param utf8    utf8字符串
 *@retval 字符数量
 */
uint32_t scui_unicode_str_by_utf8(uint32_t **unicode, uint8_t *utf8);

/*****************************************************************************/
/* tools end:*************************************************************** */
/*****************************************************************************/

#endif
