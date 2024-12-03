/*实现目标:
 *    monkey test
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static struct {
    scui_indev_type_t type;
    scui_indev_state_t state;
    
    uint8_t tick;
    bool idle;
    uint8_t count;
    
    scui_point_t ptr_pos;
    scui_opt_dir_t ptr_move_way;
    scui_coord_t ptr_move_step;
    bool ptr_move;
    
    scui_coord_t key_id;
    scui_coord_t key_val;
    
} * scui_ui_res_local = NULL;

/*@brief monkey test动画回调
 */
void scui_monkey_anim_prepare(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief monkey test动画回调
 */
void scui_monkey_anim_execute(void *instance)
{
    SCUI_LOG_INFO("");
    
    // 在空闲态选择一个输入设备
    if (scui_ui_res_local->idle) {
        scui_ui_res_local->idle = false;
        scui_ui_res_local->type = scui_rand(9) < 7 ? 0 : scui_rand(9) < 5 ? 1 : 2;
        scui_ui_res_local->type += 1;
    } else {
        scui_indev_data_t indev_data = {0};
        indev_data.type = scui_ui_res_local->type;
        // 根据输入设备类型, 伪造数据
        switch (scui_ui_res_local->type) {
        case scui_indev_type_ptr: {
            
            if (scui_ui_res_local->state == scui_indev_state_release) {
                scui_ui_res_local->state  = scui_indev_state_press;
                //
                scui_ui_res_local->tick = scui_rand(20) + 2;
                scui_ui_res_local->count = 0;
                //
                scui_ui_res_local->ptr_pos.x = SCUI_DRV_HOR_RES / 2;
                scui_ui_res_local->ptr_pos.y = SCUI_DRV_VER_RES / 2;
                scui_ui_res_local->ptr_move_step = scui_rand(75);
                scui_ui_res_local->ptr_move_way = 1 << scui_rand(3);
                scui_ui_res_local->ptr_move = scui_rand(1);
            } else {
                if (scui_ui_res_local->ptr_move) {
                    scui_coord_t step = scui_ui_res_local->ptr_move_step;
                    switch (scui_ui_res_local->ptr_move_way) {
                    case scui_opt_dir_to_u: scui_ui_res_local->ptr_pos.y -= step; break;
                    case scui_opt_dir_to_d: scui_ui_res_local->ptr_pos.y += step; break;
                    case scui_opt_dir_to_l: scui_ui_res_local->ptr_pos.x -= step; break;
                    case scui_opt_dir_to_r: scui_ui_res_local->ptr_pos.x += step; break;
                    default: SCUI_ASSERT(false);
                    }
                    /* 整理坐标范围 */
                    if (scui_ui_res_local->ptr_pos.x < 0)
                        scui_ui_res_local->ptr_pos.x = 0;
                    if (scui_ui_res_local->ptr_pos.y < 0)
                        scui_ui_res_local->ptr_pos.y = 0;
                    if (scui_ui_res_local->ptr_pos.x > SCUI_DRV_HOR_RES - 1)
                        scui_ui_res_local->ptr_pos.x = SCUI_DRV_HOR_RES - 1;
                    if (scui_ui_res_local->ptr_pos.y > SCUI_DRV_VER_RES - 1)
                        scui_ui_res_local->ptr_pos.y = SCUI_DRV_VER_RES - 1;
                }
                
                scui_ui_res_local->count++;
                if (scui_ui_res_local->tick < scui_ui_res_local->count) {
                    scui_ui_res_local->idle = true;
                    scui_ui_res_local->state = scui_indev_state_release;
                }
                
                indev_data.state = scui_ui_res_local->state;
                indev_data.ptr.ptr_pos = scui_ui_res_local->ptr_pos;
            }
            break;
        }
        case scui_indev_type_enc: {
            indev_data.state = scui_indev_state_release;
            indev_data.enc.enc_diff = scui_rand(1) ? -1 : +1;
            // 编码器是瞬时事件
            scui_ui_res_local->idle = true;
            scui_ui_res_local->state = scui_indev_state_release;
            break;
        }
        case scui_indev_type_key: {
            
            if (scui_ui_res_local->state == scui_indev_state_release) {
                scui_ui_res_local->state  = scui_indev_state_press;
                //
                scui_ui_res_local->key_id  = scui_event_key_val_enter;
                scui_ui_res_local->key_val = 0;
                scui_ui_res_local->tick = scui_rand(20) + 2;
                scui_ui_res_local->count = 0;
            } else {
                
                scui_ui_res_local->count++;
                if (scui_ui_res_local->tick < scui_ui_res_local->count) {
                    scui_ui_res_local->idle = true;
                    scui_ui_res_local->state = scui_indev_state_release;
                }
                
                indev_data.state = scui_ui_res_local->state;
                indev_data.key.key_id = scui_ui_res_local->key_id;
                indev_data.key.key_val = scui_ui_res_local->key_val;
            }
            break;
        }
        default:
            SCUI_ASSERT(false);
        }
        
        /* 事件派发 */
        scui_indev_notify(&indev_data);
    }
}

/*@brief monkey test动画回调
 */
void scui_monkey_anim_finish(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief monkey test
 */
void scui_monkey_test(void)
{
    SCUI_ASSERT(scui_ui_res_local == NULL);
    scui_ui_res_local = SCUI_MEM_ALLOC(scui_mem_type_user, sizeof(*scui_ui_res_local));
    memset(scui_ui_res_local, 0, sizeof(*scui_ui_res_local));
    scui_ui_res_local->idle = true;
    
    scui_anima_t anima = {0};
    anima.prepare = scui_monkey_anim_prepare;
    anima.expired = scui_monkey_anim_execute;
    anima.finish  = scui_monkey_anim_finish;
    anima.reload  = SCUI_ANIMA_INFINITE;
    anima.peroid  = SCUI_ANIMA_TICK;
    
    scui_handle_t scui_monkey_test_anima = SCUI_HANDLE_INVALID;
    scui_anima_create(&anima, &scui_monkey_test_anima);
    scui_anima_start(scui_monkey_test_anima);
}
