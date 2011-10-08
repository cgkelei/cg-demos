#pragma once

#include "Prerequisites.h"
#include "Singleton.h"

namespace Engine
{
	class _ApiExport Plugin
	{
	public:
		Plugin(void) {};
		virtual ~Plugin(void) {};

		/** 唯一标示插件，可根据名字查询该插件
		*/
		virtual const String& GetName() const = 0;

		virtual void Install() = 0;

		virtual void Initialise() = 0;

		virtual void Shutdown() = 0;

		virtual void Uninstall() = 0;
	};

}

