#ifdef APP_MODULE_WORLD_TIME_H
#ifdef APP_MODULE_WORLD_TIME_TABLE_H

/*实现目标:
 *一个代替协议传输的,静态本地导入世界时间时区表
 *因为这个表都是编码时注册
 *最终编译器都会将其转化为函数代码段数据表,不占用内存
 *注意:这个文件不可以以任何方式被其他地方引用
 *     它的唯一使用者为app_module_world_time.c
 */

static const app_module_world_time_t app_module_world_time_table[] = {
    {-1, 0, 0.0, 0.0, "NULL"},
};

#endif
#endif
