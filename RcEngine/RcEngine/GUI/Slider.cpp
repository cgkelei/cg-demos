#include <GUI/Slider.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>

namespace RcEngine {

Slider::Slider()
	: mMinimum(0), 
	mMaximum(100),
	mValue(0),
	mDragSlider(false),
	mThumbHovering(false)
{

}

Slider::~Slider()
{

}

void Slider::Update( float delta )
{
	if (mDragSlider)
	{
		mHovering = true;
		mThumbHovering = true;
	}
}

void Slider::OnResize()
{
	UpdateSlider();
}

void Slider::SetOrientation( UIOrientation orient )
{
	mOrientation = orient;
}

void Slider::UpdateSlider()
{
	int2 screenPos = GetScreenPosition();

	float ratio = float(mValue - mMinimum) / (mMaximum - mMinimum);

	if (mOrientation == UI_Horizontal)
	{		
		mThumbRegion.Width = mThumbRegion.Height = mSize.Y();
		mThumbRegion.X = int32_t( screenPos.X() + ratio * ( mSize.X() - mThumbRegion.Width ) );
		mThumbRegion.Y = screenPos.Y(); 
	}
	else
	{
		mThumbRegion.Width = mThumbRegion.Height = mSize.X();
		mThumbRegion.X = screenPos.X(); 
		mThumbRegion.Y = int32_t( screenPos.Y() + ratio * ( mSize.Y() - mThumbRegion.Height ) );
	}
}

void Slider::UpdateRect()
{
	UpdateSlider();
}

void Slider::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	if (!mGuiSkin)
		SetGuiSkin( UIManager::GetSingleton().GetDefaultSkin() );

	UIElementState uiThumbState = UI_State_Normal;

	// Thumb don't have focus state
	if (mVisible == false)
		uiThumbState = UI_State_Hidden;
	else if (mEnabled == false)
		uiThumbState = UI_State_Disable;
	else if (mDragSlider)
		uiThumbState = UI_State_Pressed;
	else if (mThumbHovering)
		uiThumbState = UI_State_Hover;
	else 
		uiThumbState = UI_State_Normal;

	int2 screenPos = GetScreenPosition();

	const GuiSkin::SytleImage& stateStyle = mGuiSkin->SliderThumb[mOrientation].StyleStates[uiThumbState];

	Rectanglef trackRegion, thumbRegion;

	// Draw order: track, thumb
	if (mOrientation == UI_Horizontal)
	{
		float midY = screenPos.Y() + mSize.Y() * 0.5f;

		trackRegion.X = (float)screenPos.X();
		trackRegion.Y = midY - mGuiSkin->HSliderTrack[0].TexRegion.Height * 0.5f;
		trackRegion.Width = (float)mSize.X();
		trackRegion.Height = (float)mGuiSkin->HSliderTrack[0].TexRegion.Height;	
	}
	else
	{
		float midX = screenPos.X() + mSize.X() * 0.5f;

		trackRegion.X = midX - mGuiSkin->HSliderTrack[0].TexRegion.Width * 0.5f;  
		trackRegion.Y = (float)screenPos.Y();
		trackRegion.Width = (float)mGuiSkin->HSliderTrack[0].TexRegion.Width;
		trackRegion.Height = (float)mSize.Y();

		float ratio = float(mValue - mMinimum) / (mMaximum - mMinimum);
	}
	
	thumbRegion.X = (float)mThumbRegion.X;
	thumbRegion.Y = (float)mThumbRegion.Y;
	thumbRegion.Width = (float)mThumbRegion.Width;
	thumbRegion.Height = (float)mThumbRegion.Height;

	// Track
	if (mDragSlider || mHovering)
		spriteBatch.Draw(mGuiSkin->mSkinTexAtlas, trackRegion, &mGuiSkin->HSliderTrack[1].TexRegion, mGuiSkin->HSliderTrack[1].TexColor);	
	else
		spriteBatch.Draw(mGuiSkin->mSkinTexAtlas, trackRegion, &mGuiSkin->HSliderTrack[0].TexRegion, mGuiSkin->HSliderTrack[0].TexColor);	

	// Thumb
	spriteBatch.Draw(mGuiSkin->mSkinTexAtlas, thumbRegion, &stateStyle.TexRegion, stateStyle.TexColor);	

	mHovering = false;
	mThumbHovering = false;
}

void Slider::OnHover( const int2& screenPos )
{
	UIElement::OnHover(screenPos);
	mThumbHovering = mThumbRegion.Contains(screenPos.X(), screenPos.Y());
}

void Slider::OnDragBegin( const int2& screenPos, uint32_t buttons )
{
	mDragBeginPos = screenPos;

	// Test if in thumb region
	mDragSlider = mThumbRegion.Contains(screenPos.X(), screenPos.Y());
	mDragBeginValue = mValue;
}

void Slider::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if (!mDragSlider)
		return; 

	int2 delta = screenPos - mDragBeginPos;
	
	int32_t newValue = mDragBeginValue;

	if (mOrientation == UI_Horizontal)
		newValue += int32_t( delta.X() / float(mSize.X() - mThumbRegion.Width) * float(mMaximum - mMinimum) );
	else
		newValue += int32_t( delta.Y() / float(mSize.Y() - mThumbRegion.Height) * float(mMaximum - mMinimum) );

	SetValueInternal(newValue, true);
}

void Slider::OnDragEnd( const int2& screenPos )
{
	mDragSlider = false;
}

bool Slider::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;

	// if in thumb region, OnDragBegin will handle it.
	if (mThumbRegion.Contains(screenPos.X(), screenPos.Y()))
		return false;

	if (IsInside(screenPos, true))
	{
		
		eventConsumed = true;
	}

	return eventConsumed;
}

bool Slider::OnMouseButtonRelease( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;

	if (IsInside(screenPos, true))
	{

		eventConsumed = true;
	}

	return eventConsumed;
}

void Slider::SetRange( int32_t min, int32_t max )
{
	mMinimum = min; 
	mMaximum = max;
}

void Slider::GetRange( int32_t& min, int32_t& max )
{
	min = mMinimum; max = mMaximum;
}

void Slider::SetValue( int32_t value )
{
	SetValueInternal(value, false);
}

void Slider::SetValueInternal( int32_t value, bool fromInput )
{
	value = Clamp(value, mMinimum, mMaximum);

	if (value == mValue)
		return;

	mValue = value;
	UpdateSlider();

	if (!EventValueChanged.empty() && fromInput)
		EventValueChanged(mValue);
}


}