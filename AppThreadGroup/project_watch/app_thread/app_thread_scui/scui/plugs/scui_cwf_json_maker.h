#ifndef SCUI_CWF_JSON_MAKER_H
#define SCUI_CWF_JSON_MAKER_H

/* 使用一个通用结构去描述所有item */
/* 如果谁缺字段可自行额外补充即可 */
typedef struct {
    /* linker */
    scui_cwf_json_parser_t *parser;
    scui_handle_t list_idx;     /* 列表索引 */
    /* 协议字段 */
    scui_handle_t type;         /* 渲染类别 */
    scui_handle_t source;       /* 数据源 */
    scui_handle_t key;          /* 元素角色 */
    scui_handle_t align;        /* 对齐 */
    scui_coord_t  align_ofs;    /* 对齐偏移 */
    scui_handle_t child;        /* key:layout子元素数量 */
    scui_handle_t nums;         /* seq场宽(格数,0:贴内容) */
    scui_coord_t  span;         /* 像素间隙 */
    scui_handle_t anima_ms;     /* 帧动画间隔 */
    /* 区域 */
    scui_coord_t area_x;        /* 区域点x */
    scui_coord_t area_y;        /* 区域点y */
    scui_coord_t area_w;        /* 区域宽度 */
    scui_coord_t area_h;        /* 区域高度 */
    /* 图集 */
    scui_coord_t  img_w;        /* 参考图宽 */
    scui_coord_t  img_h;        /* 参考图高 */
    scui_handle_t img_num;      /* 图数量 */
    uint16_t     *img_res;      /* 图集表 */
    /* 字体 */
    scui_handle_t font;         /* 字体句柄 */
    scui_handle_t font_res;     /* 字体下标 */
    scui_coord_t  font_size;    /* 字库尺寸 */
    scui_sbitfd_t font_type:2;  /* 字体类型 */
    /* seq文本格式 */
    scui_handle_t fixed;        /* seq固定位数(0:自然位数) */
    uint8_t       format[8];    /* seq格式匹配(fixed构造) */
    /* 运行时 */
    scui_handle_t idx_num;      /* 当前位数 */
    scui_handle_t idx_anim;     /* 当前帧 */
    scui_multi_t  anim_tick;    /* 动画累加 */
    scui_coord_t  seq_val_l;    /* seq上次值 */
    scui_sbitfd_t seq_init:1;   /* seq首刷标志 */
} scui_cwf_json_item_res_t;

#define SCUI_CWF_JSON_SEQ_MAX       (10)
#define SCUI_CWF_JSON_ANIMA_DEF     (137)
#define SCUI_CWF_JSON_ANIMA_REFR    (100)

#endif
