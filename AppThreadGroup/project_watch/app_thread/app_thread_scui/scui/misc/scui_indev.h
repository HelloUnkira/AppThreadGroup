#ifndef SCUI_INDEV_H
#define SCUI_INDEV_H

/* 输入设备类型: */
typedef enum {
    scui_indev_type_none,
    scui_indev_type_ptr,
    scui_indev_type_enc,
    scui_indev_type_key,
} scui_indev_type_t;

/* 按压状态: */
typedef enum {
    scui_indev_state_release = 0,
    scui_indev_state_press,
} scui_indev_state_t;

/* 输入设备数据 */
typedef struct {
    scui_indev_type_t  type;    /* 设备类型 */
    scui_indev_state_t state;   /* 设备状态 */
    union {
        /* 输入设备ptr */
        struct {
            scui_point_t ptr_pos;
        } ptr;
        /* 输入设备enc */
        struct {
            scui_coord_t enc_diff;
        } enc;
        /* 输入设备key */
        struct {
            scui_coord_t key_id;
            scui_coord_t key_val;
        } key;
    };
} scui_indev_data_t;

/*@brief 输入设备数据通报
 *@param data 数据
 */
void scui_indev_data_notify(scui_indev_data_t *data);

/*@brief 输入设备初始化
 */
void scui_indev_ready(void);

#endif
