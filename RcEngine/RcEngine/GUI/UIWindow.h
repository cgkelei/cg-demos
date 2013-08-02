#ifndef UIWindow_h__
#define UIWindow_h__

#include <Core/Prerequisites.h>
#include <GUI/UIElement.h>

namespace RcEngine {

class _ApiExport UIWindow : public UIElement
{
public:
	enum WindowState
	{
		Normal,
		Minimized,
		Maximized
	};

	enum DragMode
	{
		Drag_None,
		Drag_Move,
		Drag_Resize_TopLeft,
		Drag_Resize_Top,
		Drag_Resize_TopRight,
		Drag_Resize_BottomRight,
		Drag_Resize_Bottom,
		Drag_Resize_BottomLeft,
		Drag_Resize_Left,
		Drag_Resize_Right,
	};

public:
	UIWindow();
	virtual ~UIWindow();

	virtual void OnDragBegin(const int2& screenPos, uint32_t button);
	virtual void OnDragMove(const int2& screenPos, uint32_t buttons);
	virtual void OnDragEnd(const int2& screenPos);

	virtual void Update(float delta);
	virtual void Draw(SpriteBatch& spriteBatch);

	void SetMovable(bool movable);
	void SetResizable(bool resizable);
	void SetResizeBorderSize(int32_t size);

	bool IsMovable() const	{ return mMovable; }
	bool IsResizable() const { return mResizable; }

	void Minimize();
	void Maximize();
	void Restore();



protected:
	DragMode GetDragMode(const int2& position);
	void ValidatePosition();

protected:
	WindowState mWindowState;
	DragMode mDragMode;

	bool mMovable;
	bool mResizable;

	int32_t mResizeBorderSize;

	bool mMinimizing; 

	int2 mLastNormalPos;
	int2 mLastNormalSize;

	int2 mDragBeginPos;		// Position in parent region when drag begin
	int2 mDragBeginSize;    // Window size when drag begin
	int2 mDragBeginCurosr;  // Cursor position when drag begin

	

};


}

#endif // UIWindow_h__
