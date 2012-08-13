#include "Input/InputSystem.h"
#include "Input/InputDevice.h"
#include "Core/Context.h"

namespace RcEngine{
namespace Input {

Input::InputDeviceType GetRawInputDevice(uint32_t key)
{
	if (key <= Input::KC_MediaSelect && key >= Input::KC_UnAssigned)
	{
		return Input::IDT_Keyboard;
	}
	else if (key >= Input::MS_X && key <= Input::MS_Button7)
	{
		return Input::IDT_Mouse;
	}

	return Input::IDT_Keyboard;
}

		
InputSystem::InputSystem()
	: mKeyboard(nullptr), mMouse(nullptr)
{
	Core::Context::GetSingleton().SetInputSystem(this);		
}

InputSystem::~InputSystem()
{
	Safe_Delete(mMouse);
	Safe_Delete(mKeyboard);
}

void InputSystem::Update(float delata)
{
	mMouse->Update();
	mKeyboard->Update();

	DispatchActions(delata);
	DispatchStates(delata);
	DispatchRanges(delata);
	
}

void InputSystem::DispatchActions(float delata) const
{
	for (auto iter = mActions.begin(); iter != mActions.end(); ++iter)
	{
		bool value = false;
		InputDeviceType deviceType = GetRawInputDevice(iter->first);
		switch(deviceType)
		{
		case IDT_Mouse:
			{
				value = mMouse->ButtonPress((MouseCode)iter->first);
			}
			break;
		case IDT_Keyboard:
			{
				value = mKeyboard->KeyPress((KeyCode)iter->first);
			}
			break;
		default:
			assert(false);
		}

		auto foundHandler = mActionHandlers.find(iter->second);
		if ( foundHandler != mActionHandlers.end())
		{
			if (!foundHandler->second.empty())
			{
				foundHandler->second(iter->second, value, delata);
			}
		}
	}
}

void InputSystem::DispatchStates(float delata) const
{
	for (auto iter = mStates.begin(); iter != mStates.end(); ++iter)
	{
		bool value = false;
		InputDeviceType deviceType = GetRawInputDevice(iter->first);
		switch(deviceType)
		{
		case IDT_Mouse:
			{
				value = mMouse->ButtonDown((MouseCode)iter->first);
			}
			break;
		case IDT_Keyboard:
			{
				value = mKeyboard->KeyDown((KeyCode)iter->first);
			}
			break;
		default:
			assert(false);
		}

		auto foundHandler = mStateHandlers.find(iter->second);
		if ( foundHandler != mStateHandlers.end())
		{
			if (!foundHandler->second.empty())
			{
				foundHandler->second(iter->second, value, delata);
			}
		}
	}
}

void InputSystem::DispatchRanges(float delata) const
{
	for (auto iter = mRanges.begin(); iter != mRanges.end(); ++iter)
	{
		int32_t value = false;
		InputDeviceType deviceType = GetRawInputDevice(iter->first);
		assert(deviceType == IDT_Mouse);

		switch(iter->first)
		{
		case MS_X:
			value = mMouse->X();
			break;
		case MS_Y:
			value = mMouse->Y();
			break;
		case MS_XDelta:
			value = mMouse->MouseMoveX();
			break;
		case MS_YDelta:
			value = mMouse->MouseMoveY();
			break;
		default:
			assert(false);
		}

		auto foundHandler = mRangeHandlers.find(iter->second);
		if ( foundHandler != mRangeHandlers.end())
		{
			if (!foundHandler->second.empty())
			{
				foundHandler->second(iter->second, (float)value, delata);
			}
		}
	}
}

void InputSystem::Resize( uint32_t width, uint32_t height )
{
	String name = mMouse->GetName();
	mMouse->SetWindowSize(width, height);
}

void InputSystem::AddActionHandler( uint32_t action, InputActionHandler handler )
{
	if (HasAction(action))
	{
		mActionHandlers[action] = handler;
	}
}

void InputSystem::AddStateHandler(uint32_t state, InputStateHandler handler)
{
	if (HasState(state))
	{
		mStateHandlers[state] = handler;
	}
}

void InputSystem::AddRangeHandler(uint32_t range, InputRangeHandler handler)
{
	if (HasRange(range))
	{
		mRangeHandlers[range] = handler;
	}
}

bool InputSystem::HasAction( uint32_t action ) const
{
	for (auto iter = mActions.begin(); iter != mActions.end(); ++iter)
	{
		if (iter->second == action)
		{
			return true;
		}
	}
	return false;
}

bool InputSystem::HasState(uint32_t state) const
{
	for (auto iter = mStates.begin(); iter != mStates.end(); ++iter)
	{
		if (iter->second == state)
		{
			return true;
		}
	}
	return false;
}

bool InputSystem::HasRange(uint32_t range) const
{
	for (auto iter = mRanges.begin(); iter != mRanges.end(); ++iter)
	{
		if (iter->second == range)
		{
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------
/*InputMaps::InputMaps()
{

}

bool InputMaps::HasActionFromKey( uint32_t key ) const
{
	return mActions.find(key) != mActions.end();
}

bool InputMaps::HasStateFromKey( uint32_t key ) const
{
	return mStates.find(key) != mStates.end();
}

bool InputMaps::HasRangeFromKey( uint32_t key ) const
{
	return mRanges.find(key) != mRanges.end();
}

bool InputMaps::MapToAction( uint32_t key, uint32_t& action ) const
{
	auto find = mActions.find(key);
	if ( find != mActions.end() )
	{
		action = find->second;
		return true;
	}
	return false;
}

bool InputMaps::MapToState( uint32_t key, uint32_t& state ) const
{
	auto find = mStates.find(key);
	if ( find != mStates.end() )
	{
		state = find->second;
		return true;
	}
	return false;
}

bool InputMaps::MapToRange( uint32_t axis, uint32_t& range ) const
{
	for (auto iter = mRanges.begin(); iter != mRanges.end(); ++iter)
	{
		if (iter->second == range)
		{
			range = iter->second;
			return true;
		}
	}
	return false;
}*/

}
}