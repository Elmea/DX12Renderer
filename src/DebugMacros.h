#pragma once

#include <iostream>

#define LOGTYPE_VALIDATION 10	// Green
#define LOGTYPE_INFO 11		// Blue
#define LOGTYPE_WARNING 14		// Yellow
#define LOGTYPE_ERROR 12		// Red
#define LOGTYPE_LOG 15			// White

#define LOG(str) DEBUG_LOG(str, LOGTYPE_LOG)
#define DEBUG_LOG(str, logType) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), logType); std::cout << str << std::endl; SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
