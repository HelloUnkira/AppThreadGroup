/*实现目标:
 *    lv适配SDL模拟器
 */

#include "app_lv_lib.h"

#if APP_LV_DRV_USE_SDL

typedef struct {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    SDL_Texture  *texture;
    volatile bool sdl_refr_qry;
#if LV_DRV_DBUFFER
    uint32_t *tft_fb_act;
#else
    uint32_t *tft_fb;
#endif
} app_lv_display_t;

static bool app_lv_display_quit = false;
static app_lv_display_t app_lv_display_screen = {0};

/*@brief 创建屏幕
 */
static void app_lv_display_create(app_lv_display_t *disp)
{
    /* 创建SDL屏幕 */
    disp->window = SDL_CreateWindow("Watch Simulator",
                                    SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED,
                                    LV_DRV_HOR_RES * LV_DRV_ZOOM,
                                    LV_DRV_VER_RES * LV_DRV_ZOOM,
                                    0);   //SDL_WINDOW_BORDERLESS);
                                    /* 隐藏边界 */  
    /* 设置屏幕渲染规则 */
    disp->renderer = SDL_CreateRenderer(disp->window, -1, SDL_RENDERER_SOFTWARE);
    /* 设置屏幕文本渲染规则 */
    disp->texture  = SDL_CreateTexture(disp->renderer,
                                       SDL_PIXELFORMAT_ARGB8888,
                                       SDL_TEXTUREACCESS_STATIC,
                                       LV_DRV_HOR_RES,
                                       LV_DRV_VER_RES);
    /* 设置屏幕文本混合规则 */
    SDL_SetTextureBlendMode(disp->texture, SDL_BLENDMODE_BLEND);
    /* 初始化帧缓冲区为灰色(77是一个经验值) */
#if LV_DRV_DBUFFER
    // SDL_UpdateTexture(disp->texture, NULL, disp->tft_fb_act, LV_DRV_HOR_RES * sizeof(uint32_t));
    disp->tft_fb_act = malloc(LV_DRV_HOR_RES * LV_DRV_VER_RES * sizeof(uint32_t));
    memset(disp->tft_fb_act, 0x44, LV_DRV_HOR_RES * LV_DRV_VER_RES * sizeof(uint32_t));
#else
    disp->tft_fb = malloc(LV_DRV_HOR_RES * LV_DRV_VER_RES * sizeof(uint32_t));
    memset(disp->tft_fb, 0x44, LV_DRV_HOR_RES * LV_DRV_VER_RES * sizeof(uint32_t));
#endif
    disp->sdl_refr_qry = true;
}

/*@brief 更新屏幕
 */
static void app_lv_display_update(app_lv_display_t *disp)
{
    if (disp->sdl_refr_qry == true)
        disp->sdl_refr_qry  = false;
    else
        return;
    
    /* 刷新数据资源 */
    #if LV_DRV_DBUFFER
    if (disp->tft_fb_act == NULL)
        return;
    SDL_UpdateTexture(disp->texture, NULL, disp->tft_fb_act,
                      LV_DRV_HOR_RES * sizeof(uint32_t));
    #else
    SDL_UpdateTexture(disp->texture, NULL, disp->tft_fb,
                      LV_DRV_HOR_RES * sizeof(uint32_t));
    #endif
    
    /* 清除渲染器 */
    SDL_RenderClear(disp->renderer);
    #if LV_COLOR_SCREEN_TRANSP
    SDL_Rect rect = {.x = 0, .y = 0, .w = LV_DRV_HOR_RES, .h = LV_DRV_VER_RES};
    SDL_SetRenderDrawColor(disp->renderer, 0xff, 0, 0, 0xff);
    SDL_RenderDrawRect(disp->renderer, &rect);
    #endif
    /* 用包含渲染图像的纹理更新渲染器 */
    SDL_RenderCopy(disp->renderer, disp->texture, NULL, NULL);
    SDL_RenderPresent(disp->renderer);
}

/*@brief 销毁屏幕
 */
static void app_lv_display_destroy(app_lv_display_t *disp)
{
    #if LV_DRV_DBUFFER
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
static int app_lv_display_sdl_quit_filter(void * userdata, SDL_Event * event)
{
    (void)userdata;
    
    if(event->type == SDL_QUIT)
        app_lv_display_quit = true;
    
    if(event->type == SDL_WINDOWEVENT)
    if(event->window.event == SDL_WINDOWEVENT_CLOSE)
        app_lv_display_quit = true;
    
    return 1;
}

/*@brief lvgl 屏幕初始化
 */
void app_lv_display_ready(void)
{
    /* 初始化SDL */
    if (true) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_SetEventFilter(app_lv_display_sdl_quit_filter, NULL);
        app_lv_display_create(&app_lv_display_screen);

    }

    /* 反初始化SDL屏幕(不需要) */
    if (false) {
        app_lv_display_destroy(&app_lv_display_screen);
        SDL_Quit();
    }
}

/*@brief lvgl 屏幕进入低功耗
 */
void app_lv_display_dlps_enter(void)
{
    /* 无事可做,不能关闭屏幕,键盘事件来源于屏幕 */
    /* 不能关闭屏幕,否则SDL抓不到键盘事件了,让黑屏代替关闭 */
}

/*@brief lvgl 屏幕退出低功耗
 */
void app_lv_display_dlps_exit(void)
{
    /* 无事可做,不能关闭屏幕,键盘事件来源于屏幕 */
    /* 不能关闭屏幕,否则SDL抓不到键盘事件了,让黑屏代替关闭 */
}

/*@brief lvgl 屏幕刷新回调接口
 */
void app_lv_display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_coord_t hres = disp_drv->hor_res;
    lv_coord_t vres = disp_drv->ver_res;
    
    /* 如果该区域在屏幕外则返回 */
    if (area->x2 < 0 || area->y2 < 0 || area->x1 > hres - 1 || area->y1 > vres - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }
    
    #if LV_DRV_DBUFFER
    // app_lv_display_screen.tft_fb_act = (uint32_t *)color_p;
    memcpy(app_lv_display_screen.tft_fb_act, color_p, LV_DRV_HOR_RES * LV_DRV_VER_RES * sizeof(uint32_t));
    #else
    /* 32有效但也支持24向后兼容 */
    #if LV_COLOR_DEPTH != 24 && LV_COLOR_DEPTH != 32
    for(int32_t y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++)
    for(int32_t x = area->x1; x <= area->x2; x++) {
        app_lv_display_screen.tft_fb[y * disp_drv->hor_res + x] = lv_color_to32(*color_p);
        color_p++;
    }
    #else
    uint32_t w = lv_area_get_width(area);
    for(int32_t y = area->y1; y <= area->y2 && y < disp_drv->ver_res; y++) {
        memcpy(&app_lv_display_screen.tft_fb[y * LV_DRV_HOR_RES + area->x1], color_p, w * sizeof(lv_color_t));
        color_p += w;
    }
    #endif
    #endif
    
    app_lv_display_screen.sdl_refr_qry = true;
    /* 如果是最后刷新的部分则更新窗口的纹理 */
    if (lv_disp_flush_is_last(disp_drv))
        app_lv_display_update(&app_lv_display_screen);
    /* 必须调用它来告诉系统刷新准备好了 */
    lv_disp_flush_ready(disp_drv);
}

/*@brief lvgl SDL屏幕回调接口
 */
void app_lv_display_handler(SDL_Event *event)
{
    if (event->type == SDL_WINDOWEVENT)
    switch (event->window.event) {
    #if SDL_VERSION_ATLEAST(2, 0, 5)
    case SDL_WINDOWEVENT_TAKE_FOCUS:
    #endif
    case SDL_WINDOWEVENT_EXPOSED:
        app_lv_display_update(&app_lv_display_screen);
        break;
    default:
        break;
    }
}

/*@brief lvgl 屏幕需要关机
 */
bool app_lv_display_shutdown(void)
{
    return app_lv_display_quit;
}

#endif
