#ifndef UtfConversion_h
#define UtfConversion_h

#include <kmfl/kmflutfconv.h>
#include <string>

class UtfConversion
{
public:
	static std::wstring convertUtf32ToUtf16(std::basic_string<UTF32> utf32);
	static std::basic_string<UTF32> convertUtf16ToUtf32(std::wstring utf16);
	static std::basic_string<UTF32> convertUtf8ToUtf32(std::string utf8);
	static std::string convertUtf32ToUtf8(std::basic_string<UTF32> utf32);
};

#endif
