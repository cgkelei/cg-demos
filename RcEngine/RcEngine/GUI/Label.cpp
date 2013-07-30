#include <GUI/Label.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>

namespace RcEngine {

Label::Label()
	: mWordWrap(false)
{

}

Label::~Label()
{

}

void Label::SetFont( const shared_ptr<Font>& font, int32_t fontSize )
{
	mFont = font;
	mFontSize = fontSize;

	UpdateText();
}

void Label::UpdateText()
{
	uint32_t width = 0;
	uint32_t height = 0;

	if (mFont)
	{
		mRowHeight = mFont->GetRowHeight(mFontSize);

		if (mWordWrap)
		{
			int maxWidth = GetSize().X(); 

			for (auto iter = mText.begin(); iter != mText.end(); ++iter)
			{
				
			}
		}
		else
		{

		}
	}
}

void Label::SetWordWrap( bool enable )
{
	if (enable != mWordWrap)
	{
		mWordWrap = enable;
		UpdateText();
	}
}

void Label::Update( float delta )
{

}

void Label::Draw( SpriteBatch& spriteBatch )
{

}


}