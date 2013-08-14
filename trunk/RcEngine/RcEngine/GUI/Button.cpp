#include <GUI/Button.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputSystem.h>

namespace RcEngine {

const String Button::StyleName("Button Style");

Button::Button()
	: mPressedOffset(1, 2),
	  mHoverOffset(-1, -2)
{

}

Button::~Button()
{

}

void Button::Update( float delta )
{
	if (!mHovering && mPressed)
		SetPressed(false);
}

void Button::SetPressed( bool pressed )
{
	mPressed = pressed;
}

bool Button::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

bool Button::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		SetPressed(true);
		mHovering = true;

		return true;
	}

	return false;
}

bool Button::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		if (mPressed)
		{
			SetPressed(false);

			if (IsInside(screenPos, true))
			{
				if (!EventButtonClicked.empty())
					EventButtonClicked();
			}	
			else
				mHovering = false;

			return true;
		}
	}

	return false;
}

void Button::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	UIElementState uiState = UI_State_Normal;
	
	float offsetX = 0.0f;
	float offsetY = 0.0f;

	if (mVisible == false)
		uiState = UI_State_Hidden;
	else if (mEnabled == false)
		uiState = UI_State_Disable;
	else if (mPressed)
	{
		uiState = UI_State_Pressed;

		offsetX += mPressedOffset.X();
		offsetY += mPressedOffset.Y();
	}
	else if (mHovering)
	{
		uiState = UI_State_Hover;

		offsetX += mHoverOffset.X();
		offsetY += mHoverOffset.Y();
	}
	else if ( HasFocus() )
		uiState = UI_State_Focus;
	
	int2 screenPos = GetScreenPosition();

	Rectanglef btnRegion(screenPos.X() + offsetX, screenPos.Y() + offsetY, (float)mSize.X(), (float)mSize.Y());
	spriteBatch.Draw(mStyle->StyleTex, btnRegion, &mStyle->StyleStates[uiState].TexRegion, mStyle->StyleStates[uiState].TexColor);

	if (mText.length())
		mStyle->StyleFont->DrawString(spriteBatchFont, mText, mStyle->StyleFontSize, AlignCenter, btnRegion, mStyle->ForeColor);

	// Reset hovering for next frame
	mHovering = false;
}


void Button::Initialize( const GuiSkin::StyleMap* styles )
{
	if (!styles)
	{
		// Use default
		GuiSkin* defalutSkin = UIManager::GetSingleton().GetDefaultSkin();
		mStyle = &defalutSkin->Button;
	}
	else
	{
		GuiSkin::StyleMap::const_iterator iter = styles->find(StyleName);
		mStyle = iter->second;
	}
}


}
