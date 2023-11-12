/*实现目标:
 *    算法组件:运动模组
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"

/*@brief      统计运动方向数据
 *@param[out] sport_dir 运动方向数据
 *@param[in]  frame     帧数据
 *@param[in]  length    帧数据实际数量
 */
void app_arithmetic_sport_dir_proc(app_arithmetic_sport_dir_t *sport_dir, int32_t frame[][3], uint8_t length)
{
    sport_dir->axis_num += length * 3;
    /* 统计数据的有效数量和方向以及次数 */
    for (uint8_t idx = 0; idx < length; idx++) {
        /* 计算Y方向改变数据 */
        if (frame[idx][1] > (+600)) sport_dir->axis_cnt[0][1]++;
        if (frame[idx][1] < (-600)) sport_dir->axis_cnt[1][1]++;
        /* 计算Z方向改变数据 */
        if (frame[idx][2] > (-400)) sport_dir->axis_cnt[0][2]++;
        if (frame[idx][2] < (-800)) sport_dir->axis_cnt[1][2]++;
    }
    /* 数据统计足够,计算方向 */
    if (sport_dir->axis_num > (75)) {   /* 50HZ, 1.5秒 */
        sport_dir->axis_dir = (0);      /* 单向变化 */
        /* 计算Z方向改变数据 */
        if (sport_dir->axis_cnt[0][2] > (10) &&
            sport_dir->axis_cnt[1][2] > (10))
            sport_dir->axis_dir = 2;    /* 杂乱方向变化 */
        /* 计算Y方向改变数据 */
        if (sport_dir->axis_cnt[0][1] > (10) &&
            sport_dir->axis_cnt[1][1] > (10))
            sport_dir->axis_dir = 1;    /* 双向变化 */
        /* 参数重置 */
        for (uint8_t idx0 = 0; idx0 < 2; idx0++)
        for (uint8_t idx1 = 0; idx1 < 3; idx1++)
            sport_dir->axis_cnt[idx0][idx1] = 0;
            sport_dir->axis_num = 0;
    }
}
