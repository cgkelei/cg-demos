#include <GUI/UIWindow.h>
#include <Input/InputDevice.h>

namespace RcEngine {


UIWindow::UIWindow()
{

}

UIWindow::~UIWindow()
{

}

void UIWindow::OnDragBegin( const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers )
{
	if (buttons != MS_LeftButton || )
	{
	}
}

void UIWindow::OnDragMove( const IntVector2& position, const IntVector2& screenPosition, int buttons, int qualifiers )
{

}

void UIWindow::OnDragEnd( const IntVector2& position, const IntVector2& screenPosition )
{

}

}
