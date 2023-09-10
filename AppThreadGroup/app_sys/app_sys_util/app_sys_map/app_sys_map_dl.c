/*实现目标:
 *    泛型图(十字链表,重邻接表)
 *    链表容器扩容的图
 *    无向图==双向权值一致的有向图
 *    从当前地址获得它的所有者地址(编译时解析)
 *    使用_offset或者container_of或者_owner
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

/*@brief     图顶点重置
 *@param[in] v 图顶点实例
 */
void app_sys_map_dlv_reset(app_sys_map_dlv_t *v)
{
    app_sys_list_dln_reset(&v->buddy);
    app_sys_list_dll_reset(&v->e_set_i);
    app_sys_list_dll_reset(&v->e_set_o);
}

/*@brief     图边重置
 *@param[in] e 图边实例
 */
void app_sys_map_dle_reset(app_sys_map_dle_t *e)
{
    e->v_i = NULL;
    e->v_o = NULL;
    app_sys_list_dln_reset(&e->e_item_i);
    app_sys_list_dln_reset(&e->e_item_o);
}

/*@brief     图重置
 *@param[in] m 图实例
 */
void app_sys_map_dlm_reset(app_sys_map_dlm_t *m)
{
    m->confirm = NULL;
    m->visit   = NULL;
    app_sys_list_dll_reset(&m->set);
}

/*@brief     图边重置
 *@param[in] e   图边实例
 *@param[in] v_i 图顶点实例
 *@param[in] v_o 图顶点实例
 */
void app_sys_map_dle_config(app_sys_map_dle_t *e, app_sys_map_dlv_t *v_i, app_sys_map_dlv_t *v_o)
{
    e->v_i = v_i;
    e->v_o = v_o;
    app_sys_list_dln_reset(&e->e_item_i);
    app_sys_list_dln_reset(&e->e_item_o);
}

/*@brief     图重置
 *@param[in] m       图实例
 *@param[in] confirm 图比较函数
 *@param[in] visit   图访问函数
 */
void app_sys_map_dlm_config(app_sys_map_dlm_t *m, app_sys_map_dlm_fc_t confirm, app_sys_map_dlm_fv_t visit)
{
    m->confirm = confirm;
    m->visit   = visit;
}

/*@brief     从图边获得图顶点
 *@param[in] e 图边实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dle_v_i(app_sys_map_dle_t *e)
{
    return e->v_i;
}

/*@brief     从图边获得图顶点
 *@param[in] e 图边实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dle_v_o(app_sys_map_dle_t *e)
{
    return e->v_o;
}

/*@brief     获得图起始顶点
 *@param[in] m 图实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_head_v(app_sys_map_dlm_t *m)
{
    app_sys_list_dln_t *node = app_sys_list_dll_head(&m->set);
    return node == NULL ? NULL : app_sys_own_ofs(app_sys_map_dlv_t, buddy, node);
}

/*@brief     获得图末尾顶点
 *@param[in] m 图实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_tail_v(app_sys_map_dlm_t *m)
{
    app_sys_list_dln_t *node = app_sys_list_dll_tail(&m->set);
    return node == NULL ? NULL : app_sys_own_ofs(app_sys_map_dlv_t, buddy, node);
}

/*@brief     获得图顶点前一顶点
 *@param[in] m 图实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_prev_v(app_sys_map_dlv_t *v)
{
    app_sys_list_dln_t *node = app_sys_list_dln_prev(&v->buddy);
    return node == NULL ? NULL : app_sys_own_ofs(app_sys_map_dlv_t, buddy, node);
}

/*@brief     获得图顶点后一顶点
 *@param[in] m 图实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_next_v(app_sys_map_dlv_t *v)
{
    app_sys_list_dln_t *node = app_sys_list_dln_next(&v->buddy);
    return node == NULL ? NULL : app_sys_own_ofs(app_sys_map_dlv_t, buddy, node);
}

/*@brief     获得图顶点起始边
 *@param[in] v 图顶点实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_head_e(app_sys_map_dlv_t *v)
{
    app_sys_list_dln_t *node = app_sys_list_dll_head(&v->e_set_o);
    return node == NULL ? NULL : app_sys_own_ofs(app_sys_map_dle_t, e_item_o, node);
}

/*@brief     获得图顶点末尾边
 *@param[in] v 图顶点实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_tail_e(app_sys_map_dlv_t *v)
{
    app_sys_list_dln_t *node = app_sys_list_dll_tail(&v->e_set_o);
    return node == NULL ? NULL : app_sys_own_ofs(app_sys_map_dle_t, e_item_o, node);
}

/*@brief     获得图边前一边
 *@param[in] e 图边实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_prev_e(app_sys_map_dle_t *e)
{
    app_sys_list_dln_t *node = app_sys_list_dln_prev(&e->e_item_o);
    return node == NULL ? NULL : app_sys_own_ofs(app_sys_map_dle_t, e_item_o, node);
}

/*@brief     获得图边后一边
 *@param[in] e 图边实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_next_e(app_sys_map_dle_t *e)
{
    app_sys_list_dln_t *node = app_sys_list_dln_next(&e->e_item_o);
    return node == NULL ? NULL : app_sys_own_ofs(app_sys_map_dle_t, e_item_o, node);
}

/*@brief     搜索图顶点
 *@param[in] m 图实例
 *@param[in] v 图顶点实例
 *@retval    图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_search_v(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v)
{
    /* 对称语义,二选其一 */
    // app_sys_list_dll_ftra(&m->set, curr)
       app_sys_list_dll_btra(&m->set, curr) {
        app_sys_map_dlv_t *target = app_sys_own_ofs(app_sys_map_dlv_t, buddy, curr);
        if (m->confirm(target, v))
            return target;
    }
    return NULL;
}

/*@brief     插入图顶点
 *@param[in] m 图实例
 *@param[in] v 图顶点实例
 */
void app_sys_map_dlm_insert_v(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v)
{
    /* 对称语义,二选其一 */
    // app_sys_list_dll_pinsert(&m->set, NULL, &v->buddy);
       app_sys_list_dll_ainsert(&m->set, NULL, &v->buddy);
}

/*@brief     删除图顶点
 *@param[in] m 图实例
 *@param[in] v 图顶点实例
 */
void app_sys_map_dlm_remove_v(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v)
{
    app_sys_list_dll_remove(&m->set, &v->buddy);
}

/*@brief     搜索图边
 *@param[in] v_i 图顶点实例
 *@param[in] v_o 图顶点实例
 *@retval    图边实例
 */
app_sys_map_dle_t * app_sys_map_dlm_search_e(app_sys_map_dlv_t *v_i, app_sys_map_dlv_t *v_o)
{
    /* 对称语义,二选其一 */
    
    /* 这是从v_i的出度寻找的方式 */
    app_sys_list_dll_ftra(&v_i->e_set_o, curr) {
        app_sys_map_dle_t *target = app_sys_own_ofs(app_sys_map_dle_t, e_item_o, curr);
        if (target->v_o == v_o)
            return target;
    }
    return NULL;
    /* 这是从v_o的入度寻找的方式 */
    app_sys_list_dll_ftra(&v_o->e_set_i, curr) {
        app_sys_map_dle_t *target = app_sys_own_ofs(app_sys_map_dle_t, e_item_i, curr);
        if (target->v_i == v_i)
            return target;
    }
    return NULL;
}

/*@brief     插入图边
 *@param[in] e 图边实例
 */
void app_sys_map_dlm_insert_e(app_sys_map_dle_t *e)
{
    /* 对称语义,二选其一 */
    if (e->v_i == e->v_o)
        return;
    /* 向弧添加(发起者的出度:尾部追加;接收者的入度:首项插入) */
    app_sys_list_dll_ainsert(&e->v_i->e_set_o, NULL, &e->e_item_o);
    app_sys_list_dll_pinsert(&e->v_o->e_set_i, NULL, &e->e_item_i);
    /* 向弧添加(发起者的出度:首项插入;接收者的入度:尾部追加) */
    // app_sys_list_dll_pinsert(&e->v_i->e_set_o, NULL, &e->e_item_o);
    // app_sys_list_dll_ainsert(&e->v_o->e_set_i, NULL, &e->e_item_i);
}

/*@brief     删除图边
 *@param[in] e 图顶点实例
 */
void app_sys_map_dlm_remove_e(app_sys_map_dle_t *e)
{
    /* 向弧的发起者移除出度 */
    /* 向弧的接受者移除入度 */
    app_sys_list_dll_remove(&e->v_i->e_set_o, &e->e_item_o);
    app_sys_list_dll_remove(&e->v_o->e_set_i, &e->e_item_i);
}

/*@brief     访问图所有顶点
 *paramm[in] m 图实例
 */
void app_sys_map_dlm_visit_all(app_sys_map_dlm_t *m)
{
    /* 遍历检查顶点 */
    
    /* 对称语义,二选其一 */
    // app_sys_list_dll_ftra(&m->set, cur0)
       app_sys_list_dll_btra(&m->set, cur0) {
       app_sys_map_dlv_t *v = app_sys_own_ofs(app_sys_map_dlv_t, buddy, cur0);
       m->visit(v, NULL, "v:");
       m->visit(NULL, NULL, app_sys_log_line());
       /* 遍历检查顶点出度 */
       m->visit(NULL, NULL, "e_set_o:");
       m->visit(NULL, NULL, app_sys_log_line());
       /* 对称语义,二选其一 */
       // app_sys_list_dll_ftra(&v->e_set_o, cur1)
          app_sys_list_dll_btra(&v->e_set_o, cur1) {
          app_sys_map_dle_t *e = app_sys_own_ofs(app_sys_map_dle_t, e_item_o, cur1);
          m->visit(NULL, e, "\te:");
          m->visit(e->v_i, NULL, "\tv_i:");
          m->visit(e->v_o, NULL, "\tv_o:");
          m->visit(NULL, NULL, app_sys_log_line());
       }
       /* 遍历检查顶点入度 */
       m->visit(NULL, NULL, "e_set_i:");
       m->visit(NULL, NULL, app_sys_log_line());
       /* 对称语义,二选其一 */
       // app_sys_list_dll_ftra(&v->e_set_i, cur1)
          app_sys_list_dll_btra(&v->e_set_i, cur1) {
          app_sys_map_dle_t *e = app_sys_own_ofs(app_sys_map_dle_t, e_item_i, cur1);
          m->visit(NULL, e, "\te:");
          m->visit(e->v_i, NULL, "\tv_i:");
          m->visit(e->v_o, NULL, "\tv_o:");
          m->visit(NULL, NULL, app_sys_log_line());
       }
    }
    m->visit(NULL, NULL, app_sys_log_line());
}

/*@brief  获得图顶点实例索引
 *@param[in] m 图实例
 *@param[in] v 图顶点实例
 *@retval 图顶点实例索引
 */
uint32_t app_sys_map_dlm_idx_from_v(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v)
{
    uint32_t cnt = 0;
    /* 对称语义,二选其一 */
    // app_sys_list_dll_ftra(&m->set, curr)
       app_sys_list_dll_btra(&m->set, curr)
       if (&v->buddy == curr)
           return cnt;
       else
           cnt++;
    return cnt;
}

/*@brief  获得图顶点实例
 *@param[in] m   图实例
 *@param[in] idx 图顶点实例索引
 *@retval 图顶点实例
 */
app_sys_map_dlv_t * app_sys_map_dlm_idx_to_v(app_sys_map_dlm_t *m, uint32_t idx)
{
    uint32_t cnt = 0;
    /* 对称语义,二选其一 */
    // app_sys_list_dll_ftra(&m->set, curr)
       app_sys_list_dll_btra(&m->set, curr) {
        if (cnt != idx) {
            cnt++;
            continue;
        }
        return app_sys_own_ofs(app_sys_map_dlv_t, buddy, curr);
    }
    return NULL;
}

/*@brief     获得图顶点实例数量
 *@param[in] m 图实例
 *@retval    图顶点实例数量
 */
uint32_t app_sys_map_dlv_num(app_sys_map_dlm_t *m)
{
    uint32_t cnt = 0;
    /* 对称语义,二选其一 */
    // app_sys_list_dll_ftra(&m->set, curr)
       app_sys_list_dll_btra(&m->set, curr) cnt++;
    return cnt;
}

/* 基本图论扩展 */

/*@brief     深度优先搜索
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .stack, .record)
 */
bool app_sys_map_dlm_first_d(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v, app_sys_map_dlm_args_t *args)
{
    /* 提取参数 */
    uint32_t v_cnt = args->v_cnt;
    app_sys_map_dlv_t **stack = args->stack;
    bool *record = args->record;
    
    uint32_t stack_t = 0;
    /* 首顶点入栈 */
    stack[stack_t++] = v;
    /* 栈溢出检查 */
    if (stack_t >= v_cnt)
        return false;
    /* 深度优先搜索: */
    while (stack_t != 0) {
        /* 栈顶顶点出栈 */
        app_sys_map_dlv_t *target = stack[--stack_t];
        /* 检查该顶点是否被访问 */
        uint32_t idx = app_sys_map_dlm_idx_from_v(m, target);
        /* 如果该顶点没有被访问,访问它,并添加访问记录 */
        if (record[idx] == false) {
            record[idx]  = true;
            m->visit(target, NULL, NULL);
        }
        /* 遍历该顶点所有边并为其添加,同时过滤掉已经访问过的点 */
        /* 对称语义,二选其一 */
        // app_sys_list_dll_ftra(&target->e_set_o, curr)
           app_sys_list_dll_btra(&target->e_set_o, curr) {
            app_sys_map_dle_t *e = app_sys_own_ofs(app_sys_map_dle_t, e_item_o, curr);
            app_sys_map_dlv_t *tmp = app_sys_map_dle_v_o(e);
            /* 如果该顶点没有被访问,将其入栈 */
            if (record[app_sys_map_dlm_idx_from_v(m, tmp)] == false) {
                stack[stack_t++] = tmp;
                /* 栈溢出检查 */
                if (stack_t >= v_cnt)
                    return false;
            }
        }
    }
    m->visit(NULL, NULL, app_sys_log_line());
    return true;
}

/*@brief     广度优先搜索
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .queue, .record)
 */
bool app_sys_map_dlm_first_b(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v, app_sys_map_dlm_args_t *args)
{
    /* 提取参数 */
    uint32_t v_cnt = args->v_cnt;
    app_sys_map_dlv_t **queue = args->queue;
    bool *record = args->record;
    
    uint32_t queue_h = 0;
    uint32_t queue_t = 0;
    /* 首顶点队列 */
    queue[queue_t++ % v_cnt] = v;
    /* 广度优先搜索: */
    while (queue_h < queue_t) {
        /* 首向顶点出队列 */
        app_sys_map_dlv_t *target = queue[queue_h++ % v_cnt];
        /* 检查该顶点是否被访问 */
        uint32_t idx = app_sys_map_dlm_idx_from_v(m, target);
        /* 如果该顶点没有被访问,访问它,并添加访问记录 */
        if (record[idx] == false) {
            record[idx]  = true;
            m->visit(target, NULL, NULL);
        }
        /* 遍历该顶点所有边并为其添加,同时过滤掉已经访问过的点 */
        /* 对称语义,二选其一 */
        // app_sys_list_dll_ftra(&target->e_set_o, curr)
           app_sys_list_dll_btra(&target->e_set_o, curr) {
            app_sys_map_dle_t *e = app_sys_own_ofs(app_sys_map_dle_t, e_item_o, curr);
            app_sys_map_dlv_t *tmp = app_sys_map_dle_v_o(e);
            /* 如果该顶点没有被访问,将其入队列 */
            if (record[app_sys_map_dlm_idx_from_v(m, tmp)] == false) {
                queue[queue_t++ % v_cnt] = tmp;
                /* 队列小检查 */
                if (queue_t - queue_h > v_cnt)
                    return false;
            }
        }
    }
    m->visit(NULL, NULL, app_sys_log_line());
    return true;
}

/*@brief     迪杰斯特拉最短路径搜索
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .v_path, .record, .e_w_ary, .e_w_max, .e_w_cb)
 */
void app_sys_map_dlm_dijkstra(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v, app_sys_map_dlm_args_t *args)
{
    /* 提取参数 */
    uint32_t v_cnt = args->v_cnt;
    bool *record = args->record;
    app_sys_map_dlv_t **s_path = args->v_path;
    uint32_t  *e_w_ary = args->e_w_ary;
    uint32_t   e_w_max = args->e_w_max;
    uint32_t (*e_w_cb)(app_sys_map_dle_t *e) = args->e_w_cb;
    
    /* 初始化最短路径数值,设置为最大值 */
    for (uint32_t idx = 0; idx < v_cnt; e_w_ary[idx++] = e_w_max);
    /* 初始化最短路径情况 */
    for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
    for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
         s_path[idx1 * v_cnt + idx2] = idx2 != 0 ? NULL : v;
    /* 获得自己的虚拟坐标映射 */
    uint32_t idx = app_sys_map_dlm_idx_from_v(m, v);
    /* 配置到自己的最短路径为0 */
    e_w_ary[idx] = 0;
    /* 保存到一次确定的最小值及其它的最短路径 */
    uint32_t e_w_min = e_w_max;
    uint32_t idx_min = idx;
    /* 迪杰斯特拉最短路径搜索: */
    while (1) {
        /* 如果找到一个需要被确定但未被确定的顶点,标记它并准备确定它 */
        idx = idx_min;
        /* 未确定检查 */
        if (record[idx] == false)
            record[idx]  = true;
        else {
            /* 从记录中找一个未使用的下标 */
            uint32_t idx_unused = 0;
            for (idx_unused = 0; idx_unused < v_cnt; idx_unused++)
                if (record[idx_unused] == false)
                    break;
            /* 如果所有的下标记录都被确定,退出搜索 */
            if (idx_unused >= v_cnt)
                break;
            /* 如果寻找到了第一个未使用下标,开始记录它 */
            idx = idx_unused;
            idx_min = idx_unused;
            record[idx] = true;
        }
        v = app_sys_map_dlm_idx_to_v(m, idx);
        /* 开始确定该顶点 */
        /* 对称语义,二选其一 */
        // app_sys_list_dll_ftra(&v->e_set_o, curr)
           app_sys_list_dll_btra(&v->e_set_o, curr) {
            app_sys_map_dle_t *e = app_sys_own_ofs(app_sys_map_dle_t, e_item_o, curr);
            app_sys_map_dlv_t *tmp = app_sys_map_dle_v_o(e);
            /* 获得该边的权值 */
            uint32_t e_w_cur = e_w_cb(e);
            /* 获得目标的虚拟坐标映射 */
            uint32_t idx_cur = app_sys_map_dlm_idx_from_v(m, tmp);
            /* 更新最小值 */
            if (e_w_cur < e_w_min) {
                idx_min = idx_cur;
                e_w_min = e_w_cur;
            }
            /* 更新最短路径数组值,更新最短路径 */
            if (e_w_ary[idx] + e_w_cur < e_w_ary[idx_cur]) {
                /* 更新最短路径数组值 */
                e_w_ary[idx_cur] = e_w_ary[idx] + e_w_cur;
                /* 将v -> idx修改为v -> idx_cur -> idx */
                uint32_t nonius = 0;
                /* 无需担心路径溢出,除非成环否则不会重复 */
                while (s_path[idx * v_cnt + (++nonius)] != NULL)
                       s_path[idx_cur * v_cnt + nonius] =
                       s_path[idx * v_cnt + nonius];
                /* 新的路径加入 */
                s_path[idx_cur * v_cnt + nonius++] = tmp;
                /* 追加补充 */
                if (nonius < v_cnt)
                    s_path[idx_cur * v_cnt + nonius] = NULL;
            }
        }
    }
}

/*@brief     贝尔曼弗德最短路径搜索
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .v_path, .e_w_ary, .e_w_max, .e_w_cb)
 */
bool app_sys_map_dlm_bellmanford(app_sys_map_dlm_t *m, app_sys_map_dlv_t *v, app_sys_map_dlm_args_t *args)
{
    /* 提取参数 */
    uint32_t v_cnt = args->v_cnt;
    app_sys_map_dlv_t **s_path = args->v_path;
    uint32_t *e_w_ary = args->e_w_ary;
    uint32_t  e_w_max = args->e_w_max;
    uint32_t (*e_w_cb)(app_sys_map_dle_t *e) = args->e_w_cb;
    
    /* 初始化最短路径数值,设置为最大值 */
    for (uint32_t idx = 0; idx < v_cnt; e_w_ary[idx++] = e_w_max);
    /* 初始化最短路径情况 */
    for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
    for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
         s_path[idx1 * v_cnt + idx2] = idx2 != 0 ? NULL : v;
    /* 获得自己的虚拟坐标映射,配置到自己的最短路径为0 */
    e_w_ary[app_sys_map_dlm_idx_from_v(m, v)] = 0;
    /* 贝尔曼弗德最短路径搜索: */
    for (uint32_t idx = 0; idx < v_cnt; idx++) {
        bool over_upload = true;
        /* 对每一条存在的边进行搜索 */
        for (uint32_t idx_f = 0; idx_f < v_cnt; idx_f++) {
            /* 获得目标顶点 */
            app_sys_map_dlv_t *v = app_sys_map_dlm_idx_to_v(m, idx_f);
            /* 检查目标顶点是否存在边 */
            if (app_sys_map_dlm_head_e(v) == NULL)
                continue;
            /* 从已知顶点出发 */
            if (e_w_ary[idx_f] == e_w_max)
                continue;
            /* 从idx_f到idx_t,遍历检查 */
            /* 对称语义,二选其一 */
            // app_sys_list_dll_ftra(&v->e_set_o, curr)
               app_sys_list_dll_btra(&v->e_set_o, curr) {
                app_sys_map_dle_t *e = app_sys_own_ofs(app_sys_map_dle_t, e_item_o, curr);
                app_sys_map_dlv_t *tmp = app_sys_map_dle_v_o(e);
                /* 获得该边的权值 */
                uint32_t e_w_cur = e_w_cb(e);
                /* 确定从顶点vFrom到vTo */
                uint32_t idx_t = app_sys_map_dlm_idx_from_v(m, tmp);
                /* 更新最短路径数组值,更新最短路径 */
                if (e_w_ary[idx_f] + e_w_cur < e_w_ary[idx_t]) {
                    /* 更新最短路径数组值 */
                    e_w_ary[idx_t] = e_w_ary[idx_f] + e_w_cur;
                    /* 标记当前段未更新完毕 */
                    over_upload = false;
                    /* 将v -> idx_t修改为v -> idx_f -> idx_t */
                    uint32_t nonius = 0;
                    /* 无需担心路径溢出,除非成环否则不会重复 */
                    while (s_path[idx_f * v_cnt + (++nonius)] != NULL)
                           s_path[idx_t * v_cnt + nonius] =
                           s_path[idx_f * v_cnt + nonius];
                    /* 新的路径加入 */
                    s_path[idx_t * v_cnt + nonius++] = tmp;
                    /* 追加补充 */
                    if (nonius < v_cnt)
                        s_path[idx_t * v_cnt + nonius] = NULL;
                }
            }
            if (over_upload == true)
                break;
        }
    }
    /* 是否成环检查 */
    for (uint32_t idx_f = 0; idx_f < v_cnt; idx_f++) {
        /* 获得目标顶点 */
        app_sys_map_dlv_t *v = app_sys_map_dlm_idx_to_v(m, idx_f);
        /* 检查目标顶点是否存在边 */
        if (app_sys_map_dlm_head_e(v) == NULL)
            continue;
        /* 从idx_f到idx_t,遍历检查 */
        /* 对称语义,二选其一 */
        // app_sys_list_dll_ftra(&v->e_set_o, curr)
           app_sys_list_dll_btra(&v->e_set_o, curr) {
            app_sys_map_dle_t *e = app_sys_own_ofs(app_sys_map_dle_t, e_item_o, curr);
            app_sys_map_dlv_t *tmp = app_sys_map_dle_v_o(e);
            /* 获得该边的权值 */
            uint32_t e_w_cur = e_w_cb(e);
            /* 确定从顶点vFrom到vTo */
            uint32_t idx_t = app_sys_map_dlm_idx_from_v(m, tmp);
            /* 更新最短路径数组值,更新最短路径 */
            if (e_w_ary[idx_f] + e_w_cur < e_w_ary[idx_t])
                return false;
        }
    }
    return true;
}

/*@brief     弗洛伊德最短路径搜索
 *@param[in] m 图实例
 *@param[in] args(.v_cnt, .w_path, .e_w_max, .e_w_mat, .e_w_cb)
 */
void app_sys_map_dlm_floyd(app_sys_map_dlm_t *m, app_sys_map_dlm_args_t *args)
{
    /* 提取参数 */
    uint32_t   v_cnt   = args->v_cnt;
    uint32_t  *s_path  = args->w_path;
    uint32_t  *e_w_mat = args->e_w_mat;
    uint32_t   e_w_max = args->e_w_max;
    uint32_t (*e_w_cb)(app_sys_map_dle_t *e) = args->e_w_cb;
    
    /* 构建floyd权值邻接矩阵 */
    for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
    for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++) {
        /* 记录最短路径情况 */
        s_path[idx1 * v_cnt + idx2] = idx2;
        /* 过滤到自己的点 */
        if (idx1 == idx2) {
            e_w_mat[idx1 * v_cnt + idx2] = 0;
            continue;
        }
        /* 先获得顶点 */
        app_sys_map_dlv_t *v1 = app_sys_map_dlm_idx_to_v(m, idx1);
        app_sys_map_dlv_t *v2 = app_sys_map_dlm_idx_to_v(m, idx2);
        /* 再获得顶点关联边 */
        app_sys_map_dle_t *e = app_sys_map_dlm_search_e(v1, v2);
        /* 如果边不存在, 跳过它 */
        if (e == NULL)
            e_w_mat[idx1 * v_cnt + idx2] = e_w_max;
        /* 如果边存在, 获取其权值写入 */
        if (e != NULL)
            e_w_mat[idx1 * v_cnt + idx2] = e_w_cb(e);
    }
    /* 弗洛伊德最短路径搜索: */
    for (uint32_t idx0 = 0; idx0 < v_cnt; idx0++)
    for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
        if (idx0 == idx1)
            continue;
    for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++) {
        if (idx0 == idx2 || idx1 == idx2)
            continue;
        /* 获得权值 */
        uint32_t weight10 = e_w_mat[idx1 * v_cnt + idx0];
        uint32_t weight02 = e_w_mat[idx0 * v_cnt + idx2];
        /* 如果俩条边之和小于第三条边 */
        if (e_w_mat[idx1 * v_cnt + idx2] > weight10 + weight02) {
            e_w_mat[idx1 * v_cnt + idx2] = weight10 + weight02;
            s_path[idx1 * v_cnt + idx2] = 
            s_path[idx1 * v_cnt + idx0];
        }
    }
    }
}

/*@brief     普利姆最小生成树
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .record, .e_w_max, .e_w_mat_t, .e_w_mat, .e_w_cb)
 */
void app_sys_map_dlm_prim(app_sys_map_dlm_t *m, app_sys_map_dlm_args_t *args)
{
    /* 提取参数 */
    uint32_t v_cnt = args->v_cnt;
    bool *record = args->record;
    uint32_t   e_w_max = args->e_w_max;
    uint32_t  *e_w_mat_t = args->e_w_mat_t;
    uint32_t  *e_w_mat = args->e_w_mat;
    uint32_t (*e_w_cb)(app_sys_map_dle_t *e) = args->e_w_cb;
    
    for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
    for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
        e_w_mat_t[idx1 * v_cnt + idx2] = e_w_max;
    /* 构建prim权值邻接矩阵(这里采用俩边之间最小权值作为无向图的值) */
    for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
    for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++) {
        /* 过滤到自己的点 */
        if (idx1 == idx2)
            continue;
        /* 先获得顶点 */
        app_sys_map_dlv_t *v1 = app_sys_map_dlm_idx_to_v(m, idx1);
        app_sys_map_dlv_t *v2 = app_sys_map_dlm_idx_to_v(m, idx2);
        /* 再获得顶点关联边 */
        app_sys_map_dle_t *e = app_sys_map_dlm_search_e(v1, v2);
        /* 如果边不存在, 跳过它 */
        if (e == NULL)
            continue;
        /* 如果边存在, 获取其权值 */
        uint32_t e_w_cur = e_w_cb(e);
        /* 如果边存在, 写入其权值 */
        if (e_w_mat_t[idx1 * v_cnt + idx2] > e_w_cur &&
            e_w_mat_t[idx2 * v_cnt + idx1] > e_w_cur) {
            e_w_mat_t[idx1 * v_cnt + idx2] = e_w_cur;
            e_w_mat_t[idx2 * v_cnt + idx1] = e_w_cur;
        }
    }
    /* 初始化状态矩阵 */
    for (uint32_t idx = 0; idx < v_cnt; record[idx++] = false);
    /* 先随机选择一个顶点 */
    record[rand() % v_cnt] = true;
    uint32_t cur_cnt = 1;
    /* 普利姆最小生成树: */
    while (cur_cnt < v_cnt) {
        uint32_t e_w_min = e_w_max;
        uint32_t idx_i = v_cnt;
        uint32_t idx_j = v_cnt;
        /* 从集合中寻找一个顶点,从集合外寻找一个顶点 */
        for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++) {
            /* 从集合中寻找一个顶点 */
            if (record[idx1] == false)
                continue;
        for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++) {
            /* 从集合外寻找一个顶点 */
            if (record[idx2] == true)
                continue;
            /* 如果俩个顶点不存在关系 */
            if (e_w_mat_t[idx1 * v_cnt + idx2] == e_w_max)
                continue;
            /* 检查这俩个顶点是否可用 */
            if (e_w_min > e_w_mat_t[idx1 * v_cnt + idx2]) {
                e_w_min = e_w_mat_t[idx1 * v_cnt + idx2];
                idx_i = idx1;
                idx_j = idx2;
            }
        }
        }
        /* 如果寻找到集合内与集合外的最小权值边,导入该关系 */
        if (idx_i != v_cnt && idx_j != v_cnt) {
            e_w_mat[idx_i * v_cnt + idx_j] = e_w_min;
            e_w_mat[idx_j * v_cnt + idx_i] = e_w_min;
            record[idx_j] = true;
            cur_cnt++;
        }
    }
}

/*@brief     凯斯特拉最小生成树
 *@param[in] m 图实例
 *@param[in] v 起始图顶点实例
 *@param[in] args(.v_cnt, .record, .e_w_max, .e_w_mat_t, .e_w_mat, .set_s, .set_p, .e_w_cb)
 */
void app_sys_map_dlm_kruskal(app_sys_map_dlm_t *m, app_sys_map_dlm_args_t *args)
{
    /* 提取参数 */
    uint32_t v_cnt = args->v_cnt;
    bool *record = args->record;
    uint32_t  e_w_max = args->e_w_max;
    uint32_t *e_w_mat_t = args->e_w_mat_t;
    uint32_t *e_w_mat = args->e_w_mat;
    uint32_t *set_s = args->set_s;
    uint32_t *set_p = args->set_p;
    uint32_t (*e_w_cb)(app_sys_map_dle_t *e) = args->e_w_cb;
    
    for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
    for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
        e_w_mat_t[idx1 * v_cnt + idx2] = e_w_max;
    /* 构建kruskal权值邻接矩阵(这里采用俩边之间最小权值作为无向图的值) */
    for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
    for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++) {
        /* 过滤到自己的点 */
        if (idx1 == idx2)
            continue;
        /* 先获得顶点 */
        app_sys_map_dlv_t *v1 = app_sys_map_dlm_idx_to_v(m, idx1);
        app_sys_map_dlv_t *v2 = app_sys_map_dlm_idx_to_v(m, idx2);
        /* 再获得顶点关联边 */
        app_sys_map_dle_t *e = app_sys_map_dlm_search_e(v1, v2);
        /* 如果边不存在, 跳过它 */
        if (e == NULL)
            continue;
        /* 如果边存在, 获取其权值 */
        uint32_t e_w_cur = e_w_cb(e);
        /* 如果边存在, 写入其权值 */
        if (e_w_mat_t[idx1 * v_cnt + idx2] > e_w_cur &&
            e_w_mat_t[idx2 * v_cnt + idx1] > e_w_cur) {
            e_w_mat_t[idx1 * v_cnt + idx2] = e_w_cur;
            e_w_mat_t[idx2 * v_cnt + idx1] = e_w_cur;
        }
    }
    /* 使用备份的邻接矩阵,因为它会被修改 */
    for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
    for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
        e_w_mat_t[idx1 * v_cnt + idx2] = e_w_mat_t[idx1 * v_cnt + idx2];
    /* 初始化并查集 */
    for (uint32_t idx = 0; idx < v_cnt; set_s[idx] = idx++);
    for (uint32_t idx = 0; idx < v_cnt; set_p[idx] = idx++);
    /* 初始化链接数 */
    uint32_t cur_link = v_cnt;
    /* 凯斯特拉最小生成树 */
    while (cur_link > 1) {
        uint32_t e_w_min = e_w_max;
        uint32_t idx_i = v_cnt;
        uint32_t idx_j = v_cnt;
        /* 先寻找一个权值最小的边 */
        for (uint32_t idx1 = 0; idx1 < v_cnt; idx1++)
        for (uint32_t idx2 = 0; idx2 < v_cnt; idx2++)
            if (e_w_min > e_w_mat_t[idx1 * v_cnt + idx2]) {
                e_w_min = e_w_mat_t[idx1 * v_cnt + idx2];
                idx_i = idx1;
                idx_j = idx2;
            }
        /* 如果没找到最小权值边 */
        if (e_w_min == e_w_max)
            break;
        /* 找到一条最小边,检查集合组是否一致 */
        uint32_t idx_i_s = set_s[idx_i];
        uint32_t idx_j_s = set_s[idx_j];
        uint32_t idx_i_p = set_p[idx_i];
        uint32_t idx_j_p = set_p[idx_j];
        /* 迭代检查idx_i,找到最终的祖先 */
        while (idx_i_s != idx_i_p) {
               idx_i_s = set_s[idx_i_p];
               idx_i_p = set_p[idx_i_p];
        }
        /* 迭代检查idx_j,找到最终的祖先 */
        while (idx_j_s != idx_j_p) {
               idx_j_s = set_s[idx_j_p];
               idx_j_p = set_p[idx_j_p];
        }
        /* 如果是不同集合,可以对其建立联系 */
        if (idx_i_s != idx_j_s) {
            /* 合并集合,让后一个集合都归为第一个集合 */
            uint32_t idx_j_s = set_s[idx_j];
            uint32_t idx_j_p = set_p[idx_j];
            /* 这里是按秩合并 */
            while (idx_i_s != idx_j_p) {
                idx_j_s = set_s[idx_j_p];
                idx_j_p = set_p[idx_j_p];
                /* 本顶点指向第一个集合的祖先 */
                set_p[idx_j_s] = idx_i_s;
            }
            set_p[idx_j_s] = idx_i_s;
            /* 获得一条关联 */
            e_w_mat[idx_i * v_cnt + idx_j] = e_w_min;
            e_w_mat[idx_j * v_cnt + idx_i] = e_w_min;
            cur_link--;
        }
        /* 移除这条操作完毕的边 */
        e_w_mat_t[idx_i * v_cnt + idx_j] = e_w_max;
    }
}
