#include <Core/Profiler.h>
#include <Core/Timer.h>

#define INVALID_TIME	((uint64_t)(-1))
#define INVALID_INDEX   ((size_t)(-1))

namespace RcEngine {

ProfilerManager gProfilerManager;

//////////////////////////////////////////////////////////////////////////
CpuAutoProfiler::CpuAutoProfiler( const char* name )
{
	gProfilerManager.ProfilerStart(name);
}

CpuAutoProfiler::~CpuAutoProfiler()
{
	gProfilerManager.ProfilerEnd();
}

//////////////////////////////////////////////////////////////////////////
ProfilerManager::ProfilerManager()
	: mCurrNumSamples(0),
	  mLastOpenedSample(INVALID_INDEX),
	  mCallStackDepth(0)
{

}

ProfilerManager::~ProfilerManager()
{

}

void ProfilerManager::ProfilerStart( const char* name )
{
	bool found = false;
	for (uint32_t i = 0; i < mCurrNumSamples; ++i)
	{
		if (mSamples[i].SampleName == name)
		{
			mSamples[i].ParentSampleIndex = mLastOpenedSample;
			mLastOpenedSample = i;

			mSamples[i].CallStackDepth = mCallStackDepth++;
			++mSamples[i].CallCount;

			found = true;
			break;
		}
	}

	if (!found)
	{
		size_t index = mCurrNumSamples++;
		assert(index < MAX_PROFILERS);
		
		mSamples[index].SampleName = name;
		mSamples[index].SampleIndex = index;

		mSamples[index].ParentSampleIndex = mLastOpenedSample;
		mLastOpenedSample = index;

		mSamples[index].CallStackDepth = mCallStackDepth++;
		mSamples[index].CallCount = 1;
		mSamples[index].TotalTime = 0;
		mSamples[index].TotalChildTime = 0;

		mSamples[index].StartTime = GetTimeMS();
	
		if (mSamples[index].ParentSampleIndex == INVALID_INDEX)
		{
			// Root Begin
			mFrameBeginTime = mSamples[index].StartTime;
		}
	}

	printf("%s begin\n", name);
}

void ProfilerManager::ProfilerEnd( )
{
	double endTime = GetTimeMS();
	double elapsedTime = endTime - mSamples[mLastOpenedSample].StartTime;

	if (mSamples[mLastOpenedSample].ParentSampleIndex != INVALID_INDEX)
	{
		size_t parentIndex = mSamples[mLastOpenedSample].ParentSampleIndex;
		mSamples[parentIndex].TotalChildTime += elapsedTime;
	}
	else
	{
		// Root End
		mFrameEndTime = endTime;
	}

	printf("%s End\n", mSamples[mLastOpenedSample].SampleName);

	mSamples[mLastOpenedSample].TotalTime += elapsedTime;
	mLastOpenedSample = mSamples[mLastOpenedSample].ParentSampleIndex;
	--mCallStackDepth;
}

void ProfilerManager::ResetProfiler( const char* name )
{

}

void ProfilerManager::ResetAll()
{
	
}

void ProfilerManager::Output()
{


	for (uint32_t i = 0; i < mCurrNumSamples; ++i)
	{
		double sampleTime, percentage;
		//calculate the time spend on the sample itself (excluding children)
		
		sampleTime = double(mSamples[i].TotalTime - mSamples[i].TotalChildTime) / 1000;
		//percentage = ( double(sampleTime) / double(mFrameEndTime - mFrameBeginTime) ) * 100.0f;

		////add it to the sample's values
		//float totalPc;
		//totalPc=samples[i].averagePc*samples[i].dataCount;
		//totalPc+=percentage; samples[i].dataCount++;
		//samples[i].averagePc=totalPc/samples[i].dataCount;
		//if((samples[i].minPc==-1)||(percentage<samples[i].minPc))samples[i].minPc=percentage;
		//if((samples[i].maxPc==-1)||(percentage>samples[i].maxPc))samples[i].maxPc=percentage;

		////output these values
		//outputHandler->Sample(samples[i].minPc,
		//	samples[i].averagePc,
		//	samples[i].maxPc,
		//	samples[i].callCount,
		//	samples[i].name,
		//	samples[i].parentCount);

		char namebuf[256], indentedName[256];
		char avg[16], min[16], max[16], num[16];

		/*	sprintf(avg, "%3.1f", fAvg);
		sprintf(min, "%3.1f", fMin);
		sprintf(max, "%3.1f", fMax);
		sprintf(num, "%3d",   mSamples[i].CallCount);*/

		for (uint32_t indent = 0; indent < mSamples[i].CallStackDepth; ++indent)
		namebuf[indent] = ' ';

		strcpy(namebuf + mSamples[i].CallStackDepth, mSamples[i].SampleName);
		printf("time=%f, childTime=%f, call=%d, %s\n", mSamples[i].TotalTime, mSamples[i].TotalChildTime, mSamples[i].CallCount, namebuf);

		//reset the sample for next time
		mSamples[i].CallCount = 0;
		mSamples[i].TotalTime = 0;
		mSamples[i].TotalChildTime = 0;
	}
}





}