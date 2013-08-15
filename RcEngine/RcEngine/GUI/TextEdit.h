#ifndef UITextEdit_h__
#define UITextEdit_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport TextEdit : public UIElement
{
public:
	TextEdit();
	virtual ~TextEdit();

	virtual void Update();
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	virtual void Initialize(const GuiSkin::StyleMap* styles /* = nullptr */);

	virtual void OnDragBegin(const int2& position, uint32_t buttons);
	virtual void OnDragMove(const int2& position, uint32_t buttons);
	virtual void OnDragEnd(const int2& position);
	virtual bool OnTextInput(uint16_t unicode);

	void SetText(const String& text);
	const std::wstring& GetText() const				{ return mText; }

	void SetWordWrap(bool enable);
	void SetTextAlignment(Alignment align);

	float GetRowSpacing() const						{ return mRowSpacing; }
	bool GetWordWrap() const						{ return mWordWrap ; }

	uint32_t GetRowHeight() const					{ return mRowHeight; }
	const vector<uint32_t>& GetRowWidths() const	{ return mRowWidths; }

	uint32_t GetNumRows() const						{ return mRowWidths.size(); }

protected:

	int32_t GetRowStartPos(int32_t rowIdx) const;

protected:

	uint32_t mSelectStart, mSelectLength;
	
	std::wstring mText;

	shared_ptr<Font> mFont;

	vector<uint32_t> mRowWidths;
	uint32_t mRowHeight;
	float mRowSpacing;

	bool mWordWrap;

	Alignment mTextAlignment;
};


}


#endif // UITextEdit_h__
