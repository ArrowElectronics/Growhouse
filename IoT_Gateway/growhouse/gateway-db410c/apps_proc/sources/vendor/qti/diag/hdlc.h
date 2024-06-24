#ifndef __HDLC_H__
#define __HDLC_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
uint16_t hdlc_crc_byte(uint16_t crc, uint8_t ch);

uint8_t *hdlc_encode(uint8_t *src, size_t slen, size_t *dlen);

uint8_t *hdlc_decode_one(uint8_t **buf, size_t *len, size_t *msglen);
#endif
