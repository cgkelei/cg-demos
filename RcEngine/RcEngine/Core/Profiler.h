#ifndef Profiler_h__
#define Profiler_h__

#include <Core/Prerequisites.h>
#include <thread>
#include <mutex>

namespace RcEngine {

#define MAX_NUM_MARKERS_PER_FRAME 20
#define NUM_RECORDED_FRAMES       3
#define NUM_MARKERS_PER_THREAD    (MAX_NUM_MARKERS_PER_FRAME * NUM_RECORDED_FRAMES)

class _ApiExport ProfilerManager
{
private:

	struct Marker
	{
		const char* Name;
		
		uint32_t Frame;	

		// Start and End time, in nanoseconds,
		uint64_t Start, End;
	};
	
	typedef Marker CpuMarker;

	struct CpuThreadInfo
	{
		std::thread::id ThreadID;	

		size_t CurrWriteIndex;
		size_t CurrReadIndex;
		size_t NumPushedMakers;

		CpuMarker Markers[NUM_MARKERS_PER_THREAD];
		
		void Init(const std::thread::id& id)
		{
			ThreadID = id;
			CurrWriteIndex = CurrReadIndex = NumPushedMakers = 0;
		}
	};

public:
	ProfilerManager();
	~ProfilerManager();

private:
	void PushCpuMarker(const char* name);
	void PopCpuMaker();

private:
	CpuThreadInfo& FetchCpuThreadProfiler();

private:
	std::mutex mMutex;
	std::vector<CpuThreadInfo> mCpuThreadInfos;

	volatile uint32_t mCurrFrame;		// Global frame counter

	friend struct CpuAutoProfiler;
	friend struct GpuAutoProfiler;
};

extern ProfilerManager gProfilerManager;

struct _ApiExport CpuAutoProfiler
{
	CpuAutoProfiler(const char* name) { gProfilerManager.PushCpuMarker(name); }
	~CpuAutoProfiler()				  { gProfilerManager.PopCpuMaker(); }
};

#define ENGINE_CPU_PROFIER(name) CpuAutoProfiler _cpu_profiler(name)
//#define ENGINE_GPU_PROFIER(name)
}


#endif // Profiler_h__
