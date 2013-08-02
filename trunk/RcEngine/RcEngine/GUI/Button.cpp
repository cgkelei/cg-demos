#include <GUI/Button.h>
#include <Input/InputSystem.h>

namespace RcEngine {

Button::Button()
	: mPressedOffset(int2::Zero()),
	  mHoverOffset(int2::Zero())
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

void Button::Draw( SpriteBatch& spriteBatch )
{
	int2 offset = int2::Zero();

	if (mHovering)
		offset += mHoverOffset;

	if (mPressed)
		offset += mPressedOffset;	

	// Reset hovering for next frame
	mHovering = false;
}




}
