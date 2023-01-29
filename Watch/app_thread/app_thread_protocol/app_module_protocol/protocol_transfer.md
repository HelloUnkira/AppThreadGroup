### 协议传输约定

**初始约定,协议包:**

```c
#define PROTOCOL_TYPE_SIZE   10  /* k = log2(协议包命令及事件最大数量) + 1 */
#define PROTOCOL_DATA_SIZE   12  /* k = log2(协议包最大大小) + 1 */

#pragma pack(1)
typedef struct {
    uint64_t head:1;    /* 起始协议包(多包传输开始标记) */
    uint64_t tail:1;    /* 终止协议包(多包传输结束标记) */
    uint64_t offset:    PROTOCOL_DATA_SIZE;  /* 协议包数据偏移(多包传输使用) */
    uint64_t command:   PROTOCOL_TYPE_SIZE;  /* 命令字 */
    uint64_t event:     PROTOCOL_TYPE_SIZE;  /* 事件,关键字 */
    uint64_t size:      PROTOCOL_DATA_SIZE;  /* 协议包数据长度 */
    uint8_t *data;      /* 协议包数据实体 */
} app_module_protocol_package_t;
#pragma pack()
```

| --------- | ------------------------------------------------- |
| 字段      | 定义                                              |
| --------- | ------------------------------------------------- |
| head      | 首包标记,多包传输时拆包组包使用,单包固定为1       |
| tail      | 尾包标记,多包传输时拆包组包使用,单包固定为1       |
| offset    | 当前包数据偏移,多包传输时使用,单包固定为0         |
| command   | 命令,命令字                                       |
| event     | 命令子模式,关键字,包事件表明                      |
| size      | 包携带数据尺寸,为0时为应答包,数据包必须携带数据   |
| data      | 包携带数据,动态生成,传输时与包本体分开传输        |
| --------- | ------------------------------------------------- |

**注意:** 不同的设备依照传输框架以及设备环境使用不同的传输包
**=====** 传输双方是已知且明知对方的传输包格式,多字节字段采用小端序解析(LE模式)
**=====** 接收流程均需要进行超时检查(重发多次),对于超时检查失败则需要终止传输流程
**=====** 下述的package均指protocol_package_t
**=====** 传输时先传输package后传输package.data

```c
package.head = 1;
package.tail = 1;
package.offset = 0;
package.data = ......;
```



**======**

### 设备功能表获取(command:0xD1)
每个功能表数据长度为10,按位计功能配置,共计256个功能的配置项
event 0x00~0x9F: 基础功能表
event 0xA0~0xFF: 扩展功能表
......

### 设备基本信息获取(command:0xD2 event:0x01)

```c
typedef struct {
    uint16_t device_id;     // 设备ID
    uint8_t  firmware_id    // 固件版本号
    uint8_t  hardware_id    // 硬件版本号
    uint8_t  software_id    // 软件版本号
    uint8_t  platform_id    // 平台版本号
    /* 这里可以继续补充其他版本号信息...... */
    /* 例如:各种算法版本号信息,空中升级包信息等等 */
} ptl_dev_info_t;
```

### 设备系统时间获取与设置(command:0xD2 获取event:0x02 设置event:0x03)

```c
typedef struct {
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
    uint8_t  hour;
    uint8_t  minute;
    uint8_t  second;
} ptl_sys_clk_t;
```

### 设备勿扰功能获取与设置(command:0xD2 获取event:0x04 设置event:0x05)

```c
typedef struct {
    uint8_t onoff:1;        // 勿扰功能开关
    uint8_t time_table:1;   // 天范围勿扰表,无范围则默认全天勿扰
    uint8_t week_repeat:7;  // 周轮转,日一二三四五六
    uint8_t table_num;      // 勿扰表条目数量
    struct {
        uint8_t s_hour;     // 勿扰开始时
        uint8_t s_minute;   // 勿扰开始分
        uint8_t s_second;   // 勿扰开始秒
        uint8_t e_hour;     // 勿扰结束时
        uint8_t e_minute;   // 勿扰结束分
        uint8_t e_second;   // 勿扰结束秒
    } table_item[0];
} ptl_dev_disturb_t;
```



### 文件传输命令(command:0xFF)
| ----- | -------------- |
| event | 定义           |
| ----- | -------------- |
| 0x01  | 设置传输目标   |
| 0x02  | 获取传输目标   |
| 0x03  | 传输文件片段   |
| 0x04  | 主动放弃传输   |
| ----- | -------------- |

```c
typedef struct {
    uint8_t  type;          // 文件传输类型
    uint8_t  suffix[10];    // 文件后缀名,文件类型
    uint8_t  prefix[256];   // 文件前缀名,文件名字
    uint32_t size;          // 文件大小
    uint32_t crc32;         // 文件校验
    uint32_t checksum32;    // 文件校验
    uint32_t offset;        // 文件断点续传偏移,初始为0
    uint32_t error_code;    // 文件传输过程中生成的错误码
} ptl_file_t;
```

| ----- | -------------- |
| type  | 定义           |
| ----- | -------------- |
| 0x01  | 日志文件       |
| ----- | -------------- |

| ---------- | --------------------------- |
| error_code | 定义                        |
| ---------- | --------------------------- |
| 0x00       | 文件传输流程,就绪或结束     |
| 0x01       | 校验失败,当次传输丢弃       |
| 0x02       | 文件传输中,不可进行新流程   |
| 0x03       | 传输等待超时,当次传输丢弃   |
| 0x04       | 其他位置错误,当次传输丢弃   |
| ---------- | --------------------------- |
