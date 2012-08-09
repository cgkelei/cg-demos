#ifndef InputSystem_h__
#define InputSystem_h__

#include "Core/Prerequisites.h"

namespace RcEngine{
namespace Input{
	
	typedef std::pair<uint32_t, uint32_t> InputActionDef;
	typedef std::pair<uint32_t, uint32_t> InputStateDef;


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

		void Update();
		size_t GetDeviceCount() const;
		InputDevice* GetDevice(size_t index) const;


	protected:
		std::vector< InputDevice* > mDevices;
		MappedInput mCurrentMappedInput;
	};


}
}


#endif // InputSystem_h__