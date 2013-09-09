#include <GUI/RadioButton.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>
#include <Graphics/Font.h>
#include <Input/InputSystem.h>

namespace RcEngine {

const String RadioButton::RadioStyleName("RadioButton");

RadioButton::RadioButton()
	: mChecked(false),
	  mStyle(nullptr),
	  mPressed(mPressed),
	  mCanUncheck(true)
{

}

RadioButton::~RadioButton()
{

}

void RadioButton::InitGuiStyle( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if (styles)
	{
		GuiSkin::StyleMap::const_iterator iter;
		iter = styles->find(RadioStyleName);
		mStyle = iter->second;
	}
	else
	{
		GuiSkin* defaultSkin = UIManager::GetSingleton().GetDefaultSkin();
		mStyle = &defaultSkin->RadioButton;
	}

	mSize.X() = mStyle->StyleStates[UI_State_Normal].TexRegion.Width;
	mSize.Y() = mStyle->StyleStates[UI_State_Normal].TexRegion.Height;
}

void RadioButton::Update( float dt )
{
	if (!mHovering && mPressed)
		mPressed = false;
}

void RadioButton::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	float zOrder = GetDepthLayer();

	int2 screenPos = GetScreenPosition();

	Rectanglef destRect;

	destRect.X = (float)screenPos.X();
	destRect.Y = (float)screenPos.Y();
	destRect.Width = destRect.Height = float(mSize.Y());

	if (mChecked)
	{
		spriteBatch.Draw(mStyle->StyleTex, destRect, &mStyle->StyleStates[UI_State_Pressed].TexRegion, mStyle->StyleStates[UI_State_Pressed].TexColor, zOrder);
	}
	else
	{
		spriteBatch.Draw(mStyle->StyleTex, destRect, &mStyle->StyleStates[UI_State_Normal].TexRegion, mStyle->StyleStates[UI_State_Normal].TexColor, zOrder);
	}

	// Text
	destRect.SetLeft( destRect.Right() + 0.25f * destRect.Width );
	destRect.SetRight( float(screenPos.X() + mSize.X()) );

	destRect.Y = (float)screenPos.Y();
	destRect.Height = float(mSize.Y());

	mStyle->Font->DrawString(spriteBatchFont, mText, float(mSize.Y() * 1.2f), AlignLeft | AlignVCenter, destRect, mStyle->ForeColor, zOrder);

	// Reset hovering for next frame
	mHovering = false;
}

void RadioButton::SetText( const std::wstring& text )
{
	mText = text;
}

void RadioButton::Toggle()
{
	if (mChecked && mCanUncheck)
		mChecked = false;
	else if (!mChecked)
		mChecked = true;
}

bool RadioButton::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		mPressed = true;
		mHovering = true;

		return true;
	}

	return false;
}

bool RadioButton::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	if (button == MS_LeftButton)
	{
		if (mPressed)
		{
			mPressed = false;

			if (IsInside(screenPos, true))
			{
				Toggle();
			}	
			else
				mHovering = false;

			return true;
		}	
	}

	return false;
}

bool RadioButton::CanHaveFocus() const
{
	return mVisible && mEnabled;
}

//----------------------------------------------------------------------------------------------
RadioButtonGroup::RadioButtonGroup()
{

}

RadioButtonGroup::~RadioButtonGroup()
{

}

RadioButton* RadioButtonGroup::CheckedButton() const
{

}

int32_t RadioButtonGroup::CheckedIndex() const
{

}


}