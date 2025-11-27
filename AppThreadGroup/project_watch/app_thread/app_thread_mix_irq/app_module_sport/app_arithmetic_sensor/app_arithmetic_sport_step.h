#ifndef APP_ARITHMETIC_SPORT_STEP_H
#define APP_ARITHMETIC_SPORT_STEP_H

/*计步算法:此算法摘要于外界
 *计步原理:找波峰和波谷,最后波峰累加或者波谷累加
 *实现方法:采用的是波谷+三轴合并的计步方式
 *合并优势:抗干扰能力强,数据易处理,不考虑姿态
 *数据指标:50hz采样(20毫秒中断一次,20ms采样20组xyz数据)
 *计步步伐:每个步数之间间隔不能大于两秒(max_two_val = 100)
 *考虑问题:部分运动会不会计步,其他震动会不会计步
 */

typedef struct {
    /* 算法: 计步算法配置参数 */
    struct {
        int32_t dir_line_num;   // 方向更变的最大线条数,超过则抖动非常剧烈
        int32_t dir_vary_num;   // 方向变化的最大计数值,超过则方向改变
        int32_t dir_dalt_val;   // 幅度值(抬手,背手,摆臂)
        int32_t avg_val;        // 最大值与最小值之差
        int32_t min_val;        // 最小绝对值
        int32_t max_two_val;    // 连续两点之间的最大时间差值(2秒)
        int32_t base_step;      // 开始计步数值
    } cfgs[3], args;
    /* 算法: 计步算法参数 */
    uint8_t  up_down;           // 0:down;1:up
    uint8_t  dal_step;          // 分量计步值
    uint8_t  dir_num;           // 方向计数
    uint8_t  dir_vary_num;      // 方向趋势改变计数
    uint8_t  dir_vary_flag;     // 方向改变标志
    int16_t  old_val;           // 上一次值
    int16_t  min_val;           // 最小数值
    int16_t  max_val;           // 最大数值
    int16_t  min_val_l;         // 最后一次的最小数值
    int16_t  max_val_l;         // 最后一次的最大数值
    uint32_t old_pos;           // 上一次的计数点
    uint32_t cur_pos;           // 当前次的计数点
    uint32_t min_pos;           // 最小值的计数点
    uint32_t max_pos;           // 最大值的计数点
    /* 算法: 计步算法参数 */
    uint32_t dal_num;
    uint32_t num_over_y;
    uint32_t cnt;
    /*  */
} app_arithmetic_sport_step_t;

#endif
