#include <GUI/Button.h>
#include <Input/InputSystem.h>

namespace RcEngine {

Button::Button()
{

}

Button::~Button()
{

}

void Button::OnClick( const int2& position, const int2& screenPosition, uint32_t buttons, int qualifiers )
{
	if (buttons & MS_LeftButton)
	{
		mPressed = true;
		mHovering = true;

		if (!EventButtonClicked.empty())
			EventButtonClicked();
	}
}

void Button::Update( float delta )
{
	if (!mHovering && mPressed)
	{
		mPressed = false;
	}


}




}
