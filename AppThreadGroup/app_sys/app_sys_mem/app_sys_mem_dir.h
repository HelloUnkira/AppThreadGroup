#ifndef APP_SYS_MEM_DIR_H
#define APP_SYS_MEM_DIR_H

/*
 *(动态内存管理)双端分配堆
 *一个较为简单的堆分配策略,类似FreeRTOS的heap4的规则
 *设计俩端分配主要用于解决一类资源的申请和释放的管理
 *尽可能规避掉小内存块切碎连续的内存导致内存碎片
 *尽可能保证大内存块的存在以满足峰值目标使用
 *缺点:
 *    大量分配和回收请求时,速度慢
 *不适用:
 *    碎片化多,周期不稳定(不完全确定啥时候申请和释放)
 *    没有一个预估的生命周期
 *使用目标:
 *    固定生存周期,资源均值较高,数据类型接近
 *    提供较好的峰值资源块请求
 *    例如:场景的图形资源
 *         生存周期从界面加载到结束
 *         可能存在如全局背景图片等大内存块请求
 */

/* 金丝雀检查,不敏感,能力有限 */
#define APP_SYS_MEM_DIR_CANARY     ((uintptr_t)0xA55A5AA5A55A5AA5)

typedef struct {
    uintptr_t canary;
    app_sys_list_dln_t dl_node;
    uintptr_t size;
    uintptr_t offset[0];
} app_sys_mem_dir_item_t;

typedef struct {
    app_sys_list_dll_t dl_list_alloc;
    app_sys_list_dll_t dl_list_free;
    uintptr_t addr;     /* 内存地址 */
    uintptr_t size;     /* 内存大小(字节) */
    uintptr_t used;     /* 内存使用(字节) */
} app_sys_mem_dir_t;

/*@brief 初始化双端分配堆
 *@param mem_dir 双端分配堆实例
 *@param addr    内存地址(平台字节对齐)
 *@param size    内存大小(字节)
 */
void app_sys_mem_dir_ready(app_sys_mem_dir_t *mem_dir, uintptr_t addr, uintptr_t size);

/*@brief 双端分配堆获取内存
 *@param mem_dir 双端分配堆实例
 *@param size    内存大小
 *@param align   指定字节对齐(不小于平台字节对齐, 2的指数)
 *@param way     分配方向(true:正向;false:逆向)
 *@retval 内存地址
 */
void * app_sys_mem_dir_alloc_align(app_sys_mem_dir_t *mem_dir, uintptr_t size, uintptr_t align, bool way);

/*@brief 双端分配堆获取内存
 *@param mem_dir 双端分配堆实例
 *@param size    内存大小
 *@param way     分配方向(true:正向;false:逆向)
 *@retval 内存地址
 */
void * app_sys_mem_dir_alloc(app_sys_mem_dir_t *mem_dir, uintptr_t size, bool way);

/*@brief 双端分配堆重获取内存
 *       realloc语义不提供,应该规避这种情况的使用
 *       它存在造成内存封装过大的情况,使用应该规避掉对这种逻辑的产生
 */

/*@brief 双端分配堆归还内存
 *@param mem_dir 双端分配堆实例
 *@param pointer 内存地址
 */
void app_sys_mem_dir_free(app_sys_mem_dir_t *mem_dir, void *pointer);

/*@brief 双端分配堆内存使用
 *@param mem_olsf 双端分配堆实例
 *@retval 内存大小
 */
uintptr_t app_sys_mem_dir_used(app_sys_mem_dir_t *mem_dir);

/*@brief 双端分配堆计算指定内存
 *@param mem_olsf 双端分配堆实例
 *@param pointer  内存地址
 *@retval 内存大小
 */
uintptr_t app_sys_mem_dir_size(app_sys_mem_dir_t *mem_dir, void *pointer);

/*@brief 双端分配堆计算指定内存所属
 *@param mem_olsf 双端分配堆实例
 *@param pointer  内存地址
 *@retval 包含与否
 */
bool app_sys_mem_dir_inside(app_sys_mem_dir_t *mem_dir, void *pointer);

/*@brief 双端分配堆检查内存
 *@param mem_dir 双端分配堆实例
 */
bool app_sys_mem_dir_check(app_sys_mem_dir_t *mem_dir);

/*@brief 双端分配堆检查内存
 *@param mem_dir 双端分配堆实例
 *@retval 堆状态(正常, 异常)
 */
bool app_sys_mem_dir_valid(app_sys_mem_dir_t *mem_dir);

/*@brief 双端分配堆内存遍历检查
 *@param mem_olsf 双端分配堆实例
 *@param invoke   回调实例
 *@retval 堆状态(正常, 异常)
 */
bool app_sys_mem_dir_walk(app_sys_mem_dir_t *mem_dir, void (*invoke)(void *pointer, bool used));

#endif
