#ifndef APP_LV_PIC_H
#define APP_LV_PIC_H

/*本地静态的图片路径宏表
 *通过app_lv_pic.py生成
 *脚本会递归遍历..\lvgl_pic下所有文件
 *并读取相对路径与名字合成宏
 */

typedef enum {
	APP_LV_PIC_LVGL_JPG,                             // S:lvgl_pic/LVGL.jpg
	APP_LV_PIC_LVGL_PNG,                             // S:lvgl_pic/LVGL.png
	APP_LV_PIC_THEME_01_SPORTS_112_PNG,              // S:lvgl_pic/theme/01_sports/112.png
	APP_LV_PIC_THEME_01_SPORTS_44_PNG,               // S:lvgl_pic/theme/01_sports/44.png
	APP_LV_PIC_THEME_01_SPORTS_52_PNG,               // S:lvgl_pic/theme/01_sports/52.png
	APP_LV_PIC_THEME_01_SPORTS_60_PNG,               // S:lvgl_pic/theme/01_sports/60.png
	APP_LV_PIC_THEME_01_SPORTS_68_PNG,               // S:lvgl_pic/theme/01_sports/68.png
	APP_LV_PIC_THEME_01_SPORTS_76_PNG,               // S:lvgl_pic/theme/01_sports/76.png
	APP_LV_PIC_THEME_01_SPORTS_84_PNG,               // S:lvgl_pic/theme/01_sports/84.png
	APP_LV_PIC_THEME_01_SPORTS_92_PNG,               // S:lvgl_pic/theme/01_sports/92.png
	APP_LV_PIC_THEME_02_SPORTS_RECORD_112_PNG,       // S:lvgl_pic/theme/02_sports_record/112.png
	APP_LV_PIC_THEME_02_SPORTS_RECORD_44_PNG,        // S:lvgl_pic/theme/02_sports_record/44.png
	APP_LV_PIC_THEME_02_SPORTS_RECORD_52_PNG,        // S:lvgl_pic/theme/02_sports_record/52.png
	APP_LV_PIC_THEME_02_SPORTS_RECORD_60_PNG,        // S:lvgl_pic/theme/02_sports_record/60.png
	APP_LV_PIC_THEME_02_SPORTS_RECORD_68_PNG,        // S:lvgl_pic/theme/02_sports_record/68.png
	APP_LV_PIC_THEME_02_SPORTS_RECORD_76_PNG,        // S:lvgl_pic/theme/02_sports_record/76.png
	APP_LV_PIC_THEME_02_SPORTS_RECORD_84_PNG,        // S:lvgl_pic/theme/02_sports_record/84.png
	APP_LV_PIC_THEME_02_SPORTS_RECORD_92_PNG,        // S:lvgl_pic/theme/02_sports_record/92.png
	APP_LV_PIC_THEME_03_DAILY_EXERCISE_112_PNG,      // S:lvgl_pic/theme/03_daily_exercise/112.png
	APP_LV_PIC_THEME_03_DAILY_EXERCISE_44_PNG,       // S:lvgl_pic/theme/03_daily_exercise/44.png
	APP_LV_PIC_THEME_03_DAILY_EXERCISE_52_PNG,       // S:lvgl_pic/theme/03_daily_exercise/52.png
	APP_LV_PIC_THEME_03_DAILY_EXERCISE_60_PNG,       // S:lvgl_pic/theme/03_daily_exercise/60.png
	APP_LV_PIC_THEME_03_DAILY_EXERCISE_68_PNG,       // S:lvgl_pic/theme/03_daily_exercise/68.png
	APP_LV_PIC_THEME_03_DAILY_EXERCISE_76_PNG,       // S:lvgl_pic/theme/03_daily_exercise/76.png
	APP_LV_PIC_THEME_03_DAILY_EXERCISE_84_PNG,       // S:lvgl_pic/theme/03_daily_exercise/84.png
	APP_LV_PIC_THEME_03_DAILY_EXERCISE_92_PNG,       // S:lvgl_pic/theme/03_daily_exercise/92.png
	APP_LV_PIC_THEME_04_HR_112_PNG,                  // S:lvgl_pic/theme/04_hr/112.png
	APP_LV_PIC_THEME_04_HR_44_PNG,                   // S:lvgl_pic/theme/04_hr/44.png
	APP_LV_PIC_THEME_04_HR_52_PNG,                   // S:lvgl_pic/theme/04_hr/52.png
	APP_LV_PIC_THEME_04_HR_60_PNG,                   // S:lvgl_pic/theme/04_hr/60.png
	APP_LV_PIC_THEME_04_HR_68_PNG,                   // S:lvgl_pic/theme/04_hr/68.png
	APP_LV_PIC_THEME_04_HR_76_PNG,                   // S:lvgl_pic/theme/04_hr/76.png
	APP_LV_PIC_THEME_04_HR_84_PNG,                   // S:lvgl_pic/theme/04_hr/84.png
	APP_LV_PIC_THEME_04_HR_92_PNG,                   // S:lvgl_pic/theme/04_hr/92.png
	APP_LV_PIC_THEME_05_BO_112_PNG,                  // S:lvgl_pic/theme/05_bo/112.png
	APP_LV_PIC_THEME_05_BO_44_PNG,                   // S:lvgl_pic/theme/05_bo/44.png
	APP_LV_PIC_THEME_05_BO_52_PNG,                   // S:lvgl_pic/theme/05_bo/52.png
	APP_LV_PIC_THEME_05_BO_60_PNG,                   // S:lvgl_pic/theme/05_bo/60.png
	APP_LV_PIC_THEME_05_BO_68_PNG,                   // S:lvgl_pic/theme/05_bo/68.png
	APP_LV_PIC_THEME_05_BO_76_PNG,                   // S:lvgl_pic/theme/05_bo/76.png
	APP_LV_PIC_THEME_05_BO_84_PNG,                   // S:lvgl_pic/theme/05_bo/84.png
	APP_LV_PIC_THEME_05_BO_92_PNG,                   // S:lvgl_pic/theme/05_bo/92.png
	APP_LV_PIC_THEME_06_SLEEP_112_PNG,               // S:lvgl_pic/theme/06_sleep/112.png
	APP_LV_PIC_THEME_06_SLEEP_44_PNG,                // S:lvgl_pic/theme/06_sleep/44.png
	APP_LV_PIC_THEME_06_SLEEP_52_PNG,                // S:lvgl_pic/theme/06_sleep/52.png
	APP_LV_PIC_THEME_06_SLEEP_60_PNG,                // S:lvgl_pic/theme/06_sleep/60.png
	APP_LV_PIC_THEME_06_SLEEP_68_PNG,                // S:lvgl_pic/theme/06_sleep/68.png
	APP_LV_PIC_THEME_06_SLEEP_76_PNG,                // S:lvgl_pic/theme/06_sleep/76.png
	APP_LV_PIC_THEME_06_SLEEP_84_PNG,                // S:lvgl_pic/theme/06_sleep/84.png
	APP_LV_PIC_THEME_06_SLEEP_92_PNG,                // S:lvgl_pic/theme/06_sleep/92.png
	APP_LV_PIC_THEME_07_CALL_112_PNG,                // S:lvgl_pic/theme/07_call/112.png
	APP_LV_PIC_THEME_07_CALL_44_PNG,                 // S:lvgl_pic/theme/07_call/44.png
	APP_LV_PIC_THEME_07_CALL_52_PNG,                 // S:lvgl_pic/theme/07_call/52.png
	APP_LV_PIC_THEME_07_CALL_60_PNG,                 // S:lvgl_pic/theme/07_call/60.png
	APP_LV_PIC_THEME_07_CALL_68_PNG,                 // S:lvgl_pic/theme/07_call/68.png
	APP_LV_PIC_THEME_07_CALL_76_PNG,                 // S:lvgl_pic/theme/07_call/76.png
	APP_LV_PIC_THEME_07_CALL_84_PNG,                 // S:lvgl_pic/theme/07_call/84.png
	APP_LV_PIC_THEME_07_CALL_92_PNG,                 // S:lvgl_pic/theme/07_call/92.png
	APP_LV_PIC_THEME_08_WEATHER_112_PNG,             // S:lvgl_pic/theme/08_weather/112.png
	APP_LV_PIC_THEME_08_WEATHER_44_PNG,              // S:lvgl_pic/theme/08_weather/44.png
	APP_LV_PIC_THEME_08_WEATHER_52_PNG,              // S:lvgl_pic/theme/08_weather/52.png
	APP_LV_PIC_THEME_08_WEATHER_60_PNG,              // S:lvgl_pic/theme/08_weather/60.png
	APP_LV_PIC_THEME_08_WEATHER_68_PNG,              // S:lvgl_pic/theme/08_weather/68.png
	APP_LV_PIC_THEME_08_WEATHER_76_PNG,              // S:lvgl_pic/theme/08_weather/76.png
	APP_LV_PIC_THEME_08_WEATHER_84_PNG,              // S:lvgl_pic/theme/08_weather/84.png
	APP_LV_PIC_THEME_08_WEATHER_92_PNG,              // S:lvgl_pic/theme/08_weather/92.png
	APP_LV_PIC_THEME_09_MUSIC_112_PNG,               // S:lvgl_pic/theme/09_music/112.png
	APP_LV_PIC_THEME_09_MUSIC_44_PNG,                // S:lvgl_pic/theme/09_music/44.png
	APP_LV_PIC_THEME_09_MUSIC_52_PNG,                // S:lvgl_pic/theme/09_music/52.png
	APP_LV_PIC_THEME_09_MUSIC_60_PNG,                // S:lvgl_pic/theme/09_music/60.png
	APP_LV_PIC_THEME_09_MUSIC_68_PNG,                // S:lvgl_pic/theme/09_music/68.png
	APP_LV_PIC_THEME_09_MUSIC_76_PNG,                // S:lvgl_pic/theme/09_music/76.png
	APP_LV_PIC_THEME_09_MUSIC_84_PNG,                // S:lvgl_pic/theme/09_music/84.png
	APP_LV_PIC_THEME_09_MUSIC_92_PNG,                // S:lvgl_pic/theme/09_music/92.png
	APP_LV_PIC_THEME_10_MESSAGE_112_PNG,             // S:lvgl_pic/theme/10_message/112.png
	APP_LV_PIC_THEME_10_MESSAGE_44_PNG,              // S:lvgl_pic/theme/10_message/44.png
	APP_LV_PIC_THEME_10_MESSAGE_52_PNG,              // S:lvgl_pic/theme/10_message/52.png
	APP_LV_PIC_THEME_10_MESSAGE_60_PNG,              // S:lvgl_pic/theme/10_message/60.png
	APP_LV_PIC_THEME_10_MESSAGE_68_PNG,              // S:lvgl_pic/theme/10_message/68.png
	APP_LV_PIC_THEME_10_MESSAGE_76_PNG,              // S:lvgl_pic/theme/10_message/76.png
	APP_LV_PIC_THEME_10_MESSAGE_84_PNG,              // S:lvgl_pic/theme/10_message/84.png
	APP_LV_PIC_THEME_10_MESSAGE_92_PNG,              // S:lvgl_pic/theme/10_message/92.png
	APP_LV_PIC_THEME_12_STRESS_112_PNG,              // S:lvgl_pic/theme/12_stress/112.png
	APP_LV_PIC_THEME_12_STRESS_44_PNG,               // S:lvgl_pic/theme/12_stress/44.png
	APP_LV_PIC_THEME_12_STRESS_52_PNG,               // S:lvgl_pic/theme/12_stress/52.png
	APP_LV_PIC_THEME_12_STRESS_60_PNG,               // S:lvgl_pic/theme/12_stress/60.png
	APP_LV_PIC_THEME_12_STRESS_68_PNG,               // S:lvgl_pic/theme/12_stress/68.png
	APP_LV_PIC_THEME_12_STRESS_76_PNG,               // S:lvgl_pic/theme/12_stress/76.png
	APP_LV_PIC_THEME_12_STRESS_84_PNG,               // S:lvgl_pic/theme/12_stress/84.png
	APP_LV_PIC_THEME_12_STRESS_92_PNG,               // S:lvgl_pic/theme/12_stress/92.png
	APP_LV_PIC_THEME_13_BREATHING_TRAINING_112_PNG,  // S:lvgl_pic/theme/13_breathing_training/112.png
	APP_LV_PIC_THEME_13_BREATHING_TRAINING_44_PNG,   // S:lvgl_pic/theme/13_breathing_training/44.png
	APP_LV_PIC_THEME_13_BREATHING_TRAINING_52_PNG,   // S:lvgl_pic/theme/13_breathing_training/52.png
	APP_LV_PIC_THEME_13_BREATHING_TRAINING_60_PNG,   // S:lvgl_pic/theme/13_breathing_training/60.png
	APP_LV_PIC_THEME_13_BREATHING_TRAINING_68_PNG,   // S:lvgl_pic/theme/13_breathing_training/68.png
	APP_LV_PIC_THEME_13_BREATHING_TRAINING_76_PNG,   // S:lvgl_pic/theme/13_breathing_training/76.png
	APP_LV_PIC_THEME_13_BREATHING_TRAINING_84_PNG,   // S:lvgl_pic/theme/13_breathing_training/84.png
	APP_LV_PIC_THEME_13_BREATHING_TRAINING_92_PNG,   // S:lvgl_pic/theme/13_breathing_training/92.png
	APP_LV_PIC_THEME_14_ALARM_112_PNG,               // S:lvgl_pic/theme/14_alarm/112.png
	APP_LV_PIC_THEME_14_ALARM_44_PNG,                // S:lvgl_pic/theme/14_alarm/44.png
	APP_LV_PIC_THEME_14_ALARM_52_PNG,                // S:lvgl_pic/theme/14_alarm/52.png
	APP_LV_PIC_THEME_14_ALARM_60_PNG,                // S:lvgl_pic/theme/14_alarm/60.png
	APP_LV_PIC_THEME_14_ALARM_68_PNG,                // S:lvgl_pic/theme/14_alarm/68.png
	APP_LV_PIC_THEME_14_ALARM_76_PNG,                // S:lvgl_pic/theme/14_alarm/76.png
	APP_LV_PIC_THEME_14_ALARM_84_PNG,                // S:lvgl_pic/theme/14_alarm/84.png
	APP_LV_PIC_THEME_14_ALARM_92_PNG,                // S:lvgl_pic/theme/14_alarm/92.png
	APP_LV_PIC_THEME_15_VOICE_ASSISTANT_112_PNG,     // S:lvgl_pic/theme/15_voice_assistant/112.png
	APP_LV_PIC_THEME_15_VOICE_ASSISTANT_44_PNG,      // S:lvgl_pic/theme/15_voice_assistant/44.png
	APP_LV_PIC_THEME_15_VOICE_ASSISTANT_52_PNG,      // S:lvgl_pic/theme/15_voice_assistant/52.png
	APP_LV_PIC_THEME_15_VOICE_ASSISTANT_60_PNG,      // S:lvgl_pic/theme/15_voice_assistant/60.png
	APP_LV_PIC_THEME_15_VOICE_ASSISTANT_68_PNG,      // S:lvgl_pic/theme/15_voice_assistant/68.png
	APP_LV_PIC_THEME_15_VOICE_ASSISTANT_76_PNG,      // S:lvgl_pic/theme/15_voice_assistant/76.png
	APP_LV_PIC_THEME_15_VOICE_ASSISTANT_84_PNG,      // S:lvgl_pic/theme/15_voice_assistant/84.png
	APP_LV_PIC_THEME_15_VOICE_ASSISTANT_92_PNG,      // S:lvgl_pic/theme/15_voice_assistant/92.png
	APP_LV_PIC_THEME_16_FEMALE_112_PNG,              // S:lvgl_pic/theme/16_female/112.png
	APP_LV_PIC_THEME_16_FEMALE_44_PNG,               // S:lvgl_pic/theme/16_female/44.png
	APP_LV_PIC_THEME_16_FEMALE_52_PNG,               // S:lvgl_pic/theme/16_female/52.png
	APP_LV_PIC_THEME_16_FEMALE_60_PNG,               // S:lvgl_pic/theme/16_female/60.png
	APP_LV_PIC_THEME_16_FEMALE_68_PNG,               // S:lvgl_pic/theme/16_female/68.png
	APP_LV_PIC_THEME_16_FEMALE_76_PNG,               // S:lvgl_pic/theme/16_female/76.png
	APP_LV_PIC_THEME_16_FEMALE_84_PNG,               // S:lvgl_pic/theme/16_female/84.png
	APP_LV_PIC_THEME_16_FEMALE_93_PNG,               // S:lvgl_pic/theme/16_female/93.png
	APP_LV_PIC_THEME_17_STOPWATCH_112_PNG,           // S:lvgl_pic/theme/17_stopwatch/112.png
	APP_LV_PIC_THEME_17_STOPWATCH_44_PNG,            // S:lvgl_pic/theme/17_stopwatch/44.png
	APP_LV_PIC_THEME_17_STOPWATCH_52_PNG,            // S:lvgl_pic/theme/17_stopwatch/52.png
	APP_LV_PIC_THEME_17_STOPWATCH_60_PNG,            // S:lvgl_pic/theme/17_stopwatch/60.png
	APP_LV_PIC_THEME_17_STOPWATCH_68_PNG,            // S:lvgl_pic/theme/17_stopwatch/68.png
	APP_LV_PIC_THEME_17_STOPWATCH_76_PNG,            // S:lvgl_pic/theme/17_stopwatch/76.png
	APP_LV_PIC_THEME_17_STOPWATCH_84_PNG,            // S:lvgl_pic/theme/17_stopwatch/84.png
	APP_LV_PIC_THEME_17_STOPWATCH_92_PNG,            // S:lvgl_pic/theme/17_stopwatch/92.png
	APP_LV_PIC_THEME_18_TIMER_112_PNG,               // S:lvgl_pic/theme/18_timer/112.png
	APP_LV_PIC_THEME_18_TIMER_44_PNG,                // S:lvgl_pic/theme/18_timer/44.png
	APP_LV_PIC_THEME_18_TIMER_52_PNG,                // S:lvgl_pic/theme/18_timer/52.png
	APP_LV_PIC_THEME_18_TIMER_60_PNG,                // S:lvgl_pic/theme/18_timer/60.png
	APP_LV_PIC_THEME_18_TIMER_68_PNG,                // S:lvgl_pic/theme/18_timer/68.png
	APP_LV_PIC_THEME_18_TIMER_76_PNG,                // S:lvgl_pic/theme/18_timer/76.png
	APP_LV_PIC_THEME_18_TIMER_84_PNG,                // S:lvgl_pic/theme/18_timer/84.png
	APP_LV_PIC_THEME_18_TIMER_92_PNG,                // S:lvgl_pic/theme/18_timer/92.png
	APP_LV_PIC_THEME_19_FLASHLIGHT_112_PNG,          // S:lvgl_pic/theme/19_flashlight/112.png
	APP_LV_PIC_THEME_19_FLASHLIGHT_44_PNG,           // S:lvgl_pic/theme/19_flashlight/44.png
	APP_LV_PIC_THEME_19_FLASHLIGHT_52_PNG,           // S:lvgl_pic/theme/19_flashlight/52.png
	APP_LV_PIC_THEME_19_FLASHLIGHT_60_PNG,           // S:lvgl_pic/theme/19_flashlight/60.png
	APP_LV_PIC_THEME_19_FLASHLIGHT_68_PNG,           // S:lvgl_pic/theme/19_flashlight/68.png
	APP_LV_PIC_THEME_19_FLASHLIGHT_76_PNG,           // S:lvgl_pic/theme/19_flashlight/76.png
	APP_LV_PIC_THEME_19_FLASHLIGHT_84_PNG,           // S:lvgl_pic/theme/19_flashlight/84.png
	APP_LV_PIC_THEME_19_FLASHLIGHT_92_PNG,           // S:lvgl_pic/theme/19_flashlight/92.png
	APP_LV_PIC_THEME_20_FINDPHONE_112_PNG,           // S:lvgl_pic/theme/20_findphone/112.png
	APP_LV_PIC_THEME_20_FINDPHONE_44_PNG,            // S:lvgl_pic/theme/20_findphone/44.png
	APP_LV_PIC_THEME_20_FINDPHONE_52_PNG,            // S:lvgl_pic/theme/20_findphone/52.png
	APP_LV_PIC_THEME_20_FINDPHONE_60_PNG,            // S:lvgl_pic/theme/20_findphone/60.png
	APP_LV_PIC_THEME_20_FINDPHONE_68_PNG,            // S:lvgl_pic/theme/20_findphone/68.png
	APP_LV_PIC_THEME_20_FINDPHONE_76_PNG,            // S:lvgl_pic/theme/20_findphone/76.png
	APP_LV_PIC_THEME_20_FINDPHONE_84_PNG,            // S:lvgl_pic/theme/20_findphone/84.png
	APP_LV_PIC_THEME_20_FINDPHONE_92_PNG,            // S:lvgl_pic/theme/20_findphone/92.png
	APP_LV_PIC_THEME_21_CAMERA_112_PNG,              // S:lvgl_pic/theme/21_camera/112.png
	APP_LV_PIC_THEME_21_CAMERA_44_PNG,               // S:lvgl_pic/theme/21_camera/44.png
	APP_LV_PIC_THEME_21_CAMERA_52_PNG,               // S:lvgl_pic/theme/21_camera/52.png
	APP_LV_PIC_THEME_21_CAMERA_60_PNG,               // S:lvgl_pic/theme/21_camera/60.png
	APP_LV_PIC_THEME_21_CAMERA_68_PNG,               // S:lvgl_pic/theme/21_camera/68.png
	APP_LV_PIC_THEME_21_CAMERA_76_PNG,               // S:lvgl_pic/theme/21_camera/76.png
	APP_LV_PIC_THEME_21_CAMERA_84_PNG,               // S:lvgl_pic/theme/21_camera/84.png
	APP_LV_PIC_THEME_21_CAMERA_92_PNG,               // S:lvgl_pic/theme/21_camera/92.png
	APP_LV_PIC_THEME_22_CALCULATOR_112_PNG,          // S:lvgl_pic/theme/22_calculator/112.png
	APP_LV_PIC_THEME_22_CALCULATOR_44_PNG,           // S:lvgl_pic/theme/22_calculator/44.png
	APP_LV_PIC_THEME_22_CALCULATOR_52_PNG,           // S:lvgl_pic/theme/22_calculator/52.png
	APP_LV_PIC_THEME_22_CALCULATOR_60_PNG,           // S:lvgl_pic/theme/22_calculator/60.png
	APP_LV_PIC_THEME_22_CALCULATOR_68_PNG,           // S:lvgl_pic/theme/22_calculator/68.png
	APP_LV_PIC_THEME_22_CALCULATOR_76_PNG,           // S:lvgl_pic/theme/22_calculator/76.png
	APP_LV_PIC_THEME_22_CALCULATOR_84_PNG,           // S:lvgl_pic/theme/22_calculator/84.png
	APP_LV_PIC_THEME_22_CALCULATOR_92_PNG,           // S:lvgl_pic/theme/22_calculator/92.png
	APP_LV_PIC_THEME_23_SETTING_112_PNG,             // S:lvgl_pic/theme/23_setting/112.png
	APP_LV_PIC_THEME_23_SETTING_44_PNG,              // S:lvgl_pic/theme/23_setting/44.png
	APP_LV_PIC_THEME_23_SETTING_52_PNG,              // S:lvgl_pic/theme/23_setting/52.png
	APP_LV_PIC_THEME_23_SETTING_60_PNG,              // S:lvgl_pic/theme/23_setting/60.png
	APP_LV_PIC_THEME_23_SETTING_68_PNG,              // S:lvgl_pic/theme/23_setting/68.png
	APP_LV_PIC_THEME_23_SETTING_76_PNG,              // S:lvgl_pic/theme/23_setting/76.png
	APP_LV_PIC_THEME_23_SETTING_84_PNG,              // S:lvgl_pic/theme/23_setting/84.png
	APP_LV_PIC_THEME_23_SETTING_92_PNG,              // S:lvgl_pic/theme/23_setting/92.png
	APP_LV_PIC_THEME_24_MEASURE_112_PNG,             // S:lvgl_pic/theme/24_measure/112.png
	APP_LV_PIC_THEME_24_MEASURE_44_PNG,              // S:lvgl_pic/theme/24_measure/44.png
	APP_LV_PIC_THEME_24_MEASURE_52_PNG,              // S:lvgl_pic/theme/24_measure/52.png
	APP_LV_PIC_THEME_24_MEASURE_60_PNG,              // S:lvgl_pic/theme/24_measure/60.png
	APP_LV_PIC_THEME_24_MEASURE_68_PNG,              // S:lvgl_pic/theme/24_measure/68.png
	APP_LV_PIC_THEME_24_MEASURE_76_PNG,              // S:lvgl_pic/theme/24_measure/76.png
	APP_LV_PIC_THEME_24_MEASURE_84_PNG,              // S:lvgl_pic/theme/24_measure/84.png
	APP_LV_PIC_THEME_24_MEASURE_92_PNG,              // S:lvgl_pic/theme/24_measure/92.png
	APP_LV_PIC_THEME_25_WATCH_112_PNG,               // S:lvgl_pic/theme/25_watch/112.png
	APP_LV_PIC_THEME_25_WATCH_44_PNG,                // S:lvgl_pic/theme/25_watch/44.png
	APP_LV_PIC_THEME_25_WATCH_52_PNG,                // S:lvgl_pic/theme/25_watch/52.png
	APP_LV_PIC_THEME_25_WATCH_60_PNG,                // S:lvgl_pic/theme/25_watch/60.png
	APP_LV_PIC_THEME_25_WATCH_68_PNG,                // S:lvgl_pic/theme/25_watch/68.png
	APP_LV_PIC_THEME_25_WATCH_76_PNG,                // S:lvgl_pic/theme/25_watch/76.png
	APP_LV_PIC_THEME_25_WATCH_84_PNG,                // S:lvgl_pic/theme/25_watch/84.png
	APP_LV_PIC_THEME_25_WATCH_92_PNG,                // S:lvgl_pic/theme/25_watch/92.png
	APP_LV_PIC_THEME_26_ALTITUDE_PRESSURE_112_PNG,   // S:lvgl_pic/theme/26_altitude pressure/112.png
	APP_LV_PIC_THEME_26_ALTITUDE_PRESSURE_44_PNG,    // S:lvgl_pic/theme/26_altitude pressure/44.png
	APP_LV_PIC_THEME_26_ALTITUDE_PRESSURE_52_PNG,    // S:lvgl_pic/theme/26_altitude pressure/52.png
	APP_LV_PIC_THEME_26_ALTITUDE_PRESSURE_60_PNG,    // S:lvgl_pic/theme/26_altitude pressure/60.png
	APP_LV_PIC_THEME_26_ALTITUDE_PRESSURE_68_PNG,    // S:lvgl_pic/theme/26_altitude pressure/68.png
	APP_LV_PIC_THEME_26_ALTITUDE_PRESSURE_76_PNG,    // S:lvgl_pic/theme/26_altitude pressure/76.png
	APP_LV_PIC_THEME_26_ALTITUDE_PRESSURE_84_PNG,    // S:lvgl_pic/theme/26_altitude pressure/84.png
	APP_LV_PIC_THEME_26_ALTITUDE_PRESSURE_92_PNG,    // S:lvgl_pic/theme/26_altitude pressure/92.png
	APP_LV_PIC_THEME_27_COMPASS_112_PNG,             // S:lvgl_pic/theme/27_compass/112.png
	APP_LV_PIC_THEME_27_COMPASS_44_PNG,              // S:lvgl_pic/theme/27_compass/44.png
	APP_LV_PIC_THEME_27_COMPASS_52_PNG,              // S:lvgl_pic/theme/27_compass/52.png
	APP_LV_PIC_THEME_27_COMPASS_60_PNG,              // S:lvgl_pic/theme/27_compass/60.png
	APP_LV_PIC_THEME_27_COMPASS_68_PNG,              // S:lvgl_pic/theme/27_compass/68.png
	APP_LV_PIC_THEME_27_COMPASS_76_PNG,              // S:lvgl_pic/theme/27_compass/76.png
	APP_LV_PIC_THEME_27_COMPASS_84_PNG,              // S:lvgl_pic/theme/27_compass/84.png
	APP_LV_PIC_THEME_27_COMPASS_96_PNG,              // S:lvgl_pic/theme/27_compass/96.png
	APP_LV_PIC_THEME_28_ALIPAY_112_PNG,              // S:lvgl_pic/theme/28_Alipay/112.png
	APP_LV_PIC_THEME_28_ALIPAY_44_PNG,               // S:lvgl_pic/theme/28_Alipay/44.png
	APP_LV_PIC_THEME_28_ALIPAY_52_PNG,               // S:lvgl_pic/theme/28_Alipay/52.png
	APP_LV_PIC_THEME_28_ALIPAY_60_PNG,               // S:lvgl_pic/theme/28_Alipay/60.png
	APP_LV_PIC_THEME_28_ALIPAY_68_PNG,               // S:lvgl_pic/theme/28_Alipay/68.png
	APP_LV_PIC_THEME_28_ALIPAY_76_PNG,               // S:lvgl_pic/theme/28_Alipay/76.png
	APP_LV_PIC_THEME_28_ALIPAY_84_PNG,               // S:lvgl_pic/theme/28_Alipay/84.png
	APP_LV_PIC_THEME_28_ALIPAY_92_PNG,               // S:lvgl_pic/theme/28_Alipay/92.png
	APP_LV_PIC_THEME_29_TEMPERATURE_112_PNG,         // S:lvgl_pic/theme/29_temperature/112.png
	APP_LV_PIC_THEME_29_TEMPERATURE_44_PNG,          // S:lvgl_pic/theme/29_temperature/44.png
	APP_LV_PIC_THEME_29_TEMPERATURE_52_PNG,          // S:lvgl_pic/theme/29_temperature/52.png
	APP_LV_PIC_THEME_29_TEMPERATURE_60_PNG,          // S:lvgl_pic/theme/29_temperature/60.png
	APP_LV_PIC_THEME_29_TEMPERATURE_68_PNG,          // S:lvgl_pic/theme/29_temperature/68.png
	APP_LV_PIC_THEME_29_TEMPERATURE_76_PNG,          // S:lvgl_pic/theme/29_temperature/76.png
	APP_LV_PIC_THEME_29_TEMPERATURE_84_PNG,          // S:lvgl_pic/theme/29_temperature/84.png
	APP_LV_PIC_THEME_29_TEMPERATURE_92_PNG,          // S:lvgl_pic/theme/29_temperature/92.png
	APP_LV_PIC_NUM,
} app_lv_pic_t;

/*@brief     获得图片资源字符串
 *@param[in] index图片资源索引(0~n-1)
 */
const char * app_lv_pic_str_find(uint32_t index);

#endif
