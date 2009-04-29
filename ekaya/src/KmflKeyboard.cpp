#include <assert.h>
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
#include <kmfl/kmfl.h>
#include <kmfl/kmfl_register_callbacks.h>
#include <kmfl/kmflutfconv.h>

#include "KmflKeyboard.h"
#include "MessageLogger.h"

// callback functions
void ekayaKmflOutputString(void *connection, char *p)
{
	if (connection)
	{
		KmflKeyboard* keyboard = reinterpret_cast<KmflKeyboard*>(connection);
		keyboard->outputString(p);
	}
}


void ekayaKmflOutputChar(void *connection, BYTE q)
{
	if (connection)
	{
		KmflKeyboard* keyboard = reinterpret_cast<KmflKeyboard*>(connection);
		keyboard->outputChar(q);
	}
}

void ekayaKmflOutputBeep(void *connection)
{
	if (connection)
	{
		KmflKeyboard* keyboard = reinterpret_cast<KmflKeyboard*>(connection);
		keyboard->outputBeep();
	}
}

void ekayaKmflForwardKeyevent(void *connection, UINT key, UINT state)
{
	if (connection)
	{
		KmflKeyboard* keyboard = reinterpret_cast<KmflKeyboard*>(connection);
		keyboard->forwardKeyevent(key, state);
	}
}

void ekayaKmflEraseChar(void *connection)
{
	if (connection)
	{
		KmflKeyboard* keyboard = reinterpret_cast<KmflKeyboard*>(connection);
		keyboard->eraseChar();
	}
}

std::basic_string<Utf32> KmflKeyboard::sDummy;


KmflKeyboard::KmflKeyboard(int kmflId, std::string baseDir)
: mKmflId(kmflId), mContextBuffer(sDummy), mBaseDir(baseDir)
{
	mKmsi = kmfl_make_keyboard_instance(this);
	kmfl_register_callbacks(ekayaKmflOutputString, ekayaKmflOutputChar, 
		ekayaKmflOutputBeep, ekayaKmflForwardKeyevent, ekayaKmflEraseChar);
}

KmflKeyboard::~KmflKeyboard()
{
	kmfl_delete_keyboard_instance(mKmsi);
}

int KmflKeyboard::processKey(long keyId, std::basic_string<Utf32> & context, size_t contextPos)
{
	int status = kmfl_attach_keyboard(mKmsi, mKmflId);
	mContextBuffer = context;
	mContextPosition = contextPos;

	UINT state = 0;
	ITEM contextItems[KMFL_MAX_CONTEXT];
	size_t contextLen = std::min(static_cast<size_t>(KMFL_MAX_CONTEXT), contextPos);
	for (UINT i = 0; i < contextLen; i++)
	{
		contextItems[i] = context[contextLen - 1 - i];//[contextPos - contextLen + i];
	}

	set_history(mKmsi, contextItems, static_cast<UINT>(contextLen));
	status = kmfl_interpret(mKmsi, static_cast<UINT>(keyId), state);

	int newLength = static_cast<int>(mContextBuffer.length());
	context = mContextBuffer;
	status = kmfl_detach_keyboard(mKmsi);
	mContextBuffer = sDummy;
	return newLength;
}

std::basic_string<Utf32> KmflKeyboard::getDescription()
{
	int status = kmfl_attach_keyboard(mKmsi, mKmflId);
	
	std::basic_string<Utf32> desc;
	for (size_t i = 0; i < NAMELEN; i++)
	{
		Utf32 utf32 = mKmsi->kbd_name[i];
		if (utf32 == 0) break;
		desc = desc.append(1, utf32);
	}
	status = kmfl_detach_keyboard(mKmsi);
	return desc;
}

std::string KmflKeyboard::getIconFileName()
{
	return std::string(mBaseDir + kmfl_icon_file(mKmflId));
}

void KmflKeyboard::outputString(char *p)
{
	size_t utf8Len = strlen(p);
	assert(utf8Len < KMFL_MAX_CONTEXT);
	UTF32 utf32[KMFL_MAX_CONTEXT];
	UTF32 * p32 = utf32;
	// this is UTF-8 so convert to UTF-32
	size_t result = IConvertUTF8toUTF32((const UTF8 **)&p,(const UTF8 *)p+utf8Len,&p32,p32 + KMFL_MAX_CONTEXT);
	mContextBuffer = mContextBuffer.insert(mContextPosition, utf32, p32 - utf32);
	mContextPosition += (p32 - utf32);
}

void KmflKeyboard::outputChar(BYTE q)
{
	// doesn't seem to be used
	assert(false);
}

void KmflKeyboard::outputBeep(void)
{
	// ignore for now
}

void KmflKeyboard::forwardKeyevent(UINT key, UINT state)
{
	MessageLogger::logMessage("forwardKeyevent %x %x", (int)key, (int)state);
	// TODO
}

void KmflKeyboard::eraseChar(void)
{
	if (mContextPosition > 0)
	{
		--mContextPosition;
		mContextBuffer.erase(mContextPosition, mContextPosition + 1);
	}
}
