#include "DynLibManager.h"

namespace Engine
{

	SINGLETON_DECL(DynLibManager)

	DynLib::DynLib( const String& name )
		:mName(name), m_hInst(0)
	{
		
	}


	DynLib::~DynLib()
	{

	}



	void DynLib::Load()
	{
		String name = mName;
		if (name.substr(name.length() - 4, 4) != ".dll")
			name += ".dll";
		m_hInst = (HINSTANCE)LoadLibraryEx( name.c_str(), 0,  LOAD_WITH_ALTERED_SEARCH_PATH );
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


	void* DynLib::GetSymbol( const String& strName ) const throw()
	{
		return (void*)GetProcAddress(m_hInst, strName.c_str());
	}

	String DynLib::DynLibError()
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
		String ret = (char*)lpMsgBuf;
		// Free the buffer.
		LocalFree( lpMsgBuf );
		return ret;
	}


	DynLibManager::DynLibManager(void)
	{
	}

	DynLibManager::~DynLibManager(void)
	{
		for( DynLibList::iterator it = mDynLibList.begin(); it != mDynLibList.end(); ++it )
		{
			it->second->Unload();
			delete it->second;
		}

		mDynLibList.clear();
	}


	DynLib* DynLibManager::Load( const String& name )
	{
		DynLibList::iterator iter = mDynLibList.find(name);
		if (iter != mDynLibList.end())
		{
			return iter->second;
		}else
		{
			DynLib* newLib = new DynLib(name);
			newLib->Load();
			mDynLibList[name] = newLib;
			return newLib;
		}
		
		
	}


	void DynLibManager::Unload( DynLib* lib )
	{
		DynLibList::iterator iter = mDynLibList.find(lib->GetName());
		if (iter != mDynLibList.end())
		{
			mDynLibList.erase(iter);
		}
		lib->Unload();
		delete lib;
	}


}
