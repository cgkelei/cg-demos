#ifndef InputSystem_h__
#define InputSystem_h__

#include <Core/Prerequisites.h>
#include <Input/InputEvent.h>

namespace RcEngine{

typedef std::pair<uint32_t, uint32_t> InputAction;
typedef std::pair<uint32_t, uint32_t> InputState;
typedef std::pair<uint32_t, uint32_t> InputRange;


//class _ApiExport InputMaps
//{
//public:
//	InputMaps();
//	// whether exit an action which was this key mapped
//	bool HasActionFromKey(uint32_t key) const;
//	bool HasStateFromKey(uint32_t key) const;
//	bool HasRangeFromKey(uint32_t key) const;

//	bool MapToAction(uint32_t key, uint32_t& action) const;
//	bool MapToState(uint32_t key, uint32_t& state) const;
//	bool MapToRange(uint32_t axis, uint32_t& range) const;

//protected:
//	
//};

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

class Mouse;
class Keyboard;

class _ApiExport InputSystem
{
public:
	InputSystem();
	virtual ~InputSystem();

public:
	virtual const String& Name() const = 0;

	void FireEvent(const InputEvent& event);
	bool PollEvent(InputEvent* event);

	Mouse* GetMouse() const	{ return mMouse; }
	Keyboard* GetKeyboard() const { return mKeyboard; }

	void AddActionHandler(uint32_t action, InputActionHandler handler);
	void AddStateHandler(uint32_t state, InputStateHandler handler);
	void AddRangeHandler(uint32_t range, InputRangeHandler handler);

	void Update(float delata);
	void Resize(uint32_t width, uint32_t height);

public:
	template<typename ForwardIterator>
	void AddAction(ForwardIterator first, ForwardIterator last)
	{
		for (ForwardIterator iter = first; iter != last; ++iter)
		{
			mActions.insert(InputAction((*iter).first, (*iter).second));
		}
	}

	template<typename ForwardIterator>
	void AddState(ForwardIterator first, ForwardIterator last)
	{
		for (ForwardIterator iter = first; iter != last; ++iter)
		{
			mStates.insert(InputState((*iter).first, (*iter).second));
		}
	}

	template<typename ForwardIterator>
	void AddRange(ForwardIterator first, ForwardIterator last)
	{
		for (ForwardIterator iter = first; iter != last; ++iter)
		{
			mRanges.insert(InputRange((*iter).first, (*iter).second));
		}
	}

	bool HasAction(uint32_t action) const;
	bool HasState(uint32_t state) const;
	bool HasRange(uint32_t range) const;

private:
	void DispatchActions(float delata) const;
	void DispatchStates(float delata) const;
	void DispatchRanges(float delata) const;

protected:
	Mouse* mMouse;
	Keyboard* mKeyboard;

	std::queue<InputEvent> mEventQueue;

private:
	// Raw input to Action
	std::unordered_map<uint32_t, uint32_t> mActions;
	std::unordered_map<uint32_t, InputActionHandler> mActionHandlers;

	// Raw input to State 
	std::unordered_map<uint32_t, uint32_t> mStates;
	std::unordered_map<uint32_t, InputStateHandler> mStateHandlers;

	// Raw input to Range
	std::unordered_map<uint32_t, uint32_t> mRanges;
	std::unordered_map<uint32_t, InputRangeHandler> mRangeHandlers;
};

} // Namespace RcEngine


#endif // InputSystem_h__