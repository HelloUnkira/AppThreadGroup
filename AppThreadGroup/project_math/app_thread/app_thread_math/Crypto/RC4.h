#ifndef RC4_H
#define RC4_H

#include <stdint.h>

void RC4Set(uint8_t RC4Key[256], uint8_t *Key,  uint32_t Length);
void RC4Run(uint8_t RC4Key[256], uint8_t *Data, uint32_t Length);

#endif
