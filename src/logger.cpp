// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#if defined(_WIN32)
//Windows
#include <windows.h>
#endif

//mumlib
#include "mumlib2/logger.h"

namespace mumlib2 {
	void Logger::output_to_debug(const std::string& str)
	{
#if defined(_WIN32)
		OutputDebugStringA(str.c_str());
#endif
	}
}
