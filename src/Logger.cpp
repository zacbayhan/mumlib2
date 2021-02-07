//Windows
#include <windows.h>

//mumlib
#include "mumlib/Logger.hpp"

namespace mumlib {
	void Logger::output_to_debug(const std::string& str)
	{
		OutputDebugStringA(str.c_str());
	}
}
