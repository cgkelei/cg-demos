#ifndef Renderer_h__
#define Renderer_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Math/Matrix.h>


namespace RcEngine {

class FrameBuffer;

class _ApiExport Renderer
{
public:
	Renderer();
	~Renderer();

	void RenderScene(const shared_ptr<Pipeline>& pipeline);
	void UpdateShadowMap();

private:
	void DrawFSQuad(const shared_ptr<Material>& mat, const String& tech);

	/**
	 * Draw geometry with light, it can used to generate shadow map
	 */
	void DrawGeometry(const String& tech, const String& matClass);

	void DrawLightGeometry(Light* light, const String& matClass);

	void DrawRenderable(Camera* camera, const String& tech, const String& matClass);

	void SetupViewProjMatirices(const float4x4& view, const float4x4& proj);

private:
	void DrawEntity();

	void ApplyMaterial(const shared_ptr<Material>& material);

private:
	shared_ptr<FrameBuffer> mCurrentFrameBuffer;
	
	shared_ptr<Material> mCurrentMat;
	
	EffectTechnique* mCurrentTechnique;

	float4x4 mViewMat, mViewMatInv, mProjMat, mViewProjMat;

	Light* mCurrentLight;
};


}


#endif // Renderer_h__
