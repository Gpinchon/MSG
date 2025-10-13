#pragma once

#include <iostream>

#define _MSGConsoleStream(func, line) std::cout << "Message : " << func << " at line [" << line << "] : "
#define _MSGErrorStream(func, line)   std::cerr << "Error   : " << func << " at line [" << line << "] : "
#define _MSGDebugStream(func, line)   std::cerr << "Debug   : " << __DATE__ << " " << __TIME__ << " | " << func << " at line [" << line << "] : "

#ifdef NDEBUG
constexpr auto MSG_DEBUG = false;
#else
constexpr auto MSG_DEBUG = true;
#endif

#define MSGDebugStream       \
    if constexpr (MSG_DEBUG) \
    _MSGDebugStream(__FUNCTION__, __LINE__)

#define MSGConsoleStream       _MSGConsoleStream(__FUNCTION__, __LINE__)
#define MSGErrorStream         _MSGErrorStream(__FUNCTION__, __LINE__)
#define MSGConsoleLog(message) MSGConsoleStream << (message) << std::endl
#define MSGErrorLog(message)   MSGErrorStream << (message) << std::endl
#define MSGDebugLog(message)   MSGDebugStream << (message) << std::endl
#define MSGErrorFatal(message) \
    {                          \
        MSGErrorLog(message);  \
        abort();               \
    }
#define MSGErrorWarning(message) \
    {                            \
        MSGErrorLog(message);    \
    }
#define MSGCheckErrorWarning(test, message) \
    if (test)                               \
    MSGErrorWarning(message)
#define MSGCheckErrorFatal(test, message) \
    if (test)                             \
    MSGErrorFatal(message)
