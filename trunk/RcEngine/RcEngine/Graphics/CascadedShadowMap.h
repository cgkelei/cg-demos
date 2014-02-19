#ifndef CascadedShadowMap_h__
#define CascadedShadowMap_h__

#include <Core/Prerequisites.h>
#include <Graphics/FrameBuffer.h>
#include <Math/Matrix.h>

namespace RcEngine {

#define MAX_CASCADES 4
#define SHADOW_MAP_SIZE 1024

#define SHADOW_MAP_VSM

class _ApiExport CascadedShadowMap
{
public:
	CascadedShadowMap();
	virtual ~CascadedShadowMap() {}

	void UpdateShadowMatrix(const Camera& camera, const Light& directionLight);

private:
	void UpdateShadowMapSize(const Light& light);

public:	
	float mSplitPlanes[MAX_CASCADES+1];

	std::vector<float4x4> mLightViewProj;
	
	// Used in frame buffer camera
	std::vector<shared_ptr<Camera>> mLightCamera;

	std::vector<shared_ptr<RenderView>> mShadowSplitsRTV;
	
	shared_ptr<FrameBuffer> mShadowFrameBuffer;
	shared_ptr<Texture> mShadowTexture;
	shared_ptr<Texture> mShadowDepth;
};


}

#endif // CascadedShadowMap_h__
