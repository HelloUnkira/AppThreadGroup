/* 实现目标:
 *     层级接口打包
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief 初始化系统
 *@param cfg 配置参数
 */
void app_sys_ready(app_sys_cfg_t *cfg)
{
    /* 就绪系统子模组 */
    app_sys_log_ready(&cfg->log);
    app_sys_ext_mem_ready();
    app_sys_ext_mem_layout();
    app_sys_log_text_ready();
    app_sys_timer_ready();
    app_sys_build_time();
}

/*@brief 系统组件库测试
 */
bool app_sys_test(void)
{
    #if 0
    #elif 0
    /* 系统组件插件测试,仅独立测试使用 */
    /* 测试接口有大量内存申请未释放的动作 */
    void app_sys_list_dll_test(void);
         app_sys_list_dll_test();
    void app_sys_list_sll_test(void);
         app_sys_list_sll_test();
    void app_sys_heap_g_test(void);
         app_sys_heap_g_test();
    void app_sys_tree_s_test(void);
         app_sys_tree_s_test();
    void app_sys_tree_rb_test(void);
         app_sys_tree_rb_test();
    void app_sys_tree_rbs_test(void);
         app_sys_tree_rbs_test();
    void app_sys_tree_bs_test(void);
         app_sys_tree_bs_test();
    void app_sys_tree_fl_test(void);
         app_sys_tree_fl_test();
    void app_sys_tree_fw_test(void);
         app_sys_tree_fw_test();
    void app_sys_table_dl_test(void);
         app_sys_table_dl_test();
    void app_sys_table_rbs_test(void);
         app_sys_table_rbs_test();
    void app_sys_map_dlm_test(void);
         app_sys_map_dlm_test();
    void app_sys_handle_test(void);
         app_sys_handle_test();
    void app_sys_rbuf_test(void);
         app_sys_rbuf_test();
    /*  */
    return true;
    #elif 0
    void app_sys_mem_dir_test(void);
    app_sys_mem_dir_test();
    void app_sys_mem_olsf_test(void);
    app_sys_mem_olsf_test();
    void app_sys_mem_tlsf_test(void);
    app_sys_mem_tlsf_test();
    /*  */
    return true;
    #elif 0
    void app_sys_matrix_test(void);
         app_sys_matrix_test();
    void app_sys_fft_test(void);
         app_sys_fft_test();
    /*  */
    return true;
    #elif 0
    /* test sys log text */
    void app_sys_log_text_test(void);
         app_sys_log_text_test();
    /*  */
    return true;
    #elif 0
    /* test sys mem cache */
    void app_sys_ext_mem_cache_test(void);
         app_sys_ext_mem_cache_test();
    /*  */
    return true;
    #elif 0
    /* test sys timer */
    void app_sys_timer_test(void);
         app_sys_timer_test();
    /* 软件定时器依赖调度,继续测试 */
    return false;
    #else
    #endif
    /* 无测试,继续 */
    return false;
}
