#ifndef MovableObject_h__
#define MovableObject_h__

#include <Core/Prerequisites.h>
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

	/**
	 * Return a undefined bounding sphere, subclass must overload
	 * this function to return a real bounding sphere if it actually
	 * have a bounding volume.
	 */
	virtual const BoundingSpheref& GetWorldBoundingSphere() const;

	SceneNode* GetParentSceneNode() const { return mParent; }

	bool IsVisible() const { return mVisible; }

	void SetVisible( bool visible ) { mVisible = visible; }

	bool IsAttached() const  {return mAttached; }

	virtual void OnAttach( SceneNode* node ) = 0;
	virtual void OnDetach( SceneNode* node ) = 0;

protected:
	
	String mName;

	BoundingSpheref mBoundingSphere;

	SceneNode* mParent;

	SceneManager* mSceneManger;

	bool mVisible;

	bool mAttached;
};

}



#endif // MovableObject_h__
