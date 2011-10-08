#pragma once
#ifndef _DynLibManager__H
#define _DynLibManager__H


#include "Prerequisites.h"
#include "Singleton.h"

namespace Engine
{
	class _ApiExport DynLib
	{
	private:
		String mName;
		HINSTANCE m_hInst;

		String DynLibError();

	public:
		DynLib(const String& name);
		~DynLib();

		void Load();
		void Unload();

		const String& GetName() const { return mName; }
		void* GetSymbol( const String& strName ) const throw();
	};


	class _ApiExport DynLibManager : public Singleton<DynLibManager>
	{
	public:

		SINGLETON_DECL_HEADER(DynLibManager)

	protected:
		typedef std::map<String, DynLib*> DynLibList;

#pragma warning(push)
#pragma warning (disable : 4251)

		DynLibList mDynLibList;

#pragma warning(pop)

	public:
		DynLibManager(void);
		~DynLibManager(void);

		DynLib* Load(const String& name);
		void Unload(DynLib* lib);

	};
}


#endif