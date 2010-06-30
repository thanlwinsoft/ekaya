/*
 * Copyright (C) 2009 ThanLwinSoft.org
 *
 * The Ekaya library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The Ekaya library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with the Ekaya library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */
#include <windows.h>
#include <stdio.h>
#include <cassert>
#include <stdarg.h>
#include "UtfConversion.h"
#include "MessageLogger.h"

static const int MAX_MSG_LEN = 1024;

MessageLogger * MessageLogger::instance = NULL;

void ekayaLogMessage(const char * msg, ...)
{
    va_list args;
    va_start(args, msg);
    MessageLogger::logMessage(msg, &args);
    va_end(args);
}

void ekayaLogMessageArgs(const char * msg, va_list args)
{
    MessageLogger::logMessage(msg, args);
    MessageLogger::logMessage("\n");
}

void MessageLogger::logMessage(const char * msg, ...)
{
    va_list args;
    va_start(args, msg);
    char buffer[MAX_MSG_LEN];
#ifndef NDEBUG
    int len = 
#endif
        vsnprintf(buffer, MAX_MSG_LEN, msg, args);
    assert(len + 1 < MAX_MSG_LEN);
	OutputDebugStringA(buffer);
    if (instance && instance->mFile)
    {
        fprintf(instance->mFile, buffer);
        fflush(instance->mFile);
    }
    va_end(args);
}

void MessageLogger::logMessage(const char * msg, va_list & args)
{
    char buffer[MAX_MSG_LEN];
#ifndef NDEBUG
    int len = 
#endif
        vsnprintf(buffer, MAX_MSG_LEN, msg, args);
    assert(len + 1 < MAX_MSG_LEN);
	OutputDebugStringA(buffer);
    if (instance && instance->mFile)
    {
        fprintf(instance->mFile, buffer);
        fflush(instance->mFile);
    }
}

void MessageLogger::logMessage(const wchar_t * msg, ...)
{
	va_list args;
    va_start(args, msg);
    wchar_t buffer[MAX_MSG_LEN];
#ifndef NDEBUG
    int len = 
#endif
        _vsnwprintf(buffer, MAX_MSG_LEN, msg, args);
    assert(len + 1 < MAX_MSG_LEN);
	OutputDebugStringW(buffer);
    if (instance && instance->mFile)
    {
        std::string utf8Msg = UtfConversion::convertUtf16ToUtf8(std::wstring(buffer));
        fprintf(instance->mFile, utf8Msg.c_str());
        fflush(instance->mFile);
    }
    va_end(args);
}

/*
void MessageLogger::logMessage(const char * msg, int param)
{
	char msgText[256];
	sprintf_s(msgText, msg, param);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const char * msg, const char * param)
{
	char msgText[256];
	sprintf_s(msgText, msg, param);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const char * msg, int paramA, int paramB)
{
	char msgText[256];
	sprintf_s(msgText, msg, paramA, paramB);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const char * msg, int paramA, const char * paramB)
{
	char msgText[256];
	sprintf_s(msgText, msg, paramA, paramB);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const char * msg, const char * paramA, int paramB)
{
	char msgText[256];
	sprintf_s(msgText, msg, paramA, paramB);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const char * msg, long paramA, const char * paramB)
{
	char msgText[256];
	sprintf_s(msgText, msg, paramA, paramB);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const wchar_t * msg, long paramA, const wchar_t * paramB)
{
	wchar_t msgText[256];
	swprintf_s(msgText, msg, paramA, paramB);
	OutputDebugStringW(msgText);
}

void MessageLogger::logMessage(const char * msg, const char * paramA, long paramB)
{
	char msgText[256];
	sprintf_s(msgText, msg, paramA, paramB);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const char * msg, const char * paramA, const char * paramB)
{
	char msgText[256];
	sprintf_s(msgText, msg, paramA, paramB);
	OutputDebugStringA(msgText);
}
void MessageLogger::logMessage(const char * msg, int paramA, int paramB, int paramC)
{
	char msgText[256];
	sprintf_s(msgText, msg, paramA, paramB, paramC);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const char * msg, long param)
{
	char msgText[256];
	sprintf_s(msgText, msg, param);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const char * msg, long long param)
{
	char msgText[256];
	sprintf_s(msgText, msg, param);
	OutputDebugStringA(msgText);
}

void MessageLogger::logMessage(const char * msg, long long param, long long paramB)
{
	char msgText[256];
	sprintf_s(msgText, msg, param, paramB);
	OutputDebugStringA(msgText);
}
*/
