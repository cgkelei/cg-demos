#include <GUI/UIManager.h>
#include <GUI/UIElement.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Input/InputSystem.h>
#include <Input/InputEvent.h>
#include <Core/Context.h>

namespace RcEngine {

SINGLETON_DECL(UIManager)

UIManager::UIManager()
	: mDragElement(nullptr), mFocusElement(nullptr), mRootElement(nullptr), mMainWindow(nullptr),
	  mInitialize(false)
{

}

UIManager::~UIManager()
{
	SAFE_DELETE(mRootElement);
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

void UIManager::Update( float delta )
{
	/*bool mouseVisible = Context::GetSingleton().GetApplication().GetMainWindow()->IsMouseVisible();
	
	int2 mousePos = InputSystem::GetSingleton().GetMousePos();

	if (mouseVisible)
	{
		UIElement* element = GetElementFromPoint(mousePos);

		if (element && element->IsEnabled())
			element->OnMouseHover(element->ScreenToClient(mousePos));
	}*/

}

bool UIManager::OnEvent( const InputEvent& event )
{
	bool eventConsumed = false;

	switch (event.EventType)
	{
	case InputEventType::MouseButtonDown:
		{
			eventConsumed = HandleMousePress(
				int2(int(event.MouseButton.x), int(event.MouseButton.y)), 
				event.MouseButton.button);
		}
		break;
	case InputEventType::MouseButtonUp:
		{
			eventConsumed = HandleMouseRelease(
				int2(int(event.MouseButton.x), int(event.MouseButton.y)), 
				event.MouseButton.button);
		}
		break;
	case InputEventType::MouseMove:
		{
			eventConsumed = HandleMouseMove(
				int2(int(event.MouseMove.x), int(event.MouseMove.y)), 
				event.MouseMove.buttons);
		}
		break;
	case InputEventType::MouseWheel:
		{
			eventConsumed = HandleMouseWheel(
				int2(int(event.MouseWheel.x), int(event.MouseWheel.y)), 
				event.MouseWheel.wheel);
		}
		break;
	case InputEventType::KeyDown:
		{
			eventConsumed = HandleKeyPress(event.Key.key);
		}
		break;
	case InputEventType::KeyUp:
		{
			eventConsumed = HandleKeyPress(event.Key.key);
		}
		break;
	case InputEventType::Char:
		{
			eventConsumed = HandleKeyPress(event.Char.unicode);
		}
		break;
	default:
		break;
	}

	return eventConsumed;

}

bool UIManager::HandleMousePress( const int2& pos, uint32_t button )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	if (!mouseVisible)
		return false;
	
	bool eventConsumed = false;

	// Get UIElemet current mouse cover
	UIElement* element = GetElementFromPoint(pos);

	if (element && element->IsVisible())
	{
		 // Handle focusing, bringing to front
		if (button == MS_LeftButton)
		{
			SetFocusElement(element);
			element->BringToFront();
		}

		eventConsumed = element->OnClick(element->ScreenToClient(pos), button);

		// if Onclick does't consume event, handle drag 
		if (!eventConsumed && !mDragElement && button == MS_LeftButton)
		{
			mDragElement = element;
			element->OnDragBegin(element->ScreenToClient(pos), button);
		}

		// Mouse position is in UI region, So there is no need to pass event to other Game Objects
		eventConsumed = true;

	}
	else
	{
		// If clicked over no element, or a disabled element, lose focus
		SetFocusElement(NULL);
		eventConsumed = false;
	}	

	return eventConsumed;
}

bool UIManager::HandleMouseRelease( const int2& pos, uint32_t button )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	if (!mouseVisible)
		return false;

	bool eventConsumed = false;

	if (mDragElement)
	{
		if (mDragElement->IsEnabled() && mDragElement->IsVisible())
		{
			mDragElement->OnDragEnd(mDragElement->ScreenToClient(pos));
		}

		mDragElement = nullptr;
		eventConsumed = true;
	}

	return eventConsumed;
}

bool UIManager::HandleMouseMove( const int2& pos, uint32_t buttons )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	if (!mouseVisible)
		return false;

	bool eventConsumed = false;

	if (mDragElement && buttons == MS_LeftButton)
	{
		if (mDragElement->IsEnabled() && mDragElement->IsVisible())
		{
			mDragElement->OnDragMove(mDragElement->ScreenToClient(pos), buttons);
		}
		else
		{
			mDragElement->OnDragEnd(mDragElement->ScreenToClient(pos));
			mDragElement = nullptr;
		}	

		// Event consumed 
		eventConsumed = true;
	}

	return eventConsumed;
}

bool UIManager::HandleMouseWheel( const int2& pos, int32_t delta )
{
	bool eventConsumed = false;

	if (mFocusElement)
	{
		eventConsumed =  mFocusElement->OnMouseWheel(delta);
	}
	else
	{
		bool mouseVisible = mMainWindow->IsMouseVisible();

		 // If no element has actual focus, get the element at cursor
		if (mouseVisible)
		{
			UIElement* element = GetElementFromPoint(pos);

			if (element && element->IsVisible())
			{
				element->OnMouseWheel(delta);

				// mouse pos is in GUI region
				eventConsumed =  true;
			}
		}
	}

	return eventConsumed;
}

bool UIManager::HandleKeyPress( uint16_t key )
{
	bool eventConsumed = false;

	UIElement* element = GetFocusElement();
	if (element)
	{
		eventConsumed = true;

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
								return eventConsumed;
							}
							else
								next =  (next + 1) % children.size();
						}

						// no other focusable control
						break;
					}			
				}
			}
		}
		else // If none of the special keys, pass the key to the focused element			
			element->OnKeyPress(key);		
	}

	return eventConsumed;
}

bool UIManager::HandleKeyRelease( uint16_t key )
{
	return false;
}

bool UIManager::HandleTextInput( uint16_t unicode )
{
	bool eventConsumed = false;

	UIElement* element = GetFocusElement();
	if (element)
	{
		eventConsumed = element->OnTextInput(unicode);
		eventConsumed = true;
	}

	return eventConsumed;
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

void UIManager::OnGraphicsInitialize()
{
	if (!mInitialize)
	{
		//Keep track of main window
		mMainWindow = Context::GetSingleton().GetApplication().GetMainWindow();

		mRootElement = new UIElement();
		mRootElement->SetSize(int2(mMainWindow->GetWidth(), mMainWindow->GetHeight()));

		mInitialize = true;
	}
}

void UIManager::OnWindowResize( uint32_t width, uint32_t height )
{
	if (mRootElement)
	{
		mRootElement->SetSize(int2(width, height));
	}
}

}