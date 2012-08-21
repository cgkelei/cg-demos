#include <Scene/Scene.h>
#include <Scene/SceneNode.h>

namespace RcEngine {




	Scene::Scene()
	{

	}

	Scene::~Scene()
	{

	}

	void Scene::AddSceneNode( SceneNode* node )
	{
		assert(node);
		
		// old node scene
		Scene* oldScene = node->GetScene();

		// Same scene, do nothing
		if (oldScene == this)
		{
			return;
		}

		// If the node is part of another scene, remove it.
		if (oldScene && oldScene != this)
		{
			oldScene->RemoveSceneNode(node);
		}

	}

	void Scene::RemoveSceneNode( SceneNode* node )
	{

	}

}