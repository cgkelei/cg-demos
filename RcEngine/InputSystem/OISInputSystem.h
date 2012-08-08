#include "OISInputPrerequisites.h"
#include "Input/InputDevice.h"

namespace RcEngine
{
	namespace Input
	{
		class _InputExport OISInputSystem : public InputSystem
		{
		public:
			OISInputSystem();
			~OISInputSystem();

			virtual const String& Name() const ;
			OIS::InputManager* GetInputManager() const	{ return mInputManager; }

		private:
			OIS::InputManager* mInputManager;
		};
	}
}