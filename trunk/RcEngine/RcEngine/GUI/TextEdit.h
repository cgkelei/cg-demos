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

	virtual void Update(float dt);
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

	void SetText(const std::wstring& text);
	const std::wstring& GetText() const				    { return mText; }

	void SetBorderWidth(int32_t border)					{ mBorder = border; UpdateRect(); }
	void SetMultiLine(bool enable)						{ mMultiLine = enable; }

	void SetTextColor(const ColorRGBA& color)			{ mTextColor = color; }
	void SetSelectedTextColor(const ColorRGBA& color)	{ mSelTextColor = color; }

protected:

	int32_t GetRowStartPos(int32_t rowIdx) const;

	void GetCharPos(const int2& screenPos, int32_t& rowIndex, int32_t& columnIndex);

	void DeleteSlectedText();

	void PlaceCaret(int32_t rowIndex, int32_t columnIndex);

	void UpdateRect();
	void UpdateText();

protected:

	bool mMultiLine;

	std::wstring mText;
	
	IntRect mTextRect;
	IntRect mBackRect;


	struct CaretPair
	{
		float Start;
		float Half;
	};
	std::vector< std::vector<CaretPair> > mCharPositions;

	int32_t mBorder;
	int32_t mRowHeight;

	int32_t mSelectStart, mSelectLength;
	
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

	int32_t mCaretRowIndex;
	int32_t mCaretColumnIndex;

	GuiSkin::GuiStyle* mTextEditStyle;
};


}


#endif // UITextEdit_h__
