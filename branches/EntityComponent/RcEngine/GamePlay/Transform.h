#ifndef Transform_h__
#define Transform_h__

#include <GamePlay/Component.h>
#include <Math/Matrix.h>

namespace RcEngine {

class _ApiExport Transform : public Component
{
public:
	Transform() {}

	const String& GetComponentName() const { return ComponentName; }

	const float4x4& GetWorldTransform() const { return mTransform; }

public:
	static const String ComponentName;

protected:
	float4x4 mTransform; // World Transform
};

}


#endif // Transform_h__
