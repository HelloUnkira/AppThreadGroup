/*实现目标:
 *    lvgl事件
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     1   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_lv_lib.h"

/*@brief 点击返回上一层
 */
void app_lv_event_ui_backtrack_cb(lv_event_t *e)
{
    switch (lv_event_get_code(e)) {
    case LV_EVENT_CLICKED: {
        /* 轮盘复位 */
        if (app_lv_wheel_rollback(1, LV_DIR_RIGHT) ||
            app_lv_wheel_rollback(1, LV_DIR_LEFT))
            break;
        app_lv_scene_t *scene = NULL;
        app_lv_scene_del(&scene);
        break;
    }
    default:
        break;
    break;
    }
}

/*@brief 滚轮渐变蒙层回调
 */
void app_lv_event_ui_roller_mask_cb(lv_event_t * e)
{
    lv_obj_t *obj = lv_event_get_target(e);
    /* 静态蒙层索引号 */
    static int16_t roller_mask_id_t = -1;   /* 顶部蒙层索引号 */
    static int16_t roller_mask_id_b = -1;   /* 底部蒙层索引号 */
    /* 处理交互事件 */
    switch (lv_event_get_code(e)) {
    /* 覆盖检查事件 */
    case LV_EVENT_COVER_CHECK: {
        lv_event_set_cover_res(e, LV_COVER_RES_MASKED);
        break;
    }
    /* 控件绘制开始事件 */
    case LV_EVENT_DRAW_MAIN_BEGIN: {
        /* 提取绘制域信息 */
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t line_space  = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_coord_t line_height = lv_font_get_line_height(font);
        /* 定位滚轮坐标 */
        lv_area_t roller_coords;
        lv_obj_get_coords(obj, &roller_coords);
        /* 重定位上部蒙层绘制域 */
        lv_area_t rect_area;
        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;
        rect_area.y1 = roller_coords.y1;
        rect_area.y2 = roller_coords.y1 + (lv_obj_get_height(obj) - line_height - line_space) / 2;
        /* 创建上部渐变蒙层 */
        lv_draw_mask_fade_param_t * fade_mask_t = lv_mem_alloc(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_t, &rect_area, LV_OPA_TRANSP, rect_area.y1, LV_OPA_COVER, rect_area.y2);
        roller_mask_id_t = lv_draw_mask_add(fade_mask_t, NULL);
        /* 重定位下部蒙层绘制域 */
        rect_area.x1 = roller_coords.x1;
        rect_area.x2 = roller_coords.x2;
        rect_area.y1 = rect_area.y2 + line_height + line_space - 1;
        rect_area.y2 = roller_coords.y2;
        /* 创建下部渐变蒙层 */
        lv_draw_mask_fade_param_t * fade_mask_b = lv_mem_alloc(sizeof(lv_draw_mask_fade_param_t));
        lv_draw_mask_fade_init(fade_mask_b, &rect_area, LV_OPA_COVER, rect_area.y1, LV_OPA_TRANSP, rect_area.y2);
        roller_mask_id_b = lv_draw_mask_add(fade_mask_b, NULL);
        break;
    }
    /* 控件绘制结束推送事件 */
    case LV_EVENT_DRAW_POST_END: {
        lv_draw_mask_fade_param_t * fade_mask_t = lv_draw_mask_remove_id(roller_mask_id_t);
        lv_draw_mask_fade_param_t * fade_mask_b = lv_draw_mask_remove_id(roller_mask_id_b);
//        lv_draw_mask_free_param(fade_mask_t);
//        lv_draw_mask_free_param(fade_mask_b);
        lv_mem_free(fade_mask_t);
        lv_mem_free(fade_mask_b);
        roller_mask_id_t = -1;
        roller_mask_id_b = -1;
        break;
    }
    default:
        break;
    }
}

/*@brief 折线图表下部渐变回调
 */
void app_lv_event_ui_chart_fade_cb(lv_event_t * e)
{
    lv_obj_t * obj = lv_event_get_target(e);
    /* 处理交互事件 */
    switch (lv_event_get_code(e)) {
    /* 控件绘制开始事件 */
    case LV_EVENT_DRAW_PART_BEGIN: {
        /* 获取事件绘制源 */
        lv_obj_draw_part_dsc_t * draw_part_dsc = lv_event_get_draw_part_dsc(e);
        /* 在绘制线条之前添加褪色区域 */
        if (draw_part_dsc->part == LV_PART_ITEMS) {
            /*  */
            if (draw_part_dsc->p1 == 0 || draw_part_dsc->p2 == 0)
                return;
            /* 使用绘制蒙版覆盖出渐变效果 */
            lv_coord_t height = lv_obj_get_height(obj);
            lv_draw_mask_line_param_t line_mask_param = {0};
            lv_draw_mask_fade_param_t fade_mask_param = {0};
            /* 添加一个线条蒙版,保持线条下方的区域 */
            lv_draw_mask_line_points_init(&line_mask_param,
                                          draw_part_dsc->p1->x,
                                          draw_part_dsc->p1->y,
                                          draw_part_dsc->p2->x,
                                          draw_part_dsc->p2->y, LV_DRAW_MASK_LINE_SIDE_BOTTOM);
            /* 添加一个渐隐蒙版,透明底覆盖顶部 */
            lv_draw_mask_fade_init(&fade_mask_param, &obj->coords,
                                    LV_OPA_COVER,  obj->coords.y1 + height / 8,
                                    LV_OPA_TRANSP, obj->coords.y2);
            /* 添加绘制蒙版 */
            int16_t line_mask_id = lv_draw_mask_add(&line_mask_param, NULL);
            int16_t fade_mask_id = lv_draw_mask_add(&fade_mask_param, NULL);
            /* 绘制一个受蒙版影响的矩形 */
            lv_draw_rect_dsc_t draw_rect_dsc = {0};
            lv_draw_rect_dsc_init(&draw_rect_dsc);
            draw_rect_dsc.bg_color = draw_part_dsc->line_dsc->color;
            draw_rect_dsc.bg_opa = LV_OPA_COVER;
            /* 计算绘制域 */
            lv_area_t area = {
                .x1 = draw_part_dsc->p1->x,
                .x2 = draw_part_dsc->p2->x - 1,
                .y1 = LV_MIN(draw_part_dsc->p1->y, draw_part_dsc->p2->y),
                .y2 = obj->coords.y2,
            };
            #if 1
            /* 绘制区域 */
            lv_draw_rect(draw_part_dsc->draw_ctx, &draw_rect_dsc, &area);
            #else
            /* 绘制区域 */
            lv_draw_rect(&area, draw_part_dsc->clip_area, &draw_rect_dsc);
            #endif
            /* 移除绘制蒙版 */
            lv_draw_mask_free_param(&line_mask_param);
            lv_draw_mask_free_param(&fade_mask_param);
            lv_draw_mask_remove_id(line_mask_id);
            lv_draw_mask_remove_id(fade_mask_id);
        }
        /* 刷新背景图的分割线 */
        if(draw_part_dsc->part == LV_PART_MAIN) {
            /*  */
            if (draw_part_dsc->line_dsc == NULL || draw_part_dsc->p1 == NULL || draw_part_dsc->p2 == NULL)
                return;
            /* 垂直线 */
            if (draw_part_dsc->p1->x == draw_part_dsc->p2->x) {
                /* 虚线分界线,根据draw_part_dsc->id区分是哪一条虚线,这里统一不使用 */
                // if (draw_part_dsc->id == 0);
                draw_part_dsc->line_dsc->width = 1;
                draw_part_dsc->line_dsc->dash_gap = 1;
                draw_part_dsc->line_dsc->dash_width = 1;
                draw_part_dsc->line_dsc->color = lv_palette_main(LV_PALETTE_GREY);
                draw_part_dsc->line_dsc->opa = LV_OPA_COVER;
            /* 水平线 */
            } else {
                /* 虚线分界线,根据draw_part_dsc->id区分是哪一条虚线,这里统一不使用 */
                // if (draw_part_dsc->id == 0);
                draw_part_dsc->line_dsc->width = 1;
                draw_part_dsc->line_dsc->dash_gap = 1;
                draw_part_dsc->line_dsc->dash_width = 1;
                draw_part_dsc->line_dsc->color = lv_palette_main(LV_PALETTE_GREY);
                draw_part_dsc->line_dsc->opa = LV_OPA_COVER;
            }
        }
        break;
    }
    default:
        break;
    }
}
