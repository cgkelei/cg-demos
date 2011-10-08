#pragma once
#ifndef _LogManager__H
#define _LogManager__H

#include "Prerequisites.h"
#include "Singleton.h"

#pragma warning (push)
#pragma warning (disable : 4996)
#include "ezlogger/ezlogger_headers.hpp"
#pragma warning (pop)


#define LogMessageStream EZLOGGERSTREAM

//namespace Engine
//{
//
//	enum LoggingSeverity 
//	{
//		LS_NOSEVERITY,
//		LS_DEBUG,
//		LS_INFO,
//		LS_STATUS,
//		LS_WARNING,
//		LS_ERROR,
//		LS_FATAL
//	};
//
//	enum LoggingVerbosity 
//	{
//		LV_DEFAULT,
//		LV_NOTSET,
//		LV_ALWAYS ,
//		LV_OFTEN,
//		LV_REGULARLY,
//		LV_RARELY,
//		LV_VERY_RARELY 
//	};
//
//	class _ApiExport LogManager : public Singleton<LogManager>
//	{
//	public:
//		SINGLETON_DECL_HEADER(LogManager)	
//
//	protected:
//		
//		LoggingSeverity mDefaultSeverity;
//
//	 public:
//		 LogManager();
//		~LogManager();
//		
//		void LogMessage( const char* file, int line, const char* funname, const String& message, LoggingVerbosity verbosity = LV_DEFAULT );
//
//		void SetLogSeverity( LoggingSeverity severity );
//
//		/** Set global verbosity£¬only those LogMessage
//			verbosity less than global verbosity will be logged
//		*/
//		void SetGlobalLogVerbosity( LoggingVerbosity verbosity );
//	 
//	};
//
//
//#define LogMessages(message) LogManager::GetSingleton().LogMessage(__FILE__, __LINE__, __FUNCTION__, message )
//}
//
//

#endif // _LogManager__H