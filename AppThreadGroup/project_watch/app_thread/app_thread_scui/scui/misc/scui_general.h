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
        scui_color32_t color;       // 主色调
        scui_color32_t color_s;     // 始色调
        scui_color32_t color_l;     // 亮色调
    };
    union {
        scui_color32_t color_e;     // 终色调
        scui_color32_t color_d;     // 暗色调
    };
    struct {
        scui_color32_t color_f;     // 滤色调
        uint8_t        filter:1;    // 标记
    };
} scui_color_t;
#pragma pack(pop)

/* scui_color_t组合宏(这里用内联函数表出): */
static inline scui_color_t SCUI_COLOR_MAKE32(bool filter, uint32_t color_f, uint32_t color)
{return (scui_color_t){ .color.full = color, .color_f.full = color_f, .filter = filter, };}
static inline scui_color_t SCUI_COLOR_MAKE32_SE(bool filter, uint32_t color_f, uint32_t color_s, uint32_t color_e)
{return (scui_color_t){ .color_s.full = color_s, .color_e.full = color_e, .color_f.full = color_f, .filter = filter, };}
static inline scui_color_t SCUI_COLOR_MAKE32_LD(bool filter, uint32_t color_f, uint32_t color_l, uint32_t color_d)
{return (scui_color_t){ .color_l.full = color_l, .color_d.full = color_d, .color_f.full = color_f, .filter = filter, };}

/* scui_color_t常用值: */
#define SCUI_COLOR_UNUSED           SCUI_COLOR_MAKE32(false, 0x00000000, 0x00000000)
#define SCUI_COLOR_ZEROED           SCUI_COLOR_MAKE32(false, 0x00000000, 0x00000000)
#define SCUI_COLOR_FILTER_TRANS     SCUI_COLOR_MAKE32(true,  0x00000000, 0x00000000)
#define SCUI_COLOR_FILTER_BLACK     SCUI_COLOR_MAKE32(true,  0xFF000000, 0xFF000000)



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
    scui_pixel_cf_alpha4    = 0x0100 + 1 * 8 / 2,   // alpha
    scui_pixel_cf_alpha8    = 0x0100 + 1 * 8,       // alpha
    scui_pixel_cf_bmp565    = 0x0200 + 2 * 8,       // bitmap
    scui_pixel_cf_bmp888    = 0x0200 + 3 * 8,       // bitmap
    scui_pixel_cf_bmp8565   = 0x0300 + 3 * 8,       // bitmap
    scui_pixel_cf_bmp8888   = 0x0300 + 4 * 8,       // bitmap
    scui_pixel_cf_all,
} scui_pixel_cf_t;

/*@brief 实体抽象:画布
 *       画布是块独立缓冲区或共享绘制画布的缓冲区
 */
typedef struct {
    uint8_t        *pixel;      // 画布像素流地址
    scui_pixel_cf_t format;     // 画布像素流类型
    scui_coord_t    hor_res;    // 画布像素流宽度
    scui_coord_t    ver_res;    // 画布像素流高度
    scui_alpha_t    alpha;      // 画布全局透明度
} scui_surface_t;

/*@brief 操作方向(与操作位置透明切换)
 */
typedef enum {
    scui_opt_dir_none = (0),
    scui_opt_dir_to_u = (1 << 0),   // 向up
    scui_opt_dir_to_d = (1 << 1),   // 向down
    scui_opt_dir_to_l = (1 << 2),   // 向left
    scui_opt_dir_to_r = (1 << 3),   // 向right
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

/*@brief 操作检查
 */
#define scui_opt_bits_equal(bits, mask)     (((bits) & (mask)) == (mask))
#define scui_opt_bits_check(bits, mask)     (((bits) & (mask)) != (0))



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
 *@param cf_1    像素点格式
 *@param pixel_1 像素点
 *@param alpha_1 透明度
 *@param cf_2    像素点格式
 *@param pixel_2 像素点
 *@param alpha_2 透明度
 */
void scui_pixel_mix_with(scui_pixel_cf_t cf_1, void *pixel_1, scui_alpha_t alpha_1,
                         scui_pixel_cf_t cf_2, void *pixel_2, scui_alpha_t alpha_2);

/*@brief 像素点生成调色板值
 *@param cf      像素点格式
 *@param pixel   像素点
 *@param palette 调色板值
 */
void scui_palette_by_pixel(scui_pixel_cf_t cf, void *pixel, uint8_t *palette);

/*@brief 计算灰度值
 *@param bitmap 位图
 *@param bpp_x  偏移值
 *@retval 灰度值
 */
uint8_t scui_grey_bpp_x(uint8_t bitmap, uint8_t bpp, uint8_t bpp_x);

/*@brief 透明度混合
 *@param alpha1 透明度1
 *@param alpha2 透明度2
 *@retval 混合后的透明度
 */
scui_alpha_t scui_alpha_mix(scui_alpha_t alpha1, scui_alpha_t alpha2);

/*@brief 透明度混合撤销
 *@param alpha1 透明度1
 *@param alpha2 透明度2
 *@retval 混合撤销后的透明度
 */
scui_alpha_t scui_alpha_undo(scui_alpha_t alpha1, scui_alpha_t alpha2);

/*@brief 计算透明度通过百分比值
 *@param pct 透明度百分比值[0, 100]
 *@retval 透明度
 */
scui_alpha_t scui_alpha_pct(uint8_t pct);

/*@brief 像素点比特位数
 *@param cf 像素点格式
 *@retval 比特位数
 */
scui_coord_t scui_pixel_bits(scui_pixel_cf_t cf);

/*@brief 像素点是否有透明度
 *@param cf 像素点格式
 *@retval 是否有透明度
 */
bool scui_pixel_have_alpha(scui_pixel_cf_t cf);

/*@brief 画布有效区域
 *@param surface 画布实例
 *@retval 区域
 */
scui_area_t scui_surface_area(scui_surface_t *surface);

#endif
