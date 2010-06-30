/***************************************************************************
 *   Copyright (C) 2008 by Keith Stribley,,,   *
 *   devel@thanlwinsoft.org   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <setjmp.h>

#include <fstream>
#include <iostream>
#include <string>
#include <assert.h>
#include <kmfl/kmfl.h>
#include <kmfl/kmflcomp.h>
#include <kmfl/kmflutfconv.h>
#include <kmfl/libkmfl.h>

#ifdef WIN32
#include <kmfl/kmfl_register_callbacks.h>
#endif

extern "C" {

    void output_string(void *contrack, char *ptr)
    {
        if (ptr) {
            //((KmflInstance *) contrack)->output_string(ptr);
            ((std::string *) contrack)->append(ptr);
        }
    }

    void erase_char(void *contrack)
    {
        std::string * str = (std::string *)contrack;
        size_t lastCodeLength = 0;
        // find number of bytes for utf8 code point
        for (size_t i = 0; i < str->length(); i+= lastCodeLength)
        {
            if (static_cast<unsigned char>((*str)[i]) < 128) lastCodeLength = 1; 
            else if (static_cast<unsigned char>((*str)[i]) < 224) lastCodeLength = 2;
            else if (static_cast<unsigned char>((*str)[i]) < 240) lastCodeLength = 3;
            else lastCodeLength = 4;
        }
        if (str->length() >= lastCodeLength) str->erase( str->length() - lastCodeLength, str->length());
    }

    void output_char(void *contrack, unsigned char byte)
    {
        if (byte == 8) {
            erase_char(contrack);
        } else {
            char s[2];
            s[0] = static_cast<char>(byte);
            s[1] = '\0';
            output_string(contrack, s);
        }
    }

    void forward_keyevent(void *contrack, unsigned int key, unsigned int state)
    {
        std::cerr << "forward keyevent << " << key << " " << state << std::endl; 
        //((KmflInstance *) contrack)->forward_keyevent(key, state);
    }

    void output_beep(void *contrack)
    {
        std::cerr << "beep!" << std::endl;
        //((KmflInstance *) contrack)->output_beep();
    }

    void log_message(const char *fmt, va_list args)
    {
        char buffer[1024];
        vsnprintf(buffer, 1024, fmt, args);
        std::cerr << buffer << std::endl;
    }
}                                /* extern "c" */


int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << argv[0] << " file.kmn testData.txt" << std::endl;
        std::cerr << "Test data file should have the format:" << std::endl;
        std::cerr << "Odd lines: ascii typed" << std::endl;
        std::cerr << "Even lines: expected utf8 output" << std::endl;
        return 1;
    }
    void * keyboard_buffer;
    unsigned long keyboard_buffer_size;
    char * kmflFile = argv[1];
#ifdef WIN32
    kmfl_register_callbacks(output_string, output_char, output_beep, forward_keyevent, erase_char, log_message);
#endif

    keyboard_buffer_size = compile_keyboard_to_buffer(argv[1], &keyboard_buffer);
    if (keyboard_buffer_size == 0)
    {
        std::cerr << "Failed to parse " << kmflFile << std::endl;
        return 3;
    }
    write_keyboard(kmflFile, keyboard_buffer, (int)keyboard_buffer_size);
    printf("wrote %s\n", kmflFile);
    free(keyboard_buffer);
    std::string utf8Out;
    if (kmfl_load_keyboard(kmflFile))
    {
        std::cerr << "Failed to load " << kmflFile << std::endl;
        return 2;
    }
    KMSI * kmsi = kmfl_make_keyboard_instance(&utf8Out);
    if (kmfl_attach_keyboard(kmsi, 0))
        std::cerr << ("Failed to attach keyboard") << std::endl;
    int errorCount = 0;
    try
    {
        std::ifstream fileInput;
        fileInput.open(argv[2],
                std::ifstream::in | std::ifstream::binary);
        if (!fileInput.is_open())
        {
            std::cerr << "Failed to open " << argv[2] << std::endl;
            return 4;
        }
        // loop over each line
        size_t lineNum = 1;

        while (fileInput.good())
        {
            std::string utf8Line;
            std::getline(fileInput, utf8Line);
            if (!utf8Line.length()) continue;
            for (size_t i = 0; i < utf8Line.length(); i++)
            {
                kmfl_interpret(kmsi, (UINT)utf8Line[i], 0);
            }

            std::string expectedResult;
            std::getline(fileInput, expectedResult);
            if (expectedResult != utf8Out)
            {
                std::cout << "Error at line: " << lineNum << "[" << utf8Line.c_str()
                    << "] expected:" << expectedResult.c_str() << " got:" 
                    << utf8Out.c_str() << std::endl;
                ++errorCount;
            }
            else
            {
                //std::cout << "Line: " << lineNum << " Correct" << std::endl;
            }
            lineNum+= 2;
            clear_history(kmsi);
            utf8Out.erase(0, utf8Out.length());
        }
        fileInput.close();
    }
    catch (...)
    {
        std::cerr << "exception occured"<< std::endl;
    }

    kmfl_detach_keyboard(kmsi);
    kmfl_delete_keyboard_instance(kmsi);
    if (errorCount)
        std::cerr << "Found " << errorCount << " errors" << std::endl;
    else
        std::cerr << "Check passed" << std::endl;
    return errorCount;
}

