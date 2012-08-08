#include "OISInputPrerequisites.h"
#include "Input/InputDevice.h"

namespace RcEngine
{
	namespace Input
	{
		class _InputExport OISInputKeyboard : public Keyboard
		{
		public:
			OISInputKeyboard(InputSystem* inputSystem);
			~OISInputKeyboard();

			virtual bool IsKeyDown(KeyCode key) const;
			virtual const String& GetName() const ;
			virtual void Update(float delta) ;

		private:
			OIS::Keyboard* mKeyboard;
		};
	}
}