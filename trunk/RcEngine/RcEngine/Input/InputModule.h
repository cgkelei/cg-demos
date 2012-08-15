#ifndef InputModule_h__
#define InputModule_h__

#include "Core/IModule.h"

namespace RcEngine{
namespace Input{
		
class _ApiExport InputModule : public Core::IModule
{
protected:
	InputModule();

public:
	virtual ~InputModule();
	virtual InputSystem* GetInputSystem() = 0;
};

} // Namespace Input 
} // Namespace RcEngine


#endif // InputModule_h__