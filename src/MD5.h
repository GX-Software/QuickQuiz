#ifndef MD5_H  
#define MD5_H  

#define MD5FUNC		__stdcall

void MD5FUNC MakeMD5(unsigned char* inputBytes, int byteLen, unsigned char digest[16]);
void MD5FUNC MakeMD5Hex(unsigned char digest[16], char* hexStr);

#endif