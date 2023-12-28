#ifndef SCUI_ENGINE_H
#define SCUI_ENGINE_H

/*@brief 引擎就绪
 */
void scui_engine_ready(void);

/*@brief 引擎执行
 */
void scui_engine_execute(void);

/*@biref 设置引擎执行状态
 *@param 引擎是否执行
 */
bool scui_engine_execute_status_set(bool execute);

/*@biref 获得引擎执行状态
 *@retval 引擎是否执行
 */
bool scui_engine_execute_status_get(void);

#endif
