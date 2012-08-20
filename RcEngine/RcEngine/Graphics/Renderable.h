#ifndef Renderable_h__
#define Renderable_h__

#include <Core/Prerequisites.h>
#include <Math/BoundingSphere.h>
#include <Math/Matrix.h>

namespace RcEngine {


/** 
 * Renderable discrete objects which will be queued in the render pipeline, grouped by material. Classes
 * implementing this interface must be based on a single material, a single world matrix (or a collection
 * of world matrices which are blended by weights), and must be renderable via a single render operation.
 */
class _ApiExport Renderable
{
public:
	Renderable();
	virtual ~Renderable();
			
	virtual const String& GetName() const = 0;	

	virtual const BoundingSpheref& GetBoundingSphere() const = 0;

	virtual const shared_ptr<Material>& GetMaterial() const = 0;

	/** This is to allow Renderables to use a chosen Technique if they wish,
		* otherwise they will use the best Technique available for the Material they are using.
		*/
	virtual EffectTechnique* GetTechnique() const; 

	virtual const shared_ptr<RenderOperation>& GetRenderOperation() const = 0;

	virtual void GetWorldTransforms(Matrix4f* xform) const = 0;
	virtual uint32_t GetWorldTransformsCount() const = 0;

	virtual void AddToRenderQueue();

	virtual void Render();

	virtual void OnRenderBegin();
	virtual void OnRenderEnd();
		
protected:
	shared_ptr<RenderOperation> mRenderOperation;

};


} // Namespace RcEngine

#endif // Renderable_h__
