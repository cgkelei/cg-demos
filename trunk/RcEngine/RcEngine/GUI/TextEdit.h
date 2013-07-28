#ifndef UITextEdit_h__
#define UITextEdit_h__

#include <Core/Prerequisites.h>
#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport TextEdit : public UIElement
{
public:
	TextEdit();
	virtual ~TextEdit();

	virtual bool OnDragBegin(const int2& position, uint32_t buttons);
	virtual bool OnDragMove(const int2& position, uint32_t buttons);
	virtual bool OnDragEnd(const int2& position);

	virtual bool OnTextInput(uint16_t unicode);

	void SetFont(const shared_ptr<Font>& font);
	const shared_ptr<Font> GetFont() const			{ return mFont; }

	void SetText(const String& text);
	const std::wstring& GetText() const				{ return mText; }

	void SetWordWrap(bool enable);
	void SetTextAlignment(HorizontalAlignment align);

	float GetRowSpacing() const						{ return mRowSpacing; }
	bool GetWordWrap() const						{ return mWordWrap ; }

	uint32_t GetRowHeight() const					{ return mRowHeight; }
	const vector<uint32_t>& GetRowWidths() const	{ return mRowWidths; }

	uint32_t GetNumRows() const						{ return mRowWidths.size(); }

protected:

	uint32_t mSelectStart, mSelectLength;
	
	std::wstring mText;

	shared_ptr<Font> mFont;

	vector<uint32_t> mRowWidths;
	uint32_t mRowHeight;
	float mRowSpacing;

	bool mWordWrap;

	HorizontalAlignment mTextAlignment;
};


}


#endif // UITextEdit_h__
