#include <GUI/ScrollBar.h>
#include <GUI/Slider.h>
#include <GUI/Button.h>
#include <GUI/UIManager.h>

namespace RcEngine {

const String ScrollBar::ForwardStyleName("ScrollBar::ForwardStyleName");
const String ScrollBar::BackStyleName("ScrollBar::BackStyleName");
const String ScrollBar::ThumbStyleName("ScrollBar::ThumbStyleName");
const String ScrollBar::TrackStyleName("ScrollBar::TrackStyleName");

ScrollBar::ScrollBar()
{
	mForwardButton = new Button;
	mForwardButton->SetPressedOffset(int2::Zero());
	mForwardButton->SetHoverOffset(int2::Zero());

	mBackButton = new Button;
	mBackButton->SetPressedOffset(int2::Zero());
	mBackButton->SetHoverOffset(int2::Zero());

	AddChild(mForwardButton);
	AddChild(mBackButton);

	mSlider = new Slider;
	AddChild(mSlider);
	// Set default orientation
	SetOrientation(UI_Vertical);

	// Events
	mForwardButton->EventButtonClicked.bind(this, &ScrollBar::StepForward);
	mBackButton->EventButtonClicked.bind(this, &ScrollBar::StepBack);
	mSlider->EventValueChanged.bind(this, &ScrollBar::HandleSliderChanged);
}

ScrollBar::~ScrollBar()
{

}

void ScrollBar::SetOrientation( UIOrientation orient )
{
	mSlider->SetOrientation(orient);
	UpdateRect();
}

void ScrollBar::OnResize()
{
	UpdateRect();
}

void ScrollBar::UpdateRect()
{
	int2 screenPos = GetScreenPosition();

	if (mSlider->GetOrientation() == UI_Horizontal)
	{
		int32_t size = GetSize().Y();
		mForwardButton->SetPosition(int2(0, 0));
		mForwardButton->SetSize(int2(size, size));

		mForwardButton->SetPosition(int2(mSize.X() - size, 0));
		mBackButton->SetSize(int2(size, size));

		mSlider->SetPosition(int2(size, 0));
		mSlider->SetSize(int2(mSize.X() - size - size, mSize.Y()));
	}
	else
	{
		int32_t size = GetSize().X();

		mForwardButton->SetPosition(int2(0, 0));
		mForwardButton->SetSize(int2(size, size));

		mBackButton->SetPosition(int2(0, mSize.Y() - size));
		mBackButton->SetSize(int2(size, size));

		mSlider->SetPosition(int2(0, size));
		mSlider->SetSize(int2(mSize.X(), mSize.Y() - size - size));
	}
}

void ScrollBar::HandleSliderChanged( int32_t value )
{

}

void ScrollBar::Initialize( const GuiSkin::StyleMap* styles /* = nullptr */ )
{
	if( !styles )
	{
		GuiSkin* defalutSkin = UIManager::GetSingleton().GetDefaultSkin();

		GuiSkin::StyleMap styleMap;
		styleMap[Button::StyleName] = &defalutSkin->HSrollForward;
		mForwardButton->Initialize(&styleMap);

		styleMap[Button::StyleName] = &defalutSkin->HSrollBack;
		mBackButton->Initialize(&styleMap);

		styleMap[Slider::TrackStyleName] = &defalutSkin->HScrollTrack;
		styleMap[Slider::ThumbStyleName] = &defalutSkin->HSrollThumb;
		mSlider->Initialize(&styleMap);
	}
	else
	{
		GuiSkin::StyleMap::const_iterator iter;
		
		GuiSkin::StyleMap styleMap;

		iter = styles->find(ForwardStyleName);	
		styleMap[Button::StyleName] = iter->second;
		mForwardButton->Initialize(&styleMap);

		iter = styles->find(BackStyleName);	
		styleMap[Button::StyleName] = iter->second;
		mBackButton->Initialize(&styleMap);

		// Slider
		iter = styles->find(TrackStyleName);
		styleMap[Slider::TrackStyleName] = iter->second;

		iter = styles->find(ThumbStyleName);
		styleMap[Slider::ThumbStyleName] = iter->second;

		mSlider->Initialize(&styleMap);
	}
}

void ScrollBar::StepBack()
{
	mSlider->StepBack();
}

void ScrollBar::StepForward()
{
	mSlider->StepForward();
}

void ScrollBar::SetRange( int32_t min, int32_t max )
{
	mSlider->SetRange(min, max);
}

}