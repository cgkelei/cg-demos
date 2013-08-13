#ifndef ScrollBar_h__
#define ScrollBar_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport ScrollBar : public UIElement
{
public:
	ScrollBar();
	virtual ~ScrollBar();

	virtual void OnResize();

	virtual void OnDragBegin(const int2& screenPos, uint32_t buttons);
	virtual void OnDragMove(const int2& screenPos, uint32_t buttons);
	virtual void OnDragEnd(const int2& screenPos);

	void SetOrientation(UIOrientation orient);

protected:
	void HandleBackButtonPressed();
	void HandleForwardButtonPressed();
	void HandleSliderChanged(int32_t value);

	void StepBack();
	void StepForward();

	void UpdateRect();

protected:

	Button* mForwardButton;
	Button* mBackButton;
	Slider* mSlider;
};


}


#endif // ScrollBar_h__
