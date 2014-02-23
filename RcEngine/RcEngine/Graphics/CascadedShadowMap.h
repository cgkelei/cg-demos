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
	CascadedShadowMap(RenderDevice* device);
	virtual ~CascadedShadowMap() {}

	void UpdateShadowMatrix(const Camera& camera, const Light& directionLight);
	void MakeCascadedShadowMap(const Light& light);

private:
	void UpdateShadowMapSize(const Light& light);

private:	
	RenderDevice* mDevice;

	float mSplitPlanes[MAX_CASCADES+1];

	// Used in frame buffer camera
	std::vector<shared_ptr<Camera>> mLightCamera;
	std::vector<shared_ptr<RenderView>> mShadowSplitsRTV;

	shared_ptr<FrameBuffer> mShadowFrameBuffer;
	shared_ptr<Texture> mShadowDepth;
	shared_ptr<Texture> mShadowMapTempBlur;
	shared_ptr<RenderView> mShadowMapTempBlurRTV;

	shared_ptr<Material> mBlurMaterial;

	// FSQuad
	shared_ptr<RenderOperation> mFSQuadShape;

public:
	std::vector<float4x4> mLightViewProj;
	shared_ptr<Texture> mShadowTexture;
};


}

#endif // CascadedShadowMap_h__
