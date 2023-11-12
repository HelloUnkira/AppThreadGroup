#ifndef APP_ARITHMETIC_GESTURE_H
#define APP_ARITHMETIC_GESTURE_H

/*@brief     抬手亮屏,放下息屏等
 *@param[in] frame  帧数据
 *@param[in] length 帧数据实际数量
 */
void app_arithmetic_gesture_wrist(int32_t frame[][3], uint8_t length);

/*@brief     摇手
 *@param[in] frame  帧数据
 *@param[in] length 帧数据实际数量
 */
void app_arithmetic_gesture_shake(int32_t frame[][3], uint8_t length);

#endif
