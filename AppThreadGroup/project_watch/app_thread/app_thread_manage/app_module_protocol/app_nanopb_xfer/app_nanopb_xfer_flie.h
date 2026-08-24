#ifndef APP_NANOPB_XFER_FLIE_H
#define APP_NANOPB_XFER_FLIE_H

#if APP_MODULE_PROTOCOL_USE_NANOPB

/* 文件应答类型:file子消息tag */
#define APP_NANOPB_FILE_ACK_DESCRIPTOR   AppPB_File_descriptor_tag  /* 描述符应答 */
#define APP_NANOPB_FILE_ACK_PACKAGE      AppPB_File_package_tag     /* 分包应答 */
#define APP_NANOPB_FILE_ACK_DONE         AppPB_File_done_tag        /* 结束应答 */

/*@brief 初始化文件传输引擎(注入nanopb原语)
 */
void app_nanopb_xfer_file_init(void);

/*@brief 打包传输OTA升级
 */
void app_nanopb_xfer_notify_ota(void);

/*@brief 传输接收OTA升级
 */
bool app_nanopb_xfer_respond_ota(AppPB_MsgSet *message);

/*@brief 打包传输文件(拉取式:发送描述符后等待对端ack拉取分包)
 *       数据源为日志队列内容(设备上报log文件场景),与trace_text同源;
 *       轮询发送:启动软件定时器,周期轮询ack是否到达,到达后再走下一步
 */
void app_nanopb_xfer_notify_file(void);

/*@brief 文件传输轮询步进
 *       每个轮询周期检查ack是否到达,到达后发送下一分包/结束;超时则退出
 */
void app_nanopb_xfer_file_step(void);

/*@brief 传输接收文件(开始/包/结束)
 *       按状态机收发,完成分包连续性、CRC8、CRC32校验,并对每步回ack
 */
bool app_nanopb_xfer_respond_file(AppPB_MsgSet *message);

/*@brief 传输接收应答注入(文件发送状态机)
 *       收到对端ack后记录,由轮询步进消费(决定重发还是继续)
 */
void app_nanopb_xfer_file_ack(AppPB_ACK *ack);

#endif

#endif
