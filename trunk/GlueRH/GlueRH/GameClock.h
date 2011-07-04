#ifndef _GameClock__H
#define _GameClock__H

#include "Common.h"

namespace GlueRH
{
	class GameClock
	{
	public:
		GameClock(void)
		{
			Reset();
		};
		~GameClock(void)
		{

		}

		float GetElapsedTime() const { return mElapsedTime; }
		float GetElapsedAdjustedTime() const { return mElapsedAdjustedTime; }
		float GetCurrentTime() const { return mCurrentTimeBase + mCurrentTimeOffset; }

		public void Reset()
		{
			mElapsedAdjustedTime = 0;
			mSuspendStartTime = 0;
			mSuspendCount = 0;
			mBaseRealTime = 0;
			mLastRealTime = 0;

			mCurrentTimeBase = 0;
			mCurrentTimeOffset = 0;
			mBaseRealTime = GetCounter();
			mLastRealTimeValid = false;
			mFrequency = GetFrequency();
		}

		void Suspend()
		{
			mSuspendCount++;
			if (mSuspendCount == 1)
				mSuspendStartTime = GetCounter();
		}

		/// <summary>
		/// Resumes a previously suspended clock.
		/// </summary>
		void Resume()
		{
			mSuspendCount--;
			if (mSuspendCount <= 0)
			{
				mTimeLostToSuspension += GetCounter() - mSuspendStartTime;
				mSuspendStartTime = 0;
			}
		}

		void Step()
		{
			uint64 counter = GetCounter();

			if (!mLastRealTimeValid)
			{
				mLastRealTime = counter;
				mLastRealTimeValid = true;
			}

			if ( !CounterToTimeSpan(counter, mBaseRealTime, &mCurrentTimeOffset) )
			{
				// update the base value and try again to adjust for overflow
				mCurrentTimeBase += mCurrentTimeOffset;
				mBaseRealTime = mLastRealTime;

				if ( !CounterToTimeSpan(counter, mBaseRealTime, &mCurrentTimeOffset) )
				{
					mBaseRealTime = counter;
					mCurrentTimeOffset = 0;
				}
			}

			if (!CounterToTimeSpan(counter, mLastRealTime, &mElapsedTime))
			{
				mElapsedTime = 0;
			}

			if ( CounterToTimeSpan(counter, (mLastRealTime + mTimeLostToSuspension), &mElapsedAdjustedTime) )
			{
				mTimeLostToSuspension = 0;
			}
			else
			{
				mElapsedAdjustedTime = 0;
			}

			mLastRealTime = counter;
		}

	private:
		uint64 GetCounter()
		{
			LARGE_INTEGER counter;
			assert( QueryPerformanceCounter(&counter) );
			return counter.QuadPart;
		}

		float GetFrequency()
		{
			LARGE_INTEGER freq;
			assert( QueryPerformanceFrequency(&freq) ); 
			return freq.QuadPart;
		}

		bool CounterToTimeSpan(uint64 counter, uint64 base, float* result) const
		{
			if (base > counter)
				return false;
			*result = (counter - base) / (float)m_frequency;
			return true;
		}

	
	private:
		uint64 mBaseRealTime;
		uint64 mLastRealTime;
		bool mLastRealTimeValid;
		uint32 suspendCount;
		uint64 mSuspendStartTime;
		uint64 mTimeLostToSuspension;
		
		float mCurrentTimeBase;
		float mCurrentTimeOffset;
		float mElapsedTime;
		float mElapsedAdjustedTime;

		uint64 mFrequency;
		
	};
}

#endif