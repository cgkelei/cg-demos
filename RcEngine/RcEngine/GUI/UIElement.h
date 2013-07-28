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

/// UI element vertical alignment.
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

class _ApiExport UIElement
{
public:
	UIElement();
	virtual ~UIElement();

	virtual void Update(float delta);
	virtual void Draw();

	virtual bool OnMouseHover(const int2& position, uint32_t buttons);
	virtual bool OnMouseWheel( int32_t delta );
	
	virtual bool OnClick(const int2& position, uint32_t buttons);

	virtual void OnDragBegin(const int2& position, uint32_t buttons);
	virtual void OnDragMove(const int2& position, uint32_t buttons);
	virtual void OnDragEnd(const int2& position);

	virtual bool OnKeyPress(uint16_t key);
	virtual bool OnKeyRelease(uint16_t key);

	virtual bool OnTextInput(uint16_t unicode);

	virtual void OnResize();

	const String& GetName() const				{ return mName; }
	void SetName(const String& name)			{ mName = name; }
	
	void SetPosition(const int2& position);
	const int2& GetPosition() const			{ return mPosition; }
	int2 GetScreenPosition();

	void SetSize(const int2& size);
	const int2& GetSize() const				{ return mSize; }

	void SetMinSize(const int2& size);
	const int2& GetMinSize() const				{ return mMinSize; }
	const int2& GetMaxSize() const				{ return mMaxSize; }
	
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

	int2 ScreenToClient(const int2& screenPosition);
	int2 ClientToScreen(const int2& position);

	bool IsInside(int2 position, bool isScreen);

	void BringToFront();

	

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

	int2 mPosition;
	int2 mScreenPosition;
	
	int2 mSize;
	int2 mMinSize;
	int2 mMaxSize;

	std::wstring mToolTipText;

	HorizontalAlignment mHorizontalAlignment;
	VerticalAlignment mVerticalAlignment;
	FocusMode mFocusMode;

	float mOpacity;

	IntRect mClipBorder;
};


}


#endif // UIElement_h__
