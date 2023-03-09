/*实现目标:
 *    转存到外存的追踪日志信息
 */

#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_std_lib.h"
#include "app_os_adaptor.h"
#include "app_sys_log.h"
#include "app_sys_crc.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_table.h"
#include "app_sys_ext_src.h"
#include "app_sys_ext_src_table.h"
#include "app_module_trace.h"

/*@brief 函数追踪队列
 */
void app_module_trace_func(void *func)
{
    /* 函数地址搭配lst或map查阅 */
    /* 只需要简单的模式筛选既可以从日志中提取函数追踪队列的信息 */
    /* 所以这个功能只需要使用最简单的模式实现即可 */
    /* 同样的这一个功能的信息也无需转存和加载,其意义并不大 */
    APP_SYS_LOG_DEBUG("app_module_trace_func: %p", func);
}

/*@brief 日志追踪队列定义
 */
typedef union {
    uint8_t buffer[0];
    struct {
        uintptr_t offset_head;
        uintptr_t offset_tail;
        uintptr_t trace_zone;
        uint32_t crc32;
    };
} app_module_trace_text_t;

typedef union {
    uint8_t buffer[0];
    struct {
        uint32_t length;
        uint8_t  text[APP_MODULE_TRACE_LOG_MAX + 1];
    };
} app_module_trace_item_t;

static app_mutex_t app_module_trace_text_mutex = {0};
static app_module_trace_text_t app_module_trace_text = {};
static const uint32_t app_module_trace_text_size = sizeof(app_module_trace_text);
static const uint32_t app_module_trace_info_size = sizeof(uintptr_t) * 3;

/*@brief 已使用空间
 */
static uint32_t app_module_trace_text_used(void)
{
    app_mutex_take(&app_module_trace_text_mutex);
    uint32_t ret = app_module_trace_text.offset_tail -
                   app_module_trace_text.offset_head;
    app_mutex_give(&app_module_trace_text_mutex);
    return ret;
}

/*@brief 未使用空间
 */
static uint32_t app_module_trace_text_space(void)
{
    app_mutex_take(&app_module_trace_text_mutex);
    uint32_t ret = app_module_trace_text.trace_zone  -
                   app_module_trace_text.offset_tail +
                   app_module_trace_text.offset_head;
    app_mutex_give(&app_module_trace_text_mutex);
    return ret;
}

/*@brief 日志追踪队列初始化
 */
void app_module_trace_text_ready(void)
{
    app_module_trace_text_t trace_text = {};
    app_mutex_process(&app_module_trace_text_mutex);
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t  *source  = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    /* 外存空间不能小于结构本身 */
    APP_SYS_ASSERT(source->data_size > sizeof(trace_text));
    /* 读取外存的函数追踪队列结构 */
    app_sys_ext_mem_read(ext_mem, source->data_base, trace_text.buffer, sizeof(trace_text));
    uint32_t crc32 = app_sys_crc32(trace_text.buffer, app_module_trace_info_size);
    /* 通过外存的函数追踪队列结构初始化本地函数追踪队列结构 */
    app_module_trace_text = trace_text;
    if (trace_text.crc32 != crc32 ||
        trace_text.trace_zone != source->data_size - sizeof(trace_text)) {
        app_module_trace_text.offset_head = 0;
        app_module_trace_text.offset_tail = 0;
        app_module_trace_text.trace_zone  = source->data_size - sizeof(trace_text);
        uint32_t crc32 = app_sys_crc32(trace_text.buffer, app_module_trace_info_size);
        app_module_trace_text.crc32 = crc32;
    }
}

/*@brief 日志追踪队列重置
 */
void app_module_trace_text_reset(void)
{
    app_module_trace_text_t trace_text = {};
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t  *source  = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    app_mutex_take(&app_module_trace_text_mutex);
    app_module_trace_text.offset_head = 0;
    app_module_trace_text.offset_tail = 0;
    app_module_trace_text.trace_zone  = source->data_size - sizeof(trace_text);
    uint32_t crc32 = app_sys_crc32(trace_text.buffer, app_module_trace_info_size);
    app_module_trace_text.crc32 = crc32;
    app_sys_ext_mem_write(ext_mem, source->data_base, trace_text.buffer, sizeof(trace_text));
    app_mutex_give(&app_module_trace_text_mutex);
}

/*@brief 加载一条日志
 */
static void app_module_trace_text_load_one(app_module_trace_item_t *item)
{
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t  *source  = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    /*  */
    app_mutex_take(&app_module_trace_text_mutex);
    uint32_t head = app_module_trace_text.offset_head;
    uint32_t zone = app_module_trace_text.trace_zone;
    app_mutex_give(&app_module_trace_text_mutex);
    uint32_t base = source->data_base + app_module_trace_text_size;
    uint32_t size = 0; uintptr_t roff = 0;
    /* 因为,我们需要考虑读写跨越了头尾,所以逻辑需要额外复杂点 */
    head %= zone;
    if (head + sizeof(uint32_t) < zone) {
        roff = base + head;
        app_sys_ext_mem_read(ext_mem, roff, item->buffer, sizeof(uint32_t));
    } else {
        size = zone - head;
        roff = base + head;
        app_sys_ext_mem_read(ext_mem, roff, item->buffer, size);
        roff = base;
        app_sys_ext_mem_read(ext_mem, roff, item->buffer + size, sizeof(uint32_t) - size);
    }
    head += sizeof(uint32_t);
    head %= zone;
    app_mutex_take(&app_module_trace_text_mutex);
    app_module_trace_text.offset_head = head;
    app_mutex_give(&app_module_trace_text_mutex);
    /* 因为,我们需要考虑读写跨越了头尾,所以逻辑需要额外复杂点 */
    head %= zone;
    APP_SYS_ASSERT(item->length <= APP_MODULE_TRACE_LOG_MAX + 1);
    if (head + item->length < zone) {
        roff = base + head;
        app_sys_ext_mem_read(ext_mem, roff, item->text, item->length);
    } else {
        size = zone - head;
        roff = base + head;
        app_sys_ext_mem_read(ext_mem, roff, item->text, size);
        roff = base;
        app_sys_ext_mem_read(ext_mem, roff, item->text + size, item->length - size);
    }
    head += item->length;
    head %= zone;
    app_mutex_take(&app_module_trace_text_mutex);
    app_module_trace_text.offset_head = head;
    app_mutex_give(&app_module_trace_text_mutex);
    #if APP_MODULE_CHECK
    APP_SYS_LOG_INFO("item.length:%u", item->length);
    APP_SYS_LOG_INFO("item.text:%s",   item->text);
    #endif
}

/*@brief 转储一条日志
 */
static void app_module_trace_text_dump_one(app_module_trace_item_t *item)
{
    #if APP_MODULE_CHECK
    APP_SYS_LOG_INFO("item.length:%u", item->length);
    APP_SYS_LOG_INFO("item.text:%s",   item->text);
    #endif
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t  *source  = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    /*  */
    app_mutex_take(&app_module_trace_text_mutex);
    uint32_t tail = app_module_trace_text.offset_tail;
    uint32_t zone = app_module_trace_text.trace_zone;
    app_mutex_give(&app_module_trace_text_mutex);
    uint32_t base = source->data_base + app_module_trace_text_size;
    uint32_t size = 0; uintptr_t woff = 0;
    /* 因为,我们需要考虑读写跨越了头尾,所以逻辑需要额外复杂点 */
    tail %= zone;
    if (tail + sizeof(uint32_t) < zone) {
        woff = base + tail;
        app_sys_ext_mem_write(ext_mem, woff, item->buffer, sizeof(uint32_t));
    } else {
        size = zone - tail;
        woff = base + tail;
        app_sys_ext_mem_write(ext_mem, woff, item->buffer, size);
        woff = base;
        app_sys_ext_mem_write(ext_mem, woff, item->buffer + size, sizeof(uint32_t) - size);
    }
    tail += sizeof(uint32_t);
    tail %= zone;
    app_mutex_take(&app_module_trace_text_mutex);
    app_module_trace_text.offset_tail = tail;
    app_mutex_give(&app_module_trace_text_mutex);
    /* 因为,我们需要考虑读写跨越了头尾,所以逻辑需要额外复杂点 */
    tail %= zone;
    APP_SYS_ASSERT(item->length <= APP_MODULE_TRACE_LOG_MAX + 1);
    if (tail + item->length < zone) {
        woff = base + tail;
        app_sys_ext_mem_write(ext_mem, woff, item->text, item->length);
    } else {
        size = zone - tail;
        woff = base + tail;
        app_sys_ext_mem_write(ext_mem, woff, item->text, size);
        woff = base;
        app_sys_ext_mem_write(ext_mem, woff, item->text + size, item->length - size);
    }
    tail += item->length;
    tail %= zone;
    app_mutex_take(&app_module_trace_text_mutex);
    app_module_trace_text.offset_tail = tail;
    app_mutex_give(&app_module_trace_text_mutex);
}

/*@brief 更新头部资源
 */
static void app_module_trace_text_update(void)
{
    app_mutex_take(&app_module_trace_text_mutex);
    app_module_trace_text_t trace_text = app_module_trace_text;
    uint32_t crc32 = app_sys_crc32(trace_text.buffer, app_module_trace_info_size);
    app_module_trace_text.crc32 = crc32;
    app_module_trace_text = trace_text;
    app_mutex_give(&app_module_trace_text_mutex);
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t  *source  = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    app_sys_ext_mem_write(ext_mem, source->data_base, trace_text.buffer, sizeof(trace_text));
    #if APP_MODULE_CHECK
    trace_text = app_module_trace_text;
    APP_SYS_LOG_INFO("trace_text.trace_zone:%lu",   trace_text.trace_zone);
    APP_SYS_LOG_INFO("trace_text.offset_head:%lu",  trace_text.offset_head);
    APP_SYS_LOG_INFO("trace_text.offset_tail:%lu",  trace_text.offset_tail);
    #endif
}

/*@brief      日志追踪队列尾加入一段日志
 *@param[in]  text 日志文本
 */
void app_module_trace_text_dump(char text[APP_MODULE_TRACE_LOG_MAX])
{
    app_module_trace_item_t item = {};
    memcpy(item.text, text, APP_MODULE_TRACE_LOG_MAX);
    item.length = strlen(item.text) + 1;
    /* 如果空间不够,则先丢弃式加载多条 */
    while (app_module_trace_text_space() < item.length + sizeof(uint32_t)) {
        app_module_trace_item_t item_temp = {};
        app_module_trace_text_load_one(&item_temp);
    }
    app_module_trace_text_dump_one(&item);
    app_module_trace_text_update();
}

/*@brief      日志追踪队列头取出一段日志
 *@param[out] text 日志文本
 */
bool app_module_trace_text_load(char text[APP_MODULE_TRACE_LOG_MAX])
{
    if (app_module_trace_text_used() == 0)
        return false;
    app_module_trace_item_t item = {};
    app_module_trace_text_load_one(&item);
    memcpy(text, item.text, APP_MODULE_TRACE_LOG_MAX);
    app_module_trace_text_update();
    return true;
}
