#include <GUI/UIWindow.h>
#include <GUI/UIManager.h>
#include <Math/Math.h>
#include <Input/InputSystem.h>

static const int DEFAULT_RESIZE_BORDER = 4;

namespace RcEngine {

UIWindow::UIWindow()
	: mResizeBorderSize(DEFAULT_RESIZE_BORDER)
{

}

UIWindow::~UIWindow()
{

}

void UIWindow::SetResizeBorderSize( int32_t size )
{
	mResizeBorderSize = (std::max)(0, size);
}


void UIWindow::SetMovable( bool movable )
{
	mMovable = movable;
}

void UIWindow::SetResizable( bool resizable )
{
	mResizable = resizable;
}


void UIWindow::OnDragBegin( const int2& screenPos, uint32_t button )
{
	if (button != MS_LeftButton)
	{
		mDragMode = Drag_None;
		return ;
	}

	mDragBeginCurosr = screenPos;
	mDragBeginPos = GetPosition();
	mDragBeginSize = GetSize();
	mDragMode = GetDragMode(ScreenToClient(screenPos));
	//SetCursorShape(dragMode_, cursor);
}

void UIWindow::OnDragMove( const int2& screenPos, uint32_t buttons )
{
	if (mDragMode == Drag_None)
		return;

	int2 delta = screenPos - mDragBeginCurosr;

	switch (mDragMode)
	{
	case Drag_Move:
		{
			SetPosition(mDragBeginPos + delta);
		}
		break;
	case Drag_Resize_TopLeft:
		{
			SetPosition(mDragBeginPos + delta);
			SetSize(mDragBeginSize - delta);
		}
		break;
	case Drag_Resize_Top:
		{
			SetPosition( int2(mDragBeginPos.X(), mDragBeginPos.Y() + delta.Y()));
			SetSize( int2(mDragBeginSize.X(), mDragBeginSize.Y() - delta.Y()) );
		}
		break;
	case Drag_Resize_TopRight:
		{
			SetPosition( int2(mDragBeginPos.X(), mDragBeginPos.Y() + delta.Y()));
			SetSize( int2(mDragBeginSize.X() + delta.X(), mDragBeginSize.Y() - delta.Y()) );
		}
		break;
	case Drag_Resize_BottomRight:
		{
			SetSize( mDragBeginSize + delta );
		}
		break;
	case Drag_Resize_Bottom:
		{
			SetSize( int2(mDragBeginSize.X(), mDragBeginSize.Y() + delta.Y()) );
		}
		break;
	case Drag_Resize_BottomLeft:
		{
			SetPosition( int2(mDragBeginPos.X() + delta.X(), mDragBeginPos.Y()));
			SetSize( int2(mDragBeginSize.X() - delta.X(), mDragBeginSize.Y() + delta.Y()) );
		}
		break;
	case Drag_Resize_Left:
		{
			SetPosition( int2(mDragBeginPos.X() + delta.X(), mDragBeginPos.Y()));
			SetSize( int2(mDragBeginSize.X() - delta.X(), mDragBeginSize.Y()) );
		}
		break;
	case Drag_Resize_Right:
		{
			SetSize( int2(mDragBeginPos.X() + delta.X(), mDragBeginPos.Y()) );
		}
		break;
	default:
		break;
	}

	ValidatePosition();
	//SetCursorShape(dragMode_, cursor);
}


void UIWindow::OnDragEnd( const int2& position )
{
	mDragMode = Drag_None;
}

void UIWindow::Minimize()
{
	mLastNormalPos = GetPosition();
	mLastNormalSize = GetSize();
		
	// invisible all child
	for (auto iter = mChildren.begin(); iter != mChildren.end(); ++iter)
	{
		UIElement* element = *iter;
		element->SetVisible(false);
	}

	mWindowState = Minimized;
	mMinimizing = true;
	
	//SetPosition()
	//SetSize();
}

UIWindow::DragMode UIWindow::GetDragMode( const int2& position )
{
	DragMode mode = Drag_None;

	const int2& windowSize = GetSize();
	const int32_t windowWidth = windowSize.X();
	const int32_t windowHeight = windowSize.Y();

	// Top row
	if (position.Y() < mResizeBorderSize)
	{
		if (mMovable)
			mode = Drag_Move;

		if (mResizable)
		{
			mode = Drag_Resize_Top;

			if (position.X() < mResizeBorderSize)
				mode = Drag_Resize_TopLeft;

			if (position.X() >= windowWidth - mResizeBorderSize)
				mode = Drag_Resize_TopRight;
		}
	}
	// Bottom row
	else if (position.Y() >= windowHeight - mResizeBorderSize)
	{
		if (mMovable)
			mode = Drag_Move;

		if (mResizable)
		{
			mode = Drag_Resize_Bottom;

			if (position.X() < mResizeBorderSize)
				mode = Drag_Resize_BottomLeft;

			if (position.X() >= windowWidth - mResizeBorderSize)
				mode = Drag_Resize_BottomRight;
		}
	}
	// Middle
	else
	{
		if (mMovable)
			mode = Drag_Move;

		if (mResizable)
		{
			if (position.X() < mResizeBorderSize)
				mode = Drag_Resize_Left;

			if (position.X() >= windowWidth - mResizeBorderSize)
				mode = Drag_Resize_Right;
		}
	}

	return mode;
}

void UIWindow::ValidatePosition()
{
	if (!mParent)
		return;

	const int2& parentSize = mParent->GetSize();
	int2 position = GetPosition();
	int2 halfSize = GetSize() / 2;

	position.X() = Clamp(position.X(), -halfSize.X(), parentSize.X() - halfSize.X());
	position.Y() = Clamp(position.X(), -halfSize.Y(), parentSize.Y() - halfSize.Y());

	SetPosition(position);
}

void UIWindow::Update( float delta )
{

}

void UIWindow::Draw( SpriteBatch& spriteBatch )
{

}


}
