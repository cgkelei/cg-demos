#include <Core/Profiler.h>
#include <Core/Timer.h>

#define INVALID_TIME	((uint64_t)(-1))

inline uint32_t IncrementCircle(uint32_t idx, uint32_t circleSize)
{
	if (++idx >= circleSize)
		return 0;

	return idx;
}

inline uint32_t DecrementCycle(uint32_t idx, uint32_t circleSize)
{
	if (idx == 0)
		return circleSize - 1;
	
	return --idx;
}

namespace RcEngine {

ProfilerManager gProfilerManager;

//////////////////////////////////////////////////////////////////////////
ProfilerManager::CpuThreadInfo&  ProfilerManager::FetchCpuThreadProfiler()
{
	std::thread::id threadID = std::this_thread::get_id();
	
	for (CpuThreadInfo& threadInfo : mCpuThreadInfos)
	{
		if (threadInfo.ThreadID == threadID)
			return threadInfo;
	}

	mMutex.lock(); 
	mCpuThreadInfos.resize(mCpuThreadInfos.size()+1);
	CpuThreadInfo& newThreadInfo = mCpuThreadInfos.back();
	mMutex.unlock();

	newThreadInfo.Init(threadID);
	return newThreadInfo;
}

void ProfilerManager::PushCpuMarker( const char* name )
{
	CpuThreadInfo& threadInfo = FetchCpuThreadProfiler();

	CpuMarker& maker = threadInfo.Markers[threadInfo.CurrWriteIndex];

	maker.Name = name;
	maker.Start = GetTimeNS();
	maker.End = INVALID_TIME;
	maker.Frame = mCurrFrame;
	
	threadInfo.CurrWriteIndex = IncrementCircle(threadInfo.CurrWriteIndex, NUM_MARKERS_PER_THREAD);
}

void ProfilerManager::PopCpuMaker()
{
	CpuThreadInfo& threadInfo = FetchCpuThreadProfiler();

	uint32_t i = DecrementCycle(threadInfo.CurrWriteIndex, NUM_MARKERS_PER_THREAD);
	while ( threadInfo.Markers[i].End != INVALID_TIME )
		i = DecrementCycle(i, NUM_MARKERS_PER_THREAD);
	
	CpuMarker& marker = threadInfo.Markers[i];
	assert(marker.End == INVALID_TIME);
	marker.End = GetTimeNS();
}

ProfilerManager::ProfilerManager()
{

}

ProfilerManager::~ProfilerManager()
{

}




}