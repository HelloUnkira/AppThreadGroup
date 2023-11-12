#ifndef APP_ARITHMETIC_SPORT_H
#define APP_ARITHMETIC_SPORT_H

/* 运动方向数据统计 */
typedef struct {
    uint32_t axis_cnt[2][3];    // [x_up,x_down][y_up,y_down][z_up,z_down]
    uint32_t axis_num;          // 总次数
    uint32_t axis_dir;          // 0:单向;1:双向;2:杂乱方向
} app_arithmetic_sport_dir_t;

/*@brief      统计运动方向数据
 *@param[out] sport_dir 运动方向数据
 *@param[in]  frame     帧数据
 *@param[in]  length    帧数据实际数量
 */
void app_arithmetic_sport_dir_proc(app_arithmetic_sport_dir_t *sport_dir, int32_t frame[][3], uint8_t length);

#endif
