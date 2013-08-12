#ifndef Label_h__
#define Label_h__

#include <Core/Prerequisites.h>
#include <GUI/UIElement.h>

namespace RcEngine {

/**
 * A text label, label is a non focusable control, no input event
 */
class _ApiExport Label : public UIElement
{
public:
	Label();
	Label(const int2& pos, const std::wstring& text, Alignment textAlign = AlignCenter);
	virtual ~Label();
	
	void SetFont(const shared_ptr<Font>& font, int32_t fontSize);
	const shared_ptr<Font> GetFont() const			{ return mFont; }

	void SetText(const std::wstring& text, Alignment textAlign = AlignCenter);
	const std::wstring& GetText() const				{ return mText; }

	void SetWordWrap(bool enable);
	bool GetWordWrap() const						{ return mWordWrap ; }
	
	void SetTextAlignment(Alignment align);
	Alignment GetTextAlignment() const	{ return mTextAlignment; }

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);
	virtual void OnResize();

protected:
	void UpdateText();

protected:
	shared_ptr<Font> mFont;

	std::wstring mText;
	std::wstring mPrintText;

	Alignment mTextAlignment;

	vector<float> mRowWidths;

	int32_t mFontSize;

	bool mWordWrap;	
};


}



#endif // Label_h__
