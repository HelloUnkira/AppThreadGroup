/*实现目标:
 *    lv适配SDL模拟器
 */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include <SDL2/SDL.h>
#include "lv_drv_conf.h"
#include "lvgl.h"
#include "app_lv_mouse.h"
#include "app_lv_mousewheel.h"
#include "app_lv_keyboard.h"

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

static bool app_lv_display_sdl_quit = false;
static app_mutex_t app_lv_display_mutex = {0};
static app_lv_display_t app_lv_display_screen = {0};

/*@brief 创建SDL屏幕
 */
static void app_lv_display_sdl_create(app_lv_display_t *disp)
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
    disp->renderer = SDL_CreateRenderer(disp->window, -1,
                                        SDL_RENDERER_SOFTWARE);
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
    SDL_UpdateTexture(disp->texture, NULL, disp->tft_fb_act,
                      LV_DRV_HOR_RES * 4);
#else
    disp->tft_fb = malloc(LV_DRV_HOR_RES * LV_DRV_VER_RES * 4);
    memset(disp->tft_fb, 0x44, LV_DRV_HOR_RES * LV_DRV_VER_RES * 4);
#endif
    disp->sdl_refr_qry = true;
}

/*@brief 更新SDL屏幕
 */
static void app_lv_display_sdl_update(app_lv_display_t *disp)
{
    /* 刷新数据资源 */
    #if LV_DRV_DBUFFER
    if (disp->tft_fb_act == NULL)
        return;
    SDL_UpdateTexture(disp->texture, NULL,
                      disp->tft_fb_act,
                      LV_DRV_HOR_RES * 4);
    #else
    SDL_UpdateTexture(disp->texture, NULL,
                      disp->tft_fb,
                      LV_DRV_HOR_RES * 4);
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

/*@brief SDL屏幕退出
 */
static void app_lv_display_sdl_clean_up(void)
{
    SDL_DestroyTexture(app_lv_display_screen.texture);
    SDL_DestroyRenderer(app_lv_display_screen.renderer);
    SDL_DestroyWindow(app_lv_display_screen.window);
    SDL_Quit();
}

/*@brief SDL屏幕退出回调事件
 */
static int app_lv_display_sdl_quit_filter(void * userdata, SDL_Event * event)
{
    (void)userdata;
    
    app_mutex_take(&app_lv_display_mutex);
    
    if(event->type == SDL_QUIT)
        app_lv_display_sdl_quit = true;
    
    if(event->type == SDL_WINDOWEVENT)
    if(event->window.event == SDL_WINDOWEVENT_CLOSE)
        app_lv_display_sdl_quit = true;
    
    app_mutex_give(&app_lv_display_mutex);
    
    return 1;
}

/*@brief SDL输出设备需要关机
 */
bool app_lv_display_shutdown(void)
{
    app_mutex_take(&app_lv_display_mutex);
    bool retval = app_lv_display_sdl_quit;
    app_mutex_give(&app_lv_display_mutex);
    return retval;
}

/*@brief lvgl初始化SDL屏幕
 */
void app_lv_display_ready(void)
{
    app_mutex_process(&app_lv_display_mutex);
    /* 初始化SDL */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetEventFilter(app_lv_display_sdl_quit_filter, NULL);
    app_lv_display_sdl_create(&app_lv_display_screen);
}

/*@brief lvgl反初始化SDL屏幕
 */
void app_lv_display_over(void)
{
    app_lv_display_sdl_clean_up();
}

/*@brief lvgl屏幕刷新回调接口
 */
void app_lv_display_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    lv_coord_t hres = disp_drv->hor_res;
    lv_coord_t vres = disp_drv->ver_res;

    /* 如果该区域在屏幕外则返回 */
    if (area->x2 < 0 ||
        area->y2 < 0 ||
        area->x1 > hres - 1 ||
        area->y1 > vres - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }
    
    app_mutex_take(&app_lv_display_mutex);
    
    #if LV_DRV_DBUFFER
    app_lv_display_screen.tft_fb_act = (uint32_t *)color_p;
    #else
    int32_t y;
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
    if (app_lv_display_screen.sdl_refr_qry == true) {
        app_lv_display_screen.sdl_refr_qry  = false;
        app_lv_display_sdl_update(&app_lv_display_screen);
    }
    
    app_mutex_give(&app_lv_display_mutex);
    
    /* 必须调用它来告诉系统刷新准备好了 */
    lv_disp_flush_ready(disp_drv);
}

/*@brief SDL输出设备回调接口
 */
void app_lv_display_handler(SDL_Event *event)
{
    if (event->type == SDL_WINDOWEVENT)
    switch (event->window.event) {
    #if SDL_VERSION_ATLEAST(2, 0, 5)
        case SDL_WINDOWEVENT_TAKE_FOCUS:
    #endif
        case SDL_WINDOWEVENT_EXPOSED:
            app_mutex_take(&app_lv_display_mutex);
            app_lv_display_sdl_update(&app_lv_display_screen);
            app_mutex_give(&app_lv_display_mutex);
            break;
        default:
            break;
    }
}
