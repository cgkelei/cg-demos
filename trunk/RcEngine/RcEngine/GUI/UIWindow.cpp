#include <GUI/UIWindow.h>
#include <GUI/UIManager.h>
#include <GUI/Button.h>
#include <Math/Math.h>
#include <Input/InputSystem.h>

static const int DEFAULT_RESIZE_BORDER = 4;

namespace RcEngine {

UIWindow::UIWindow()
	: mBorderThickness(DEFAULT_RESIZE_BORDER),
	  mWindowState(UIWindow::Normal),
	  mDragMode(UIWindow::Drag_None),
	  mMovable(true)
{
	mCloseBtn = new Button();
	mCloseBtn->SetVisible(false);
	mCloseBtn->SetHoverOffset(int2::Zero());
	mCloseBtn->SetPressedOffset(int2::Zero());
	mCloseBtn->EventButtonClicked.bind(this, &UIWindow::Close);

	mMinimizeBtn = new Button();
	mMinimizeBtn->SetVisible(false);
	mMinimizeBtn->SetHoverOffset(int2::Zero());
	mMinimizeBtn->SetPressedOffset(int2::Zero());
	mMinimizeBtn->EventButtonClicked.bind(this, &UIWindow::Minimize);

	mMaximizeBtn = new Button();
	mMaximizeBtn->SetVisible(false);
	mMaximizeBtn->SetHoverOffset(int2::Zero());
	mMaximizeBtn->SetPressedOffset(int2::Zero());
	mMaximizeBtn->EventButtonClicked.bind(this, &UIWindow::Maximize);

	mRestoreBtn = new Button();
	mRestoreBtn->SetVisible(false);
	mRestoreBtn->SetHoverOffset(int2::Zero());
	mRestoreBtn->SetPressedOffset(int2::Zero());
	mRestoreBtn->EventButtonClicked.bind(this, &UIWindow::Restore);

	AddChild(mCloseBtn);
	AddChild(mMinimizeBtn);
	AddChild(mMaximizeBtn);
	AddChild(mRestoreBtn);

	// Set default border style
	SetBorderStyle(UIWindow::Border_Resizable);
}

UIWindow::~UIWindow()
{

}

void UIWindow::SetMovable( bool movable )
{
	mMovable = movable;
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
	case Drag_Resize_Bottom:
		{
			SetSize( int2(mDragBeginSize.X(), mDragBeginSize.Y() + delta.Y()) );
		}		
		break;
	case Drag_Resize_BottomRight:
		{
			SetSize( mDragBeginSize + delta );
		}
		break;
	case Drag_Resize_Right:
		{
			SetSize( int2(mDragBeginPos.X() + delta.X(), mDragBeginPos.Y()) );
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
	if (position.Y() < mBorderThickness)
	{
		if (mMovable)
			mode = Drag_Move;

		if (IsResizable())
		{
			mode = Drag_Resize_Top;

			if (position.X() < mBorderThickness)
				mode = Drag_Resize_TopLeft;

			if (position.X() >= windowWidth - mBorderThickness)
				mode = Drag_Resize_TopRight;
		}
	}
	// Bottom row
	else if (position.Y() >= windowHeight - mBorderThickness)
	{
		if (IsResizable())
		{
			mode = Drag_Resize_Bottom;

			if (position.X() < mBorderThickness)
				mode = Drag_Resize_BottomLeft;

			if (position.X() >= windowWidth - mBorderThickness)
				mode = Drag_Resize_BottomRight;
		}
	}
	// Middle
	else
	{
		if (mMovable)
			mode = Drag_Move;

		if (IsResizable())
		{
			if (position.X() < mBorderThickness)
				mode = Drag_Resize_Left;

			if (position.X() >= windowWidth - mBorderThickness)
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

void UIWindow::InitGuiStyle( const GuiSkin::StyleMap* styles /*= nullptr*/ )
{

}

void UIWindow::SetBorderThickness( int32_t thickness )
{

}

void UIWindow::UpdateRect()
{
	
}

void UIWindow::SetBorderStyle( BorderStyle style )
{
	switch (style)
	{
	case Border_None:
		{
			mCloseBtn->SetEnable(false);
			mCloseBtn->SetVisible(false);

			mMinimizeBtn->SetEnable(false);
			mMinimizeBtn->SetVisible(false);

			mMaximizeBtn->SetEnable(false);
			mMaximizeBtn->SetVisible(false);

			mRestoreBtn->SetEnable(false);
			mRestoreBtn->SetVisible(false);

			SetBorderThickness(0);
		}
		break;

	case Border_Fixed:
		{
			mCloseBtn->SetEnable(true);
			mCloseBtn->SetVisible(true);

			mMinimizeBtn->SetEnable(false);
			mMinimizeBtn->SetVisible(false);

			mMaximizeBtn->SetEnable(false);
			mMaximizeBtn->SetVisible(false);

			mRestoreBtn->SetEnable(false);
			mRestoreBtn->SetVisible(false);
		}
		break;

	case Border_Resizable:
	default:
		{
			mCloseBtn->SetEnable(true);
			mCloseBtn->SetVisible(true);

			mMinimizeBtn->SetEnable(true);
			mMinimizeBtn->SetVisible(true);

			mMaximizeBtn->SetEnable(true);
			mMaximizeBtn->SetVisible(true);

			mRestoreBtn->SetEnable(true);
			mRestoreBtn->SetVisible(true);
		}
		break;
	}
}

//void UIWindow::DrawButtons()
//{
//	if (mBorderStyle != Border_None)
//	{
//		mCloseBtn->setp
//
//		mb.Left = Size.X - 22;
//		btClose.Top = 4;
//		btClose.Draw(spriteBatch, Position);
//	}
//
//	if (this.style == BorderStyle.Sizable)
//	{
//		if (hasMaximizeButton)
//		{
//			if (state != WindowState.Maximized)
//			{
//				btMaximize.Left = Size.X - 37;
//				btMaximize.Top = 4;
//				btMaximize.Draw(spriteBatch, Position);
//			}
//			else
//			{
//				btRestore.Left = Size.X - 37;
//				btRestore.Top = 4;
//				btRestore.Draw(spriteBatch, Position);
//			}
//
//			if (hasMinimizeButton)
//			{
//				if (state != WindowState.Minimized)
//				{
//					btMinimize.Left = Size.X - 52;
//					btMinimize.Top = 4;
//					btMinimize.Draw(spriteBatch, Position);
//				}
//				else
//				{
//					btRestore.Left = Size.X - 52;
//					btRestore.Top = 4;
//					btRestore.Draw(spriteBatch, Position);
//				}
//			}
//		}
//		else if (hasMinimizeButton)
//		{
//			if (state != WindowState.Minimized)
//			{
//				btMinimize.Left = Size.X - 37;
//				btMinimize.Top = 4;
//				btMinimize.Draw(spriteBatch, Position);
//			}
//			else
//			{
//				btRestore.Left = Size.X - 37;
//				btRestore.Top = 4;
//				btRestore.Draw(spriteBatch, Position);
//			}
//		}
//}
//
//
//}

}