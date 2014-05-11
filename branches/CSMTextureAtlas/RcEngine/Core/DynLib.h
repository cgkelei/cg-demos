#ifndef DynLib_h__
#define DynLib_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

typedef void (*DLL_START_PLUGIN)(IModule** pModule);

enum ModuleType
{
	MT_Unknown = -1,
	MT_Render_OpengGL = 0,
	MT_Render_D3D10,
	MT_Input_OIS,
	MT_Count
};

//! Module names for plugin import
#ifdef WIN32
#  ifdef _DEBUG
const std::string ModuleNames[MT_Count] =
{
	"OpenGLRenderSystem_d",
	"D3D10RenderSystem_d",
	"InputSystem_d"
};
#else
const std::string ModuleNames[MT_Count] =
{
	"OpenGLRenderSystem",
	"D3D10RenderSystem",
	"InputSystem"
};
#endif
#endif

class DynLib
{
public:
	DynLib(const std::string& name);
	~DynLib();

	void Load();
	void Unload();

	const String& GetName() const { return mName; }
	void* GetSymbol( const String& strName ) const throw();

private:
	String mName;
	HINSTANCE m_hInst;

	std::string DynLibError();
};


} // Namespace RcEngine

#endif // DynLib_h__