#ifndef UIManager_h__
#define UIManager_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>
#include <GUI/UIElement.h>

namespace RcEngine {

class UIElement;
class Cursor;

class UIManager : public Singleton<UIManager>
{
public:
	SINGLETON_DECL_HEADER(UIManager);

public:
	UIManager();
	~UIManager();

	void SetFocusElement(UIElement* element);
	UIElement* GetFocusElement()						{ return mFocusElement; }	


	void Update(float delta);


	
private:
	void HandleKeyPress(uint8_t key, uint32_t qualifiers);
	void HandleKeyRelease(uint8_t key, uint32_t qualifiers);

	void HandleMousePress(const int2& pos, uint32_t buttons, int qualifiers);
	void HandleMouseRelease(const int2& pos, uint32_t buttons, int qualifiers);
	void HandleMouseMove(const int2& pos, uint32_t buttons, int qualifiers);
	void HandleMouseWheel(int32_t delta, uint32_t buttons, uint32_t qualifiers);

	UIElement* GetElementFromPoint(const int2& pos);
	void GetElementFromPoint(UIElement*& result, UIElement* current, const int2& pos);

	UIElement* GetFocusableElement(UIElement* element);


protected:
	
	UIElement* mFocusElement;
	UIElement* mRootElement;
	UIElement* mDragElement;

	Cursor* mCursor;
};

}


#endif // UIManager_h__
