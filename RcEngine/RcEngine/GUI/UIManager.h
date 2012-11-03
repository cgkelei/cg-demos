#ifndef UIManager_h__
#define UIManager_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>

namespace RcEngine {

class UIElement;

class UIManager : public Singleton<UIManager>
{
public:
	SINGLETON_DECL_HEADER(UIManager);

public:
	UIManager();
	~UIManager();


	void SetFocusElement(UIElement* element);

	UIElement* GetFocusElement()		{ return mFocusElement; }
	
public:
	void HandleKeyDown(uint8_t key, uint32_t qualifiers);
	void HandleKeyUp(uint8_t key, uint32_t qualifiers);


protected:
	
	UIElement* mFocusElement;
	UIElement* mRootElement;
};

}


#endif // UIManager_h__
