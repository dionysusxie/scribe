// Base64.h: interface for the CBase64 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASE64_H_INCLUDED_)
#define AFX_BASE64_H_INCLUDED_

#include <string>
using namespace std;

class CBase64  
{
public:
	CBase64();
	virtual ~CBase64();

public:

	// 将ANSI字符串转成Base64字符串
	static string encode(const string in_str);
	static void encode(char* pIn, unsigned long dwInLen, char* pOut, unsigned long* pdwOutLen);

	// 将Base64字符串转成ANSI字符串
	static string decode(const string in_str);
	static void decode(char* pIn, unsigned long dwInLen, char* pOut, unsigned long* pdwOutLen);

	// 将ANSI格式文件转成Base64格式文件
	static bool encode(const string cstrSrc, const string cstrDes);
	static bool encodeMemMap(const char* fIn, const char* fOut);

	// 将Base64格式文件转成ANSI格式文件
	static bool decode(const string cstrSrc, const string cstrDes);
	static bool decodeMemMap(const char* fIn, const char* fOut);

	static char* AllocMemBase64(unsigned long dwANSILen);
	static char* AllocMemANSI(unsigned long dwBase64Len);
	static void FreeMemBase64(char* pBase64);
	static void FreeMemANSI(char* pANSI);

protected:
	static	unsigned long CalcANSItoBase64Len(unsigned long dwANSILen);
	static	unsigned long CalcBase64toANSILen(unsigned long dwBase64Len, const string strBase64End2="");

private:

    // encode table(编码表)
    const static string _base64_encode_chars;
 
    // decode table(解码表)
    const static char _base64_decode_chars[128];

};

#endif // !defined(AFX_BASE64_H_INCLUDED_)
