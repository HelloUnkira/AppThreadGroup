/* 实现目标:
 *     层级接口打包
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

lv_group_t *app_lv_group = NULL;

/*@brief lvgl组实例
 */
lv_group_t *app_lv_group_inst(void)
{
    return app_lv_group;
}

/*@brief 初始化lvgl
 */
void app_lv_ready(void)
{
    #if APP_EXT_DEV_GUI_IS_LVGL
    /*@brief lvgl 输入设备回调接口
     *@brief lvgl 屏幕刷新回调接口
     */
    void app_dev_gui_ptr_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
    void app_dev_gui_key_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
    void app_dev_gui_enc_lv_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
    void app_dev_gui_disp_lv_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);
    void app_dev_gui_disp_lv_rounder(lv_disp_drv_t* disp_drv, lv_area_t* area);
    
    #if LV_DRV_DBUFFER
    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buffer1[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    static lv_color_t buffer2[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    lv_disp_draw_buf_init(&disp_buf, buffer1, buffer2,
                          LV_DRV_HOR_RES * LV_DRV_VER_RES);
    #else
    static lv_disp_draw_buf_t disp_buf;
    static lv_color_t buffer1[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    static lv_color_t buffer2[LV_DRV_HOR_RES * LV_DRV_VER_RES] = {0};
    lv_disp_draw_buf_init(&disp_buf, buffer1, NULL,
                          LV_DRV_HOR_RES * LV_DRV_VER_RES);
    #endif
    /* 创建显示设备 */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf   = &disp_buf;
    disp_drv.flush_cb   = app_dev_gui_disp_lv_flush;
    disp_drv.rounder_cb = app_dev_gui_disp_lv_rounder;
    disp_drv.hor_res    = LV_DRV_HOR_RES;
    disp_drv.ver_res    = LV_DRV_VER_RES;
    disp_drv.antialiasing = 1;
    /* 创建显示 */
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);
    /* 创建显示的默认主题 */
    lv_disp_set_theme(disp, lv_theme_default_init(disp,
                            lv_palette_main(LV_PALETTE_BLUE),
                            lv_palette_main(LV_PALETTE_RED),
                            LV_THEME_DEFAULT_DARK,
                            LV_FONT_DEFAULT));
    /* 创建输入设备组 */
    app_lv_group = lv_group_create();
    /* 注册输入设备:鼠标,键盘,鼠标滑轮 */
    static lv_indev_drv_t mo_dev;
    static lv_indev_drv_t kb_dev;
    static lv_indev_drv_t mw_dev;
    lv_indev_drv_init(&mo_dev);
    lv_indev_drv_init(&kb_dev);
    lv_indev_drv_init(&mw_dev);
    mo_dev.type = LV_INDEV_TYPE_POINTER;
    kb_dev.type = LV_INDEV_TYPE_KEYPAD;
    mw_dev.type = LV_INDEV_TYPE_ENCODER;
    mo_dev.read_cb = app_dev_gui_ptr_lv_read;
    kb_dev.read_cb = app_dev_gui_key_lv_read;
    mw_dev.read_cb = app_dev_gui_enc_lv_read;
    lv_indev_t *mo_indev = lv_indev_drv_register(&mo_dev);
    lv_indev_t *kb_indev = lv_indev_drv_register(&kb_dev);
    lv_indev_t *mw_indev = lv_indev_drv_register(&mw_dev);
    lv_indev_set_group(kb_indev, app_lv_group);
    lv_indev_set_group(mw_indev, app_lv_group);
    lv_group_set_default(app_lv_group);
    /* 鼠标贴图:cursor */
    extern lv_img_dsc_t app_lv_ptr_icon;
    lv_obj_t *mo_cur = lv_img_create(lv_scr_act());
    lv_img_set_src(mo_cur, &app_lv_ptr_icon);
    lv_indev_set_cursor(mo_indev, mo_cur);
    /* 设置顶部边界 */
    lv_obj_t *top = lv_layer_top();
    lv_obj_set_style_radius(top, 45, 0);
    lv_obj_set_style_border_width(top, 1, 0);
    lv_obj_set_style_border_color(top, lv_palette_main(LV_PALETTE_GREY), 0);
    #endif
}
