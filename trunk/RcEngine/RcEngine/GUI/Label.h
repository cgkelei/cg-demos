#ifndef Label_h__
#define Label_h__

#include <Core/Prerequisites.h>
#include <GUI/UIElement.h>

namespace RcEngine {

class Font;

/**
 * A text label, label is a non focusable control, no input event
 */
class _ApiExport Label : public UIElement
{
public:
	Label();
	virtual ~Label();
	
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
	void UpdateText();

protected:
	shared_ptr<Font> mFont;

	std::wstring mText;

	vector<uint32_t> mRowWidths;
	uint32_t mRowHeight;
	float mRowSpacing;

	bool mWordWrap;

	HorizontalAlignment mTextAlignment;
};


}



#endif // Label_h__
