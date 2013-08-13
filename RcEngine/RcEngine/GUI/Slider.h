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

public:
	Slider();
	virtual ~Slider();

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);
	
	virtual void OnResize();

	virtual void OnHover(const int2& screenPos);
	virtual void OnDragBegin(const int2& screenPos, uint32_t buttons);
	virtual void OnDragMove(const int2& screenPos, uint32_t buttons);
	virtual void OnDragEnd(const int2& screenPos);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	void SetOrientation(UIOrientation orient);
	UIOrientation GetOrientation() const  { return mOrientation; }
	
	void SetValue(int32_t value);
	int32_t GetValue() const			{ return mValue; }
	
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

	bool mThumbHovering;

	bool mDragSlider;
	int32_t mDragBeginValue;
	int2 mDragBeginPos;		

	IntRect mThumbRegion;
};



}


#endif // Slider_h__
