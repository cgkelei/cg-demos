#ifndef Button_h__
#define Button_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport Button : public UIElement
{
public:
	/**
	 * Button clicked event.
	 */
	typedef fastdelegate::FastDelegate0<> ButtonClickedEventHandler;
	ButtonClickedEventHandler EventButtonClicked;

public:
	Button();
	virtual ~ Button();

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual bool CanHaveFocus() const;

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual void SetGuiStyle(UIElementState uiState, const GuiSkin::SytleImage& styleImage);

	void SetPressedOffset(const int2& offset)	{ mPressedOffset = offset; }
	const int2& GetPressedOffset() const        { return mPressedOffset; }

	void SetHoverOffset(const int2& offset)		{ mHoverOffset = offset; }
	const int2& GetHoverOffset() const			{ return mHoverOffset; }

	void SetText(const std::wstring& txt)		{ mText = txt; }
	const std::wstring& GetText() const         { return mText; }

private:

	void SetPressed(bool pressed);

protected:

	bool mPressed;

	int2 mPressedOffset;
	int2 mHoverOffset;

	std::wstring mText;

	GuiSkin::GuiStyle mGuiStyle;
};


}

#endif // Button_h__
