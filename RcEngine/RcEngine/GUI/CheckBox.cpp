#include <GUI/CheckBox.h>
#include <Input/InputSystem.h>

namespace RcEngine {

CheckBox::CheckBox( const int2& pos, const std::wstring& text, bool checked )
{
	mCheched = checked;
	mPressed = false;
}

CheckBox::~CheckBox()
{

}

void CheckBox::Update( float delta )
{
	if (!mHovering && mPressed)
		mPressed = false;
}

void CheckBox::Draw( SpriteBatch& spriteBatch )
{

}

void CheckBox::SetChecked( bool enalbe )
{
	if (mCheched != enalbe)
	{
		mCheched = enalbe;

		if (!EventStateChange.empty())
			EventStateChange(mCheched);
	}
}

bool CheckBox::IsChecked() const
{
	return mCheched;
}

bool CheckBox::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

bool CheckBox::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		if (mPressed)
		{
			mPressed = false;

			if (IsInside(screenPos, true))
			{
				SetChecked(!mCheched);
			}	
			else
				mHovering = false;

			return true;
		}	
	}

	return false;
}

bool CheckBox::OnMouseButtonPress( const int2& position, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		mPressed = true;
		mHovering = true;

		return true;
	}

	return false;
}

}

