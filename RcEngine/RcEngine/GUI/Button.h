#ifndef Button_h__
#define Button_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport Button : public UIElement
{
public:
	// Button clicked event delegate
	typedef fastdelegate::FastDelegate0<> ButtonClickedEventHandler;

public:
	Button();
	virtual ~ Button();

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual bool CanHaveFocus() const;

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	void SetPressedOffset(const int2& offset)	{ mPressedOffset; }
	const int2& GetPressedOffset() const        { return mPressedOffset; }

	void SetText(const std::wstring& txt)		{ mText = txt; }
	const std::wstring& GetText() const         { return mText; }

private:

	void SetPressed(bool pressed);

public:
	ButtonClickedEventHandler EventButtonClicked;

protected:

	bool mPressed;

	int2 mPressedOffset;
	int2 mHoverOffset;

	std::wstring mText;

};


}

#endif // Button_h__
