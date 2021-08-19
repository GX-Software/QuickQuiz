#include "StdAfx.h"
#include "Base64.h"

#define BASE64_FALSE		0
#define BASE64_TRUE			1

static const char base64_alphabet[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
	'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'+', '/'
};

static const int base64_realphabet[] = {
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1,  0, -1, -1,
	-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
	-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};

static size_t __stdcall CalcBase64Size(size_t in_size)
{
	size_t r = in_size / 3 * 4;
	if (in_size % 3)
	{
		r += 4;
	}

	return r;
}

/*
计算解码后的二进制长度
由于Base64码内部可能有错误，为了避免内存溢出，故不对内容进行判断
直接计算解码后的最大可能长度，解码过程中再计算实际长度
*/
static size_t __stdcall CalcBinSize(char *in, size_t in_size)
{
	return ((in_size + 3) / 4 * 3);
}

static char __stdcall SwitchToChar(int index, unsigned char c1, unsigned char c2)
{
	switch(index)
	{
	case 0:
		return base64_alphabet[c1 >> 2];

	case 1:
		return base64_alphabet[((c1 & 0x03) << 4) | (c2 >> 4)];

	case 2:
		return base64_alphabet[((c1 & 0x0F) << 2) | (c2 >> 6)];

	case 3:
		return base64_alphabet[c1 & 0x3F];

	default:
		return 0;
	}
}

static int __stdcall SwitchToBin(int index, char c1, char c2)
{
	int a = base64_realphabet[c1], b = base64_realphabet[c2];
	int r = 0;

	if (a < 0 || b < 0)
	{
		return -1;
	}

	switch(index)
	{
	case 0:
		r = (a << 2) | ((b & 0x30) >> 4);
		break;

	case 1:
		r = (a << 4) | ((b & 0x3C) >> 2);
		break;

	case 2:
		r = (a << 6) | (b & 0x3F);
		break;
	}

	return (r & 0xFF);
}

int __stdcall Base64ToBin(char *in,
						  size_t in_size,
						  unsigned char *out,
						  size_t *out_size)
{
	size_t i = 0, o = 0, e = 0;
	int r;

	if (!out)
	{
		*out_size = CalcBinSize(in, in_size);
		return BASE64_TRUE;
	}

	while(o < *out_size && i < in_size)
	{
		switch(in[i])
		{
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			++i;
			continue;

		case '=':
			break;
		}

		if (i + 1 == in_size)
		{
			r = SwitchToBin(o % 3, in[i], '=');
		}
		else
		{
			r = SwitchToBin(o % 3, in[i], in[i + 1]);
		}

		// 不可识别的字符
		if (r < 0)
		{
			*out_size = o;
			return BASE64_FALSE;
		}
		out[o] = (unsigned char)r;

		++i;
		++o;
		if (!(o % 3))
		{
			++i;
		}
	}

	// 计算不足3字节的内容
	if (i >= in_size)
	{
		i = in_size - 1;
	}
	while(1)
	{
		if (in[i] == '=')
		{
			++e;
			if (!i)
			{
				break;
			}
			--i;
		}
		else
		{
			break;
		}
	}
	*out_size = o - e;
	return BASE64_TRUE;
}

int __stdcall BinToBase64(unsigned char *in,
						  size_t in_size,
						  char *out,
						  size_t *out_size)
{
	size_t i = 0, o = 0;
	
	if (!out)
	{
		*out_size = CalcBase64Size(in_size);
		return BASE64_TRUE;
	}
	
	while(o < *out_size)
	{
		if (i + 1 == in_size)
		{
			out[o] = SwitchToChar(o % 4, in[i], 0);
			if (o % 4)
			{
				++o;
				break;
			}
		}
		else
		{
			out[o] = SwitchToChar(o % 4, in[i], in[i + 1]);
		}
		
		switch(o % 4)
		{
		case 1:
		case 2:
		case 3:
			++i;
			break;
		}
		++o;
	}
	
	while(o < *out_size && (o % 4))
	{
		out[o++] = '=';
	}
	*out_size = o;
	return BASE64_TRUE;
}