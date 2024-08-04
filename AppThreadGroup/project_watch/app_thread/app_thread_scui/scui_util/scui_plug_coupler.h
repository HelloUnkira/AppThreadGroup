#ifndef SCUI_PLUG_COUPLER_H
#define SCUI_PLUG_COUPLER_H

/*@brief 插件:连接器
 *       该辅助目标主要将主控件与子控件树(自定义控件树)
 *       之间构建一个桥梁,子控件树有一个自己的画布
 *       绘制流程中,将子控件树的画布作用到主控件的绘制流程之上
 *       主控件类型:custom
 *       从控件树类型:custom为根控件的控件树
 *@warn: 主控件必须要保证自己启用帧动画效果
         否则无法派发给从控件树帧动画事件
 */

typedef struct {
    scui_event_cb_t  list_widget_m_cb;  // 绘制标记回调
    scui_surface_t **list_surface_s;    // 从控件树画布列表(slave)
    scui_handle_t   *list_widget_s;     // 从控件树列表(slave)
    scui_handle_t   *list_widget_m;     // 主控件列表(master)
    scui_area_t     *list_draw_clip;    // 绘制目标剪切域
    scui_handle_t    list_draw_idx;     // 绘制目标
    scui_handle_t    list_num;          // 列表数量
    bool            *list_draw;         // 绘制标记
    bool            *list_refr;         // 刷新标记
} scui_plug_coupler_t;

/*@brief 插件:连接器:创建
 *@param coupler  连接器实例地址
 *@param list_num 控件数量
 */
void scui_plug_coupler_create(scui_plug_coupler_t **coupler, scui_handle_t list_num);

/*@brief 插件:连接器:销毁
 *@param coupler 连接器实例
 */
void scui_plug_coupler_destroy(scui_plug_coupler_t *coupler);

/*@brief 插件:连接器:回收
 *@param coupler 连接器实例
 *@param any     回收所有
 */
void scui_plug_coupler_recycle(scui_plug_coupler_t *coupler, bool any);

/*@brief 控件事件响应回调(需要包装)
 *@param coupler 连接器实例
 *@param event   事件
 */
void scui_plug_coupler_widget_s_event_proc(scui_plug_coupler_t *coupler, scui_event_t *event);

/*@brief 控件事件响应回调(需要包装)
 *@param coupler 连接器实例
 *@param event   事件
 */
void scui_plug_coupler_widget_m_event_proc(scui_plug_coupler_t *coupler, scui_event_t *event);

#endif
