/*实现目标:
 *    图像源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* GIF */
#include "gifdec.h"

/* rlottie */
#include "rlottie_capi.h"

/*@brief 本地内部数据
 *       用于维护帧数据的合理迭代
 */
typedef union {
    struct {
        gd_GIF *gif;
    };
    struct {
        struct Lottie_Animation_S *Animation;
    } rlottie;
} scui_vedio_local_t;

/*@brief 图像帧数据销毁
 *@param vedio 图像帧实例
 */
void scui_vedio_burn(scui_vedio_t *vedio)
{
    SCUI_ASSERT(vedio != NULL);
    scui_vedio_local_t *local = vedio->local;
    
    switch (vedio->type) {
    case scui_image_type_gif: {
        
        /* 销毁GIF的管理器 */
        if (local != NULL && local->gif != NULL)
            gd_close_gif(local->gif);
        
        uintptr_t data_bin = vedio->image.pixel.data_bin;
        SCUI_MEM_FREE((void *)data_bin);
        
        scui_handle_clear(vedio->frame);
        SCUI_MEM_FREE(vedio->data);
        SCUI_MEM_FREE(vedio->local);
        
        vedio->local = NULL;
        vedio->data  = NULL;
        vedio->frame = SCUI_HANDLE_INVALID;
        memset(&vedio->image, 0, sizeof(scui_image_t));
        break;
    }
    case scui_image_type_lottie: {
        
        /* 销毁Lottie的管理器 */
        if (local != NULL && local->rlottie.Animation != NULL)
            lottie_animation_destroy(local->rlottie.Animation);
        
        uintptr_t data_bin = vedio->image.pixel.data_bin;
        SCUI_MEM_FREE((void *)data_bin);
        
        scui_handle_clear(vedio->frame);
        SCUI_MEM_FREE(vedio->data);
        SCUI_MEM_FREE(vedio->local);
        
        vedio->local = NULL;
        vedio->data  = NULL;
        vedio->frame = SCUI_HANDLE_INVALID;
        memset(&vedio->image, 0, sizeof(scui_image_t));
        break;
    }
    default:
        SCUI_LOG_ERROR("unsupport type");
        SCUI_ASSERT(false);
        return;
    }
}

/*@brief 图像帧数据构造
 *@param vedio 图像帧实例
 */
void scui_vedio_make(scui_vedio_t *vedio)
{
    SCUI_ASSERT(vedio != NULL);
    SCUI_ASSERT(vedio->local == NULL);
    scui_vedio_local_t *local = NULL;
    local = SCUI_MEM_ZALLOC(scui_mem_type_mix, sizeof(scui_vedio_local_t));
    vedio->local = local;
    
    switch (vedio->type) {
    case scui_image_type_gif: {
        
        scui_image_t *image = scui_handle_source_check(vedio->handle);
        SCUI_ASSERT(image->type == scui_image_type_gif);
        
        SCUI_ASSERT(vedio->data == NULL);
        vedio->size = image->pixel.size_bin;
        vedio->data = SCUI_MEM_ALLOC(scui_mem_type_graph, vedio->size);
        scui_image_src_read(image, vedio->data);
        
        /* 生成GIF的管理器 */
        local->gif = gd_open_gif_data(vedio->data);
        SCUI_ASSERT(local->gif->width != 0 && local->gif->height != 0);
        /* 为GIF的帧图像开辟资源(注意:解出资源为ARGB8888, 要做一次本地转换到设备使用) */
        uintptr_t size_bin = 4 * local->gif->width * local->gif->height;
        uintptr_t data_bin = SCUI_MEM_ALLOC(scui_mem_type_graph, size_bin);
        vedio->image.type = scui_image_type_mem;
        vedio->image.format = scui_pixel_cf_bmp8888;
        vedio->image.pixel.width  = local->gif->width;
        vedio->image.pixel.height = local->gif->height;
        vedio->image.pixel.data_bin = data_bin;
        vedio->image.pixel.size_bin = size_bin;
        vedio->frame = scui_handle_find();
        scui_handle_linker(vedio->frame, &vedio->image);
        
        /* 固定单次: 只播一轮到尾返回0, 循环更迭由控件层管理 */
        local->gif->loop_count = 1;
        break;
    }
    case scui_image_type_lottie: {
        
        scui_image_t *image = scui_handle_source_check(vedio->handle);
        SCUI_ASSERT(image->type == scui_image_type_lottie);
        
        SCUI_ASSERT(vedio->data == NULL);
        vedio->size = image->pixel.size_bin + 1;
        vedio->data = SCUI_MEM_ALLOC(scui_mem_type_graph, vedio->size);
        scui_image_src_read(image, vedio->data);
        vedio->data[vedio->size - 1] = '\0';
        
        /* 生成Lottie的管理器 */
        local->rlottie.Animation = lottie_animation_from_data(vedio->data, vedio->data, "");
        
        /* 为GIF的帧图像开辟资源(注意:解出资源为ARGB8888, 要做一次本地转换到设备使用) */
        size_t rlottie_width = 0, rlottie_height = 0;
        lottie_animation_get_size(local->rlottie.Animation, &rlottie_width, &rlottie_height);
        SCUI_ASSERT(rlottie_width != 0 && rlottie_height != 0);
        
        uintptr_t size_bin = 4 * rlottie_width * rlottie_height;
        uintptr_t data_bin = SCUI_MEM_ALLOC(scui_mem_type_graph, size_bin);
        vedio->image.type = scui_image_type_mem;
        vedio->image.format = scui_pixel_cf_bmp8888;
        vedio->image.pixel.width  = rlottie_width;
        vedio->image.pixel.height = rlottie_height;
        vedio->image.pixel.data_bin = data_bin;
        vedio->image.pixel.size_bin = size_bin;
        vedio->frame = scui_handle_find();
        scui_handle_linker(vedio->frame, &vedio->image);
        
        /* 更新基础参数 */
        vedio->lottie.frame = lottie_animation_get_totalframe(local->rlottie.Animation);
        vedio->lottie.rate  = lottie_animation_get_framerate(local->rlottie.Animation);
        vedio->lottie.index = 0;
        break;
    }
    default:
        SCUI_LOG_ERROR("unsupport type");
        SCUI_ASSERT(false);
        return;
    }
}

/*@brief 图像帧数据获取
 *@param vedio 图像帧实例
 *@retval 获得有效帧
 */
bool scui_vedio_data(scui_vedio_t *vedio)
{
    SCUI_ASSERT(vedio != NULL);
    scui_vedio_local_t *local = vedio->local;
    
    switch (vedio->type) {
    case scui_image_type_gif: {
        
        /* 获得一个帧, 到达末尾退出 */
        int frame_get = gd_get_frame(local->gif);
        SCUI_ASSERT(frame_get != -1);
        
        /* 获得一个帧数据 */
        gd_render_frame(local->gif, local->gif->canvas);
        
        /* 到尾: 本轮播放结束(已回绕), 由上层更迭循环 */
        if (frame_get == 0)
            return true;
        
        /* 这里不可以直接将canvas使用, 因为GIF帧会依赖之前的帧 */
        uintptr_t size_bin = vedio->image.pixel.size_bin;
        uintptr_t data_bin = vedio->image.pixel.data_bin;
        memcpy((void *)data_bin, local->gif->canvas, size_bin);
        
        #if 0
        /* 帧数据转为本地设备格式(可选) */
        vedio->image.format = scui_pixel_cf_bmp8888;
        scui_image_cf_cvt(&vedio->image, false);
        #endif
        
        return false;
        break;
    }
    case scui_image_type_lottie: {
        
        uint32_t rlottie_width  = vedio->image.pixel.width;
        uint32_t rlottie_height = vedio->image.pixel.height;
        uintptr_t size_bin = vedio->image.pixel.size_bin;
        uintptr_t data_bin = vedio->image.pixel.data_bin;
        
        lottie_animation_render(local->rlottie.Animation, vedio->lottie.index,
            (void *)data_bin, rlottie_width, rlottie_height, rlottie_width * 4);
        
        #if 0
        /* 帧数据转为本地设备格式(可选) */
        vedio->image.format = scui_pixel_cf_bmp8888;
        scui_image_cf_cvt(&vedio->image, false);
        #endif
        
        /* 固定接口一步步向后切, 到尾回绕并返回本轮结束 */
        vedio->lottie.index++;
        if (vedio->lottie.index >= vedio->lottie.frame) {
            vedio->lottie.index = 0;
            return true;
        }
        return false;
        break;
    }
    default:
        SCUI_LOG_ERROR("unsupport type");
        SCUI_ASSERT(false);
        return false;
    }
}
