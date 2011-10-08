#include "LogManager.h"
#include "Exception.h"

namespace Engine
{
	axter::severity MappingEZLoger(LoggingSeverity severity)
	{
		switch(severity)
		{
		case LS_NOSEVERITY:
			return axter::no_severity;
		case LS_DEBUG:
			return axter::debug;
		case LS_INFO:
			return axter::info;
		case LS_STATUS:
			return axter::status;
		case LS_WARNING:
			return axter::warn;
		case LS_ERROR:
			return axter::error;
		case LS_FATAL:
			return axter::fatal;
		}
		throw Exception(Exception::ERR_NOT_IMPLEMENTED, "MappingEZLoger", "Mapping To EZLoger Error");
	}

	axter::verbosity MappingEZLoger(LoggingVerbosity verbosity)
	{
		switch(verbosity)
		{
		case LV_DEFAULT:
			return axter::log_default_verbosity_level;
		case LV_NOTSET:
			return axter::log_verbosity_not_set;
		case LV_ALWAYS:
			return axter::log_always;
		case LV_OFTEN:
			return axter::log_often;
		case LV_REGULARLY:
			return  axter::log_regularly;
		case LV_RARELY:
			return  axter::log_rarely;
		case LV_VERY_RARELY:
			return axter::log_very_rarely;
		}
		throw Exception(Exception::ERR_NOT_IMPLEMENTED, "MappingEZLoger", "Mapping To EZLoger Error");
	}

	SINGLETON_DECL(LogManager)

	LogManager::LogManager()
	{
		mDefaultSeverity = LS_NOSEVERITY;
	}

	LogManager::~LogManager()
	{
	}

	void LogManager::LogMessage( const char* file, int line, const char* funname, const String& message, LoggingVerbosity verbosity /*= LV_DEFAULT */)
	{
		//Complex extended data example
		axter::ezlogger<>(file, line, funname, axter::levels(MappingEZLoger(verbosity), MappingEZLoger(mDefaultSeverity)), true) << message << std::endl;
	}


	void LogManager::SetLogSeverity( LoggingSeverity severity )
	{
		mDefaultSeverity = severity;
	}

	void LogManager::SetGlobalLogVerbosity( LoggingVerbosity verbosity )
	{
		axter::ezlogger<>::set_verbosity_level_tolerance(MappingEZLoger(verbosity));
	}
}