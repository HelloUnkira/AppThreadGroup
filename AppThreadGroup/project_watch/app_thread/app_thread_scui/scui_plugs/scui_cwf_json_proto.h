#ifndef SCUI_CWF_JSON_PROTO_H
#define SCUI_CWF_JSON_PROTO_H

#define SCUI_CWF_JSON_VER		{17, 1, 0, 0}

typedef enum {
	scui_cwf_json_format_none = 0,
	scui_cwf_json_format_palette4,
	scui_cwf_json_format_palette8,
	scui_cwf_json_format_bmp565,
	scui_cwf_json_format_bmp888,
	scui_cwf_json_format_bmp8565,
	scui_cwf_json_format_bmp8888,
	scui_cwf_json_format_num,
} scui_cwf_json_format_t;

typedef enum {
	scui_cwf_json_type_none = 0,
	scui_cwf_json_type_img,
	scui_cwf_json_type_txt,
	scui_cwf_json_type_num,
} scui_cwf_json_type_t;

typedef enum {
	scui_cwf_json_type_img_none = 0,
	scui_cwf_json_type_img_preview,
	scui_cwf_json_type_img_simple,
	scui_cwf_json_type_img_watch,
	scui_cwf_json_type_img_day,
	scui_cwf_json_type_img_hour,
	scui_cwf_json_type_img_minute,
	scui_cwf_json_type_img_second,
	scui_cwf_json_type_img_month,
	scui_cwf_json_type_img_week,
	scui_cwf_json_type_img_ampm,
	scui_cwf_json_type_img_hr_prog,
	scui_cwf_json_type_img_hr,
	scui_cwf_json_type_img_kcal_prog,
	scui_cwf_json_type_img_kcal,
	scui_cwf_json_type_img_step_prog,
	scui_cwf_json_type_img_step,
	scui_cwf_json_type_img_batt_prog,
	scui_cwf_json_type_img_batt,
	scui_cwf_json_type_img_dist_prog,
	scui_cwf_json_type_img_dist,
	scui_cwf_json_type_img_temp,
	scui_cwf_json_type_img_temp_unit,
	scui_cwf_json_type_img_anim,
	scui_cwf_json_type_img_num,
} scui_cwf_json_type_img_t;

typedef enum {
	scui_cwf_json_type_txt_none = 0,
	scui_cwf_json_type_txt_week,
	scui_cwf_json_type_txt_ampm,
	scui_cwf_json_type_txt_day,
	scui_cwf_json_type_txt_mon,
	scui_cwf_json_type_txt_digit_mon,
	scui_cwf_json_type_txt_digit_mon_day,
	scui_cwf_json_type_txt_day_digit_mon,
	scui_cwf_json_type_txt_bat,
	scui_cwf_json_type_txt_bat_pct,
	scui_cwf_json_type_txt_num,
} scui_cwf_json_type_txt_t;

#endif
