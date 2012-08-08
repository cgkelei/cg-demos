#ifndef InputModule_h__
#define InputModule_h__

#include "Core/IModule.h"

namespace RcEngine
{
	namespace Input
	{
		class _ApiExport InputModule : public IModule
		{
		public:
			InputModule();
			virtual ~InputModule();

			const String& GetName() const ;
			InputDevice* GetInputDevice();

			void Initialise();
			void Shutdown();

		private:
			InputDevice* mInputDevice;
		};
	}
}


#endif // InputModule_h__