/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 全局绘图资源(内部资源, 内部使用): */
static scui_surface_t scui_draw_surface_tvg;

/* 检查配置: */
#include "scui_draw_thorvg.h"
#if SCUI_DRAW_USE_THORVG != SCUI_DRAW_USE_THORVG_SRC
#error "draw engine unmatch config"
#endif

/* 矢量绘图引擎(thorvg): */
#include "thorvg_capi.h"

#if SCUI_DRAW_USE_THORVG

/* 就绪canvas画布实例 */
static Tvg_Canvas * scui_draw_tvg_canvas_ready(void)
{
    scui_surface_t *tvg_surface = &scui_draw_surface_tvg;
    scui_area_t tvg_area = scui_surface_area(tvg_surface);
    
    /* 固定:创建画布并绑定资源 */
    Tvg_Canvas *canvas = tvg_swcanvas_create();
    tvg_swcanvas_set_target(canvas, tvg_surface->pixel, tvg_surface->hor_res,
        tvg_surface->hor_res, tvg_surface->ver_res, TVG_COLORSPACE_ARGB8888S);
    /* 固定:设置画布绘制剪切域 */
    tvg_canvas_set_viewport(canvas, 0, 0, tvg_area.w, tvg_area.h);
    /* 固定:清空画布绘制剪切域底图(填成全透明黑底) */
    scui_draw_area_fill(true, tvg_surface, tvg_area,
        scui_alpha_cover, SCUI_COLOR_ZEROED);
    
    /*  */
    return canvas;
}

/* 绘制调度销毁canvas画布实例 */
static void scui_draw_tvg_canvas_sched(Tvg_Canvas *canvas)
{
    /* 固定:更新画布 */
    tvg_canvas_update(canvas);
    /* 固定:绘制并同步 */
    if (tvg_canvas_draw(canvas) == TVG_RESULT_SUCCESS)
        tvg_canvas_sync(canvas);
    /* 固定:清空画笔 */
    tvg_canvas_clear(canvas, true);
    /* 固定:关闭画布 */
    tvg_canvas_destroy(canvas);
}

/*@brief 矢量引擎回调
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_tvg(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->graph.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->graph.dst_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->graph.src_alpha;
    
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    #if SCUI_DRAW_USE_THORVG
    scui_surface_t *tvg_surface = &scui_draw_surface_tvg;
    scui_area_t tvg_area = scui_surface_area(tvg_surface);
    Tvg_Canvas *canvas = scui_draw_tvg_canvas_ready();
    
    draw_dsc->graph.src_tvg_canvas = canvas;
    draw_dsc->graph.src_tvg_offset.x = dst_clip->x;
    draw_dsc->graph.src_tvg_offset.y = dst_clip->y;
    draw_dsc->graph.src_tvg_cb(draw_dsc);
    
    /* 固定:画布绘制 */
    scui_draw_tvg_canvas_sched(canvas);
    
    /* 固定:资源获取绘制 */
    scui_draw_area_blend(true, dst_surface, *dst_clip,
        &scui_draw_surface_tvg, tvg_area, SCUI_COLOR_UNUSED);
    
    #endif
}

#endif

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 *@retval 支持度(true:支持;false:不支持;)
 */
static bool scui_draw_ctx_graph_part_TVG(scui_draw_dsc_t *draw_dsc)
{
    #if SCUI_DRAW_USE_THORVG
    if (draw_dsc->type == scui_draw_type_pixel_tvg) {
        scui_draw_tvg(draw_dsc);
        return true;
    }
    #endif
    
    return false;
}

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 *@retval 支持度(true:支持;false:不支持;)
 */
bool scui_draw_ctx_graph_TVG(scui_draw_dsc_t *draw_dsc)
{
    #if SCUI_DRAW_USE_THORVG
    scui_surface_t *dst_surface = draw_dsc->graph.dst_surface;
    scui_area_t dst_clip_reb = draw_dsc->graph.dst_clip;
    scui_area_t dst_clip = draw_dsc->graph.dst_clip;
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter2(&dst_clip, &dst_area))
         return false;
    
    /* 计算当前可承载最大剪切域 */
    scui_coord_t tvg_item = scui_min(SCUI_HOR_RES, dst_clip.w);
    scui_coord_t tvg_line = SCUI_DRAW_CACHE_THORVG / tvg_item;
    tvg_line = scui_min(tvg_line, dst_clip.h);
    
    /* 更新绘制画布的实际宽高 */
    scui_surface_t *tvg_surface = &scui_draw_surface_tvg;
    tvg_surface->hor_res = tvg_item;
    tvg_surface->ver_res = tvg_line;
    tvg_surface->stride  = tvg_surface->hor_res * tvg_surface->pbyte;
    
    /* 完全绘制: */
    bool retval = true;
    if (tvg_item == dst_clip.w && tvg_line == dst_clip.h) {
        draw_dsc->graph.dst_clip = dst_clip;
        retval = scui_draw_ctx_graph_part_TVG(draw_dsc);
        draw_dsc->graph.dst_clip = dst_clip_reb;
        return retval;
    }
    
    SCUI_LOG_INFO("cache block draw");
    /* 分块绘制:根据cache最大资源将dst_clip拆分成多个块 */
    scui_coord_t tvg_items = dst_clip.w / tvg_item + 1;
    scui_coord_t tvg_lines = dst_clip.h / tvg_line + 1;
    if (tvg_item == dst_clip.w) tvg_items = 1;
    if (tvg_line == dst_clip.h) tvg_lines = 1;
    
    for (scui_coord_t idx_line = 0; idx_line < tvg_lines; idx_line++)
    for (scui_coord_t idx_item = 0; idx_item < tvg_lines; idx_item++) {
        draw_dsc->graph.dst_clip.y = dst_clip.y + idx_line * tvg_line;
        draw_dsc->graph.dst_clip.x = dst_clip.x + idx_item * tvg_item;
        
        /* 最后剩余的行列单独计算实际宽高(rest) */
        scui_coord_t clip_h = 0; bool line_r = idx_line == tvg_lines - 1;
        scui_coord_t clip_w = 0; bool item_r = idx_item == tvg_items - 1;
        clip_h = line_r ? dst_clip.h - idx_line * tvg_line : tvg_line;
        clip_w = item_r ? dst_clip.w - idx_item * tvg_item : tvg_item;
        draw_dsc->graph.dst_clip.h = clip_h;
        draw_dsc->graph.dst_clip.w = clip_w;
        
        retval |= scui_draw_ctx_graph_part_TVG(draw_dsc);
    }
    
    /* 此处还原(不确定有没有意义) */
    draw_dsc->graph.dst_clip = dst_clip_reb;
    return retval;
    #endif
}

/*@brief 就绪绘图引擎
 */
void scui_draw_tvg_ready(void)
{
    #if SCUI_DRAW_USE_THORVG
    tvg_engine_init(TVG_ENGINE_SW, 0);
    /* 一旦启用, 则永恒常驻, 无需释放 */
    /* tvg_engine_term(TVG_ENGINE_SW); */
    
    scui_surface_t *tvg_surface = &scui_draw_surface_tvg;
    tvg_surface->format = scui_pixel_cf_bmp8888;
    tvg_surface->pbyte  = scui_pixel_byte(tvg_surface->format);
    tvg_surface->alpha  = scui_alpha_cover;
    
    scui_multi_t tvg_pixel_num  = SCUI_DRAW_CACHE_THORVG;
    scui_coord_t tvg_pixel_rem  = sizeof(scui_color_wt_t) - tvg_surface->pbyte;
    scui_multi_t tvg_pixel_size = tvg_pixel_num * tvg_surface->pbyte + tvg_pixel_rem;
    tvg_surface->pixel = SCUI_MEM_ALLOC(scui_mem_type_graph, tvg_pixel_size);
    
    /* 画布宽高在绘制过程中决定 */
    tvg_surface->hor_res = 0;
    tvg_surface->ver_res = 0;
    #endif
}
