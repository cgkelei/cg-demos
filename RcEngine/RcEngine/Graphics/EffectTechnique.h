#ifndef EffectTechnique_h__
#define EffectTechnique_h__

#include <Core/Prerequisites.h>
#include <Graphics/EffectPass.h>
#include <Graphics/EffectAnnotation.h>


namespace RcEngine {

class EffectPass;

class _ApiExport EffectTechnique 
{
	friend class Effect;

public:
	EffectTechnique();
	~EffectTechnique();

	const String& GetTechniqueName() const				{ return mName; }

	EffectTechnique* Clone(Effect& effect);

	vector<EffectPass*>& GetPasses()	{ return mPasses; }

	EffectPass* GetPassByName(const String& name);
	EffectPass* GetPassByIndex(uint32_t index);

protected:
	String mName;
	bool mValid;
	vector<EffectPass*> mPasses;
};


} // Namespace RcEngine

#endif // EffectTechnique_h__