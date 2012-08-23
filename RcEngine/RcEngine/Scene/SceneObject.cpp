#include <Scene/SceneObject.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>

namespace RcEngine {

// undefined bounding sphere
static const BoundingSpheref UnDefineBoundingSphere;

SceneObject::SceneObject()
{

}

SceneObject::SceneObject( const String& name )
{

}

SceneObject::~SceneObject()
{

}

const BoundingSpheref& SceneObject::GetWorldBoundingSphere() const
{
	return UnDefineBoundingSphere;
}

}