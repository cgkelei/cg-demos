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
	virtual ~Label();

	virtual void InitGuiStyle(const GuiSkin::StyleMap* styles = nullptr);
	virtual void OnResize();
	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	void SetFont(const shared_ptr<Font>& font, float fontSize);
	const shared_ptr<Font>& GetFont() const			{ return mFont; }

	void SetText(const std::wstring& text );
	const std::wstring& GetText() const				{ return mText; }

	void SetWordWrap(bool enable);
	bool GetWordWrap() const						{ return mWordWrap ; }
	
	void SetTextAlignment(uint32_t align);
	uint32_t GetTextAlignment() const	{ return mTextAlignment; }

	
protected:
	void UpdateText();

protected:
	shared_ptr<Font> mFont;

	std::wstring mText;
	std::wstring mPrintText;

	uint32_t mTextAlignment;

	vector<float> mRowWidths;

	float mFontSize;

	bool mWordWrap;	
};


}



#endif // Label_h__
