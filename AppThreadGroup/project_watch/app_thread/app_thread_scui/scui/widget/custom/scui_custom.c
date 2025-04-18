/*实现目标:
 *    控件
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/*@brief 控件构造
 *@param inst       控件实例
 *@param inst_maker 控件实例构造器
 *@param handle     控件句柄
 *@param layout     通过布局创建
 */
void scui_custom_make(void *inst, void *inst_maker, scui_handle_t *handle, bool layout)
{
    /* 基类对象 */
    scui_widget_t *widget = inst;
    scui_widget_maker_t *widget_maker = inst_maker;
    /* 本类对象 */
    scui_custom_t *custom = widget;
    scui_custom_maker_t *custom_maker = widget_maker;
    
    /* 构造基础控件实例 */
    scui_widget_make(widget, widget_maker, handle, layout);
    SCUI_ASSERT(scui_widget_type_check(*handle, scui_widget_type_custom));
    // 自定义控件既可以是根控件(子画布控件树),也可以是非根控件
}

/*@brief 控件析构
 *@param handle 控件句柄
 */
void scui_custom_burn(scui_handle_t handle)
{
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    // 回收绘制描述符(全局)
    SCUI_MEM_FREE(custom->draw_dsc);
    
    /* 析构基础控件实例 */
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_burn(widget);
}

/*@brief 自定义控件绘制描述符(全局,唯一)
 *       一个控件仅使用一个绘制描述符
 *@param handle 自定义控件句柄
 */
void scui_custom_draw_dsc(scui_handle_t handle, void **draw_dsc)
{
    SCUI_ASSERT(scui_widget_type_check(handle, scui_widget_type_custom));
    scui_widget_t *widget = scui_handle_source_check(handle);
    scui_custom_t *custom = (void *)widget;
    
    SCUI_ASSERT(draw_dsc != NULL);
    if (custom->draw_dsc == NULL) {
        custom->draw_dsc  = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_custom_draw_dsc_t));
        memset(custom->draw_dsc, 0, sizeof(scui_custom_draw_dsc_t));
    }
    
    SCUI_ASSERT(custom->draw_dsc != NULL);
    *draw_dsc = custom->draw_dsc;
}
