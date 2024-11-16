#ifndef SCUI_CWF_JSON_MAKER_H
#define SCUI_CWF_JSON_MAKER_H

// 使用一个通用结构去描述所有item
// 如果谁缺字段可自行额外补充即可
typedef struct {
    /* linker */
    scui_cwf_json_parser_t *parser;
    uint32_t list_idx;
    /* struct */
    union {
    /* type img */
    struct {
    scui_coord_t img_w;
    scui_coord_t img_h;
    uint64_t  img_bits;
    uint16_t  img_span;
    uint16_t  img_num;
    uint16_t *img_ofs;
    /* parament */
    uint8_t  *idx_ofs;
    uint16_t  idx_num;
    uint16_t  idx_anim;
    };
    /* type txt */
    struct {
    uint64_t *txt_none;
    };
    /* keep adding... */
    };
} scui_csf_json_item__res_t;

#endif
