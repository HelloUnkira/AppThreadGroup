/*实现目标:
 *    cwf 数据源映射
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

/* 本表顺序与协议 scui_cwf_json_source 一致 */
const scui_cwf_json_source_info_t scui_cwf_json_source_info[scui_cwf_json_source_num] = {
    [scui_cwf_json_source_none]      = {0,     0},
    [scui_cwf_json_source_year]      = {0,    99},
    [scui_cwf_json_source_month]     = {1,    12},
    [scui_cwf_json_source_day]       = {1,    31},
    [scui_cwf_json_source_hour]      = {0,    23},
    [scui_cwf_json_source_minute]    = {0,    59},
    [scui_cwf_json_source_second]    = {0,    59},
    [scui_cwf_json_source_week]      = {0,     6},
    [scui_cwf_json_source_ampm]      = {0,     1},
    [scui_cwf_json_source_batt]      = {0,   100},
    [scui_cwf_json_source_hr]        = {0,   255},
    [scui_cwf_json_source_spo2]      = {0,   100},
    [scui_cwf_json_source_stress]    = {0,   100},
    [scui_cwf_json_source_step]      = {0, 99999},
    [scui_cwf_json_source_dist]      = {0,  9999},
    [scui_cwf_json_source_kcal]      = {0,  9999},
    [scui_cwf_json_source_temp]      = {0,  9999},
    [scui_cwf_json_source_dist_unit] = {0,     1},
    [scui_cwf_json_source_temp_unit] = {0,     1},
    [scui_cwf_json_source_anim]      = {0,     0},
};

/*@brief 取数据源当前值
 *@param source 数据源
 *@param val 值(输出)
 *@retval 是否取到
 */
bool scui_cwf_json_source_value(scui_cwf_json_source_t source, scui_coord_t *val)
{
    switch (source) {
    case scui_cwf_json_source_year:      *val = scui_presenter.get_year();      break;
    case scui_cwf_json_source_month:     *val = scui_presenter.get_mon();       break;
    case scui_cwf_json_source_day:       *val = scui_presenter.get_day();       break;
    case scui_cwf_json_source_hour:      *val = scui_presenter.get_hour();      break;
    case scui_cwf_json_source_minute:    *val = scui_presenter.get_min();       break;
    case scui_cwf_json_source_second:    *val = scui_presenter.get_sec();       break;
    case scui_cwf_json_source_week:      *val = scui_presenter.get_week();      break;
    case scui_cwf_json_source_ampm:      *val = scui_presenter.get_hour() >= 12 ? 1 : 0; break;
    case scui_cwf_json_source_batt:      *val = scui_presenter.get_batt_cur();  break;
    case scui_cwf_json_source_hr:        *val = scui_presenter.get_hr_cur();    break;
    case scui_cwf_json_source_spo2:      *val = scui_presenter.get_spo2_cur();  break;
    case scui_cwf_json_source_stress:    *val = scui_presenter.get_stress_cur(); break;
    case scui_cwf_json_source_step:      *val = scui_presenter.get_step_cur();  break;
    case scui_cwf_json_source_dist:      *val = scui_presenter.get_dist_cur();  break;
    case scui_cwf_json_source_kcal:      *val = scui_presenter.get_kcal_cur();  break;
    case scui_cwf_json_source_temp:      *val = scui_presenter.get_temp_cur();  break;
    case scui_cwf_json_source_temp_unit: *val = scui_presenter.get_temp_unit() ? 0 : 1; break;
    default: SCUI_LOG_WARN("cwf json source unsupported:%d", source); return false;
    }
    
    return true;
}

/*@brief 取数据源值域(有系统值域时优先取系统值域)
 *@param source  数据源
 *@param val_min 值域下限(输出)
 *@param val_max 值域上限(输出)
 */
void scui_cwf_json_source_range(scui_cwf_json_source_t source, scui_coord_t *val_min, scui_coord_t *val_max)
{
    switch (source) {
    case scui_cwf_json_source_batt:
        *val_min = scui_presenter.get_batt_min();
        *val_max = scui_presenter.get_batt_max();
        return;
    case scui_cwf_json_source_hr:
        *val_min = scui_presenter.get_hr_min();
        *val_max = scui_presenter.get_hr_max();
        return;
    case scui_cwf_json_source_spo2:
        *val_min = scui_presenter.get_spo2_min();
        *val_max = scui_presenter.get_spo2_max();
        return;
    case scui_cwf_json_source_stress:
        *val_min = scui_presenter.get_stress_min();
        *val_max = scui_presenter.get_stress_max();
        return;
    case scui_cwf_json_source_step:
        *val_min = scui_presenter.get_step_min();
        *val_max = scui_presenter.get_step_max();
        return;
    case scui_cwf_json_source_dist:
        *val_min = scui_presenter.get_dist_min();
        *val_max = scui_presenter.get_dist_max();
        return;
    case scui_cwf_json_source_kcal:
        *val_min = scui_presenter.get_kcal_min();
        *val_max = scui_presenter.get_kcal_max();
        return;
    default:
        break;
    }
    
    *val_min = scui_cwf_json_source_info[source].val_min;
    *val_max = scui_cwf_json_source_info[source].val_max;
}
