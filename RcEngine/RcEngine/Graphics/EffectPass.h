#ifndef EffectPass_h__
#define EffectPass_h__

#include <Core/Prerequisites.h>
#include <Graphics/EffectAnnotation.h>

namespace RcEngine {
namespace Render {

class _ApiExport EffectPass
{
public:
	EffectPass();
	virtual ~EffectPass();

	const String& GetPassName() const	{ return mName; }

	EffectAnnotationList& GetAnnotations();
	EffectAnnotation* GetAnnotationByName(const String& name);
	EffectAnnotation* GetAnnotationByIndex(uint32_t index);

	virtual bool BeginPass() = 0;
	virtual void EndPass() = 0;

protected:
	String mName;
	bool mValid;
	EffectAnnotationList mAnnotations;

};

typedef vector<EffectPass*> EffectPassList;

} // Namespace Render
} // Namespace RcEngine

#endif // EffectPass_h__