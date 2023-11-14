#ifndef APP_ARITHMETIC_SPORT_H
#define APP_ARITHMETIC_SPORT_H

typedef struct {
    /* 多运动方向数据统计 */
    struct {
        uint32_t axis_cnt[2][3];    // [x_up,x_down][y_up,y_down][z_up,z_down]
        uint32_t axis_num;          // 总次数
        uint32_t axis_dir;          // 0:单向;1:双向;2:杂乱方向
    } dir;
    /* 多运动类型数据统计 */
    struct {
        app_module_data_center_sport_mode_t     mode;   /* 算法运动模式 */
        app_module_data_center_sport_mix_type_t type;   /* 多运动类型 */
        union {
            uint8_t buffer[0];
            int32_t data;
        };
    } type;
    /* 运动数据类型统计 */
    app_arithmetic_sport_step_t step;
    app_arithmetic_sport_run_t  run;
    /*  */
} app_arithmetic_sport_t;

/*@brief      运动流程处理
 *@param[out] sport  运动数据
 *@param[in]  frame  帧数据
 *@param[in]  length 帧数据实际数量
 */
void app_arithmetic_sport_proc(app_arithmetic_sport_t *sport, int32_t frame[][3], uint8_t length);

#endif
