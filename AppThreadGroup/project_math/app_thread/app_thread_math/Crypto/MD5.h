#ifndef MD5_H
#define MD5_H

#include <stdint.h>
#include <stdbool.h>

/* MD5消息摘要 */
void MD5_Run(uint8_t *Text, uint32_t Length, uint8_t Digest[16]);

#endif
