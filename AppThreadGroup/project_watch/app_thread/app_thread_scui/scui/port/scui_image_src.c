/*实现目标:
 *    图像源
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "scui.h"

/*@brief 图片资源卸载
 *@param image_unit 图片资源缓存节点
 */
void scui_image_src_unload(scui_image_unit_t *image_unit)
{
    image_unit->data = SCUI_MEM_ALLOC(scui_mem_is_image, image_unit->image->pixel.size_mem);
    // 使用image_uint->image生成==>image_unit->data
}

/*@brief 图片资源加载
 *@param image_unit 图片资源缓存节点
 */
void scui_image_src_load(scui_image_unit_t *image_unit)
{
    // 回收image_unit->data
    SCUI_MEM_FREE(image_unit->data);
    image_unit->data = NULL;
}

/*@brief 图片资源检查
 *@param image 图片资源
 */
bool scui_image_src_check(scui_image_t *image)
{
    /* 有必要就可以做 */
    return true;
}
