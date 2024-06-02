/*实现目标:
 *    字符串布局资源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 字符串预处理(变形,连字符,布局)
 *@param args    字符串绘制参数
 *@param name    字库名字句柄
 *@param utf8    字符串(utf8)
 *@param unicode 字符串(unicode)
 */
void scui_string_args_prepare(scui_string_args_t *args, scui_handle_t name, uint8_t *utf8)
{
    SCUI_ASSERT(args != NULL && utf8 != SCUI_HANDLE_INVALID);
    SCUI_ASSERT(name != SCUI_HANDLE_INVALID);
    
    if (args->unicode == NULL) {
        /* 统计字符数量 */
        args->number  = scui_utf8_str_num(utf8);
        args->unicode = SCUI_MEM_ALLOC(scui_mem_type_font, 4 * (args->number + 1));
        memset(args->unicode, 0, 4 * (args->number + 1));
        /* 将utf8转为unicode */
        scui_utf8_str_to_unicode(utf8, args->number, args->unicode);
    }
    
    /* 特定字体进行文字变形 */
    /* ... */
    
    /* 文字进行排版布局 */
    if (args->layout == NULL) {
        /* 一般单行不需要排版布局 */
        if (args->line_multi) {
            SCUI_ASSERT(false);
            /* ... */
        }
    }
}

/*@brief 字符串排版布局清除
 *@param layout 排版布局
 */
void scui_string_args_layout_clear(void *layout)
{
    if (layout != NULL)
        SCUI_MEM_FREE(layout);
}

/*@brief 字符串绘制信息清除
 *@param args 绘制信息清除
 */
void scui_string_args_clear(scui_string_args_t *args)
{
    scui_string_args_layout_clear(args->layout);
    
    if (args->unicode != NULL)
        SCUI_MEM_FREE(args->unicode);
}
