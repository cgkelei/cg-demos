#ifndef Renderer_h__
#define Renderer_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>


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
	void DrawGeometry(const String& tech, const String& matClass, RenderOrder order);
	void DrawRenderable(Camera* camera, const String& tech, const String& matClass, RenderOrder order);

private:
	void DrawEntity();

	void ApplyMaterial(const shared_ptr<Material>& material);

private:
	shared_ptr<FrameBuffer> mCurrentFrameBuffer;
	
	shared_ptr<Material> mCurrentMat;
	
	EffectTechnique* mCurrentTechnique;

	Matrix4f mViewMat, mViewMatInv, mProjMat, mViewProjMat, mViewProjMatInv;

	Light* mCurrentLight;
};


}


#endif // Renderer_h__
