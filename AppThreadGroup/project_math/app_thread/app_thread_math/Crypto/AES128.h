#ifndef AES128_H
#define AES128_H

#include <stdint.h>

/* AES128加(Flag:0)解(Flag:1)密 */
void AES128_Run(uint8_t *Text, uint32_t Length, uint8_t Key[16], uint8_t Flag);

#endif
