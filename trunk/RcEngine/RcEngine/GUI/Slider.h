#ifndef Slider_h__
#define Slider_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport Slider : public UIElement
{
public:
	/**
	 * Slider value changed event
	 */
	typedef fastdelegate::FastDelegate1<int32_t> ValueChangedEventHandler;
	ValueChangedEventHandler EventValueChanged;

	static const String TrackStyleName;
	static const String ThumbStyleName;

public:
	Slider();
	virtual ~Slider();

	virtual void Initialize(const GuiSkin::StyleMap* styles = nullptr);

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);
	
	virtual void OnResize();

	virtual void OnHover(const int2& screenPos);
	virtual void OnDragBegin(const int2& screenPos, uint32_t button);
	virtual void OnDragMove(const int2& screenPos, uint32_t buttons);
	virtual void OnDragEnd(const int2& screenPos);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	void SetSingleStep(int32_t step);
	int32_t GetSingleStep() const;

	void StepForward();
	void StepBack();

	void SetThumbStyle(GuiSkin::GuiStyle* style)  { mThumbStyle = style; }
	void SetTrackStyle(GuiSkin::GuiStyle* style)  { mTrackStyle = style; }

	void SetOrientation(UIOrientation orient);
	UIOrientation GetOrientation() const		  { return mOrientation; }
	
	void SetValue(int32_t value);
	int32_t GetValue() const					  { return mValue; }
	
	void SetRange(int32_t  min, int32_t  max);
	void GetRange(int32_t& min, int32_t& max);

protected:
	void UpdateSlider();
	void UpdateRect();
	void SetValueInternal( int32_t nValue, bool fromInput );

protected:
	UIOrientation mOrientation;

	int32_t mValue;
	int32_t mMinimum, mMaximum;

	int32_t mSingleStep;

	bool mThumbHovering;

	bool mDragSlider;
	int32_t mDragBeginValue;
	int2 mDragBeginPos;		

	IntRect mThumbRegion;

	GuiSkin::GuiStyle* mThumbStyle;
	GuiSkin::GuiStyle* mTrackStyle;
};



}


#endif // Slider_h__
