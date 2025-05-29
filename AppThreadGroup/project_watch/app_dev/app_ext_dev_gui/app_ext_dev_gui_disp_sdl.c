/* 实现目标:
 *     gui驱动
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_dev_lib.h"

#if APP_EXT_DEV_GUI_USE_SDL

// 在更新过程中该分支未持续更新
// 需要重新适配SDL模式下的驱动
#error "not adaptor finish yet"

#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL
#define APP_DEV_GUI_DRV_HOR_RES     LV_DRV_HOR_RES
#define APP_DEV_GUI_DRV_VER_RES     LV_DRV_VER_RES
#define APP_DEV_GUI_DRV_ZOOM        LV_DRV_ZOOM
#define APP_DEV_GUI_DRV_DBUF        LV_DRV_DBUFFER
#elif APP_EXT_DEV_GUI_IS_SCUI
#define APP_DEV_GUI_DRV_HOR_RES     SCUI_HOR_RES
#define APP_DEV_GUI_DRV_VER_RES     SCUI_VER_RES
#define APP_DEV_GUI_DRV_ZOOM        1
#define APP_DEV_GUI_DRV_DBUF        1
#endif

typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Texture  *texture;
    volatile bool sdl_refr_qry;
    #if APP_DEV_GUI_DRV_DBUF
    uint32_t *tft_fb_act;
    #else
    uint32_t *tft_fb;
    #endif
} app_dev_gui_disp_t;

/* 设备gui_disp抽象操作参数 */
typedef struct {
    /*  */
    app_dev_gui_disp_t display;
    /*  */
    bool quit;
    /*  */
} app_dev_gui_disp_cfg_t;

/*@brief 设备适配简易转接层
 */
static inline app_dev_t * app_dev_gui_disp_inst(void)
{
    return &app_dev_gui_disp;
}

#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL

/*@brief lvgl 屏幕刷新回调接口
 */
void app_dev_gui_disp_lv_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    lv_coord_t hres = disp_drv->hor_res;
    lv_coord_t vres = disp_drv->ver_res;
    
    /* 如果该区域在屏幕外则返回 */
    if (area->x2 < 0 || area->y2 < 0 || area->x1 > hres - 1 || area->y1 > vres - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }
    
    #if APP_DEV_GUI_DRV_DBUF
    // cfg->display.tft_fb_act = (uint32_t *)color_p;
    memcpy(cfg->display.tft_fb_act, color_p, APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_VER_RES * sizeof(uint32_t));
    #else
    /* 32有效但也支持24向后兼容 */
    #if LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32
    for(int32_t y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++)
    for(int32_t x = area->x1; x <= area->x2; x++) {
        cfg->display.tft_fb[y * disp_drv->hor_res + x] = lv_color_to32(*color_p);
        color_p++;
    }
    #else
    uint32_t w = lv_area_get_width(area);
    for(int32_t y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
        memcpy(&cfg->display.tft_fb[y * APP_DEV_GUI_DRV_HOR_RES + area->x1], color_p, w * sizeof(lv_color_t));
        color_p += w;
    }
    #endif
    #endif
    
    cfg->display.sdl_refr_qry = true;
    /* 如果是最后刷新的部分则更新窗口的纹理 */
    if (lv_disp_flush_is_last(disp_drv))
        app_dev_gui_disp_update(&cfg->display);
    /* 必须调用它来告诉系统刷新准备好了 */
    lv_disp_flush_ready(disp_drv);
}

/*@brief lvgl 屏幕刷新回调接口
 */
void app_dev_gui_disp_lv_rounder(lv_disp_drv_t* disp_drv, lv_area_t* area)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    area->x1 = 0;
    area->y1 = 0;
    area->x2 = APP_DEV_GUI_DRV_HOR_RES - 1;
    area->y2 = APP_DEV_GUI_DRV_VER_RES - 1;
}

#else
#endif

/*@brief 创建屏幕
 */
static void app_dev_gui_disp_create(app_dev_gui_disp_t *disp)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    /* 创建SDL屏幕 */
    disp->window = SDL_CreateWindow("Watch Simulator",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_ZOOM,
                                    APP_DEV_GUI_DRV_VER_RES * APP_DEV_GUI_DRV_ZOOM,
                                    0);   //SDL_WINDOW_BORDERLESS);
                                    /* 隐藏边界 */  
    /* 设置屏幕渲染规则 */
    disp->renderer = SDL_CreateRenderer(disp->window, -1, SDL_RENDERER_SOFTWARE);
    /* 设置屏幕文本渲染规则 */
    disp->texture  = SDL_CreateTexture(disp->renderer,
                                       SDL_PIXELFORMAT_ARGB8888,
                                       SDL_TEXTUREACCESS_STATIC,
                                       APP_DEV_GUI_DRV_HOR_RES,
                                       APP_DEV_GUI_DRV_VER_RES);
    /* 设置屏幕文本混合规则 */
    SDL_SetTextureBlendMode(disp->texture, SDL_BLENDMODE_BLEND);
    /* 初始化帧缓冲区为灰色(77是一个经验值) */
    #if APP_DEV_GUI_DRV_DBUF
    // SDL_UpdateTexture(disp->texture, NULL, disp->tft_fb_act, APP_DEV_GUI_DRV_HOR_RES * sizeof(uint32_t));
    disp->tft_fb_act = malloc(APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_VER_RES * sizeof(uint32_t));
    memset(disp->tft_fb_act, 0x44, APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_VER_RES * sizeof(uint32_t));
    #else
    disp->tft_fb = malloc(APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_VER_RES * sizeof(uint32_t));
    memset(disp->tft_fb, 0x44, APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_VER_RES * sizeof(uint32_t));
    #endif
    disp->sdl_refr_qry = true;
}

/*@brief 更新屏幕
 */
static void app_dev_gui_disp_update(app_dev_gui_disp_t *disp)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    if (disp->sdl_refr_qry == true)
        disp->sdl_refr_qry  = false;
    else
        return;
    
    /* 刷新数据资源 */
    #if APP_DEV_GUI_DRV_DBUF
    if (disp->tft_fb_act == NULL)
        return;
    SDL_UpdateTexture(disp->texture, NULL, disp->tft_fb_act,
                      APP_DEV_GUI_DRV_HOR_RES * sizeof(uint32_t));
    #else
    SDL_UpdateTexture(disp->texture, NULL, disp->tft_fb,
                      APP_DEV_GUI_DRV_HOR_RES * sizeof(uint32_t));
    #endif
    
    /* 清除渲染器 */
    SDL_RenderClear(disp->renderer);
    
    #if APP_EXT_DEV_GUI_IS_LVGL
    #if LV_COLOR_SCREEN_TRANSP
    SDL_Rect rect = {.x = 0, .y = 0, .w = APP_DEV_GUI_DRV_HOR_RES, .h = APP_DEV_GUI_DRV_VER_RES};
    SDL_SetRenderDrawColor(disp->renderer, 0xff, 0, 0, 0xff);
    SDL_RenderDrawRect(disp->renderer, &rect);
    #endif
    #endif
    
    /* 用包含渲染图像的纹理更新渲染器 */
    SDL_RenderCopy(disp->renderer, disp->texture, NULL, NULL);
    SDL_RenderPresent(disp->renderer);
}

/*@brief 销毁屏幕
 */
static void app_dev_gui_disp_destroy(app_dev_gui_disp_t *disp)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    #if APP_DEV_GUI_DRV_DBUF
        free(disp->tft_fb_act);
    #else
        free(disp->tft_fb);
    #endif

    SDL_DestroyTexture(disp->texture);
    SDL_DestroyRenderer(disp->renderer);
    SDL_DestroyWindow(disp->window);
}

/*@brief SDL屏幕退出回调事件
 */
static int app_dev_gui_disp_sdl_quit_filter(void * userdata, SDL_Event * event)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    (void)userdata;
    
    if(event->type == SDL_QUIT)
        cfg->quit = true;
    
    if(event->type == SDL_WINDOWEVENT)
    if(event->window.event == SDL_WINDOWEVENT_CLOSE)
        cfg->quit = true;
    
    return 1;
}

/*@brief lvgl SDL屏幕回调接口
 */
void app_dev_gui_disp_msg_cb(SDL_Event *event)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    if (event->type == SDL_WINDOWEVENT)
    switch (event->window.event) {
    #if SDL_VERSION_ATLEAST(2, 0, 5)
    case SDL_WINDOWEVENT_TAKE_FOCUS:
    #endif
    case SDL_WINDOWEVENT_EXPOSED:
        app_dev_gui_disp_update(&cfg->display);
        break;
    default:
        break;
    }
}

/*@brief SDL 屏幕需要关机
 */
bool app_dev_gui_disp_shutdown(void)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    return cfg->quit;
}

/*@brief gui_disp设备初始化
 *@param driver 设备实例
 */
static inline void app_dev_gui_disp_hal_ready(app_dev_t *driver)
{
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    /* 初始化SDL */
    if (true) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_SetEventFilter(app_dev_gui_disp_sdl_quit_filter, NULL);
        app_dev_gui_disp_create(&cfg->display);

    }
    
    /* 反初始化SDL屏幕(不需要) */
    if (false) {
        app_dev_gui_disp_destroy(&cfg->display);
        SDL_Quit();
    }
}

/*@brief gui_disp设备进入dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_disp_hal_dlps_enter(app_dev_t *driver)
{
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    /* 无事可做,不能关闭屏幕,键盘事件来源于屏幕 */
    /* 不能关闭屏幕,否则SDL抓不到键盘事件了,让黑屏代替关闭 */
}

/*@brief gui_disp设备退出dlps
 *@param driver 设备实例
 */
static inline void app_dev_gui_disp_hal_dlps_exit(app_dev_t *driver)
{
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    /* 无事可做,不能关闭屏幕,键盘事件来源于屏幕 */
    /* 不能关闭屏幕,否则SDL抓不到键盘事件了,让黑屏代替关闭 */
}

/* 静态配置的设备操作参数 */
static app_dev_gui_disp_cfg_t app_dev_gui_disp_cfg = {
    .display = {0},
    .quit = false,
};

/* 静态配置的设备操作集合 */
static const app_dev_gui_disp_api_t app_dev_gui_disp_api = {
    .ready          = app_dev_gui_disp_hal_ready,
    .dlps_enter     = app_dev_gui_disp_hal_dlps_enter,
    .dlps_exit      = app_dev_gui_disp_hal_dlps_exit,
};

/* 动态的设备操作数据 */
static app_dev_gui_disp_data_t app_dev_gui_disp_data = {
    .args = NULL,
};

/* 静态配置的设备实例 */
const app_dev_t app_dev_gui_disp = {
    .name = "app_dev_gui_disp",
    .cfg  = &app_dev_gui_disp_cfg,
    .api  = &app_dev_gui_disp_api,
    .data = &app_dev_gui_disp_data,
};

#endif
