#include <GUI/Slider.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>

namespace RcEngine {

Slider::Slider()
	: mMinimum(0), 
	  mMaximum(100),
	  mValue(0),
	  mPressed(false)
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


void Slider::UpdateSlider()
{
	int2 screenPos = GetScreenPosition();

	float ratio = float(mValue - mMinimum) / (mMaximum - mMinimum);

	if (mOrientation == Horizontal)
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
	int2 screenPos = GetScreenPosition();
}

void Slider::Draw( SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont )
{
	if (!mVisible)
		return;

	if (!mGuiSkin)
		SetGuiSkin( UIManager::GetSingleton().GetDefaultSkin() );

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

	int2 screenPos = GetScreenPosition();

	Rectanglef trackRegion, thumbRegion;

	// Draw order: track, thumb

	if (mOrientation == Horizontal)
	{
		float midY = screenPos.Y() + mSize.Y() * 0.5f;

		trackRegion.X = (float)screenPos.X();
		trackRegion.Y = midY - mGuiSkin->HSliderTrack.TexRegion.Height * 0.5f;
		trackRegion.Width = (float)mSize.X();
		trackRegion.Height = (float)mGuiSkin->HSliderTrack.TexRegion.Height;
		
		float ratio = float(mValue - mMinimum) / (mMaximum - mMinimum);

		thumbRegion.X = screenPos.X() + ratio * ( mSize.X() - mGuiSkin->HSliderThumb.TexRegion.Width );
		thumbRegion.Y = (float)screenPos.Y(); 

		thumbRegion.Width = (float)mSize.Y(); 
		thumbRegion.Height = (float)mSize.Y();
	}
	else
	{
		float midX = screenPos.X() + mSize.X() * 0.5f;

		trackRegion.X = midX - mGuiSkin->HSliderTrack.TexRegion.Width * 0.5f;  
		trackRegion.Y = (float)screenPos.Y();
		trackRegion.Width = (float)mGuiSkin->HSliderTrack.TexRegion.Width;
		trackRegion.Height = (float)mSize.Y();

		float ratio = float(mValue - mMinimum) / (mMaximum - mMinimum);

		thumbRegion.X = (float)screenPos.X();
		thumbRegion.Y = screenPos.Y() + ratio * ( mSize.Y() - mGuiSkin->HSliderThumb.TexRegion.Height ); 

		thumbRegion.Width = (float)mSize.X(); 
		thumbRegion.Height = (float)mSize.X();
	}

	spriteBatch.Draw(mGuiSkin->mSkinTexAtlas, trackRegion, &mGuiSkin->HSliderTrack.TexRegion, mGuiSkin->HSliderTrack.TexColor);	
	spriteBatch.Draw(mGuiSkin->mSkinTexAtlas, thumbRegion, &mGuiSkin->HSliderThumb.TexRegion, mGuiSkin->HSliderThumb.TexColor);	

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
}

void Slider::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if (!mDragSlider)
		return; 

	int32_t newValue;
	int2 delta = screenPos - mDragBeginPos;
	
	if (mOrientation == Horizontal)
	{
		int32_t newKnobX = Clamp(mThumbRegion.X + delta.X(), 0, mSize.X() - mThumbRegion.Width);
		
		float ratio = (float)newKnobX / float( mSize.X() - mThumbRegion.Width );
		newValue = mMinimum + int32_t( (mMaximum - mMinimum) * ratio );
	}
	else
	{
		int32_t newKnobY = Clamp(mThumbRegion.Y + delta.Y(), 0, mSize.Y() - mThumbRegion.Height);

		float ratio = (float)newKnobY / float( mSize.Y() - mThumbRegion.Height );
		newValue = int32_t( mMinimum + (mMaximum - mMinimum) * ratio );
	}

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

	if (!EventValueChange.empty() && fromInput)
		EventValueChange(mValue);
}

}