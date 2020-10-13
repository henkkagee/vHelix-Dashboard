/*
 *  Copyright (c) 2020 Henrik Granö
 *  See the license in the root directory for the full notice
*/

#ifndef PROCESSIO_H
#define PROCESSIO_H

#define NOMINMAX // windows.h min and max macros conflict with std::min and std::max

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <strsafe.h>
#include <cstring>
#include <stdio.h>
#include <string>
#include <sstream>
#include <thread>


// Functions for creating child processes to run different modules

namespace processio {
    std::string handle(char* cmd);
    void CreateChildProcess(char* cmd);
    void WriteToPipe(void);
    void ReadFromPipe(void);
    void ErrorExit(PTSTR);
}

// Simple guard class to reset the stdout buffer
struct cout_redirect {
    cout_redirect( std::streambuf * new_buffer )
        : old( std::cout.rdbuf( new_buffer ) )
    { }

    ~cout_redirect( ) {
        std::cout.rdbuf( old );
    }

private:
    std::streambuf * old;
};

#endif // PROCESSIO_H
