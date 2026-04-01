/*实现目标:
 *    图片资源缓存
 */

#define SCUI_LOG_LOCAL_STATUS       1
#define SCUI_LOG_LOCAL_LEVEL        2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "scui.h"

static scui_cache_image_t scui_cache_image = {0};

/*@brief 哈希散列函数,哈希摘要函数
 */
static uint32_t scui_cache_image_fd_t(scui_table_dln_t *node)
{
    scui_cache_lru_unit_t *unit = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node);
    scui_cache_image_unit_t *image_unit = scui_own_ofs(scui_cache_image_unit_t, lru_unit, unit);
    
    return scui_cache_lru_hash((void *)&image_unit->image->pixel.data_bin, sizeof(uintptr_t));
}

/*@brief 哈希比较函数
 */
static bool scui_cache_image_fc_t(scui_table_dln_t *node1, scui_table_dln_t *node2)
{
    scui_cache_lru_unit_t *unit1 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node1);
    scui_cache_lru_unit_t *unit2 = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node2);
    scui_cache_image_unit_t *image_unit1 = scui_own_ofs(scui_cache_image_unit_t, lru_unit, unit1);
    scui_cache_image_unit_t *image_unit2 = scui_own_ofs(scui_cache_image_unit_t, lru_unit, unit2);
    
    /* 来源和地址同步匹配才可确认资源一致性 */
    bool cond_bin = false;
    bool cond_tar = false;
    
    if (image_unit1->image->pixel.data_bin ==
        image_unit2->image->pixel.data_bin)
        cond_bin = true;
    
    if (image_unit1->image->from == SCUI_HANDLE_INVALID &&
        image_unit2->image->from == SCUI_HANDLE_INVALID)
        cond_tar = true;
    if (image_unit1->image->from != SCUI_HANDLE_INVALID &&
        image_unit2->image->from != SCUI_HANDLE_INVALID) {
        const char *from_str1 = scui_handle_source(image_unit1->image->from);
        const char *from_str2 = scui_handle_source(image_unit2->image->from);
        if (strcmp(from_str1, from_str2) == 0)
            cond_tar = true;
    }
    
    return cond_bin && cond_tar ? true : false;
}

/*@brief 哈希访问函数
 */
static void scui_cache_image_fv_t(scui_table_dln_t *node, uint32_t idx)
{
    scui_cache_lru_unit_t *unit = scui_own_ofs(scui_cache_lru_unit_t, ht_node, node);
    scui_cache_image_unit_t *image_unit = scui_own_ofs(scui_cache_image_unit_t, lru_unit, unit);
    
    static const char *image_unit_local = "local";
    char * image_unit_from = image_unit_local;
    if (image_unit->image->from != SCUI_HANDLE_INVALID)
        image_unit_from = scui_handle_source(image_unit->image->from);
    
    SCUI_LOG_INFO("- width:%x",          image_unit->image->pixel.width);
    SCUI_LOG_INFO("- height:%x",         image_unit->image->pixel.height);
    SCUI_LOG_INFO("- data_bin<src>:%x",  image_unit->image->pixel.data_bin);
    SCUI_LOG_INFO("- size_bin<src>:%x",  image_unit->image->pixel.size_bin);
    SCUI_LOG_INFO("- type:%x",           image_unit->image->type);
    SCUI_LOG_INFO("- format:%x",         image_unit->image->format);
    SCUI_LOG_INFO("- from:%s",           image_unit_from);
    SCUI_LOG_INFO("- count:%x",          unit->count);
    SCUI_LOG_INFO("- lock:%x",           unit->lock);
}

/*@brief 节点占用回调
 */
static scui_multi_t scui_cache_image_unit_get_size(scui_cache_lru_unit_t *unit)
{
    scui_cache_image_unit_t *image_unit = scui_own_ofs(scui_cache_image_unit_t, lru_unit, unit);
    return scui_image_size(image_unit->image);
}

/*@brief 节点销毁回调
 */
static void scui_cache_image_unit_new_node(scui_cache_lru_unit_t **unit, scui_cache_lru_unit_t *unit_t)
{
    scui_cache_image_unit_t *image_unit_t = scui_own_ofs(scui_cache_image_unit_t, lru_unit, unit_t);
    scui_cache_image_unit_t *image_unit = SCUI_MEM_RALLOC(scui_mem_type_graph, sizeof(scui_cache_image_unit_t));
    image_unit->data  = SCUI_MEM_RALLOC(scui_mem_type_graph, scui_image_size(image_unit_t->image));
    image_unit->image = image_unit_t->image;
    /* 图片资源加载 */
    scui_image_src_read(image_unit->image, image_unit->data);
    
    *unit = &image_unit->lru_unit;
}

/*@brief 节点销毁回调
 */
static void scui_cache_image_unit_cpy_node(scui_cache_lru_unit_t *unit, scui_cache_lru_unit_t *unit_t)
{
    scui_cache_image_unit_t *image_unit_t = scui_own_ofs(scui_cache_image_unit_t, lru_unit, unit_t);
    scui_cache_image_unit_t *image_unit   = scui_own_ofs(scui_cache_image_unit_t, lru_unit, unit);
    *image_unit = *image_unit_t;
}

/*@brief 节点创建回调
 */
static void scui_cache_image_unit_del_node(scui_cache_lru_unit_t  *unit)
{
    scui_cache_image_unit_t *image_unit = scui_own_ofs(scui_cache_image_unit_t, lru_unit, unit);
    /* 图片资源卸载 */
    SCUI_MEM_FREE(image_unit->data);
    SCUI_MEM_FREE(image_unit);
}

/*@brief 图片初始化
 */
void scui_cache_image_ready(void)
{
    #if SCUI_CACHE_HASH_IMAGE != 0
    scui_cache_image_t *cache = &scui_cache_image;
    
    cache->lru_table.dlt_fd = scui_cache_image_fd_t;
    cache->lru_table.dlt_fc = scui_cache_image_fc_t;
    cache->lru_table.dlt_fv = scui_cache_image_fv_t;
    cache->lru_table.ht_list_num = SCUI_CACHE_HASH_IMAGE;
    cache->lru_table.total = SCUI_CACHE_TOTAL_IMAGE;
    cache->lru_table.get_size = scui_cache_image_unit_get_size;
    cache->lru_table.new_node = scui_cache_image_unit_new_node;
    cache->lru_table.cpy_node = scui_cache_image_unit_cpy_node;
    cache->lru_table.del_node = scui_cache_image_unit_del_node;
    scui_cache_lru_ready(&cache->lru_table);
    #endif
}

/*@brief 图片资源重校正
 *       将计数器重衰减到0以刷新权重
 */
void scui_cache_image_rectify(void)
{
    #if SCUI_CACHE_HASH_IMAGE != 0
    scui_cache_image_t *cache = &scui_cache_image;
    scui_cache_lru_rectify(&cache->lru_table);
    #endif
}

/*@brief 图片资源检查
 */
void scui_cache_image_visit(void)
{
    #if SCUI_CACHE_HASH_IMAGE != 0
    scui_cache_image_t *cache = &scui_cache_image;
    scui_cache_lru_visit(&cache->lru_table);
    #endif
}

/*@brief 图片资源使用
 *@param usage 图片资源使用
 */
void scui_cache_image_usage(uint32_t *usage)
{
    #if SCUI_CACHE_HASH_IMAGE != 0
    scui_cache_image_t *cache = &scui_cache_image;
    scui_cache_lru_usage(&cache->lru_table, usage);
    #endif
}

/*@brief 图片资源数量
 *@param nodes 图片资源数量
 */
void scui_cache_image_nodes(uint32_t *nodes)
{
    #if SCUI_CACHE_HASH_IMAGE != 0
    scui_cache_image_t *cache = &scui_cache_image;
    scui_cache_lru_nodes(&cache->lru_table, nodes);
    #endif
}

/*@brief 图片资源清除
 */
void scui_cache_image_clear(void)
{
    #if SCUI_CACHE_HASH_IMAGE != 0
    scui_cache_image_t *cache = &scui_cache_image;
    scui_cache_lru_clear(&cache->lru_table, false);
    #endif
}

/*@brief 图片资源缓存无效化(指定目标)
 *@brief image_unit 图片资源缓存节点
 */
void scui_cache_image_invalidate(scui_cache_image_unit_t *image_unit)
{
    SCUI_ASSERT(image_unit != NULL);
    /* 内存图片直达(不走缓存管理) */
    if (image_unit->image->type == scui_image_type_mem) {
        image_unit->data = image_unit->image->pixel.data_bin;
        return;
    }
    
    #if SCUI_CACHE_HASH_IMAGE != 0
    scui_cache_image_t *cache = &scui_cache_image;
    scui_cache_lru_invalidate(&cache->lru_table, &image_unit->lru_unit);
    #endif
}

/*@brief 图片资源卸载
 *@brief image_unit 图片资源缓存节点
 */
void scui_cache_image_unload(scui_cache_image_unit_t *image_unit)
{
    SCUI_ASSERT(image_unit != NULL);
    /* 内存图片直达(不走缓存管理) */
    if (image_unit->image->type == scui_image_type_mem) {
        image_unit->data = image_unit->image->pixel.data_bin;
        return;
    }
    
    #if SCUI_CACHE_HASH_IMAGE != 0
    scui_cache_image_t *cache = &scui_cache_image;
    scui_cache_lru_unload(&cache->lru_table, &image_unit->lru_unit);
    #else
    /* 卸载图像资源 */
    SCUI_MEM_FREE(image_unit->data);
    #endif
}

/*@brief 图片资源加载
 */
void scui_cache_image_load(scui_cache_image_unit_t *image_unit)
{
    SCUI_ASSERT(image_unit != NULL);
    /* 内存图片直达(不走缓存管理) */
    if (image_unit->image->type == scui_image_type_mem) {
        image_unit->data = image_unit->image->pixel.data_bin;
        return;
    }
    
    #if SCUI_CACHE_HASH_IMAGE != 0
    scui_cache_image_t *cache = &scui_cache_image;
    scui_cache_lru_load(&cache->lru_table, &image_unit->lru_unit);
    #else
    /* 加载图片资源 */
    uintptr_t image_size = scui_image_size(image_unit->image);
    image_unit->data = SCUI_MEM_RALLOC(scui_mem_type_graph, image_size);
    scui_image_src_read(image_unit->image, image_unit->data);
    #endif
}
