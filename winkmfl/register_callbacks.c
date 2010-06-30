/*
 * Copyright (C) 2009 ThanLwinSoft.org
 *
 * The KMFL library is free software; you can redistribute it and/or
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
 * License along with the KMFL library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 *
 */
#include <kmfl/kmfl.h>
#include <kmfl/kmfl_register_callbacks.h>

void(*fOutputString)(void *connection, char *p) = 0;
void(*fOutputChar)(void *connection, BYTE q) = 0;
void(*fOutputBeep)(void *connection) = 0;
void(*fForwardKeyevent)(void *connection, UINT key, UINT state) = 0;
void(*fEraseChar)(void *connection) = 0;
void(*fLogMessage)(const char *fmt, ...) = 0;

__declspec(dllexport)
void kmfl_register_callbacks(void(*poutput_string)(void *connection, char *p), 
						void(*poutput_char)(void *connection, BYTE q),
						void(*poutput_beep)(void *connection),
						void(*pforward_keyevent)(void *connection, UINT key, UINT state),
						void(*perase_char)(void *connection),
                        void(*plog_message)(const char *fmt, va_list args))
{
	fOutputString = poutput_string;
	fOutputChar = poutput_char;
	fOutputBeep = poutput_beep;
	fForwardKeyevent = pforward_keyevent;
	fEraseChar = perase_char;
    fLogMessage = plog_message;
}

__declspec(dllexport)
void output_string(void *connection, char *p)
{
	if (fOutputString)
		(fOutputString)(connection,p);
}

__declspec(dllexport)
void output_char(void *connection, BYTE q)
{
	if (fOutputChar)
		(fOutputChar)(connection,q);
}

__declspec(dllexport)
void output_beep(void *connection)
{
	if (fOutputBeep)
		(fOutputBeep)(connection);
}

__declspec(dllexport)
void forward_keyevent(void *connection, UINT key, UINT state)
{
	if (fForwardKeyevent)
		(fForwardKeyevent)(connection,key,state);
}

__declspec(dllexport)
void erase_char(void *connection)
{
	if (fEraseChar)
		(fEraseChar)(connection);
}

__declspec(dllexport)
void log_message(const char *fmt, ...)
{
	if (fLogMessage)
    {
        va_list args;
        va_start(args, fmt);
		(fLogMessage)(fmt, args);
        va_end(args);
    }
}
