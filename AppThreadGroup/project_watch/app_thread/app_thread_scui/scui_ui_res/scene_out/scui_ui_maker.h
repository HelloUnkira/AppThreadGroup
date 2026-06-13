#ifndef SCUI_UI_MAKER_H
#define SCUI_UI_MAKER_H

/*一个通过scui_widget_parser脚本生成的widget动态构造器配置
 */

/*@brief 控件构造器默认初始化
 *@param maker 控件构造器实例指针
 *@param type  控件类型(scui_widget_type_t)
 */
void scui_widget_maker_def_cfg(void *maker, scui_widget_type_t type);

#endif
