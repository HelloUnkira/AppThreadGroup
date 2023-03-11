#ifndef app_third_multi_lang_str_H
#define app_third_multi_lang_str_H

/*一个代替注册的,脚本自动生成的,本地静态的字符串表
 *通过app_third_multi_lang_str.py生成
 *参考app_third_multi_lang_str.json中的模式生成源
 */

typedef enum {
	app_third_multi_lang_str_0X0000,	/* NULL */
	app_third_multi_lang_str_0X0001,	/* Watch List */
	app_third_multi_lang_str_0X0002,	/* Calculator */
	app_third_multi_lang_str_0X0003,	/* Stopwatch */
	app_third_multi_lang_str_0X0004,	/* Run */
	app_third_multi_lang_str_0X0005,	/* Reset */
	app_third_multi_lang_str_0X0006,	/* Pause */
	app_third_multi_lang_str_0X0007,	/* Tick */
	app_third_multi_lang_str_0X0008,	/* Countdown */
	app_third_multi_lang_str_0X0009,	/* Run */
	app_third_multi_lang_str_0X000a,	/* Set */
	app_third_multi_lang_str_0X000b,	/* Reset */
	app_third_multi_lang_str_0X000c,	/* Pause */
	app_third_multi_lang_str_0X000d,	/* Countdown Times Up */
	app_third_multi_lang_str_0X000e,	/* Confirm */
	app_third_multi_lang_str_NUM,
} app_third_multi_lang_str_t;

/*@brief设置搜索语言
 *@param[in] index语言编号(0~n-1)
 */
void app_third_multi_lang_str_default(uint32_t index);

/*@brief获得多国语字符串
 *@param[in] index字符串编号(0~n-1)
 */
const char * app_third_multi_lang_str_get(uint32_t index);

#endif
