#ifndef APP_EXT_DEV_GUI_H
#define APP_EXT_DEV_GUI_H

/*@brief arch<:>
 */
#ifndef APP_EXT_DEV_GUI_USE_WIN
#define APP_EXT_DEV_GUI_USE_WIN     0
#endif
#ifndef APP_EXT_DEV_GUI_USE_SDL
#define APP_EXT_DEV_GUI_USE_SDL     0
#endif
/*@brief arch<s>
 */
#if 0
#elif APP_EXT_DEV_GUI_USE_WIN && APP_ARCH_IS_PC
/* dependency */
#include <Windows.h>
#include <windows.h>
#include <windowsx.h>
#elif APP_EXT_DEV_GUI_USE_SDL && APP_ARCH_IS_PC
/* dependency */
#include <SDL2/SDL.h>
#else
#error "unsupport arch or unknown arch"
#endif
/*@brief arch<e>
 */



/*@brief gui framework<:>
 */
#ifndef APP_EXT_DEV_GUI_IS_LVGL
#define APP_EXT_DEV_GUI_IS_LVGL     0
#endif
/*@brief gui framework<s>
 */
#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL
/* dependency */
#include "lvgl.h"
#include "lv_conf.h"
#include "lv_drv_conf.h"
#else
#error "unknown framework"
#endif
/*@brief gui framework<e>
 */

#endif