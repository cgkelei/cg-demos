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
	: mSkyBox(nullptr), mSkyBoxNode(nullptr), mAnimationController(new AnimationController)
{
	Context::GetSingleton().SetSceneManager(this);

	RegisterType(SOT_Entity, "Entity Type", nullptr, nullptr, Entity::FactoryFunc);
	RegisterType(SOT_Light, "Light Type", nullptr, nullptr, Light::FactoryFunc);
}

SceneManager::~SceneManager()
{
	ClearScene();
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
	SceneNode* root;

	if (mAllSceneNodes.empty())
	{
		root = CreateSceneNodeImpl("SceneRoot");
		mAllSceneNodes.push_back(root);
	}
	else
	{
		root = mAllSceneNodes[0];
	}

	return root;
}

AnimationController* SceneManager::GetAnimationController() const
{
	return mAnimationController;
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

void SceneManager::UpdateRenderQueue(Camera* cam, RenderOrder order)
{
	mRendeQueue.clear();
	GetRootSceneNode()->OnUpdateRenderQueues(cam, order);

	// Update skynode same with camera positon, add sky box to render queue
	if (mSkyBoxNode)
	{
		mSkyBoxNode->SetPosition( cam->GetPosition() );
		mRendeQueue.push_back( RenderQueueItem(SOT_Sky, mSkyBox, 0) );
	}
}


void SceneManager::UpdateSceneGraph( float delta )
{
	// update anination controller first 
	mAnimationController->Update(delta);

	// update scene node transform
	GetRootSceneNode()->Update();
}


Entity* SceneManager::CreateEntity( const String& entityName, const String& meshName, const String& groupName )
{
	auto found = mRegistry.find(SOT_Entity);
	if (found == mRegistry.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Entity type haven't Registed", "SceneManager::CreateEntity");
	}

	/*if (mSceneObjects.find(entityName) != mSceneObjects.end())
	{
	ENGINE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "Scene Object with name " +entityName+" already exits", "SceneManager::CreateLight");
	}*/

	NameValuePairList params;
	params["ResourceGroup"] = groupName;
	params["Mesh"] = meshName;

	Entity* entity = static_cast<Entity*>((found->second.factoryFunc)(entityName, &params));
	mSceneObjectCollections[SOT_Entity].push_back(entity);
	
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

Light* SceneManager::CreateLight( const String& name)
{
	auto found = mRegistry.find(SOT_Light);
	if (found == mRegistry.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Light type haven't Registed", "SceneManager::CreateEntity");
	}

	/*if (mSceneObjects.find(name) != mSceneObjects.end())
	{
		ENGINE_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "Scene Object with name " +name+" already exits", "SceneManager::CreateLight");
	}*/

	Light* light = static_cast<Light*>((found->second.factoryFunc)(name, nullptr));
	mSceneObjectCollections[SOT_Light].push_back(light);

	return light;
}


void SceneManager::ClearScene()
{
	Safe_Delete(mAnimationController);

	for (auto iter = mAllSceneNodes.begin(); iter != mAllSceneNodes.end(); ++iter)
	{
		Safe_Delete(*iter);
	}
	mAllSceneNodes.clear();

	for (auto iIter = mSceneObjectCollections.begin(); iIter != mSceneObjectCollections.end(); ++iIter)
	{
		for (auto jIter = iIter->second.begin(); jIter != iIter->second.end(); ++jIter)
		{
			Safe_Delete(*jIter);
		}
	}
	mSceneObjectCollections.clear();
}

void SceneManager::RegisterType( uint32_t type, const String& typeString, ResTypeInitializationFunc inf, ResTypeReleaseFunc rf, ResTypeFactoryFunc ff )
{
	SceneObjectRegEntry entry;
	entry.typeString = typeString;
	entry.initializationFunc = inf;
	entry.releaseFunc = rf;
	entry.factoryFunc = ff;
	mRegistry[type] = entry;

	// Initialize resource type
	if( inf != 0 ) (*inf)();
}



}