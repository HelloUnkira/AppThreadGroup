/*
 *实现目的:
 *    测试lvgl
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_pipe.h"
#include "app_thread_master.h"
#include "app_thread_lvgl.h"

#include "lvgl.h"
#include <SDL2/SDL.h>
#include "lvgl/examples/lv_examples.h"
#include "lv_examples/lv_demo.h"

/*@brief 测试lvgl
 */
void app_lv_ui_test(void)
{
    lv_obj_t *tv = lv_tileview_create(lv_scr_act());

    /*Tile1: just a label*/
    lv_obj_t * tile1 = lv_tileview_add_tile(tv, 0, 0, LV_DIR_BOTTOM);
    lv_obj_t * label = lv_label_create(tile1);
    lv_label_set_text(label, "Scroll down");
    lv_obj_center(label);


    /*Tile2: a button*/
    lv_obj_t * tile2 = lv_tileview_add_tile(tv, 0, 1, LV_DIR_TOP | LV_DIR_RIGHT);

    lv_obj_t * btn = lv_btn_create(tile2);

    label = lv_label_create(btn);
    lv_label_set_text(label, "Scroll up or right");

    lv_obj_set_size(btn, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(btn);

    /*Tile3: a list*/
    lv_obj_t * tile3 = lv_tileview_add_tile(tv, 1, 1, LV_DIR_LEFT);
    lv_obj_t * list = lv_list_create(tile3);
    lv_obj_set_size(list, LV_PCT(100), LV_PCT(100));

    lv_list_add_btn(list, NULL, "One");
    lv_list_add_btn(list, NULL, "Two");
    lv_list_add_btn(list, NULL, "Three");
    lv_list_add_btn(list, NULL, "Four");
    lv_list_add_btn(list, NULL, "Five");
    lv_list_add_btn(list, NULL, "Six");
    lv_list_add_btn(list, NULL, "Seven");
    lv_list_add_btn(list, NULL, "Eight");
    lv_list_add_btn(list, NULL, "Nine");
    lv_list_add_btn(list, NULL, "Ten");

    
    /* 基础测试: */
    // lv_obj_t *btn = lv_btn_create(lv_scr_act());
    // lv_obj_t *label = lv_label_create(btn);     //创建label
    // lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    // lv_label_set_text(label, "LVGL:Hello");     //设置label字内容
    // lv_obj_center(label);
    
    
    /* LVGL 自带样板测试 */
    // lv_example_switch_1();
    // lv_example_calendar_1();
    // lv_example_btnmatrix_2();
    // lv_example_checkbox_1();
    // lv_example_colorwheel_1();
    // lv_example_chart_6();
    // lv_example_table_2();
    // lv_example_scroll_2();
    // lv_example_textarea_1();
    // lv_example_msgbox_1();
    // lv_example_dropdown_2();
    // lv_example_btn_1();
    // lv_example_scroll_1();
    // lv_example_tabview_1();
    // lv_example_tabview_1();
    // lv_example_flex_3();
    // lv_example_label_1();
    
    /* LVGL Simulator Demo样板测试 */
    // lv_demo_widgets();
    // lv_demo_keypad_encoder();
    // lv_demo_benchmark();
    // lv_demo_stress();
    // lv_demo_music();
}
