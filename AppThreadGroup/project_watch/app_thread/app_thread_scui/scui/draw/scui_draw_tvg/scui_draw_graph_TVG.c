/*实现目标:
 *    基础图元绘制
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 检查配置: */
#if SCUI_DRAW_USE_THORVG != SCUI_DRAW_USE_THORVG_SRC
#error "draw engine unmatch config"
#endif

/* 矢量绘图引擎(thorvg): */
#include "thorvg_capi.h"

#if SCUI_DRAW_USE_THORVG

/* 将canvas画布资源对齐成目标画布尺寸 */
static bool scui_draw_tvg_canvas_switch(scui_surface_t *dst_surface, scui_area_t *dst_clip)
{
    SCUI_ASSERT(dst_surface != NULL && dst_clip != NULL);
    
    scui_surface_t *tvg_surface = &scui_draw_graph.tvg_surface;
    scui_coord_t    tvg_hor_res =  scui_draw_graph.tvg_hor_res;
    scui_coord_t    tvg_ver_res =  scui_draw_graph.tvg_ver_res;
    
    /* 像素资源太少, 不支持同步, 当次舍弃 */
    if (tvg_hor_res * tvg_ver_res < dst_surface->hor_res * dst_clip->h)
        return false;
    
    /* 更新画布的当前区域 */
    tvg_surface->hor_res = dst_surface->hor_res;
    tvg_surface->ver_res = dst_clip->h;
    return true;
}

/* 生成canvas画布实例 */
static Tvg_Canvas * scui_draw_tvg_canvas_ready(scui_area_t *draw_area)
{
    SCUI_ASSERT(draw_area != NULL);
    scui_surface_t *tvg_surface = &scui_draw_graph.tvg_surface;
    
    /* 固定:创建画布并绑定资源 */
    Tvg_Canvas *canvas = tvg_swcanvas_create();
    tvg_swcanvas_set_target(canvas, tvg_surface->pixel, tvg_surface->hor_res,
        tvg_surface->hor_res, tvg_surface->ver_res, TVG_COLORSPACE_ARGB8888S);
    /* 固定:设置画布绘制剪切域 */
    tvg_canvas_set_viewport(canvas,
        draw_area->x, draw_area->y,
        draw_area->w, draw_area->h);
    /* 固定:清空画布绘制剪切域底图(填成全透明黑底) */
    scui_draw_area_fill(true, tvg_surface, *draw_area,
        scui_alpha_cover, SCUI_COLOR_ZEROED);
    
    /*  */
    return canvas;
}

/* 销毁canvas画布实例 */
static void scui_draw_tvg_canvas_sched(Tvg_Canvas *canvas)
{
    /* 固定:绘制并同步 */
    if (tvg_canvas_draw(canvas) == TVG_RESULT_SUCCESS)
        tvg_canvas_sync(canvas);
    /* 固定:关闭画布 */
    tvg_canvas_destroy(canvas);
}
#endif

/*@brief 线条绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_line(scui_draw_dsc_t *draw_dsc)
{
    /* draw dsc args<s> */
    scui_surface_t *dst_surface =  draw_dsc->graph.dst_surface;
    scui_area_t    *dst_clip    = &draw_dsc->graph.dst_clip;
    scui_alpha_t    src_alpha   =  draw_dsc->graph.src_alpha;
    scui_color_t    src_color   =  draw_dsc->graph.src_color;
    scui_coord_t    src_width   =  draw_dsc->graph.src_width;
    scui_point_t    src_pos_1   =  draw_dsc->graph.src_pos_1;
    scui_point_t    src_pos_2   =  draw_dsc->graph.src_pos_2;
    /* draw dsc args<e> */
    /* */
    SCUI_ASSERT(dst_surface != NULL && dst_surface->pixel != NULL && dst_clip != NULL);
    
    if (src_alpha == scui_alpha_trans)
        return;
    
    if (src_width <= 0)
        src_width  = 1;
    
    scui_area_t dst_clip_v = {0};   /* v:vaild */
    scui_area_t dst_area = scui_surface_area(dst_surface);
    if (!scui_area_inter(&dst_clip_v, &dst_area, dst_clip))
         return;
    
    scui_area_t draw_area = {0};
    draw_area.w = dst_clip_v.w;
    draw_area.h = dst_clip_v.h;
    
    #if SCUI_DRAW_USE_THORVG
    if (scui_draw_tvg_canvas_switch(dst_surface, &dst_clip_v)) {
        Tvg_Canvas *canvas = scui_draw_tvg_canvas_ready(&draw_area);
        
        /* 固定:取用画笔并配置信息 */
        Tvg_Paint *paint = tvg_shape_new();
        tvg_paint_set_opacity(paint, src_alpha);
        tvg_shape_set_stroke_cap(paint, TVG_STROKE_CAP_ROUND);
        tvg_shape_set_stroke_width(paint, src_width);
        tvg_shape_set_stroke_color(paint,
            src_color.color.ch.r, src_color.color.ch.g,
            src_color.color.ch.b, src_color.color.ch.a);
        tvg_shape_move_to(paint, src_pos_1.x - dst_clip_v.x, src_pos_1.y - dst_clip_v.y);
        tvg_shape_line_to(paint, src_pos_2.x - dst_clip_v.x, src_pos_2.y - dst_clip_v.y);
        tvg_canvas_push(canvas, paint);
        
        scui_draw_tvg_canvas_sched(canvas);
        
        /* 固定:资源获取绘制 */
        scui_draw_area_blend(true, dst_surface, dst_clip_v,
            &scui_draw_graph.tvg_surface, draw_area, SCUI_COLOR_UNUSED);
    }
    #endif
}

/*@brief 圆绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_circle(scui_draw_dsc_t *draw_dsc)
{

}

/*@brief 圆角矩形绘制
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_crect(scui_draw_dsc_t *draw_dsc)
{

}

/*@brief 圆角矩形阴影绘制
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_shadow(scui_draw_dsc_t *draw_dsc)
{

}

/*@brief 弧绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 */
static void scui_draw_arc(scui_draw_dsc_t *draw_dsc)
{

}

/*@brief 基础图元绘制(抗锯齿)
 *@param draw_dsc 绘制描述符实例
 *@retval 支持度(true:支持;false:不支持;)
 */
bool scui_draw_ctx_graph_TVG(scui_draw_dsc_t *draw_dsc)
{
    #if SCUI_DRAW_USE_THORVG
    switch (draw_dsc->type) {
    case scui_draw_type_pixel_line:
        scui_draw_line(draw_dsc);
        return true;
    case scui_draw_type_pixel_circle:
        scui_draw_circle(draw_dsc);
        return false;
    case scui_draw_type_pixel_crect:
        scui_draw_crect(draw_dsc);
        return false;
    case scui_draw_type_pixel_shadow:
        scui_draw_shadow(draw_dsc);
        return false;
    case scui_draw_type_pixel_arc:
        scui_draw_arc(draw_dsc);
        return false;
    /* 继续补充,剩下的不移植了, 一般用不上了 */
    default:
        SCUI_LOG_ERROR("unknown type:%d", draw_dsc->type);
        return false;
    }
    #endif
    
    return false;
}

/*@brief 就绪绘图引擎
 */
void scui_draw_thorvg_ready(void)
{
    #if SCUI_DRAW_USE_THORVG
    tvg_engine_init(TVG_ENGINE_SW, 0);
    /* 一旦启用, 则永恒常驻, 无需释放 */
    /* tvg_engine_term(TVG_ENGINE_SW); */
    
    scui_coord_t tvg_surface_byte = scui_pixel_bits(scui_pixel_cf_bmp8888) / 8;
    scui_coord_t tvg_surface_rem  = sizeof(scui_color_wt_t) - tvg_surface_byte;
    scui_multi_t tvg_surface_size = tvg_surface_byte * SCUI_HOR_RES * SCUI_VER_RES + tvg_surface_rem;
    
    /* 奢侈的给它开辟一块巨无霸空间 */
    /* 只有保证完全在该区域内绘图才支持 */
    scui_draw_graph.tvg_surface.pixel   = SCUI_MEM_ALLOC(scui_mem_type_graph, tvg_surface_size);
    scui_draw_graph.tvg_surface.format  = scui_pixel_cf_bmp8888;
    scui_draw_graph.tvg_surface.hor_res = SCUI_HOR_RES;
    scui_draw_graph.tvg_surface.ver_res = SCUI_VER_RES;
    scui_draw_graph.tvg_surface.alpha   = scui_alpha_cover;
    
    /* 保留原始的宽高 */
    scui_draw_graph.tvg_hor_res = scui_draw_graph.tvg_surface.hor_res;
    scui_draw_graph.tvg_ver_res = scui_draw_graph.tvg_surface.ver_res;
    #endif
}
