#ifndef SCUI_CWF_JSON_MAKER_H
#define SCUI_CWF_JSON_MAKER_H

/* 使用一个通用结构去描述所有item */
/* 如果谁缺字段可自行额外补充即可 */
typedef struct {
    /* linker */
    scui_cwf_json_parser_t *parser;
    uint32_t list_idx;
    /* 协议字段 */
    uint8_t  type;                              /* 渲染类别 */
    uint8_t  source;                            /* 数据源 */
    uint8_t  key;                               /* 元素角色 */
    uint8_t  align;                             /* 对齐(协议对齐表下标) */
    int8_t   align_ofs;                         /* 对齐参照目标索引偏移 */
    uint16_t child;                             /* key:layout 子元素数量 */
    uint16_t nums;                              /* seq场宽(格数,0=贴内容) */
    uint16_t span;                              /* 像素间隙 */
    uint16_t anima_ms;                          /* 帧动画间隔 */
    /* 区域 */
    scui_coord_t area_x;
    scui_coord_t area_y;
    scui_coord_t area_w;
    scui_coord_t area_h;
    /* 图集 */
    uint16_t  img_num;
    uint16_t *img_res;
    scui_coord_t img_w;
    scui_coord_t img_h;
    /* 运行时 */
    uint16_t  idx_num;
    uint16_t  idx_anim;
    scui_multi_t anim_tick;
} scui_cwf_json_item_res_t;

#define SCUI_CWF_JSON_SEQ_MAX                       (10)
#define SCUI_CWF_JSON_ANIMA_DEF                     (137)
#define SCUI_CWF_JSON_ANIMA_REFR                    (100)

#endif
