/* libkmfl.h
 * Copyright (C) 2005 SIL International and Tavultesoft Pty Ltd
 *
 * This file is part of the KMFL library.
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

// LIBKMFL.H: Header for interpreter for Keyboard Mapping for Linux

#ifndef LIBKMFL_H

#ifdef _WIN32
#define KMFL_EXPORT __declspec(dllexport)
#else
#define KMFL_EXPORT
#endif

#ifdef	__cplusplus
extern "C" {
#endif
KMFL_EXPORT
int kmfl_interpret(KMSI *p_kmsi, UINT key, UINT state);
KMFL_EXPORT
int kmfl_load_keyboard(const char *file);
KMFL_EXPORT
int kmfl_check_keyboard(const char *file);
KMFL_EXPORT
int kmfl_reload_keyboard(int keyboard_number);
KMFL_EXPORT
int kmfl_reload_all_keyboards(void);
KMFL_EXPORT
int kmfl_unload_keyboard(int keyboard_number);
KMFL_EXPORT
int kmfl_unload_all_keyboards(void);

KMFL_EXPORT
KMSI *kmfl_make_keyboard_instance(void *connection);
KMFL_EXPORT
int kmfl_delete_keyboard_instance(KMSI *p_kmsi);
KMFL_EXPORT
int kmfl_delete_all_keyboard_instances(void);

KMFL_EXPORT
int kmfl_attach_keyboard(KMSI *p_kmsi, int keyboard_number);
KMFL_EXPORT
int kmfl_detach_keyboard(KMSI *p_kmsi);

KMFL_EXPORT
int kmfl_keyboard_number(char *name);
KMFL_EXPORT
const char *kmfl_keyboard_name(int keyboard_number);
KMFL_EXPORT
const char *kmfl_icon_file(int keyboard_number);

int kmfl_get_header(KMSI *p_kmsi,int hdrID,char *buf,int buflen);

void DBGMSG(int debug,const char *fmt,...);
void *ERRMSG(const char *fmt,...);
KMFL_EXPORT
void clear_history(KMSI *p_kmsi);
KMFL_EXPORT
int deadkey_in_history(KMSI *p_kmsi);
KMFL_EXPORT
void set_history(KMSI *p_kmsi, ITEM * items, UINT nitems);

extern int kmfl_debug;

#ifdef	__cplusplus
}
#endif	

#endif /* *** end of LIBKMFL.H *** */
