#ifndef SCUI_DISP_H
#define SCUI_DISP_H

typedef struct {
    scui_coord_t hor_res;
    scui_coord_t ver_res;
} scui_disp_t;

/*@brief 设置显示器水平距离
 *@param hor_res 水平距离
 */
void scui_disp_set_hor_res(scui_coord_t hor_res);

/*@brief 设置显示器垂直距离
 *@param ver_res 垂直距离
 */
void scui_disp_set_ver_res(scui_coord_t ver_res);

/*@brief 获取显示器水平距离
 *@retval 水平距离
 */
scui_coord_t scui_disp_get_hor_res(void);

/*@brief 获取显示器垂直距离
 *@retval 垂直距离
 */
scui_coord_t scui_disp_get_ver_res(void);

/*@brief 显示器初始化
 */
void scui_disp_ready(void);

#endif
