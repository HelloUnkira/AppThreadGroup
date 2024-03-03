#ifndef SCUI_EVENT_INDEV_H
#define SCUI_EVENT_INDEV_H

typedef enum {
    scui_event_key_val_up       = 17,   /*0x11*/
    scui_event_key_val_down     = 18,   /*0x12*/
    scui_event_key_val_right    = 19,   /*0x13*/
    scui_event_key_val_left     = 20,   /*0x14*/
    scui_event_key_val_esc      = 27,   /*0x1b*/
    scui_event_key_val_del      = 127,  /*0x7f*/
    scui_event_key_val_back     = 8,    /*0x08*/
    scui_event_key_val_enter    = 10,   /*0x0a, '\n'*/
    scui_event_key_val_next     = 9,    /*0x09, '\t'*/
    scui_event_key_val_prev     = 11,   /*0x0b, '*/
    scui_event_key_val_home     = 2,    /*0x02, stx*/
    scui_event_key_val_end      = 3,    /*0x03, etx*/
} scui_event_key_val_t;

#endif
