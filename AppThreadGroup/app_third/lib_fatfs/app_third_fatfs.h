#ifndef APP_THIRD_FATFS_H
#define APP_THIRD_FATFS_H

#define APP_THIRD_FATFS_USE_TEST_MODE   0

/*@brief     文件系统基本信息
 *@param[in] path 根路径
 */
void app_third_fatfs_info(char *path);

/*@brief     递归遍历文件系统的文件列表
 *@param[in] path 根路径
 */
void app_third_fatfs_walk(char *path);

/*@brief 初始化FatFS
 */
void app_third_fatfs_init(void);

/*@brief 反初始化FatFS
 */
void app_third_fatfs_deinit(void);

/*@brief     通过文件系统将文件打包到外存映射中
 *           注意:仅在PC上构建
 *@param[in] path 路径
 *@param[in] num  路径数量
 */
void app_third_fatfs_remake(char *path[], int8_t num);

#endif
