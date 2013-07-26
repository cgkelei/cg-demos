#include <Input/InputSystem.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>
#include <Core/Context.h>

namespace RcEngine{

SINGLETON_DECL(InputSystem)
		
InputSystem::InputSystem()
	: mMouseMove(0, 0), mMousePos(0, 0)
{
	mKeyState.resize(MS_Button7);  // All for Key + Mouse + Joysticks
}

InputSystem::~InputSystem()
{
}

void InputSystem::FireEvent( const InputEvent& event )
{
	switch (event.EventType)
	{
	case InputEventType::KeyDown:
		{
			mKeyState[event.Key.key] = true;
			mJustPressed.push_back(event.Key.key);
		}
		break;

	case InputEventType::KeyUp:
		{
			mKeyState[event.Key.key] = false;
			mjustReleased.push_back(event.Key.key);
		}
		break;

	case InputEventType::MouseButtonDown:
		{
			mKeyState[event.MouseButton.button] = true;
			mJustPressed.push_back(event.MouseButton.button);
		}
		break;
	case InputEventType::MouseButtonUp:
		{
			mKeyState[event.MouseButton.button] = false;
			mjustReleased.push_back(event.MouseButton.button);
		}
		break;

	case InputEventType::MouseMove:
		{
			mMousePos.X() = event.MouseMove.x;
			mMousePos.Y() = event.MouseMove.y;
		}
		break;

	default:
		break;
	}

	mEventQueue.push(event);
}

bool InputSystem::PollEvent( InputEvent* event )
{
	if ( !mEventQueue.empty() )
	{
		*event = mEventQueue.front();
		mEventQueue.pop();
		return true;
	}

	return false;
}

void InputSystem::BeginEvents()
{
	mJustPressed.clear();
	mjustReleased.clear();
}

void InputSystem::EndEvents()
{
	Window* mainWindow = Context::GetSingleton().GetApplication().GetMainWindow();

	mMouseMove = mMousePos - mLastMousePos;

	if (mainWindow->IsMouseVisible())
	{
		mLastMousePos.X() = mainWindow->GetWidth() / 2;
		mLastMousePos.Y() =  mainWindow->GetHeight() / 2;

		mainWindow->ForceMouseToCenter();
	}
	else
	{
		mLastMousePos = mMousePos;
	}
}

bool InputSystem::KeyDown( KeyCode key ) const
{
	return mKeyState[key];
}

bool InputSystem::KeyPress( KeyCode key ) const
{
	for (size_t i = 0; i < mJustPressed.size(); ++i)
	{
		if (mJustPressed[i] == key)	
			return true;
	}

	return false;
}

bool InputSystem::KeyRelease( KeyCode key ) const
{
	for (size_t i = 0; i < mjustReleased.size(); ++i)
	{
		if (mjustReleased[i] == key)	
			return true;
	}

	return false;
}

bool InputSystem::MouseButtonDown( MouseCode button ) const
{
	return mKeyState[button];
}

bool InputSystem::MouseButtonPress( MouseCode button ) const
{
	for (size_t i = 0; i < mJustPressed.size(); ++i)
	{
		if (mJustPressed[i] == button)	
			return true;
	}

	return false;
}

bool InputSystem::MouseButtonRelease( MouseCode button ) const
{
	for (size_t i = 0; i < mjustReleased.size(); ++i)
	{
		if (mjustReleased[i] == button)	
			return true;
	}

	return false;
}

}