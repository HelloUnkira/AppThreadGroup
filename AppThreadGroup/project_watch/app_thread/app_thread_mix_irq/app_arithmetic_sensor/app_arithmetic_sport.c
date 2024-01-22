/*实现目标:
 *    算法组件:运动模组
 */

#define APP_SYS_LOG_LOCAL_STATUS    1
#define APP_SYS_LOG_LOCAL_LEVEL     2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_arithmetic_lib.h"

void app_arithmetic_sport_step_stop(app_arithmetic_sport_t *sport);
void app_arithmetic_sport_step_proc(app_arithmetic_sport_t *sport, int32_t frame[][3], uint8_t length);

void app_arithmetic_sport_run_stop(app_arithmetic_sport_t *sport);
void app_arithmetic_sport_run_proc(app_arithmetic_sport_t *sport, int32_t frame[][3], uint8_t length);

/*@brief 运动流程处理
 *@param sport  运动数据
 *@param frame  帧数据
 *@param length 帧数据实际数量
 */
void app_arithmetic_sport_proc(app_arithmetic_sport_t *sport, int32_t frame[][3], uint8_t length)
{
    /*
     *运动方向检测
     */
    sport->dir.axis_num += length * 3;
    /* 统计数据的有效数量和方向以及次数 */
    for (uint8_t idx = 0; idx < length; idx++) {
        /* 计算Y方向改变数据 */
        if (frame[idx][1] > (+600)) sport->dir.axis_cnt[0][1]++;
        if (frame[idx][1] < (-600)) sport->dir.axis_cnt[1][1]++;
        /* 计算Z方向改变数据 */
        if (frame[idx][2] > (-400)) sport->dir.axis_cnt[0][2]++;
        if (frame[idx][2] < (-800)) sport->dir.axis_cnt[1][2]++;
    }
    /* 数据统计足够,计算方向 */
    if (sport->dir.axis_num > (75)) {   /* 50HZ, 1.5秒 */
        sport->dir.axis_dir = (0);      /* 单向变化 */
        /* 计算Z方向改变数据 */
        if (sport->dir.axis_cnt[0][2] > (10) &&
            sport->dir.axis_cnt[1][2] > (10))
            sport->dir.axis_dir = 2;    /* 杂乱方向变化 */
        /* 计算Y方向改变数据 */
        if (sport->dir.axis_cnt[0][1] > (10) &&
            sport->dir.axis_cnt[1][1] > (10))
            sport->dir.axis_dir = 1;    /* 双向变化 */
        /* 参数重置 */
        for (uint8_t idx0 = 0; idx0 < 2; idx0++)
        for (uint8_t idx1 = 0; idx1 < 3; idx1++)
            sport->dir.axis_cnt[idx0][idx1] = 0;
            sport->dir.axis_num = 0;
    }
    /*
     *运动算法检测(暂未加更复杂的目标)
     *不是所有的运动算法都需要处理这种类型的帧数据
     *运动未开启或运动是健走正常计步模式,其他运动暂一律为跑步模式
     *走路模式共用计步算法
     */
    if (0) ;
    else if (sport->type.type == app_module_data_center_sport_mix_null ||
             sport->type.type == app_module_data_center_sport_mix_walk) {
        if (0) ;
        /* 单向检测:正常计步模式 */
        else if (sport->dir.axis_dir == 0) {
            /* 跑步算法未关闭,关闭它 */
            if (sport->type.mode == app_module_data_center_sport_run)
                app_arithmetic_sport_run_stop(sport);
            /* 现在更新为计步算法 */
            app_arithmetic_sport_step_proc(sport, frame, length);
        }
        /* 双向检测:跑步模式 */
        else if (sport->dir.axis_dir == 1) {
            /* 计步算法未关闭,关闭它 */
            if (sport->type.mode == app_module_data_center_sport_walk)
                app_arithmetic_sport_step_stop(sport);
            /* 现在更新为跑步算法 */
            app_arithmetic_sport_run_proc(sport, frame, length);
        }
        /* 杂向检测:平划摆动则不计步 */
        else ;
    }
    // 继续补充其他模式算法
    // else if (sport->type.type == app_module_data_center_sport_mix_null)
    else {
        /* 跑步算法未关闭,关闭它 */
        if (sport->type.mode == app_module_data_center_sport_walk)
            app_arithmetic_sport_step_stop(sport);
        /* 现在更新为计步算法 */
        app_arithmetic_sport_run_proc(sport, frame, length);
    }
}



/* 算法: 数据总和 */
typedef struct {
    /* 算法: 用户信息默认参数 */
    struct {
        uint8_t lr;             // 0:left hand; 1:right hand
        uint8_t sex:1;          // 0:man;       1:woman
        uint8_t age;
        uint8_t height;         // cm
        uint8_t weight;         // kg
        uint8_t step_spacing;   // 步间距
    } user_info;
    /* 算法: 三轴缓存数据,带方向转化 */
    struct {
        int16_t xyz_data[3];
        int16_t num;
        int16_t y_up;
        int16_t y_down;
        int16_t z_up;
        int16_t z_down;
        int16_t dir;    // 数据变化(0:单方向; 1:双方向; 2:杂乱)
    } axis_info;
    /* 算法: 运动数据类型 */
    struct {
        app_module_data_center_sport_mode_t         type;
        app_module_data_center_sport_mix_type_t     sport_type;
    } sport_type;
    /* 算法: 运动数据 */
    struct {
        uint32_t utc;
        uint32_t step;
        uint32_t distance;
        uint32_t calories;
        uint32_t duration;
    } sport_info;
    /*  */
} app_arithmetic_data_t;



/*@brief 用户信息默认参数更新
 *@param data 算法数据实例
 */
void app_arithmetic_data_update_user_info(app_arithmetic_data_t *data)
{
    if (data->user_info.height == 0) {
        data->user_info.height  = 170;
        data->user_info.weight  = 60;
        data->user_info.age     = 25;
        data->user_info.sex     = 0;
    }
    if (data->user_info.height <= 120)
        data->user_info.height  = 120;
    if (data->user_info.height >= 220)
        data->user_info.height  = 220;
    /* 通过用户信息计算用户步间距 */
    {
        /* 身高,腿长,腿长一步迈的大,计算行步距离间隔(正常成年男性) */
        data->user_info.step_spacing = data->user_info.height * 45 / 100;
        /* 年纪不同(老人和小孩) */
        if (data->user_info.age < 16 ||
            data->user_info.age > 45)
            data->user_info.step_spacing -= 5;
        /* 性别不同(女性) */
        if (data->user_info.sex == 1)
            data->user_info.step_spacing -= 10;
        /* 最低的正常步间距 */
        if (data->user_info.step_spacing <= 40)
            data->user_info.step_spacing  = 40;
    }
}

/*@brief 使用步数更新运动数据
 *@param data 算法数据实例
 *@param step 步数
 */
void app_arithmetic_data_update_step_sport_info(app_arithmetic_data_t *data, uint32_t step)
{
    if (step == 0)
        return;
    
    app_module_clock_t clock = {0};
    app_module_clock_get_system_clock(&clock);
    /* 保留步数 */
    data->sport_info.step += step;
    /* 保留运动时间(累加) */
    if (data->sport_info.utc == 0) {
        data->sport_info.duration += clock.utc - data->sport_info.utc;
        data->sport_info.utc = clock.utc;
    }
    /* 使用步数更新距离和卡路里 */
    uint32_t distance = data->user_info.step_spacing * data->sport_info.step;
    uint32_t calories = data->user_info.step_spacing * data->sport_info.step * data->user_info.weight;
    /* 使用步数更新距离和卡路里 */
    switch (data->sport_type.type) {
    default:
        distance = distance / 100;
        calories = calories * 0.8214 / 100;
        break;
    case app_module_data_center_sport_run:
        distance = distance * 115 / 100 / 100;
        calories = calories * 1.0360 / 100;
        break;
    case app_module_data_center_sport_walk:
        distance = distance / 100;
        calories = calories * 0.8214 / 100;
        break;
    }
    /* 更新距离和卡路里 */
    data->sport_info.distance = distance;
    data->sport_info.calories = calories;
}



