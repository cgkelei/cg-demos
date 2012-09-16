#ifndef ShaderProgram_h__
#define ShaderProgram_h__

#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>

namespace RcEngine {

class Shader;
class Effect;

class _ApiExport ShaderProgram
{
public:
	ShaderProgram(Effect& effect);
	virtual~ ShaderProgram();

	void AttachShader( const shared_ptr<Shader>& shader ); 

	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	virtual bool LinkProgram() = 0;
	
	bool Validate() const { return mValidate; }
	const String& GetLinkInfo() const { return mLinkerOutput; }
	
protected:
	Effect& mEffect;
	vector<shared_ptr<Shader> > mAttachedShaders;
	String mLinkerOutput;	// Only for opengl render system
	bool mValidate;

	struct ParameterBind
	{
		String Name;
		EffectParameterType Type;
		std::function<void()> ShaderParamSetFunc;
		EffectParameter* EffectParameter;
	};
	vector<ParameterBind> mParameterBinds;
};

}


#endif // ShaderProgram_h__
