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

class MessageLogger
{
public:
	static void logMessage(const char * msg);
	static void logMessage(const char * msg, int param);
	static void logMessage(const char * msg, const char * paramA);
	static void logMessage(const char * msg, int paramA, int paramB);
	static void logMessage(const char * msg, int paramA, const char * paramB);
	static void logMessage(const char * msg, const char * paramA, int paramB);
	static void logMessage(const char * msg, const char * paramA, const char * paramB);
	static void logMessage(const char * msg, int paramA, int paramB, int paramC);
	static void logMessage(const char * msg, long param);
	static void logMessage(const char * msg, long long param);
	static void logMessage(const char * msg, long long param, long long paramB);
	static void logMessage(const char * msg, long paramA, const char * paramB);
	static void logMessage(const wchar_t * msg, long paramA, const wchar_t * paramB);
	static void logMessage(const char * msg, const char * paramA, long paramB);
	static void logMessage(const wchar_t * msg);
};

#endif