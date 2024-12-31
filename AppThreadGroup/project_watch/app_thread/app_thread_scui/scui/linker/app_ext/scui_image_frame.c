/*实现目标:
 *    图像源
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

// GIF
#include "gifdec.h"

/*@brief 本地内部数据
 *       用于维护帧数据的合理迭代
 */
typedef union {
    struct {
        gd_GIF *gif;
    };
} scui_image_frame_local_t;

/*@brief 帧数据格式转换(就地转换)
 */
static void scui_image_frame_gif_cvt_cf(void *dec, uint32_t pixel_cnt)
{
    scui_color8888_t *argb8888 = dec;
    
    for(uint32_t idx = 0; idx < pixel_cnt; idx++) {
        
        scui_color8565_t  png_pixel = {0};
        png_pixel.ch.a = argb8888[idx].byte[3];
        png_pixel.ch.r = argb8888[idx].byte[2] >> 3;
        png_pixel.ch.g = argb8888[idx].byte[1] >> 2;
        png_pixel.ch.b = argb8888[idx].byte[0] >> 3;
        
        scui_color8565_t *pixel = (void *)((uint8_t *)dec + 3 * idx);
        *pixel = png_pixel;
    }
}

/*@brief 图像帧数据销毁
 *@param image_frame 图像帧实例
 */
void scui_image_frame_burn(scui_image_frame_t *image_frame)
{
    SCUI_ASSERT(image_frame != NULL);
    scui_image_frame_local_t *local = image_frame->local;
    
    switch (image_frame->type) {
    case scui_image_type_gif: {
        
        // 生成GIF的管理器
        if (local != NULL && local->gif != NULL)
            gd_close_gif(local->gif);
        
        uintptr_t data_bin = image_frame->image.pixel.data_bin;
        SCUI_MEM_FREE((void *)data_bin);
        
        scui_handle_set(image_frame->frame, NULL);
        SCUI_MEM_FREE(image_frame->data);
        SCUI_MEM_FREE(image_frame->local);
        
        image_frame->local = NULL;
        image_frame->data  = NULL;
        image_frame->frame = SCUI_HANDLE_INVALID;
        memset(&image_frame->image, 0, sizeof(scui_image_t));
        break;
    }
    default:
        SCUI_LOG_ERROR("unsupport type");
        SCUI_ASSERT(false);
        return;
    }
}

/*@brief 图像帧数据构造
 *@param image_frame 图像帧实例
 */
void scui_image_frame_make(scui_image_frame_t *image_frame)
{
    SCUI_ASSERT(image_frame != NULL);
    SCUI_ASSERT(image_frame->local == NULL);
    scui_image_frame_local_t *local = NULL;
    local = SCUI_MEM_ALLOC(scui_mem_type_mix, sizeof(scui_image_frame_local_t));
    memset(local, 0, sizeof(scui_image_frame_local_t));
    image_frame->local = local;
    
    switch (image_frame->type) {
    case scui_image_type_gif: {
        
        scui_image_t *image = scui_handle_get(image_frame->handle);
        SCUI_ASSERT(image != NULL && image->type == scui_image_type_gif);
        
        SCUI_ASSERT(image_frame->data == NULL);
        image_frame->size = image->pixel.size_bin;
        image_frame->data = SCUI_MEM_ALLOC(scui_mem_type_graph, image_frame->size);
        scui_image_src_read(image, image_frame->data);
        
        // 生成GIF的管理器
        local->gif = gd_open_gif_data(image_frame->data);
        // 为GIF的帧图像开辟资源(注意:解出资源为ARGB8888, 要做一次本地转换到设备使用)
        uintptr_t size_bin = 4 * local->gif->width * local->gif->height;
        uintptr_t data_bin = SCUI_MEM_ALLOC(scui_mem_type_graph, size_bin);
        image_frame->image.type = scui_image_type_mem;
        image_frame->image.format = scui_pixel_cf_bmp8565;
        image_frame->image.pixel.width  = local->gif->width;
        image_frame->image.pixel.height = local->gif->height;
        image_frame->image.pixel.data_bin = data_bin;
        image_frame->image.pixel.size_bin = size_bin;
        image_frame->frame = scui_handle_find();
        scui_handle_set(image_frame->frame, &image_frame->image);
        
        local->gif->loop_count = image_frame->loop;
        
        break;
    }
    default:
        SCUI_LOG_ERROR("unsupport type");
        SCUI_ASSERT(false);
        return;
    }
}

/*@brief 图像帧数据获取
 *@param image_frame 图像帧实例
 *@retval 获得有效帧
 */
bool scui_image_frame_data(scui_image_frame_t *image_frame)
{
    SCUI_ASSERT(image_frame != NULL);
    scui_image_frame_local_t *local = image_frame->local;
    
    switch (image_frame->type) {
    case scui_image_type_gif: {
        
        // 获得一个帧, 到达末尾退出
        int frame_get = gd_get_frame(local->gif);
        SCUI_ASSERT(frame_get != -1);
        
        // 获得一个帧数据
        gd_render_frame(local->gif, local->gif->canvas);
        
        if (frame_get == 0)
            return false;
        
        // 这里不可以直接将canvas使用, 因为GIF帧会依赖之前的帧
        uintptr_t size_bin = image_frame->image.pixel.size_bin;
        uintptr_t data_bin = image_frame->image.pixel.data_bin;
        memcpy((void *)data_bin, local->gif->canvas, size_bin);
        
        // 帧数据转为本地设备格式
        uint32_t pixel_cnt = local->gif->width * local->gif->height;
        scui_image_frame_gif_cvt_cf((void *)data_bin, pixel_cnt);
        
        return true;
        break;
    }
    default:
        SCUI_LOG_ERROR("unsupport type");
        SCUI_ASSERT(false);
        return false;
    }
}
