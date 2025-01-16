#include <string>
#include <windows.h>

#include <iostream>
#include <stdexcept>

namespace TabGraph::Win32 {
void CheckError(const std::string& a_File, const uint32_t& a_Line, const std::string& a_Procedure)
{
    const auto errorCode = GetLastError();
    SetLastError(ERROR_SUCCESS);
    std::string errorString = a_File + "@" + std::to_string(a_Line) + " : " + a_Procedure + " failed, **ERROR CODE** : " + std::to_string(errorCode);
    std::cerr << errorString << std::endl;
    throw std::runtime_error(errorString);
}
}
