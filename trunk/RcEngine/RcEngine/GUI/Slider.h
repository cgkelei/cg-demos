#ifndef Slider_h__
#define Slider_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport Slider : public UIElement
{
public:
	enum Orientation
	{
		Horizontal,
		Vertical
	};

	typedef fastdelegate::FastDelegate1<int32_t> ValueChangeEventHandler;

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

	void SetOrientation(Orientation o)  { mOrientation = o; }
	Orientation GetOrientation() const  { return mOrientation; }
	
	void SetValue(int32_t value);
	int32_t GetValue() const			{ return mValue; }
	
	void SetRange(int32_t  min, int32_t  max);
	void GetRange(int32_t& min, int32_t& max);

protected:
	void UpdateSlider();
	void UpdateRect();
	void SetValueInternal( int32_t nValue, bool fromInput );

public:
	ValueChangeEventHandler EventValueChange;

protected:
	Orientation mOrientation;

	bool mPressed;

	bool mThumbHovering;

	int32_t mValue;
	int32_t mMinimum, mMaximum;

	int2 mDragBeginPos;
	
	int32_t mThumbPos;
	bool mDragSlider;

	IntRect mThumbRegion;
};



}


#endif // Slider_h__
