#include <Core/Timer.h>
#include <windows.h>

namespace RcEngine {

Timer::Timer()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
	mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}


float Timer::GetGameTime() const
{
	if(mStopped)
	{
		return (float)((mStopTime - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return (float)
			((mCurrTime - mBaseTime - mStopTime) * mSecondsPerCount);
	}
}


float Timer::GetDeltaTime() const
{
	return (float)mDeltaTime;
}

void Timer::Tick()
{
	if(mStopped)
	{
		mDeltaTime = 0;
		return;
	}

	// Get Time this frame
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	mCurrTime = currentTime;

	// Times difference between this and previous frame
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

	// Prepare for next frame
	mPrevTime = mCurrTime;
	
	// Force nonnegative. The DXSDK's CDXUTTimer mentions that if the
	// processor goes into a power save mode or we get shuffled to
	// another processor, then mDeltaTime can be negative.
	if(mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}

void Timer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void Timer::Stop()
{
	if(!mStopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mStopTime = currTime;
		mStopped = true;
	}
}

void Timer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if(mStopped)
	{
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

} // Namespace RcEngine