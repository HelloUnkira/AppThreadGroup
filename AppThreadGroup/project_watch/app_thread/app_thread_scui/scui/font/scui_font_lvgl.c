/*实现目标:
 *    字体
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#if 1 // bin field

typedef struct {
    char *name;
    int8_t bit_pos;
    uint8_t byte_value;
} bit_iterator_t;

#pragma pack(push, 1)
typedef struct font_header_bin {
    uint32_t version;
    uint16_t tables_count;
    uint16_t font_size;
    uint16_t ascent;
    int16_t descent;
    uint16_t typo_ascent;
    int16_t typo_descent;
    uint16_t typo_line_gap;
    int16_t min_y;
    int16_t max_y;
    uint16_t default_advance_width;
    uint16_t kerning_scale;
    uint8_t index_to_loc_format;
    uint8_t glyph_id_format;
    uint8_t advance_width_format;
    uint8_t bits_per_pixel;
    uint8_t xy_bits;
    uint8_t wh_bits;
    uint8_t advance_width_bits;
    uint8_t compression_id;
    uint8_t subpixels_mode;
    uint8_t padding;
    int16_t underline_position;
    uint16_t underline_thickness;
} font_header_bin_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct cmap_table_bin {
    uint32_t data_offset;
    uint32_t range_start;
    uint16_t range_length;
    uint16_t glyph_id_start;
    uint16_t data_entries_count;
    uint8_t format_type;
    uint8_t padding;
} cmap_table_bin_t;
#pragma pack(pop)

#endif

#if 1

#define LV_FONT_FMT_TXT_LARGE   0
#define LV_USE_USER_DATA        0

/** This describes a glyph.*/
typedef struct {
#if LV_FONT_FMT_TXT_LARGE == 0
    uint32_t bitmap_index : 20;     /**< Start index of the bitmap. A font can be max 1 MB.*/
    uint32_t adv_w : 12;            /**< Draw the next glyph after this width. 8.4 format (real_value * 16 is stored).*/
    uint8_t box_w;                  /**< Width of the glyph's bounding box*/
    uint8_t box_h;                  /**< Height of the glyph's bounding box*/
    int8_t ofs_x;                   /**< x offset of the bounding box*/
    int8_t ofs_y;                   /**< y offset of the bounding box. Measured from the top of the line*/
#else
    uint32_t bitmap_index;          /**< Start index of the bitmap. A font can be max 4 GB.*/
    uint32_t adv_w;                 /**< Draw the next glyph after this width. 28.4 format (real_value * 16 is stored).*/
    uint16_t box_w;                 /**< Width of the glyph's bounding box*/
    uint16_t box_h;                 /**< Height of the glyph's bounding box*/
    int16_t ofs_x;                  /**< x offset of the bounding box*/
    int16_t ofs_y;                  /**< y offset of the bounding box. Measured from the top of the line*/
#endif
} lv_font_fmt_txt_glyph_dsc_t;

/** Format of font character map.*/
enum {
    LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL,
    LV_FONT_FMT_TXT_CMAP_SPARSE_FULL,
    LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY,
    LV_FONT_FMT_TXT_CMAP_SPARSE_TINY,
};

typedef uint8_t lv_font_fmt_txt_cmap_type_t;

/**
 * Map codepoints to a `glyph_dsc`s
 * Several formats are supported to optimize memory usage
 * See https://github.com/lvgl/lv_font_conv/blob/master/doc/font_spec.md
 */
typedef struct {
    /** First Unicode character for this range*/
    uint32_t range_start;

    /** Number of Unicode characters related to this range.
     * Last Unicode character = range_start + range_length - 1*/
    uint16_t range_length;

    /** First glyph ID (array index of `glyph_dsc`) for this range*/
    uint16_t glyph_id_start;

    /*
    According the specification there are 4 formats:
        https://github.com/lvgl/lv_font_conv/blob/master/doc/font_spec.md

    For simplicity introduce "relative code point":
        rcp = codepoint - range_start

    and a search function:
        search a "value" in an "array" and returns the index of "value".

    Format 0 tiny
        unicode_list == NULL && glyph_id_ofs_list == NULL
        glyph_id = glyph_id_start + rcp

    Format 0 full
        unicode_list == NULL && glyph_id_ofs_list != NULL
        glyph_id = glyph_id_start + glyph_id_ofs_list[rcp]

    Sparse tiny
        unicode_list != NULL && glyph_id_ofs_list == NULL
        glyph_id = glyph_id_start + search(unicode_list, rcp)

    Sparse full
        unicode_list != NULL && glyph_id_ofs_list != NULL
        glyph_id = glyph_id_start + glyph_id_ofs_list[search(unicode_list, rcp)]
    */

    const uint16_t * unicode_list;

    /** if(type == LV_FONT_FMT_TXT_CMAP_FORMAT0_...) it's `uint8_t *`
     * if(type == LV_FONT_FMT_TXT_CMAP_SPARSE_...)  it's `uint16_t *`
     */
    const void * glyph_id_ofs_list;

    /** Length of `unicode_list` and/or `glyph_id_ofs_list`*/
    uint16_t list_length;

    /** Type of this character map*/
    lv_font_fmt_txt_cmap_type_t type;
} lv_font_fmt_txt_cmap_t;

/** A simple mapping of kern values from pairs*/
typedef struct {
    /*To get a kern value of two code points:
       1. Get the `glyph_id_left` and `glyph_id_right` from `lv_font_fmt_txt_cmap_t
       2. for(i = 0; i < pair_cnt * 2; i += 2)
             if(gylph_ids[i] == glyph_id_left &&
                gylph_ids[i+1] == glyph_id_right)
                 return values[i / 2];
     */
    const void * glyph_ids;
    const int8_t * values;
    uint32_t pair_cnt   : 30;
    uint32_t glyph_ids_size : 2;    /*0: `glyph_ids` is stored as `uint8_t`; 1: as `uint16_t`*/
} lv_font_fmt_txt_kern_pair_t;

/** More complex but more optimal class based kern value storage*/
typedef struct {
    /*To get a kern value of two code points:
          1. Get the `glyph_id_left` and `glyph_id_right` from `lv_font_fmt_txt_cmap_t
          2. Get the class of the left and right glyphs as `left_class` and `right_class`
              left_class = left_class_mapping[glyph_id_left];
              right_class = right_class_mapping[glyph_id_right];
          3. value = class_pair_values[(left_class-1)*right_class_cnt + (right_class-1)]
        */

    const int8_t * class_pair_values;    /*left_class_cnt * right_class_cnt value*/
    const uint8_t * left_class_mapping;   /*Map the glyph_ids to classes: index -> glyph_id -> class_id*/
    const uint8_t * right_class_mapping;  /*Map the glyph_ids to classes: index -> glyph_id -> class_id*/
    uint8_t left_class_cnt;
    uint8_t right_class_cnt;
} lv_font_fmt_txt_kern_classes_t;

/** Bitmap formats*/
typedef enum {
    LV_FONT_FMT_TXT_PLAIN      = 0,
    LV_FONT_FMT_TXT_COMPRESSED = 1,
    LV_FONT_FMT_TXT_COMPRESSED_NO_PREFILTER = 1,
} lv_font_fmt_txt_bitmap_format_t;

typedef struct {
    uint32_t last_letter;
    uint32_t last_glyph_id;
} lv_font_fmt_txt_glyph_cache_t;

/*Describe store additional data for fonts*/
typedef struct {
    /*The bitmaps of all glyphs*/
    const uint8_t * glyph_bitmap;

    /*Describe the glyphs*/
    const lv_font_fmt_txt_glyph_dsc_t * glyph_dsc;

    /*Map the glyphs to Unicode characters.
     *Array of `lv_font_cmap_fmt_txt_t` variables*/
    const lv_font_fmt_txt_cmap_t * cmaps;

    /**
     * Store kerning values.
     * Can be `lv_font_fmt_txt_kern_pair_t *  or `lv_font_kern_classes_fmt_txt_t *`
     * depending on `kern_classes`
     */
    const void * kern_dsc;

    /*Scale kern values in 12.4 format*/
    uint16_t kern_scale;

    /*Number of cmap tables*/
    uint16_t cmap_num       : 9;

    /*Bit per pixel: 1, 2, 3, 4, 8*/
    uint16_t bpp            : 4;

    /*Type of `kern_dsc`*/
    uint16_t kern_classes   : 1;

    /*
     * storage format of the bitmap
     * from `lv_font_fmt_txt_bitmap_format_t`
     */
    uint16_t bitmap_format  : 2;

    /*Cache the last letter and is glyph id*/
    lv_font_fmt_txt_glyph_cache_t * cache;
} lv_font_fmt_txt_dsc_t;

struct _lv_font_t;
/** Describes the properties of a glyph.*/
typedef struct {
    const struct _lv_font_t *
        resolved_font; /**< Pointer to a font where the glyph was actually found after handling fallbacks*/
    uint16_t adv_w; /**< The glyph needs this space. Draw the next glyph after this width.*/
    uint16_t box_w; /**< Width of the glyph's bounding box*/
    uint16_t box_h; /**< Height of the glyph's bounding box*/
    int16_t ofs_x;  /**< x offset of the bounding box*/
    int16_t ofs_y;  /**< y offset of the bounding box*/
    uint8_t bpp: 4;  /**< Bit-per-pixel: 1, 2, 4, 8*/
    uint8_t is_placeholder: 1; /** Glyph is missing. But placeholder will still be displayed */
} lv_font_glyph_dsc_t;

/** The bitmaps might be upscaled by 3 to achieve subpixel rendering.*/
enum {
    LV_FONT_SUBPX_NONE,
    LV_FONT_SUBPX_HOR,
    LV_FONT_SUBPX_VER,
    LV_FONT_SUBPX_BOTH,
};

typedef uint8_t lv_font_subpx_t;

/** Describe the properties of a font*/
typedef struct _lv_font_t {
    /** Get a glyph's descriptor from a font*/
    bool (*get_glyph_dsc)(const struct _lv_font_t *, lv_font_glyph_dsc_t *, uint32_t letter, uint32_t letter_next);

    /** Get a glyph's bitmap from a font*/
    const uint8_t * (*get_glyph_bitmap)(const struct _lv_font_t *, uint32_t);

    /*Pointer to the font in a font pack (must have the same line height)*/
    scui_coord_t line_height;         /**< The real line height where any text fits*/
    scui_coord_t base_line;           /**< Base line measured from the top of the line_height*/
    uint8_t subpx  : 2;             /**< An element of `lv_font_subpx_t`*/

    int8_t underline_position;      /**< Distance between the top of the underline and base line (< 0 means below the base line)*/
    int8_t underline_thickness;     /**< Thickness of the underline*/

    const void * dsc;               /**< Store implementation specific or run_time data or caching here*/
    const struct _lv_font_t * fallback;   /**< Fallback font for missing glyph. Resolved recursively */
#if LV_USE_USER_DATA
    void * user_data;               /**< Custom user data for font.*/
#endif

    /* bin[glyph]:扩充字段 */
    font_header_bin_t bin_head;
    uintptr_t bin_glyph_offset;
    uint32_t  loca_count;
    uint32_t *glyph_offset;
    

} lv_font_t;

#if 0   // iter

static bit_iterator_t init_bit_iterator(lv_fs_file_t * fp)
{
    bit_iterator_t it;
    it.fp = fp;
    it.bit_pos = -1;
    it.byte_value = 0;
    return it;
}

static unsigned int read_bits(bit_iterator_t * it, int n_bits, lv_fs_res_t * res)
{
    unsigned int value = 0;
    while(n_bits--) {
        it->byte_value = it->byte_value << 1;
        it->bit_pos--;

        if(it->bit_pos < 0) {
            it->bit_pos = 7;
            *res = lv_fs_read(it->fp, &(it->byte_value), 1, NULL);
            if(*res != LV_FS_RES_OK) {
                return 0;
            }
        }
        int8_t bit = (it->byte_value & 0x80) ? 1 : 0;

        value |= (bit << n_bits);
    }
    *res = LV_FS_RES_OK;
    return value;
}

static int read_bits_signed(bit_iterator_t * it, int n_bits, lv_fs_res_t * res)
{
    unsigned int value = read_bits(it, n_bits, res);
    if(value & (1 << (n_bits - 1))) {
        value |= ~0u << n_bits;
    }
    return value;
}

#endif

/*@brief lvgl 读取标签
 */
static uint32_t read_label(char *name, uintptr_t offset, char *label)
{
    /* 从bin文件布局角度
     * 每一个标签由 标签总长度(4字节) + 标签名称(4字节) + 标签内容(总长度 - 8)字节表示
     */
    
    uint32_t length = 0;
    scui_font_src_read(name, offset + 0, (uint8_t *)&length, 4);
    
    char label_4[5] = {0};
    scui_font_src_read(name, offset + 4, (uint8_t *)label_4, 4);
    SCUI_ASSERT(strcmp(label_4, label) == 0);
    
    return length;
}

/*@brief lvgl 加载标签(cmap)
 */
static uint32_t load_cmaps(char *name, uintptr_t offset, lv_font_fmt_txt_dsc_t * font_dsc)
{
    uint32_t cmaps_length = read_label(name, offset, "cmap");
    SCUI_ASSERT(cmaps_length > 0);
    
    uint32_t cmaps_subtables_count = 0;
    scui_font_src_read(name, offset + 8, (uint8_t *)&cmaps_subtables_count, 4);
    SCUI_ASSERT(cmaps_subtables_count > 0);
    
    uint32_t cmaps_subtables_size = cmaps_subtables_count * sizeof(lv_font_fmt_txt_cmap_t);
    lv_font_fmt_txt_cmap_t * cmaps = SCUI_MEM_ALLOC(scui_mem_type_font, cmaps_subtables_size);
    memset(cmaps, 0, cmaps_subtables_size);
    
    font_dsc->cmaps = cmaps;
    font_dsc->cmap_num = cmaps_subtables_count;
    
    uint32_t cmaps_tables_size = sizeof(cmap_table_bin_t) * font_dsc->cmap_num;
    cmap_table_bin_t * cmaps_tables = SCUI_MEM_ALLOC(scui_mem_type_font, cmaps_tables_size);
    scui_font_src_read(name, offset + 12, (uint8_t *)cmaps_tables, cmaps_tables_size);
    
    /* 加载每一个子标签(原型为load_cmaps_tables, 这里整理到内部) */
    for(uint32_t idx = 0; idx < font_dsc->cmap_num; idx++) {
        uint32_t ofs = offset + 12 + cmaps_tables_size + cmaps_tables[idx].data_offset;
        
        lv_font_fmt_txt_cmap_t *cmap = (lv_font_fmt_txt_cmap_t *)&font_dsc->cmaps[idx];
        
        cmap->range_start = cmaps_tables[idx].range_start;
        cmap->range_length = cmaps_tables[idx].range_length;
        cmap->glyph_id_start = cmaps_tables[idx].glyph_id_start;
        cmap->type = cmaps_tables[idx].format_type;
        
        switch(cmaps_tables[idx].format_type) {
            case LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL: {
                    uint8_t ids_size = sizeof(uint8_t) * cmaps_tables[idx].data_entries_count;
                    cmap->glyph_id_ofs_list = SCUI_MEM_ALLOC(scui_mem_type_font, ids_size);
                    scui_font_src_read(name, ofs, (uint8_t *)cmap->glyph_id_ofs_list, ids_size);
                    cmap->list_length = cmap->range_length;
                    break;
                }
            case LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY:
                break;
            case LV_FONT_FMT_TXT_CMAP_SPARSE_FULL:
            case LV_FONT_FMT_TXT_CMAP_SPARSE_TINY: {
                    uint32_t list_size = sizeof(uint16_t) * cmaps_tables[idx].data_entries_count;
                    cmap->unicode_list = SCUI_MEM_ALLOC(scui_mem_type_font, list_size);
                    cmap->list_length = cmaps_tables[idx].data_entries_count;
                    scui_font_src_read(name, ofs, (uint8_t *)cmap->unicode_list, list_size);
                    
                    if(cmaps_tables[idx].format_type == LV_FONT_FMT_TXT_CMAP_SPARSE_FULL) {
                        cmap->glyph_id_ofs_list = SCUI_MEM_ALLOC(scui_mem_type_font, cmap->list_length);
                        scui_font_src_read(name, ofs, (uint8_t *)cmap->glyph_id_ofs_list, sizeof(uint16_t) * cmap->list_length);
                    }
                    break;
                }
            default:
                SCUI_LOG_WARN("Unknown cmaps format type %d.", cmaps_tables[idx].format_type);
                break;
        }
    }
    
    SCUI_MEM_FREE(cmaps_tables);
    return cmaps_length;
}

/* 原型load_glyph被完全定向改造
 * 我们不需要一次性加载整个字库源
 * 而是根据字的使用情况去动态加载
 */

static uint32_t load_kern(char *name, uintptr_t offset, lv_font_fmt_txt_dsc_t * font_dsc, uint8_t format)
{
    uint32_t kern_length = read_label(name, offset, "kern");
    SCUI_ASSERT(kern_length > 0);
    
    uint8_t kern_format_type;
    uint32_t padding;
    scui_font_src_read(name, offset + 8, (uint8_t *)&kern_format_type, 1);
    scui_font_src_read(name, offset + 9, (uint8_t *)&padding, 3);
    
    if(0 == kern_format_type) { /*sorted pairs*/
        lv_font_fmt_txt_kern_pair_t * kern_pair = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(lv_font_fmt_txt_kern_pair_t));
        memset(kern_pair, 0, sizeof(lv_font_fmt_txt_kern_pair_t));
        
        font_dsc->kern_dsc = kern_pair;
        font_dsc->kern_classes = 0;
        
        uint32_t glyph_entries;
        scui_font_src_read(name, offset + 12, (uint8_t *)&glyph_entries, 4);
        
        uint32_t ids_size = format == 0 ? sizeof(int8_t) * 2 * glyph_entries : sizeof(int16_t) * 2 * glyph_entries;
        
        uint8_t * glyph_ids = SCUI_MEM_ALLOC(scui_mem_type_font, ids_size);
        int8_t * values = SCUI_MEM_ALLOC(scui_mem_type_font, glyph_entries);
        
        kern_pair->glyph_ids_size = format;
        kern_pair->pair_cnt = glyph_entries;
        kern_pair->glyph_ids = glyph_ids;
        kern_pair->values = values;
        
        scui_font_src_read(name, offset + 16, (uint8_t *)glyph_ids, ids_size);
        scui_font_src_read(name, offset + 16 + ids_size, (uint8_t *)values, glyph_entries);
    }
    else if(3 == kern_format_type) { /*array M*N of classes*/
        lv_font_fmt_txt_kern_classes_t * kern_classes = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(lv_font_fmt_txt_kern_classes_t));
        memset(kern_classes, 0, sizeof(lv_font_fmt_txt_kern_classes_t));
        
        font_dsc->kern_dsc = kern_classes;
        font_dsc->kern_classes = 1;
        
        uint16_t kern_class_mapping_length;
        uint8_t kern_table_rows;
        uint8_t kern_table_cols;
        
        scui_font_src_read(name, offset + 12 + 0, (uint8_t *)&kern_class_mapping_length, 2);
        scui_font_src_read(name, offset + 12 + 2, (uint8_t *)&kern_table_rows, 1);
        scui_font_src_read(name, offset + 12 + 3, (uint8_t *)&kern_table_cols, 1);
        
        uint32_t kern_values_length = sizeof(int8_t) * kern_table_rows * kern_table_cols;
        
        uint8_t * kern_left = SCUI_MEM_ALLOC(scui_mem_type_font, kern_class_mapping_length);
        uint8_t * kern_right = SCUI_MEM_ALLOC(scui_mem_type_font, kern_class_mapping_length);
        int8_t * kern_values = SCUI_MEM_ALLOC(scui_mem_type_font, kern_values_length);
        
        kern_classes->left_class_mapping  = kern_left;
        kern_classes->right_class_mapping = kern_right;
        kern_classes->left_class_cnt = kern_table_rows;
        kern_classes->right_class_cnt = kern_table_cols;
        kern_classes->class_pair_values = kern_values;
        
        uintptr_t ofs_l = 0;
        uintptr_t ofs_r = kern_class_mapping_length;
        uintptr_t ofs_v = kern_class_mapping_length * 2;
        
        scui_font_src_read(name, offset + 16 + ofs_l, (uint8_t *)kern_left, kern_class_mapping_length);
        scui_font_src_read(name, offset + 16 + ofs_r, (uint8_t *)kern_right, kern_class_mapping_length);
        scui_font_src_read(name, offset + 16 + ofs_v, (uint8_t *)kern_values, kern_values_length);
    }
    else {
        
        SCUI_LOG_WARN("Unknown kern_format_type: %d", kern_format_type);
    }
    
    return kern_length;
}

static lv_font_t * lv_font_load(char *name)
{
    lv_font_t *font = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(lv_font_t));
    memset(font, 0, sizeof(lv_font_t));
    
    lv_font_fmt_txt_dsc_t *font_dsc = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(lv_font_fmt_txt_dsc_t));
    memset(font_dsc, 0, sizeof(lv_font_fmt_txt_dsc_t));
    font->dsc = font_dsc;
    
    /* bin[head] */
    uint32_t header_length = read_label(name, 0, "head");
    SCUI_ASSERT(header_length > 0);
    font_header_bin_t font_header;
    scui_font_src_read(name, 0 + 8, (uint8_t *)&font_header, sizeof(font_header_bin_t));
    
    font->base_line = -font_header.descent;
    font->line_height = font_header.ascent - font_header.descent;
    /* @等待适配 */
    // font->get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt;
    // font->get_glyph_bitmap = lv_font_get_bitmap_fmt_txt;
    font->subpx = font_header.subpixels_mode;
    font->underline_position = font_header.underline_position;
    font->underline_thickness = font_header.underline_thickness;
    
    font_dsc->bpp = font_header.bits_per_pixel;
    font_dsc->kern_scale = font_header.kerning_scale;
    font_dsc->bitmap_format = font_header.compression_id;
    
    /* bin[cmap] */
    uint32_t cmaps_offset = header_length;
    uint32_t cmaps_length = load_cmaps(name, cmaps_offset, font_dsc);
    
    /* bin[loca] */
    uint32_t loca_offset = cmaps_offset + cmaps_length;
    uint32_t loca_length = read_label(name, loca_offset, "loca");
    
    uint32_t loca_count;
    scui_font_src_read(name, loca_offset + 8, (uint8_t *)&loca_count, 4);
    
    uint32_t *glyph_offset = SCUI_MEM_ALLOC(scui_mem_type_font, 4 * (loca_count + 1));
    
    if (font_header.index_to_loc_format == 0) {
        for(uint32_t i = 0; i < loca_count; ++i) {
            uint16_t offset;
            scui_font_src_read(name, loca_offset + 12 + i * 2, (uint8_t *)&offset, 2);
            glyph_offset[i] = offset;
        }
    }
    if (font_header.index_to_loc_format == 1) {
            scui_font_src_read(name, loca_offset + 12, (uint8_t *)&glyph_offset, loca_count * 4);
    }
    
    /* bin[glyph](外部扩充,该字段不加载,保存需要加载该字段的所有参数信息,转为动态加载) */
    uint32_t bin_glyph_offset = loca_offset + loca_length;
    uint32_t bin_glyph_langth = read_label(name, bin_glyph_offset, "glyf");
    
    font->bin_head = font_header;
    font->bin_glyph_offset = bin_glyph_offset;
    font->glyph_offset = glyph_offset;
    font->loca_count = loca_count;
    
    /* bin[kern] */
    if(font_header.tables_count >= 4) {
        
        uint32_t kern_offset = bin_glyph_offset + bin_glyph_langth;
        uint32_t kern_length = load_kern(name, kern_offset, font_dsc, font_header.glyph_id_format);
    }
    
    return font;
}

static void lv_font_free(lv_font_t * font)
{
    if(NULL != font) {
        
        if (font->glyph_offset != NULL)
            SCUI_MEM_FREE(font->glyph_offset);
        
        lv_font_fmt_txt_dsc_t * dsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
        
        if(NULL != dsc) {
            
            if(dsc->kern_classes == 0) {
                lv_font_fmt_txt_kern_pair_t * kern_dsc = dsc->kern_dsc;
                
                if(NULL != kern_dsc) {
                    
                    if(kern_dsc->glyph_ids)
                        SCUI_MEM_FREE((void *)kern_dsc->glyph_ids);
                        
                    if(kern_dsc->values)
                        SCUI_MEM_FREE((void *)kern_dsc->values);
                        
                    SCUI_MEM_FREE((void *)kern_dsc);
                }
            }
            else {
                lv_font_fmt_txt_kern_classes_t * kern_dsc = dsc->kern_dsc;
                
                if(NULL != kern_dsc) {
                    
                    if(kern_dsc->class_pair_values)
                        SCUI_MEM_FREE((void *)kern_dsc->class_pair_values);
                        
                    if(kern_dsc->left_class_mapping)
                        SCUI_MEM_FREE((void *)kern_dsc->left_class_mapping);
                        
                    if(kern_dsc->right_class_mapping)
                        SCUI_MEM_FREE((void *)kern_dsc->right_class_mapping);
                        
                    SCUI_MEM_FREE((void *)kern_dsc);
                }
            }
            
            lv_font_fmt_txt_cmap_t * cmaps =
                (lv_font_fmt_txt_cmap_t *)dsc->cmaps;
            
            if(NULL != cmaps) {
                for(int i = 0; i < dsc->cmap_num; ++i) {
                    
                    if(NULL != cmaps[i].glyph_id_ofs_list)
                        SCUI_MEM_FREE((void *)cmaps[i].glyph_id_ofs_list);
                    
                    if(NULL != cmaps[i].unicode_list)
                        SCUI_MEM_FREE((void *)cmaps[i].unicode_list);
                }
                SCUI_MEM_FREE(cmaps);
            }
            
            if(NULL != dsc->glyph_bitmap)
                SCUI_MEM_FREE((void *)dsc->glyph_bitmap);
            
            if(NULL != dsc->glyph_dsc)
                SCUI_MEM_FREE((void *)dsc->glyph_dsc);
            
            SCUI_MEM_FREE(dsc);
        }
        SCUI_MEM_FREE(font);
    }
}

#endif

/*@brief 字库加载
 *@param name   字库名称
 *@param handle 字库句柄
 */
void scui_font_load(char *name, scui_handle_t *handle)
{
    *handle = scui_handle_find();
    lv_font_t *font = lv_font_load(name);
    scui_handle_set(*handle, font);
}

/*@brief 字库卸载
 *@param handle 字库句柄
 */
void scui_font_free(scui_handle_t handle)
{
    lv_font_t *font = scui_handle_get(handle);
    SCUI_ASSERT(font != NULL);
    lv_font_free(font);
    scui_handle_set(handle, NULL);
}
