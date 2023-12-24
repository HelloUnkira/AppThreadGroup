#ifndef SCUI_COLOR_H
#define SCUI_COLOR_H

/* alpha: */
typedef uint8_t scui_alpha_t;

/* RGB565: */
#pragma pack(1)
typedef union {
    struct {
        uint8_t b:5;
        uint8_t g:6;
        uint8_t r:5;
    } ch;
    uint8_t byte[2];
} scui_color565_t;
#pragma pack()

/* RGB888: */
#pragma pack(1)
typedef union {
    struct {
        uint8_t b:8;
        uint8_t g:8;
        uint8_t r:8;
    } ch;
    uint8_t byte[3];
} scui_color888_t;
#pragma pack()

/* ARGB6666: */
#pragma pack(1)
typedef union {
    struct {
        uint8_t b:6;
        uint8_t g:6;
        uint8_t r:6;
        uint8_t a:6;
    } ch;
    uint8_t byte[3];
} scui_color6666_t;
#pragma pack()

/* ARGB8888: */
#pragma pack(1)
typedef union {
    struct {
        uint8_t b:8;
        uint8_t g:8;
        uint8_t r:8;
        uint8_t a:8;
    } ch;
    uint8_t byte[4];
} scui_color8888_t;
#pragma pack()

/* 几个不同像素格式转化(EX指字节逆序) */
#define SCUI_RGB888_TO_565(x)        ((((x) >> 19 & 0x1F) << 11) | (((x) >> 10 & 0x3F) << 5) | (((x) >> 3 & 0x1F) <<  0))
#define SCUI_RGB888_TO_565_EX(x)     ((((x) >> 19 & 0x1F) <<  0) | (((x) >> 10 & 0x38) << 5) | (((x) >> 3 & 0x1F) << 11) | (((x) >> 10 & 0x07) << 8))
#define SCUI_RGB666_TO_565(x)        ((((x) >> 13 & 0x1F) << 11) | (((x) >>  6 & 0x3F) << 5) | (((x) >> 1 & 0x1F) <<  0))
#define SCUI_RGB666_TO_565_EX(x)     ((((x) >> 13 & 0x1F) <<  0) | (((x) >>  6 & 0x38) << 5) | (((x) >> 1 & 0x1F) << 11) | (((x) >>  6 & 0x07) << 8))

#endif
