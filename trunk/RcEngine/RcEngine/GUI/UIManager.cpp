#include <GUI/UIManager.h>
#include <GUI/UIElement.h>
#include <GUI/GuiSkin.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Graphics/Font.h>
#include <Graphics/Material.h>
#include <Graphics/SpriteBatch.h>
#include <Input/InputSystem.h>
#include <Input/InputEvent.h>
#include <Resource/ResourceManager.h>
#include <Core/Context.h>

namespace RcEngine {

SINGLETON_DECL(UIManager)

UIManager::UIManager()
	: mDragElement(nullptr), mFocusElement(nullptr), mRootElement(nullptr), mMainWindow(nullptr),
	  mInitialize(false), mDefaultSkin(nullptr)
{

}

UIManager::~UIManager()
{
	SAFE_DELETE(mRootElement);
	SAFE_DELETE(mDefaultSkin);
	mFocusElement = mDragElement = nullptr;
}


void UIManager::OnGraphicsInitialize()
{
	if (!mInitialize)
	{
		//Keep track of main window
		mMainWindow = Context::GetSingleton().GetApplication().GetMainWindow();

		mRootElement = new UIElement();
		mRootElement->SetSize(int2(mMainWindow->GetWidth(), mMainWindow->GetHeight()));

		ResourceManager& resMan = ResourceManager::GetSingleton();

		mFont = std::static_pointer_cast<Font>(resMan.GetResourceByName(RT_Font,"Consolas Regular", "General"));

		mSpriteBatch = std::make_shared<SpriteBatch>();

		mSpriteBatchFont = std::make_shared<SpriteBatch>( 
			std::static_pointer_cast<Material>(resMan.GetResourceByName(RT_Material, "Font.material.xml", "General")) );

		mInitialize = true;
	}
}

void UIManager::OnGraphicsFinalize()
{
	mSpriteBatch.reset();
	mFont.reset();
	mSpriteBatchFont.reset();
	SAFE_DELETE(mDefaultSkin);
}

void UIManager::OnWindowResize( uint32_t width, uint32_t height )
{
	if (mRootElement)
	{
		mRootElement->SetSize(int2(width, height));
	}
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

	if (element && element->CanHaveFocus())
	{
		mFocusElement = element;
	}
}

UIElement* UIManager::GetElementAtPoint( const int2& pos )
{
	UIElement* result = 0;
	GetElementAtPoint(result, mRootElement, pos);
	return result;
}

UIElement* UIManager::GetFocusableElement( UIElement* element )
{
	while (element)
	{
		if (element->CanHaveFocus())
			break;
		element = element->GetParent();
	}
	return element;
}

void UIManager::Update( float delta )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	int2 mousePos = InputSystem::GetSingleton().GetMousePos();

	if (mouseVisible)
	{
		UIElement* element = GetElementAtPoint(mousePos);

		if (element && !mDragElement)
			element->OnHover(mousePos);			
	}

	Update(mRootElement, delta);
}

void UIManager::Update( UIElement* element, float dt )
{
	element->Update(dt);
	
	for (auto& child : element->GetChildren())
		Update(child, dt);
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

bool UIManager::HandleMousePress( const int2& screenPos, uint32_t button )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	if (!mouseVisible)
		return false;
	
	bool eventConsumed = false;

	// Get UIElemet current mouse cover
	UIElement* element = GetElementAtPoint(screenPos);

	if (element && element->IsVisible() && element->IsEnabled())
	{
		 // Handle focusing, bringing to front
		if (button == MS_LeftButton)
		{
			SetFocusElement(element);
			element->BringToFront();
		}

		eventConsumed = element->OnMouseButtonPress(screenPos, button);

		// if button press doesn't consume event, handle drag 
		if (!eventConsumed && !mDragElement && button == MS_LeftButton)
		{
			mDragElement = element;
			element->OnDragBegin(screenPos, button);
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

bool UIManager::HandleMouseRelease( const int2& screenPos, uint32_t button )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	if (!mouseVisible)
		return false;

	bool eventConsumed = false;

	if (mDragElement)
	{
		if (mDragElement->IsEnabled() && mDragElement->IsVisible())
		{
			mDragElement->OnDragEnd(screenPos);
		}

		mDragElement = nullptr;
		eventConsumed = true;
	}
	else
	{
		if (mFocusElement)
		{
			eventConsumed = mFocusElement->OnMouseButtonRelease(screenPos, button);
			eventConsumed = true;
		}
	}

	return eventConsumed;
}

bool UIManager::HandleMouseMove( const int2& screenPos, uint32_t buttons )
{
	bool mouseVisible = mMainWindow->IsMouseVisible();

	if (!mouseVisible)
		return false;

	bool eventConsumed = false;

	if (mDragElement && (buttons & InputEventType::LeftButtonMask) )
	{
		if (mDragElement->IsEnabled() && mDragElement->IsVisible())
		{
			mDragElement->OnDragMove(screenPos, buttons);
		}
		else
		{
			mDragElement->OnDragEnd(screenPos);
			mDragElement = nullptr;
		}	

		// Event consumed 
		eventConsumed = true;
	}

	return eventConsumed;
}

bool UIManager::HandleMouseWheel( const int2& screenPos, int32_t delta )
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
			UIElement* element = GetElementAtPoint(screenPos);

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
		//if (key == KC_Tab)
		//{
		//	UIElement* topLevel = element->GetParent();
		//	while(topLevel && topLevel->GetParent() != mRootElement)
		//		topLevel = topLevel->GetParent();

		//	if (topLevel)
		//	{
		//		vector<UIElement*> children;
		//		topLevel->FlattenChildren(children);

		//		for (size_t i = 0; i < children.size(); ++i)
		//		{
		//			if ( element == children[i] )
		//			{
		//				size_t next = (i + 1) % children.size();

		//				while( next != i )
		//				{
		//					UIElement* nextElement = children[next];
		//					FocusMode childFocusPolicy = nextElement->GetFocusMode();

		//					if (childFocusPolicy & FM_TabFocus)
		//					{
		//						SetFocusElement(nextElement);
		//						return eventConsumed;
		//					}
		//					else
		//						next =  (next + 1) % children.size();
		//				}

		//				// no other focusable control
		//				break;
		//			}			
		//		}
		//	}
		//}
		//else // If none of the special keys, pass the key to the focused element			
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

void UIManager::GetElementAtPoint( UIElement*& result, UIElement* current, const int2& pos )
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
				GetElementAtPoint(result, element, pos);
		}
	}
}

void UIManager::Render()
{
	if (mRootElement)
	{
		mSpriteBatchFont->Begin();
		mSpriteBatch->Begin();

		const int2& rootSize = mRootElement->GetSize();
		RenderUIElement(mRootElement, IntRect(0, 0, rootSize.X(), rootSize.Y()));
				
		mSpriteBatch->End();
		mSpriteBatchFont->End();
		
		mSpriteBatch->Flush();
		mSpriteBatchFont->Flush();
	}
}

void UIManager::RenderUIElement( UIElement* element, const IntRect& currentScissor )
{
	std::vector<UIElement*>& children = element->GetChildren();

	// If parent container is not visible, not draw children
	if (!element->IsVisible())
		return;

	// Draw container first
	element->Draw(*mSpriteBatch, *mSpriteBatchFont);

	for (UIElement* child : children)
	{
		RenderUIElement(child, currentScissor);
	}	
}

GuiSkin* UIManager::GetDefaultSkin()
{
	// todo: xml gui skin
	if (!mDefaultSkin)
	{
		mDefaultSkin = new GuiSkin;

		mDefaultSkin->BackColor = ColorRGBA(1, 1, 1, 100.0f / 255);
		mDefaultSkin->ForeColor = ColorRGBA(1, 1, 1, 200.0f / 255);

		mDefaultSkin->mFont = mFont;
		mDefaultSkin->mFontSize = 20;

		ResourceManager& resMan = ResourceManager::GetSingleton();
		mDefaultSkin->mSkinTexAtlas = std::static_pointer_cast<TextureResource>(
			resMan.GetResourceByName(RT_Texture,"dxutcontrols.dds", "General"))->GetTexture();

		// Button
		for (int i = 0; i < UI_State_Count; ++i)
		{
			mDefaultSkin->Button.StyleStates[i].TexRegion = IntRect(0, 0, 136, 54);
			mDefaultSkin->Button.StyleStates[i].TexColor = ColorRGBA(1, 1, 1, 150.0f / 255);
		}

		mDefaultSkin->Button.StyleStates[UI_State_Hover].TexRegion = IntRect(136, 0, 252 - 136, 54);

		// Check Box
		mDefaultSkin->CheckBox[0].TexRegion = IntRect(0, 54, 27, 27); 
		mDefaultSkin->CheckBox[0].TexColor = ColorRGBA(1, 1, 1, 150.0f / 255);
		mDefaultSkin->CheckBox[1].TexRegion = IntRect(27, 54, 27, 27);
		mDefaultSkin->CheckBox[1].TexColor = ColorRGBA(1, 1, 1, 150.0f / 255);

		// Slider 
		mDefaultSkin->HSliderTrack[0].TexRegion.SetLeft(1);
		mDefaultSkin->HSliderTrack[0].TexRegion.SetRight(93);
		mDefaultSkin->HSliderTrack[0].TexRegion.SetTop(187);
		mDefaultSkin->HSliderTrack[0].TexRegion.SetBottom(228);
		mDefaultSkin->HSliderTrack[0].TexColor = ColorRGBA(0.5, 0.5, 0.5, 150.0f / 255);

		mDefaultSkin->HSliderTrack[1].TexRegion.SetLeft(1);
		mDefaultSkin->HSliderTrack[1].TexRegion.SetRight(93);
		mDefaultSkin->HSliderTrack[1].TexRegion.SetTop(187);
		mDefaultSkin->HSliderTrack[1].TexRegion.SetBottom(228);
		mDefaultSkin->HSliderTrack[1].TexColor = ColorRGBA(1, 1, 1, 150.0f / 255);

		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Normal].TexRegion.SetLeft(151);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Normal].TexRegion.SetRight(192);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Normal].TexRegion.SetTop(193);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Normal].TexRegion.SetBottom(234);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Normal].TexColor = ColorRGBA(1, 1, 1, 255.0 / 255);

		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Hover].TexRegion.SetLeft(151);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Hover].TexRegion.SetRight(192);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Hover].TexRegion.SetTop(193);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Hover].TexRegion.SetBottom(234);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Hover].TexColor = ColorRGBA(1, 0, 1, 255.0 / 255);

		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Pressed].TexRegion.SetLeft(151);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Pressed].TexRegion.SetRight(192);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Pressed].TexRegion.SetTop(193);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Pressed].TexRegion.SetBottom(234);
		mDefaultSkin->SliderThumb[UI_Horizontal].StyleStates[UI_State_Pressed].TexColor = ColorRGBA(1, 1, 1, 255.0 / 255);

	}


	return mDefaultSkin;
}

}