/*实现目标:
 *    泛型图(十字链表,重邻接表)测试
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

typedef struct {
    /* 边带一个权值 */
    uint32_t weight;
    app_sys_map_dle_t dlm_e;
} app_sys_map_dle_test_t;

typedef struct {
    /* 顶点带一个标号 */
    char label;
    app_sys_map_dlv_t dlm_v;
} app_sys_map_dlv_test_t;

bool app_sys_map_dlm_fc_test(app_sys_map_dlv_t *v1, app_sys_map_dlv_t *v2)
{
    app_sys_map_dlv_test_t *tv1 = app_sys_own_ofs(app_sys_map_dlv_test_t, dlm_v, v1);
    app_sys_map_dlv_test_t *tv2 = app_sys_own_ofs(app_sys_map_dlv_test_t, dlm_v, v2);
    
    return tv1->label == tv2->label;
}

void app_sys_map_dlm_fv_test(app_sys_map_dlv_t *v, app_sys_map_dle_t *e, char *str)
{
    app_sys_map_dlv_test_t *tv = app_sys_own_ofs(app_sys_map_dlv_test_t, dlm_v, v);
    app_sys_map_dle_test_t *te = app_sys_own_ofs(app_sys_map_dle_test_t, dlm_e, e);
    
    if (str != NULL)
        APP_SYS_LOG_DEBUG_RAW(str);
    if (v != NULL)
        APP_SYS_LOG_DEBUG_RAW("%c", tv->label);
    if (e != NULL)
        APP_SYS_LOG_DEBUG_RAW("%d", te->weight);
}

uint32_t app_sys_map_dle_test_w(app_sys_map_dle_t *e)
{
    app_sys_map_dle_test_t *te = app_sys_own_ofs(app_sys_map_dle_test_t, dlm_e, e);
    
    return te->weight;
}

void app_sys_map_dlm_test(void)
{
    #define APP_SYS_MAP_DLM_TEST_VERTEX     10
    
    app_sys_map_dlm_t dlm_m = {0};

    app_sys_map_dlm_reset(&dlm_m);
    app_sys_map_dlm_config(&dlm_m, app_sys_map_dlm_fc_test, app_sys_map_dlm_fv_test);
    
    /* 顶点 */
    for (uint32_t idx0 = 0; idx0 < APP_SYS_MAP_DLM_TEST_VERTEX; idx0++) {
        app_sys_map_dlv_test_t *tv = app_mem_alloc(sizeof(app_sys_map_dlv_test_t));
        tv->label = 'A' + idx0;
        app_sys_map_dlv_reset(&tv->dlm_v);
        app_sys_map_dlm_insert_v(&dlm_m, &tv->dlm_v);
    }
    
    APP_SYS_LOG_INFO("m v num:%d", app_sys_map_dlv_num(&dlm_m));
    /* 生成满环图(这里生成双边图) */
    for (uint32_t idx0 = 0; idx0 < APP_SYS_MAP_DLM_TEST_VERTEX; idx0++)
    for (uint32_t idx1 = 0; idx1 < APP_SYS_MAP_DLM_TEST_VERTEX; idx1++) {
        app_sys_map_dlv_test_t tv1 = {.label = 'A' + idx0};
        app_sys_map_dlv_test_t tv2 = {.label = 'A' + idx1};
        
        if (idx0 == idx1)
            continue;
        
        app_sys_map_dlv_t *v1 = app_sys_map_dlm_search_v(&dlm_m, &tv1.dlm_v);
        app_sys_map_dlv_t *v2 = app_sys_map_dlm_search_v(&dlm_m, &tv2.dlm_v);
        APP_SYS_ASSERT(v1 != NULL && v2 != NULL);
        
        if (app_sys_map_dlm_search_e(v1, v2) != NULL)
            continue;
        
        app_sys_map_dle_test_t *te = app_mem_alloc(sizeof(app_sys_map_dle_test_t));
        te->weight = 1 + rand() % 99;   //不等于0
        app_sys_map_dle_config(&te->dlm_e, v1, v2);
        app_sys_map_dlm_insert_e(&te->dlm_e);
    }
    
    /* 随机删除一些边 */
    for (uint32_t idx0 = 0; idx0 < APP_SYS_MAP_DLM_TEST_VERTEX * 2; idx0) {
        app_sys_map_dlv_test_t tv1 = {.label = 'A' + rand() % APP_SYS_MAP_DLM_TEST_VERTEX};
        app_sys_map_dlv_test_t tv2 = {.label = 'A' + rand() % APP_SYS_MAP_DLM_TEST_VERTEX};
        
        if (tv1.label == tv2.label)
            continue;
        
        app_sys_map_dlv_t *v1 = app_sys_map_dlm_search_v(&dlm_m, &tv1.dlm_v);
        app_sys_map_dlv_t *v2 = app_sys_map_dlm_search_v(&dlm_m, &tv2.dlm_v);
        APP_SYS_ASSERT(v1 != NULL && v2 != NULL);
        
        app_sys_map_dle_t *e = app_sys_map_dlm_search_e(v1, v2);
        if (e == NULL)
            continue;
        
        app_sys_map_dlm_remove_e(e);
        app_sys_map_dle_test_t *te = app_sys_own_ofs(app_sys_map_dle_test_t, dlm_e, e);
        app_mem_free(te);
        
        idx0++;
    }
    
    app_sys_map_dlm_visit_all(&dlm_m);
    
    /* 基本图论扩展 */
    {
        uint32_t v_cnt          = APP_SYS_MAP_DLM_TEST_VERTEX;
        bool               record[APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        app_sys_map_dlv_t  *stack[APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        app_sys_map_dlv_t  *queue[APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        app_sys_map_dlv_t *v_path[APP_SYS_MAP_DLM_TEST_VERTEX * APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        uint32_t           w_path[APP_SYS_MAP_DLM_TEST_VERTEX * APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        uint32_t    e_w_ary[APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        uint32_t  e_w_mat_t[APP_SYS_MAP_DLM_TEST_VERTEX * APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        uint32_t    e_w_mat[APP_SYS_MAP_DLM_TEST_VERTEX * APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        uint32_t      set_s[APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        uint32_t      set_p[APP_SYS_MAP_DLM_TEST_VERTEX] = {0};
        
        app_sys_map_dlm_args_t args = {
            .v_cnt  = v_cnt,
            .record = record,
            .stack  = stack,
            .queue  = queue,
            .v_path = v_path,
            .w_path = w_path,
            .e_w_cb     = app_sys_map_dle_test_w,
            .e_w_max    = 100 * APP_SYS_MAP_DLM_TEST_VERTEX,
            .e_w_ary    = e_w_ary,
            .e_w_mat_t  = e_w_mat_t,
            .e_w_mat    = e_w_mat,
            .set_s      = set_s,
            .set_p      = set_p,
        };
        
        /* 深度优先搜索 */
        memset(record, 0, sizeof(record));
        {
            APP_SYS_LOG_INFO("depth start");
            app_sys_map_dlv_test_t tv = {.label = 'A' + rand() % 5};
            app_sys_map_dlv_t *v = app_sys_map_dlm_search_v(&dlm_m, &tv.dlm_v);
            APP_SYS_LOG_INFO("check:%d", app_sys_map_dlm_first_d(&dlm_m, v, &args));
            APP_SYS_LOG_INFO("depth end");
        }
        
        /* 广度优先搜索 */
        memset(record, 0, sizeof(record));
        {
            APP_SYS_LOG_INFO("breadth start");
            app_sys_map_dlv_test_t tv = {.label = 'A' + rand() % 5};
            app_sys_map_dlv_t *v = app_sys_map_dlm_search_v(&dlm_m, &tv.dlm_v);
            APP_SYS_LOG_INFO("check:%d", app_sys_map_dlm_first_b(&dlm_m, v, &args));
            APP_SYS_LOG_INFO("breadth end");
        }
        
        /* 迪杰斯特拉最短路径搜索 */
        memset(record, 0, sizeof(record));
        memset(v_path, 0, sizeof(v_path));
        memset(e_w_ary, 0, sizeof(e_w_ary));
        {
            APP_SYS_LOG_INFO("dijkstra start");
            app_sys_map_dlv_test_t tv = {.label = 'A' + rand() % 5};
            app_sys_map_dlv_t *v = app_sys_map_dlm_search_v(&dlm_m, &tv.dlm_v);
            app_sys_map_dlm_dijkstra(&dlm_m, v, &args);
            
            APP_SYS_LOG_INFO("v_path:");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
                app_sys_map_dlm_fv_test(app_sys_map_dlm_idx_to_v(&dlm_m, idx1), NULL, "v:");
                APP_SYS_LOG_INFO_RAW("\t%u%s", e_w_ary[idx1], app_sys_log_line());
                APP_SYS_LOG_INFO_RAW("path:");
                for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
                    if (v_path[idx1 * v_cnt + idx2] != NULL)
                        app_sys_map_dlm_fv_test(v_path[idx1 * v_cnt + idx2], NULL, "->");
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            }
            APP_SYS_LOG_INFO("dijkstra end");
        }
        
        /* 贝尔曼弗德最短路径搜索 */
        memset(record, 0, sizeof(record));
        memset(v_path, 0, sizeof(v_path));
        memset(e_w_ary, 0, sizeof(e_w_ary));
        {
            APP_SYS_LOG_INFO("bellmanford start");
            app_sys_map_dlv_test_t tv = {.label = 'A' + rand() % 5};
            app_sys_map_dlv_t *v = app_sys_map_dlm_search_v(&dlm_m, &tv.dlm_v);
            APP_SYS_LOG_INFO("check:%d", app_sys_map_dlm_bellmanford(&dlm_m, v, &args));
            
            APP_SYS_LOG_INFO("v_path:");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
                app_sys_map_dlm_fv_test(app_sys_map_dlm_idx_to_v(&dlm_m, idx1), NULL, "v :");
                APP_SYS_LOG_INFO_RAW("\t%u%s", e_w_ary[idx1], app_sys_log_line());
                APP_SYS_LOG_INFO_RAW("Path:");
                for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
                    if (v_path[idx1 * v_cnt + idx2] != NULL)
                        app_sys_map_dlm_fv_test(v_path[idx1 * v_cnt + idx2], NULL, "->");
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            }
            APP_SYS_LOG_INFO("bellmanford end");
        }
        
        /* 弗洛伊德最短路径搜索 */
        memset(record, 0, sizeof(record));
        memset(w_path, 0, sizeof(w_path));
        memset(e_w_mat, 0, sizeof(e_w_mat));
        {
            APP_SYS_LOG_INFO("floyd start");
            app_sys_map_dlm_floyd(&dlm_m, &args);

            APP_SYS_LOG_INFO("e_w_mat:");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
            for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
                APP_SYS_LOG_INFO_RAW("%u\t", e_w_mat[idx1 * v_cnt + idx2]);
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            }
            
            APP_SYS_LOG_INFO("w_path:");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
            for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
                APP_SYS_LOG_INFO_RAW("%u\t", w_path[idx1 * v_cnt + idx2]);
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            }
            
            /* 随机输出一条最短路径 */
            while (1) {
                app_sys_map_dlv_test_t tv1 = {.label = 'A' + rand() % APP_SYS_MAP_DLM_TEST_VERTEX};
                app_sys_map_dlv_test_t tv2 = {.label = 'A' + rand() % APP_SYS_MAP_DLM_TEST_VERTEX};
                
                if (tv1.label == tv2.label)
                    continue;
                
                app_sys_map_dlv_t *v1 = app_sys_map_dlm_search_v(&dlm_m, &tv1.dlm_v);
                app_sys_map_dlv_t *v2 = app_sys_map_dlm_search_v(&dlm_m, &tv2.dlm_v);
                APP_SYS_ASSERT(v1 != NULL && v2 != NULL);
                
                uint32_t idx1 = app_sys_map_dlm_idx_from_v(&dlm_m, v1);
                uint32_t idx2 = app_sys_map_dlm_idx_from_v(&dlm_m, v2);
                uint32_t idx0 = w_path[idx1 * v_cnt + idx2];
                if (idx0 == idx2)
                    continue;
                
                app_sys_map_dlm_fv_test(v1, NULL, "v1:");
                APP_SYS_LOG_INFO_RAW("->");
                app_sys_map_dlm_fv_test(v2, NULL, "v2:");
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
                
                app_sys_map_dlm_fv_test(v1, NULL, "->");
                while (idx0 != idx2) {
                    app_sys_map_dlv_t *v = app_sys_map_dlm_idx_to_v(&dlm_m, idx0);
                    app_sys_map_dlm_fv_test(v, NULL, "->");
                    idx0 = w_path[idx0 * v_cnt + idx2];
                }
                app_sys_map_dlm_fv_test(v2, NULL, "->");
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
                
                break;
            }
            APP_SYS_LOG_INFO("floyd end");
        }
        
        /* 普利姆最小生成树 */
        memset(record, 0, sizeof(record));
        memset(e_w_mat_t, 0, sizeof(e_w_mat_t));
        memset(e_w_mat, 0, sizeof(e_w_mat));
        {
            APP_SYS_LOG_INFO("prim start");
            app_sys_map_dlm_prim(&dlm_m, &args);
            /*
            APP_SYS_LOG_INFO("e_w_mat_t:");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
            for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
                APP_SYS_LOG_INFO_RAW("%u\t", e_w_mat_t[idx1 * v_cnt + idx2]);
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            }
            APP_SYS_LOG_INFO("e_w_mat:");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
            for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
                APP_SYS_LOG_INFO_RAW("%u\t", e_w_mat[idx1 * v_cnt + idx2]);
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            }
            */
            APP_SYS_LOG_INFO("tree edge(no dir):");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
            for (uint32_t idx2 = idx1 + 1; idx2 < v_cnt; idx2++)
                if (e_w_mat[idx1 * v_cnt + idx2] != 0) {
                    app_sys_map_dlv_test_t tv1 = {.label = 'A' + idx1};
                    app_sys_map_dlv_test_t tv2 = {.label = 'A' + idx2};
                    app_sys_map_dlv_t *v1 = app_sys_map_dlm_search_v(&dlm_m, &tv1.dlm_v);
                    app_sys_map_dlv_t *v2 = app_sys_map_dlm_search_v(&dlm_m, &tv2.dlm_v);
                    APP_SYS_ASSERT(v1 != NULL && v2 != NULL);
                    APP_SYS_LOG_INFO_RAW("(");
                    app_sys_map_dlm_fv_test(v1, NULL, "");
                    APP_SYS_LOG_INFO_RAW(",");
                    app_sys_map_dlm_fv_test(v2, NULL, "");
                    APP_SYS_LOG_INFO_RAW(")");
                }
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            
            APP_SYS_LOG_INFO("prim end");
        }
        
        /* 凯斯特拉最小生成树 */
        memset(record, 0, sizeof(record));
        memset(e_w_mat_t, 0, sizeof(e_w_mat_t));
        memset(e_w_mat_t, 0, sizeof(e_w_mat_t));
        memset(e_w_mat, 0, sizeof(e_w_mat));
        {
            APP_SYS_LOG_INFO("kruskal start");
            app_sys_map_dlm_kruskal(&dlm_m, &args);
            /*
            APP_SYS_LOG_INFO("e_w_mat_t:");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
            for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
                APP_SYS_LOG_INFO_RAW("%u\t", e_w_mat_t[idx1 * v_cnt + idx2]);
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            }
            APP_SYS_LOG_INFO("e_w_mat:");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
            for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
                APP_SYS_LOG_INFO_RAW("%u\t", e_w_mat[idx1 * v_cnt + idx2]);
                APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            }
            */
            APP_SYS_LOG_INFO("tree edge(no dir):");
            for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
            for (uint32_t idx2 = idx1 + 1; idx2 < v_cnt; idx2++)
                if (e_w_mat[idx1 * v_cnt + idx2] != 0) {
                    app_sys_map_dlv_test_t tv1 = {.label = 'A' + idx1};
                    app_sys_map_dlv_test_t tv2 = {.label = 'A' + idx2};
                    app_sys_map_dlv_t *v1 = app_sys_map_dlm_search_v(&dlm_m, &tv1.dlm_v);
                    app_sys_map_dlv_t *v2 = app_sys_map_dlm_search_v(&dlm_m, &tv2.dlm_v);
                    APP_SYS_ASSERT(v1 != NULL && v2 != NULL);
                    APP_SYS_LOG_INFO_RAW("(");
                    app_sys_map_dlm_fv_test(v1, NULL, "");
                    APP_SYS_LOG_INFO_RAW(",");
                    app_sys_map_dlm_fv_test(v2, NULL, "");
                    APP_SYS_LOG_INFO_RAW(")");
                }
            APP_SYS_LOG_INFO_RAW(app_sys_log_line());
            
            APP_SYS_LOG_INFO("kruskal end");
        }
    }
}
