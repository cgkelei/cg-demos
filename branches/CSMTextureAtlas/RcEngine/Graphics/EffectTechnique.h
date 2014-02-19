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
	EffectTechnique(Effect& effect);
	~EffectTechnique();

	const String& GetTechniqueName() const				{ return mName; }
	Effect& GetEffect()									{ return mEffect; }

	EffectTechnique* Clone(Effect& effect);

	vector<EffectPass*>& GetPasses()					{ return mPasses; }
	const vector<EffectPass*>& GetPasses() const		{ return mPasses; }

	EffectPass* GetPassByName(const String& name) const;
	EffectPass* GetPassByIndex(uint32_t index) const;

protected:
	Effect& mEffect;

	String mName;
	bool mValid;
	vector<EffectPass*> mPasses;
};


} // Namespace RcEngine

#endif // EffectTechnique_h__