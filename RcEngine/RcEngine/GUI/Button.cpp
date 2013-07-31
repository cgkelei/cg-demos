#include <GUI/Button.h>
#include <Input/InputSystem.h>

namespace RcEngine {

Button::Button()
{

}

Button::~Button()
{

}

void Button::Update( float delta )
{
	if (!mHovering && mPressed)
	{
		SetPressed(false);
	}


}

void Button::OnResize()
{

}

void Button::SetPressed( bool pressed )
{
	mPressed = pressed;
}

bool Button::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

bool Button::OnMouseButtonPress( const int2& position, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		SetPressed(true);
		mHovering = true;

		return true;
	}

	return false;
}

bool Button::OnMouseButtonRelease( const int2& position, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		if (mPressed)
		{
			SetPressed(false);

			if (IsInside(position, false))
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




}
