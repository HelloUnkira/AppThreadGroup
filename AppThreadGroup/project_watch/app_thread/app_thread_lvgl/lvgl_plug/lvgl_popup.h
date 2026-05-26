#ifndef LVGL_POPUP_H
#define LVGL_POPUP_H

// 可选事件优先级, 按等级(准备了7个,不够就自行调整数字)
#define LVGL_POPUP_PRIO_LOWEST                (0x01)
#define LVGL_POPUP_PRIO_LOW                   (0x10)
#define LVGL_POPUP_PRIO_NORMAL_BELOW          (0x70)
#define LVGL_POPUP_PRIO_NORMAL                (0x80)
#define LVGL_POPUP_PRIO_NORMAL_ABOVE          (0x90)
#define LVGL_POPUP_PRIO_HIGH                  (0xE0)
#define LVGL_POPUP_PRIO_HIGHEST               (0xFF)

typedef struct
{
    uint32_t page_id;       // 界面id号
    uint32_t prio;          // 优先级, 数字越大优先级越高
} lvgl_popup_t;

void lvgl_popup_ready(void);
void lvgl_popup_notify(uint32_t type);
void lvgl_popup_quit(uint32_t type);

#endif
