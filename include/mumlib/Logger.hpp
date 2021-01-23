#pragma once

//stdlib
#include <string>
#include <sstream>

namespace mumlib {
	class Logger {
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
