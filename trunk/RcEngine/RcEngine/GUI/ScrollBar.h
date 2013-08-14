#ifndef ScrollBar_h__
#define ScrollBar_h__

#include <GUI/UIElement.h>
#include <GUI/Slider.h>

namespace RcEngine {

class _ApiExport ScrollBar : public UIElement
{
public:

	static const String ForwardStyleName;
	static const String BackStyleName;
	static const String ThumbStyleName;
	static const String TrackStyleName;

public:
	ScrollBar();
	virtual ~ScrollBar();

	virtual void Initialize(const GuiSkin::StyleMap* styles /* = nullptr */);

	virtual void OnResize();

	void SetOrientation(UIOrientation orient);

	void SetRange(int32_t min, int32_t max);
	void GetRange(int32_t& min, int32_t& max)	{ mSlider->GetRange(min, max); }
	
	inline int32_t GetValue() const	 { return mSlider->GetValue(); }

	void Scroll(int32_t delta);

protected:
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
