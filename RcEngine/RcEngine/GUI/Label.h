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
	
	void SetFont(const shared_ptr<Font>& font, int32_t fontSize);
	const shared_ptr<Font> GetFont() const			{ return mFont; }

	void SetText(const std::wstring& text);
	const std::wstring& GetText() const				{ return mText; }

	void SetWordWrap(bool enable);
	bool GetWorldWrap() const						{ return mWordWrap; }

	float GetRowSpacing() const						{ return mRowSpacing; }
	bool GetWordWrap() const						{ return mWordWrap ; }
	
	void SetTextAlignment(HorizontalAlignment align);
	HorizontalAlignment GetTextAlignment() const	{ return mTextAlignment; }

	int32_t GetRowHeight() const					{ return mRowHeight; }
	const vector<int32_t>& GetRowWidths() const		{ return mRowWidths; }

	int32_t GetNumRows() const						{ return mRowWidths.size(); }


	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch);

protected:
	void UpdateText();

protected:
	shared_ptr<Font> mFont;

	std::wstring mText;

	vector<int32_t> mRowWidths;

	int32_t mFontSize;
	int32_t mRowHeight;

	float mRowSpacing;

	bool mWordWrap;

	HorizontalAlignment mTextAlignment;
};


}



#endif // Label_h__
