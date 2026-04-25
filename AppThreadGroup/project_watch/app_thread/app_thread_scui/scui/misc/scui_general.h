#ifndef SCUI_GENERAL_H
#define SCUI_GENERAL_H

/* RGB565: */
#pragma pack(push, 1)
typedef union {
    uint16_t full;
    uint8_t  byte[2];
    struct {
        uint16_t b:5;
        uint16_t g:6;
        uint16_t r:5;
    } ch;
} scui_color565_t;
#pragma pack(pop)

/* RGB888: */
#pragma pack(push, 1)
typedef union {
    uint8_t byte[3];
    struct {
        uint32_t b:8;
        uint32_t g:8;
        uint32_t r:8;
    } ch;
} scui_color888_t;
#pragma pack(pop)

/* ARGB8565: */
#pragma pack(push, 1)
typedef union {
    uint8_t byte[3];
    struct {
        uint32_t b:5;
        uint32_t g:6;
        uint32_t r:5;
        uint32_t a:8;
    } ch;
} scui_color8565_t;
#pragma pack(pop)

/* ARGB8888: */
#pragma pack(push, 1)
typedef union {
    uint32_t full;
    uint8_t  byte[4];
    struct {
        uint32_t b:8;
        uint32_t g:8;
        uint32_t r:8;
        uint32_t a:8;
    } ch;
} scui_color8888_t;
#pragma pack(pop)

/* color wildcard type */
typedef uint32_t scui_color_wt_t;
/* color single type */
typedef scui_color8888_t scui_color32_t;

/*@brief: 过度色, 固定此格式主要上层通用性
 */
#pragma pack(push, 1)
typedef struct {
    union {
        scui_color32_t color;       /* 主色调 */
        scui_color32_t color_s;     /* 始色调 */
        scui_color32_t color_l;     /* 亮色调 */
    };
    union {
        scui_color32_t color_e;     /* 终色调 */
        scui_color32_t color_d;     /* 暗色调 */
    };
    struct {
        scui_color32_t color_f;     /* 滤色调 */
        uint8_t        filter:1;    /* 标记 */
    };
} scui_color_t;
#pragma pack(pop)

/* scui_color_t组合宏(这里用内联函数表出): */
static inline scui_color_t SCUI_COLOR_MAKE(bool filter, scui_color32_t color_f, scui_color32_t color)
{return (scui_color_t){ .color = color, .color_f = color_f, .filter = filter, };}
static inline scui_color_t SCUI_COLOR_MAKE_SE(bool filter, scui_color32_t color_f, scui_color32_t color_s, scui_color32_t color_e)
{return (scui_color_t){ .color_s = color_s, .color_e = color_e, .color_f = color_f, .filter = filter, };}
static inline scui_color_t SCUI_COLOR_MAKE_LD(bool filter, scui_color32_t color_f, scui_color32_t color_l, scui_color32_t color_d)
{return (scui_color_t){ .color_l = color_l, .color_d = color_d, .color_f = color_f, .filter = filter, };}

/* scui_color_t组合宏(这里用内联函数表出): */
static inline scui_color_t SCUI_COLOR_MAKE32(bool filter, uint32_t color_f, uint32_t color)
{return (scui_color_t){ .color.full = color, .color_f.full = color_f, .filter = filter, };}
static inline scui_color_t SCUI_COLOR_MAKE32_SE(bool filter, uint32_t color_f, uint32_t color_s, uint32_t color_e)
{return (scui_color_t){ .color_s.full = color_s, .color_e.full = color_e, .color_f.full = color_f, .filter = filter, };}
static inline scui_color_t SCUI_COLOR_MAKE32_LD(bool filter, uint32_t color_f, uint32_t color_l, uint32_t color_d)
{return (scui_color_t){ .color_l.full = color_l, .color_d.full = color_d, .color_f.full = color_f, .filter = filter, };}

/* scui_color_t组合宏(这里用内联函数表出): */
static inline scui_color32_t SCUI_COLOR32_MAKE8(uint8_t a8, uint8_t r8, uint8_t g8, uint8_t b8)
{return (scui_color32_t){.ch.a = a8, .ch.r = r8, .ch.g = g8, .ch.b = b8,};}
static inline scui_color32_t SCUI_COLOR32_MAKE32(uint32_t color)
{return (scui_color32_t){.full = color};}

/* scui_color_t常用值: */
#define SCUI_COLOR_UNUSED           SCUI_COLOR_MAKE32(false, 0x00000000, 0x00000000)
#define SCUI_COLOR_ZEROED           SCUI_COLOR_MAKE32(false, 0x00000000, 0x00000000)
#define SCUI_COLOR_FILTER_TRANS     SCUI_COLOR_MAKE32(true,  0x00000000, 0x00000000)
#define SCUI_COLOR_FILTER_BLACK     SCUI_COLOR_MAKE32(true,  0xFF000000, 0xFF000000)
#define SCUI_COLOR_BLACK            SCUI_COLOR_MAKE32(false, 0x00000000, 0xFF000000)
#define SCUI_COLOR_WHITE            SCUI_COLOR_MAKE32(false, 0x00000000, 0xFFFFFFFF)

/* scui_color_t常用值: */
#define SCUI_COLOR32_BLACK          SCUI_COLOR32_MAKE32(0xFF000000)
#define SCUI_COLOR32_WHITE          SCUI_COLOR32_MAKE32(0xFFFFFFFF)

/*@brief 调色板
 */
typedef enum {
    SCUI_PALETTE_RED,
    SCUI_PALETTE_PINK,
    SCUI_PALETTE_PURPLE,
    SCUI_PALETTE_DEEP_PURPLE,
    SCUI_PALETTE_INDIGO,
    SCUI_PALETTE_BLUE,
    SCUI_PALETTE_LIGHT_BLUE,
    SCUI_PALETTE_CYAN,
    SCUI_PALETTE_TEAL,
    SCUI_PALETTE_GREEN,
    SCUI_PALETTE_LIGHT_GREEN,
    SCUI_PALETTE_LIME,
    SCUI_PALETTE_YELLOW,
    SCUI_PALETTE_AMBER,
    SCUI_PALETTE_ORANGE,
    SCUI_PALETTE_DEEP_ORANGE,
    SCUI_PALETTE_BROWN,
    SCUI_PALETTE_BLUE_GREY,
    SCUI_PALETTE_GREY,
    
    SCUI_PALETTE_LAST,
    SCUI_PALETTE_NONE = 0xff,
} scui_palette_t;

/*@brief: 透明度枚举值
 *        设备透明度格式:
 *        固定到[0x00, 0xFF]
 */
#pragma pack(push, 1)
typedef enum {
    scui_alpha_pct0   = 0,
    scui_alpha_pct10  = 25,
    scui_alpha_pct20  = 51,
    scui_alpha_pct30  = 76,
    scui_alpha_pct40  = 102,
    scui_alpha_pct50  = 127,
    scui_alpha_pct60  = 153,
    scui_alpha_pct70  = 178,
    scui_alpha_pct80  = 204,
    scui_alpha_pct90  = 229,
    scui_alpha_pct100 = 255,
    scui_alpha_trans  = scui_alpha_pct0,
    scui_alpha_cover  = scui_alpha_pct100,
} scui_alpha_t;
#pragma pack(pop)

/* 透明度百分比 */
static inline scui_alpha_t scui_alpha_pct(uint8_t pct)
{return scui_map(pct, 0, 100, scui_alpha_pct0, scui_alpha_pct100);}
/* 透明度混合 */
static inline scui_alpha_t scui_alpha_mix(scui_alpha_t alpha1, scui_alpha_t alpha2)
{return SCUI_DIV_0xFF(((uint16_t)alpha1 * (uint16_t)alpha2));}

/*@brief: 设备像素点格式:
 *        设备像素点格式是直达显示器的帧缓冲或画布
 *        无需额外携带透明度是因为在渲染的过程中
 *        透明度通道会让颜色值加以吸收
 *@note:
 *      前8字节表明唯一类型
 *      后8字节表明数据尺寸
 */
typedef enum {
    scui_pixel_cf_def       = 0,
    scui_pixel_cf_type_mask = 0xFF00,
    scui_pixel_cf_bits_mask = 0x00FF,
    
    scui_pixel_cf_none      = 0,
    scui_pixel_cf_alpha4    = 0x0100 + 1 * 8 / 2,   /* alpha */
    scui_pixel_cf_alpha8    = 0x0100 + 1 * 8,       /* alpha */
    scui_pixel_cf_bmp565    = 0x0200 + 2 * 8,       /* bitmap */
    scui_pixel_cf_bmp888    = 0x0200 + 3 * 8,       /* bitmap */
    scui_pixel_cf_bmp8565   = 0x0300 + 3 * 8,       /* bitmap */
    scui_pixel_cf_bmp8888   = 0x0300 + 4 * 8,       /* bitmap */
    scui_pixel_cf_all,
} scui_pixel_cf_t;

/* 像素点比特位,字节数 */
static inline scui_coord_t scui_pixel_bits(scui_pixel_cf_t cf)
{return (cf & scui_pixel_cf_bits_mask);}
static inline scui_coord_t scui_pixel_byte(scui_pixel_cf_t cf)
{return (cf & scui_pixel_cf_bits_mask) >> 3;}
static inline bool scui_pixel_type_bmp(scui_pixel_cf_t cf)
{return (cf & scui_pixel_cf_type_mask) != 0x0100;}
static inline bool scui_pixel_alpha_in(scui_pixel_cf_t cf)
{return (cf & scui_pixel_cf_type_mask) != 0x0200;}

/*@brief 实体抽象:画布
 *       画布是块独立缓冲区或共享绘制画布的缓冲区
 */
typedef struct {
    uint8_t        *pixel;      /* 像素流地址 */
    scui_coord_t    pbyte;      /* 像素字节数 */
    scui_pixel_cf_t format;     /* 像素流类型 */
    scui_coord_t    hor_res;    /* 像素流宽度 */
    scui_coord_t    ver_res;    /* 像素流高度 */
    scui_multi_t    stride;     /* 像素流跨度 */
    scui_alpha_t    alpha;      /* 全局透明度 */
} scui_surface_t;

/* 画布偏移地址,画布坐标偏移地址 */
#if 0
#define scui_surface_point_ofs(surface, y, x)   ((y) * (surface)->hor_res + (x))
#define scui_surface_pbyte_ofs(surface, y, x)   ((y) * (surface)->stride  + (x) * (surface)->pbyte)
#define scui_surface_pixel_ofs(surface, y, x)   ((surface)->pixel + scui_surface_pbyte_ofs(surface, y, x))
#else
static inline scui_multi_t scui_surface_point_ofs(scui_surface_t *surface, scui_coord_t y, scui_coord_t x)
{
    return y * surface->hor_res + x;
}
static inline scui_multi_t scui_surface_pbyte_ofs(scui_surface_t *surface, scui_coord_t y, scui_coord_t x)
{
    SCUI_ASSERT(surface->pbyte == scui_pixel_bits(surface->format) / 8);
    SCUI_ASSERT(surface->stride == surface->hor_res * scui_pixel_bits(surface->format) / 8);
    return y * surface->stride + x * surface->pbyte;
}
static inline uint8_t * scui_surface_pixel_ofs(scui_surface_t *surface, scui_coord_t y, scui_coord_t x)
{
    return surface->pixel + scui_surface_pbyte_ofs(surface, y, x);
}
#endif


/* 画布区域 */
static inline scui_area_t scui_surface_area(scui_surface_t *surface)
{return (scui_area_t){.w = surface->hor_res,.h = surface->ver_res,};}

/*@brief 操作方向(与操作位置透明切换)
 */
typedef enum {
    scui_opt_dir_none = (0),
    scui_opt_dir_to_u = (1 << 0),   /* 向up */
    scui_opt_dir_to_d = (1 << 1),   /* 向down */
    scui_opt_dir_to_l = (1 << 2),   /* 向left */
    scui_opt_dir_to_r = (1 << 3),   /* 向right */
    scui_opt_dir_ver  = (scui_opt_dir_to_u | scui_opt_dir_to_d),
    scui_opt_dir_hor  = (scui_opt_dir_to_l | scui_opt_dir_to_r),
    scui_opt_dir_all  = (scui_opt_dir_ver  | scui_opt_dir_hor ),
} scui_opt_dir_t;

/*@brief 操作位置(与操作方向透明切换)
 */
typedef enum {
    scui_opt_pos_none = scui_opt_dir_none,
    scui_opt_pos_u    = scui_opt_dir_to_d,
    scui_opt_pos_d    = scui_opt_dir_to_u,
    scui_opt_pos_l    = scui_opt_dir_to_r,
    scui_opt_pos_r    = scui_opt_dir_to_l,
    scui_opt_pos_c    = scui_opt_dir_all,
    scui_opt_pos_ver  = scui_opt_dir_ver,
    scui_opt_pos_hor  = scui_opt_dir_hor,
    scui_opt_pos_all  = scui_opt_dir_all,
} scui_opt_pos_t;

typedef enum {
    /* 0x yy xx */
    scui_opt_align_mask_ixl = 0x0001,
    scui_opt_align_mask_ixm = 0x0002,
    scui_opt_align_mask_ixr = 0x0004,
    scui_opt_align_mask_oxl = 0x0010,
    scui_opt_align_mask_oxr = 0x0020,
    
    scui_opt_align_mask_iyt = 0x0100,
    scui_opt_align_mask_iym = 0x0200,
    scui_opt_align_mask_iyb = 0x0400,
    scui_opt_align_mask_oyt = 0x1000,
    scui_opt_align_mask_oyb = 0x2000,
    
    scui_opt_align_itl = 0x0101,    /* in top left */
    scui_opt_align_itm = 0x0102,    /* in top middle */
    scui_opt_align_itr = 0x0104,    /* in top right */
    scui_opt_align_ibl = 0x0401,    /* in bottom left */
    scui_opt_align_ibm = 0x0402,    /* in bottom middle */
    scui_opt_align_ibr = 0x0404,    /* in bottom right */
    scui_opt_align_ilm = 0x0201,    /* in left middle */
    scui_opt_align_irm = 0x0204,    /* in right middle */
    scui_opt_align_icc = 0x0202,    /* in center center */
    
    scui_opt_align_otl = 0x1001,    /* out top left */
    scui_opt_align_otm = 0x1002,    /* out top middle */
    scui_opt_align_otr = 0x1004,    /* out top right */
    scui_opt_align_obl = 0x2001,    /* out bottom left */
    scui_opt_align_obm = 0x2002,    /* out bottom middle */
    scui_opt_align_obr = 0x2004,    /* out bottom right */
    
    scui_opt_align_olt = 0x0110,    /* out left top */
    scui_opt_align_olm = 0x0210,    /* out left middle */
    scui_opt_align_olb = 0x0410,    /* out left bottom */
    scui_opt_align_ort = 0x0120,    /* out right top */
    scui_opt_align_orm = 0x0220,    /* out right middle */
    scui_opt_align_orb = 0x0420,    /* out right bottom */
    
} scui_opt_align_t;

/*@brief 通用结构体位域
 */
typedef uintptr_t scui_sbitfd_t;

/*@brief 操作检查
 */
#define scui_opt_bits_equal(bits, mask)     (((bits) & (mask)) == (mask))
#define scui_opt_bits_check(bits, mask)     (((bits) & (mask)) != (0))

/*@brief 颜色值混合color32
 *@param color32   颜色值color32
 *@param color32_c 颜色值color32
 *@param color32_t 颜色值color32
 *@param pct_c     color32_c百分比
 */
void scui_color32_mix_with(scui_color32_t *color32, scui_color32_t *color32_c, scui_color32_t *color32_t, scui_coord_t pct_c);

/*@brief 调色板生成颜色值
 *@param color   颜色值
 *@param palette 调色板
 *@param type    调色板类型(0:主调色板;1:亮调色板;2:暗调色板)
 *@param level   亮度等级(0~5)
 */
void scui_color_by_palette(scui_color_t *color, scui_palette_t palette, uint8_t type, uint8_t level);

/*@brief 像素点是否有透明度
 *@param cf 像素点格式
 *@retval 是否有透明度
 */
bool scui_pixel_alpha_in(scui_pixel_cf_t cf);

/*@brief 像素点配置
 *@param cf    像素点格式
 *@param pixel 像素点
 *@param tar   像素点(目标值)
 */
void scui_pixel_by_cf(scui_pixel_cf_t cf, void *pixel, void *tar);

/*@brief 颜色值转换(仅scui_color_t类型的内部元素转换)
 *@param cf    像素点格式
 *@param pixel 像素点
 *@param color 颜色值
 */
void scui_pixel_by_color(scui_pixel_cf_t cf, void *pixel, scui_color8888_t color);

/*@brief 像素点作用透明度
 *@param cf    像素点格式
 *@param pixel 像素点
 *@param alpha 透明度
 */
void scui_pixel_mix_alpha(scui_pixel_cf_t cf, void *pixel, scui_alpha_t alpha);

/*@brief 像素点融合(同步作用透明度)
 *@param dst_cf 目标像素点格式
 *@param dst_p  目标像素点
 *@param src_cf 源像素点格式
 *@param src_p  源像素点
 *@param src_a  源像素点透明度
 */
void scui_pixel_mix_with(scui_pixel_cf_t dst_cf, void *dst_p,
                         scui_pixel_cf_t src_cf, void *src_p, scui_alpha_t src_a);

/*@brief 像素点生成灰度值
 *@param cf    像素点格式
 *@param pixel 像素点
 *@retval grey 灰度值
 */
uint8_t scui_pixel_grey_by(scui_pixel_cf_t cf, void *pixel);

/*@brief 计算灰度值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 灰度值
 */
uint8_t scui_pixel_grey_bpp_x(uint8_t bitmap, uint8_t bpp, uint8_t bpp_x);

/*@brief 透明度混合撤销
 *@param alpha1 透明度1
 *@param alpha2 透明度2
 *@retval 混合撤销后的透明度
 */
scui_alpha_t scui_alpha_undo(scui_alpha_t alpha1, scui_alpha_t alpha2);

#endif
