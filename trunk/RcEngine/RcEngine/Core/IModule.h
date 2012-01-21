#ifndef IModule_h__
#define IModule_h__

#include "Core/Prerequisites.h"

namespace RcEngine
{
	namespace Core
	{
		class _ApiExport IModule
		{
		protected:
			IModule(void);

		public:
			virtual ~IModule(void);

			virtual const std::string& GetName() const = 0;	
			virtual void Initialise() = 0;
			virtual void Shutdown() = 0;

		};
	}
}



#endif // IModule_h__