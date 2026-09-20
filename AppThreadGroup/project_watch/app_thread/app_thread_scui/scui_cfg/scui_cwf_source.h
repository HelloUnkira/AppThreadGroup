#ifndef SCUI_CWF_SOURCE_H
#define SCUI_CWF_SOURCE_H

/* 数据源天然值域(协议: 值域不填时用 source 天然值域) */
typedef struct {
    scui_coord_t val_min;
    scui_coord_t val_max;
} scui_cwf_json_source_info_t;

/* 本表顺序与协议 scui_cwf_json_source 一致 */
extern const scui_cwf_json_source_info_t scui_cwf_json_source_info[scui_cwf_json_source_num];

/*@brief 取数据源当前值
 *@param source 数据源
 *@param val 值(输出)
 *@retval 是否取到
 */
bool scui_cwf_json_source_value(scui_cwf_json_source_t source, scui_coord_t *val);

/*@brief 取数据源值域(有系统值域时优先取系统值域)
 *@param source  数据源
 *@param val_min 值域下限(输出)
 *@param val_max 值域上限(输出)
 */
void scui_cwf_json_source_range(scui_cwf_json_source_t source, scui_coord_t *val_min, scui_coord_t *val_max);

#endif
