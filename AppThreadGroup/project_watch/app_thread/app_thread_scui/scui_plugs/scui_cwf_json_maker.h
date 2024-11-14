#ifndef SCUI_CWF_JSON_MAKER_H
#define SCUI_CWF_JSON_MAKER_H

// 使用一个通用结构去描述所有item
// 如果谁缺字段可自行额外补充即可
typedef struct {
    uint64_t  img_bits;
    uint16_t  img_span;
    uint16_t  img_num;
    uint16_t *img_ofs;
} scui_csf_json_item__res_t;

#endif
