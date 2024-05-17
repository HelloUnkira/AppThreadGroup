字库以其文字绘制这里不做额外实现
字库与图形框架本身分割且依赖度较低
通常来说可以通过移植改造lvgl等开源框架去实现该目标
这样子也可以复用对应框架的字库生成工具
字库本身只为获取点阵字符的数据流



#if 0

// 最终只需要完成该目标即可
// 因为它本身只需要在固定区域绘制固定内容
void scui_draw_string(scui_surface_t *dst_surface, scui_area_t *dst_clip,
                      scui_area_t *src_clip, scui_point_t *offset, char *string)
{
    // 1.文字预处理(变形,连字符,等等...)
    // 2.排版布局更新
    // 3.循环取字符绘制
}

// 一些辅助接口如:
// 单行,多行:计算总宽度,总高度
scui_string_area_calc

#endif


