#include <GUI/UIManager.h>
#include <GUI/UIElement.h>
#include <Input/InputSystem.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Core/Context.h>

namespace RcEngine {

SINGLETON_DECL(UIManager)

UIManager::UIManager()
	: mDragElement(nullptr),
	  mFocusElement(nullptr),
	  mRootElement(nullptr)
{
	
	InputAction actions[] =
	{
		InputAction(MS_X,          MouseMove),
		InputAction(MS_Y,          MouseMove),
		InputAction(MS_LeftButton, MouseUpDown),
	};

	//if (inputSystem)
	{
		//inputSystem->AddAction(actions, actions+ sizeof(actions)/ sizeof(InputAction));
		//inputSystem->AddStateHandler(Forward, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleMove));
		//inputSystem->AddStateHandler(Backward, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleMove));
		//inputSystem->AddStateHandler(MoveLeft, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleMove));
		//inputSystem->AddStateHandler(MoveRight, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleMove));
	}


}

UIManager::~UIManager()
{

}

void UIManager::SetFocusElement( UIElement* element )
{
	if (element)
	{
		if (mFocusElement == element)
			return;

		// Search for an element in the hierarchy that can alter focus. If none found, exit
		element = GetFocusableElement(element);
		if (!element)
			return;
	}

	// Remove focus from the old element
	if (mFocusElement)
		mFocusElement = nullptr;

	if (element && element->GetFocusMode() != FM_NoFocus)
	{
		mFocusElement = element;
	}
}


void UIManager::Update( float delta )
{
	bool mouseVisible = Context::GetSingleton().GetApplication().GetMainWindow()->IsMouseVisible();
	
	int2 mousePos = InputSystem::GetSingleton().GetMousePos();

	if (mouseVisible)
	{
		UIElement* element = GetElementFromPoint(mousePos);

		if (element && element->IsEnabled())
			element->OnMouseHover(element->ScreenToClient(mousePos), mousePos, 0, 0);
	}

}

void UIManager::HandleKeyPress( uint8_t key, uint32_t qualifiers )
{
	UIElement* element = GetFocusElement();
	if (element)
	{
		// Switch focus between focusable elements in the same top level window
		if (key == KC_Tab)
		{
			UIElement* topLevel = element->GetParent();
			while(topLevel && topLevel->GetParent() != mRootElement)
				topLevel = topLevel->GetParent();

			if (topLevel)
			{
				vector<UIElement*> children;
				topLevel->FlattenChildren(children);

				for (size_t i = 0; i < children.size(); ++i)
				{
					if ( element == children[i] )
					{
						size_t next = (i + 1) % children.size();

						while( next != i )
						{
							UIElement* nextElement = children[next];
							FocusMode childFocusPolicy = nextElement->GetFocusMode();

							if (childFocusPolicy & FM_TabFocus)
							{
								SetFocusElement(nextElement);
								return;
							}
							else
								next =  (next + 1) % children.size();
						}

						// no other focusable control
						break;
					}			
				}
			}
			else // If none of the special keys, pass the key to the focused element			
				element->OnKeyPress(key, qualifiers);
		}
	}
}

void UIManager::HandleKeyRelease( uint8_t key, uint32_t qualifiers )
{

}

void UIManager::HandleMousePress( const int2& pos, uint32_t buttons, int qualifiers )
{
	bool mouseVisible = Context::GetSingleton().GetApplication().GetMainWindow()->IsMouseVisible();

	if (mouseVisible)
	{
		UIElement* element = GetElementFromPoint(pos);

		if (element)
		{
			if (buttons == MS_LeftButton)
			{
				SetFocusElement(element);
				element->BringToFront();
			}

			element->OnClick(element->ScreenToClient(pos), pos, buttons, qualifiers);

			// Handle start of drag. OnClick() may have caused destruction of the element, so check the pointer again
			if (element && !mDragElement && buttons == MS_LeftButton)
			{
				mDragElement = element;
				element->OnDragBegin(element->ScreenToClient(pos), pos, buttons, qualifiers);
			}
		}
		else
		{
			// If clicked over no element, or a disabled element, lose focus
			SetFocusElement(NULL);
		}	
	}
}

void UIManager::HandleMouseRelease( const int2& pos, uint32_t buttons, int qualifiers )
{
	bool mouseVisible = Context::GetSingleton().GetApplication().GetMainWindow()->IsMouseVisible();

	if (mouseVisible)
	{
		if (mDragElement)
		{
			if (mDragElement->IsEnabled() && mDragElement->IsVisible())
				mDragElement->OnDragEnd(mDragElement->ScreenToClient(pos), pos);
		}
	}
}


void UIManager::HandleMouseMove( const int2& pos, uint32_t buttons, int qualifiers )
{
	bool mouseVisible = Context::GetSingleton().GetApplication().GetMainWindow()->IsMouseVisible();

	if (mouseVisible && mDragElement && buttons)
	{
		if (mDragElement->IsEnabled() && mDragElement->IsVisible())
			mDragElement->OnDragMove(mDragElement->ScreenToClient(pos), pos, buttons, qualifiers);

		mDragElement = nullptr;
	}
}


void UIManager::HandleMouseWheel( int32_t delta, uint32_t buttons, uint32_t qualifiers )
{
	if (mFocusElement)
	{
		mFocusElement->OnMouseWheel(delta, buttons, qualifiers);
	}
}

void UIManager::GetElementFromPoint(UIElement*& result, UIElement* current, const int2& pos )
{
	if (!current)
		return;

	for ( UIElement* element : current->GetChildren() )
	{
		bool hasChildren = (element->GetNumChildren() > 0);

		if (element->IsVisible())
		{
			if (element->IsInside(pos, true) && element->IsEnabled())
				result = element;

			if (hasChildren)
				GetElementFromPoint(result, element, pos);
		}
	}

}

UIElement* UIManager::GetElementFromPoint( const int2& pos )
{
	UIElement* result = 0;
	GetElementFromPoint(result, mRootElement, pos);
	return result;
}

UIElement* UIManager::GetFocusableElement( UIElement* element )
{
	while (element)
	{
		if (element->GetFocusMode() != FM_NoFocus)
			break;
		element = element->GetParent();
	}
	return element;
}

bool UIManager::OnEvent( const InputEvent& event )
{
	bool eventConsumed = false;



	return eventConsumed;

}



}