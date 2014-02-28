#ifndef DeferredPath_h__
#define DeferredPath_h__

#include <Core/Prerequisites.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/ColorRGBA.h>
#include <Resource/Resource.h>

namespace RcEngine {

class CascadedShadowMap;

class _ApiExport RenderPath
{
public:
	RenderPath();
	virtual ~RenderPath() {}

	virtual void OnGraphicsInit();
	virtual void OnWindowResize(uint32_t width, uint32_t height) {}
	virtual void RenderScene() {}

protected:
	void DrawFSQuad(const shared_ptr<Material>& material, const String& tech);
	void DrawOverlays();

protected:
	RenderDevice* mDevice;
	SceneManager* mSceneMan;
	shared_ptr<RenderOperation> mFSQuadShape;
};

class _ApiExport DeferredPath : public RenderPath
{
public:
	DeferredPath();
	virtual ~DeferredPath() {}

	virtual void OnGraphicsInit();
	virtual void OnWindowResize(uint32_t width, uint32_t height);
	virtual void RenderScene();

	shared_ptr<Effect> GetDeferredEffect() const; 
	CascadedShadowMap* GetShadowManager() const  { return mShadowMan; }

protected:
	void GenereateGBuffer();
	void ComputeSSAO();
	void DeferredLighting(); // Lighting pass
	void DeferredShading();  // Shading pass
	void EnvironmentLighting();
	void DrawBackground();

	void DrawDirectionalLightShape(Light* light, const String& tech);


protected:

	// Normal + Specular Shininess,  Albedo + Specular Intensity
	shared_ptr<Texture> mGBuffer[2];
	shared_ptr<RenderView> mGBufferRTV[2];
	
	shared_ptr<Texture> mDepthStencilBuffer;
	shared_ptr<RenderView> mDepthStencilView;

	shared_ptr<Texture> mLightAccumulateBuffer;
	shared_ptr<Texture> mDepthStencilBufferLight; // Used for stencil optimization for light volume
	shared_ptr<RenderView> mLightAccumulateRTV;
	shared_ptr<RenderView> mDepthStencilBufferLightView;

	shared_ptr<Texture> mHDRBuffer;
	shared_ptr<RenderView> mHDRBufferRTV;

	shared_ptr<FrameBuffer> mGBufferFB;
	shared_ptr<FrameBuffer> mLightAccumulateFB;
	shared_ptr<FrameBuffer> mHDRFB;

	shared_ptr<Material> mDeferedMaterial;
	shared_ptr<Material> mDebugViewMaterial;

	CascadedShadowMap* mShadowMan;
};

}


#endif // DeferredPath_h__
