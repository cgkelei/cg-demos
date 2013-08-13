#include <GUI/ScrollBar.h>
#include <GUI/Slider.h>
#include <GUI/Button.h>

namespace RcEngine {

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
	mForwardButton->EventButtonClicked.bind(this, &ScrollBar::HandleForwardButtonPressed);
	mBackButton->EventButtonClicked.bind(this, &ScrollBar::HandleBackButtonPressed);
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

void ScrollBar::HandleBackButtonPressed()
{

}

void ScrollBar::HandleForwardButtonPressed()
{

}

void ScrollBar::HandleSliderChanged( int32_t value )
{

}


}