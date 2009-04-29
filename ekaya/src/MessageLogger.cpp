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
#include <windows.h>
#include <stdio.h>
#include "MessageLogger.h"

void MessageLogger::logMessage(const char * msg)
{
	OutputDebugString(msg);
}

void MessageLogger::logMessage(const char * msg, int param)
{
	char msgText[256];
	sprintf_s(msgText, msg, param);
	OutputDebugString(msgText);
}

void MessageLogger::logMessage(const char * msg, int paramA, int paramB)
{
	char msgText[256];
	sprintf_s(msgText, msg, paramA, paramB);
	OutputDebugString(msgText);
}

void MessageLogger::logMessage(const char * msg, int paramA, int paramB, int paramC)
{
	char msgText[256];
	sprintf_s(msgText, msg, paramA, paramB, paramC);
	OutputDebugString(msgText);
}

void MessageLogger::logMessage(const char * msg, long param)
{
	char msgText[256];
	sprintf_s(msgText, msg, param);
	OutputDebugString(msgText);
}