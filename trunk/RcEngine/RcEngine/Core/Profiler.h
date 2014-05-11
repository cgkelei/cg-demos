#ifndef Profiler_h__
#define Profiler_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

#define INVALID_TIME	((uint64_t)(-1))
#define MAX_PROFILERS   (200)

struct _ApiExport ProfilerSample
{
	const char* SampleName;
	
	size_t SampleIndex;
	size_t ParentSampleIndex;

	uint32_t CallStackDepth;

	double TotalTime;
	double TotalChildTime;
	uint32_t CallCount;

	double StartTime;

	ProfilerSample()
		: TotalTime(0), CallCount(0)
	{

	}
};

class _ApiExport ProfilerManager
{
public:
	ProfilerManager();
	~ProfilerManager();

	void ProfilerStart(const char* name);
	void ProfilerEnd();

	void ResetProfiler(const char* name);
	void ResetAll();

	void Output();

private:
	ProfilerSample mSamples[MAX_PROFILERS];

	size_t mLastOpenedSample;

	uint32_t mCurrNumSamples;
	uint32_t mCallStackDepth;

	double mFrameBeginTime;
	double mFrameEndTime;

	volatile uint32_t mCurrFrame;		// Global frame counter

	friend struct CpuAutoProfiler;
	friend struct GpuAutoProfiler;
};

 _ApiExport extern ProfilerManager gProfilerManager;

struct _ApiExport CpuAutoProfiler
{
	CpuAutoProfiler(const char* name);
	~CpuAutoProfiler();

	uint64_t StartTime;
};

#define ENGINE_CPU_AUTO_PROFIER(name) CpuAutoProfiler _cpu_profiler(name)
#define ENGINE_PUSH_CPU_PROFIER(name) gProfilerManager.ProfilerStart(name)
#define ENGINE_POP_CPU_PROFIER(name)  gProfilerManager.ProfilerEnd()
#define ENGINE_DUMP_PROFILERS()		  gProfilerManager.Output()
}


#endif // Profiler_h__
