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


typedef Vector<int32_t, 2> IntVector2;

class _ApiExport UIElement
{
public:
	UIElement();
	virtual ~UIElement();
	
	virtual void OnMouseCover(const IntVector2& position, const IntVector2& screenPosition, uint32_t buttons, uint32_t qualifiers, Cursor* cursor);

	virtual void OnMouseDown(const IntVector2& position, const IntVector2& screenPosition, uint32_t buttons, int qualifiers, Cursor* cursor);
	virtual void OnMouseUp(const IntVector2& position, const IntVector2& screenPosition, uint32_t buttons, int qualifiers, Cursor* cursor);

	virtual void OnMouseWheel(int32_t delta, uint32_t buttons, uint32_t qualifiers);

	virtual void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers, Cursor* cursor);
	virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers, Cursor* cursor);
	virtual void OnDragEnd(const IntVector2& position, const IntVector2& screenPosition, Cursor* cursor);

	virtual void OnKeyDown(uint8_t key, uint32_t buttons, uint32_t qualifiers);
	virtual void OnKeyUp(uint8_t key, uint32_t buttons, uint32_t qualifiers);

	void SetName(const String& name);
	void SetPosition(const IntVector2& position);
	void SetPosition(int32_t x, int32_t y);
	void SetSize(const IntVector2& size);
	void SetSize(int32_t width, int32_t height);

	const String& GetName() const				{ return mName; }
	const IntVector2& GetPosition() const		{ return mPosition; }
	IntVector2 GetScreenPosition();

	const IntVector2& GetMinSize() const		{ return mMinSize; }
	const IntVector2& GetMaxSize() const		{ return mMaxSize; }

	bool HasFocus() const;
	bool IsMouseCover() const					{ return mCovering; }
	bool IsActive() const					    { return mActive; }
	bool IsVisible() const						{ return mVisible; }

	UIElement* GetParent() const							{ return mParent; }
	const std::vector<UIElement*>& GetChildren() const		{ return mChildren; }

	uint32_t GetNumChildren(bool recursive = false) const;
	UIElement* GetChild(const String& name, bool recursive = false) const;
	UIElement* GetChild(uint32_t index) const;
	
	IntVector2 ScreenToClient(const IntVector2& screenPosition);
	IntVector2 ClientToScreen(const IntVector2& position);

	bool IsInside(IntVector2 position, bool isScreen);

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

	IntRect mClipBorder;
};


}


#endif // UIElement_h__
