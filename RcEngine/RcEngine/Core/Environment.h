#ifndef Environment_h__
#define Environment_h__

#include <Core/Prerequisites.h>
#include <Core/Singleton.h>

namespace RcEngine{

class Application;
class RHDevice;
class RHFactory;

class _ApiExport Environment : public Singleton<Environment>
{
public:
	SINGLETON_DECL_HEADER(Environment)

public:
	Environment();
	~Environment();
	
	inline Application* GetApplication() const			{ assert(mApplication); return mApplication; }
	inline RHDevice*    GetRHDevice() const				{ assert(mRHDevice); return mRHDevice; }
	inline RHFactory*   GetRHFactory() const			{ assert(mRHFactory); return mRHFactory; }

private:

	friend class Application;
	friend class RHDevice;
	friend class RHFactory;

	Application* mApplication;
	RHDevice* mRHDevice;
	RHFactory* mRHFactory;
};

} // Namespace RcEngine

#endif // Environment_h__
