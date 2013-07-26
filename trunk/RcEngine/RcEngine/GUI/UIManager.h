#ifndef UIManager_h__
#define UIManager_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>
#include <GUI/UIElement.h>

namespace RcEngine {

class UIElement;
class Cursor;
union InputEvent;

class _ApiExport UIManager : public Singleton<UIManager>
{
public:
	SINGLETON_DECL_HEADER(UIManager);

	enum UIEvent
	{
		KeyUpDown,
		MouseUpDown,
		MouseMove,
		MouseWheel,
	};

public:
	UIManager();
	~UIManager();

	void SetFocusElement(UIElement* element);
	UIElement* GetFocusElement()						{ return mFocusElement; }	


	void Update(float delta);


	/** 
	 * Handle GUI Event, if event is handled by GUI, no need to pass to other game object.
	 * return true if the event is consumed, false if need to pass to others.
	 */
	bool OnEvent(const InputEvent& event);
	
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
