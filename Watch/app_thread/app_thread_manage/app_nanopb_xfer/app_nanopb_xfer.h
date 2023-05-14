#ifndef APP_NANOPB_XFER_H
#define APP_NANOPB_XFER_H

/* 创建nanopb encode流程函数 */
#define APP_NANOPB_ENCODE(msg_type_field, msg_type, msg_name)       \
static inline uint32_t app_nanopb_encode_##msg_type_field##(msg_type *msg_name, uint8_t *buffer, uint32_t size)     \
{                                                                   \
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, size);     \
    if (!pb_encode(&stream, msg_type_field, msg_name))              \
         APP_SYS_LOG_ERROR("encode fail:%s", stream.errmsg);        \
    return stream.bytes_written;                                    \
}

/* 创建nanopb decode流程函数 */
#define APP_NANOPB_DECODE(msg_type_field, msg_type, msg_name)       \
static inline uint32_t app_nanopb_decode_##msg_type_field##(msg_type *msg_name, uint8_t *buffer, uint32_t size)     \
{                                                                   \
    pb_istream_t stream = pb_istream_from_buffer(buffer, size);     \
    if (!pb_decode(&stream, msg_type_field, msg_name))              \
         APP_SYS_LOG_ERROR("decode fail:%s", stream.errmsg);        \
    return stream.bytes_left;                                       \
}

#endif
