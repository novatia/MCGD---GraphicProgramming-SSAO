#pragma once

#include <sstream>
#include <ctime>

#ifndef XTEST_UNUSED_VAR
#   define XTEST_UNUSED_VAR(x) (void)(x);
#endif


#ifndef  XTEST_DEBUG_LOG
#   define XTEST_DEBUG_LOG(s) \
	{ \
		std::time_t currentTime = std::time(0); \
		std::tm localTime; \
		localtime_s(&localTime, &currentTime); \
		std::wostringstream oStringStream; \
		oStringStream << "[XTEST-" << localTime.tm_hour << ":" << localTime.tm_min << ":" << localTime.tm_sec << "] " << s << std::endl; \
		OutputDebugStringW( oStringStream.str().c_str() );  \
	}
#endif


// wide char version of compiler macros
#ifndef WFILE
#	define W_FILE_MACRO_EXPANSION2(f) L##f
#	define W_FILE_MACRO_EXPANSION1(f) W_FILE_MACRO_EXPANSION2(f)
#	define WFILE W_FILE_MACRO_EXPANSION1(__FILE__)
#endif

#ifndef WDATE
#	define W_DATE_MACRO_EXPANSION2(f) L##f
#	define W_DATE_MACRO_EXPANSION1(f) W_DATE_MACRO_EXPANSION2(f)
#	define WDATE W_DATE_MACRO_EXPANSION1(__DATE__)
#endif

#ifndef WTIME
#	define W_TIME_MACRO_EXPANSION2(f) L##f
#	define W_TIME_MACRO_EXPANSION1(f) W_TIME_MACRO_EXPANSION2(f)
#	define WTIME W_TIME_MACRO_EXPANSION1(__TIME__)
#endif

#ifndef WFUNC
#	define W_FUNC_MACRO_EXPANSION2(f) L##f
#	define W_FUNC_MACRO_EXPANSION1(f) W_FUNC_MACRO_EXPANSION2(f)
#	define WFUNC W_FUNC_MACRO_EXPANSION1(__FUNCTION__)
#endif

