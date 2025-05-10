#pragma once

#include <iostream>

#define _consoleStream(func, line) std::cout << "Message : " << func << " at line [" << line << "] : "
#define _errorStream(func, line)   std::cerr << "Error   : " << func << " at line [" << line << "] : "
#define _debugStream(func, line)   std::cerr << "Debug   : " << __DATE__ << " " << __TIME__ << " | " << func << " at line [" << line << "] : "

#ifdef NDEBUG
constexpr auto MSG_DEBUG = false;
#else
constexpr auto MSG_DEBUG = true;
#endif

#define debugStream          \
    if constexpr (MSG_DEBUG) \
    _debugStream(__FUNCTION__, __LINE__)

#define consoleStream       _consoleStream(__FUNCTION__, __LINE__)
#define errorStream         _errorStream(__FUNCTION__, __LINE__)
#define consoleLog(message) consoleStream << (message) << std::endl
#define errorLog(message)   errorStream << (message) << std::endl
#define debugLog(message)   debugStream << (message) << std::endl
#define errorFatal(message) \
    {                       \
        errorLog(message);  \
        abort();            \
    }
#define errorWarning(message) \
    {                         \
        errorLog(message);    \
    }
#define checkErrorWarning(test, message) \
    if (test)                            \
    errorWarning(message)
#define checkErrorFatal(test, message) \
    if (test)                          \
    errorFatal(message)
