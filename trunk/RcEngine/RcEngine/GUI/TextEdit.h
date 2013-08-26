#ifndef UITextEdit_h__
#define UITextEdit_h__

#include <GUI/UIElement.h>

namespace RcEngine {

/**
 * TextEdit: a single line or multiple line GUI control.
 *
 * Todo:  Add Word Wrap, ScrollBar.
 *
 */

class _ApiExport TextEdit : public UIElement
{
public:
	typedef fastdelegate::FastDelegate1<std::wstring> ReturnEventHandler;
	ReturnEventHandler EventReturnPressed;

public:
	TextEdit();
	virtual ~TextEdit();

	virtual void Update(float dt);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual void OnDragBegin(const int2& position, uint32_t buttons);
	virtual void OnDragMove(const int2& position, uint32_t buttons);
	virtual void OnDragEnd(const int2& position);

	virtual bool OnKeyPress(uint16_t key);
	virtual bool OnTextInput(uint16_t unicode);
	
	virtual bool CanHaveFocus() const;

	void SetText(const std::wstring& text);
	const std::wstring& GetText() const				    { return mText; }

	void SetBorderWidth(int32_t border)					{ mBorder = border; UpdateRect(); }
	void SetMultiLine(bool enable)						{ mMultiLine = enable; }

	void SetTextColor(const ColorRGBA& color)			{ mTextColor = color; }
	void SetSelectedTextColor(const ColorRGBA& color)	{ mSelTextColor = color; }

protected:

	int32_t GetRowStartPos(int32_t rowIdx) const;

	/**
	 * Calculate caret position from screen position.
	 */
	void GetCaretAt(const int2& screenPos, size_t& caretX, size_t& caretY);
	
	/**
	 * Calculate caret position from char index in original text.
	 */
	void GetCaretPos(size_t index, size_t& caretX, size_t& caretY);
	
	/**
	 * Calculate char index based on caret pos (x, y).
	 */
	size_t GetCharIndex(size_t caretX, size_t caretY);

	void DeleteSlectedText();
	void DeletePreChar();
	void DeleteNextChar();

	void PlaceCaret(size_t newCaretX, size_t newCaretY);

	void UpdateRect();
	void UpdateText();
	void WrapText();
	void UpdateVisisbleText();

	void HandleVScrollBar(int32_t value);
	
protected:

	bool mMultiLine;
	bool mWordWrap;

	std::wstring mText;
	std::wstring mWrappedText;
	std::wstring mPrintText;

	size_t mFirstVisibleLine;
	size_t mNumVisibleLines;
	size_t mNumLines;

	IntRect mTextRect;
	IntRect mBackRect;

	std::vector< std::vector<float> > mCharPositions;
	std::vector<int32_t> mWrappedBreakAt;

	int32_t mBorder;
	float mRowHeight;
	
	ColorRGBA mTextColor;
	ColorRGBA mSelTextColor;
	ColorRGBA mSelBkColor;
	ColorRGBA mCaretColor;

	ScrollBar* mVertScrollBar;
	
	int32_t mScrollBarWidth;

	bool mDragMouse;

	float mCaretBlinkRate;
	float mCaretBlinkTimer;
	bool mCaretOn;

	// Caret Position, measured in char index
	size_t mCaretY, mCaretX;
	size_t mSelectStartX, mSelectStartY;

	GuiSkin::GuiStyle* mTextEditStyle;
};


}


#endif // UITextEdit_h__
