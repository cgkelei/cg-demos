#ifndef Button_h__
#define Button_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class Button : UIElement
{
public:
	typedef fastdelegate::FastDelegate0<> ButtonClickedEventHandler;


public:
	Button();
	virtual ~ Button();

	virtual void OnClick(const int2& position, const int2& screenPosition, uint32_t buttons, int qualifiers);
	
	virtual void Update(float delta);

	void SetPressedOffset(const int2& offset)	{ mPressedOffset; }
	const int2& GetPressedOffset() const        { return mPressedOffset; }

public:
	ButtonClickedEventHandler EventButtonClicked;


protected:

	bool mPressed;

	int2 mPressedOffset;

};


}

#endif // Button_h__
