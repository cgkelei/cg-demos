#include <Core/DynLib.h>
#include <Core/Utility.h>

namespace RcEngine {
namespace Core {

DynLib::DynLib( const std::string& name )
	:mName(name), m_hInst(0)
{

}


DynLib::~DynLib()
{

}



void DynLib::Load()
{
	std::string name = mName;
	if (name.substr(name.length() - 4, 4) != ".dll")
		name += ".dll";

	std::wstring wname;
	Convert(wname, name);
	m_hInst = (HINSTANCE)LoadLibraryEx(wname.c_str(), 0,  LOAD_WITH_ALTERED_SEARCH_PATH );
	if (!m_hInst)
	{
		std::cout << DynLibError() << std::endl;
		assert(false);
	}
}

void DynLib::Unload()
{
	if( !FreeLibrary(m_hInst) )
	{
		assert(false);
	}
}


void* DynLib::GetSymbol( const std::string& strName ) const throw()
{
	return (void*)GetProcAddress(m_hInst, strName.c_str());
}

std::string DynLib::DynLibError()
{
	LPVOID lpMsgBuf; 
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL, 
		GetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR) &lpMsgBuf, 
		0, 
		NULL 
		); 
	std::string ret = (char*)lpMsgBuf;
	// Free the buffer.
	LocalFree( lpMsgBuf );
	return ret;
}

} // Namespace Core
} // Namespace RcEngine