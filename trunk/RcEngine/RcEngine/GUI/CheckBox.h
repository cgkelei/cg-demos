#ifndef CheckBox_h__
#define CheckBox_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport CheckBox : public UIElement
{
public:
	typedef fastdelegate::FastDelegate1<bool> StateChangeEventHandler;

public:
	CheckBox();
	virtual ~CheckBox();

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual bool CanHaveFocus() const;

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	void SetChecked(bool enalbe);
	bool IsChecked() const;

	void SetText(const std::wstring& text);

protected:
	void UpdateRect();

public:
	StateChangeEventHandler EventStateChange;
	
protected:
	
	bool mPressed;
	bool mCheched;

	std::wstring mText;
	
	// Used for draw
	Rectanglef mCheckRect;
	Rectanglef mTextRect;
};


}

#endif // CheckBox_h__
