#ifndef UIElement_h__
#define UIElement_h__

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Math/Vector.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class Cursor;

enum Alignment
{
	AlignLeft     = 1UL << 0,
	AlignRight    = 1UL << 1,
	AlignHCenter  = 1UL << 2,

	AlignTop      = 1UL << 3,
	AlignBottom   = 1UL << 4,
	AlignVCenter  = 1UL << 5,

	AlignCenter   = AlignHCenter | AlignVCenter
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
	virtual void Draw(SpriteBatch& spriteBatch);

	virtual void OnResize();

	virtual void OnMouseHover(const int2& position);
	
	virtual void OnDragBegin(const int2& position, uint32_t buttons);
	virtual void OnDragMove(const int2& position, uint32_t buttons);
	virtual void OnDragEnd(const int2& position);

	virtual bool OnMouseButtonPress(const int2& position, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& position, uint32_t button);
	
	virtual bool OnMouseWheel( int32_t delta );

	virtual bool OnKeyPress(uint16_t key);
	virtual bool OnKeyRelease(uint16_t key);

	virtual bool OnTextInput(uint16_t unicode);

	virtual bool CanHaveFocus() const;
	
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

	float mOpacity;

	IntRect mClipBorder;
};


}


#endif // UIElement_h__
