/*实现目标:
 *    窗口:xxx
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 切表盘: 编码器累计位移阈值(达到才切换一个表盘, 避免轻碰即切) */
static const scui_coord_t scui_ui_home_dial_span = 3;

static struct {
    void   *cwf_json_inst;
    bool    ptr_long_jump;
    bool    dial_jumping;           /* 已发起表盘过渡(等待过渡窗口回跳) */
} * scui_ui_res_local = NULL;

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_float_s_event_proc(scui_event_t * event)
{
    switch (event->type) {
    case scui_event_draw_graph: {
        
        scui_color_t color = {
            .color.ch.a = 0xFF,
            .color.ch.r = scui_rand(0xFF),
            .color.ch.g = scui_rand(0xFF),
            .color.ch.b = scui_rand(0xFF),
        };
        scui_widget_draw_color(event->object, NULL, color);
        break;
    }
    default:
        break;
    }
}

/*@brief 控件事件响应回调
 *@param event 事件
 */
void scui_ui_scene_home_event_proc(scui_event_t *event)
{
    switch (event->type) {
    case scui_event_create: {
        scui_window_local_res_set(event->object, sizeof(*scui_ui_res_local));
        scui_window_local_res_get(event->object, &scui_ui_res_local);
        // cwf json 测试
        /* 表盘索引与清单由 presenter 提供(切表盘重建界面时保持) */
        uint32_t cwf_idx = scui_presenter.cwf_idx_get();
        if (cwf_idx >= scui_presenter.cwf_dial_num())
            cwf_idx  = 0;
        scui_cwf_json_make(&scui_ui_res_local->cwf_json_inst, scui_presenter.cwf_dial_name(cwf_idx), event->object);
        SCUI_LOG_INFO("home-create: idx:%u/%u", cwf_idx, scui_presenter.cwf_dial_num());
        
        
        
        // xfloat 构造测试
        // 常用于华强北风格手表, 表盘界面右拉出快捷栏(属于插件,并非控件,一次只能使用一个)
        // scui_ui_scene_xfloat_create(event->object, SCUI_HOR_RES / 3, scui_opt_pos_l, scui_ui_scene_float_s_event_proc);
        // scui_ui_scene_xfloat_create(event->object, SCUI_HOR_RES / 3, scui_opt_pos_r, scui_ui_scene_float_s_event_proc);
        // scui_ui_scene_xfloat_create(event->object, SCUI_VER_RES / 3, scui_opt_pos_u, scui_ui_scene_float_s_event_proc);
        // scui_ui_scene_xfloat_create(event->object, SCUI_VER_RES / 3, scui_opt_pos_d, scui_ui_scene_float_s_event_proc);
        break;
    }
    case scui_event_destroy:
        SCUI_LOG_INFO("home-destroy");
        // cwf json 测试
        scui_cwf_json_burn(&scui_ui_res_local->cwf_json_inst);
        break;
    case scui_event_focus_get:
        break;
    case scui_event_focus_lost:
        break;
    case scui_event_ptr_click: {
        #if 1
        // 内部模糊测试使用(内部使用!!!)
        /* 问题:存在之后的窗口重绘失效当前虚化 */
        /* 暂定:锁定窗口绘制, 禁止绘制该控件树 */
        bool draw_lock = false;
        scui_window_draw_lock_get(event->object, &draw_lock);
        
        if (draw_lock) {
            scui_window_draw_lock_set(event->object, false);
            scui_widget_draw(event->object, NULL, false, 0);
        } else {
            scui_window_draw_lock_set(event->object, true);
            /* 背景窗口虚化 */
            static uint8_t mode = 0; mode++;
            scui_widget_draw(event->object, NULL, false, 0);
            // scui_widget_draw_dither(event->object, NULL, mode % 2 == 0, 0);
            scui_widget_draw_blur(event->object, NULL, mode % 2 == 0, 0, 0);
            scui_widget_refr(event->object, false);
        }
        #endif
        break;
    }
    case scui_event_ptr_down:
        scui_ui_res_local->ptr_long_jump = false;
        break;
    case scui_event_ptr_hold:
        if (event->ptr_tick > 3000) {
            if (!scui_ui_res_local->ptr_long_jump) SCUI_LOG_INFO("ptr long hold");
            scui_ui_res_local->ptr_long_jump = true;
        }
        break;
    case scui_event_ptr_up:
        if (scui_ui_res_local->ptr_long_jump) {
            scui_ui_res_local->ptr_long_jump = true;
            #if SCUI_MEM_FEAT_MINI == 0
            scui_window_stack_add_by(SCUI_UI_SCENE_LANTERN,
                scui_window_switch_circle, scui_opt_dir_none, false);
            #endif
        }
        break;
    
    #if 1
    case scui_event_enc_tick: {
        scui_event_mask_over(event);
        
        /* 表盘过渡中: 丢弃编码器
         * 窗口切换动画进行中输入独占(框架仅覆盖 ptr_move/ptr_fling, 编码器未覆盖);
         * 已发起过渡但过渡窗口尚未回跳时同理, 否则会额外加减索引并重复发起跳转 */
        if (scui_window_switch_work() || scui_ui_res_local->dial_jumping)
            break;
        
        /* 编码器累计到位才切换: 同向累加, 反向对消(跨界面重建保持) */
        static scui_coord_t dial_acc = 0;
        dial_acc += event->enc_way == 0 ? event->enc_diff : -event->enc_diff;
        if (scui_abs(dial_acc) < scui_ui_home_dial_span)
            break;
        
        /* 表盘索引: 0 <-> n 循环; 方向决定过渡窗口从哪一侧进入
         * (rtl: 新窗口在右侧, 从右进入; ltr: 新窗口在左侧, 从左进入)
         * 左右相反则对调这两个方向即可 */
        uint32_t     cwf_num = scui_presenter.cwf_dial_num();
        uint32_t     cwf_idx = scui_presenter.cwf_idx_get();
        scui_opt_dir_t dir = scui_opt_dir_none;
        if (dial_acc > 0) {
            cwf_idx += 1;
            if (cwf_idx >= cwf_num)
                cwf_idx  = 0;
            dir = scui_opt_dir_rtl;
        } else {
            if (cwf_idx == 0)
                cwf_idx  = cwf_num - 1;
            else
                cwf_idx -= 1;
            dir = scui_opt_dir_ltr;
        }
        scui_presenter.cwf_idx_set(cwf_idx);
        SCUI_LOG_INFO("home-dial-jump: idx:%u/%u dir:%d acc:%d", cwf_idx, cwf_num, dir, dial_acc);
        /* 保留超出阈值的余量, 连续转动不丢格 */
        dial_acc -= dial_acc > 0 ? scui_ui_home_dial_span : -scui_ui_home_dial_span;
        
        /* 覆盖式跳转到过渡窗口: 本窗口出栈销毁, 由过渡窗口承担切换动画
         * 过渡窗口动画结束后(focus_get)无动画切回, 本窗口重建并取用新索引 */
        if (scui_window_stack_cover_by(SCUI_UI_SCENE_HOME_SW, scui_window_switch_center_in, dir))
            scui_ui_res_local->dial_jumping = true;
        break;
    }
    #endif
    
    case scui_event_key_click: {
        if (event->key_id == scui_event_key_val_esc) {
            /* 特殊键(Esc): 切换窗口切换动画类型 */
            scui_window_switch_type_t jump_type = 0;
            scui_window_switch_type_t move_type = 0;
            scui_window_switch_get_jump_type(&jump_type);
            scui_window_switch_get_move_type(&move_type);
            jump_type++;
            move_type++;
            
            if (jump_type <= scui_window_switch_single_s)
                jump_type  = scui_window_switch_single_e - 1;
            if (jump_type >= scui_window_switch_single_e)
                jump_type  = scui_window_switch_single_s + 1;
            
            if (move_type <= scui_window_switch_single_s)
                move_type  = scui_window_switch_single_e - 1;
            if (move_type >= scui_window_switch_single_e)
                move_type  = scui_window_switch_single_s + 1;
            
            scui_window_switch_set_jump_type(jump_type);
            scui_window_switch_set_move_type(move_type);
            
            scui_event_mask_over(event);
            break;
        }
        
        if (event->key_id == scui_event_key_val_enter) {
            static scui_ui_scene_list_type_t type = 0; type++;
            if (type < scui_ui_scene_list_type_s + 1)
                type = scui_ui_scene_list_type_e - 1;
            if (type > scui_ui_scene_list_type_e - 1)
                type = scui_ui_scene_list_type_s + 1;
            
            switch (type) {
            #if SCUI_MEM_FEAT_MINI == 0
            case scui_ui_scene_list_type_list_scale: { // 缩放列表
                #if 0
                // 裁内存选择(去掉过场动画)
                scui_window_stack_add(SCUI_UI_SCENE_LIST_SCALE, false,
                    scui_window_switch_none, scui_opt_dir_none);
                #else
                scui_window_stack_add(SCUI_UI_SCENE_LIST_SCALE, false);
                #endif
                break;
            }
            #endif
            case scui_ui_scene_list_type_list_arc: // 弧形列表
                scui_window_stack_add(SCUI_UI_SCENE_LIST_ARC, false);
                break;
            case scui_ui_scene_list_type_honeycomb: // 蜂窝
                scui_window_stack_add(SCUI_UI_SCENE_HONEYCOMB, false);
                break;
            case scui_ui_scene_list_type_waterfall: // 瀑布
                scui_window_stack_add(SCUI_UI_SCENE_WATERFALL, false);
                break;
            case scui_ui_scene_list_type_themewheel: // 波轮
                scui_window_stack_add(SCUI_UI_SCENE_THUMBWHEEL, false);
                break;
            case scui_ui_scene_list_type_spread: // 扩散
                scui_window_stack_add(SCUI_UI_SCENE_SPREAD, false);
                break;
            default:
                break;
            }
            
            scui_event_mask_over(event);
        }
        
        break;
    }
    default:
        break;
    }
}
