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

enum FocusMode
{
	FM_NoFocus = 0,
	FM_TabFocus = 0x1,
	FM_ClickFocus = 0x2,
	FM_Tab_ClickFocus = FM_TabFocus | FM_ClickFocus,
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

typedef Vector<int32_t, 2> Point;
typedef Vector<int32_t, 2> Size;

class _ApiExport UIElement
{
public:
	UIElement();
	virtual ~UIElement();

	virtual void OnMouseHover(const Point& position, const Point& screenPosition, uint32_t buttons, uint32_t qualifiers);
	virtual void OnMouseWheel(int32_t delta, uint32_t buttons, uint32_t qualifiers);
	
	virtual void OnClick(const Point& position, const Point& screenPosition, uint32_t buttons, int qualifiers);

	virtual void OnDragBegin(const Point& position, const Point& screenPosition, int buttons, int qualifiers);
	virtual void OnDragMove(const Point& position, const Point& screenPosition, int buttons, int qualifiers);
	virtual void OnDragEnd(const Point& position, const Point& screenPosition);

	virtual void OnKeyPress(uint8_t key, uint32_t qualifiers);
	virtual void OnKeyRelease(uint8_t key, uint32_t qualifiers);

	const String& GetName() const				{ return mName; }
	void SetName(const String& name)			{ mName = name; }
	
	void SetPosition(const Point& position);
	const Point& GetPosition() const			{ return mPosition; }
	Point GetScreenPosition();

	void SetSize(const Size& size);
	const Point& GetSize() const				{ return mSize; }

	void SetMinSize(const Size& size);
	const Point& GetMinSize() const				{ return mMinSize; }
	const Point& GetMaxSize() const				{ return mMaxSize; }
	
	void SetFocusMode(FocusMode mode)			{ mFocusMode = mode; }
	FocusMode GetFocusMode() const			    { return mFocusMode; }

	void SetVisible(bool visible);
	bool IsVisible() const						{ return mVisible; }

	bool HasFocus() const;
	bool IsMouseHover() const					{ return mHovering; }

	void SetEnable(bool enable)					{ mEnabled = enable; }
	bool IsEnabled() const						{ return mEnabled; }
	
	const std::wstring& GetToolTip() const		{ return mToolTipText; }
	void SetToolTip(const std::wstring& txt)    { mToolTipText = txt; }

	IntRect GetArea() const;
	IntRect GetGlobalArea();
		
	void SetParent(UIElement* parent);
	UIElement* GetParent() const				{ return mParent; }

	uint32_t GetNumChildren(bool recursive = false) const;
	
	UIElement* GetChild(const String& name, bool recursive = false) const;
	UIElement* GetChild(uint32_t index) const;

	std::vector<UIElement*>& GetChildren()						{ return mChildren; }
	const std::vector<UIElement*>& GetChildren() const			{ return mChildren; }

	void FlattenChildren(std::vector<UIElement*>& children) const;	

	void AddChild(UIElement* child);
	void RemoveChild(UIElement* child);
	
	// Remove from parent
	void Remove();

	Point ScreenToClient(const Point& screenPosition);
	Point ClientToScreen(const Point& position);

	bool IsInside(Point position, bool isScreen);

	void BringToFront();

	virtual void Draw();

protected:
	void MarkDirty();

protected:

	String mName;

	UIElement* mParent;
	std::vector<UIElement*> mChildren;

	bool mHovering;
	bool mVisible;
	bool mEnabled;
	bool mPositionDirty;

	Point mPosition;
	Point mScreenPosition;
	
	Size mSize;
	Size mMinSize;
	Size mMaxSize;

	std::wstring mToolTipText;

	HorizontalAlignment mHorizontalAlignment;
	VerticalAlignment mVerticalAlignment;
	FocusMode mFocusMode;

	float mOpacity;

	IntRect mClipBorder;
};


}


#endif // UIElement_h__
