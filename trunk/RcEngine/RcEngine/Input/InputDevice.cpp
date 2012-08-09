#include "InputDevice.h"
#include "Core/Context.h"

namespace RcEngine{
namespace Input{

	InputDevice::InputDevice(InputSystem* inputSystem)
		:mInputSystem(inputSystem)
	{

	}

//----------------------------------------------------------------
	InputDevice::~InputDevice()
	{

	}

	InputSystem::InputSystem()
	{
		Core::Context::GetSingleton().SetInputSystem(this);
	}

	InputSystem::~InputSystem()
	{

	}

	void InputSystem::Update( )
	{
		// update all input device
		for(auto iter = mDevices.begin(); iter != mDevices.end(); ++iter)
		{
			(*iter)->Update();
		}

		//
	}

	size_t InputSystem::GetDeviceCount() const
	{
		return mDevices.size();
	}

	InputDevice* InputSystem::GetDevice( size_t index ) const
	{
		assert(index < mDevices.size());
		return mDevices[index];
	}

//-------------------------------------------------------------------
	InputMaps::InputMaps()
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
	}

//---------------------------------------------------------------------------------
	Mouse::Mouse( InputSystem* inputSystem )
		: InputDevice(inputSystem)
	{
	}

	Mouse::~Mouse()
	{

	}

	bool Mouse::ButtonDown( MouseCode button ) const
	{
		return ((mButtons[mIndex]) & (1L<<(button-MS_LeftButton))) != 0;
	}

	bool Mouse::ButtonPress( MouseCode button ) const
	{
		return (((mButtons[mIndex]) & (1L<<(button-MS_LeftButton))) != 0)
			&& (((mButtons[!mIndex]) & (1L<<(button-MS_LeftButton))) == 0);
			
	}


	Keyboard::Keyboard( InputSystem* inputSystem )
		: InputDevice(inputSystem), mIndex(0)
	{

	}

	Keyboard::~Keyboard()
	{

	}

	bool Keyboard::KeyDown( KeyCode key ) const
	{
		return mKeyBuffer[mIndex][key];
	}

	bool Keyboard::KeyPress( KeyCode key ) const
	{
		return (mKeyBuffer[mIndex][key] && !mKeyBuffer[!mIndex][key]);
	}


}
}
