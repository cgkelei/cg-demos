#ifndef UIElement_h__
#define UIElement_h__

#include <Core/Prerequisites.h>
#include <Graphics/Renderable.h>
#include <Math/Vector.h>
#include <Math/Rectangle.h>
#include <Math/ColorRGBA.h>
#include <GUI/GuiSkin.h>

namespace RcEngine {

class Cursor;
class GuiSkin;

class _ApiExport UIElement
{
public:
	UIElement();
	virtual ~UIElement();

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch, SpriteBatch& spriteBatchFont);

	// Events
	virtual void OnResize();

	virtual void OnHover(const int2& screenPos);
	
	virtual void OnDragBegin(const int2& screenPos, uint32_t buttons);
	virtual void OnDragMove(const int2& screenPos, uint32_t buttons);
	virtual void OnDragEnd(const int2& screenPos);

	virtual bool OnMouseButtonPress(const int2& screenPos, uint32_t button);
	virtual bool OnMouseButtonRelease(const int2& screenPos, uint32_t button);
	virtual bool OnMouseWheel( int32_t delta );

	virtual bool OnKeyPress(uint16_t key);
	virtual bool OnKeyRelease(uint16_t key);
	virtual bool OnTextInput(uint16_t unicode);

	virtual bool CanHaveFocus() const;
	
	const String& GetName() const				{ return mName; }
	void SetName(const String& name)			{ mName = name; }
	
	void SetPosition(const int2& position);
	const int2& GetPosition() const				{ return mPosition; }
	
	int2 GetScreenPosition();

	/**
	 * Set control size, if this is first time call SetSize, make sure
	 * control position has been set already. 
	 */
	void SetSize(const int2& size);
	const int2& GetSize() const					{ return mSize; }

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

	void SetGuiSkin(GuiSkin* skin);
	GuiSkin* GetGuiSkin() const				{ return mGuiSkin; }

protected:
	void MarkDirty();

	virtual void UpdateRect();

protected:

	String mName;

	std::wstring mToolTipText;

	UIElement* mParent;
	std::vector<UIElement*> mChildren;

	GuiSkin* mGuiSkin;

	bool mHovering;
	bool mVisible;
	bool mEnabled;

	bool mPositionDirty;

	int2 mPosition;
	int2 mScreenPosition;
	
	int2 mSize;
	int2 mMinSize;
	int2 mMaxSize;

	IntRect mClipBorder;

	float mOpacity;
};


}


#endif // UIElement_h__
