#include <GUI/UIWindow.h>
#include <GUI/UIManager.h>
#include <Math/Math.h>
#include <Input/InputDevice.h>

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

void UIWindow::OnDragBegin( const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers )
{
	if (buttons != MS_LeftButton)
	{
		mDragMode = Drag_None;
		return ;
	}

	mDragMode = GetDragMode(position);

	//SetCursorShape(dragMode_, cursor);

}

void UIWindow::OnDragMove( const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers )
{
	if (mDragMode == Drag_None)
		return;

	const IntVector2& originalScreenPos = GetScreenPosition();
	const IntVector2& originalSize = GetSize();
	const IntVector2& originalPos = GetPosition();

	IntVector2 delta = screenPosition - originalScreenPos;

	switch (mDragMode)
	{
	case Drag_Move:
		{
			SetPosition(originalPos + delta);
		}
		break;
	case Drag_Resize_TopLeft:
		{
			SetPosition(originalPos + delta);
			SetSize(originalSize - delta);
		}
		break;
	case Drag_Resize_Top:
		{
			// only affect topleft y 
			SetPosition( IntVector2(originalPos.X(), originalPos.Y() + delta.Y()));
			SetSize( IntVector2(originalSize.X(), originalSize.Y() - delta.Y()) );
		}
		break;
	case Drag_Resize_TopRight:
		{
			SetPosition( IntVector2(originalPos.X(), originalPos.Y() + delta.Y()));
			SetSize( IntVector2(originalSize.X() + delta.X(), originalSize.Y() - delta.Y()) );
		}
		break;
	case Drag_Resize_BottomRight:
		{
			SetSize( originalSize + delta );
		}
		break;
	case Drag_Resize_Bottom:
		{
			SetSize( IntVector2(originalSize.X(), originalSize.Y() + delta.Y()) );
		}
		break;
	case Drag_Resize_BottomLeft:
		{
			SetPosition( IntVector2(originalPos.X() + delta.X(), originalPos.Y()));
			SetSize( IntVector2(originalSize.X() - delta.X(), originalSize.Y() + delta.Y()) );
		}
		break;
	case Drag_Resize_Left:
		{
			SetPosition( IntVector2(originalPos.X() + delta.X(), originalPos.Y()));
			SetSize( IntVector2(originalSize.X() - delta.X(), originalSize.Y()) );
		}
		break;
	case Drag_Resize_Right:
		{
			SetSize( IntVector2(originalSize.X() + delta.X(), originalSize.Y()) );
		}
		break;
	default:
		break;
	}

	ValidatePosition();

}

void UIWindow::OnDragEnd( const IntVector2& position, const IntVector2& screenPosition )
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

UIWindow::DragMode UIWindow::GetDragMode( const IntVector2& position )
{
	DragMode mode = Drag_None;

	const IntVector2& windowSize = GetSize();
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
	{
		return;
	}

	const IntVector2& parentSize = mParent->GetSize();
	IntVector2 position = GetPosition();
	IntVector2 halfSize = GetSize() / 2;

	position.X() = Math<IntVector2::value_type>::Clamp(position.X(), -halfSize.X(), parentSize.X() - halfSize.X());
	position.Y() = Math<IntVector2::value_type>::Clamp(position.X(), -halfSize.Y(), parentSize.Y() - halfSize.Y());

	SetPosition(position);
}

void UIWindow::Draw()
{
	UIManager& uiMan = UIManager::GetSingleton();
}




}
