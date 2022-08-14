// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (c) 2015-2022 mumlib2 contributors

#pragma once

//stdlib
#include <string>
#include <sstream>

// mumlib2
#include "mumlib2/export.h"

namespace mumlib2 {
	class MUMLIB2_EXPORT Logger {
	public:
		Logger() {};
		Logger(const std::string&) {};
		Logger(const char*) {};
		~Logger() {}

		template<class... Args>
		void log(Args... args)
		{
			std::stringstream ss;
			(ss << ... << args) << "\n";
			output_to_debug(ss.str());
		}

		template<typename... Args>
		void crit(const std::string& text, Args... args)
		{
		}

		template<typename... Args>
		void debug(const std::string& text, Args... args)
		{
		}

		template<typename... Args>
		void error(const std::string& text, Args... args)
		{
		}

		template<typename... Args>
		void info(const std::string& text, Args... args)
		{
		}

		template<typename... Args>
		void notice(const std::string& text, Args... args)
		{
		}

		template<typename... Args>
		void warn(const std::string& text, Args... args)
		{
		}

	private:
		void output_to_debug(const std::string& str);
	};
}
