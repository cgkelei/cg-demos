#include <GUI/UIManager.h>
#include <GUI/UIElement.h>
#include <Input/InputDevice.h>

namespace RcEngine {

SINGLETON_DECL(UIManager)

UIManager::UIManager()
{

}

UIManager::~UIManager()
{

}

void UIManager::HandleKeyDown( uint8_t key, uint32_t qualifiers )
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
				topLevel->GetChildren(children, true);

				for (size_t i = 0; i < children.size(); ++i)
				{
					if ( element == children[i])
					{
						size_t next = (i + 1) % children.size();
						
						while( next != i )
						{
							UIElement* nextElement = children[next];
							FocusPolicy childFocusPolicy = nextElement->GetFocusPolicy();

							if (childFocusPolicy & FP_TabFocus)
							{
								SetFocusElement(nextElement);
								return ;
							}
							else
							{
								next =  (next + 1) % children.size();
							}
						}

						// no other focusable control
						break;
					}			
				}
			}
		}
		else
		{
			// If none of the special keys, pass the key to the focused element
			element->OnKeyDown(key, qualifiers);
		}
	}
}

void UIManager::HandleKeyUp( uint8_t key, uint32_t qualifiers )
{

}

void UIManager::SetFocusElement( UIElement* element )
{

}

void UIManager::DrawButton( const IntRect& area, const String& text, UIElementState state )
{

}



}