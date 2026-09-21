#ifndef SCUI_CWF_JSON_PROTO_H
#define SCUI_CWF_JSON_PROTO_H

#define SCUI_CWF_JSON_VER		{17, 1, 0, 0}

typedef enum {
	scui_cwf_json_pixel_cf_none = 0,
	scui_cwf_json_pixel_cf_alpha1,
	scui_cwf_json_pixel_cf_alpha2,
	scui_cwf_json_pixel_cf_alpha4,
	scui_cwf_json_pixel_cf_alpha8,
	scui_cwf_json_pixel_cf_index1,
	scui_cwf_json_pixel_cf_index2,
	scui_cwf_json_pixel_cf_index4,
	scui_cwf_json_pixel_cf_index8,
	scui_cwf_json_pixel_cf_bmp565,
	scui_cwf_json_pixel_cf_bmp888,
	scui_cwf_json_pixel_cf_bmp8565,
	scui_cwf_json_pixel_cf_bmp8888,
	scui_cwf_json_pixel_cf_num,
} scui_cwf_json_pixel_cf_t;

typedef enum {
	scui_cwf_json_image_cf_bmp = 0,
	scui_cwf_json_image_cf_idx,
	scui_cwf_json_image_cf_lz4,
	scui_cwf_json_image_cf_jpg,
	scui_cwf_json_image_cf_png,
	scui_cwf_json_image_cf_gif,
	scui_cwf_json_image_cf_lottie,
	scui_cwf_json_image_cf_mp4,
	scui_cwf_json_image_cf_mem,
	scui_cwf_json_image_cf_num,
} scui_cwf_json_image_cf_t;

typedef enum {
	scui_cwf_json_type_none = 0,
	scui_cwf_json_type_img,
	scui_cwf_json_type_map,
	scui_cwf_json_type_seq,
	scui_cwf_json_type_anim,
	scui_cwf_json_type_font,
	scui_cwf_json_type_group,
	scui_cwf_json_type_num,
} scui_cwf_json_type_t;

typedef enum {
	scui_cwf_json_source_none = 0,
	scui_cwf_json_source_year,
	scui_cwf_json_source_month,
	scui_cwf_json_source_day,
	scui_cwf_json_source_hour,
	scui_cwf_json_source_minute,
	scui_cwf_json_source_second,
	scui_cwf_json_source_week,
	scui_cwf_json_source_ampm,
	scui_cwf_json_source_batt,
	scui_cwf_json_source_hr,
	scui_cwf_json_source_spo2,
	scui_cwf_json_source_stress,
	scui_cwf_json_source_step,
	scui_cwf_json_source_dist,
	scui_cwf_json_source_kcal,
	scui_cwf_json_source_temp,
	scui_cwf_json_source_dist_unit,
	scui_cwf_json_source_temp_unit,
	scui_cwf_json_source_anim,
	scui_cwf_json_source_int,
	scui_cwf_json_source_float,
	scui_cwf_json_source_num,
} scui_cwf_json_source_t;

typedef enum {
	scui_cwf_json_key_one = 0,
	scui_cwf_json_key_layout,
	scui_cwf_json_key_preview,
	scui_cwf_json_key_watch,
	scui_cwf_json_key_num,
} scui_cwf_json_key_t;

typedef enum {
	scui_cwf_json_font_ttf = 0,
	scui_cwf_json_font_fixed,
	scui_cwf_json_font_num,
} scui_cwf_json_font_t;

typedef enum {
	scui_cwf_json_align_itl = 0,
	scui_cwf_json_align_itm,
	scui_cwf_json_align_itr,
	scui_cwf_json_align_ibl,
	scui_cwf_json_align_ibm,
	scui_cwf_json_align_ibr,
	scui_cwf_json_align_ilm,
	scui_cwf_json_align_irm,
	scui_cwf_json_align_icc,
	scui_cwf_json_align_otl,
	scui_cwf_json_align_otm,
	scui_cwf_json_align_otr,
	scui_cwf_json_align_obl,
	scui_cwf_json_align_obm,
	scui_cwf_json_align_obr,
	scui_cwf_json_align_olt,
	scui_cwf_json_align_olm,
	scui_cwf_json_align_olb,
	scui_cwf_json_align_ort,
	scui_cwf_json_align_orm,
	scui_cwf_json_align_orb,
	scui_cwf_json_align_num,
} scui_cwf_json_align_t;

typedef enum {
	scui_cwf_json_lang_en = 0,
	scui_cwf_json_lang_zh,
	scui_cwf_json_lang_de,
	scui_cwf_json_lang_fra,
	scui_cwf_json_lang_nl,
	scui_cwf_json_lang_pt,
	scui_cwf_json_lang_jp,
	scui_cwf_json_lang_ar,
	scui_cwf_json_lang_fa,
	scui_cwf_json_lang_num,
} scui_cwf_json_lang_t;

#define SCUI_CWF_JSON_PIXEL_CF_FONT		(0xFE)

#define SCUI_CWF_JSON_SEQ_SET	"0123456789-+%/:*"
#define SCUI_CWF_JSON_SEQ_NUM	16

#endif
