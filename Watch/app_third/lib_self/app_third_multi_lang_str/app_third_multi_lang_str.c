/*一个代替注册的,脚本自动生成的,本地静态的字符串表
 *通过app_third_multi_lang_str.py生成
 *参考app_third_multi_lang_str.json中的模式生成源
 */

#include "app_std_lib.h"
#include "app_third_multi_lang_str.h"

static const char * app_third_multi_lang_str_table[15][3] = {
#if 0
	{"NULL","无效的","無効な",},
	{"Watch List","监视列表","監視リスト",},
	{"Calculator","计算器","計算機",},
	{"Stopwatch","秒表","ストップウォッチ",},
	{"Run","跑","走る",},
	{"Reset","调整","リセット",},
	{"Pause","暂停","一時停止",},
	{"Tick","打上钩","ひっかかる",},
	{"Countdown","倒计时","カウントダウン",},
	{"Run","跑","走る",},
	{"Set","设置","設定＃セッテイ＃",},
	{"Reset","调整","リセット",},
	{"Pause","暂停","一時停止",},
	{"Countdown Times Up","倒数计时","カウントダウン",},
	{"Confirm","证实","かくにん",},
#else
	{"","",""},
#endif
};

static uint32_t app_third_multi_lang_str_index = 0;

/*@brief设置搜索语言
 *@param[in] index语言编号(0~n-1)
 */
void app_third_multi_lang_str_default(uint32_t index)
{
	app_third_multi_lang_str_index = index;
}

/*@brief获得多国语字符串
 *@param[in] index字符串编号(0~n-1)
 */
const char * app_third_multi_lang_str_get(uint32_t index)
{
	return app_third_multi_lang_str_table[index][app_third_multi_lang_str_index];
}
