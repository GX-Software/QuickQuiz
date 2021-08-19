#ifndef _AES_H_GXSOFTWARE_
#define _AES_H_GXSOFTWARE_

typedef unsigned char		byte;
typedef unsigned long		word;

#define	AESFUNC				__stdcall

#define AES_128

#define AES_OK				0		// 解密无误
#define AES_FAIL			1		// 解密无法完成
#define AES_NOEND			2		// 由于输出内存长度不足导致解密不完整
#define AES_KEYERROR		3		// 设定的密码过短或过长

#ifdef __cplusplus
extern "C" {
#endif
	/*
	设定密码，在加密前必须调用此函数
	长度若超过加密要求的长度，设定失败，返回AES_KEYERROR
	若长度不足，则自动补至加密要求的长度，方式为重复已有的密码
	*/
	int AESFUNC AES_SetKey(byte *key, size_t size);

	/*
	提供ECB和CBC两种模式的加密和解密
	osize保存已解密长度
	若osize不足以解密全部内容，则返回AES_NOEND，out中仍然存放已解密内容
	in和out可以指向同一块内存，不会造成错误
	*/
	int AESFUNC AES_Encrypt_ECB(byte *in, size_t isize, byte *out, size_t *osize);
	int AESFUNC AES_Decrypt_ECB(byte *in, size_t isize, byte *out, size_t *osize);
	int AESFUNC AES_Encrypt_CBC(byte *in, size_t isize, byte *out, size_t *osize);
	int AESFUNC AES_Decrypt_CBC(byte *in, size_t isize, byte *out, size_t *osize);

#ifdef __cplusplus
}
#endif

#endif