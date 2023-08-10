/*实现目标:
 *    转存到外存的追踪日志信息
 */

#define APP_SYS_LOG_RECORD_LIMIT     1
#define APP_SYS_LOG_LOCAL_STATUS     1
#define APP_SYS_LOG_LOCAL_LEVEL      2   /* 0:DEBUG,1:INFO,2:WARN,3:ERROR,4:NONE */

#include "app_ext_lib.h"
#include "app_sys_log.h"
#include "app_sys_log_text.h"
#include "app_sys_ext_mem.h"
#include "app_sys_ext_mem_table.h"
#include "app_sys_ext_src.h"
#include "app_sys_ext_src_table.h"
#include "app_sys_util.h"

static app_mutex_t app_sys_log_text_mutex = {0};
static app_sys_log_text_t app_sys_log_text;
static const uintptr_t app_sys_log_text_size = sizeof(app_sys_log_text);
static const uintptr_t app_sys_log_info_size = sizeof(app_sys_log_text.info);

/*@brief 加载一条日志(无锁,本地使用)
 *@param[in]  offset 偏移值
 *@param[in]  zone   日志总空间
 *@param[out] item   日志条目
 *@param[out] offset_new 更新后的偏移值
 *@retval     日志信息紊乱,需复位
 */
static bool app_sys_log_text_load_one(app_sys_log_item_t *item, uintptr_t offset, uintptr_t zone, uintptr_t *offset_new)
{
    offset %= zone;
    /* 条目是一条条更新的,所以只要对其进行操作就一定要锁定整个动作 */
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t *ext_src = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    uintptr_t base = app_sys_log_text_size + ext_src->data_base;
    /* 因为,我们需要考虑读写跨越了头尾,所以逻辑需要额外复杂点 */
    if (offset + sizeof(uintptr_t) < zone) {
        if (sizeof(uintptr_t) != 0)
        app_sys_ext_mem_read(ext_mem, base + offset, item->buffer, sizeof(uintptr_t));
    } else {
        if (zone - offset != 0)
        app_sys_ext_mem_read(ext_mem, base + offset, item->buffer, zone - offset);
        if (sizeof(uintptr_t) - (zone - offset) != 0)
        app_sys_ext_mem_read(ext_mem, base, item->buffer + zone - offset, sizeof(uintptr_t) - (zone - offset));
    }
    offset += sizeof(uintptr_t);
    offset %= zone;
    /* 日志数据紊乱,清除并投递警告 */
    if (item->length > APP_SYS_LOG_TEXT_MAX + 1) {
        APP_SYS_LOG_ERROR("trace log data invalid");
        return false;
    }
    /* 因为,我们需要考虑读写跨越了头尾,所以逻辑需要额外复杂点 */
    if (offset + item->length < zone) {
        if (item->length != 0)
        app_sys_ext_mem_read(ext_mem, base + offset, item->text, item->length);
    } else {
        if (zone - offset != 0)
        app_sys_ext_mem_read(ext_mem, base + offset, item->text, zone - offset);
        if (item->length - (zone - offset) != 0)
        app_sys_ext_mem_read(ext_mem, base, item->text + zone - offset, item->length - (zone - offset));
    }
    offset += item->length;
    offset %= zone;
    /*  */
    APP_SYS_LOG_INFO("item.length:%u", item->length);
    APP_SYS_LOG_INFO("item.text:%s",   item->text);
    
    *offset_new = offset;
    return true;
}

/*@brief 转储一条日志(无锁,本地使用)
 *@param[in]  offset 偏移值
 *@param[in]  zone   日志总空间
 *@param[in]  item   日志条目
 *@param[out] offset_new 更新后的偏移值
 *@retval     日志信息紊乱,需复位
 */
static bool app_sys_log_text_dump_one(app_sys_log_item_t *item, uintptr_t offset, uintptr_t zone, uintptr_t *offset_new)
{
    offset %= zone;
    /* 条目是一条条更新的,所以只要对其进行操作就一定要锁定整个动作 */
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t *ext_src = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    uintptr_t base = app_sys_log_text_size + ext_src->data_base;
    /* 因为,我们需要考虑读写跨越了头尾,所以逻辑需要额外复杂点 */
    if (offset + sizeof(uintptr_t) < zone) {
        if (sizeof(uintptr_t) != 0)
        app_sys_ext_mem_write(ext_mem, base + offset, item->buffer, sizeof(uintptr_t));
    } else {
        if (zone - offset != 0)
        app_sys_ext_mem_write(ext_mem, base + offset, item->buffer, zone - offset);
        if (sizeof(uintptr_t) - (zone - offset) != 0)
        app_sys_ext_mem_write(ext_mem, base, item->buffer + zone - offset, sizeof(uintptr_t) - (zone - offset));
    }
    offset += sizeof(uintptr_t);
    offset %= zone;
    /* 日志数据紊乱,清除并投递警告 */
    if (item->length > APP_SYS_LOG_TEXT_MAX + 1) {
        APP_SYS_LOG_ERROR("trace log data invalid");
        return false;
    }
    /* 因为,我们需要考虑读写跨越了头尾,所以逻辑需要额外复杂点 */
    if (offset + item->length < zone) {
        if (sizeof(uintptr_t) != 0)
        app_sys_ext_mem_write(ext_mem, base + offset, item->text, item->length);
    } else {
        if (zone - offset != 0)
        app_sys_ext_mem_write(ext_mem, base + offset, item->text, zone - offset);
        if (item->length - (zone - offset) != 0)
        app_sys_ext_mem_write(ext_mem, base, item->text + zone - offset, item->length - (zone - offset));
    }
    offset += item->length;
    offset %= zone;
    /*  */
    APP_SYS_LOG_INFO("item.length:%u", item->length);
    APP_SYS_LOG_INFO("item.text:%s",   item->text);
    
    *offset_new = offset;
    return true;
}

/*@brief 已使用空间
 */
static uintptr_t app_sys_log_text_used(void)
{
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    uintptr_t retval = app_sys_log_text.info.tail - app_sys_log_text.info.head;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    return retval;
}

/*@brief 未使用空间
 */
static uintptr_t app_sys_log_text_space(void)
{
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    uintptr_t retval = app_sys_log_text.info.zone - app_sys_log_text.info.tail + app_sys_log_text.info.head;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    return retval;
}

/*@brief 记录点索引回退
 */
static void app_sys_log_text_rewind(void)
{
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    uintptr_t zone = app_sys_log_text.info.zone;
    uintptr_t head = app_sys_log_text.info.head;
    uintptr_t tail = app_sys_log_text.info.tail;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    /* 环形索引记录回退 */
    bool not_update = true;
    uintptr_t rewind_index = (uintptr_t)1 << (sizeof(uintptr_t) * 8 - 1);
    if (head >= rewind_index ||
        tail >= rewind_index) {
        rewind_index /= zone;
        rewind_index *= zone;
        head -= rewind_index;
        tail -= rewind_index;
        not_update = false;
    }
    if (not_update)
        return;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    app_sys_log_text.info.head = head;
    app_sys_log_text.info.tail = tail;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
}

/*@brief 更新头部资源
 */
static void app_sys_log_text_reflush(void)
{
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t *ext_src = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    /* 记录点索引回退 */
    app_sys_log_text_rewind();
    /* 更新校验 */
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    app_sys_log_text.crc32 = app_sys_crc32(app_sys_log_text.buffer, app_sys_log_info_size);
    app_sys_log_text_t trace_text = app_sys_log_text;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    /* 回写日志追踪队列结构以刷新外存 */
    app_sys_ext_mem_write(ext_mem, ext_src->data_base, trace_text.buffer, app_sys_log_text_size);
    /*  */
    APP_SYS_LOG_INFO("trace_text.zone:%lu", trace_text.info.zone);
    APP_SYS_LOG_INFO("trace_text.head:%lu", trace_text.info.head);
    APP_SYS_LOG_INFO("trace_text.tail:%lu", trace_text.info.tail);
    APP_SYS_LOG_INFO("trace_text.peek:%lu", trace_text.info.peek);
}

/*@brief 日志追踪队列复位
 */
void app_sys_log_text_reset(void)
{
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t *ext_src = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    APP_SYS_ASSERT(app_sys_log_text_size < ext_src->data_size);
    /* 重置日志追踪队列结构 */
    app_sys_log_text_t trace_text;
    trace_text.info.head  = 0;
    trace_text.info.tail  = 0;
    trace_text.info.peek  = 0;
    trace_text.info.zone  = ext_src->data_size - app_sys_log_text_size;
    trace_text.crc32 = app_sys_crc32(trace_text.buffer, app_sys_log_info_size);
    /* 更新日志追踪队列结构 */
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    app_sys_log_text = trace_text;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    /* 回写日志追踪队列结构以刷新外存 */
    app_sys_log_text_reflush();
}

/*@brief 日志追踪队列初始化
 */
void app_sys_log_text_ready(void)
{
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_static);
    const app_sys_ext_mem_t *ext_mem = app_sys_ext_mem_find_by_name("mix_chunk_small");
    const app_sys_ext_src_t *ext_src = app_sys_ext_src_find_by_name("mix_chunk_small", "trace log text");
    APP_SYS_ASSERT(app_sys_log_text_size < ext_src->data_size);
    /* 读取外存的函数追踪队列结构 */
    app_sys_log_text_t trace_text;
    app_sys_ext_mem_read(ext_mem, ext_src->data_base, trace_text.buffer, app_sys_log_text_size);
    uint32_t crc32 = app_sys_crc32(trace_text.buffer, app_sys_log_info_size);
    /* 更新日志追踪队列结构 */
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    app_sys_log_text = trace_text;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    /*  */
    APP_SYS_LOG_INFO("trace_text.zone:%lu", trace_text.info.zone);
    APP_SYS_LOG_INFO("trace_text.head:%lu", trace_text.info.head);
    APP_SYS_LOG_INFO("trace_text.tail:%lu", trace_text.info.tail);
    APP_SYS_LOG_INFO("trace_text.peek:%lu", trace_text.info.peek);
    /* 错误的信息需要对其进行复位 */
    if (trace_text.crc32 != crc32 || trace_text.info.zone != ext_src->data_size - app_sys_log_text_size)
        app_sys_log_text_reset();
}

/*@brief      日志追踪队列尾转入一段日志
 *@param[in]  text       日志文本
 *@param[in]  need_cover 覆盖式加入日志
 *                       如果空间不足则会不断丢弃最旧的数据
 *                       直到空间完全释放也不能加入
 *                       或者可以加入新条目为止
 *@retval     成功或者失败
 */
bool app_sys_log_text_dump(char text[APP_SYS_LOG_TEXT_MAX], bool need_cover)
{
    bool retval = false;
    app_sys_log_item_t trace_item;
    memcpy(trace_item.text, text, APP_SYS_LOG_TEXT_MAX);
    trace_item.text[APP_SYS_LOG_TEXT_MAX] = '\0';
    trace_item.length = strlen(trace_item.text) + 1;
    /* 如果空间不够,则先丢弃式加载多条 */
    while (app_sys_log_text_space() < trace_item.length + sizeof(uint32_t)) {
        /* 如果是非覆盖式转储,禁止丢弃先前数据 */
        if (!need_cover) {
            APP_SYS_LOG_WARN("abort dump. discard previous data is forbidden by user");
            return false;
        }
        /* 如果完全为空,说明空间太小了 */
        if (app_sys_log_text_used() == 0) {
            APP_SYS_LOG_WARN("extern memory zone is too small");
            return false;
        }
        /* 加载一个条目,然后丢弃它 */
        app_sys_log_item_t trace_item_temp;
        app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
        uintptr_t zone = app_sys_log_text.info.zone;
        uintptr_t head = app_sys_log_text.info.head % zone, head_new = 0;
        retval = app_sys_log_text_load_one(&trace_item_temp, head, zone, &head_new);
        app_sys_log_text.info.head += (head_new - head) % zone;
        app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
        /* 注意返回值,日志信息紊乱要终止 */
        if (!retval) {
            app_sys_log_text_reset();
            APP_SYS_LOG_ERROR("data is disorder, need reset now");
            return false;
        }
        APP_SYS_LOG_INFO("discard previous data:%s", trace_item_temp.text);
    }
    /* 现在空间已经足够,转储新条目 */
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    uintptr_t zone = app_sys_log_text.info.zone;
    uintptr_t tail = app_sys_log_text.info.tail % zone, tail_new = 0;
    retval = app_sys_log_text_dump_one(&trace_item, tail, zone, &tail_new);
    app_sys_log_text.info.tail += (tail_new - tail) % zone;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    /* 注意返回值,日志信息紊乱要终止 */
    if (!retval) {
        app_sys_log_text_reset();
        APP_SYS_LOG_ERROR("data is disorder, need reset now");
        return false;
    }
    app_sys_log_text_reflush();
    return true;
}

/*@brief      日志追踪队列头转出一段日志
 *@param[out] text 日志文本
 *@retval     成功或者失败
 */
bool app_sys_log_text_load(char text[APP_SYS_LOG_TEXT_MAX])
{
    if (app_sys_log_text_used() == 0)
        return false;
    /* 加载一个条目 */
    bool retval = false;
    app_sys_log_item_t trace_item;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    uintptr_t zone = app_sys_log_text.info.zone;
    uintptr_t head = app_sys_log_text.info.head % zone, head_new = 0;
    retval = app_sys_log_text_load_one(&trace_item, head, zone, &head_new);
    app_sys_log_text.info.head += (head_new - head) % zone;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    /* 注意返回值,日志信息紊乱要终止 */
    if (!retval) {
        app_sys_log_text_reset();
        APP_SYS_LOG_ERROR("data is disorder, need reset now");
        return false;
    }
    memcpy(text, trace_item.text, APP_SYS_LOG_TEXT_MAX);
    app_sys_log_text_reflush();
    return true;
}

/*@brief      日志追踪队列窥探一段日志(自动迭代到下一段日志)
 *@param[out] text 日志文本
 *@retval     成功或者失败
 */
bool app_sys_log_text_peek(char text[APP_SYS_LOG_TEXT_MAX])
{
    if (app_sys_log_text_used() == 0)
        return false;
    bool retval = false;
    /* 未知之前的load和dump对peek造成多少影响,这里要做容错处理 */
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    /* 如果peek落在无效数据区域,将其调整回开头 */
    /* 因为它的进动也会是到开头,且我们并不知道什么时候才算是合理的到达末尾 */
    if (app_sys_log_text.info.peek < app_sys_log_text.info.head ||
        app_sys_log_text.info.peek > app_sys_log_text.info.tail)
        app_sys_log_text.info.peek = app_sys_log_text.info.head;
    /* 当到达了末尾,我们此次摘取一个空日志条目,并更新peek */
    if (app_sys_log_text.info.peek == app_sys_log_text.info.tail) {
        app_sys_log_text.info.peek  = app_sys_log_text.info.head;
        text[0] = '\0';
        retval = true;
    }
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    /* 当到达了末尾,我们此次摘取一个空日志条目 */
    if (retval)
        return true;
    /* 加载一个条目 */
    app_sys_log_item_t trace_item;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    uintptr_t zone = app_sys_log_text.info.zone;
    uintptr_t peek = app_sys_log_text.info.peek % zone, peek_new = 0;
    retval = app_sys_log_text_load_one(&trace_item, peek, zone, &peek_new);
    app_sys_log_text.info.peek += (peek_new - peek) % zone;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
    /* 注意返回值,日志信息紊乱要终止 */
    if (!retval) {
        app_sys_log_text_reset();
        APP_SYS_LOG_ERROR("data is disorder, need reset now");
        return false;
    }
    memcpy(text, trace_item.text, APP_SYS_LOG_TEXT_MAX);
    app_sys_log_text_reflush();
    return true;
}

/*@brief 日志追踪队列窥探指针重置
 */
void app_sys_log_text_peek_reset(void)
{
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_take);
    app_sys_log_text.info.peek = app_sys_log_text.info.head;
    app_mutex_process(&app_sys_log_text_mutex, app_mutex_give);
}

/*@brief     日志持久化转接口
 *           这只是一个转接口,外界不可使用
 *@param[in] text 日志文本
 */
void app_sys_log_text_persistent(char *text)
{
    app_sys_log_text_dump(text, true);
}

/*@brief 日志追踪队列转储和加载测试
 *       此测试模式只能单独使用
 */
void app_sys_log_text_test(void)
{
    /* 复位日志追踪队列 */
    app_sys_log_text_reset();
    
    /* 改一下这里即可 */
    uint32_t time_s = 5;        // s
    uint32_t peroid = 1000;     // us
    uint32_t target = 1000 * 1000 / peroid;  //1s执行次数(无需修改)
    
    /* 测试(注意:I/O可能被占满,导致时间不准) */
    for (uint32_t count = 0; count < time_s * target; count++) {
        
        if (count % target == 0)
            APP_SYS_LOG_WARN("log test count:%d", count);
        
        static uint32_t offset = 0;
        uint8_t log_text_i[APP_SYS_LOG_TEXT_MAX * 2] = {0};
        uint8_t log_text_o[APP_SYS_LOG_TEXT_MAX * 2] = {0};
        
        for (uint32_t idx = 0; idx < APP_SYS_LOG_TEXT_MAX; idx++)
            log_text_i[idx] = log_text_i[idx + APP_SYS_LOG_TEXT_MAX] = '0' + idx % 10;
        
        app_sys_log_text_dump(log_text_i + offset, true);
        app_sys_log_text_load(log_text_o);
        
        if (memcmp(log_text_o, log_text_i + offset, APP_SYS_LOG_TEXT_MAX) != 0)
            APP_SYS_LOG_ERROR("log error");
        
        offset += 1;
        offset %= APP_SYS_LOG_TEXT_MAX;
        
        app_delay_us(peroid);
    }
}