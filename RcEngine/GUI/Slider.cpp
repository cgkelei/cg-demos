#include <GUI/Slider.h>
#include <GUI/UIManager.h>
#include <Graphics/SpriteBatch.h>

namespace RcEngine {

const String Slider::TrackStyleName("Slider::TrackStyleName");
const String Slider::ThumbStyleName("Slider::ThumbStyleName");

Slider::Slider( UIOrientation orient )
	: mOrientation(orient),
	  mMinimum(0), 
	  mMaximum(100),
	  mValue(0),
	  mSingleStep(1),
	  mDragSlider(false),
	  mThumbHovering(false),
	  mThumbStyle(nullptr),
	  mTrackStyle(nullptr)
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

	const GuiSkin::SytleImage& stateStyle = mThumbStyle->StyleStates[uiThumbState];

	Rectanglef trackRegion, thumbRegion;

	// Draw order: track, thumb
	if (mOrientation == UI_Horizontal)
	{
		float midY = screenPos.Y() + mSize.Y() * 0.5f;

		trackRegion.X = (float)screenPos.X();
		trackRegion.Y = midY - mTrackStyle->StyleStates[UI_State_Normal].TexRegion.Height * 0.5f;
		trackRegion.Width = (float)mSize.X();
		trackRegion.Height = (float)mTrackStyle->StyleStates[UI_State_Normal].TexRegion.Height;	
	}
	else
	{
		float midX = screenPos.X() + mSize.X() * 0.5f;

		trackRegion.X = midX - mTrackStyle->StyleStates[UI_State_Normal].TexRegion.Width * 0.5f;  
		trackRegion.Y = (float)screenPos.Y();
		trackRegion.Width = (float)mTrackStyle->StyleStates[UI_State_Normal].TexRegion.Width;
		trackRegion.Height = (float)mSize.Y();	
	}

	thumbRegion.X = (float)mThumbRegion.X;
	thumbRegion.Y = (float)mThumbRegion.Y;
	thumbRegion.Width = (float)mThumbRegion.Width;
	thumbRegion.Height = (float)mThumbRegion.Height;

	// Track
	if (mDragSlider || mHovering)
		spriteBatch.Draw(mTrackStyle->StyleTex, trackRegion, &mTrackStyle->StyleStates[UI_State_Hover].TexRegion, mTrackStyle->StyleStates[UI_State_Hover].TexColor);	
	else
		spriteBatch.Draw(mTrackStyle->StyleTex, trackRegion, &mTrackStyle->StyleStates[UI_State_Normal].TexRegion, mTrackStyle->StyleStates[UI_State_Normal].TexColor);	

	// Thumb
	spriteBatch.Draw(mThumbStyle->StyleTex, thumbRegion, &stateStyle.TexRegion, stateStyle.TexColor);	

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

void Slider::StepForward()
{
	SetValueInternal(mValue + mSingleStep, true);
}

void Slider::StepBack()
{
	SetValueInternal(mValue - mSingleStep, true);
}

bool Slider::OnMouseButtonPress( const int2& screenPos, uint32_t button )
{
	bool eventConsumed = false;

	// if in thumb region, OnDragBegin will handle it.
	if (mThumbRegion.Contains(screenPos.X(), screenPos.Y()))
		return false;

	if (IsInside(screenPos, true))
	{
		if (mOrientation == UI_Horizontal)
		{
			if (screenPos.X() > mThumbRegion.X)
				StepForward();
			else
				StepBack();
		}
		else
		{
			if (screenPos.Y() > mThumbRegion.Y)
				StepForward();
			else
				StepBack();
		}

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

	UpdateSlider();
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

void Slider::InitGuiStyle( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{
	if (!styles)
	{
		GuiSkin* defalutSkin = UIManager::GetSingleton().GetDefaultSkin();

		if (mOrientation == UI_Horizontal)
		{
			mTrackStyle = &defalutSkin->HSliderTrack;
			mThumbStyle = &defalutSkin->HSliderThumb;
		}
		else
		{
			mTrackStyle = &defalutSkin->VSliderTrack;
			mThumbStyle = &defalutSkin->VSliderThumb;
		}
	}
	else
	{
		GuiSkin::StyleMap::const_iterator iter;
		iter = styles->find(ThumbStyleName);
		mThumbStyle = iter->second;

		iter = styles->find(TrackStyleName);
		mTrackStyle = iter->second;
	}
}



}