#ifndef UITextEdit_h__
#define UITextEdit_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport TextEdit : public UIElement
{
public:
	typedef fastdelegate::FastDelegate1<std::wstring> ReturnEventHandler;
	ReturnEventHandler EventReturnPressed;

public:
	TextEdit();
	virtual ~TextEdit();

	virtual void Update();
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual void Initialize(const GuiSkin::StyleMap* styles /* = nullptr */);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual void OnDragBegin(const int2& position, uint32_t buttons);
	virtual void OnDragMove(const int2& position, uint32_t buttons);
	virtual void OnDragEnd(const int2& position);

	virtual bool OnKeyPress(uint16_t key);
	virtual bool OnTextInput(uint16_t unicode);
	
	virtual bool CanHaveFocus() const;

	void SetText(const String& text);
	const std::wstring& GetText() const				    { return mText; }

	void SetBorderWidth(int32_t border)					{ mBorder = border; UpdateRect(); }

	void SetTextColor(const ColorRGBA& color)			{ mTextColor = color; }
	void SetSelectedTextColor(const ColorRGBA& color)	{ mSelTextColor = color; }

	void SetTextAlignment(Alignment align);

protected:

	int32_t GetRowStartPos(int32_t rowIdx) const;

	int32_t GetCharPos(const int2& screenPos) const;

	void DeleteSlectedText();

	void PlaceCaret(int32_t charPos);

	void UpdateRect();
	void UpdateText();

protected:

	bool mMultiLine;

	std::wstring mText;
	
	IntRect mTextRect;

	std::vector<int2> mCharPositions;

	int32_t mBorder;

	int32_t mCaretPos;       // Caret position, in characters
	int32_t mSelectStart, mSelectLength;
	
	ColorRGBA mTextColor;
	ColorRGBA mSelTextColor;
	ColorRGBA mSelBkColor;
	ColorRGBA mCaretColor;

	bool mDragMouse;

	// Caret blink rate.
	float cursorBlinkRate_;
	// Caret blink timer.
	float cursorBlinkTimer_;

	Alignment mTextAlignment;

	GuiSkin::GuiStyle* mStyle;
};


}


#endif // UITextEdit_h__
