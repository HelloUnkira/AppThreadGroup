#ifndef APP_SYS_MAP_OL_H
#define APP_SYS_MAP_OL_H

/* 载体为十字链表,重邻接表的图 */
/* 链表容器扩容的图 */
/* 无向图==双向权值一致的有向图 */

typedef struct {
    app_sys_list_dn_t buddy;    /* 伙伴顶点项 */
    app_sys_list_dl_t e_set_i;  /* 入度边集合 */
    app_sys_list_dl_t e_set_o;  /* 出度边集合 */
} app_sys_map_dlv_t;

typedef struct {
    app_sys_list_dn_t e_item_i; /* 入度边项 */
    app_sys_list_dn_t e_item_o; /* 出度边项 */
    app_sys_map_dlv_t *v_i;    /* 弧的发起者 */
    app_sys_map_dlv_t *v_o;    /* 弧的接受者 */
} app_sys_map_dle_t;

typedef bool (*app_sys_map_dlm_fc_t)(app_sys_map_dlv_t *v_i, app_sys_map_dlv_t *v_o);
typedef void (*app_sys_map_dlm_fv_t)(app_sys_map_dlv_t *v,   app_sys_map_dle_t *e, char *str);

typedef struct {
    app_sys_list_dl_t set;         /* 顶点集合 */
    app_sys_map_dlm_fc_t confirm;    /* 比较函数 */
    app_sys_map_dlm_fv_t visit;      /* 访问函数 */
} app_sys_map_dlm_t;

/*@brief     图顶点重置
 *@param[in] v 图顶点实例
 */
void app_sys_map_dlv_reset(app_sys_map_dlv_t *v);

/*@brief     图边重置
 *@param[in] e 图边实例
 */
void app_sys_map_dle_reset(app_sys_map_dle_t *e);

/*@brief     图重置
 *@param[in] m 图实例
 */
void app_sys_map_dlm_reset(app_sys_map_dlm_t *m);

/*@brief     图边重置
 *@param[in] e   图边实例
 *@param[in] v_i 图顶点实例
 *@param[in] v_o 图顶点实例
 */
void app_sys_map_dle_config(app_sys_map_dle_t *e, app_sys_map_dlv_t *v_i, app_sys_map_dlv_t *v_o);

/*@brief     图重置
 *@param[in] m       图实例
 *@param[in] confirm 图比较函数
 *@param[in] visit   图访问函数
 */
void app_sys_map_dlm_config(app_sys_map_dlm_t *m, app_sys_map_dlm_fc_t confirm, app_sys_map_dlm_fv_t visit);

/*@brief     从图边获得图顶点
 *@param[in] e 图边实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dle_v_i(app_sys_map_dle_t *e);

/*@brief     从图边获得图顶点
 *@param[in] e 图边实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dle_v_o(app_sys_map_dle_t *e);

/*@brief     获得图起始顶点
 *@param[in] m 图实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_head_v(app_sys_map_dlm_t *m);

/*@brief     获得图末尾顶点
 *@param[in] m 图实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_tail_v(app_sys_map_dlm_t *m);

/*@brief     获得图顶点前一顶点
 *@param[in] m 图实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_prev_v(app_sys_map_dlv_t *v);

/*@brief     获得图顶点后一顶点
 *@param[in] m 图实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_next_v(app_sys_map_dlv_t *v);

/*@brief     获得图顶点起始边
 *@param[in] v 图顶点实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_head_e(app_sys_map_dlv_t *v);

/*@brief     获得图顶点末尾边
 *@param[in] v 图顶点实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_tail_e(app_sys_map_dlv_t *v);

/*@brief     获得图边前一边
 *@param[in] e 图边实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_prev_e(app_sys_map_dle_t *e);

/*@brief     获得图边后一边
 *@param[in] e 图边实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_next_e(app_sys_map_dle_t *e);

/*@brief     搜索图顶点
 *@param[in] m 图实例
 *@param[in] v 图顶点实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_search_v(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v);

/*@brief     插入图顶点
 *@param[in] m 图实例
 *@param[in] v 图顶点实例
 */
void app_sys_map_dlm_insert_v(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v);

/*@brief     删除图顶点
 *@param[in] m 图实例
 *@param[in] v 图顶点实例
 */
void app_sys_map_dlm_remove_v(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v);

/*@brief     搜索图边
 *@param[in] v_i 图顶点实例
 *@param[in] v_o 图顶点实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_search_e(app_sys_map_dlv_t *v_i, app_sys_map_dlv_t *v_o);

/*@brief     插入图边
 *@param[in] e 图边实例
 */
void app_sys_map_dlm_insert_e(app_sys_map_dle_t *e);

/*@brief     删除图边
 *@param[in] e 图边实例
 */
void app_sys_map_dlm_remove_e(app_sys_map_dle_t *e);

/*@brief     访问图所有顶点
 *paramm[in] m 图实例
 */
void app_sys_map_dlm_visit_all(app_sys_map_dlm_t *m);

/*@brief  获得图顶点实例索引
 *@param[in] m 图实例
 *@param[in] v 图顶点实例
 *@retval 图顶点实例索引
 */
uint32_t app_sys_map_dlm_idx_from_v(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v);

/*@brief  获得图顶点实例
 *@param[in] m   图实例
 *@param[in] idx 图顶点实例索引
 *@retval 图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_idx_to_v(app_sys_map_dlm_t *m, uint32_t idx);

/*@brief     获得图顶点实例数量
 *@param[in] m 图实例
 *@retval    图顶点实例数量
 */
uint32_t app_sys_map_dlv_num(app_sys_map_dlm_t *m);

/* 基本图论扩展(参数太多了,^_^打包) */

typedef struct {
    uint32_t v_cnt;             // 图顶点数量
    bool               *record; // 辅助访问记录数组:[v_cnt]
    app_sys_map_dlv_t **stack;  // 辅助栈:[v_cnt]
    app_sys_map_dlv_t **queue;  // 辅助队列:[v_cnt]
    app_sys_map_dlv_t **v_path; // 最短路径记录表:[v_cnt][v_cnt]
    uint32_t          **w_path; // 最短路径记录表:[v_cnt][v_cnt]
    uint32_t (*e_w_cb)(app_sys_map_dle_t *e);
    uint32_t   e_w_max;     // 最大权值
    uint32_t  *e_w_ary;     // 最短权值记录数组:[v_cnt]
    uint32_t  *e_w_mat;     // 权值矩阵:[v_cnt][v_cnt]
    uint32_t  *e_w_mat_t;   // 辅助权值矩阵:[v_cnt][v_cnt]
    uint32_t  *set_s;       // 辅助并查集:[v_cnt]
    uint32_t  *set_p;       // 辅助并查集:[v_cnt]
} app_sys_map_dlm_args_t;

/*@brief     深度优先搜索
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .stack, .record)
 */
bool app_sys_map_dlm_first_d(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v, app_sys_map_dlm_args_t *args);

/*@brief     广度优先搜索
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .queue, .record)
 */
bool app_sys_map_dlm_first_b(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v, app_sys_map_dlm_args_t *args);

/*@brief     迪杰斯特拉最短路径搜索
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .v_path, .record, .e_w_ary, .e_w_max, .e_w_cb)
 */
void app_sys_map_dlm_dijkstra(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v, app_sys_map_dlm_args_t *args);

/*@brief     贝尔曼弗德最短路径搜索
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .v_path, .e_w_ary, .e_w_max, .e_w_cb)
 */
bool app_sys_map_dlm_bellmanford(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v, app_sys_map_dlm_args_t *args);

/*@brief     弗洛伊德最短路径搜索
 *@param[in] m 图实例
 *@param[in] args(.v_cnt, .w_path, .e_w_max, .e_w_mat, .e_w_cb)
 */
void app_sys_map_dlm_floyd(app_sys_map_dlm_t *m, app_sys_map_dlm_args_t *args);

/*@brief     普利姆最小生成树
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .record, .e_w_max, .e_w_mat_t, .e_w_mat, .e_w_cb)
 */
void app_sys_map_dlm_prim(app_sys_map_dlm_t *m, app_sys_map_dlm_args_t *args);

/*@brief     凯斯特拉最小生成树
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .record, .e_w_max, .e_w_mat_t, .e_w_mat, .set, .set_parent, .e_w_cb)
 */
void app_sys_map_dlm_kruskal(app_sys_map_dlm_t *m, app_sys_map_dlm_args_t *args);

#endif
