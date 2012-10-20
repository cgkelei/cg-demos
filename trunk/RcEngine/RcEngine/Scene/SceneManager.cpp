#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/SceneObject.h>
#include <Scene/Entity.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Material.h>
#include <Graphics/Mesh.h>
#include <Graphics/Camera.h>
#include <Graphics/Sky.h>
#include <Graphics/AnimationController.h>
#include <Core/Exception.h>
#include <Core/Context.h>
#include <IO/FileStream.h>
#include <IO/FileSystem.h>
#include <Resource/ResourceManager.h>
#include <Scene/SubEntity.h>
#include <Scene/Light.h>

namespace RcEngine {

SceneManager::SceneManager()
	: mSceneRoot(nullptr), mSkyBox(nullptr), mSkyBoxNode(nullptr),
	 mAnimationController(new AnimationController)
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

	// Update skynode same with camera positon, add sky box to render queue
	if (mSkyBoxNode)
	{
		mSkyBoxNode->SetPosition( cam->GetPosition() );
		mRendeQueue.push_back( RenderQueueItem(SOT_Sky, mSkyBox, 0) );
	}
}

void SceneManager::UpdateSceneGraph( float delta )
{
	mAnimationController->Update(delta);
	GetRootSceneNode()->Update();
}


Entity* SceneManager::CreateEntity( const String& entityName, const String& meshName, const String& groupName )
{
	Entity* entity;

	if (mSceneObjects.find(entityName) != mSceneObjects.end())
	{
		ENGINE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "Scene Object with name " +entityName+" already exits", "SceneManager::CreateLight");
	}

	shared_ptr<Mesh> mesh = std::static_pointer_cast<Mesh>(
		ResourceManager::GetSingleton().GetResourceByName(ResourceTypes::Mesh, meshName, groupName));

	mesh->Load();

	entity = new Entity(entityName, mesh);
	mSceneObjects.insert(std::make_pair(entityName, entity));

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

void SceneManager::CreateSkyBox( const shared_ptr<Texture>& texture, bool cubemap /*= true*/, float distance /*= 100.0f */ )
{
	if (mSkyBox)
	{
		Safe_Delete(mSkyBox);
	}

	if (!mSkyBoxNode)
	{
		mSkyBoxNode = static_cast<SceneNode*>(GetRootSceneNode()->CreateChildSceneNode("SkyBox"));
	}


	mSkyBox = new Sky(distance, cubemap);

	if (cubemap)
	{
		mSkyBox->GetMaterial()->SetTexture("SkyCubeMap", texture);
	}
	else
	{
		mSkyBox->GetMaterial()->SetTexture("SkyPlaneMaps", texture);
	}
	

	mSkyBoxNode->AttachObject(mSkyBox);
}

void SceneManager::RenderScene()
{
	for (auto iter = mRendeQueue.begin(); iter != mRendeQueue.end(); ++iter)
	{
		RenderQueueItem& item = *iter;
		item.Renderable->Render();
	}
}

AnimationController* SceneManager::GetAnimationController() const
{
	return mAnimationController;
}

Light* SceneManager::CreateLight( const String& name)
{
	if (mSceneObjects.find(name) != mSceneObjects.end())
	{
		ENGINE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "Scene Object with name " +name+" already exits", "SceneManager::CreateLight");
	}

	Light* light = new Light(name);
	mSceneObjects.insert(std::make_pair(name, light));

	return light;
}

Entity* SceneManager::FindEntity( const String& entityName )
{
	Entity* retVal = nullptr;

	auto found = mSceneObjects.find(entityName);

	if (found != mSceneObjects.end())
	{
		if (found->second->GetSceneObjectType() == SOT_Entity)
		{
			retVal = static_cast<Entity*>(found->second);
		}
	}

	return retVal;
}

Light* SceneManager::FindLight( const String& lightName )
{
	Light* retVal = nullptr;

	auto found = mSceneObjects.find(lightName);

	if (found != mSceneObjects.end())
	{
		if (found->second->GetSceneObjectType() == SOT_Light)
		{
			retVal = static_cast<Light*>(found->second);
		}
	}

	return retVal;
}

}