#ifndef READ_WRITE_H
#define READ_WRITE_H

#include "inttype.h"
#include <iostream>


unsigned read_uint16_big(FILE *f);
uint16_t read_uint16_lit(FILE *f);
unsigned char read_uint8(FILE *f);
unsigned int read_uint32_lit(FILE *f);
std::string Read4ByteString(FILE *f);
void write_uint8(FILE *f,uint8_t x);
void write_uint16_big(FILE *f,uint16_t x);
void write_uint16_lit(FILE *f,uint16_t x);
void write_uint32_lit(FILE *f,uint32_t x);

#endif
