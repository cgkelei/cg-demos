#include "OISInputPrerequisites.h"
#include "Input/InputDevice.h"

namespace RcEngine
{
	namespace Input
	{
		class _InputExport OISInputMouse : public Mouse
		{
		public:
			OISInputMouse(InputSystem* inputSystem, uint32_t width, uint32_t height);
			~OISInputMouse();

			virtual const String& GetName() const;
			virtual void Update(float delta);

			virtual long X() const;
			virtual long Y() const;
			virtual bool ButtonDown(MouseCode button) const;

		private:
			OIS::Mouse* mMouse;
		};
	}
}