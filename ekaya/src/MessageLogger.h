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
#ifndef MessageLogger_h
#define MessageLogger_h

#include <stdio.h>
#include <stdarg.h>
#include "Ekaya.h"

#ifdef __cplusplus

namespace EKAYA_NS {

class MessageLogger
{
public:
	static void logMessage(const char * msg, ...);
	static void logMessage(const wchar_t * msg, ...);
    static void logMessage(const char * msg, va_list & args);

    MessageLogger(const char * filename) : mFile(fopen(filename, "wb")) { instance = this; };
    ~MessageLogger() { if (mFile) { fclose(mFile); mFile = NULL; } instance = NULL; };
private:
    static MessageLogger * instance;
    FILE * mFile;
};

}

extern "C" {
#endif

    extern __declspec(dllexport) void ekayaLogMessage(const char * msg, ...);
    extern __declspec(dllexport) void ekayaLogMessageArgs(const char * msg, va_list args);

#ifdef __cplusplus
}
#endif


#endif