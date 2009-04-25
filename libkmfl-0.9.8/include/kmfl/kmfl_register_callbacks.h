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
#ifndef kmfl_register_callbacks_h
#define kmfl_register_callbacks_h

#ifdef  __cplusplus
extern "C" {
#endif

void kmfl_register_callbacks(void(*poutput_string)(void *connection, char *p), 
						void(*poutput_char)(void *connection, BYTE q),
						void(*poutput_beep)(void *connection),
						void(*pforward_keyevent)(void *connection, UINT key, UINT state),
						void(*perase_char)(void *connection));

#ifdef  __cplusplus
}
#endif

#endif
