#include <GUI/CheckBox.h>
#include <GUI/UIManager.h>
#include <Graphics/Font.h>
#include <Graphics/SpriteBatch.h>
#include <Input/InputSystem.h>

namespace RcEngine {

CheckBox::CheckBox( )
	: mCheched(false), mPressed(false), mStyle(nullptr)
{

}

CheckBox::~CheckBox()
{

}

void CheckBox::Update( float delta )
{
	if (!mHovering && mPressed)
		mPressed = false;
}

void CheckBox::SetChecked( bool enalbe )
{
	if (mCheched != enalbe)
	{
		mCheched = enalbe;

		if (!EventStateChanged.empty())
			EventStateChanged(mCheched);
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

void CheckBox::SetText( const std::wstring& text )
{
	mText = text;
}

void CheckBox::UpdateRect()
{
	const int2& screenPos = GetScreenPosition();

	mCheckRect.X = (float)screenPos.X();
	mCheckRect.Y = (float)screenPos.Y();
	mCheckRect.Width = mCheckRect.Height = float(mSize.Y());

	mTextRect.SetLeft( mCheckRect.Right() + 0.25f * mCheckRect.Width );
	mTextRect.SetRight( float(screenPos.X() + mSize.X()) );

	mTextRect.Y = (float)screenPos.Y();
	mTextRect.Height = float(mSize.Y());
}

void CheckBox::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	UIElementState uiState = UI_State_Normal;

	if (mVisible == false)
		uiState = UI_State_Hidden;
	else if (mEnabled == false)
		uiState = UI_State_Disable;
	else if (mPressed)
		uiState = UI_State_Pressed;
	else if (mHovering)
		uiState = UI_State_Hover;
	else if (HasFocus())
		uiState = UI_State_Focus;

	if (mCheched)
	{
		spriteBatch.Draw(mStyle->StyleTex, mCheckRect, &mStyle->StyleStates[UI_State_Normal].TexRegion, mStyle->BackColor);
		spriteBatch.Draw(mStyle->StyleTex, mCheckRect, &mStyle->StyleStates[UI_State_Pressed].TexRegion, mStyle->BackColor);
	}
	else
		spriteBatch.Draw(mStyle->StyleTex, mCheckRect, &mStyle->StyleStates[UI_State_Normal].TexRegion, mStyle->BackColor);

	mStyle->StyleFont->DrawString(spriteBatchFont, mText, mStyle->StyleFontSize, AlignLeft | AlignVCenter, mTextRect, mStyle->ForeColor);

	// Reset hovering for next frame
	mHovering = false;
}

void CheckBox::Initialize( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{
	if (!styles)
	{
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();
		mStyle = &defaultSkin->CheckBox;
	}
	else
	{
		GuiSkin::StyleMap::const_iterator iter = styles->find(String("CheckBox Style"));
		mStyle = iter->second;
	}
}


}

