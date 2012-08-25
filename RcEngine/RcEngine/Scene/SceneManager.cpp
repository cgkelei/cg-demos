#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/SceneObject.h>
#include <Scene/Entity.h>
#include <Graphics/RenderDevice.h>
#include <Core/Exception.h>
#include <Core/Context.h>
#include <Graphics/Mesh.h>
#include <IO/FileStream.h>

namespace RcEngine {

SceneManager::SceneManager()
	: mSceneRoot(nullptr)
{
	Context::GetSingleton().SetSceneManager(this);
}

SceneManager::~SceneManager()
{

}

SceneNode* SceneManager::CreateSceneNode( const String& name )
{
	SceneNode* node = CreateSceneNodeImpl(name);
	mAllSceneNodes.push_back(node);
	return node;
}


SceneNode* SceneManager::CreateSceneNodeImpl( const String& name )
{
	return new SceneNode(this, name);
}

SceneNode* SceneManager::GetRootSceneNode()
{
	if (!mSceneRoot)
	{
		mSceneRoot = CreateSceneNodeImpl("SceneRoot");
	}

	return mSceneRoot;
}

void SceneManager::DestroySceneNode( SceneNode* node )
{
	auto found = std::find(mAllSceneNodes.begin(), mAllSceneNodes.end(), node);

	if (found == mAllSceneNodes.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "SceneNode '" + node->GetName() + "' not found.",
			"SceneManager::DestroySceneNode");
	}

	// detach from parent (don't do this in destructor since bulk destruction behaves differently)
	Node* parentNode = (*found)->GetParent();
	if (parentNode)
	{
		parentNode->DetachChild((*found));
	}

	delete (*found);
	mAllSceneNodes.erase(found);
}

void SceneManager::UpdateRenderQueue( Camera* cam )
{
	mRendeQueue.clear();
	GetRootSceneNode()->FindVisibleObjects(cam);
}

void SceneManager::UpdateSceneGraph( )
{
	GetRootSceneNode()->Update();
}

void SceneManager::AddToRenderQueue( SceneObject* sceneObject )
{
	assert(sceneObject->Renderable() == true);

	if (sceneObject->GetSceneObjectType() == SOT_Entity)
	{
		Entity* entity = static_cast<Entity*>(sceneObject);

		for (uint32_t i = 0;  i < entity->GetNumSubEntities();  ++i)
		{
			RenderQueueItem item;
			item.Type = SOT_Entity;
			item.SortKey = 0;
			item.Renderable = entity->GetSubEntity(i);
			mRendeQueue.push_back( item );
		}
	}

}

void SceneManager::RenderScene()
{
	//RenderDevice* device = Context::GetSingleton().GetRenderDevice();

	for (size_t i = 0; i < mRendeQueue.size(); ++i)
	{
		mRendeQueue[i].Renderable->Render();
	}
}

Entity* SceneManager::CreateEntity( const String& name, const String& filePath )
{
	FileStream modelSource(filePath, RcEngine::FILE_READ);
	
	Entity* entity = new Entity(name, Mesh::Load(modelSource));

	mEntityLists.push_back(entity);

	return entity;
}

SceneNode* SceneManager::FindSceneNode( const String& name ) const
{
	for (size_t i = 0; i < mAllSceneNodes.size(); ++i)
	{
		if (mAllSceneNodes[i]->GetName() == name)
		{
			return mAllSceneNodes[i];
		}
	}
	return nullptr;
}

}