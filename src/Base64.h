#ifndef _BASE_64_H_
#define _BASE_64_H_

#include <stdio.h>

#define BASE64_FALSE		0
#define BASE64_TRUE			1

#ifdef __cplusplus
extern "C" {
#endif

/*
out_size需要输入out的大小
当*out为空时，返回所需的空间大小，按字节计算，不包含末尾0
*/
extern int __stdcall Base64ToBin(char *in, size_t in_size, unsigned char *out, size_t *out_size);
extern int __stdcall BinToBase64(unsigned char *in, size_t in_size, char *out, size_t *out_size);

#ifdef __cplusplus
};
#endif

#endif