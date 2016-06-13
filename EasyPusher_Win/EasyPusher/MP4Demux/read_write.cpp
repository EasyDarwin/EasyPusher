// for win32 precompiled header [6/7/2016 SwordTwelve]
#include "StdAfx.h"
#include "read_write.h"
#include "Public_class.h"


unsigned read_uint16_big(FILE *f)
{
	unsigned short k = 0;
	fread(&k, sizeof(k), 1, f);
	return k;
}

uint16_t read_uint16_lit(FILE *f)
{
	uint16_t k;
	fread(&k, sizeof(k), 1, f);
	return ((k&0xff00) >> 8)|((k&0xff) << 8);
}

unsigned char read_uint8(FILE *f)
{
	unsigned char x;
	fread(&x, sizeof(x), 1, f);
	return x;
}

unsigned int read_uint32_lit(FILE *f){
	unsigned char pData = 0;
	unsigned long lValue = 0;
	unsigned int cur=ftell(f);
	fread(&pData, sizeof(pData), 1, f);
	lValue += pData<<24;//*16777216;//24
	fread(&pData, sizeof(pData), 1, f);
	lValue += pData<<16;//*65536;//16
	
	fread(&pData, sizeof(pData), 1, f);
	lValue += pData<<8;//*256;//8
	fread(&pData, sizeof(pData), 1, f);
	lValue += pData;
	return lValue;
}
std::string Read4ByteString(FILE *f)
{
	char strString[5];
	fread(&strString, 4, 1, f);
	strString[4] = '\0';
	std::string strType = strString;
	return strType;
}
void write_uint8(FILE *f,uint8_t x){
	fwrite(&x,sizeof(x),1,f);
}
void write_uint16_big(FILE *f,uint16_t x){
	fwrite(&x,sizeof(x),1,f);
}
void write_uint16_lit(FILE *f,uint16_t x){

}
void write_uint32_lit(FILE *f,uint32_t x){
	char *m =new char[4];  
	m[0] = x >> 24;
	m[1] = ((x >> 16) & 0xff);
	m[2] = ((x >> 8) & 0xff);
	m[3] = x & 0xff;
	fwrite(&m[0],sizeof(m[0]),1,f);
	fwrite(&m[1],sizeof(m[1]),1,f);
	fwrite(&m[2],sizeof(m[2]),1,f);
	fwrite(&m[3],sizeof(m[3]),1,f);
}