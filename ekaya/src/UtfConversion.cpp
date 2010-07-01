/*
 * Copyright (C) 2009 ThanLwinSoft.org
 *
 * The Ekaya library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The KMFL library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with the Ekaya library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */
#include "UtfConversion.h"

using namespace EKAYA_NS;

// TODO consider caching the buffers

std::wstring UtfConversion::convertUtf32ToUtf16(std::basic_string<UTF32> utf32)
{
	const UTF32 * input = utf32.c_str();
	size_t maxBuffer = utf32.length()*2;
	UTF16 * output = new UTF16[maxBuffer];
	UTF16 * pBuffer = output;
	size_t status = IConvertUTF32toUTF16(&input, input + utf32.length(), &pBuffer, pBuffer + maxBuffer);
	if (!status)
	{
		std::wstring data(reinterpret_cast<wchar_t *>(output), pBuffer - output);
		delete [] output;
		return data;
	}
	delete [] output;
	return std::wstring(L"");
}

std::basic_string<UTF32> UtfConversion::convertUtf16ToUtf32(std::wstring utf16)
{
	const UTF16 * input = reinterpret_cast<const UTF16*>(utf16.c_str());
	size_t maxBuffer = utf16.length();
	UTF32 * output = new UTF32[maxBuffer];
	UTF32 * pBuffer = output;
	size_t status = IConvertUTF16toUTF32(&input, input + utf16.length(), &pBuffer, pBuffer + maxBuffer);
	if (!status)
	{
		std::basic_string<UTF32>data(output, pBuffer - output);
		delete [] output;
		return data;
	}
	delete [] output;
	return std::basic_string<UTF32>();
}

std::basic_string<UTF32> UtfConversion::convertUtf8ToUtf32(std::string utf8)
{
	const UTF8 * input = reinterpret_cast<const UTF8*>(utf8.c_str());
	size_t maxBuffer = utf8.length();
	UTF32 * output = new UTF32[maxBuffer];
	UTF32 * pBuffer = output;
	size_t status = IConvertUTF8toUTF32(&input, input + utf8.length(), &pBuffer, pBuffer + maxBuffer);
	if (!status)
	{
		std::basic_string<UTF32>data(output, pBuffer - output);
		delete [] output;
		return data;
	}
    delete [] output;
	return std::basic_string<UTF32>();
}

std::string UtfConversion::convertUtf32ToUtf8(std::basic_string<UTF32> utf32)
{
	const UTF32 * input = utf32.c_str();
	size_t maxBuffer = utf32.length()*4;
	char * output = new char[maxBuffer];
	UTF8 * pBuffer = reinterpret_cast<UTF8*>(output);
	size_t status = IConvertUTF32toUTF8(&input, input + utf32.length(), &pBuffer, pBuffer + maxBuffer);
	if (!status)
	{
		std::string data(output, reinterpret_cast<char *>(pBuffer) - output);
		delete [] output;
		return data;
	}
	delete [] output;
	return std::string("");
}

std::wstring UtfConversion::convertUtf8ToUtf16(std::string utf8)
{
	const UTF8 * input = reinterpret_cast<const UTF8*>(utf8.c_str());
	size_t maxBuffer = utf8.length();
	UTF16 * output = new UTF16[maxBuffer];
	UTF16 * pBuffer = output;
	size_t status = IConvertUTF8toUTF16(&input, input + utf8.length(), &pBuffer, pBuffer + maxBuffer);
	if (!status)
	{
		std::wstring data(reinterpret_cast<const wchar_t*>(output), pBuffer - output);
		delete [] output;
		return data;
	}
    delete [] output;
	return std::wstring();
}

std::string UtfConversion::convertUtf16ToUtf8(std::wstring utf16)
{
	const UTF16 * input =  reinterpret_cast<const UTF16*>(utf16.c_str());
	size_t maxBuffer = utf16.length()*3;
	char * output = new char[maxBuffer];
	UTF8 * pBuffer = reinterpret_cast<UTF8*>(output);
	size_t status = IConvertUTF16toUTF8(&input, input + utf16.length(), &pBuffer, pBuffer + maxBuffer);
	if (!status)
	{
		std::string data(output, reinterpret_cast<char *>(pBuffer) - output);
		delete [] output;
		return data;
	}
	delete [] output;
	return std::string("");
}

