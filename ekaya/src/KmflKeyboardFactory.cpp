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

#include <kmfl/kmfl.h>
#include <kmfl/libkmfl.h>

#include "KmflKeyboard.h"
#include "KmflKeyboardFactory.h"

const std::string KmflKeyboardFactory::KMFL_DIR = "\\ThanLwinSoft.org\\Ekaya\\kmfl\\";
const std::string KmflKeyboardFactory::KMFL_PATTERN = "*.kmn";

std::vector<EkayaKeyboard*>
KmflKeyboardFactory::loadKeyboards()
{
	std::vector<EkayaKeyboard*> keyboards;

	char* appDir = NULL;
    size_t requiredSize;
	
    getenv_s( &requiredSize, NULL, 0, "APPDATA");
	appDir = new char[requiredSize];
	if (!appDir)
		return keyboards;
	std::string basePath;
	std::string pattern;
	if (appDir)
	{
		getenv_s( &requiredSize, appDir, requiredSize, "APPDATA" );
		basePath = std::string(appDir) + KMFL_DIR;
		pattern = basePath + KMFL_PATTERN;
	}
	else
	{
		return keyboards;
	}
	HANDLE hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAA ffd;

	// read files in user's APPDATA dir
	hFind = FindFirstFileA(pattern.c_str(), &ffd);
	while (INVALID_HANDLE_VALUE != hFind)
	{
		int kmflId = kmfl_load_keyboard((basePath + ffd.cFileName).c_str());
		if (kmflId > -1)
		{
			keyboards.push_back(new KmflKeyboard(kmflId, basePath, ffd.cFileName));
		}
		if (FindNextFileA(hFind, &ffd) == 0) break;
	}
	delete [] appDir;
	// read files under Program Files
	getenv_s( &requiredSize, NULL, 0, "ProgramFiles");
	appDir = new char[requiredSize];
	if (!appDir)
		return keyboards;
	getenv_s( &requiredSize, appDir, requiredSize, "ProgramFiles" );
	basePath = std::string(appDir) + KMFL_DIR;
	pattern = basePath + KMFL_PATTERN;
	hFind = FindFirstFileA(pattern.c_str(), &ffd);
	while (INVALID_HANDLE_VALUE != hFind)
	{
		int kmflId = kmfl_load_keyboard((basePath + ffd.cFileName).c_str());
		if (kmflId > -1)
		{
			keyboards.push_back(new KmflKeyboard(kmflId, basePath, ffd.cFileName));
		}
		if (FindNextFileA(hFind, &ffd) == 0) break;
	}
	delete [] appDir;
	return keyboards;
}

KmflKeyboardFactory::~KmflKeyboardFactory()
{
	// should we use this or delete them individually?
	kmfl_unload_all_keyboards();
}
