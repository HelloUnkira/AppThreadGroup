#ifndef APP_MODULE_FUSION_ANALYSIS_H
#define APP_MODULE_FUSION_ANALYSIS_H

#define APP_MODULE_FUSION_ANALYSIS_XMSEC    200     /* 一次主动测量时间 */

typedef struct {
    void *data;
} app_module_fusion_analysis_t;

/*@brief 数据融合分析模组事件处理
 *       内部使用: 被mix custom线程使用
 */
void app_module_fusion_analysis_xmsec_update(void);

/*@brief 启动数据融合分析
 *       此时手势模组工作在轮询模式,中断响应关闭
 */
void app_module_fusion_analysis_start(void);

/*@brief 关闭数据融合分析
 *       此时手势模组工作在中断模式,轮询响应关闭
 */
void app_module_fusion_analysis_stop(void);

/*@brief 初始化数据融合分析模组
 */
void app_module_fusion_analysis_ready(void);

#endif
