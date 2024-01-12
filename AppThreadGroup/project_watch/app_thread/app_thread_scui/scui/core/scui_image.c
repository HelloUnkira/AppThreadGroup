/*实现目标:
 *    图像处理
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 图片合法性检查
 *@param image 图片信息
 *@retval 合法或不合法
 */
bool scui_image_check(scui_image_t *image)
{
    bool check = true;
    
    if (image->format != scui_image_format_p4 &&
        image->format != scui_image_format_rgb565 &&
        image->format != scui_image_format_argb8565)
        check = false;
    if (image->status > scui_image_status_num)
        check = false;
    
    /* 图片不在内存中,做一次检查 */
    check  = check && scui_image_src_check(image);
    
    if (!check) {
        APP_SYS_LOG_ERROR("image info check error:");
        APP_SYS_LOG_ERROR_RAW("< format:%x",        image->format);
        APP_SYS_LOG_ERROR_RAW("- status:%x",        image->status);
        APP_SYS_LOG_ERROR_RAW("- width:%x",         image->pixel.width);
        APP_SYS_LOG_ERROR_RAW("- height:%x",        image->pixel.height);
        APP_SYS_LOG_ERROR_RAW("- data_raw:%x",      image->pixel.data_raw);
        APP_SYS_LOG_ERROR_RAW("- data_mem:%x",      image->pixel.data_mem);
        APP_SYS_LOG_ERROR_RAW("- size_raw:%x",      image->pixel.size_raw);
        APP_SYS_LOG_ERROR_RAW("- size_mem:%x >",    image->pixel.size_mem);
    }
    return check;
}
