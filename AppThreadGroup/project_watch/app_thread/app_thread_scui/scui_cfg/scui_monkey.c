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
static void scui_monkey_anima_ready(void *instance)
{
    SCUI_LOG_INFO("");
}

/*@brief monkey test动画回调
 */
static void scui_monkey_anima_expire(void *instance)
{
    SCUI_LOG_INFO("");
    
    // 在空闲态选择一个输入设备
    if (scui_ui_res_local->idle) {
        scui_ui_res_local->idle = false;
        scui_ui_res_local->type = 1 + (scui_rand(9) < 7 ? 0 : scui_rand(9) < 5 ? 1 : 2);
    } else {
        scui_indev_data_t indev_data = {0};
        indev_data.type = scui_ui_res_local->type;
        // 根据输入设备类型, 伪造数据
        switch (scui_ui_res_local->type) {
        case scui_indev_type_ptr: {
            
            if (scui_ui_res_local->state == scui_indev_state_release) {
                scui_ui_res_local->state  = scui_indev_state_press;
                //
                scui_ui_res_local->tick = scui_rand(10) + 5;
                scui_ui_res_local->count = 0;
                //
                scui_ui_res_local->ptr_pos.x = SCUI_HOR_RES / 2;
                scui_ui_res_local->ptr_pos.y = SCUI_VER_RES / 2;
                scui_ui_res_local->ptr_move_step = scui_rand(20) + 20;
                scui_ui_res_local->ptr_move_way = 1 << scui_rand(3);
                scui_ui_res_local->ptr_move = scui_rand(9) < 5 ? 1 : 0;
                
                indev_data.state = scui_ui_res_local->state;
                indev_data.ptr.ptr_pos = scui_ui_res_local->ptr_pos;
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
                    if (scui_ui_res_local->ptr_pos.x > SCUI_HOR_RES - 1)
                        scui_ui_res_local->ptr_pos.x = SCUI_HOR_RES - 1;
                    if (scui_ui_res_local->ptr_pos.y > SCUI_VER_RES - 1)
                        scui_ui_res_local->ptr_pos.y = SCUI_VER_RES - 1;
                }
                
                scui_ui_res_local->count++;
                if (scui_ui_res_local->tick < scui_ui_res_local->count) {
                    scui_ui_res_local->idle = true;
                    scui_ui_res_local->state = scui_indev_state_release;
                    scui_ui_res_local->count = 0;
                }
                
                indev_data.state = scui_ui_res_local->state;
                indev_data.ptr.ptr_pos = scui_ui_res_local->ptr_pos;
                SCUI_LOG_DEBUG("state: %d", indev_data.state);
                SCUI_LOG_DEBUG("pos_x: %d", indev_data.ptr.ptr_pos.x);
                SCUI_LOG_DEBUG("pos_y: %d", indev_data.ptr.ptr_pos.y);
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
                //只取一些特殊值并且调整权重
                uint32_t key_id = scui_event_key_val_enter;
                uint32_t key_id_rand = scui_rand(100);
                if (scui_betw_lr(key_id_rand,  0,  30)) key_id = scui_event_key_val_enter;
                if (scui_betw_lr(key_id_rand, 30,  60)) key_id = scui_event_key_val_back;
                if (scui_betw_lr(key_id_rand, 60,  70)) key_id = scui_event_key_val_up;
                if (scui_betw_lr(key_id_rand, 70,  80)) key_id = scui_event_key_val_down;
                if (scui_betw_lr(key_id_rand, 80,  90)) key_id = scui_event_key_val_left;
                if (scui_betw_lr(key_id_rand, 90, 100)) key_id = scui_event_key_val_right;
                
                scui_ui_res_local->key_id  = key_id;
                scui_ui_res_local->key_val = 0;
                scui_ui_res_local->tick = scui_rand(10) + 2;
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
static void scui_monkey_anima_finish(void *instance)
{
    SCUI_LOG_INFO("");
}
/*@brief monkey test
 */
void scui_monkey_test(void)
{
    SCUI_ASSERT(scui_ui_res_local == NULL);
    scui_ui_res_local = SCUI_MEM_ZALLOC(scui_mem_type_user, sizeof(*scui_ui_res_local));
    scui_ui_res_local->idle = true;
    
    scui_anima_t anima = {0};
    anima.ready  = scui_monkey_anima_ready;
    anima.expire = scui_monkey_anima_expire;
    anima.finish = scui_monkey_anima_finish;
    anima.reload = SCUI_ANIMA_INFINITE;
    anima.period = SCUI_ANIMA_TICK;
    
    scui_handle_t scui_monkey_test_anima = SCUI_HANDLE_INVALID;
    scui_anima_create(&anima, &scui_monkey_test_anima);
    scui_anima_start(scui_monkey_test_anima);
}
