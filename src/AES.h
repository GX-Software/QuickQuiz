#ifndef _AES_H_GXSOFTWARE_
#define _AES_H_GXSOFTWARE_

typedef unsigned char		byte;
typedef unsigned long		word;

#define	AESFUNC				__stdcall

#define AES_128

#define AES_OK				0		// ��������
#define AES_FAIL			1		// �����޷����
#define AES_NOEND			2		// ��������ڴ泤�Ȳ��㵼�½��ܲ�����
#define AES_KEYERROR		3		// �趨��������̻����

#ifdef __cplusplus
extern "C" {
#endif
	/*
	�趨���룬�ڼ���ǰ������ô˺���
	��������������Ҫ��ĳ��ȣ��趨ʧ�ܣ�����AES_KEYERROR
	�����Ȳ��㣬���Զ���������Ҫ��ĳ��ȣ���ʽΪ�ظ����е�����
	*/
	int AESFUNC AES_SetKey(byte *key, size_t size);

	/*
	�ṩECB��CBC����ģʽ�ļ��ܺͽ���
	osize�����ѽ��ܳ���
	��osize�����Խ���ȫ�����ݣ��򷵻�AES_NOEND��out����Ȼ����ѽ�������
	in��out����ָ��ͬһ���ڴ棬������ɴ���
	*/
	int AESFUNC AES_Encrypt_ECB(byte *in, size_t isize, byte *out, size_t *osize);
	int AESFUNC AES_Decrypt_ECB(byte *in, size_t isize, byte *out, size_t *osize);
	int AESFUNC AES_Encrypt_CBC(byte *in, size_t isize, byte *out, size_t *osize);
	int AESFUNC AES_Decrypt_CBC(byte *in, size_t isize, byte *out, size_t *osize);

#ifdef __cplusplus
}
#endif

#endif