#ifndef CheckBox_h__
#define CheckBox_h__

#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport CheckBox : public UIElement
{
public:
	typedef fastdelegate::FastDelegate1<bool> StateChangeEventHandler;

public:
	CheckBox(const int2& pos, const std::wstring& text, bool checked);
	virtual ~CheckBox();

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);

	virtual bool CanHaveFocus() const;

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch);

	void SetChecked(bool enalbe);
	bool IsChecked() const;

public:

	StateChangeEventHandler EventStateChange;
	
protected:
	
	bool mPressed;
	bool mCheched;
};


}

#endif // CheckBox_h__
