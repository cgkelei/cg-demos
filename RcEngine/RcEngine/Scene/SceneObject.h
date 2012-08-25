#ifndef MovableObject_h__
#define MovableObject_h__

#include <Core/Prerequisites.h>
#include <Math/Matrix.h>
#include <Math/BoundingSphere.h>

namespace RcEngine {


enum SceneObejctType
{
	SOT_Entity,
	SOT_Light,
	SOT_Camera,
	SOT_Particle
};


/**
 * Abstract class defining a scene object in a scene, referenced from Ogre.
 * Note that scene management use the same design concept whit Ogre. We try 
 * to decouple the struct(the scene graph) from its content.Unlike other engines
 * use complex inheritance hierarchy, subclass every type from scene node. We use
 * Composition, all node content data which can move is subclass scene object.
 * Like Entity, Camera, Light. A scene object can attach to a scene node.
 */
class _ApiExport SceneObject
{
public:
	SceneObject();
	SceneObject( const String& name );
	virtual ~SceneObject();

	const String& GetName() const { return mName; }

	virtual SceneObejctType GetSceneObjectType() const = 0;

	virtual bool Renderable() const = 0;
	/**
	 * Return a undefined bounding sphere, subclass must overload
	 * this function to return a real bounding sphere if it actually
	 * have a bounding volume.
	 */
	virtual const BoundingSpheref& GetWorldBoundingSphere() const;
	
	/**
	 * Get Local bounding sphere.
	 */
	virtual const BoundingSpheref& GetBoundingSphere() const;
	

	SceneNode* GetParentSceneNode() const { return mParent; }

	const Matrix4f& GetWorldTransform() const;

	bool IsVisible() const { return mVisible; }

	void SetVisible( bool visible ) { mVisible = visible; }

	bool IsAttached() const  { return mParent != nullptr; }

	virtual void OnAttach( SceneNode* node ) ;
	virtual void OnDetach( SceneNode* node ) ;

protected:
	
	String mName;

	BoundingSpheref mBoundingSphere;

	SceneNode* mParent;

	SceneManager* mSceneManger;

	bool mVisible;

};

}



#endif // MovableObject_h__
