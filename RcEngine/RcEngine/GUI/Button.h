#ifndef Button_h__
#define Button_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport Button : UIElement
{
public:
	typedef fastdelegate::FastDelegate0<> ButtonClickedEventHandler;

public:
	Button();
	virtual ~ Button();

	virtual bool OnMouseButtonPress(const int2& position, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& position, uint32_t button);

	virtual void OnResize();
	virtual bool CanHaveFocus() const;

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch);

	void SetPressedOffset(const int2& offset)	{ mPressedOffset; }
	const int2& GetPressedOffset() const        { return mPressedOffset; }

private:

	void SetPressed(bool pressed);

public:
	ButtonClickedEventHandler EventButtonClicked;


protected:

	bool mPressed;

	int2 mPressedOffset;

};


}

#endif // Button_h__
