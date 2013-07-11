#ifndef BOBHASH_H
#define BOBHASH_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef WIN32
typedef  unsigned long  long uint64_t;   /* unsigned 8-byte quantities */
typedef  unsigned long  int  uint32_t;   /* unsigned 4-byte quantities */
typedef  unsigned       char uint8_t;
typedef  unsigned       short uint16_t;
#else 
#include <stdint.h>
#endif
uint32_t bob_hash(const char* str);
#endif
