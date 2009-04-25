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

#include <stdio.h>

#include <kmfl/kmfl.h>
#include <libkmfl.h>

#include "KmflKeyboard.h"
#include "KmflKeyboardFactory.h"


std::vector<EkayaKeyboard*>
KmflKeyboardFactory::loadKeyboards()
{
	std::vector<EkayaKeyboard*> keyboards;

	char* appDir = NULL;
    size_t requiredSize;

    getenv_s( &requiredSize, NULL, 0, "APPDATA");
	appDir = new char[requiredSize];
	std::string dataPath;
	if (appDir)
	{
		getenv_s( &requiredSize, appDir, requiredSize, "APPDATA" );
		dataPath = std::string(appDir) + "\\ThanLwinSoft.org\\Ekaya\\kmfl";
	}

	// TODO find system and user application directories and iterate over *.kmn files
	// temp HACK
	std::string path = "C:\\Users\\keith\\projects\\ekaya\\myWin2.2.kmn";
	//DIR *dir = opendir(dataPath.c_str());

 //   if (dir != NULL)
	//{
 //       struct dirent *file = readdir(dir);
	//	while (file != NULL)
	//	{
	//		//"C:\\Users\\keith\\projects\\ekaya\\myWin2.2.kmn";
	//		std::string absPath = path + "\\" + file->d_name;
	//		if (absPath.substr(absPath.length() - 4, 4) == ".kmn")
	//		{
				int kmflId = kmfl_load_keyboard(path.c_str());
				if (kmflId > -1)
				{
					keyboards.push_back(new KmflKeyboard(kmflId));
				}
	//		}
	//		file = readdir(dir);
	//	}
	//}

	return keyboards;
}

KmflKeyboardFactory::~KmflKeyboardFactory()
{
	// should we use this or delete them individually?
	kmfl_unload_all_keyboards();
}
