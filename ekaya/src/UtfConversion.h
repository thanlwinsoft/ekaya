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
#ifndef UtfConversion_h
#define UtfConversion_h

#include <kmfl/kmflutfconv.h>
#include <string>

class UtfConversion
{
public:
	static std::wstring convertUtf32ToUtf16(std::basic_string<UTF32> utf32);
	static std::wstring convertUtf8ToUtf16(std::string utf8);
	static std::basic_string<UTF32> convertUtf16ToUtf32(std::wstring utf16);
	static std::basic_string<UTF32> convertUtf8ToUtf32(std::string utf8);
	static std::string convertUtf32ToUtf8(std::basic_string<UTF32> utf32);
	static std::string convertUtf16ToUtf8(std::wstring utf16);
};

#endif
