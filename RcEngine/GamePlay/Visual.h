#ifndef Visual_h__
#define Visual_h__

#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>
#include <GamePlay/Component.h>

namespace RcEngine {

class _ApiExport Visual : public Component, public SceneObject
{
public:
	Visual();
	~Visual();

	// Inherit from component
	virtual const String& GetComponentName() const = 0;
	
	// Inherit from SceneObject
	virtual void OnUpdateRenderQueue(RenderQueue* renderQueue, const Camera& cam, RenderOrder order);

	virtual void OnAttach( Node* node ) ;
	virtual void OnDetach( Node* node ) ;

public:
	static const String ComponentName;

};


}


#endif // Visual_h__
