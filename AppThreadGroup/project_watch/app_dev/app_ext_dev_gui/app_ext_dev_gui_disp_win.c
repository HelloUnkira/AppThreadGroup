/* 实现目标:
 *     gui驱动
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"

#if APP_EXT_DEV_GUI_USE_WIN

/*  */
#define APP_DEV_GUI_DISP_STY      (WS_OVERLAPPEDWINDOW & ~(WS_SIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME))
#define APP_DEV_GUI_DISP_STY_EX   (WS_EX_CLIENTEDGE)
#define APP_DEV_GUI_DISP_DPI_DEF  (96)

/*  */
#if 0
#elif APP_EXT_DEV_GUI_IS_LVGL
#define APP_DEV_GUI_DRV_HOR_RES     LV_DRV_HOR_RES
#define APP_DEV_GUI_DRV_VER_RES     LV_DRV_VER_RES
#define APP_DEV_GUI_DRV_ZOOM        LV_DRV_ZOOM
#define APP_DEV_GUI_DRV_DBUF        LV_DRV_DBUFFER
#elif APP_EXT_DEV_GUI_IS_SCUI
#define APP_DEV_GUI_DRV_HOR_RES     SCUI_DRV_HOR_RES
#define APP_DEV_GUI_DRV_VER_RES     SCUI_DRV_VER_RES
#define APP_DEV_GUI_DRV_ZOOM        1
#define APP_DEV_GUI_DRV_DBUF        SCUI_DRV_DBUFFER
#endif

/*  */
typedef struct {
    WNDCLASSEXW     window_class;
    HINSTANCE       instance;
    HICON           icon;
    HWND            window;
    HDC             buf_hdc;
    UINT32         *pixel_buf;
    SIZE_T          pixel_buf_size;
    int             dpi_value;
} app_dev_gui_disp_t;

/* 设备gui_disp抽象操作参数 */
typedef struct {
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
/*@brief lvgl 屏幕接口
 */
void app_dev_gui_disp_flush_manual(lv_coord_t x, lv_coord_t y, lv_coord_t w, lv_coord_t h)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    RECT rect;
    
    rect.top    = MulDiv(y * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF);
    rect.left   = MulDiv(x * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF);
    rect.right  = MulDiv(w * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF) + rect.left;
    rect.bottom = MulDiv(h * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF) + rect.top;
    
    #if 0
    HDC win_hdc = GetDC(g_window_handle);
    if (win_hdc) {
        int prev_mode = SetStretchBltMode(win_hdc, HALFTONE);
        
        StretchBlt(win_hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,
                   cfg->display.window, x, y, w, h, SRCCOPY);
        
        SetStretchBltMode(win_hdc, prev_mode);
        ReleaseDC(cfg->display.window, win_hdc);
    }
    #else
    InvalidateRect(cfg->display.window, &rect, false);
    UpdateWindow(cfg->display.window);
    #endif
}

/*@brief lvgl 屏幕刷新回调接口
 */
void app_dev_gui_disp_lv_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    #if 0
    #elif (LV_COLOR_DEPTH == 32) || \
          (LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP == 0) || \
          (LV_COLOR_DEPTH == 8)  || \
          (LV_COLOR_DEPTH == 1)
        (void)area;
        memcpy(cfg->display.pixel_buf, color_p,
               cfg->display.pixel_buf_size);
    #elif (LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0)
    UINT16  current  = 0;
    SIZE_T  data_cnt = cfg->display.pixel_buf_size / sizeof(UINT16);
    PUINT16 data_src = (PUINT16)color_p;
    PUINT16 data_dst = (PUINT16)cfg->display.pixel_buf;
    for (SIZE_T idx = 0; idx < data_cnt; idx++) {
         current  = *data_src;
        *data_dst = (LOBYTE(current) << 8) | HIBYTE(current);
        data_src++;
        data_dst++;
    }
    #else
    for (int y = area->y1; y <= area->y2; ++y)
    for (int x = area->x1; x <= area->x2; ++x) {
        cfg->display.pixel_buf[y * disp_drv->hor_res + x] = lv_color_to32(*color_p);
        color_p++;
    }
    #endif
    
    HDC win_hdc = GetDC(cfg->display.window);
    if (win_hdc) {
        int prev_mode = SetStretchBltMode(win_hdc, HALFTONE);
        
        StretchBlt(win_hdc, 0, 0,
            MulDiv(APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF),
            MulDiv(APP_DEV_GUI_DRV_VER_RES * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF),
            cfg->display.buf_hdc, 0, 0, APP_DEV_GUI_DRV_HOR_RES, APP_DEV_GUI_DRV_VER_RES, SRCCOPY);
        
        SetStretchBltMode(win_hdc, prev_mode);
        ReleaseDC(cfg->display.window, win_hdc);
    }
    
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
#elif APP_EXT_DEV_GUI_IS_SCUI
/*@brief scui 屏幕刷新回调接口
 */
void app_dev_gui_disp_scui_flush(scui_surface_t *surface)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    APP_SYS_ASSERT(SCUI_DRV_PIXEL_DEPTH / 8 == SCUI_PIXEL_SIZE);
    APP_SYS_ASSERT(surface->clip.w * surface->clip.h * SCUI_PIXEL_SIZE == cfg->display.pixel_buf_size);
    
    memcpy(cfg->display.pixel_buf, surface->pixel, cfg->display.pixel_buf_size);
    
    HDC win_hdc = GetDC(cfg->display.window);
    if (win_hdc) {
        int prev_mode = SetStretchBltMode(win_hdc, HALFTONE);
        
        StretchBlt(win_hdc, 0, 0,
            MulDiv(APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF),
            MulDiv(APP_DEV_GUI_DRV_VER_RES * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF),
            cfg->display.buf_hdc, 0, 0, APP_DEV_GUI_DRV_HOR_RES, APP_DEV_GUI_DRV_VER_RES, SRCCOPY);
        
        SetStretchBltMode(win_hdc, prev_mode);
        ReleaseDC(cfg->display.window, win_hdc);
    }
}
#else
#endif

/*@brief Win 屏幕接口(内部扩展)
 */
UINT app_dev_gui_disp_get_dpi(bool is_def)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    if (is_def)
        return APP_DEV_GUI_DISP_DPI_DEF;
    if (cfg->display.dpi_value != 0)
        return cfg->display.dpi_value;
    else
        return APP_DEV_GUI_DISP_DPI_DEF;
}

/*@brief Win 屏幕接口(内部扩展)
 */
HWND app_dev_gui_disp_get_window(void)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    return cfg->display.window;
}

/*@brief Win 屏幕接口
 */
static UINT app_dev_gui_disp_get_dpi_for_window(_In_ HWND WindowHandle)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    typedef enum {
        MDT_EFFECTIVE_DPI   = 0,
        MDT_ANGULAR_DPI     = 1,
        MDT_RAW_DPI         = 2,
        MDT_DEFAULT         = MDT_EFFECTIVE_DPI,
    } dpi_type_t;
    
    typedef HRESULT(WINAPI* function_t)(HMONITOR, dpi_type_t, UINT*, UINT*);
    
    UINT retval = (UINT)(-1);
    HMODULE module_handle = LoadLibraryW(L"SHCore.dll");

    if (module_handle) {
        function_t function = (function_t)(GetProcAddress(module_handle, "GetDpiForMonitor"));
        
        if (function) {
            HMONITOR monitor_handle = MonitorFromWindow(WindowHandle, MONITOR_DEFAULTTONEAREST);
            
            UINT dpi_x = 0, dpi_y = 0;
            if (SUCCEEDED(function(monitor_handle, MDT_EFFECTIVE_DPI, &dpi_x, &dpi_y)))
                retval = dpi_x;
        }

        FreeLibrary(module_handle);
    }

    if (retval == (UINT)(-1)) {
        HDC window_dc_handle = GetDC(WindowHandle);
        if (window_dc_handle)
        {
            retval = GetDeviceCaps(window_dc_handle, LOGPIXELSX);
            ReleaseDC(WindowHandle, window_dc_handle);
        }
    }

    if (retval == (UINT)(-1))
        retval  = APP_DEV_GUI_DISP_DPI_DEF;

    return retval;
}

/*@brief Win 屏幕接口
 */
static BOOL app_dev_gui_disp_reg_touch_window(HWND hWnd,ULONG ulFlags)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    HMODULE module_handle = GetModuleHandleW(L"user32.dll");
    if (!module_handle)
        return FALSE;
    
    typedef BOOL(WINAPI* function_t)(HWND, ULONG);
    
    function_t function = (function_t)(GetProcAddress(module_handle, "RegisterTouchWindow"));
    if (!function)
        return FALSE;
    
    return function(hWnd, ulFlags);
}

/*@brief Win 屏幕接口
 */
static BOOL app_dev_gui_disp_enable_child_win_dpi_msg(HWND WindowHandle)
{ 
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    // This hack is only for Windows 10 TH1/TH2 only.
    // We don't need this hack if the Per Monitor Aware V2 is existed.
    OSVERSIONINFOEXW os_version_info_ex     = { 0 };
    os_version_info_ex.dwOSVersionInfoSize  = sizeof(OSVERSIONINFOEXW);
    os_version_info_ex.dwMajorVersion       = 10;
    os_version_info_ex.dwMinorVersion       = 0;
    os_version_info_ex.dwBuildNumber        = 14393;
    if (!VerifyVersionInfoW(&os_version_info_ex,
        VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER,
        VerSetConditionMask(
            VerSetConditionMask(
                VerSetConditionMask(0,
                    VER_MAJORVERSION,
                    VER_GREATER_EQUAL),
                VER_MINORVERSION,
                VER_GREATER_EQUAL),
            VER_BUILDNUMBER,
            VER_LESS)))
        return FALSE;
    
    HMODULE module_handle = GetModuleHandleW(L"user32.dll");
    if (!module_handle)
        return FALSE;
    
    typedef BOOL(WINAPI* function_t)(HWND, BOOL);
    
    function_t function = (function_t)(GetProcAddress(module_handle, "EnableChildWindowDpiMessage"));
    if (!function)
        return FALSE;
    
    return function(WindowHandle, TRUE);
}

/*@brief Win 屏幕接口
 */
static HDC app_dev_gui_disp_frame_buffer(HWND WindowHandle, LONG Width, LONG Height, UINT32** PixelBuffer, SIZE_T* PixelBufferSize)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    HDC win_frame_buf_hdc = NULL;

    if (PixelBuffer && PixelBufferSize) {
        HDC win_hdc = GetDC(WindowHandle);
        if (win_hdc)
        {
            win_frame_buf_hdc = CreateCompatibleDC(win_hdc);
            ReleaseDC(WindowHandle, win_hdc);
        }
        
        #if 0
        #elif APP_EXT_DEV_GUI_IS_LVGL
        #define APP_EXT_DEV_GUI_COLOR_DEPTH     LV_COLOR_DEPTH
        #elif APP_EXT_DEV_GUI_IS_SCUI
        #define APP_EXT_DEV_GUI_COLOR_DEPTH     SCUI_DRV_PIXEL_DEPTH
        #else
        #endif
        
        if (win_frame_buf_hdc)
        {
            #if 0
            #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 32
            BITMAPINFO bitmap_info = { 0 };
            #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 16
            typedef struct {
                BITMAPINFOHEADER bmiHeader;
                DWORD bmiColorMask[3];
            } BITMAPINFO_16BPP, *PBITMAPINFO_16BPP;

            BITMAPINFO_16BPP bitmap_info = { 0 };
            #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 8
            typedef struct {
                BITMAPINFOHEADER bmiHeader;
                RGBQUAD bmiColors[256];
            } BITMAPINFO_8BPP, *PBITMAPINFO_8BPP;

            BITMAPINFO_8BPP bitmap_info = { 0 };
            #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 1
            typedef struct {
                BITMAPINFOHEADER bmiHeader;
                RGBQUAD bmiColors[2];
            } BITMAPINFO_1BPP, *PBITMAPINFO_1BPP;

            BITMAPINFO_1BPP bitmap_info = { 0 };
            #else
            BITMAPINFO bitmap_info = { 0 };
            #endif
            
            bitmap_info.bmiHeader.biSize    = sizeof(BITMAPINFOHEADER);
            bitmap_info.bmiHeader.biWidth   = Width;
            bitmap_info.bmiHeader.biHeight  = -Height;
            bitmap_info.bmiHeader.biPlanes  = 1;
            #if 0
            #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 32
            bitmap_info.bmiHeader.biBitCount        = 32;
            bitmap_info.bmiHeader.biCompression     = BI_RGB;
            #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 16
            bitmap_info.bmiHeader.biBitCount        = 16;
            bitmap_info.bmiHeader.biCompression     = BI_BITFIELDS;
            bitmap_info.bmiColorMask[0] = 0xF800;
            bitmap_info.bmiColorMask[1] = 0x07E0;
            bitmap_info.bmiColorMask[2] = 0x001F;
            #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 8
            bitmap_info.bmiHeader.biBitCount        = 8;
            bitmap_info.bmiHeader.biCompression     = BI_RGB;
            for (size_t idx = 0; idx < 256; idx++) {
                lv_color8_t color;
                color.full = idx;
                bitmap_info.bmiColors[idx].rgbRed = LV_COLOR_GET_R(color) * 36;
                bitmap_info.bmiColors[idx].rgbGreen = LV_COLOR_GET_G(color) * 36;
                bitmap_info.bmiColors[idx].rgbBlue = LV_COLOR_GET_B(color) * 85;
                bitmap_info.bmiColors[idx].rgbReserved = 0xFF;
            }
            #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 1
            bitmap_info.bmiHeader.biBitCount        = 8;
            bitmap_info.bmiHeader.biCompression     = BI_RGB;
            bitmap_info.bmiHeader.biClrUsed         = 2;
            bitmap_info.bmiHeader.biClrImportant    = 2;
            bitmap_info.bmiColors[0].rgbRed         = 0x00;
            bitmap_info.bmiColors[0].rgbGreen       = 0x00;
            bitmap_info.bmiColors[0].rgbBlue        = 0x00;
            bitmap_info.bmiColors[0].rgbReserved    = 0xFF;
            bitmap_info.bmiColors[1].rgbRed         = 0xFF;
            bitmap_info.bmiColors[1].rgbGreen       = 0xFF;
            bitmap_info.bmiColors[1].rgbBlue        = 0xFF;
            bitmap_info.bmiColors[1].rgbReserved    = 0xFF;
            #else
            bitmap_info.bmiHeader.biBitCount        = 32;
            bitmap_info.bmiHeader.biCompression     = BI_RGB;
            #endif

            HBITMAP bitmap_handle = CreateDIBSection(win_frame_buf_hdc,
                                                    (PBITMAPINFO)(&bitmap_info),
                                                     DIB_RGB_COLORS, (void**)PixelBuffer, NULL,0);
            if (bitmap_handle) {
                #if 0
                #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 32
                *PixelBufferSize = Width * Height * sizeof(UINT32);
                #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 16
                *PixelBufferSize = Width * Height * sizeof(UINT16);
                #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 8
                *PixelBufferSize = Width * Height * sizeof(UINT8);
                #elif APP_EXT_DEV_GUI_COLOR_DEPTH == 1
                *PixelBufferSize = Width * Height * sizeof(UINT8);
                #else
                *PixelBufferSize = Width * Height * sizeof(UINT32);
                #endif
                DeleteObject(SelectObject(win_frame_buf_hdc, bitmap_handle));
                DeleteObject(bitmap_handle);
            }
            else
            {
                DeleteDC(win_frame_buf_hdc);
                win_frame_buf_hdc = NULL;
            }
        }
    }

    return win_frame_buf_hdc;
}

/*@brief 创建屏幕
 */
static void app_dev_gui_disp_create(app_dev_gui_disp_t *disp)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    int retval = 0;
    HRESULT CALLBACK app_ext_dev_drv_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    
    disp->instance = GetModuleHandleW(NULL);
    /* 1.初始化屏幕类 */
    disp->window_class.style        = 0;
    disp->window_class.cbSize       = sizeof(WNDCLASSEX);
    disp->window_class.lpfnWndProc  = app_ext_dev_drv_msg_cb;
    disp->window_class.cbClsExtra   = 0;
    disp->window_class.cbWndExtra   = 0;
    disp->window_class.hInstance        = disp->instance;
    disp->window_class.hIcon            = disp->icon;
    disp->window_class.hIconSm          = disp->icon;
    disp->window_class.hCursor          = LoadCursorW(NULL, IDC_ARROW);
    disp->window_class.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    disp->window_class.lpszMenuName     = NULL;
    disp->window_class.lpszClassName    = L"Watch Simulator";
    retval = RegisterClassExW(&disp->window_class);
    // APP_SYS_ASSERT(retval != 0);
    /* 2.创建屏幕 */
    disp->window = CreateWindowExW(APP_DEV_GUI_DISP_STY_EX,
                                   L"Watch Simulator",
                                   #if 0
                                   #elif APP_EXT_DEV_GUI_IS_LVGL
                                   L"App Thread Group LVGL Watch",
                                   #elif APP_EXT_DEV_GUI_IS_SCUI
                                   L"App Thread Group SCUI Watch",
                                   #else
                                   #endif
                                   APP_DEV_GUI_DISP_STY,
                                   CW_USEDEFAULT, 0,
                                   CW_USEDEFAULT, 0,
                                   NULL, NULL, disp->instance, NULL);
    // APP_SYS_ASSERT(disp->window != 0);
    /* 3.更新DPI */
    disp->dpi_value = app_dev_gui_disp_get_dpi_for_window(disp->window);
    /* 4.更新屏幕域 */
    RECT win_size;
    win_size.top    = 0;
    win_size.left   = 0;
    win_size.right  = MulDiv(APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_ZOOM, disp->dpi_value, APP_DEV_GUI_DISP_DPI_DEF);
    win_size.bottom = MulDiv(APP_DEV_GUI_DRV_VER_RES * APP_DEV_GUI_DRV_ZOOM, disp->dpi_value, APP_DEV_GUI_DISP_DPI_DEF);
    AdjustWindowRectEx(&win_size, APP_DEV_GUI_DISP_STY, FALSE, APP_DEV_GUI_DISP_STY_EX);
    OffsetRect(&win_size, -win_size.left, -win_size.top);
    SetWindowPos(disp->window, NULL, 0, 0, win_size.right, win_size.bottom,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
    /* 5.触摸使能 */
    app_dev_gui_disp_reg_touch_window(disp->window, 0);
    app_dev_gui_disp_enable_child_win_dpi_msg(disp->window);
    /* 6.创建帧缓冲区 */
    HDC frame_buf_hdc = app_dev_gui_disp_frame_buffer(disp->window, APP_DEV_GUI_DRV_HOR_RES, APP_DEV_GUI_DRV_VER_RES,
                                                   &disp->pixel_buf,
                                                   &disp->pixel_buf_size);
    DeleteDC(disp->buf_hdc);
    disp->buf_hdc = frame_buf_hdc;
    /* 7.显示更新 */
    ShowWindow(disp->window, SW_SHOW);
    UpdateWindow(disp->window);
}

/*@brief 更新屏幕
 */
static void app_dev_gui_disp_update(app_dev_gui_disp_t *disp)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
}

/*@brief 销毁屏幕
 */
static void app_dev_gui_disp_destroy(app_dev_gui_disp_t *disp)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
}

/*@brief Win屏幕消息回调事件
 */
HRESULT CALLBACK app_dev_gui_disp_msg_cb(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    app_dev_t *driver = app_dev_gui_disp_inst();
    app_dev_gui_disp_cfg_t *cfg = driver->cfg;
    app_dev_gui_disp_data_t *data = driver->data;
    
    switch (uMsg)
    {
    case WM_DPICHANGED: {
        RECT c_rect;
        LPRECT s_rect = (LPRECT)lParam;
        cfg->display.dpi_value = HIWORD(wParam);
        
        SetWindowPos(hWnd, NULL, s_rect->left,
                                 s_rect->top,
                                 s_rect->right,
                                 s_rect->bottom, SWP_NOZORDER | SWP_NOACTIVATE);
        
        GetClientRect(hWnd, &c_rect);
        
        int win_hor = MulDiv(APP_DEV_GUI_DRV_HOR_RES * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF);
        int win_ver = MulDiv(APP_DEV_GUI_DRV_VER_RES * APP_DEV_GUI_DRV_ZOOM, cfg->display.dpi_value, APP_DEV_GUI_DISP_DPI_DEF);
        
        SetWindowPos(hWnd, NULL,
            s_rect->left,
            s_rect->top,
            s_rect->right  + (win_hor - c_rect.right),
            s_rect->bottom + (win_ver - c_rect.bottom), SWP_NOZORDER | SWP_NOACTIVATE);
        
        return TRUE;
    }
    case WM_PAINT: {
        RECT rect;
        PAINTSTRUCT ps;
        HDC win_dc = BeginPaint(hWnd, &ps);
        if (win_dc) {
            rect.top    = MulDiv(ps.rcPaint.top,    APP_DEV_GUI_DISP_DPI_DEF, cfg->display.dpi_value);
            rect.left   = MulDiv(ps.rcPaint.left,   APP_DEV_GUI_DISP_DPI_DEF, cfg->display.dpi_value);
            rect.right  = MulDiv(ps.rcPaint.right,  APP_DEV_GUI_DISP_DPI_DEF, cfg->display.dpi_value);
            rect.bottom = MulDiv(ps.rcPaint.bottom, APP_DEV_GUI_DISP_DPI_DEF, cfg->display.dpi_value);
            
            int prev_mode = SetStretchBltMode(win_dc, HALFTONE);
            
            StretchBlt(win_dc,
                       ps.rcPaint.left,
                       ps.rcPaint.top,
                       ps.rcPaint.right  - ps.rcPaint.left,
                       ps.rcPaint.bottom - ps.rcPaint.top,
                       cfg->display.buf_hdc,
                       rect.left,
                       rect.top,
                       rect.right  - rect.left,
                       rect.bottom - rect.top, SRCCOPY);
            
            SetStretchBltMode(win_dc, prev_mode);
            
            EndPaint(hWnd, &ps);
        }
        return TRUE;
    }
    case WM_DESTROY:
        cfg->quit = true;
        /* 让主流程直接接收进程 */
        /* 该动作交付系统完成 */
        /* 因为还有一段显示内容 */
        // PostQuitMessage(0);
        return TRUE;
    case WM_CLOSE:
        /* 点击窗口右上角关闭会产生该事件 */
        /* 我们在此处拦截它,不让它进一步传入默认处理流程 */
        cfg->quit = true;
        return TRUE;
    case WM_QUIT:
        cfg->quit = true;
        return TRUE;
    }
    return 0;
}

/*@brief Win 屏幕需要关机
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
        app_dev_gui_disp_create(&cfg->display);
    }
    
    /* 反初始化SDL屏幕(不需要) */
    if (false) {
        app_dev_gui_disp_destroy(&cfg->display);
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
