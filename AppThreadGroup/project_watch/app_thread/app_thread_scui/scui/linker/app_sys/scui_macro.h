#ifndef SCUI_MACRO_H
#define SCUI_MACRO_H

/* 转为适配app_sys_lib宏
 */

#define scui_mem_r          app_sys_mem_r
#define scui_mem_w          app_sys_mem_w
#define scui_mabs           app_sys_mabs
#define scui_abs            app_sys_abs
#define scui_min            app_sys_min
#define scui_max            app_sys_max
#define scui_dist           app_sys_dist
#define scui_betw_xx        app_sys_betw_xx
#define scui_betw_lx        app_sys_betw_lx
#define scui_betw_xr        app_sys_betw_xr
#define scui_betw_lr        app_sys_betw_lr
#define scui_map            app_sys_map
#define scui_clamp          app_sys_clamp
#define scui_arr_len        app_sys_arr_len
#define scui_own_ofs        app_sys_own_ofs
#define scui_pow2_check     app_sys_pow2_check

#define scui_stringify      app_sys_stringify
#define scui_concat         app_sys_concat
#define scui_crc32          app_sys_crc32
#define scui_crc8           app_sys_crc8

#endif
