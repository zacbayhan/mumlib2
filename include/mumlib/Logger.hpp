#pragma once

//stdlib
#include <iostream>
#include <string>


namespace mumlib {
	class Logger {
	public:
		explicit Logger(const std::string& category);
		explicit Logger(const char* category);
		
		template<typename... Args>
		void crit(const std::string& text, Args... args)
		{
		//	std::cout << "[crit]" << text << std::endl;
		}

		template<typename... Args>
		void debug(const std::string& text, Args... args)
		{
		//	std::cout << "[debug]" << text << std::endl;
		}

		template<typename... Args>
		void error(const std::string& text, Args... args)
		{
		//	std::cout << "[error]" << text << std::endl;
		}

		template<typename... Args>
		void info(const std::string& text, Args... args)
		{
		//	std::cout << "[info]" << text << std::endl;
		}

		template<typename... Args>
		void notice(const std::string& text, Args... args)
		{
		//	std::cout << "[notice]" << text << std::endl;
		}

		template<typename... Args>
		void warn(const std::string& text, Args... args)
		{
		//	std::cout << "[warn]" << text << std::endl;
		}
	};
}
