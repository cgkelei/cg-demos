#ifndef UIElement_h__
#define UIElement_h__

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Math/Vector.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class Cursor;

enum HorizontalAlignment
{
	HA_Left = 0,
	HA_Center,
	HA_Right
};

/// %UI element vertical alignment.
enum VerticalAlignment
{
	VA_Top = 0,
	VA_Center,
	VA_Bottom
};

enum FocusPolicy
{
	FP_NoFocus = 0,
	FP_TabFocus = 0x1,
	FP_ClickFocus = 0x2,
	FP_Tab_ClickFocus = FP_TabFocus | FP_ClickFocus,
};

enum UIElementState
{
	UI_State_Normal = 0,
	UI_State_Disable,
	UI_State_Hidden,
	UI_State_Focus,
	UI_State_Selected,
	UI_State_MouseOver,
	UI_State_Pressed,
};


typedef Vector<int32_t, 2> IntVector2;

class _ApiExport UIElement
{
public:
	UIElement();
	virtual ~UIElement();
	
	virtual void OnMouseCover(const IntVector2& position, const IntVector2& screenPosition, uint32_t buttons, uint32_t qualifiers);

	virtual void OnMouseDown(const IntVector2& position, const IntVector2& screenPosition, uint32_t buttons, int qualifiers);
	virtual void OnMouseUp(const IntVector2& position, const IntVector2& screenPosition, uint32_t buttons, int qualifiers);

	virtual void OnMouseWheel(int32_t delta, uint32_t buttons, uint32_t qualifiers);

	virtual void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers);
	virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers);
	virtual void OnDragEnd(const IntVector2& position, const IntVector2& screenPosition);

	virtual void OnKeyDown(uint8_t key, uint32_t qualifiers);
	virtual void OnKeyUp(uint8_t key, uint32_t qualifiers);

	void SetName(const String& name);
	void SetPosition(const IntVector2& position);
	void SetPosition(int32_t x, int32_t y);
	void SetSize(const IntVector2& size);
	void SetSize(int32_t width, int32_t height);
	void SetFocusPolicy(FocusPolicy policy);
	void SetVisible(bool visible);

	const String& GetName() const				{ return mName; }
	const IntVector2& GetPosition() const		{ return mPosition; }
	IntVector2 GetScreenPosition();

	const IntVector2& GetSize() const			{ return mSize; }
	const IntVector2& GetMinSize() const		{ return mMinSize; }
	const IntVector2& GetMaxSize() const		{ return mMaxSize; }

	IntRect GetArea() const;
	IntRect GetGlobalArea();

	bool HasFocus() const;
	bool IsMouseCover() const					{ return mCovering; }
	bool IsActive() const					    { return mActive; }
	bool IsVisible() const						{ return mVisible; }
	FocusPolicy GetFocusPolicy() const			{ return mFocusPolicy; }

	UIElement* GetParent() const							{ return mParent; }

	uint32_t GetNumChildren(bool recursive = false) const;
	const std::vector<UIElement*>& GetChildren() const		{ return mChildren; }
	
	void GetChildren(std::vector<UIElement*>& children, bool recursive = true) const;	
	UIElement* GetChild(const String& name, bool recursive = false) const;
	UIElement* GetChild(uint32_t index) const;
	
	IntVector2 ScreenToClient(const IntVector2& screenPosition);
	IntVector2 ClientToScreen(const IntVector2& position);

	bool IsInside(IntVector2 position, bool isScreen);

	virtual void Draw();

private:
	void MarkDirty();

protected:

	String mName;
	UIElement* mParent;
	std::vector<UIElement*> mChildren;

	bool mCovering;
	bool mVisible;
	bool mActive;
	bool mPositionDirty;

	IntVector2 mPosition;
	IntVector2 mScreenPosition;
	
	IntVector2 mSize;
	IntVector2 mMinSize;
	IntVector2 mMaxSize;

	HorizontalAlignment mHorizontalAlignment;
	VerticalAlignment mVerticalAlignment;
	FocusPolicy mFocusPolicy;

	float mOpacity;

	IntRect mClipBorder;
};


}


#endif // UIElement_h__
