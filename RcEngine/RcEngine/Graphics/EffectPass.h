#ifndef EffectPass_h__
#define EffectPass_h__

#include <Core/Prerequisites.h>
#include <Math/ColorRGBA.h>
#include <Graphics/EffectAnnotation.h>

namespace RcEngine {


class ShaderProgram;
class Effect;

class _ApiExport EffectPass
{
	friend class Effect;

public:
	EffectPass();
	~EffectPass();

	const String& GetPassName() const	{ return mName; }

	void BeginPass();
	void EndPass();

protected:
	String mName;
	bool mValid;
	shared_ptr<ShaderProgram> mShaderProgram;
	shared_ptr<BlendState> mBlendState;
	shared_ptr<DepthStencilState> mDepthStencilState;
	shared_ptr<RasterizerState> mRasterizerState;
	uint16_t mFrontStencilRef, mBackStencilRef;
	ColorRGBA mBlendColor;
	uint32_t mSampleMask;
};


} // Namespace RcEngine

#endif // EffectPass_h__