/*实现目标:
 *    胜者树测试
 *    败者树测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

void app_sys_tree_fl_test(void)
{
    int32_t min = -100;
    int32_t max = +100;
    int32_t node[10] = {0};
    int32_t leave[10 + 1] = {min, };
    for (uint32_t idx = 1; idx < 11; idx++)
        leave[idx] = rand() % 10;
    
    app_sys_tree_fl_t fl_tree = {
        .leave = leave,
        .node  = node,
        .len   = 10,
    };
    app_sys_tree_fl_reflush(&fl_tree);
    
    /* fl_tree */
    for (uint32_t idx = 0; idx < 10; idx++) {
        APP_SYS_LOG_DEBUG_RAW("node:");
        for (uint32_t I = 0; I < 10; I++)
            APP_SYS_LOG_DEBUG_RAW("%2u ", fl_tree.node[I]);
            APP_SYS_LOG_DEBUG_RAW(app_sys_log_line());
        
        APP_SYS_LOG_DEBUG_RAW("leave:");
        for (uint32_t I = 1; I < 11; I++)
            APP_SYS_LOG_DEBUG_RAW("%2u ", fl_tree.leave[I]);
            APP_SYS_LOG_DEBUG_RAW(app_sys_log_line());
    }
    
    APP_SYS_LOG_INFO_RAW("fl_tree:");
    for (uint32_t idx = 0; idx < 10; idx++) {
        APP_SYS_LOG_INFO_RAW("%d, ", fl_tree.leave[fl_tree.node[0]]);
        fl_tree.leave[fl_tree.node[0]] = max;
        app_sys_tree_fl_adjust(&fl_tree, fl_tree.node[0]);
    }
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
}

void app_sys_tree_fw_test(void)
{
    int32_t min = -100;
    int32_t max = +100;
    int32_t node[10] = {0};
    int32_t leave[10 + 1] = {min, };
    for (uint32_t idx = 1; idx < 11; idx++)
        leave[idx] = rand() % 10;
    
    app_sys_tree_fw_t fw_tree = {
        .leave = leave,
        .node  = node,
        .len   = 10,
    };
    app_sys_tree_fw_reflush(&fw_tree);
    
    /* fw_tree */
    for (uint32_t idx = 0; idx < 10; idx++) {
        APP_SYS_LOG_DEBUG_RAW("node:");
        for (uint32_t I = 0; I < 10; I++)
            APP_SYS_LOG_DEBUG_RAW("%2u ", fw_tree.node[I]);
            APP_SYS_LOG_DEBUG_RAW(app_sys_log_line());
        
        APP_SYS_LOG_DEBUG_RAW("leave:");
        for (uint32_t I = 1; I < 11; I++)
            APP_SYS_LOG_DEBUG_RAW("%2u ", fw_tree.leave[I]);
            APP_SYS_LOG_DEBUG_RAW(app_sys_log_line());
    }
    
    APP_SYS_LOG_INFO_RAW("fw_tree:");
    for (uint32_t idx = 0; idx < 10; idx++) {
        APP_SYS_LOG_INFO_RAW("%d, ", fw_tree.leave[fw_tree.node[1]]);
        fw_tree.leave[fw_tree.node[1]] = max;
        app_sys_tree_fw_adjust(&fw_tree, fw_tree.node[1]);
    }
    APP_SYS_LOG_INFO_RAW(app_sys_log_line());
}
