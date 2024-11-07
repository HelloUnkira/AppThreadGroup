/*实现目标:
 *    字体
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        0   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

#if defined(LVGL_VERSION_MAJOR) || defined(LVGL_VERSION_MINOR) || defined(LVGL_VERSION_PATCH)
#error "do not import any lvgl header files"
#endif

#if 1 // bin field

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

#define LV_FONT_FMT_TXT_LARGE       0
#define LV_USE_USER_DATA            0
#define LV_USE_FONT_COMPRESSED      1

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
    uintptr_t bin_loca_offset;
    uint32_t  loca_length;
    uint32_t  loca_count;
    // uint32_t *glyph_offset;
    
    /* font fs扩展字段 */
    scui_font_src_t font_src;
    uint32_t size;
    char name[128];

} lv_font_t;

#if 1   // iter

typedef struct {
    lv_font_t *font;
    uintptr_t  ofs;
    
    int8_t bit_pos;
    uint8_t byte_value;
} bit_iterator_t;

static bit_iterator_t init_bit_iterator(lv_font_t *font, uintptr_t ofs)
{
    bit_iterator_t it;
    
    it.font = font;
    
    it.bit_pos = -1;
    it.byte_value = 0;
    return it;
}

static unsigned int read_bits(bit_iterator_t * it, int n_bits)
{
    unsigned int value = 0;
    while (n_bits--) {
        it->byte_value = it->byte_value << 1;
        it->bit_pos--;
        
        if (it->bit_pos < 0) {
            it->bit_pos = 7;
            scui_font_src_read(&it->font->font_src, &it->byte_value, 1);
        }
        int8_t bit = (it->byte_value & 0x80) ? 1 : 0;
        
        value |= (bit << n_bits);
    }
    return value;
}

static int read_bits_signed(bit_iterator_t * it, int n_bits)
{
    unsigned int value = read_bits(it, n_bits);
    
    if (value & (1 << (n_bits - 1)))
        value |= ~0u << n_bits;
    
    return value;
}

#endif

#if 1   // bsearch compare cb

static int32_t unicode_list_compare(const void * ref, const void * element)
{
    return ((int32_t)(*(uint16_t *)ref)) - ((int32_t)(*(uint16_t *)element));
}

static int32_t kern_pair_8_compare(const void * ref, const void * element)
{
    const uint8_t * ref8_p = ref;
    const uint8_t * element8_p = element;

    /*If the MSB is different it will matter. If not return the diff. of the LSB*/
    if(ref8_p[0] != element8_p[0]) return (int32_t)ref8_p[0] - element8_p[0];
    else return (int32_t) ref8_p[1] - element8_p[1];

}

static int32_t kern_pair_16_compare(const void * ref, const void * element)
{
    const uint16_t * ref16_p = ref;
    const uint16_t * element16_p = element;

    /*If the MSB is different it will matter. If not return the diff. of the LSB*/
    if(ref16_p[0] != element16_p[0]) return (int32_t)ref16_p[0] - element16_p[0];
    else return (int32_t) ref16_p[1] - element16_p[1];
}

#endif

#if LV_USE_FONT_COMPRESSED

typedef enum
{
    RLE_STATE_SINGLE = 0,
    RLE_STATE_REPEATE,
    RLE_STATE_COUNTER,
} rle_state_t;

static uint32_t rle_rdp;
static const uint8_t * rle_in;
static uint8_t rle_bpp;
static uint8_t rle_prev_v;
static uint8_t rle_cnt;
static rle_state_t rle_state;

/**
 * Read bits from an input buffer. The read can cross byte boundary.
 * @param in the input buffer to read from.
 * @param bit_pos index of the first bit to read.
 * @param len number of bits to read (must be <= 8).
 * @return the read bits
 */
static inline uint8_t get_bits(const uint8_t * in, uint32_t bit_pos, uint8_t len)
{
    uint8_t bit_mask;
    switch(len) {
        case 1:
            bit_mask = 0x1;
            break;
        case 2:
            bit_mask = 0x3;
            break;
        case 3:
            bit_mask = 0x7;
            break;
        case 4:
            bit_mask = 0xF;
            break;
        case 8:
            bit_mask = 0xFF;
            break;
        default:
            bit_mask = (uint16_t)((uint16_t) 1 << len) - 1;
    }

    uint32_t byte_pos = bit_pos >> 3;
    bit_pos = bit_pos & 0x7;

    if(bit_pos + len >= 8) {
        uint16_t in16 = (in[byte_pos] << 8) + in[byte_pos + 1];
        return (in16 >> (16 - bit_pos - len)) & bit_mask;
    }
    else {
        return (in[byte_pos] >> (8 - bit_pos - len)) & bit_mask;
    }
}

/**
 * Write `val` data to `bit_pos` position of `out`. The write can NOT cross byte boundary.
 * @param out buffer where to write
 * @param bit_pos bit index to write
 * @param val value to write
 * @param len length of bits to write from `val`. (Counted from the LSB).
 * @note `len == 3` will be converted to `len = 4` and `val` will be upscaled too
 */
static inline void bits_write(uint8_t * out, uint32_t bit_pos, uint8_t val, uint8_t len)
{
    if(len == 3) {
        len = 4;
        switch(val) {
            case 0:
                val = 0;
                break;
            case 1:
                val = 2;
                break;
            case 2:
                val = 4;
                break;
            case 3:
                val = 6;
                break;
            case 4:
                val = 9;
                break;
            case 5:
                val = 11;
                break;
            case 6:
                val = 13;
                break;
            case 7:
                val = 15;
                break;
        }
    }

    uint16_t byte_pos = bit_pos >> 3;
    bit_pos = bit_pos & 0x7;
    bit_pos = 8 - bit_pos - len;

    uint8_t bit_mask = (uint16_t)((uint16_t) 1 << len) - 1;
    out[byte_pos] &= ((~bit_mask) << bit_pos);
    out[byte_pos] |= (val << bit_pos);
}

static inline void rle_init(const uint8_t * in,  uint8_t bpp)
{
    rle_in = in;
    rle_bpp = bpp;
    rle_state = RLE_STATE_SINGLE;
    rle_rdp = 0;
    rle_prev_v = 0;
    rle_cnt = 0;
}

static inline uint8_t rle_next(void)
{
    uint8_t v = 0;
    uint8_t ret = 0;

    if(rle_state == RLE_STATE_SINGLE) {
        ret = get_bits(rle_in, rle_rdp, rle_bpp);
        if(rle_rdp != 0 && rle_prev_v == ret) {
            rle_cnt = 0;
            rle_state = RLE_STATE_REPEATE;
        }

        rle_prev_v = ret;
        rle_rdp += rle_bpp;
    }
    else if(rle_state == RLE_STATE_REPEATE) {
        v = get_bits(rle_in, rle_rdp, 1);
        rle_cnt++;
        rle_rdp += 1;
        if(v == 1) {
            ret = rle_prev_v;
            if(rle_cnt == 11) {
                rle_cnt = get_bits(rle_in, rle_rdp, 6);
                rle_rdp += 6;
                if(rle_cnt != 0) {
                    rle_state = RLE_STATE_COUNTER;
                }
                else {
                    ret = get_bits(rle_in, rle_rdp, rle_bpp);
                    rle_prev_v = ret;
                    rle_rdp += rle_bpp;
                    rle_state = RLE_STATE_SINGLE;
                }
            }
        }
        else {
            ret = get_bits(rle_in, rle_rdp, rle_bpp);
            rle_prev_v = ret;
            rle_rdp += rle_bpp;
            rle_state = RLE_STATE_SINGLE;
        }

    }
    else if(rle_state == RLE_STATE_COUNTER) {
        ret = rle_prev_v;
        rle_cnt--;
        if(rle_cnt == 0) {
            ret = get_bits(rle_in, rle_rdp, rle_bpp);
            rle_prev_v = ret;
            rle_rdp += rle_bpp;
            rle_state = RLE_STATE_SINGLE;
        }
    }

    return ret;
}

/**
 * Decompress one line. Store one pixel per byte
 * @param out output buffer
 * @param w width of the line in pixel count
 */
static inline void decompress_line(uint8_t * out, scui_coord_t w)
{
    scui_coord_t i;
    for(i = 0; i < w; i++) {
        out[i] = rle_next();
    }
}

/**
 * The compress a glyph's bitmap
 * @param in the compressed bitmap
 * @param out buffer to store the result
 * @param px_num number of pixels in the glyph (width * height)
 * @param bpp bit per pixel (bpp = 3 will be converted to bpp = 4)
 * @param prefilter true: the lines are XORed
 */
static void decompress(const uint8_t * in, uint8_t * out, scui_coord_t w, scui_coord_t h, uint8_t bpp, bool prefilter)
{
    uint32_t wrp = 0;
    uint8_t wr_size = bpp;
    if(bpp == 3) wr_size = 4;

    rle_init(in, bpp);

    uint8_t * line_buf1 = SCUI_MEM_ALLOC(scui_mem_type_font, w);
    uint8_t * line_buf2 = NULL;

    if(prefilter) {
        line_buf2 = SCUI_MEM_ALLOC(scui_mem_type_font, w);
    }

    decompress_line(line_buf1, w);

    scui_coord_t y;
    scui_coord_t x;

    for(x = 0; x < w; x++) {
        bits_write(out, wrp, line_buf1[x], bpp);
        wrp += wr_size;
    }

    for(y = 1; y < h; y++) {
        if(prefilter) {
            decompress_line(line_buf2, w);

            for(x = 0; x < w; x++) {
                line_buf1[x] = line_buf2[x] ^ line_buf1[x];
                bits_write(out, wrp, line_buf1[x], bpp);
                wrp += wr_size;
            }
        }
        else {
            decompress_line(line_buf1, w);

            for(x = 0; x < w; x++) {
                bits_write(out, wrp, line_buf1[x], bpp);
                wrp += wr_size;
            }
        }
    }

    SCUI_MEM_FREE(line_buf1);
    SCUI_MEM_FREE(line_buf2);
}

#endif

#if 1

static uint32_t get_glyph_dsc_id(const lv_font_t *font, uint32_t letter)
{
    if(letter == '\0') return 0;

    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;

    /*Check the cache first*/
    if(fdsc->cache && letter == fdsc->cache->last_letter) return fdsc->cache->last_glyph_id;

    uint16_t i;
    for(i = 0; i < fdsc->cmap_num; i++) {

        /*Relative code point*/
        uint32_t rcp = letter - fdsc->cmaps[i].range_start;
        if(rcp > fdsc->cmaps[i].range_length) continue;
        uint32_t glyph_id = 0;
        if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY) {
            glyph_id = fdsc->cmaps[i].glyph_id_start + rcp;
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL) {
            const uint8_t * gid_ofs_8 = fdsc->cmaps[i].glyph_id_ofs_list;
            glyph_id = fdsc->cmaps[i].glyph_id_start + gid_ofs_8[rcp];
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_SPARSE_TINY) {
            uint16_t key = rcp;
            uint16_t * p = scui_bsearch(fdsc->cmaps[i].unicode_list, fdsc->cmaps[i].list_length,
                                        sizeof(fdsc->cmaps[i].unicode_list[0]), &key,
                                        unicode_list_compare);

            if(p) {
                uintptr_t ofs = p - fdsc->cmaps[i].unicode_list;
                glyph_id = fdsc->cmaps[i].glyph_id_start + ofs;
            }
        }
        else if(fdsc->cmaps[i].type == LV_FONT_FMT_TXT_CMAP_SPARSE_FULL) {
            uint16_t key = rcp;
            uint16_t * p = scui_bsearch(fdsc->cmaps[i].unicode_list, fdsc->cmaps[i].list_length,
                                        sizeof(fdsc->cmaps[i].unicode_list[0]), &key,
                                        unicode_list_compare);

            if(p) {
                uintptr_t ofs = p - fdsc->cmaps[i].unicode_list;
                const uint16_t * gid_ofs_16 = fdsc->cmaps[i].glyph_id_ofs_list;
                glyph_id = fdsc->cmaps[i].glyph_id_start + gid_ofs_16[ofs];
            }
        }

        /*Update the cache*/
        if(fdsc->cache) {
            fdsc->cache->last_letter = letter;
            fdsc->cache->last_glyph_id = glyph_id;
        }
        return glyph_id;
    }

    if(fdsc->cache) {
        fdsc->cache->last_letter = letter;
        fdsc->cache->last_glyph_id = 0;
    }
    return 0;
}

static int8_t get_kern_value(const lv_font_t *font, uint32_t gid_left, uint32_t gid_right)
{
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;

    int8_t value = 0;

    if(fdsc->kern_classes == 0) {
        /*Kern pairs*/
        const lv_font_fmt_txt_kern_pair_t * kdsc = fdsc->kern_dsc;
        if(kdsc->glyph_ids_size == 0) {
            /*Use binary search to find the kern value.
             *The pairs are ordered left_id first, then right_id secondly.*/
            const uint16_t * g_ids = kdsc->glyph_ids;
            uint16_t g_id_both = (gid_right << 8) + gid_left; /*Create one number from the ids*/
            uint16_t * kid_p = scui_bsearch(g_ids, kdsc->pair_cnt, 2, &g_id_both, kern_pair_8_compare);

            /*If the `g_id_both` were found get its index from the pointer*/
            if(kid_p) {
                uintptr_t ofs = kid_p - g_ids;
                value = kdsc->values[ofs];
            }
        }
        else if(kdsc->glyph_ids_size == 1) {
            /*Use binary search to find the kern value.
             *The pairs are ordered left_id first, then right_id secondly.*/
            const uint32_t * g_ids = kdsc->glyph_ids;
            uint32_t g_id_both = (gid_right << 16) + gid_left; /*Create one number from the ids*/
            uint32_t * kid_p = scui_bsearch(g_ids, kdsc->pair_cnt, 4, &g_id_both, kern_pair_16_compare);

            /*If the `g_id_both` were found get its index from the pointer*/
            if(kid_p) {
                uintptr_t ofs = kid_p - g_ids;
                value = kdsc->values[ofs];
            }

        }
        else {
            /*Invalid value*/
        }
    }
    else {
        /*Kern classes*/
        const lv_font_fmt_txt_kern_classes_t * kdsc = fdsc->kern_dsc;
        uint8_t left_class = kdsc->left_class_mapping[gid_left];
        uint8_t right_class = kdsc->right_class_mapping[gid_right];

        /*If class = 0, kerning not exist for that glyph
         *else got the value form `class_pair_values` 2D array*/
        if(left_class > 0 && right_class > 0) {
            value = kdsc->class_pair_values[(left_class - 1) * kdsc->right_class_cnt + (right_class - 1)];
        }

    }
    return value;
}

#endif

/*@brief lvgl 读取标签
 */
static uint32_t read_label(lv_font_t *font, uintptr_t offset, char *label)
{
    /* 从bin文件布局角度
     * 每一个标签由 标签总长度(4字节) + 标签名称(4字节) + 标签内容(总长度 - 8)字节表示
     */
    
    uint32_t length = 0;
    scui_font_src_seek(&font->font_src, offset + 0);
    scui_font_src_read(&font->font_src, &length, 4);
    
    char label_4[5] = {0};
    scui_font_src_seek(&font->font_src, offset + 4);
    scui_font_src_read(&font->font_src, label_4, 4);
    SCUI_ASSERT(strcmp(label_4, label) == 0);
    
    return length;
}

/*@brief lvgl 加载标签(cmap)
 */
static uint32_t load_cmaps(lv_font_t *font, uintptr_t offset, lv_font_fmt_txt_dsc_t * font_dsc)
{
    uint32_t cmaps_length = read_label(font, offset, "cmap");
    SCUI_ASSERT(cmaps_length > 0);
    
    uint32_t cmaps_subtables_count = 0;
    scui_font_src_seek(&font->font_src, offset + 8);
    scui_font_src_read(&font->font_src, &cmaps_subtables_count, 4);
    SCUI_ASSERT(cmaps_subtables_count > 0);
    
    uint32_t cmaps_subtables_size = cmaps_subtables_count * sizeof(lv_font_fmt_txt_cmap_t);
    lv_font_fmt_txt_cmap_t * cmaps = SCUI_MEM_ALLOC(scui_mem_type_font, cmaps_subtables_size);
    font->size += cmaps_subtables_size;
    memset(cmaps, 0, cmaps_subtables_size);
    
    font_dsc->cmaps = cmaps;
    font_dsc->cmap_num = cmaps_subtables_count;
    
    uint32_t cmaps_tables_size = sizeof(cmap_table_bin_t) * font_dsc->cmap_num;
    cmap_table_bin_t * cmaps_tables = SCUI_MEM_ALLOC(scui_mem_type_font, cmaps_tables_size);
    font->size += cmaps_tables_size;
    
    scui_font_src_seek(&font->font_src, offset + 12);
    scui_font_src_read(&font->font_src, cmaps_tables, cmaps_tables_size);
    
    /* 加载每一个子标签(原型为load_cmaps_tables, 这里整理到内部) */
    for(uint32_t idx = 0; idx < font_dsc->cmap_num; idx++) {
        uint32_t ofs = offset + cmaps_tables[idx].data_offset;
        
        lv_font_fmt_txt_cmap_t *cmap = (lv_font_fmt_txt_cmap_t *)&font_dsc->cmaps[idx];
        
        cmap->range_start = cmaps_tables[idx].range_start;
        cmap->range_length = cmaps_tables[idx].range_length;
        cmap->glyph_id_start = cmaps_tables[idx].glyph_id_start;
        cmap->type = cmaps_tables[idx].format_type;
        
        switch(cmaps_tables[idx].format_type) {
            case LV_FONT_FMT_TXT_CMAP_FORMAT0_FULL: {
                    uint8_t ids_size = sizeof(uint8_t) * cmaps_tables[idx].data_entries_count;
                    cmap->glyph_id_ofs_list = SCUI_MEM_ALLOC(scui_mem_type_font, ids_size);
                    font->size += ids_size;
                    scui_font_src_seek(&font->font_src, ofs);
                    scui_font_src_read(&font->font_src, cmap->glyph_id_ofs_list, ids_size);
                    cmap->list_length = cmap->range_length;
                    break;
                }
            case LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY:
                break;
            case LV_FONT_FMT_TXT_CMAP_SPARSE_FULL:
            case LV_FONT_FMT_TXT_CMAP_SPARSE_TINY: {
                    uint32_t list_size = sizeof(uint16_t) * cmaps_tables[idx].data_entries_count;
                    cmap->unicode_list = SCUI_MEM_ALLOC(scui_mem_type_font, list_size);
                    font->size += list_size;
                    cmap->list_length = cmaps_tables[idx].data_entries_count;
                    scui_font_src_seek(&font->font_src, ofs);
                    scui_font_src_read(&font->font_src, cmap->unicode_list, list_size);
                    
                    if(cmaps_tables[idx].format_type == LV_FONT_FMT_TXT_CMAP_SPARSE_FULL) {
                        cmap->glyph_id_ofs_list = SCUI_MEM_ALLOC(scui_mem_type_font, list_size);
                        font->size += list_size;
                        scui_font_src_seek(&font->font_src, ofs + list_size);
                        scui_font_src_read(&font->font_src, cmap->glyph_id_ofs_list, list_size);
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

static uint32_t load_kern(lv_font_t *font, uintptr_t offset, lv_font_fmt_txt_dsc_t * font_dsc, uint8_t format)
{
    uint32_t kern_length = read_label(font, offset, "kern");
    SCUI_ASSERT(kern_length > 0);
    
    uint8_t kern_format_type;
    uint32_t padding;
    scui_font_src_seek(&font->font_src, offset + 8);
    scui_font_src_read(&font->font_src, &kern_format_type, 1);
    scui_font_src_seek(&font->font_src, offset + 9);
    scui_font_src_read(&font->font_src, &padding, 3);
    
    if(0 == kern_format_type) { /*sorted pairs*/
        lv_font_fmt_txt_kern_pair_t * kern_pair = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(lv_font_fmt_txt_kern_pair_t));
        font->size += sizeof(lv_font_fmt_txt_kern_pair_t);
        memset(kern_pair, 0, sizeof(lv_font_fmt_txt_kern_pair_t));
        
        font_dsc->kern_dsc = kern_pair;
        font_dsc->kern_classes = 0;
        
        uint32_t glyph_entries;
        scui_font_src_seek(&font->font_src, offset + 12);
        scui_font_src_read(&font->font_src, &glyph_entries, 4);
        
        uint32_t ids_size = format == 0 ? sizeof(int8_t) * 2 * glyph_entries : sizeof(int16_t) * 2 * glyph_entries;
        
        uint8_t * glyph_ids = SCUI_MEM_ALLOC(scui_mem_type_font, ids_size);
        int8_t * values = SCUI_MEM_ALLOC(scui_mem_type_font, glyph_entries);
        font->size += ids_size;
        font->size += glyph_entries;
        
        kern_pair->glyph_ids_size = format;
        kern_pair->pair_cnt = glyph_entries;
        kern_pair->glyph_ids = glyph_ids;
        kern_pair->values = values;
        
        scui_font_src_seek(&font->font_src, offset + 16);
        scui_font_src_read(&font->font_src, glyph_ids, ids_size);
        scui_font_src_seek(&font->font_src, offset + 16 + ids_size);
        scui_font_src_read(&font->font_src, values, glyph_entries);
    }
    else if(3 == kern_format_type) { /*array M*N of classes*/
        lv_font_fmt_txt_kern_classes_t * kern_classes = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(lv_font_fmt_txt_kern_classes_t));
        font->size += sizeof(lv_font_fmt_txt_kern_classes_t);
        memset(kern_classes, 0, sizeof(lv_font_fmt_txt_kern_classes_t));
        
        font_dsc->kern_dsc = kern_classes;
        font_dsc->kern_classes = 1;
        
        uint16_t kern_class_mapping_length;
        uint8_t kern_table_rows;
        uint8_t kern_table_cols;
        
        scui_font_src_seek(&font->font_src, offset + 12 + 0);
        scui_font_src_read(&font->font_src, &kern_class_mapping_length, 2);
        scui_font_src_seek(&font->font_src, offset + 12 + 2);
        scui_font_src_read(&font->font_src, &kern_table_rows, 1);
        scui_font_src_seek(&font->font_src, offset + 12 + 3);
        scui_font_src_read(&font->font_src, &kern_table_cols, 1);
        
        uint32_t kern_values_length = sizeof(int8_t) * kern_table_rows * kern_table_cols;
        
        uint8_t * kern_left = SCUI_MEM_ALLOC(scui_mem_type_font, kern_class_mapping_length);
        uint8_t * kern_right = SCUI_MEM_ALLOC(scui_mem_type_font, kern_class_mapping_length);
        int8_t * kern_values = SCUI_MEM_ALLOC(scui_mem_type_font, kern_values_length);
        font->size += kern_class_mapping_length;
        font->size += kern_class_mapping_length;
        font->size += kern_values_length;
        
        kern_classes->left_class_mapping  = kern_left;
        kern_classes->right_class_mapping = kern_right;
        kern_classes->left_class_cnt = kern_table_rows;
        kern_classes->right_class_cnt = kern_table_cols;
        kern_classes->class_pair_values = kern_values;
        
        uintptr_t ofs_l = 0;
        uintptr_t ofs_r = kern_class_mapping_length;
        uintptr_t ofs_v = kern_class_mapping_length * 2;
        
        scui_font_src_seek(&font->font_src, offset + 16 + ofs_l);
        scui_font_src_read(&font->font_src, kern_left, kern_class_mapping_length);
        scui_font_src_seek(&font->font_src, offset + 16 + ofs_r);
        scui_font_src_read(&font->font_src, kern_right, kern_class_mapping_length);
        scui_font_src_seek(&font->font_src, offset + 16 + ofs_v);
        scui_font_src_read(&font->font_src, kern_values, kern_values_length);
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
    font->size += sizeof(lv_font_t);
    
    strcpy(font->name, name);
    scui_font_src_open(&font->font_src, font->name);
    
    lv_font_fmt_txt_dsc_t *font_dsc = SCUI_MEM_ALLOC(scui_mem_type_font, sizeof(lv_font_fmt_txt_dsc_t));
    font->size += sizeof(lv_font_fmt_txt_dsc_t);
    memset(font_dsc, 0, sizeof(lv_font_fmt_txt_dsc_t));
    font->dsc = font_dsc;
    
    /* bin[head] */
    uint32_t header_length = read_label(font, 0, "head");
    SCUI_ASSERT(header_length > 0);
    font_header_bin_t font_header;
    scui_font_src_seek(&font->font_src, 0 + 8);
    scui_font_src_read(&font->font_src, &font_header, sizeof(font_header_bin_t));
    
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
    uint32_t cmaps_length = load_cmaps(font, cmaps_offset, font_dsc);
    
    /* bin[loca] */
    uint32_t loca_offset = cmaps_offset + cmaps_length;
    uint32_t loca_length = read_label(font, loca_offset, "loca");
    
    uint32_t loca_count;
    scui_font_src_seek(&font->font_src, loca_offset + 8);
    scui_font_src_read(&font->font_src, &loca_count, 4);
    
    #if 0   // 不需要使用此字段信息
    uint32_t *glyph_offset = SCUI_MEM_ALLOC(scui_mem_type_font, 4 * (loca_count + 1));
    
    if (font_header.index_to_loc_format == 0) {
        uint16_t offset;
        for(uint32_t i = 0; i < loca_count; ++i) {
            scui_font_src_seek(&font->font_src, loca_offset + 12 + i * 2);
            scui_font_src_read(&font->font_src, &offset, 2);
            glyph_offset[i] = offset;
        }
    }
    if (font_header.index_to_loc_format == 1) {
        scui_font_src_seek(&font->font_src, loca_offset + 12);
        scui_font_src_read(&font->font_src, &glyph_offset, loca_count * 4);
    }
    #endif
    
    /* bin[glyph](外部扩充,该字段不加载,保存需要加载该字段的所有参数信息,转为动态加载) */
    uint32_t bin_glyph_offset = loca_offset + loca_length;
    uint32_t bin_glyph_langth = read_label(font, bin_glyph_offset, "glyf");
    // font->dsc.glyph_bitmap
    // font->dsc.glyph_src
    
    font->bin_head = font_header;
    font->bin_glyph_offset = bin_glyph_offset;
    font->bin_loca_offset = loca_offset;
    font->loca_length = loca_length;
    font->loca_count = loca_count;
    // font->glyph_offset = glyph_offset;
    
    /* bin[kern] */
    if(font_header.tables_count >= 4) {
        
        #if 0   // 不需要使用此字段信息
        uint32_t kern_offset = bin_glyph_offset + bin_glyph_langth;
        uint32_t kern_length = load_kern(font, kern_offset, font_dsc, font_header.glyph_id_format);
        #endif
    }
    
    scui_font_src_close(&font->font_src);
    return font;
}

static void lv_font_free(lv_font_t * font)
{
    if(NULL != font) {
        
        #if 0   // 不需要使用此字段信息
        if (font->glyph_offset != NULL)
            SCUI_MEM_FREE(font->glyph_offset);
        #endif
        
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

/*@brief 这里补充一个2进制流左移位接口
 *       只支持小于一字节的比特移位
 */
static void lv_font_util_shift_left2(uint8_t *operand, uint32_t length, uint64_t bits2)
{
    SCUI_ASSERT(bits2 != 0 && bits2 < 8);
    
    /* 只去高位移低位,最后一个字节不管(反正要丢弃) */
    for (uint32_t idx = 0; idx + 1 < length; idx++) {
         uint8_t bits_1 = operand[idx];
         uint8_t bits_2 = operand[idx + 1];
         uint8_t bits_low  = bits_1 << bits2;
         uint8_t bits_high = bits_2 >> (8 - bits2);
         operand[idx] = bits_low | bits_high;
    }
}

// lv_font_get_bitmap_fmt_txt
// lv_font_get_glyph_dsc_fmt_txt
// 原型改造,修订到可以直接动态获取字形和字体信息

static void lvgl_font_glpyh_load(lv_font_t *font, scui_font_glyph_t *glyph)
{
    uint32_t unicode_letter      = glyph->unicode_letter;
    uint32_t unicode_letter_next = glyph->unicode_letter_next;
    
    bool is_tab = false;
    if (unicode_letter == '\t') {
        unicode_letter  = ' ';
        is_tab = true;
    }
    lv_font_fmt_txt_dsc_t * fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
    uint32_t gid = get_glyph_dsc_id(font, unicode_letter);
    if (gid == 0)
        return;
    
    int8_t kvalue = 0;
    if(fdsc->kern_dsc) {
        uint32_t gid_next = get_glyph_dsc_id(font, unicode_letter_next);
        if(gid_next) {
            kvalue = get_kern_value(font, gid, gid_next);
        }
    }
    
    #if 1   // glyph_offset不存在,这里动态加载指定letter的偏移量
    
    font_header_bin_t *font_header = &font->bin_head;
    uintptr_t glyph_offset = font->bin_glyph_offset;
    uintptr_t loca_offset  = font->bin_loca_offset;
    uint32_t  loca_length  = font->loca_length;
    uint32_t  loca_count   = font->loca_count;
    uint32_t  offset1 = 0;
    uint32_t  offset2 = 0;
    
    if (font_header->index_to_loc_format == 0) {
        uint16_t offset_2[2];
        scui_font_src_seek(&font->font_src, loca_offset + 12 + gid * 2);
        scui_font_src_read(&font->font_src, offset_2, 2 * 2);
        offset1 = offset_2[0];
        offset2 = offset_2[1];
    }
    if (font_header->index_to_loc_format == 1) {
        uint32_t offset_2[2];
        scui_font_src_seek(&font->font_src, loca_offset + 12 + gid * 4);
        scui_font_src_read(&font->font_src, offset_2, 4 * 2);
        offset1 = offset_2[0];
        offset2 = offset_2[1];
    }
    
    /* glyph_src */
    /* glyph_bitmap */
    uint32_t glyph_length = read_label(font, glyph_offset, "glyf");
    
    /* 偏移到目标字符处 */
    scui_font_src_seek(&font->font_src, glyph_offset + offset1);
    
    bit_iterator_t bit_it = init_bit_iterator(font, 0);
    
    if (font_header->advance_width_bits == 0)
        glyph->adv_w = font_header->default_advance_width;
    else
        glyph->adv_w = read_bits(&bit_it, font_header->advance_width_bits);
    
    if (font_header->advance_width_format == 0)
        glyph->adv_w *= 16;
    
    glyph->ofs_x = read_bits_signed(&bit_it, font_header->xy_bits);
    glyph->ofs_y = read_bits_signed(&bit_it, font_header->xy_bits);
    glyph->box_w = read_bits(&bit_it, font_header->wh_bits);
    glyph->box_h = read_bits(&bit_it, font_header->wh_bits);
    
    if (!(glyph->ofs_x <= 128 && glyph->box_w < 63 && glyph->box_w != 0) ||
        !(glyph->ofs_y <= 128 && glyph->box_h < 63 && glyph->box_h != 0))
        return;
    
    uint32_t nbits = font_header->advance_width_bits + 2 * font_header->xy_bits + 2 * font_header->wh_bits;
    uint32_t next_offset = (gid < loca_count - 1) ? offset2 : glyph_length;
    uint32_t bitmap_size = next_offset - offset1 - nbits / 8;
    SCUI_ASSERT(bitmap_size > 0 && bitmap_size < 1024 * 1024);
    if (gid == 0) {
        glyph->adv_w = 0;
        glyph->box_w = 0;
        glyph->box_h = 0;
        glyph->ofs_x = 0;
        glyph->ofs_y = 0;
    }
    
    //空格字符是合法字符
    if (glyph->box_w != 0 && glyph->box_h != 0) {
        
        /* 生成内存并加载bitmap */
        glyph->bitmap = SCUI_MEM_ALLOC(scui_mem_type_font, bitmap_size);
        glyph->bitmap_size = bitmap_size;
        
        /* byte级 */
        if (nbits % 8 == 0) {
            scui_font_src_seek(&font->font_src, glyph_offset + offset1 + nbits / 8);
            scui_font_src_read(&font->font_src, glyph->bitmap, bitmap_size);
        } else {
            scui_font_src_seek(&font->font_src, glyph_offset + offset1 + nbits / 8);
            #if 1
            bit_it = init_bit_iterator(font, 0);
            read_bits(&bit_it, nbits % 8);
            /* 偏移到目标字符处 */
            // 这里不能直接读前一段,因为数据源不是byte对齐的地址,且双方的对齐无法同步
            for (uint32_t k = 0; k < bitmap_size - 1; k++)
                 glyph->bitmap[k] = read_bits(&bit_it, 8);
            glyph->bitmap[bitmap_size - 1] = read_bits(&bit_it, 8 - nbits % 8);
            /* The last fragment should be on the MSB but read_bits() will place it to the LSB */
            glyph->bitmap[bitmap_size - 1] = glyph->bitmap[bitmap_size - 1] << (nbits % 8);
            #else
            // 是否考虑先批量读完,然后再移位丢弃掉前面多余的部分
            uint8_t *bitmap_plus = SCUI_MEM_ALLOC(scui_mem_type_font, bitmap_size + 1);
            scui_font_src_read(&font->font_src, bitmap_plus, bitmap_size + 1);
            lv_font_util_shift_left2(bitmap_plus, bitmap_size + 1, nbits % 8);
            memcpy(glyph->bitmap, bitmap_plus, bitmap_size);
            /* The last fragment should be on the MSB but read_bits() will place it to the LSB */
            glyph->bitmap[bitmap_size - 1] = glyph->bitmap[bitmap_size - 1] << (nbits % 8);
            SCUI_MEM_FREE(bitmap_plus);
            #endif
        }
        
        /* 行程码压缩,使用bitmap源生成解压缩后的bitmap然后替换 */
        if (font_header->compression_id != LV_FONT_FMT_TXT_PLAIN) {
            uint32_t glyph_size = glyph->box_w * glyph->box_h;
            uint32_t decompress_size = glyph_size;
            
            /* Compute memory size needed to hold decompressed glyph, rounding up */
            switch (font_header->bits_per_pixel) {
                case 1: decompress_size = (glyph_size + 7) >> 3;   break;
                case 2: decompress_size = (glyph_size + 3) >> 2;   break;
                case 3: decompress_size = (glyph_size + 1) >> 1;   break;
                case 4: decompress_size = (glyph_size + 1) >> 1;   break;
            }
            
            uint8_t *compress_bitmap   = glyph->bitmap;
            uint8_t *decompress_bitmap = SCUI_MEM_ALLOC(scui_mem_type_font, decompress_size);
            bool prefilter = font_header->compression_id == LV_FONT_FMT_TXT_COMPRESSED ? true : false;
            
            /* 行程码解压缩 */
            decompress(compress_bitmap, decompress_bitmap, glyph->box_w, glyph->box_h, font_header->bits_per_pixel, prefilter);
            SCUI_MEM_FREE(compress_bitmap);
            glyph->bitmap = decompress_bitmap;
            glyph->bitmap_size = decompress_size;
        }
    }
    #endif
    
    /* Put together a glyph dsc */
    int32_t kv = ((int32_t)((int32_t)kvalue * fdsc->kern_scale) >> 4);
    uint32_t adv_w = glyph->adv_w;
    if (is_tab)
        adv_w *= 2;
    adv_w += kv;
    adv_w = (adv_w + (1 << 3)) >> 4;

    glyph->adv_w = adv_w * 16;
    glyph->bpp = (uint8_t)fdsc->bpp;
    if (is_tab)
        glyph->box_w = glyph->box_w * 2;
    
    if (fdsc->glyph_dsc != NULL)
        SCUI_MEM_FREE(fdsc->glyph_dsc);
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
    
    /* 只去支持1,2,4,8的bpp */
    uint8_t bpp = font->bin_head.bits_per_pixel;
    SCUI_ASSERT(scui_pow2_check(bpp));
}

/*@brief 字库卸载
 *@param handle 字库句柄
 */
void scui_font_unload(scui_handle_t handle)
{
    lv_font_t *font = scui_handle_get(handle);
    SCUI_ASSERT(font != NULL);
    lv_font_free(font);
    scui_handle_set(handle, NULL);
}

/*@brief 字库大小
 *@param handle 字库句柄
 *@retval 字库大小
 */
uint32_t scui_font_size(scui_handle_t handle)
{
    lv_font_t *font = scui_handle_get(handle);
    SCUI_ASSERT(font != NULL);
    
    return font->size;
}

/*@brief 字库参数信息
 *@param handle 字库句柄
 *@retval 字库参数信息
 */
scui_coord_t scui_font_base_line(scui_handle_t handle)
{
    lv_font_t *font = scui_handle_get(handle);
    SCUI_ASSERT(font != NULL);
    
    return font->base_line;
}

/*@brief 字库参数信息
 *@param handle 字库句柄
 *@retval 字库参数信息
 */
scui_coord_t scui_font_line_height(scui_handle_t handle)
{
    lv_font_t *font = scui_handle_get(handle);
    SCUI_ASSERT(font != NULL);
    
    return font->line_height;
}

/*@brief 字库参数信息
 *@param handle 字库句柄
 *@retval 字库参数信息
 */
scui_coord_t scui_font_underline(scui_handle_t handle)
{
    lv_font_t *font = scui_handle_get(handle);
    SCUI_ASSERT(font != NULL);
    
    return font->underline_position;
}

/*@brief 字型加载
 *@param glyph 字形信息
 */
void scui_font_glyph_load(scui_font_glyph_t *glyph)
{
    SCUI_ASSERT(glyph != NULL);
    SCUI_ASSERT(glyph->bitmap == NULL);
    lv_font_t *font = scui_handle_get(glyph->handle);
    SCUI_ASSERT(font != NULL);
    
    /* 只去支持1,2,4,8的bpp */
    uint8_t bpp = font->bin_head.bits_per_pixel;
    SCUI_ASSERT(scui_pow2_check(bpp));
    
    if (glyph->unicode_letter  < 0x20   ||
        glyph->unicode_letter == 0x20   || glyph->unicode_letter == 0xA0   ||
        glyph->unicode_letter == 0x200E || glyph->unicode_letter == 0x202A ||
        glyph->unicode_letter == 0x202C || glyph->unicode_letter == 0x202B) {
        
        if (glyph->space_width != 0) {
            glyph->ofs_x = 0;
            glyph->box_h = glyph->space_width;
            glyph->adv_w = glyph->space_width << 4;
            glyph->box_w = glyph->space_width;
        }
        return;
    }
    
    scui_font_src_open(&font->font_src, font->name);
    lvgl_font_glpyh_load(font, glyph);
    scui_font_src_close(&font->font_src);
    
    if (glyph->bitmap == NULL) {
        
        if (glyph->space_width != 0) {
            glyph->ofs_x = 0;
            glyph->box_h = glyph->space_width;
            glyph->adv_w = glyph->space_width << 4;
            glyph->box_w = glyph->space_width;
        }
    }
}

/*@brief 字型卸载
 *@param glyph 字形信息
 */
void scui_font_glyph_unload(scui_font_glyph_t *glyph)
{
    SCUI_MEM_FREE(glyph->bitmap);
}
