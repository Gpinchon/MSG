#pragma once

#include <string>

#ifdef _DEBUG
// Similar to assert except it attempts to GetLastError to find more intel
#define WIN32_CHECK_ERROR(condition)                                     \
    {                                                                    \
        if (!(condition)) {                                              \
            TabGraph::Win32::CheckError(__FILE__, __LINE__, #condition); \
        };                                                               \
    }
#else
#define WIN32_CHECK_ERROR(condition) \
    {                                \
        if (!(condition)) { };       \
    }
#endif _DEBUG

namespace TabGraph::Win32 {
void CheckError(const std::string& a_File, const uint32_t& a_Line, const std::string& a_Procedure);
}
