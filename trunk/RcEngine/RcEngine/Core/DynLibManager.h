#ifndef _DynLibManager__H
#define _DynLibManager__H

#include "Prerequisites.h"
#include "Singleton.h"

namespace RcEngine{
namespace Core{

class DynLib
{
public:
	DynLib(const std::string& name);
	~DynLib();

	void Load();
	void Unload();

	const std::string& GetName() const { return mName; }
	void* GetSymbol( const std::string& strName ) const throw();

private:
	std::string mName;
	HINSTANCE m_hInst;

	std::string DynLibError();

};


class DynLibManager : public Singleton<DynLibManager>
{
public:
	SINGLETON_DECL_HEADER(DynLibManager)

public:
	DynLibManager(void);
	~DynLibManager(void);

	DynLib* Load(const std::string& name);
	void Unload(DynLib* lib);

protected:
	typedef std::map<std::string, DynLib*> DynLibList;
	DynLibList mDynLibList;

};


} // Namespace Core
} // Namespace RcEngine

#endif