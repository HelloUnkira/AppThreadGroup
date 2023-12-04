#ifndef APP_MODULE_WEATHER_H
#define APP_MODULE_WEATHER_H

typedef union {
    /* 数据类型1 */
    enum {
        app_mdoule_weather_type1_s = 0,
        app_mdoule_weather_hot,                                 //热
        app_mdoule_weather_cold,                                //冷
        app_mdoule_weather_type1_e,
    } type1;
    /* 数据类型2 */
    enum {
        app_mdoule_weather_type2_s = 0,
        app_module_weather_sunset,                              //日出
        app_module_weather_sunrise,                             //日落
        app_mdoule_weather_type2_e,
    } type2;
    /* 常规数据类型 */
    enum {
        app_mdoule_weather_type_s = 0,
        /* 词缀:待归系列 */
        app_mdoule_weather_sunny,                               //晴天
        app_mdoule_weather_cloudy,                              //多云
        app_mdoule_weather_overcast,                            //阴天
        app_mdoule_weather_typhoon,                             //台风
        app_mdoule_weather_cloudy_to_sunny,                     //多云转晴
        app_mdoule_weather_wind_high,                           //大风
        app_mdoule_weather_wind_cold_breeze,                    //清风
        /* 词缀:雨雪系列 */
        app_mdoule_weather_rain,                                //雨
        app_mdoule_weather_rain_shower,                         //阵雨
        app_mdoule_weather_rain_showery,                        //雷阵雨
        app_mdoule_weather_rain_freezing,                       //冻雨
        app_mdoule_weather_rain_light,                          //小雨
        app_mdoule_weather_rain_moderate,                       //中雨
        app_mdoule_weather_rain_heavy,                          //大雨
        app_mdoule_weather_rain_storm,                          //暴雨
        app_mdoule_weather_rain_downpour,                       //大暴雨
        app_mdoule_weather_rain_downpour_heavy,                 //特大暴雨
        app_mdoule_weather_rain_small_to_moderate,              //小到中雨
        app_mdoule_weather_rain_moderate_to_heavy,              //中到大雨
        app_mdoule_weather_rain_heavy_to_storm,                 //大到暴雨
        app_mdoule_weather_rain_storm_to_torrential,            //暴雨到大暴雨
        app_mdoule_weather_rain_torrential_to_downpour_heavy,  //大暴雨到特大暴雨
        app_mdoule_weather_snow,                                //雪
        app_mdoule_weather_snow_shower,                         //阵雪
        app_mdoule_weather_snow_light,                          //小雪
        app_mdoule_weather_snow_moderate,                       //中雪
        app_mdoule_weather_snow_heavy,                          //大雪
        app_mdoule_weather_snow_storm,                          //暴雪
        app_mdoule_weather_snow_small_to_moderate,              //小到中雪
        app_mdoule_weather_snow_moderate_to_heavy,              //中到大雪
        app_mdoule_weather_snow_heavy_to_snowstorm,             //大到暴雪
        app_mdoule_weather_hail,                                //冰雹
        app_mdoule_weather_sleet,                               //雨夹雪
        /* 词缀:雾沙霾系列 */
        app_mdoule_weather_fog,                                 //雾
        app_mdoule_weather_fog_smog,                            //浓雾
        app_mdoule_weather_fog_heavy,                           //大雾
        app_mdoule_weather_fog_strong,                          //强浓雾
        app_mdoule_weather_fog_strong_especially,               //特强大雾
        app_mdoule_weather_sand,                                //沙
        app_mdoule_weather_sand_storm,                          //沙尘暴
        app_mdoule_weather_sand_floating,                       //浮沙
        app_mdoule_weather_sand_blowing,                        //扬沙
        app_mdoule_weather_sand_strong,                         //强沙尘暴
        app_mdoule_weather_haze,                                //霾
        app_mdoule_weather_haze_moderate,                       //中度霾
        app_mdoule_weather_haze_heavy,                          //重度霾
        app_mdoule_weather_haze_severe,                         //严重霾
        /* 词缀:... */
        /***其他配置(暂不使用):天气情况:
          0x00:其他, 0x01:晴, 0x02:多云,
          0x03:阴,0x04:雨,0x05:暴雨,0x06:雷阵雨,0x07:雪,
          0x08:雨夹雪,0x09:台风,0x0A:沙尘暴,0x0B:夜间晴,0x0C:夜间多云,
          0x0D:热,0x0E:冷,0x0F:清风,0x10:大风,0x11:雾霭,0x12:阵雨,
          0x13:多云转晴,0x14:新月(N),0x15:蛾眉月(WXC),0x16:上弦月(FQ),
          0x17:盈凸月(WXG),0x18:满月(F),0x19:亏凸月(WNG),0x20:下弦月(LQ),0x21:残月(WNC)
        ***/
        app_mdoule_weather_unkown,                              //未知
        app_mdoule_weather_type_e,
    } type;
} app_module_weather_type_t;

#define APP_MODULE_WEATHER_MAX          (10)
#define APP_MODULE_WEATHER_HOUR_MAX     (48)
#define APP_MODULE_WEATHER_FUTURE_MAX   (7)
#define APP_MODULE_WEATHER_CITY_MAX     (75)

typedef struct {
    uint64_t utc;                   //此刻时间
    uint8_t  type;                  //天气类型
    uint8_t  cur_temp;              //当前温度(适配负数,温度加100传输)
    uint8_t  max_temp;              //最大温度(适配负数,温度加100传输)
    uint8_t  min_temp;              //最少温度(适配负数,温度加100传输)
    uint8_t  quality;               //空气质量
    uint8_t  humidity;              //湿度
    uint8_t  ultraviolet;           //紫外线强度
    uint8_t  precipitation;         //降水概率
    uint8_t  wind_speed;            //风速
    uint8_t  hour_item;
    uint8_t  future_item;
    struct {
        uint8_t type;               //天气类型
        uint8_t temp;               //温度(适配负数,温度加100传输)
        uint8_t precipitation;      //降水出现的概率
    } hour[APP_MODULE_WEATHER_HOUR_MAX];
    struct {
        uint8_t sunrise[2];         //日出[时,分]
        uint8_t sunset[2];          //日落[时,分]
        uint8_t type;               //天气类型
        uint8_t max_temp;           //最大温度(适配负数,温度加100传输)
        uint8_t min_temp;           //最少温度(适配负数,温度加100传输)
    } future[APP_MODULE_WEATHER_FUTURE_MAX];
    uint8_t city[APP_MODULE_WEATHER_CITY_MAX];
} app_module_weather_t;

/*@brief 天气设置
 *@param weather 天气参数
 *@param idx     天气索引
 */
void app_module_weather_set(app_module_weather_t *weather, uint8_t idx);

/*@brief 天气获取
 *@param weather 天气参数
 *@param idx     天气索引
 */
void app_module_weather_get(app_module_weather_t *weather, uint8_t idx);

#endif
