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
#include <algorithm>

#include <kmfl/kmfl.h>
#include <kmfl/kmfl_register_callbacks.h>
#include <kmfl/kmflutfconv.h>

#include "UtfConversion.h"
#include "KmflKeyboard.h"
#include "MessageLogger.h"

using namespace EKAYA_NS;

#undef min
#undef max

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

namespace EKAYA_NS {

std::basic_string<Utf32> KmflKeyboard::sDummy;


KmflKeyboard::KmflKeyboard(int kmflId, std::string baseDir, std::string filename)
: mKmflId(kmflId), mContextBuffer(sDummy), mBaseDir(baseDir), mFilename(filename)
{
    mKmsi = kmfl_make_keyboard_instance(this);
    kmfl_register_callbacks(ekayaKmflOutputString, ekayaKmflOutputChar, 
        ekayaKmflOutputBeep, ekayaKmflForwardKeyevent, ekayaKmflEraseChar,
        ekayaLogMessageArgs);
      int status = kmfl_attach_keyboard(mKmsi, mKmflId);
    MessageLogger::logMessage("KMFL attached keyboard %d status %d\n", mKmflId, status);
}

// KmflKeyboard::KmflKeyboard(const KmflKeyboard & parent)
    // : mKmflId(-1), mContextBuffer(sDummy)
// {
    // assert(false);
// }

KmflKeyboard::~KmflKeyboard()
{
    int status = kmfl_detach_keyboard(mKmsi);
    MessageLogger::logMessage("KMFL detached keyboard %d status %d\n", mKmflId, status);
    kmfl_delete_keyboard_instance(mKmsi);
}

std::pair<size_t, size_t> KmflKeyboard::processKey(long keyId, std::basic_string<Utf32> & context, size_t contextPos)
{
    int status = 0;
    //kmfl_attach_keyboard(mKmsi, mKmflId);
    mContextBuffer = context;
    mContextPosition = contextPos;

    UINT state = 0;
    ITEM contextItems[KMFL_MAX_CONTEXT];
    assert(contextPos <= KMFL_MAX_CONTEXT);
    size_t contextLen = ::std::min(static_cast<size_t>(KMFL_MAX_CONTEXT), contextPos);
    bool replaceHistory = false;
    size_t iKmfl = 1; // KMFL history is offset by 1
    for (UINT i = 0; i < contextLen; i++, iKmfl++)
    {
        // Skip over dead keys in kmfl history, since they aren't in the real context
        while (mKmsi->history[iKmfl] & 0x5000000)
        {
            ++iKmfl;
        }
        contextItems[i] = context[contextLen - 1 - i];
        // have a peek at the raw history and see if we need to set it
        // otherwise, we may lose dead keys.
        if (mKmsi->nhistory < iKmfl ||
            (mKmsi->history[iKmfl] & 0xffffff) != contextItems[i])
        {
            MessageLogger::logMessage("KMFL reset history %d,%d %x!=%x len %d\n", i, iKmfl,
                 contextItems[i], mKmsi->history[iKmfl], mKmsi->nhistory);
            replaceHistory = true;
        }
    }
    if (replaceHistory)
    {
        set_history(mKmsi, contextItems, static_cast<UINT>(contextLen));
    }
    status = kmfl_interpret(mKmsi, static_cast<UINT>(keyId), state);
    MessageLogger::logMessage("kmfl_interpret status=%d\n", status);

    size_t newLength = (mContextBuffer.length());
    context = mContextBuffer;
    //status = kmfl_detach_keyboard(mKmsi);
    mContextBuffer = sDummy;
    return std::make_pair(mContextPosition, newLength);
}

std::basic_string<Utf32> KmflKeyboard::getDescription()
{
    //int status = kmfl_attach_keyboard(mKmsi, mKmflId);
    
    std::basic_string<Utf32> desc;
    for (size_t i = 0; i < NAMELEN; i++)
    {
        Utf32 utf32 = mKmsi->kbd_name[i];
        if (utf32 == 0) break;
        desc = desc.append(1, utf32);
    }
    //status = kmfl_detach_keyboard(mKmsi);
    return desc;
}

std::basic_string<UTF32> KmflKeyboard::getIconFileName()
{
    return UtfConversion::convertUtf8ToUtf32(std::string(mBaseDir + kmfl_icon_file(mKmflId)));
}

std::basic_string<UTF32> KmflKeyboard::getHelpFileName()
{
    size_t extPos = mFilename.find(".kmn");
    std::string helpFileName(mBaseDir + mFilename.substr(0, extPos) + ".html");
    FILE * test = NULL;
    fopen_s(&test, helpFileName.c_str(), "r");
    if (test)
    {
        fclose(test);
        return UtfConversion::convertUtf8ToUtf32(helpFileName);
    }
    helpFileName = std::string(mBaseDir + mFilename.substr(0, extPos) + ".pdf");
    fopen_s(&test, helpFileName.c_str(), "r");
    if (test)
    {
        fclose(test);
        return UtfConversion::convertUtf8ToUtf32(helpFileName);
    }
    return std::basic_string<UTF32>();
}

void KmflKeyboard::outputString(char *p)
{
    size_t utf8Len = strlen(p);
    MessageLogger::logMessage("KMFL outputString length %d %s\n", utf8Len, p); 
    assert(utf8Len < KMFL_MAX_CONTEXT);
    UTF32 utf32[KMFL_MAX_CONTEXT];
    UTF32 * p32 = utf32;
    // this is UTF-8 so convert to UTF-32
    //size_t result = 
    IConvertUTF8toUTF32((const UTF8 **)&p,(const UTF8 *)p+utf8Len,&p32,p32 + KMFL_MAX_CONTEXT);
    mContextBuffer = mContextBuffer.insert(mContextBuffer.length(), utf32, p32 - utf32);
}

void KmflKeyboard::outputChar(BYTE q)
{
    // doesn't seem to be used
    MessageLogger::logMessage("KMFL outputChar %x\n", q);
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

} // EKAYA_NS
