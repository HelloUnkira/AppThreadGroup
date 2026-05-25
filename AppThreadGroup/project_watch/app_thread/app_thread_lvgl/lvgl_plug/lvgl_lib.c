#include "lvgl_lib.h"

lv_group_t *lvgl_indev_group = NULL;

void lvgl_lib_ready(void)
{
    #if APP_DEV_GUI_IS_LVGL
    /*@brief lvgl 输入设备回调接口
     *@brief lvgl 屏幕刷新回调接口
     */
    void app_dev_gui_ptr_lv_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
    void app_dev_gui_key_lv_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
    void app_dev_gui_enc_lv_read(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);
    void app_dev_gui_disp_lv_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area,
        lv_color_t *color_p);
    void app_dev_gui_disp_lv_rounder(lv_disp_drv_t *disp_drv, lv_area_t *area);
    
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
            lv_palette_main(LV_PALETTE_BLUE),  lv_palette_main(LV_PALETTE_RED),
            LV_THEME_DEFAULT_DARK, LV_FONT_DEFAULT));
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
    /* 创建输入设备组 */
    lvgl_indev_group = lv_group_create();
    lv_indev_set_group(kb_indev, lvgl_indev_group);
    lv_indev_set_group(mw_indev, lvgl_indev_group);
    
    #if 1
    /* 鼠标贴图:cursor */
    extern lv_img_dsc_t app_lv_ptr_icon;
    lv_obj_t *mo_cur = lv_img_create(lv_scr_act());
    lv_img_set_src(mo_cur, &app_lv_ptr_icon);
    lv_indev_set_cursor(mo_indev, mo_cur);
    #endif
    #endif
}

static bool app_lvgl_tick_exec_package_absorb(void *pkg_old, void *pkg_new)
{
    app_thread_package_t *package_old = pkg_old;
    app_thread_package_t *package_new = pkg_new;
    
    /* 多的直接丢弃即可,这里为空 */
    return true;
}

static bool app_lvgl_tick_drv_package_absorb(void *pkg_old, void *pkg_new)
{
    app_thread_package_t *package_old = pkg_old;
    app_thread_package_t *package_new = pkg_new;
    
    /* 多的直接丢弃即可,这里为空 */
    return true;
}

void app_lvgl_tick_exec_update(void)
{
    app_thread_package_t package =
    {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_sched,
        .event  = app_thread_lvgl_sched_exec,
        .absorb = app_lvgl_tick_exec_package_absorb,
    };
    app_thread_package_notify(&package);
}

void app_lvgl_drv_update(void)
{
    app_thread_package_t package =
    {
        .thread   = app_thread_id_lvgl,
        .module   = app_thread_lvgl_sched,
        .event    = app_thread_lvgl_sched_drv,
        .priority = app_thread_package_priority_normal_above,
        .absorb   = app_lvgl_tick_drv_package_absorb,
    };
    app_thread_package_notify(&package);
}

void app_lvgl_scene_start(void)
{
    /* 向线程发送场景启动事件 */
    app_thread_package_t package =
    {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_ui,
        .event  = app_thread_lvgl_ui_scene_start,
    };
    app_thread_package_notify(&package);
}

void app_lvgl_scene_stop(void)
{
    /* 向线程发送场景停止事件 */
    app_thread_package_t package =
    {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_ui,
        .event  = app_thread_lvgl_ui_scene_stop,
    };
    app_thread_package_notify(&package);
}

void app_lvgl_scene_dlps(bool status)
{
    /* 向线程发送场景休眠唤醒事件 */
    app_thread_package_t package =
    {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_sched,
    };
    
    if (status)
    {
        package.event = app_thread_lvgl_sched_dlps_enter;
    }
    else
    {
        package.event = app_thread_lvgl_sched_dlps_exit;
    }
    
    app_thread_package_notify(&package);
}

void app_lvgl_scene_shutdown(void)
{
    /* 向线程发送场景停止事件 */
    app_thread_package_t package =
    {
        .thread = app_thread_id_lvgl,
        .module = app_thread_lvgl_ui,
        .event  = app_thread_lvgl_ui_scene_shutdown,
    };
    app_thread_package_notify(&package);
}

static app_sys_timer_t app_lvgl_timer = {0};

void app_lvgl_timer_start(void)
{
    app_sys_timer_start(&app_lvgl_timer);
}

void app_lvgl_timer_stop(void)
{
    app_sys_timer_stop(&app_lvgl_timer);
}

static void app_lvgl_timer_handler(void *timer)
{
    static uint8_t count = 0;
    count++;
    
    /* lvgl时钟约减事件(跨线程就地更新) */
    if (count % LV_SCHED_TICK_INC == 0)
    {
        lv_tick_inc(LV_SCHED_TICK_INC);
    }
    
    /* lvgl时钟调度事件 */
    if (count % LV_SCHED_TICK_EXEC == 0)
    {
        app_lvgl_tick_exec_update();
    }
    
    /* lvgl驱动检查事件 */
    if (count % LV_SCHED_DRV_EVENT == 0)
    {
        app_lvgl_drv_update();
    }
}

void app_lvgl_timer_ready(void)
{
    app_lvgl_timer.expired = app_lvgl_timer_handler;
    app_lvgl_timer.peroid  = 1;
    app_lvgl_timer.reload  = true;
}
