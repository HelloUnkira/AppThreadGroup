#ifndef SCUI_LIST_LL_H
#define SCUI_LIST_LL_H

/* 转为适配app_sys_list_l接口
 */

#define scui_list_dll_t             app_sys_list_dll_t
#define scui_list_dln_t             app_sys_list_dln_t
#define scui_list_dll_reset         app_sys_list_dll_reset
#define scui_list_dln_reset         app_sys_list_dln_reset
#define scui_list_dll_head          app_sys_list_dll_head
#define scui_list_dll_tail          app_sys_list_dll_tail
#define scui_list_dln_prev          app_sys_list_dln_prev
#define scui_list_dln_next          app_sys_list_dln_next
#define scui_list_dll_pinsert       app_sys_list_dll_pinsert
#define scui_list_dll_ainsert       app_sys_list_dll_ainsert
#define scui_list_dll_remove        app_sys_list_dll_remove
#define scui_list_dll_btra          app_sys_list_dll_btra
#define scui_list_dll_ftra          app_sys_list_dll_ftra

#define scui_list_sll_t             app_sys_list_sll_t
#define scui_list_sln_t             app_sys_list_sln_t
#define scui_list_sll_reset         app_sys_list_sll_reset
#define scui_list_sln_reset         app_sys_list_sln_reset
#define scui_list_sll_head          app_sys_list_sll_head
#define scui_list_sll_tail          app_sys_list_sll_tail
#define scui_list_sln_buddy         app_sys_list_sln_buddy
#define scui_list_sll_pinsert       app_sys_list_sll_pinsert
#define scui_list_sll_ainsert       app_sys_list_sll_ainsert
#define scui_list_sll_insert        app_sys_list_sll_insert
#define scui_list_sll_remove        app_sys_list_sll_remove
#define scui_list_sll_tra           app_sys_list_sll_tra

#define scui_queue_slq_t            app_sys_queue_slq_t
#define scui_queue_sln_t            app_sys_queue_sln_t
#define scui_queue_dlq_t            app_sys_queue_dlq_t
#define scui_queue_dln_t            app_sys_queue_dln_t
#define scui_queue_slq_dequeue      app_sys_queue_slq_dequeue
#define scui_queue_slq_enqueue      app_sys_queue_slq_enqueue
#define scui_queue_dlq_dequeue      app_sys_queue_dlq_dequeue
#define scui_queue_dlq_enqueue      app_sys_queue_dlq_enqueue

#define scui_queue_slpq_t           app_sys_queue_slpq_t
#define scui_queue_slpn_t           app_sys_queue_slpn_t
#define scui_queue_dlpq_t           app_sys_queue_dlpq_t
#define scui_queue_dlpn_t           app_sys_queue_dlpn_t
#define scui_queue_slpq_dequeue     app_sys_queue_slpq_dequeue
#define scui_queue_slpq_enqueue     app_sys_queue_slpq_enqueue
#define scui_queue_dlpq_dequeue     app_sys_queue_dlpq_dequeue
#define scui_queue_dlpq_enqueue     app_sys_queue_dlpq_enqueue

#define scui_stack_sls_t            app_sys_stack_sls_t
#define scui_stack_sln_t            app_sys_stack_sln_t
#define scui_stack_dls_t            app_sys_stack_dls_t
#define scui_stack_dls_t            app_sys_stack_dls_t
#define scui_stack_sls_pop          app_sys_stack_sls_pop
#define scui_stack_sls_push         app_sys_stack_sls_push
#define scui_stack_dls_pop          app_sys_stack_dls_pop
#define scui_stack_dls_push         app_sys_stack_dls_push

#endif
