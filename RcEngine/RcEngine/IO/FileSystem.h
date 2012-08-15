#ifndef StreamWriter_h__
#define StreamWriter_h__

#include "Core/Prerequisites.h"
#include "Core/Singleton.h"

namespace RcEngine {

class _ApiExport FileSystem : public Singleton<FileSystem>  
{
public:
	SINGLETON_DECL_HEADER(FileSystem);

public:
	FileSystem();
	virtual ~FileSystem();

};

} //Namespace RcEngine

#endif // StreamWriter_h__