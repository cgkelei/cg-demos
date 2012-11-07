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
	~UIWindow();

	void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers);
	void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers);
	void OnDragEnd(const IntVector2& position, const IntVector2& screenPosition);

	void SetMovable(bool movable);
	void SetResizable(bool resizable);
	void SetResizeBorderSize(int32_t size);

	bool IsMovable() const	{ return mMovable; }
	bool IsResizable() const { return mResizable; }

	void Minimize();
	void Maximize();
	void Restore();

	virtual void Draw();

protected:
	DragMode GetDragMode(const IntVector2& position);
	void ValidatePosition();

protected:
	WindowState mWindowState;
	DragMode mDragMode;

	bool mMovable;
	bool mResizable;

	int32_t mResizeBorderSize;

	bool mMinimizing; 

	IntVector2 mLastNormalPos;
	IntVector2 mLastNormalSize;

	

};


}

#endif // UIWindow_h__
