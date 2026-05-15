#ifndef SCUI_SYMBOL_INF_H
#define SCUI_SYMBOL_INF_H

/* 符号字符串: */
/* 从lvgl舶来的标签,试试效果 */
/*In the font converter use this list as range:
      61441, 61448, 61451, 61452, 61453, 61457, 61459, 61461, 61465, 61468,
      61473, 61478, 61479, 61480, 61502, 61507, 61512, 61515, 61516, 61517,
      61521, 61522, 61523, 61524, 61543, 61544, 61550, 61552, 61553, 61556,
      61559, 61560, 61561, 61563, 61587, 61589, 61636, 61637, 61639, 61641,
      61664, 61671, 61674, 61683, 61724, 61732, 61787, 61931, 62016, 62017,
      62018, 62019, 62020, 62087, 62099, 62189, 62212, 62810, 63426, 63650
*/

/*****************************************************************************/

/*@brief 符号字符编码
 *@param symbol 符号字符串
 *@retval 符号字符编码
 */
uint32_t scui_symbol_code(uint8_t *symbol);

/*@brief 符号字符区域
 *@param font_name 符号字库名字句柄
 *@param symbol    符号字符编码
 *@retval 符号字符区域
 */
scui_area_t scui_symbol_area(scui_handle_t font_name, uint32_t symbol);

/*****************************************************************************/

#define SCUI_SYMBOL_STR_AUDIO                   "\xEF\x80\x81" /*61441, 0xF001*/
#define SCUI_SYMBOL_STR_VIDEO                   "\xEF\x80\x88" /*61448, 0xF008*/
#define SCUI_SYMBOL_STR_LIST                    "\xEF\x80\x8B" /*61451, 0xF00B*/
#define SCUI_SYMBOL_STR_OK                      "\xEF\x80\x8C" /*61452, 0xF00C*/
#define SCUI_SYMBOL_STR_CLOSE                   "\xEF\x80\x8D" /*61453, 0xF00D*/
#define SCUI_SYMBOL_STR_POWER                   "\xEF\x80\x91" /*61457, 0xF011*/
#define SCUI_SYMBOL_STR_SETTINGS                "\xEF\x80\x93" /*61459, 0xF013*/
#define SCUI_SYMBOL_STR_HOME                    "\xEF\x80\x95" /*61461, 0xF015*/
#define SCUI_SYMBOL_STR_DOWNLOAD                "\xEF\x80\x99" /*61465, 0xF019*/
#define SCUI_SYMBOL_STR_DRIVE                   "\xEF\x80\x9C" /*61468, 0xF01C*/
#define SCUI_SYMBOL_STR_REFRESH                 "\xEF\x80\xA1" /*61473, 0xF021*/
#define SCUI_SYMBOL_STR_MUTE                    "\xEF\x80\xA6" /*61478, 0xF026*/
#define SCUI_SYMBOL_STR_VOLUME_MID              "\xEF\x80\xA7" /*61479, 0xF027*/
#define SCUI_SYMBOL_STR_VOLUME_MAX              "\xEF\x80\xA8" /*61480, 0xF028*/
#define SCUI_SYMBOL_STR_IMAGE                   "\xEF\x80\xBE" /*61502, 0xF03E*/
#define SCUI_SYMBOL_STR_TINT                    "\xEF\x81\x83" /*61507, 0xF043*/
#define SCUI_SYMBOL_STR_PREV                    "\xEF\x81\x88" /*61512, 0xF048*/
#define SCUI_SYMBOL_STR_PLAY                    "\xEF\x81\x8B" /*61515, 0xF04B*/
#define SCUI_SYMBOL_STR_PAUSE                   "\xEF\x81\x8C" /*61516, 0xF04C*/
#define SCUI_SYMBOL_STR_STOP                    "\xEF\x81\x8D" /*61517, 0xF04D*/
#define SCUI_SYMBOL_STR_NEXT                    "\xEF\x81\x91" /*61521, 0xF051*/
#define SCUI_SYMBOL_STR_EJECT                   "\xEF\x81\x92" /*61522, 0xF052*/
#define SCUI_SYMBOL_STR_LEFT                    "\xEF\x81\x93" /*61523, 0xF053*/
#define SCUI_SYMBOL_STR_RIGHT                   "\xEF\x81\x94" /*61524, 0xF054*/
#define SCUI_SYMBOL_STR_PLUS                    "\xEF\x81\xA7" /*61543, 0xF067*/
#define SCUI_SYMBOL_STR_MINUS                   "\xEF\x81\xA8" /*61544, 0xF068*/
#define SCUI_SYMBOL_STR_EYE_OPEN                "\xEF\x81\xAE" /*61550, 0xF06E*/
#define SCUI_SYMBOL_STR_EYE_CLOSE               "\xEF\x81\xB0" /*61552, 0xF070*/
#define SCUI_SYMBOL_STR_WARNING                 "\xEF\x81\xB1" /*61553, 0xF071*/
#define SCUI_SYMBOL_STR_SHUFFLE                 "\xEF\x81\xB4" /*61556, 0xF074*/
#define SCUI_SYMBOL_STR_UP                      "\xEF\x81\xB7" /*61559, 0xF077*/
#define SCUI_SYMBOL_STR_DOWN                    "\xEF\x81\xB8" /*61560, 0xF078*/
#define SCUI_SYMBOL_STR_LOOP                    "\xEF\x81\xB9" /*61561, 0xF079*/
#define SCUI_SYMBOL_STR_DIRECTORY               "\xEF\x81\xBB" /*61563, 0xF07B*/
#define SCUI_SYMBOL_STR_UPLOAD                  "\xEF\x82\x93" /*61587, 0xF093*/
#define SCUI_SYMBOL_STR_CALL                    "\xEF\x82\x95" /*61589, 0xF095*/
#define SCUI_SYMBOL_STR_CUT                     "\xEF\x83\x84" /*61636, 0xF0C4*/
#define SCUI_SYMBOL_STR_COPY                    "\xEF\x83\x85" /*61637, 0xF0C5*/
#define SCUI_SYMBOL_STR_SAVE                    "\xEF\x83\x87" /*61639, 0xF0C7*/
#define SCUI_SYMBOL_STR_BARS                    "\xEF\x83\x89" /*61641, 0xF0C9*/
#define SCUI_SYMBOL_STR_ENVELOPE                "\xEF\x83\xA0" /*61664, 0xF0E0*/
#define SCUI_SYMBOL_STR_CHARGE                  "\xEF\x83\xA7" /*61671, 0xF0E7*/
#define SCUI_SYMBOL_STR_PASTE                   "\xEF\x83\xAA" /*61674, 0xF0EA*/
#define SCUI_SYMBOL_STR_BELL                    "\xEF\x83\xB3" /*61683, 0xF0F3*/
#define SCUI_SYMBOL_STR_KEYBOARD                "\xEF\x84\x9C" /*61724, 0xF11C*/
#define SCUI_SYMBOL_STR_GPS                     "\xEF\x84\xA4" /*61732, 0xF124*/
#define SCUI_SYMBOL_STR_FILE                    "\xEF\x85\x9B" /*61787, 0xF158*/
#define SCUI_SYMBOL_STR_WIFI                    "\xEF\x87\xAB" /*61931, 0xF1EB*/
#define SCUI_SYMBOL_STR_BATTERY_FULL            "\xEF\x89\x80" /*62016, 0xF240*/
#define SCUI_SYMBOL_STR_BATTERY_3               "\xEF\x89\x81" /*62017, 0xF241*/
#define SCUI_SYMBOL_STR_BATTERY_2               "\xEF\x89\x82" /*62018, 0xF242*/
#define SCUI_SYMBOL_STR_BATTERY_1               "\xEF\x89\x83" /*62019, 0xF243*/
#define SCUI_SYMBOL_STR_BATTERY_EMPTY           "\xEF\x89\x84" /*62020, 0xF244*/
#define SCUI_SYMBOL_STR_USB                     "\xEF\x8a\x87" /*62087, 0xF287*/
#define SCUI_SYMBOL_STR_BLUETOOTH               "\xEF\x8a\x93" /*62099, 0xF293*/
#define SCUI_SYMBOL_STR_TRASH                   "\xEF\x8B\xAD" /*62189, 0xF2ED*/
#define SCUI_SYMBOL_STR_EDIT                    "\xEF\x8C\x84" /*62212, 0xF304*/
#define SCUI_SYMBOL_STR_BACKSPACE               "\xEF\x95\x9A" /*62810, 0xF55A*/
#define SCUI_SYMBOL_STR_SD_CARD                 "\xEF\x9F\x82" /*63426, 0xF7C2*/
#define SCUI_SYMBOL_STR_NEW_LINE                "\xEF\xA2\xA2" /*63650, 0xF8A2*/

#define SCUI_SYMBOL_STR_BULLET                  "\xE2\x80\xA2" /*20042, 0x2022*/
#define SCUI_SYMBOL_STR_DUMMY                   "\xEF\xA3\xBF" /*63743,.0xF8FF */

/*****************************************************************************/

#endif
