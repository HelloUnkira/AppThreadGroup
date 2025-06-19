#ifndef APP_SYS_LIB_H
#define APP_SYS_LIB_H

/*  */
#include "app_sys_misc.h"
#include "app_sys_opt.h"
/*  */
#include "app_sys_log.h"
#include "app_sys_log_text.h"
/*  */
#include "app_sys_list_l.h"
#include "app_sys_heap_g.h"
#include "app_sys_tree_s.h"
#include "app_sys_tree_rb.h"
#include "app_sys_tree_rbs.h"
#include "app_sys_tree_bs.h"
#include "app_sys_tree_fd.h"
#include "app_sys_table_dl.h"
#include "app_sys_table_rbs.h"
#include "app_sys_map_dl.h"
#include "app_sys_handle.h"
#include "app_sys_rbuf.h"
/*  */
#include "app_sys_mem_dir.h"
#include "app_sys_mem_olsf.h"
#include "app_sys_mem_tlsf.h"
#include "app_sys_mem_slab.h"
/*  */
#include "app_sys_matrix.h"
#include "app_sys_fft.h"
/*  */
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_cache.h"
#include "app_sys_ext_mem_table.h"
#include "app_sys_ext_mem_src.h"
#include "app_sys_ext_mem_src_table.h"
/*  */
#include "app_sys_timer.h"
#include "app_sys_clock.h"
#include "app_sys_pipe.h"

typedef struct {
    app_sys_log_t log;
} app_sys_cfg_t;

/*@brief 初始化系统
 *@param cfg 配置参数
 */
void app_sys_ready(app_sys_cfg_t *cfg);

/*@brief 系统组件库测试
 */
bool app_sys_test(void);

#endif