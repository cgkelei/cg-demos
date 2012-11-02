#ifndef UIManager_h__
#define UIManager_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>

namespace RcEngine {


class UIManager : public Singleton<UIManager>
{
public:
	SINGLETON_DECL_HEADER(UIManager);

public:
	UIManager();
	~UIManager();


private:


};

}


#endif // UIManager_h__
