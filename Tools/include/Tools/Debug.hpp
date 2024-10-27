/*
 * @Author: gpi
 * @Date:   2019-02-22 16:19:03
 * @Last Modified by:   gpi
 * @Last Modified time: 2019-10-07 13:43:39
 */

#pragma once

#include <iostream>

#define _consoleStream(func, line) std::cout << "Message : " << func << " at line [" << line << "] : "
#define _errorStream(func, line)   std::cerr << "Error   : " << func << " at line [" << line << "] : "

#define consoleStream       _consoleStream(__FUNCTION__, __LINE__)
#define errorStream         _errorStream(__FUNCTION__, __LINE__)
#define consoleLog(message) consoleStream << (message) << std::endl;
#define errorLog(message)   errorStream << (message) << std::endl;

#ifndef NDEBUG
#define _debugStream(func, line) std::cerr << "Debug   : " << __DATE__ << " " << __TIME__ << " | " << func << " at line [" << line << "] : "
#define debugLog(message)        _debugStream(__FUNCTION__, __LINE__) << message << std::endl;
#else
#define debugLog(message) void(message);
#endif
