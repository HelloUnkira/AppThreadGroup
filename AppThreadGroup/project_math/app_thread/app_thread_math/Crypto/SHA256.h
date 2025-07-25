#ifndef SHA256_H
#define SHA256_H

#include <stdint.h>
#include <stdbool.h>

/* SHA256消息摘要 */
void SHA256_Run(uint8_t *Text, uint32_t Length, uint8_t Digest[32]);

#endif
