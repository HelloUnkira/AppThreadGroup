/*实现目标:
 *    算法组件:运动模组
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_lib.h"
#include "app_thread_group.h"
#include "app_arithmetic_lib.h"

/*@brief 计步参数配置
 *@param sport_step 运动数据实例
 */
static void app_arithmetic_sport_step_cfgs(app_arithmetic_sport_step_t *sport_step)
{
    /*  */
    // 计步算法参数(X轴)
    sport_step->cfgs[0].dir_line_num   = 8;    // 方向更变的最大线条数,超过则抖动非常剧烈
    sport_step->cfgs[0].dir_vary_num   = 5;    // 方向变化的最大计数值,超过则方向改变
    sport_step->cfgs[0].dir_dalt_val   = 150;  // 幅度值(抬手,背手,摆臂)
    sport_step->cfgs[0].avg_val        = 700;  // 最大值与最小值之差
    sport_step->cfgs[0].min_val        = 300;  // 最小值
    sport_step->cfgs[0].max_two_val    = 100;  // 连续两点之间的最大时间差值(2秒)
    sport_step->cfgs[0].base_step      = 20;   // 初始计步数值
    // 计步算法参数(Y轴)
    sport_step->cfgs[1].dir_line_num   = 8;    // 方向更变的最大线条数,超过则抖动非常剧烈
    sport_step->cfgs[1].dir_vary_num   = 5;    // 方向变化的最大计数值,超过则方向改变
    sport_step->cfgs[1].dir_dalt_val   = 150;  // 幅度值(抬手,背手,摆臂)
    sport_step->cfgs[1].avg_val        = 700;  // 最大值与最小值之差
    sport_step->cfgs[1].min_val        = 300;  // 最小值
    sport_step->cfgs[1].max_two_val    = 100;  // 连续两点之间的最大时间差值(2秒)
    sport_step->cfgs[1].base_step      = 20;   // 初始计步数值
    // 计步算法参数(Z轴)
    sport_step->cfgs[2].dir_line_num   = 6;    // 方向更变的最大线条数,超过则抖动非常剧烈
    sport_step->cfgs[2].dir_vary_num   = 5;    // 方向变化的最大计数值,超过则方向改变
    sport_step->cfgs[2].dir_dalt_val   = 190;  // 幅度值(抬手,背手,摆臂)
    sport_step->cfgs[2].avg_val        = 700;  // 最大值与最小值之差
    sport_step->cfgs[2].min_val        = 200;  // 最小值
    sport_step->cfgs[2].max_two_val    = 100;  // 连续两点之间的最大时间差值(2秒)
    sport_step->cfgs[2].base_step      = 20;   // 初始计步数值
    /*  */
}

/*@brief 计步计数
 *@param sport_step  运动数据实例
 *@param value 步数值
 *@param sport_step  运动数据实例
 */
static void app_arithmetic_sport_step_val(app_arithmetic_sport_step_t *sport_step, int32_t value)
{
    /* 如果一定时长内未找到波蜂或波谷,则计步终止 */
    /* 比较宽度是否达标,比较频率是否达标 */
    
    /* 初次计数初始化一下 */
    if (sport_step->cur_pos == 0) {
        sport_step->max_val  = -32767;
        sport_step->min_val  = +32767;
        sport_step->old_val  = value;
        sport_step->max_pos  = 0;
        sport_step->min_pos  = 0;
        sport_step->old_pos  = 0;
    }
    /* 累计计数 */
    sport_step->cur_pos++;
    /* 如果高频波连续出现一定次数,则说明抖动非常剧烈，可以中止计步了 */
    if (sport_step->args.max_two_val  < sport_step->cur_pos - sport_step->old_pos ||
        sport_step->args.dir_line_num < sport_step->dir_num) {
        /* 连续两点超过一定的时长,则计步停止 */
        sport_step->dal_step = -1;
        sport_step->dir_num  = +0;        // 方向计数清0
        sport_step->dir_vary_flag = 0;    // 将方向沿清0
        return;
    }
    /* 0:down;1:up */
    if(sport_step->up_down == 0) {
        /* 下降趋势,更新最小值 */
        if (sport_step->min_val > value) {
            sport_step->min_val = value;
            sport_step->min_pos = sport_step->cur_pos;
            sport_step->dir_vary_num = 0;
            if (sport_step->dir_vary_flag != 1) {
                sport_step->dir_vary_flag  = 1;   // 将方向置为上升沿方向
                sport_step->dir_num++;            // 方向计数加1
            }
        } else {
            /* 当方向变化后,保存最后一次的最小点值 */
            /* 如果最大值与最小值之差大于临界值,则认为是有效的 */
            sport_step->min_val_l = sport_step->min_val;
            int16_t dal_val = sport_step->max_val - sport_step->min_val;
            /* 波谷计步 */
            if (sport_step->dir_vary_flag != 2) {
                sport_step->dir_vary_flag  = 2;   // 将方向置为下降沿方向
                sport_step->dir_num++;            // 方向计数加1
                /* 计算均值 */
                int16_t avg_val = (sport_step->max_val_l + sport_step->min_val_l) / 2;
                avg_val = avg_val > 0 ? avg_val : -avg_val;
                /* 如果最大值与最小值之差小于临界值,则认为是无效的 */
                if (sport_step->args.dir_dalt_val < dal_val) {
                    /* 最大值与最小值的平均值大于临界值 */
                    if (sport_step->args.dir_line_num > sport_step->dir_num &&
                        sport_step->args.avg_val < avg_val) {
                        /* 计步值自加,波谷计步 */
                      //sport_step->dal_step++;
                        sport_step->dal_step = 0; // 波谷不累计,但是也要改变状态
                        sport_step->dir_num  = 0; // 否则会导致stop
                    }
                }
            }
            sport_step->max_pos = sport_step->cur_pos;  // 保存当前的点位置到最大值中
            sport_step->max_val = value;          // 更新最大值
            /* 符合上升趋势,改变方向,找波峰 */
            /* 比较趋势的变化,5次耗时20ms*5,也就是最少100毫秒的趋势数据 */
            if (sport_step->args.dir_vary_num < ++sport_step->dir_vary_num &&
                sport_step->args.dir_dalt_val < dal_val) {
                sport_step->up_down       = 1;    // 改变趋势方向
                sport_step->old_pos       = sport_step->min_pos;
                sport_step->old_val       = sport_step->min_val;
                sport_step->max_val_l     = sport_step->max_val;    // 保存当前的最大点值
                sport_step->dir_vary_flag = 0; 
                sport_step->dir_vary_num  = 0;
            }
        }
    } else {
        /* 上升趋势,更新最大值 */
        if (sport_step->max_val < value) {
            sport_step->max_val = value;
            sport_step->max_pos = sport_step->cur_pos;
            sport_step->dir_vary_num = 0;
            if (sport_step->dir_vary_flag != 3) {
                sport_step->dir_vary_flag  = 3; 
                sport_step->dir_num++; 
            }
        } else {
            /* 当方向变化后,保存最后一次的最大点值 */
            /* 如果最大值与最小值之差大于临界值,则认为是有效的 */
            sport_step->max_val_l = sport_step->max_val;
            int16_t dal_val = sport_step->max_val - sport_step->min_val;
            /* 波峰计步 */
            if (sport_step->dir_vary_flag != 4) {
                sport_step->dir_vary_flag  = 4;   // 将方向置为上升沿方向
                sport_step->dir_num++;            // 方向计数加1,用来过滤高频震动
                /* 计算均值 */
                int16_t avg_val = (sport_step->max_val_l + sport_step->min_val_l) / 2;
                avg_val = avg_val > 0 ? avg_val : -avg_val;
                /* 如果最大值与最小值之差小于临界值,则认为是无效的 */
                if (sport_step->args.dir_dalt_val < dal_val) {
                    /* 最大值与最小值的平均值要大于临界值 */
                    if (sport_step->args.dir_line_num > sport_step->dir_num &&
                        sport_step->args.avg_val < avg_val) {
                        /* 计步值自加,波峰计步 */
                        sport_step->dal_step++;
                      //sport_step->dal_step = 0;
                        sport_step->dir_num  = 0; 
                    }
                }
            }
            sport_step->min_pos = sport_step->cur_pos;  // 保存当前的点位置到最大值中
            sport_step->min_val = value;          // 更新最大值
            /* 符合下降趋势,改变方向,找波谷 */
            /* 比较趋势的变化,5次耗时20ms*5,也就是最少100毫秒的趋势数据 */
            if (sport_step->args.dir_vary_num < ++sport_step->dir_vary_num &&
                sport_step->args.dir_dalt_val < dal_val) {
                sport_step->up_down       = 0;    // 改变趋势方向
                sport_step->old_pos       = sport_step->max_pos;
                sport_step->old_val       = sport_step->max_val;
                sport_step->min_val_l     = sport_step->min_val;    // 保存当前的最小点值
                sport_step->dir_vary_flag = 0; 
                sport_step->dir_vary_num  = 0;
            }
        }
    }
}

void app_arithmetic_sport_step_stop(app_arithmetic_sport_t *sport)
{
}

void app_arithmetic_sport_step_proc(app_arithmetic_sport_t *sport, int32_t frame[][3], uint8_t length)
{
    static int32_t max_y = 0;
    static int32_t min_y = 0;
    
    sport->type.mode = app_module_data_center_sport_walk;
    
    if (sport->step.dal_num == 0)
        max_y = min_y = frame[0][1];
    
    int32_t sum_y = 0;
    int32_t sum_z = 0;
    /* 判断是X轴还是Y轴移动，若X轴平均值大于Y轴，则表示X轴移动，否则表示Y轴移动 */
    for(uint8_t idx = 0; idx < length; idx++)
    {
        sum_y += frame[idx][1];
        sum_z += frame[idx][2];
        /* 计算一段时间内Y轴的最大与最小值 */
        if (max_y < frame[idx][1])
            max_y = frame[idx][1];
        if (min_y > frame[idx][1])
            min_y = frame[idx][1];
    }
    sport->step.dal_num += length;
    sport->step.cnt     += length;
    
    int32_t val_z = sum_z / length;
    uint8_t arg_ofs = 0;
    
    /* 判断Z轴是否垂直向上,此时相当于是抬手平移走路 */
    if (val_z < (-700)) {
        /* 当Z轴运动时,判断Y轴是否也有运动,若Y轴也有运动则不是Z轴的标准运动 */
        if (sport->step.dal_num > 35) { //700ms
            sport->step.dal_num = 0;
            int32_t dal_y = max_y - min_y;
            if (dal_y > (250))
                sport->step.num_over_y = 0;
            if (dal_y > (450))
                sport->step.num_over_y++;
            if (sport->step.num_over_y > 3) {
                
            }
            
        }
        
        
    }
    
}
