/*实现目标:
 *    nanopb消息结构体(AppPB_XXX)与JSON对象的通用转换层
 *    由 app_json_xfer_parser.py 自动生成, 请勿手动修改
 */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_protocol_lib.h"

#if APP_MODULE_PROTOCOL_USE_JSON

/*@brief 打包转换 ACK 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_ack(const AppPB_ACK *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "error_code", msg->error_code);
    cJSON_AddNumberToObject(obj, "type", msg->type);
    cJSON_AddNumberToObject(obj, "index", msg->index);
    return obj;
}

/*@brief 解析转换 JSON 对象为 ACK 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_ack(cJSON *obj, AppPB_ACK *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *error_code_item = cJSON_GetObjectItem(obj, "error_code");
    if (error_code_item == NULL)
        return false;
    msg->error_code = (uint64_t)cJSON_GetNumberValue(error_code_item);
    cJSON *type_item = cJSON_GetObjectItem(obj, "type");
    if (type_item == NULL)
        return false;
    msg->type = (uint64_t)cJSON_GetNumberValue(type_item);
    cJSON *index_item = cJSON_GetObjectItem(obj, "index");
    if (index_item == NULL)
        return false;
    msg->index = (uint64_t)cJSON_GetNumberValue(index_item);
    return true;
}

/*@brief 打包转换 Account 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_account(const AppPB_Account *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddStringToObject(obj, "account", msg->account);
    cJSON_AddNumberToObject(obj, "acc_len", msg->acc_len);
    cJSON_AddNumberToObject(obj, "pair_state", msg->pair_state);
    cJSON_AddNumberToObject(obj, "app_role", msg->app_role);
    return obj;
}

/*@brief 解析转换 JSON 对象为 Account 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_account(cJSON *obj, AppPB_Account *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *account_item = cJSON_GetObjectItem(obj, "account");
    if (account_item == NULL)
        return false;
    snprintf(msg->account, sizeof(msg->account), "%s", cJSON_GetStringValue(account_item));
    cJSON *acc_len_item = cJSON_GetObjectItem(obj, "acc_len");
    if (acc_len_item == NULL)
        return false;
    msg->acc_len = (uint64_t)cJSON_GetNumberValue(acc_len_item);
    cJSON *pair_state_item = cJSON_GetObjectItem(obj, "pair_state");
    if (pair_state_item == NULL)
        return false;
    msg->pair_state = (uint64_t)cJSON_GetNumberValue(pair_state_item);
    cJSON *app_role_item = cJSON_GetObjectItem(obj, "app_role");
    if (app_role_item == NULL)
        return false;
    msg->app_role = (uint64_t)cJSON_GetNumberValue(app_role_item);
    return true;
}

/*@brief 打包转换 Alarm 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_alarm(const AppPB_Alarm *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "index", msg->index);
    cJSON_AddNumberToObject(obj, "repeat", msg->repeat);
    cJSON_AddNumberToObject(obj, "on", msg->on);
    cJSON_AddNumberToObject(obj, "hour", msg->hour);
    cJSON_AddNumberToObject(obj, "min", msg->min);
    cJSON_AddStringToObject(obj, "name", msg->name);
    return obj;
}

/*@brief 解析转换 JSON 对象为 Alarm 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_alarm(cJSON *obj, AppPB_Alarm *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *index_item = cJSON_GetObjectItem(obj, "index");
    if (index_item == NULL)
        return false;
    msg->index = (uint64_t)cJSON_GetNumberValue(index_item);
    cJSON *repeat_item = cJSON_GetObjectItem(obj, "repeat");
    if (repeat_item == NULL)
        return false;
    msg->repeat = (uint64_t)cJSON_GetNumberValue(repeat_item);
    cJSON *on_item = cJSON_GetObjectItem(obj, "on");
    if (on_item == NULL)
        return false;
    msg->on = (uint64_t)cJSON_GetNumberValue(on_item);
    cJSON *hour_item = cJSON_GetObjectItem(obj, "hour");
    if (hour_item == NULL)
        return false;
    msg->hour = (uint64_t)cJSON_GetNumberValue(hour_item);
    cJSON *min_item = cJSON_GetObjectItem(obj, "min");
    if (min_item == NULL)
        return false;
    msg->min = (uint64_t)cJSON_GetNumberValue(min_item);
    cJSON *name_item = cJSON_GetObjectItem(obj, "name");
    if (name_item == NULL)
        return false;
    snprintf(msg->name, sizeof(msg->name), "%s", cJSON_GetStringValue(name_item));
    return true;
}

/*@brief 打包转换 Contact 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_contact(const AppPB_Contact *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddStringToObject(obj, "name", msg->name);
    cJSON_AddNumberToObject(obj, "name_len", msg->name_len);
    cJSON_AddStringToObject(obj, "phone", msg->phone);
    cJSON_AddNumberToObject(obj, "phone_len", msg->phone_len);
    cJSON_AddNumberToObject(obj, "state", msg->state);
    return obj;
}

/*@brief 解析转换 JSON 对象为 Contact 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_contact(cJSON *obj, AppPB_Contact *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *name_item = cJSON_GetObjectItem(obj, "name");
    if (name_item == NULL)
        return false;
    snprintf(msg->name, sizeof(msg->name), "%s", cJSON_GetStringValue(name_item));
    cJSON *name_len_item = cJSON_GetObjectItem(obj, "name_len");
    if (name_len_item == NULL)
        return false;
    msg->name_len = (uint64_t)cJSON_GetNumberValue(name_len_item);
    cJSON *phone_item = cJSON_GetObjectItem(obj, "phone");
    if (phone_item == NULL)
        return false;
    snprintf(msg->phone, sizeof(msg->phone), "%s", cJSON_GetStringValue(phone_item));
    cJSON *phone_len_item = cJSON_GetObjectItem(obj, "phone_len");
    if (phone_len_item == NULL)
        return false;
    msg->phone_len = (uint64_t)cJSON_GetNumberValue(phone_len_item);
    cJSON *state_item = cJSON_GetObjectItem(obj, "state");
    if (state_item == NULL)
        return false;
    msg->state = (uint64_t)cJSON_GetNumberValue(state_item);
    return true;
}

/*@brief 打包转换 DevInfo 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_dev_info(const AppPB_DevInfo *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddStringToObject(obj, "model", msg->model);
    cJSON_AddStringToObject(obj, "hw_ver", msg->hw_ver);
    cJSON_AddStringToObject(obj, "sw_ver", msg->sw_ver);
    cJSON_AddStringToObject(obj, "sn", msg->sn);
    cJSON_AddStringToObject(obj, "bt_addr", msg->bt_addr);
    cJSON_AddStringToObject(obj, "pid", msg->pid);
    cJSON_AddNumberToObject(obj, "battery", msg->battery);
    return obj;
}

/*@brief 解析转换 JSON 对象为 DevInfo 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_dev_info(cJSON *obj, AppPB_DevInfo *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *model_item = cJSON_GetObjectItem(obj, "model");
    if (model_item == NULL)
        return false;
    snprintf(msg->model, sizeof(msg->model), "%s", cJSON_GetStringValue(model_item));
    cJSON *hw_ver_item = cJSON_GetObjectItem(obj, "hw_ver");
    if (hw_ver_item == NULL)
        return false;
    snprintf(msg->hw_ver, sizeof(msg->hw_ver), "%s", cJSON_GetStringValue(hw_ver_item));
    cJSON *sw_ver_item = cJSON_GetObjectItem(obj, "sw_ver");
    if (sw_ver_item == NULL)
        return false;
    snprintf(msg->sw_ver, sizeof(msg->sw_ver), "%s", cJSON_GetStringValue(sw_ver_item));
    cJSON *sn_item = cJSON_GetObjectItem(obj, "sn");
    if (sn_item == NULL)
        return false;
    snprintf(msg->sn, sizeof(msg->sn), "%s", cJSON_GetStringValue(sn_item));
    cJSON *bt_addr_item = cJSON_GetObjectItem(obj, "bt_addr");
    if (bt_addr_item == NULL)
        return false;
    snprintf(msg->bt_addr, sizeof(msg->bt_addr), "%s", cJSON_GetStringValue(bt_addr_item));
    cJSON *pid_item = cJSON_GetObjectItem(obj, "pid");
    if (pid_item == NULL)
        return false;
    snprintf(msg->pid, sizeof(msg->pid), "%s", cJSON_GetStringValue(pid_item));
    cJSON *battery_item = cJSON_GetObjectItem(obj, "battery");
    if (battery_item == NULL)
        return false;
    msg->battery = (uint64_t)cJSON_GetNumberValue(battery_item);
    return true;
}

/*@brief 打包转换 DevParam 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_dev_param(const AppPB_DevParam *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "lang_id", msg->lang_id);
    cJSON_AddNumberToObject(obj, "zone", msg->zone);
    cJSON_AddNumberToObject(obj, "is_12h", msg->is_12h);
    cJSON_AddNumberToObject(obj, "is_mi", msg->is_mi);
    cJSON_AddNumberToObject(obj, "brt", msg->brt);
    return obj;
}

/*@brief 解析转换 JSON 对象为 DevParam 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_dev_param(cJSON *obj, AppPB_DevParam *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *lang_id_item = cJSON_GetObjectItem(obj, "lang_id");
    if (lang_id_item == NULL)
        return false;
    msg->lang_id = (uint64_t)cJSON_GetNumberValue(lang_id_item);
    cJSON *zone_item = cJSON_GetObjectItem(obj, "zone");
    if (zone_item == NULL)
        return false;
    msg->zone = (uint64_t)cJSON_GetNumberValue(zone_item);
    cJSON *is_12h_item = cJSON_GetObjectItem(obj, "is_12h");
    if (is_12h_item == NULL)
        return false;
    msg->is_12h = (uint64_t)cJSON_GetNumberValue(is_12h_item);
    cJSON *is_mi_item = cJSON_GetObjectItem(obj, "is_mi");
    if (is_mi_item == NULL)
        return false;
    msg->is_mi = (uint64_t)cJSON_GetNumberValue(is_mi_item);
    cJSON *brt_item = cJSON_GetObjectItem(obj, "brt");
    if (brt_item == NULL)
        return false;
    msg->brt = (uint64_t)cJSON_GetNumberValue(brt_item);
    return true;
}

/*@brief 打包转换 ElecCard 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_elec_card(const AppPB_ElecCard *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "is_activate", msg->is_activate);
    cJSON_AddNumberToObject(obj, "is_reported", msg->is_reported);
    return obj;
}

/*@brief 解析转换 JSON 对象为 ElecCard 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_elec_card(cJSON *obj, AppPB_ElecCard *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *is_activate_item = cJSON_GetObjectItem(obj, "is_activate");
    if (is_activate_item == NULL)
        return false;
    msg->is_activate = (uint64_t)cJSON_GetNumberValue(is_activate_item);
    cJSON *is_reported_item = cJSON_GetObjectItem(obj, "is_reported");
    if (is_reported_item == NULL)
        return false;
    msg->is_reported = (uint64_t)cJSON_GetNumberValue(is_reported_item);
    return true;
}

/*@brief 打包转换 FemCycle 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_fem_cycle(const AppPB_FemCycle *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "remind_sw", msg->remind_sw);
    cJSON_AddNumberToObject(obj, "menstr_remind", msg->menstr_remind);
    cJSON_AddNumberToObject(obj, "menstr_end_remind", msg->menstr_end_remind);
    cJSON_AddNumberToObject(obj, "ovulat_remind", msg->ovulat_remind);
    cJSON_AddNumberToObject(obj, "ovulat_end_remind", msg->ovulat_end_remind);
    cJSON_AddNumberToObject(obj, "cycle_start", msg->cycle_start);
    cJSON_AddNumberToObject(obj, "cycle_end", msg->cycle_end);
    cJSON_AddNumberToObject(obj, "keep_days", msg->keep_days);
    cJSON_AddNumberToObject(obj, "cycle_days", msg->cycle_days);
    return obj;
}

/*@brief 解析转换 JSON 对象为 FemCycle 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_fem_cycle(cJSON *obj, AppPB_FemCycle *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *remind_sw_item = cJSON_GetObjectItem(obj, "remind_sw");
    if (remind_sw_item == NULL)
        return false;
    msg->remind_sw = (uint64_t)cJSON_GetNumberValue(remind_sw_item);
    cJSON *menstr_remind_item = cJSON_GetObjectItem(obj, "menstr_remind");
    if (menstr_remind_item == NULL)
        return false;
    msg->menstr_remind = (uint64_t)cJSON_GetNumberValue(menstr_remind_item);
    cJSON *menstr_end_remind_item = cJSON_GetObjectItem(obj, "menstr_end_remind");
    if (menstr_end_remind_item == NULL)
        return false;
    msg->menstr_end_remind = (uint64_t)cJSON_GetNumberValue(menstr_end_remind_item);
    cJSON *ovulat_remind_item = cJSON_GetObjectItem(obj, "ovulat_remind");
    if (ovulat_remind_item == NULL)
        return false;
    msg->ovulat_remind = (uint64_t)cJSON_GetNumberValue(ovulat_remind_item);
    cJSON *ovulat_end_remind_item = cJSON_GetObjectItem(obj, "ovulat_end_remind");
    if (ovulat_end_remind_item == NULL)
        return false;
    msg->ovulat_end_remind = (uint64_t)cJSON_GetNumberValue(ovulat_end_remind_item);
    cJSON *cycle_start_item = cJSON_GetObjectItem(obj, "cycle_start");
    if (cycle_start_item == NULL)
        return false;
    msg->cycle_start = (uint64_t)cJSON_GetNumberValue(cycle_start_item);
    cJSON *cycle_end_item = cJSON_GetObjectItem(obj, "cycle_end");
    if (cycle_end_item == NULL)
        return false;
    msg->cycle_end = (uint64_t)cJSON_GetNumberValue(cycle_end_item);
    cJSON *keep_days_item = cJSON_GetObjectItem(obj, "keep_days");
    if (keep_days_item == NULL)
        return false;
    msg->keep_days = (uint64_t)cJSON_GetNumberValue(keep_days_item);
    cJSON *cycle_days_item = cJSON_GetObjectItem(obj, "cycle_days");
    if (cycle_days_item == NULL)
        return false;
    msg->cycle_days = (uint64_t)cJSON_GetNumberValue(cycle_days_item);
    return true;
}

/*@brief 打包转换 FileDes 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_file_des(const AppPB_FileDes *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddStringToObject(obj, "name", msg->name);
    cJSON_AddNumberToObject(obj, "utc64", msg->utc64);
    cJSON_AddNumberToObject(obj, "crc32", msg->crc32);
    cJSON_AddNumberToObject(obj, "size", msg->size);
    cJSON_AddNumberToObject(obj, "crc8", msg->crc8);
    return obj;
}

/*@brief 解析转换 JSON 对象为 FileDes 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_file_des(cJSON *obj, AppPB_FileDes *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *name_item = cJSON_GetObjectItem(obj, "name");
    if (name_item == NULL)
        return false;
    snprintf(msg->name, sizeof(msg->name), "%s", cJSON_GetStringValue(name_item));
    cJSON *utc64_item = cJSON_GetObjectItem(obj, "utc64");
    if (utc64_item == NULL)
        return false;
    msg->utc64 = (uint64_t)cJSON_GetNumberValue(utc64_item);
    cJSON *crc32_item = cJSON_GetObjectItem(obj, "crc32");
    if (crc32_item == NULL)
        return false;
    msg->crc32 = (uint64_t)cJSON_GetNumberValue(crc32_item);
    cJSON *size_item = cJSON_GetObjectItem(obj, "size");
    if (size_item == NULL)
        return false;
    msg->size = (uint64_t)cJSON_GetNumberValue(size_item);
    cJSON *crc8_item = cJSON_GetObjectItem(obj, "crc8");
    if (crc8_item == NULL)
        return false;
    msg->crc8 = (uint64_t)cJSON_GetNumberValue(crc8_item);
    return true;
}

/*@brief 打包转换 FileEnd 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_file_end(const AppPB_FileEnd *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "code", msg->code);
    return obj;
}

/*@brief 解析转换 JSON 对象为 FileEnd 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_file_end(cJSON *obj, AppPB_FileEnd *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *code_item = cJSON_GetObjectItem(obj, "code");
    if (code_item == NULL)
        return false;
    msg->code = (uint64_t)cJSON_GetNumberValue(code_item);
    return true;
}

/*@brief 打包转换 HeartRate 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_heart_rate(const AppPB_HeartRate *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "is_auto", msg->is_auto);
    cJSON_AddNumberToObject(obj, "lwarn_on", msg->lwarn_on);
    cJSON_AddNumberToObject(obj, "hwarn_on", msg->hwarn_on);
    cJSON_AddNumberToObject(obj, "lwarn", msg->lwarn);
    cJSON_AddNumberToObject(obj, "hwarn", msg->hwarn);
    cJSON_AddNumberToObject(obj, "test_min", msg->test_min);
    cJSON_AddNumberToObject(obj, "static_type", msg->static_type);
    return obj;
}

/*@brief 解析转换 JSON 对象为 HeartRate 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_heart_rate(cJSON *obj, AppPB_HeartRate *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *is_auto_item = cJSON_GetObjectItem(obj, "is_auto");
    if (is_auto_item == NULL)
        return false;
    msg->is_auto = (uint64_t)cJSON_GetNumberValue(is_auto_item);
    cJSON *lwarn_on_item = cJSON_GetObjectItem(obj, "lwarn_on");
    if (lwarn_on_item == NULL)
        return false;
    msg->lwarn_on = (uint64_t)cJSON_GetNumberValue(lwarn_on_item);
    cJSON *hwarn_on_item = cJSON_GetObjectItem(obj, "hwarn_on");
    if (hwarn_on_item == NULL)
        return false;
    msg->hwarn_on = (uint64_t)cJSON_GetNumberValue(hwarn_on_item);
    cJSON *lwarn_item = cJSON_GetObjectItem(obj, "lwarn");
    if (lwarn_item == NULL)
        return false;
    msg->lwarn = (uint64_t)cJSON_GetNumberValue(lwarn_item);
    cJSON *hwarn_item = cJSON_GetObjectItem(obj, "hwarn");
    if (hwarn_item == NULL)
        return false;
    msg->hwarn = (uint64_t)cJSON_GetNumberValue(hwarn_item);
    cJSON *test_min_item = cJSON_GetObjectItem(obj, "test_min");
    if (test_min_item == NULL)
        return false;
    msg->test_min = (uint64_t)cJSON_GetNumberValue(test_min_item);
    cJSON *static_type_item = cJSON_GetObjectItem(obj, "static_type");
    if (static_type_item == NULL)
        return false;
    msg->static_type = (uint64_t)cJSON_GetNumberValue(static_type_item);
    return true;
}

/*@brief 打包转换 MotionSum 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_motion_sum(const AppPB_MotionSum *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "hr_value", msg->hr_value);
    cJSON_AddNumberToObject(obj, "hr_ts", msg->hr_ts);
    cJSON_AddNumberToObject(obj, "kcal", msg->kcal);
    cJSON_AddNumberToObject(obj, "distance", msg->distance);
    cJSON_AddNumberToObject(obj, "elevation", msg->elevation);
    cJSON_AddNumberToObject(obj, "sleep_time", msg->sleep_time);
    cJSON_AddNumberToObject(obj, "step", msg->step);
    return obj;
}

/*@brief 解析转换 JSON 对象为 MotionSum 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_motion_sum(cJSON *obj, AppPB_MotionSum *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *hr_value_item = cJSON_GetObjectItem(obj, "hr_value");
    if (hr_value_item == NULL)
        return false;
    msg->hr_value = (uint64_t)cJSON_GetNumberValue(hr_value_item);
    cJSON *hr_ts_item = cJSON_GetObjectItem(obj, "hr_ts");
    if (hr_ts_item == NULL)
        return false;
    msg->hr_ts = (uint64_t)cJSON_GetNumberValue(hr_ts_item);
    cJSON *kcal_item = cJSON_GetObjectItem(obj, "kcal");
    if (kcal_item == NULL)
        return false;
    msg->kcal = (uint64_t)cJSON_GetNumberValue(kcal_item);
    cJSON *distance_item = cJSON_GetObjectItem(obj, "distance");
    if (distance_item == NULL)
        return false;
    msg->distance = (uint64_t)cJSON_GetNumberValue(distance_item);
    cJSON *elevation_item = cJSON_GetObjectItem(obj, "elevation");
    if (elevation_item == NULL)
        return false;
    msg->elevation = (uint64_t)cJSON_GetNumberValue(elevation_item);
    cJSON *sleep_time_item = cJSON_GetObjectItem(obj, "sleep_time");
    if (sleep_time_item == NULL)
        return false;
    msg->sleep_time = (uint64_t)cJSON_GetNumberValue(sleep_time_item);
    cJSON *step_item = cJSON_GetObjectItem(obj, "step");
    if (step_item == NULL)
        return false;
    msg->step = (uint64_t)cJSON_GetNumberValue(step_item);
    return true;
}

/*@brief 打包转换 MsgInfo 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_msg_info(const AppPB_MsgInfo *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddStringToObject(obj, "app_name", msg->app_name);
    cJSON_AddStringToObject(obj, "contact", msg->contact);
    cJSON_AddStringToObject(obj, "content", msg->content);
    cJSON_AddNumberToObject(obj, "msg_id", msg->msg_id);
    cJSON_AddStringToObject(obj, "phone", msg->phone);
    cJSON_AddNumberToObject(obj, "msg_type", msg->msg_type);
    cJSON_AddNumberToObject(obj, "vibrate", msg->vibrate);
    return obj;
}

/*@brief 解析转换 JSON 对象为 MsgInfo 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_msg_info(cJSON *obj, AppPB_MsgInfo *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *app_name_item = cJSON_GetObjectItem(obj, "app_name");
    if (app_name_item == NULL)
        return false;
    snprintf(msg->app_name, sizeof(msg->app_name), "%s", cJSON_GetStringValue(app_name_item));
    cJSON *contact_item = cJSON_GetObjectItem(obj, "contact");
    if (contact_item == NULL)
        return false;
    snprintf(msg->contact, sizeof(msg->contact), "%s", cJSON_GetStringValue(contact_item));
    cJSON *content_item = cJSON_GetObjectItem(obj, "content");
    if (content_item == NULL)
        return false;
    snprintf(msg->content, sizeof(msg->content), "%s", cJSON_GetStringValue(content_item));
    cJSON *msg_id_item = cJSON_GetObjectItem(obj, "msg_id");
    if (msg_id_item == NULL)
        return false;
    msg->msg_id = (uint64_t)cJSON_GetNumberValue(msg_id_item);
    cJSON *phone_item = cJSON_GetObjectItem(obj, "phone");
    if (phone_item == NULL)
        return false;
    snprintf(msg->phone, sizeof(msg->phone), "%s", cJSON_GetStringValue(phone_item));
    cJSON *msg_type_item = cJSON_GetObjectItem(obj, "msg_type");
    if (msg_type_item == NULL)
        return false;
    msg->msg_type = (uint64_t)cJSON_GetNumberValue(msg_type_item);
    cJSON *vibrate_item = cJSON_GetObjectItem(obj, "vibrate");
    if (vibrate_item == NULL)
        return false;
    msg->vibrate = (uint64_t)cJSON_GetNumberValue(vibrate_item);
    return true;
}

/*@brief 打包转换 Music 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_music(const AppPB_Music *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddStringToObject(obj, "singer", msg->singer);
    cJSON_AddStringToObject(obj, "song_name", msg->song_name);
    cJSON_AddNumberToObject(obj, "play_st", msg->play_st);
    cJSON_AddNumberToObject(obj, "max_vol", msg->max_vol);
    cJSON_AddNumberToObject(obj, "cur_vol", msg->cur_vol);
    cJSON_AddNumberToObject(obj, "app_st", msg->app_st);
    return obj;
}

/*@brief 解析转换 JSON 对象为 Music 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_music(cJSON *obj, AppPB_Music *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *singer_item = cJSON_GetObjectItem(obj, "singer");
    if (singer_item == NULL)
        return false;
    snprintf(msg->singer, sizeof(msg->singer), "%s", cJSON_GetStringValue(singer_item));
    cJSON *song_name_item = cJSON_GetObjectItem(obj, "song_name");
    if (song_name_item == NULL)
        return false;
    snprintf(msg->song_name, sizeof(msg->song_name), "%s", cJSON_GetStringValue(song_name_item));
    cJSON *play_st_item = cJSON_GetObjectItem(obj, "play_st");
    if (play_st_item == NULL)
        return false;
    msg->play_st = (uint64_t)cJSON_GetNumberValue(play_st_item);
    cJSON *max_vol_item = cJSON_GetObjectItem(obj, "max_vol");
    if (max_vol_item == NULL)
        return false;
    msg->max_vol = (uint64_t)cJSON_GetNumberValue(max_vol_item);
    cJSON *cur_vol_item = cJSON_GetObjectItem(obj, "cur_vol");
    if (cur_vol_item == NULL)
        return false;
    msg->cur_vol = (uint64_t)cJSON_GetNumberValue(cur_vol_item);
    cJSON *app_st_item = cJSON_GetObjectItem(obj, "app_st");
    if (app_st_item == NULL)
        return false;
    msg->app_st = (uint64_t)cJSON_GetNumberValue(app_st_item);
    return true;
}

/*@brief 打包转换 NotDisturb 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_not_disturb(const AppPB_NotDisturb *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "on", msg->on);
    cJSON_AddNumberToObject(obj, "shour", msg->shour);
    cJSON_AddNumberToObject(obj, "smin", msg->smin);
    cJSON_AddNumberToObject(obj, "ehour", msg->ehour);
    cJSON_AddNumberToObject(obj, "emin", msg->emin);
    cJSON_AddNumberToObject(obj, "repeat", msg->repeat);
    return obj;
}

/*@brief 解析转换 JSON 对象为 NotDisturb 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_not_disturb(cJSON *obj, AppPB_NotDisturb *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *on_item = cJSON_GetObjectItem(obj, "on");
    if (on_item == NULL)
        return false;
    msg->on = (uint64_t)cJSON_GetNumberValue(on_item);
    cJSON *shour_item = cJSON_GetObjectItem(obj, "shour");
    if (shour_item == NULL)
        return false;
    msg->shour = (uint64_t)cJSON_GetNumberValue(shour_item);
    cJSON *smin_item = cJSON_GetObjectItem(obj, "smin");
    if (smin_item == NULL)
        return false;
    msg->smin = (uint64_t)cJSON_GetNumberValue(smin_item);
    cJSON *ehour_item = cJSON_GetObjectItem(obj, "ehour");
    if (ehour_item == NULL)
        return false;
    msg->ehour = (uint64_t)cJSON_GetNumberValue(ehour_item);
    cJSON *emin_item = cJSON_GetObjectItem(obj, "emin");
    if (emin_item == NULL)
        return false;
    msg->emin = (uint64_t)cJSON_GetNumberValue(emin_item);
    cJSON *repeat_item = cJSON_GetObjectItem(obj, "repeat");
    if (repeat_item == NULL)
        return false;
    msg->repeat = (uint64_t)cJSON_GetNumberValue(repeat_item);
    return true;
}

/*@brief 打包转换 Ota 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_ota(const AppPB_Ota *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "cmd", msg->cmd);
    cJSON_AddNumberToObject(obj, "state", msg->state);
    cJSON_AddNumberToObject(obj, "ready_cond", msg->ready_cond);
    return obj;
}

/*@brief 解析转换 JSON 对象为 Ota 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_ota(cJSON *obj, AppPB_Ota *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *cmd_item = cJSON_GetObjectItem(obj, "cmd");
    if (cmd_item == NULL)
        return false;
    msg->cmd = (uint64_t)cJSON_GetNumberValue(cmd_item);
    cJSON *state_item = cJSON_GetObjectItem(obj, "state");
    if (state_item == NULL)
        return false;
    msg->state = (uint64_t)cJSON_GetNumberValue(state_item);
    cJSON *ready_cond_item = cJSON_GetObjectItem(obj, "ready_cond");
    if (ready_cond_item == NULL)
        return false;
    msg->ready_cond = (uint64_t)cJSON_GetNumberValue(ready_cond_item);
    return true;
}

/*@brief 打包转换 Position 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_position(const AppPB_Position *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "speed", msg->speed);
    cJSON_AddNumberToObject(obj, "distance", msg->distance);
    cJSON_AddNumberToObject(obj, "altitude", msg->altitude);
    cJSON_AddNumberToObject(obj, "total_dist", msg->total_dist);
    cJSON_AddNumberToObject(obj, "start_time", msg->start_time);
    cJSON_AddNumberToObject(obj, "end_time", msg->end_time);
    cJSON_AddNumberToObject(obj, "latitude", msg->latitude);
    cJSON_AddNumberToObject(obj, "longitude", msg->longitude);
    cJSON_AddNumberToObject(obj, "bearing", msg->bearing);
    cJSON_AddNumberToObject(obj, "accuracy", msg->accuracy);
    return obj;
}

/*@brief 解析转换 JSON 对象为 Position 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_position(cJSON *obj, AppPB_Position *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *speed_item = cJSON_GetObjectItem(obj, "speed");
    if (speed_item == NULL)
        return false;
    msg->speed = (uint64_t)cJSON_GetNumberValue(speed_item);
    cJSON *distance_item = cJSON_GetObjectItem(obj, "distance");
    if (distance_item == NULL)
        return false;
    msg->distance = (uint64_t)cJSON_GetNumberValue(distance_item);
    cJSON *altitude_item = cJSON_GetObjectItem(obj, "altitude");
    if (altitude_item == NULL)
        return false;
    msg->altitude = (uint64_t)cJSON_GetNumberValue(altitude_item);
    cJSON *total_dist_item = cJSON_GetObjectItem(obj, "total_dist");
    if (total_dist_item == NULL)
        return false;
    msg->total_dist = (uint64_t)cJSON_GetNumberValue(total_dist_item);
    cJSON *start_time_item = cJSON_GetObjectItem(obj, "start_time");
    if (start_time_item == NULL)
        return false;
    msg->start_time = (uint64_t)cJSON_GetNumberValue(start_time_item);
    cJSON *end_time_item = cJSON_GetObjectItem(obj, "end_time");
    if (end_time_item == NULL)
        return false;
    msg->end_time = (uint64_t)cJSON_GetNumberValue(end_time_item);
    cJSON *latitude_item = cJSON_GetObjectItem(obj, "latitude");
    if (latitude_item == NULL)
        return false;
    msg->latitude = (uint64_t)cJSON_GetNumberValue(latitude_item);
    cJSON *longitude_item = cJSON_GetObjectItem(obj, "longitude");
    if (longitude_item == NULL)
        return false;
    msg->longitude = (uint64_t)cJSON_GetNumberValue(longitude_item);
    cJSON *bearing_item = cJSON_GetObjectItem(obj, "bearing");
    if (bearing_item == NULL)
        return false;
    msg->bearing = (uint64_t)cJSON_GetNumberValue(bearing_item);
    cJSON *accuracy_item = cJSON_GetObjectItem(obj, "accuracy");
    if (accuracy_item == NULL)
        return false;
    msg->accuracy = (uint64_t)cJSON_GetNumberValue(accuracy_item);
    return true;
}

/*@brief 打包转换 SportMng 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sport_mng(const AppPB_SportMng *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "max_add_num", msg->max_add_num);
    cJSON_AddNumberToObject(obj, "min_add_num", msg->min_add_num);
    cJSON *sport_type_arr = cJSON_AddArrayToObject(obj, "sport_type");
    for (pb_size_t sport_type_i = 0; sport_type_i < msg->sport_type_count; sport_type_i++)
        cJSON_AddItemToArray(sport_type_arr, cJSON_CreateNumber(msg->sport_type[sport_type_i]));
    return obj;
}

/*@brief 解析转换 JSON 对象为 SportMng 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sport_mng(cJSON *obj, AppPB_SportMng *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *max_add_num_item = cJSON_GetObjectItem(obj, "max_add_num");
    if (max_add_num_item == NULL)
        return false;
    msg->max_add_num = (uint64_t)cJSON_GetNumberValue(max_add_num_item);
    cJSON *min_add_num_item = cJSON_GetObjectItem(obj, "min_add_num");
    if (min_add_num_item == NULL)
        return false;
    msg->min_add_num = (uint64_t)cJSON_GetNumberValue(min_add_num_item);
    cJSON *sport_type_arr = cJSON_GetObjectItem(obj, "sport_type");
    if (sport_type_arr == NULL)
        return false;
    msg->sport_type_count = cJSON_GetArraySize(sport_type_arr);
    for (int sport_type_i = 0; sport_type_i < msg->sport_type_count; sport_type_i++)
        msg->sport_type[sport_type_i] = (uint64_t)cJSON_GetNumberValue(cJSON_GetArrayItem(sport_type_arr, sport_type_i));
    return true;
}

/*@brief 打包转换 SportRcd 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sport_rcd(const AppPB_SportRcd *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "id", msg->id);
    cJSON_AddNumberToObject(obj, "status", msg->status);
    cJSON_AddNumberToObject(obj, "start_time", msg->start_time);
    cJSON_AddNumberToObject(obj, "end_time", msg->end_time);
    cJSON_AddNumberToObject(obj, "calorie", msg->calorie);
    cJSON_AddNumberToObject(obj, "distance", msg->distance);
    cJSON_AddNumberToObject(obj, "step", msg->step);
    cJSON_AddNumberToObject(obj, "duration", msg->duration);
    cJSON_AddNumberToObject(obj, "speed", msg->speed);
    cJSON_AddNumberToObject(obj, "type", msg->type);
    return obj;
}

/*@brief 解析转换 JSON 对象为 SportRcd 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sport_rcd(cJSON *obj, AppPB_SportRcd *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *id_item = cJSON_GetObjectItem(obj, "id");
    if (id_item == NULL)
        return false;
    msg->id = (uint64_t)cJSON_GetNumberValue(id_item);
    cJSON *status_item = cJSON_GetObjectItem(obj, "status");
    if (status_item == NULL)
        return false;
    msg->status = (uint64_t)cJSON_GetNumberValue(status_item);
    cJSON *start_time_item = cJSON_GetObjectItem(obj, "start_time");
    if (start_time_item == NULL)
        return false;
    msg->start_time = (uint64_t)cJSON_GetNumberValue(start_time_item);
    cJSON *end_time_item = cJSON_GetObjectItem(obj, "end_time");
    if (end_time_item == NULL)
        return false;
    msg->end_time = (uint64_t)cJSON_GetNumberValue(end_time_item);
    cJSON *calorie_item = cJSON_GetObjectItem(obj, "calorie");
    if (calorie_item == NULL)
        return false;
    msg->calorie = (uint64_t)cJSON_GetNumberValue(calorie_item);
    cJSON *distance_item = cJSON_GetObjectItem(obj, "distance");
    if (distance_item == NULL)
        return false;
    msg->distance = (uint64_t)cJSON_GetNumberValue(distance_item);
    cJSON *step_item = cJSON_GetObjectItem(obj, "step");
    if (step_item == NULL)
        return false;
    msg->step = (uint64_t)cJSON_GetNumberValue(step_item);
    cJSON *duration_item = cJSON_GetObjectItem(obj, "duration");
    if (duration_item == NULL)
        return false;
    msg->duration = (uint64_t)cJSON_GetNumberValue(duration_item);
    cJSON *speed_item = cJSON_GetObjectItem(obj, "speed");
    if (speed_item == NULL)
        return false;
    msg->speed = (uint64_t)cJSON_GetNumberValue(speed_item);
    cJSON *type_item = cJSON_GetObjectItem(obj, "type");
    if (type_item == NULL)
        return false;
    msg->type = (uint64_t)cJSON_GetNumberValue(type_item);
    return true;
}

/*@brief 打包转换 SportState 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sport_state(const AppPB_SportState *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "monitor_st", msg->monitor_st);
    cJSON_AddNumberToObject(obj, "operator_t", msg->operator_t);
    cJSON_AddNumberToObject(obj, "sport_type", msg->sport_type);
    cJSON_AddNumberToObject(obj, "start_time", msg->start_time);
    cJSON_AddNumberToObject(obj, "workout_type", msg->workout_type);
    cJSON_AddNumberToObject(obj, "op_time", msg->op_time);
    return obj;
}

/*@brief 解析转换 JSON 对象为 SportState 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sport_state(cJSON *obj, AppPB_SportState *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *monitor_st_item = cJSON_GetObjectItem(obj, "monitor_st");
    if (monitor_st_item == NULL)
        return false;
    msg->monitor_st = (uint64_t)cJSON_GetNumberValue(monitor_st_item);
    cJSON *operator_t_item = cJSON_GetObjectItem(obj, "operator_t");
    if (operator_t_item == NULL)
        return false;
    msg->operator_t = (uint64_t)cJSON_GetNumberValue(operator_t_item);
    cJSON *sport_type_item = cJSON_GetObjectItem(obj, "sport_type");
    if (sport_type_item == NULL)
        return false;
    msg->sport_type = (uint64_t)cJSON_GetNumberValue(sport_type_item);
    cJSON *start_time_item = cJSON_GetObjectItem(obj, "start_time");
    if (start_time_item == NULL)
        return false;
    msg->start_time = (uint64_t)cJSON_GetNumberValue(start_time_item);
    cJSON *workout_type_item = cJSON_GetObjectItem(obj, "workout_type");
    if (workout_type_item == NULL)
        return false;
    msg->workout_type = (uint64_t)cJSON_GetNumberValue(workout_type_item);
    cJSON *op_time_item = cJSON_GetObjectItem(obj, "op_time");
    if (op_time_item == NULL)
        return false;
    msg->op_time = (uint64_t)cJSON_GetNumberValue(op_time_item);
    return true;
}

/*@brief 打包转换 SportTgt 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sport_tgt(const AppPB_SportTgt *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "motion_t", msg->motion_t);
    cJSON_AddNumberToObject(obj, "goal_type", msg->goal_type);
    cJSON_AddNumberToObject(obj, "step", msg->step);
    cJSON_AddNumberToObject(obj, "kcal", msg->kcal);
    cJSON_AddNumberToObject(obj, "meter", msg->meter);
    cJSON_AddNumberToObject(obj, "duration", msg->duration);
    return obj;
}

/*@brief 解析转换 JSON 对象为 SportTgt 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sport_tgt(cJSON *obj, AppPB_SportTgt *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *motion_t_item = cJSON_GetObjectItem(obj, "motion_t");
    if (motion_t_item == NULL)
        return false;
    msg->motion_t = (uint64_t)cJSON_GetNumberValue(motion_t_item);
    cJSON *goal_type_item = cJSON_GetObjectItem(obj, "goal_type");
    if (goal_type_item == NULL)
        return false;
    msg->goal_type = (uint64_t)cJSON_GetNumberValue(goal_type_item);
    cJSON *step_item = cJSON_GetObjectItem(obj, "step");
    if (step_item == NULL)
        return false;
    msg->step = (uint64_t)cJSON_GetNumberValue(step_item);
    cJSON *kcal_item = cJSON_GetObjectItem(obj, "kcal");
    if (kcal_item == NULL)
        return false;
    msg->kcal = (uint64_t)cJSON_GetNumberValue(kcal_item);
    cJSON *meter_item = cJSON_GetObjectItem(obj, "meter");
    if (meter_item == NULL)
        return false;
    msg->meter = (uint64_t)cJSON_GetNumberValue(meter_item);
    cJSON *duration_item = cJSON_GetObjectItem(obj, "duration");
    if (duration_item == NULL)
        return false;
    msg->duration = (uint64_t)cJSON_GetNumberValue(duration_item);
    return true;
}

/*@brief 打包转换 SysClock 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_sys_clock(const AppPB_SysClock *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "year", msg->year);
    cJSON_AddNumberToObject(obj, "month", msg->month);
    cJSON_AddNumberToObject(obj, "day", msg->day);
    cJSON_AddNumberToObject(obj, "hour", msg->hour);
    cJSON_AddNumberToObject(obj, "minute", msg->minute);
    cJSON_AddNumberToObject(obj, "second", msg->second);
    cJSON_AddNumberToObject(obj, "zone", msg->zone);
    cJSON_AddNumberToObject(obj, "mode", msg->mode);
    return obj;
}

/*@brief 解析转换 JSON 对象为 SysClock 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_sys_clock(cJSON *obj, AppPB_SysClock *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *year_item = cJSON_GetObjectItem(obj, "year");
    if (year_item == NULL)
        return false;
    msg->year = (uint64_t)cJSON_GetNumberValue(year_item);
    cJSON *month_item = cJSON_GetObjectItem(obj, "month");
    if (month_item == NULL)
        return false;
    msg->month = (uint64_t)cJSON_GetNumberValue(month_item);
    cJSON *day_item = cJSON_GetObjectItem(obj, "day");
    if (day_item == NULL)
        return false;
    msg->day = (uint64_t)cJSON_GetNumberValue(day_item);
    cJSON *hour_item = cJSON_GetObjectItem(obj, "hour");
    if (hour_item == NULL)
        return false;
    msg->hour = (uint64_t)cJSON_GetNumberValue(hour_item);
    cJSON *minute_item = cJSON_GetObjectItem(obj, "minute");
    if (minute_item == NULL)
        return false;
    msg->minute = (uint64_t)cJSON_GetNumberValue(minute_item);
    cJSON *second_item = cJSON_GetObjectItem(obj, "second");
    if (second_item == NULL)
        return false;
    msg->second = (uint64_t)cJSON_GetNumberValue(second_item);
    cJSON *zone_item = cJSON_GetObjectItem(obj, "zone");
    if (zone_item == NULL)
        return false;
    msg->zone = (uint64_t)cJSON_GetNumberValue(zone_item);
    cJSON *mode_item = cJSON_GetObjectItem(obj, "mode");
    if (mode_item == NULL)
        return false;
    msg->mode = (uint64_t)cJSON_GetNumberValue(mode_item);
    return true;
}

/*@brief 打包转换 TraceTxt 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_trace_txt(const AppPB_TraceTxt *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddStringToObject(obj, "trace_text", msg->trace_text);
    return obj;
}

/*@brief 解析转换 JSON 对象为 TraceTxt 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_trace_txt(cJSON *obj, AppPB_TraceTxt *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *trace_text_item = cJSON_GetObjectItem(obj, "trace_text");
    if (trace_text_item == NULL)
        return false;
    snprintf(msg->trace_text, sizeof(msg->trace_text), "%s", cJSON_GetStringValue(trace_text_item));
    return true;
}

/*@brief 打包转换 UserPhys 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_user_phys(const AppPB_UserPhys *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "age", msg->age);
    cJSON_AddNumberToObject(obj, "birthday", msg->birthday);
    cJSON_AddNumberToObject(obj, "gender", msg->gender);
    cJSON_AddNumberToObject(obj, "height", msg->height);
    cJSON_AddNumberToObject(obj, "weight", msg->weight);
    cJSON_AddNumberToObject(obj, "vo2max", msg->vo2max);
    cJSON_AddNumberToObject(obj, "run_step", msg->run_step);
    cJSON_AddNumberToObject(obj, "walk_step", msg->walk_step);
    return obj;
}

/*@brief 解析转换 JSON 对象为 UserPhys 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_user_phys(cJSON *obj, AppPB_UserPhys *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *age_item = cJSON_GetObjectItem(obj, "age");
    if (age_item == NULL)
        return false;
    msg->age = (uint64_t)cJSON_GetNumberValue(age_item);
    cJSON *birthday_item = cJSON_GetObjectItem(obj, "birthday");
    if (birthday_item == NULL)
        return false;
    msg->birthday = (uint64_t)cJSON_GetNumberValue(birthday_item);
    cJSON *gender_item = cJSON_GetObjectItem(obj, "gender");
    if (gender_item == NULL)
        return false;
    msg->gender = (uint64_t)cJSON_GetNumberValue(gender_item);
    cJSON *height_item = cJSON_GetObjectItem(obj, "height");
    if (height_item == NULL)
        return false;
    msg->height = (uint64_t)cJSON_GetNumberValue(height_item);
    cJSON *weight_item = cJSON_GetObjectItem(obj, "weight");
    if (weight_item == NULL)
        return false;
    msg->weight = (uint64_t)cJSON_GetNumberValue(weight_item);
    cJSON *vo2max_item = cJSON_GetObjectItem(obj, "vo2max");
    if (vo2max_item == NULL)
        return false;
    msg->vo2max = (uint64_t)cJSON_GetNumberValue(vo2max_item);
    cJSON *run_step_item = cJSON_GetObjectItem(obj, "run_step");
    if (run_step_item == NULL)
        return false;
    msg->run_step = (uint64_t)cJSON_GetNumberValue(run_step_item);
    cJSON *walk_step_item = cJSON_GetObjectItem(obj, "walk_step");
    if (walk_step_item == NULL)
        return false;
    msg->walk_step = (uint64_t)cJSON_GetNumberValue(walk_step_item);
    return true;
}

/*@brief 打包转换 Weather 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_weather(const AppPB_Weather *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "temp_cur", msg->temp_cur);
    cJSON_AddNumberToObject(obj, "temp_max", msg->temp_max);
    cJSON_AddNumberToObject(obj, "temp_min", msg->temp_min);
    cJSON_AddNumberToObject(obj, "phen", msg->phen);
    cJSON_AddNumberToObject(obj, "humi", msg->humi);
    cJSON_AddNumberToObject(obj, "uv", msg->uv);
    cJSON_AddNumberToObject(obj, "pm25", msg->pm25);
    cJSON_AddNumberToObject(obj, "aqi", msg->aqi);
    cJSON_AddStringToObject(obj, "city", msg->city);
    cJSON *day_arr = cJSON_AddArrayToObject(obj, "day");
    for (pb_size_t day_i = 0; day_i < msg->day_count; day_i++)
        cJSON_AddItemToArray(day_arr, app_json_xfer_pack_weather_day(&msg->day[day_i]));
    return obj;
}

/*@brief 解析转换 JSON 对象为 Weather 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_weather(cJSON *obj, AppPB_Weather *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *temp_cur_item = cJSON_GetObjectItem(obj, "temp_cur");
    if (temp_cur_item == NULL)
        return false;
    msg->temp_cur = (uint64_t)cJSON_GetNumberValue(temp_cur_item);
    cJSON *temp_max_item = cJSON_GetObjectItem(obj, "temp_max");
    if (temp_max_item == NULL)
        return false;
    msg->temp_max = (uint64_t)cJSON_GetNumberValue(temp_max_item);
    cJSON *temp_min_item = cJSON_GetObjectItem(obj, "temp_min");
    if (temp_min_item == NULL)
        return false;
    msg->temp_min = (uint64_t)cJSON_GetNumberValue(temp_min_item);
    cJSON *phen_item = cJSON_GetObjectItem(obj, "phen");
    if (phen_item == NULL)
        return false;
    msg->phen = (uint64_t)cJSON_GetNumberValue(phen_item);
    cJSON *humi_item = cJSON_GetObjectItem(obj, "humi");
    if (humi_item == NULL)
        return false;
    msg->humi = (uint64_t)cJSON_GetNumberValue(humi_item);
    cJSON *uv_item = cJSON_GetObjectItem(obj, "uv");
    if (uv_item == NULL)
        return false;
    msg->uv = (uint64_t)cJSON_GetNumberValue(uv_item);
    cJSON *pm25_item = cJSON_GetObjectItem(obj, "pm25");
    if (pm25_item == NULL)
        return false;
    msg->pm25 = (uint64_t)cJSON_GetNumberValue(pm25_item);
    cJSON *aqi_item = cJSON_GetObjectItem(obj, "aqi");
    if (aqi_item == NULL)
        return false;
    msg->aqi = (uint64_t)cJSON_GetNumberValue(aqi_item);
    cJSON *city_item = cJSON_GetObjectItem(obj, "city");
    if (city_item == NULL)
        return false;
    snprintf(msg->city, sizeof(msg->city), "%s", cJSON_GetStringValue(city_item));
    cJSON *day_arr = cJSON_GetObjectItem(obj, "day");
    if (day_arr == NULL)
        return false;
    msg->day_count = cJSON_GetArraySize(day_arr);
    for (int day_i = 0; day_i < msg->day_count; day_i++)
        app_json_xfer_unpack_weather_day(cJSON_GetArrayItem(day_arr, day_i), &msg->day[day_i]);
    return true;
}

/*@brief 打包转换 WeatherDay 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_weather_day(const AppPB_WeatherDay *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "temp_max", msg->temp_max);
    cJSON_AddNumberToObject(obj, "temp_min", msg->temp_min);
    cJSON_AddNumberToObject(obj, "phen", msg->phen);
    cJSON_AddNumberToObject(obj, "humi", msg->humi);
    return obj;
}

/*@brief 解析转换 JSON 对象为 WeatherDay 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_weather_day(cJSON *obj, AppPB_WeatherDay *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *temp_max_item = cJSON_GetObjectItem(obj, "temp_max");
    if (temp_max_item == NULL)
        return false;
    msg->temp_max = (uint64_t)cJSON_GetNumberValue(temp_max_item);
    cJSON *temp_min_item = cJSON_GetObjectItem(obj, "temp_min");
    if (temp_min_item == NULL)
        return false;
    msg->temp_min = (uint64_t)cJSON_GetNumberValue(temp_min_item);
    cJSON *phen_item = cJSON_GetObjectItem(obj, "phen");
    if (phen_item == NULL)
        return false;
    msg->phen = (uint64_t)cJSON_GetNumberValue(phen_item);
    cJSON *humi_item = cJSON_GetObjectItem(obj, "humi");
    if (humi_item == NULL)
        return false;
    msg->humi = (uint64_t)cJSON_GetNumberValue(humi_item);
    return true;
}

/*@brief 打包转换 WorldClock 消息为 JSON 对象
 *@param msg 源消息结构体
 *@retval 生成的 JSON 对象(需要调用者 cJSON_Delete 回收)
 */
cJSON *app_json_xfer_pack_world_clock(const AppPB_WorldClock *msg)
{
    cJSON *obj = cJSON_CreateObject();
    if (obj == NULL || msg == NULL)
        return obj;
    cJSON_AddNumberToObject(obj, "now_index", msg->now_index);
    cJSON_AddNumberToObject(obj, "max_count", msg->max_count);
    cJSON_AddStringToObject(obj, "city_name", msg->city_name);
    cJSON_AddNumberToObject(obj, "zone_offset", msg->zone_offset);
    cJSON_AddNumberToObject(obj, "longitude", msg->longitude);
    cJSON_AddNumberToObject(obj, "latitude", msg->latitude);
    cJSON_AddNumberToObject(obj, "city_id", msg->city_id);
    return obj;
}

/*@brief 解析转换 JSON 对象为 WorldClock 消息结构体
 *@param obj 源 JSON 对象
 *@param msg 目标消息结构体
 *@retval 解析是否成功
 */
bool app_json_xfer_unpack_world_clock(cJSON *obj, AppPB_WorldClock *msg)
{
    if (obj == NULL || msg == NULL)
        return false;
    cJSON *now_index_item = cJSON_GetObjectItem(obj, "now_index");
    if (now_index_item == NULL)
        return false;
    msg->now_index = (uint64_t)cJSON_GetNumberValue(now_index_item);
    cJSON *max_count_item = cJSON_GetObjectItem(obj, "max_count");
    if (max_count_item == NULL)
        return false;
    msg->max_count = (uint64_t)cJSON_GetNumberValue(max_count_item);
    cJSON *city_name_item = cJSON_GetObjectItem(obj, "city_name");
    if (city_name_item == NULL)
        return false;
    snprintf(msg->city_name, sizeof(msg->city_name), "%s", cJSON_GetStringValue(city_name_item));
    cJSON *zone_offset_item = cJSON_GetObjectItem(obj, "zone_offset");
    if (zone_offset_item == NULL)
        return false;
    msg->zone_offset = (uint64_t)cJSON_GetNumberValue(zone_offset_item);
    cJSON *longitude_item = cJSON_GetObjectItem(obj, "longitude");
    if (longitude_item == NULL)
        return false;
    msg->longitude = (uint64_t)cJSON_GetNumberValue(longitude_item);
    cJSON *latitude_item = cJSON_GetObjectItem(obj, "latitude");
    if (latitude_item == NULL)
        return false;
    msg->latitude = (uint64_t)cJSON_GetNumberValue(latitude_item);
    cJSON *city_id_item = cJSON_GetObjectItem(obj, "city_id");
    if (city_id_item == NULL)
        return false;
    msg->city_id = (uint64_t)cJSON_GetNumberValue(city_id_item);
    return true;
}

#endif
