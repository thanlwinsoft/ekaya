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
#ifndef KmflKeyboard_h
#define KmflKeyboard_h

#include "EkayaKeyboard.h"

#include <kmfl/kmfl.h>
#include <libkmfl.h>

enum { KMFL_MAX_CONTEXT = 16 };

class KmflKeyboard : public EkayaKeyboard
{
public:
	KmflKeyboard(int kmflId, std::string baseDir);
	~KmflKeyboard();
	virtual int processKey(long keyId, std::basic_string<Utf32> & context, size_t contextPos);
	virtual std::basic_string<Utf32> getIconFileName();

	virtual std::basic_string<Utf32> getDescription();
	// KMFL call back methods
	void outputString(char *p);
	void outputChar(BYTE q);
	void outputBeep(void);
	void forwardKeyevent(UINT key, UINT state);
	void eraseChar(void);
private:
	int mKmflId;
	KMSI * mKmsi;
	std::basic_string<Utf32> & mContextBuffer;
	size_t mContextPosition;
	static std::basic_string<Utf32> sDummy;
	std::string mBaseDir;
};


#endif
