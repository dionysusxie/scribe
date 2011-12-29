// Base64.cpp: implementation of the CBase64 class.
//
//////////////////////////////////////////////////////////////////////

#include "Base64.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


// Base64.cpp: implementation of the Base64 class.
//
//
// 用途：Base64的编码与解码
// Modified by: Tonydeng
// date：2011-03-29
//////////////////////////////////////////////////////////////////////
#include "Base64.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>




//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBase64::CBase64()
{
	
}

CBase64::~CBase64()
{

}


//////////////////////////////////////////////////////////////////////////
// 函数:    DWORD CalcANSItoBase64Len()
// 功能:    计算ANSI字符串转成Base64字符串需要多少内存
// 参数:    dwANSILen ANSI字符串的长度
// 返回值:  DWORD Base64字符串的长度
//////////////////////////////////////////////////////////////////////////
unsigned long CBase64::CalcANSItoBase64Len(unsigned long dwANSILen)
{
	return (dwANSILen%3) ? (dwANSILen+3)/3*4 : dwANSILen/3*4;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    DWORD CalcBase64toANSILen()
// 功能:    计算Base64字符串转成ANSI字符串需要多少内存
// 参数:    dwANSILen Base64字符串的长度
//   strBase64End2 Base64字符串结尾的二个字符串
// 返回值:  DWORD ANSI字符串的长度
//////////////////////////////////////////////////////////////////////////
unsigned long CBase64::CalcBase64toANSILen(unsigned long dwBase64Len, const string strBase64End2)
{
 //计算'='出现的次数，
 int count = 0;
 int i;
 int length = strBase64End2.length();

 for ( i=0; i< length; i++ )
 {
	if('=' == strBase64End2[i])
	{
	  count ++;
	}
 }

 unsigned long dwANSILen = (dwBase64Len%4) ? (dwBase64Len+4)/4*3 : dwBase64Len/4*3;
 dwANSILen -= count;
 return dwANSILen;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    PSTR AllocMemBase64()
// 功能:    分配Base64字符串所需要的空间，这个内存需要用户手动删除
// 参数:    dwANSILen ANSI字符串的长度
// 返回值:  PSTR Base64内存地址
//////////////////////////////////////////////////////////////////////////
char* CBase64::AllocMemBase64(unsigned long dwANSILen)
{
 int len = CBase64::CalcANSItoBase64Len(dwANSILen);
 char* pBase64 = new char[len+1];
 memset(pBase64, 0, len+1);
 
 return pBase64;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    PSTR AllocMemANSI()
// 功能:    分配Base64字符串所需要的空间，这个内存需要用户手动删除
// 参数:    dwANSILen ANSI字符串的长度
// 返回值:  PSTR Base64内存地址
//////////////////////////////////////////////////////////////////////////
char* CBase64::AllocMemANSI(unsigned long dwBase64Len)
{
 int len = CBase64::CalcBase64toANSILen(dwBase64Len);
 char* pANSI = new char[len+1];
 memset(pANSI, 0, len+1);
 
 return pANSI;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    void FreeMemBase64()
// 功能:    删除用AllocMemBase64分配的内存
// 参数:    pBase64 分配内在的地址
// 返回值:  void
//////////////////////////////////////////////////////////////////////////
void CBase64::FreeMemBase64(char* pBase64)
{
 assert(pBase64);
 delete[] pBase64;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    void FreeMemANSI()
// 功能:    删除用AllocMemANSI分配的内存
// 参数:    pANSI 分配内在的地址
// 返回值:  void
//////////////////////////////////////////////////////////////////////////
void CBase64::FreeMemANSI(char* pANSI)
{
	assert(pANSI);
	delete[] pANSI;
}





// 用于编码的字符
const string CBase64::_base64_encode_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// 用于解码的字符
const char CBase64::_base64_decode_chars[] = 
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};


//////////////////////////////////////////////////////////////////////////
// 函数:    CString encode()
// 功能:    将ANSI字符串转成Base64字符串
// 参数:    in_str ANSI字符串
// 返回值:  CString Base64字符串
// 日期:    [6/23/2005]
//////////////////////////////////////////////////////////////////////////
string CBase64::encode(const string in_str)
{
	string out_str;
    unsigned char c1, c2, c3;
    int i = 0;
    int len = in_str.length();

    while ( i<len )
    {
        // read the first byte
        c1 = in_str[i++];
        if ( i==len )       // pad with "="
        {
            out_str += _base64_encode_chars[ c1>>2 ];
            out_str += _base64_encode_chars[ (c1&0x3)<<4 ];
            out_str += "==";
            break;
        }

        // read the second byte
        c2 = in_str[i++];
        if ( i==len )       // pad with "="
        {
            out_str += _base64_encode_chars[ c1>>2 ];
            out_str += _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
            out_str += _base64_encode_chars[ (c2&0xF)<<2 ];
            out_str += "=";
            break;
        }

        // read the third byte
        c3 = in_str[i++];
        // convert into four bytes string
        out_str += _base64_encode_chars[ c1>>2 ];
        out_str += _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
        out_str += _base64_encode_chars[ ((c2&0xF)<<2) | ((c3&0xC0)>>6) ];
        out_str += _base64_encode_chars[ c3&0x3F ];
    }

    return out_str;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    CString decode()
// 功能:    将Base64字符串转成ANSI字符串
// 参数:    in_str Base64字符串
// 返回值:  CString ANSI字符串
// 日期:    [6/23/2005]
//////////////////////////////////////////////////////////////////////////
string CBase64::decode(const string in_str)
{
	string out_str;
    char c1, c2, c3, c4;
    int i = 0;
    int len = in_str.length();
 
    while ( i<len)
    {
        // read the first byte
        do
        {
            c1 = _base64_decode_chars[ (int)in_str[i++] ];
        } while ( i<len && c1==-1);

        if ( c1==-1)
        {
            break;
        }

        // read the second byte
        do
        {
            c2 = _base64_decode_chars[ (int)in_str[i++] ];
        } while ( i<len && c2==-1);

        if ( c2==-1 )
        {
            break;
        }

        // assamble the first byte
        out_str += char( (c1<<2) | ((c2&0x30)>>4) );

        // read the third byte
        do
        {
            c3 = in_str[i++];
            if ( c3==61 )       // meet with "=", break
                return out_str;
            c3 = _base64_decode_chars[ (int)c3 ];
        } while ( i<len && c3==-1);

        if ( c3==-1 )
        {
            break;
        }

        // assamble the second byte
        out_str += char( ((c2&0XF)<<4) | ((c3&0x3C)>>2) );

        // read the fourth byte
        do
        {
            c4 = in_str[i++];
            if ( c4==61 )       // meet with "=", break
                return out_str;

            c4 = _base64_decode_chars[ (int)c4 ];
        } while ( i<len && c4==-1 );

        if ( c4==-1 )
        {
            break;
        }

        // assamble the third byte
        out_str += char( ((c3&0x03)<<6) | c4 );
    }

    return out_str;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    void encode()
// 功能:    将ANSI字符串转成Base64字符串
// 参数:    pIn  ANSI字符串
//   dwInLen ANSI字符串的长度
//   pOut 放Base64字符串的内存
//   pdwOutLen Base64字符串的长度
// 返回值:  void
// 日期:    [6/24/2005]
//////////////////////////////////////////////////////////////////////////
void CBase64::encode(char* pIn, unsigned long dwInLen, char* pOut, unsigned long* pdwOutLen)
{
    unsigned char c1, c2, c3;
    int i = 0, n = 0;
    int len = dwInLen;

    while ( i<len )
    {
        // read the first byte
        c1 = pIn[i++];
        if ( i==len )       // pad with "="
        {
            pOut[n++] = _base64_encode_chars[ c1>>2 ];
            pOut[n++] = _base64_encode_chars[ (c1&0x3)<<4 ];
            pOut[n++] = '=';
            pOut[n++] = '=';
           break;
        }

        // read the second byte
        c2 = pIn[i++];
        if ( i==len )       // pad with "="
        {
            pOut[n++] = _base64_encode_chars[ c1>>2 ];
            pOut[n++] = _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
            pOut[n++] = _base64_encode_chars[ (c2&0xF)<<2 ];
            pOut[n++] = '=';
            break;
        }

        // read the third byte
        c3 = pIn[i++];
        // convert into four bytes string
        pOut[n++] = _base64_encode_chars[ c1>>2 ];
        pOut[n++] = _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
        pOut[n++] = _base64_encode_chars[ ((c2&0xF)<<2) | ((c3&0xC0)>>6) ];
        pOut[n++] = _base64_encode_chars[ c3&0x3F ];
    }

    *pdwOutLen = n;
    return;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    void decode()
// 功能:    将Base64字符串转成ANSI字符串
// 参数:    pIn  Base64字符串
//   dwInLen Base64字符串的长度
//   pOut 放ANSI字符串的内存
//   pdwOutLen ANSI字符串的长度
// 返回值:  void
// 日期:    [6/24/2005]
//////////////////////////////////////////////////////////////////////////
void CBase64::decode(char* pIn, unsigned long dwInLen, char* pOut, unsigned long* pdwOutLen)
{
    char c1, c2, c3, c4;
    int i = 0, n = 0;
    int len = dwInLen;

    while ( i<len)
    {
        // read the first byte
        do
        {
            c1 = _base64_decode_chars[ (int)pIn[i++] ];
        } while ( i<len && c1==-1);

        if ( c1==-1)
        {
            break;
        }

        // read the second byte
        do
        {
            c2 = _base64_decode_chars[ (int)pIn[i++] ];
        } while ( i<len && c2==-1);

        if ( c2==-1 )
        {
            break;
        }

        // assamble the first byte
        pOut[n++] = char( (c1<<2) | ((c2&0x30)>>4) );

        // read the third byte
        do
        {
            c3 = pIn[i++];
            if ( c3==61 )       // meet with "=", break
                goto end; //return;

            c3 = _base64_decode_chars[ (int)c3 ];
        } while ( i<len && c3==-1);

        if ( c3==-1 )
        {
            break;
        }

        // assamble the second byte
        pOut[n++] = char( ((c2&0XF)<<4) | ((c3&0x3C)>>2) );

        // read the fourth byte
        do
        {
            c4 = pIn[i++];
            if ( c4==61 )       // meet with "=", break
                goto end; //return;

            c4 = _base64_decode_chars[ (int)c4 ];
        } while ( i<len && c4==-1 );

        if ( c4==-1 )
        {
            break;
        }

        // assamble the third byte
        pOut[n++] = char( ((c3&0x03)<<6) | c4 );
    }

end:
	*pdwOutLen = n;

	return;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    BOOL encodeMemMap()
// 功能:    将ANSI格式文件转成Base64格式文件
// 参数:    fIn  ANSI格式的文件名
//   fOut Base64格式的文件名
// 返回值:  BOOL TRUE(成功) FALSE(失败)
// 日期:    [6/24/2005]
//////////////////////////////////////////////////////////////////////////
bool CBase64::encodeMemMap(const char* fIn, const char* fOut)
{
	int hIn, hOut, hInMap, hOutMap;
	char* pIn;
	char* pInFile;
	char* pOut;
	char* pOutFile;

	unsigned long dwInLow, dwOutLow;
	unsigned long dwOutLen = 0;

	mode_t	mode=0;
	int	flag=0;

	mode |= S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
	flag |= O_CREAT|O_RDONLY;

	hIn = open(fIn,flag, mode);
	hInMap = open(fIn, flag, mode);

	hOut = open(fOut, O_CREAT|O_WRONLY, mode);
	hOutMap = open(fOut, O_CREAT|O_RDWR, mode);

	dwInLow = lseek(hIn, 0, SEEK_END);

//	dwInLow = filelength(hIn);
	dwOutLow = CalcANSItoBase64Len(dwInLow);

	pInFile = (char *)mmap(NULL,dwInLow,PROT_READ, MAP_SHARED, hInMap, 0); //|MAP_VARIABLE
	pOutFile = (char *)mmap( 0, dwOutLow, PROT_READ | PROT_WRITE, MAP_SHARED, hOutMap, 0);

	if(MAP_FAILED == pInFile || MAP_FAILED == pOutFile )
	{
		return false;
	}

	//转换
	pIn = pInFile;
	pOut = pOutFile;
	encode(pIn, dwInLow, pOut, &dwOutLen);

	munmap(pOutFile, dwOutLow);
	munmap(pInFile, dwInLow);

	close(hOutMap);
	close(hInMap);

	close(hOut);
	close(hIn);

	return true;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    BOOL decodeMemMap()
// 功能:    将Base64格式文件转成ANSI格式文件
// 参数:    fIn  Base64格式的文件名
//   fOut ANSI格式的文件名
// 返回值:  BOOL TRUE(成功) FALSE(失败)
// 日期:    [6/24/2005]
//////////////////////////////////////////////////////////////////////////
bool CBase64::decodeMemMap(const char* fIn, const char* fOut)
{
	int hIn, hOut, hInMap, hOutMap;
	char* pIn;
	char* pInFile;
	char* pOut;
	char* pOutFile;

	unsigned long dwInLow, dwOutLow;
	unsigned long dwOutLen = 0;
	char szBuf[3] = "";

	mode_t	mode=0;
	int	flag=0;

	mode |= S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
	flag |= O_CREAT|O_RDONLY;

	hIn = open(fIn,flag, mode);
	dwInLow = lseek(hIn, 0, SEEK_END);


	if (dwInLow >= 2)
	{
		unsigned int size;
		lseek(hIn, -2, SEEK_END);
		size = read(hIn, szBuf, 2);
		dwOutLow = CalcBase64toANSILen(dwInLow, szBuf);
	}
	else
	{
		dwOutLow = CalcBase64toANSILen(dwInLow, "");
	}

	hInMap = open(fIn, flag, mode);
	pInFile = (char*)mmap( 0,dwInLow,PROT_READ, MAP_SHARED, hInMap, 0);

	hOut = open(fOut, O_CREAT|O_WRONLY, mode);
	hOutMap = open(fOut, O_CREAT|O_RDWR, mode);
	pOutFile = (char*)mmap( 0, dwOutLow, PROT_READ | PROT_WRITE, MAP_SHARED, hOutMap, 0);

	if(MAP_FAILED == pInFile || MAP_FAILED == pOutFile )
	{
		return false;
	}

	//转换
	pIn = pInFile;
	pOut = pOutFile;
	decode(pIn, dwInLow, pOut, &dwOutLen);

	munmap(pOutFile, dwOutLow);
	munmap(pInFile, dwInLow);

	close(hOutMap);
	close(hInMap);

	close(hOut);
	close(hIn);

	return true;
}



//////////////////////////////////////////////////////////////////////////
// 函数:    BOOL encode()
// 功能:    将ANSI格式文件转成Base64格式文件
// 参数:    cstrSrc ANSI格式的文件名
//   cstrDes Base64格式的文件名
// 返回值:  BOOL TRUE(成功) FALSE(失败)
// 日期:    [6/24/2005]
//////////////////////////////////////////////////////////////////////////
bool CBase64::encode(const string cstrSrc, const string cstrDes)
{
	fstream srcFile;
	fstream desFile;

	srcFile.open(cstrSrc.c_str(), std::ios::in);
	desFile.open(cstrDes.c_str(), std::ios::trunc | ios::out);
	if( !srcFile.is_open() ||  !desFile.is_open())
	{
		return false;
	}

	char buffer[4096];
	unsigned long dwBase64 = 0;
	while(!srcFile.eof())
	{
		memset(buffer, 0, sizeof(buffer));
		srcFile.getline(buffer, sizeof(buffer));

		int nBytesRead = strlen(buffer);

		// 将Base64格式写入文件
		char* pszBase64 = AllocMemBase64(nBytesRead);
		dwBase64 = 0;

		encode(buffer, nBytesRead, pszBase64, &dwBase64);
		desFile.write(pszBase64, dwBase64);

		FreeMemBase64(pszBase64);
	}

/*
	srcFile.seekg(0, ios::end);      //设置文件指针到文件流的尾部
	streampos pos = srcFile.tellg();  //读取文件指针的位置
	srcFile.seekg(0, ios::beg);

	unsigned long length = pos;
	int size = 4095; //必须是3的倍数，不然就被'='所烦。解码只要以四的倍数即可
	while( length > 0 )
	{
		char* buffer = NULL;
		buffer = new char[size];
		srcFile.read(buffer, size);

		unsigned long nBytesRead = srcFile.gcount();

		if(nBytesRead > 0)
		{
			// 将Base64格式写入文件
			char* pszBase64 = AllocMemBase64(nBytesRead);
			unsigned long dwBase64 = 0;

			encode(buffer, nBytesRead, pszBase64, &dwBase64);
			desFile.write(pszBase64, dwBase64);

			FreeMemBase64(pszBase64);
		}

		delete[] buffer;
		length -= nBytesRead;
	}
*/
	srcFile.close();
	desFile.close();

	return true;
}


//////////////////////////////////////////////////////////////////////////
// 函数:    BOOL decode()
// 功能:    将Base64格式文件转成ANSI格式文件
// 参数:    cstrSrc Base64格式的文件名
//   cstrDes ANSI格式的文件名
// 返回值:  BOOL TRUE(成功) FALSE(失败)
// 日期:    [6/24/2005]
//////////////////////////////////////////////////////////////////////////
bool CBase64::decode(const string cstrSrc, const string cstrDes)
{
	fstream srcFile;
	fstream desFile;

	srcFile.open(cstrSrc.c_str(), std::ios::in);
	desFile.open(cstrDes.c_str(), std::ios::trunc | ios::out);

	if( !srcFile.is_open() ||  !desFile.is_open())
	{
		return false;
	}

	char buffer[4096];
	unsigned long dwANSI = 0;
	while(!srcFile.eof())
	{
		memset(buffer, 0, sizeof(buffer));
		srcFile.getline(buffer, sizeof(buffer));

		int nBytesRead = strlen(buffer);

		char* pszANSI = AllocMemANSI(nBytesRead);
		dwANSI = 0;
		decode(buffer, nBytesRead, pszANSI, &dwANSI);
		desFile.write(pszANSI, dwANSI);
		FreeMemANSI(pszANSI);
	}

/*
	srcFile.seekg(0, ios::end);      //设置文件指针到文件流的尾部
	streampos pos = srcFile.tellg();  //读取文件指针的位置
	srcFile.seekg(0, ios::beg);

	// 读取ANSI文件放入cstrANSI
	unsigned long length = pos;
	int size = 4096; //必须是4的倍数


	while(length>0)
	{
	   char* buffer = NULL;
	   buffer = new char[size];
	   memset(buffer, 0, size);
	   srcFile.read(buffer, size);
	   unsigned int nBytesRead = srcFile.gcount();

	   if(nBytesRead > 0)
	   {
		   char* pszANSI = AllocMemANSI(nBytesRead);
		   unsigned long dwANSI = 0;
		   decode(buffer, nBytesRead, pszANSI, &dwANSI);
		   desFile.write(pszANSI, dwANSI);
		   FreeMemANSI(pszANSI);
	   }

	   delete[] buffer;

	   length -= nBytesRead;
	}
*/
	srcFile.close();
	desFile.close();

	return true;
}


