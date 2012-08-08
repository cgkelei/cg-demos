#ifndef InputDevice_h__
#define InputDevice_h__

#include "Core/Prerequisites.h"

namespace RcEngine{
namespace Input{

	// 键盘动作
	enum KeyCode
	{
		KC_UNASSIGNED					= 0x00,
		KS_Escape                       = 0x01,
		KS_1                            = 0x02,
		KS_2                            = 0x03,
		KS_3                            = 0x04,
		KS_4                            = 0x05,
		KS_5                            = 0x06,
		KS_6                            = 0x07,
		KS_7                            = 0x08,
		KS_8                            = 0x09,
		KS_9                            = 0x0A,
		KS_0                            = 0x0B,
		KS_Minus                        = 0x0C,         // - on main keyboard
		KS_Equals                       = 0x0D,
		KS_BackSpace            = 0x0E,         // backspace
		KS_Tab                          = 0x0F,
		KS_Q                            = 0x10,
		KS_W                            = 0x11,
		KS_E                            = 0x12,
		KS_R                            = 0x13,
		KS_T                            = 0x14,
		KS_Y                            = 0x15,
		KS_U                            = 0x16,
		KS_I                            = 0x17,
		KS_O                            = 0x18,
		KS_P                            = 0x19,
		KS_LeftBracket          = 0x1A,
		KS_ReftBracket          = 0x1B,
		KS_Enter                        = 0x1C,         // Enter on main keyboard
		KS_LeftCtrl                     = 0x1D,
		KS_A                            = 0x1E,
		KS_S                            = 0x1F,
		KS_D                            = 0x20,
		KS_F                            = 0x21,
		KS_G                            = 0x22,
		KS_H                            = 0x23,
		KS_J                            = 0x24,
		KS_K                            = 0x25,
		KS_L                            = 0x26,
		KS_Semicolon					= 0x27,
		KS_Apostrophe				    = 0x28,
		KS_Grave                        = 0x29,         // accent grave
		KS_LeftShift					= 0x2A,
		KS_BackSlash					= 0x2B,
		KS_Z                            = 0x2C,
		KS_X                            = 0x2D,
		KS_C                            = 0x2E,
		KS_V                            = 0x2F,
		KS_B                            = 0x30,
		KS_N                            = 0x31,
		KS_M                            = 0x32,
		KS_Comma                        = 0x33,
		KS_Period                       = 0x34,         // . on main keyboard
		KS_Slash                        = 0x35,         // / on main keyboard
		KS_RightShift					= 0x36,
		KS_NumPadStar					= 0x37,         // * on numeric keypad
		KS_LeftAlt                      = 0x38,
		KS_Space                        = 0x39,
		KS_CapsLock                     = 0x3A,
		KS_F1                           = 0x3B,
		KS_F2                           = 0x3C,
		KS_F3                           = 0x3D,
		KS_F4                           = 0x3E,
		KS_F5                           = 0x3F,
		KS_F6                           = 0x40,
		KS_F7                           = 0x41,
		KS_F8                           = 0x42,
		KS_F9                           = 0x43,
		KS_F10                          = 0x44,
		KS_NumLock                      = 0x45,
		KS_ScrollLock					= 0x46,
		KS_NumPad7                      = 0x47,
		KS_NumPad8                      = 0x48,
		KS_NumPad9                      = 0x49,
		KS_NumPadMinus					= 0x4A,         // - on numeric keypad
		KS_NumPad4                      = 0x4B,
		KS_NumPad5                      = 0x4C,
		KS_NumPadD6                     = 0x4D,
		KS_NumPadPlus					= 0x4E,         // + on numeric keypad
		KS_NumPad1                      = 0x4F,
		KS_NumPad2                      = 0x50,
		KS_NumPad3                      = 0x51,
		KS_NumPad0                      = 0x52,
		KS_NumPadPeriod					= 0x53,         // . on numeric keypad
		KS_OEM_102                      = 0x56,         // <> or \| on RT 102-key keyboard (Non-U.S.)
		KS_F11                          = 0x57,
		KS_F12                          = 0x58,
		KS_F13                          = 0x64,         //                     (NEC PC98)
		KS_F14                          = 0x65,         //                     (NEC PC98)
		KS_F15                          = 0x66,         //                     (NEC PC98)
		KS_Kana                         = 0x70,         // (Japanese keyboard)
		KS_ABNT_C1                      = 0x73,         // /? on Brazilian keyboard
		KS_Convert                      = 0x79,         // (Japanese keyboard)
		KS_NoConvert					= 0x7B,         // (Japanese keyboard)
		KS_Yen                          = 0x7D,         // (Japanese keyboard)
		KS_ABNT_C2                      = 0x7E,         // Numpad . on Brazilian keyboard
		KS_NumPadEquals					= 0x8D,         // = on numeric keypad (NEC PC98)
		KS_PrevTrack					= 0x90,         // Previous Track (DKS_CIRCUMFLEX on Japanese keyboard)
		KS_AT                           = 0x91,         //                     (NEC PC98)
		KS_Colon                        = 0x92,         //                     (NEC PC98)
		KS_Underline					= 0x93,         //                     (NEC PC98)
		KS_Kanji                        = 0x94,         // (Japanese keyboard)
		KS_Stop                         = 0x95,         //                     (NEC PC98)
		KS_AX                           = 0x96,         //                     (Japan AX)
		KS_Unlabeled					= 0x97,         //                        (J3100)
		KS_NextTrack					= 0x99,         // Next Track
		KS_NumPadEnter					= 0x9C,         // Enter on numeric keypad
		KS_RightCtrl					= 0x9D,
		KS_Mute                         = 0xA0,         // Mute
		KS_Calculator					= 0xA1,         // Calculator
		KS_PlayPause					= 0xA2,         // Play / Pause
		KS_MediaStop					= 0xA4,         // Media Stop
		KS_VolumeDown					= 0xAE,         // Volume -
		KS_VolumeUp                     = 0xB0,         // Volume +
		KS_WebHome                      = 0xB2,         // Web home
		KS_NumPadComma					= 0xB3,         // , on numeric keypad (NEC PC98)
		KS_NumPadSlash					= 0xB5,         // / on numeric keypad
		KS_SysRQ                        = 0xB7,
		KS_RightAlt                     = 0xB8,         // right Alt
		KS_Pause                        = 0xC5,         // Pause
		KS_Home                         = 0xC7,         // Home on arrow keypad
		KS_UpArrow                      = 0xC8,         // UpArrow on arrow keypad
		KS_PageUp                       = 0xC9,         // PgUp on arrow keypad
		KS_LeftArrow				    = 0xCB,         // LeftArrow on arrow keypad
		KS_RightArrow					= 0xCD,         // RightArrow on arrow keypad
		KS_End                          = 0xCF,         // End on arrow keypad
		KS_DownArrow					= 0xD0,         // DownArrow on arrow keypad
		KS_PageDown                     = 0xD1,         // PgDn on arrow keypad
		KS_Insert                       = 0xD2,         // Insert on arrow keypad
		KS_Delete                       = 0xD3,         // Delete on arrow keypad
		KS_LeftWin                      = 0xDB,         // Left Windows key
		KS_RightWin                     = 0xDC,         // Right Windows key
		KS_Apps                         = 0xDD,         // AppMenu key
		KS_Power                        = 0xDE,         // System Power
		KS_Sleep                        = 0xDF,         // System Sleep
		KS_Wake                         = 0xE3,         // System Wake
		KS_WebSearch					= 0xE5,         // Web Search
		KS_WebFavorites					= 0xE6,         // Web Favorites
		KS_WebRefresh				    = 0xE7,         // Web Refresh
		KS_WebStop                      = 0xE8,         // Web Stop
		KS_WebForward					= 0xE9,         // Web Forward
		KS_WebBack                      = 0xEA,         // Web Back
		KS_MyComputer					= 0xEB,         // My Computer
		KS_Mail                         = 0xEC,         // Mail
		KS_MediaSelect					= 0xED,         // Media Select
	};

	// 鼠标动作
	enum MouseCode
	{
		MS_X                            = 0x100,
		MS_Y                            = 0x101,
		MS_Z                            = 0x102,
		MS_LeftButton                   = 0x103,
		MS_RightButton                  = 0x104,
		MS_MiddleButton                 = 0x105,
		MS_Button3                      = 0x106,
		MS_Button4                      = 0x107,
		MS_Button5                      = 0x108,
		MS_Button6                      = 0x109,
		MS_Button7                      = 0x10A,
	};


typedef std::pair<uint32_t, uint32_t> InputActionDef;
typedef std::pair<uint32_t, uint32_t> InputStateDef;

/**
/* Brief any device that is able to interact with the user (keyboard, mouse, joystick, touchscreen, ...)
/*/
class _ApiExport InputDevice
{
public:
	InputDevice(InputSystem* inputSystem);
	virtual ~InputDevice();

	virtual const String& GetName() const = 0;

	 /**
      * Updates all input states of this device
      */
	virtual void Update(float delta) = 0;

protected:
	InputSystem* mInputSystem;
};

class _ApiExport InputMaps
{
public:
	InputMaps();

	template<typename ForwardIterator>
	void AddAction(ForwardIterator first, ForwardIterator last)
	{
		for (ForwardIterator iter = first; iter != last; ++iter)
		{
			mActions.insert(std::make_pair((*iter).second, (*iter).first));
		}
	}

	template<typename ForwardIterator>
	void AddState(ForwardIterator first, ForwardIterator last)
	{
		for (ForwardIterator iter = first; iter != last; ++iter)
		{
			mActions.insert(std::make_pair((*iter).second, (*iter).first));
		}
	}

	// whether exit an action which was this key mapped
	bool HasActionFromKey(uint32_t key) const;
	bool HasStateFromKey(uint32_t key) const;
	bool HasRangeFromKey(uint32_t key) const;

	bool MapToAction(uint32_t key, uint32_t& action) const;
	bool MapToState(uint32_t key, uint32_t& state) const;
	bool MapToRange(uint32_t axis, uint32_t& range) const;

protected:
	// Raw input to Action
	std::unordered_map<uint32_t, uint32_t> mActions;
	
	// Raw input to State 
	std::unordered_map<uint32_t, uint32_t> mStates;

	// Raw input to Range
	std::unordered_map<uint32_t, uint32_t> mRanges;
};

typedef fastdelegate::FastDelegate1<void, const InputSystem&> CancellableEventHandler;

class _ApiExport InputSystem
{
public:
	struct _ApiExport MappedInput
	{
		unordered_set<uint32_t> Actions;
		unordered_set<uint32_t> States;
		unordered_map<uint32_t, float> Ranges;

		// Consumption helpers
		void EatAction(uint32_t action)           { Actions.erase(action); }
		void EatState(uint32_t state)             { States.erase(state); }
		void EatRange(uint32_t range)			  { Ranges.erase(range); }
	};

public:
	InputSystem();
	virtual ~InputSystem();

	virtual const String& Name() const = 0;
	
	void Update(float delta);
	size_t GetDeviceCount() const;
	InputDevice* GetDevice(size_t index) const;


protected:
	std::vector< InputDevice* > mDevices;
	MappedInput mCurrentMappedInput;
};

class _ApiExport Mouse : public InputDevice
{
public:
	Mouse(InputSystem* inputSystem);
	virtual ~Mouse();

	virtual long X() const = 0;
	virtual long Y() const = 0;
	virtual bool ButtonDown(MouseCode button) const = 0;

};


class _ApiExport Keyboard : public InputDevice
{
public:
	Keyboard(InputSystem* inputSystem);
	virtual ~Keyboard();

	virtual bool IsKeyDown(KeyCode key) const = 0;
};






	
}
}


#endif // InputDevice_h__