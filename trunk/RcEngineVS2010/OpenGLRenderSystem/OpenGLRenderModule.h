#ifndef OpenGLRenderModule_h__
#define OpenGLRenderModule_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/IRenderModule.h>


namespace RcEngine {

class _OpenGLExport OpenGLRenderModule : public IRenderModule
{
public:
	OpenGLRenderModule(void);
	~OpenGLRenderModule(void);

	const std::string& GetName() const ;
	RenderDevice* GetRenderDevice();
	
	void Initialise();
	void Shutdown();
		

private:
	RenderDevice* mRenderDevice;
};

}


#endif // OpenGLRenderModule_h__