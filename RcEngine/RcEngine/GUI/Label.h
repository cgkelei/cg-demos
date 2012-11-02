#ifndef Label_h__
#define Label_h__

#include <Core/Prerequisites.h>
#include <GUI/UIElement.h>

namespace RcEngine {

class Font;

class _ApiExport Label : public UIElement
{
public:
	Label();
	~Label();

	
	void SetFont(const shared_ptr<Font>& font);
	void SetText(const String& text);
	void SetWordWrap(bool enable);
	void SetTextAlignment(HorizontalAlignment align);

	/**
	 * Selects text from position start and for length characters.
	 */
	void SetSelection(uint32_t start, uint32_t length);


	float GetRowSpacing() const						{ return mRowSpacing; }
	bool GetWordWrap() const						{ return mWordWrap ; }
	const String& GetText() const					{ return mText; }
	const shared_ptr<Font> GetFont() const			{ return mFont; }
	uint32_t GetRowHeight() const					{ return mRowHeight; }
	uint32_t GetNumRows() const						{ return mRowWidths.size(); }
	const vector<uint32_t>& GetRowWidths() const	{ return mRowWidths; }
	const ColorRGBA& GetSelectionColor() const		{ return mSelectionColor; }
	const ColorRGBA& GetHoverColor() const			{ return mHoverColor; }

protected:
	void UpdateText();
	void ValidateSelection();

protected:
	shared_ptr<Font> mFont;

	String mText;

	vector<uint32_t> mRowWidths;
	uint32_t mRowHeight;
	float mRowSpacing;

	bool mWordWrap;

	HorizontalAlignment mTextAlignment;

	// Selection background color.
	ColorRGBA mSelectionColor;

	// Hover background color.
	ColorRGBA mHoverColor;


};


}



#endif // Label_h__
