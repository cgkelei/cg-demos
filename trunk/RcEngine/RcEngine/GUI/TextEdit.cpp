#include <GUI/TextEdit.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputEvent.h>
#include <Input/InputSystem.h>

namespace RcEngine {

TextEdit::TextEdit()
{
	mTextColor = ColorRGBA( 1.0f, 0.06, 0.06, 0.06 );
	mSelTextColor = ColorRGBA( 1.0f, 1.0f, 1.0f, 1.0f );
	mSelBkColor = ColorRGBA( 1.0f, 0.156f, 0.196f, 0.36f );
	mCaretColor = ColorRGBA( 1.0f, 0, 0, 0 );

	mCaretPos = mSelectStart = 0;

	mDragMouse = false;
	mBorder = 8;

}

TextEdit::~TextEdit()
{

}

bool TextEdit::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

void TextEdit::SetTextAlignment( Alignment align )
{

}

void TextEdit::Initialize( const GuiSkin::StyleMap* styles /* = nullptr */ )
{

}

void TextEdit::Update()
{

}

void TextEdit::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{

}

void TextEdit::OnDragBegin( const int2& position, uint32_t buttons )
{
	if (buttons & InputEventType::LeftButtonMask)
	{
		if (mDragMouse && IsInside(position, true))
		{

		}
	}
}

void TextEdit::OnDragMove( const int2& position, uint32_t buttons )
{

}

void TextEdit::OnDragEnd( const int2& position )
{

}

int32_t TextEdit::GetCharPos( const int2& screenPos ) const
{

}

bool TextEdit::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	mDragMouse = true;
}

bool TextEdit::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	mDragMouse = false;
}

void TextEdit::PlaceCaret( int32_t charPos )
{

}

void TextEdit::UpdateRect()
{
	UIElement::UpdateRect();

	int2 screenPos = GetScreenPosition();

	mTextRect.X = screenPos.X() + mBorder;
	mTextRect.Width = mSize.X() - mBorder - mBorder;

	mTextRect.Y = screenPos.Y() + mBorder;
	mTextRect.Height = mSize.Y() - mBorder - mBorder;
}

bool TextEdit::OnKeyPress( uint16_t key )
{
	switch (key)
	{
	case KC_LeftArrow:
		{

		}
		break;

	case KC_RightArrow:
		{

		}
		break;

	case KC_UpArrow:
		{

		}
		break;

	case KC_DownArrow:
		{

		}
		break;

	case KC_Delete:
		{

		}
		break;

	case KC_Enter:
		{
			if( mMultiLine )
			{

			}
			else
			{
				if (!EventReturnPressed.empty())
					EventReturnPressed(L"");
			}

		}
		break;
	
	case KC_Home:
		{
			
		}
		break;
	
	default:
		break;
	}

	return true;
}

bool TextEdit::OnTextInput( uint16_t unicode )
{
	bool eventConsumed = false;



	return eventConsumed;
}

void TextEdit::UpdateText()
{
	const float fontScale = (float)mStyle->FontSize / mStyle->Font->GetFontSize();

	for (size_t i = 0; i < mText.length(); ++i)
	{
		wchar_t ch = mText[i];


	}
}







}