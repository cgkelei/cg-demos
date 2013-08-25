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
#include <Graphics/SpriteBatch.h>
#include <Graphics/RenderQueue.h>
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
	: mSkyBox(nullptr), 
	  mSkyBoxNode(nullptr)
{
	Context::GetSingleton().SetSceneManager(this);

	// Register all known scene object types
	RegisterType(SOT_Entity, "Entity Type", nullptr, nullptr, Entity::FactoryFunc);
	RegisterType(SOT_Light, "Light Type", nullptr, nullptr, Light::FactoryFunc);
	RegisterType(SOT_Sprite, "Sprite Type", nullptr, nullptr, SpriteEntity::FactoryFunc);

	mAnimationController = new AnimationController;
	mRenderQueue = new RenderQueue;
}

SceneManager::~SceneManager()
{
	ClearScene();
	SAFE_DELETE(mAnimationController);
	SAFE_DELETE(mRenderQueue);
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

Light* SceneManager::CreateLight( const String& name )
{
	auto lightFactoryIter = mRegistry.find(SOT_Light);
	if (lightFactoryIter == mRegistry.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Light type haven't Registed", "SceneManager::CreateEntity");
	}

	Light* light = static_cast<Light*>((lightFactoryIter->second.factoryFunc)(name, nullptr));
	mSceneObjectCollections[SOT_Light].push_back(light);

	// keep track of light
	mAllSceneLights.push_back(light);

	return light;
}

Entity* SceneManager::CreateEntity( const String& entityName, const String& meshName, const String& groupName )
{
	auto entityFactoryIter = mRegistry.find(SOT_Entity);
	if (entityFactoryIter == mRegistry.end())
	{
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Entity type haven't Registed", "SceneManager::CreateEntity");
	}

	NameValuePairList params;
	params["ResourceGroup"] = groupName;
	params["Mesh"] = meshName;

	Entity* entity = static_cast<Entity*>((entityFactoryIter->second.factoryFunc)(entityName, &params));
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

SpriteEntity* SceneManager::CreateSpriteEntity( const shared_ptr<Texture>& tex, const shared_ptr<Material>& mat )
{
	auto factoryIter = mRegistry.find(SOT_Sprite);
	if (factoryIter == mRegistry.end())
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Entity type haven't Registed", "SceneManager::CreateSpriteEntity");

	SpriteEntity* entity = static_cast<SpriteEntity*>((factoryIter->second.factoryFunc)(mat->GetName(), NULL));
	entity->SetSpriteContent(tex, mat);

	// all sprite is attached in scene root node
	GetRootSceneNode()->AttachObject(entity);

	mSceneObjectCollections[SOT_Sprite].push_back(entity);

	return entity;
}


void SceneManager::CreateSkyBox( const shared_ptr<Texture>& texture, bool cubemap /*= true*/, float distance /*= 100.0f */ )
{
	if (mSkyBox)
	{
		SAFE_DELETE(mSkyBox);
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

void SceneManager::UpdateRenderQueue(Camera* cam, RenderOrder order)
{
	mRenderQueue->ClearAllQueue();

	GetRootSceneNode()->OnUpdateRenderQueues(cam, order);

	// Update skynode same with camera positon, add sky box to render queue
	if (mSkyBoxNode)
	{
		mSkyBoxNode->SetPosition( cam->GetPosition() );
		mRenderQueue->AddToQueue(  RenderQueueItem(SOT_Sky, mSkyBox, 0) ,RenderQueue::BucketSky);
	}
}

void SceneManager::UpdateSceneGraph( float delta )
{
	// update anination controller first 
	mAnimationController->Update(delta);

	// update scene node transform
	GetRootSceneNode()->Update();
}

void SceneManager::RenderScene()
{
	auto& renderBuckets = mRenderQueue->GetAllRenderBuckets();
	
	for (auto iter = renderBuckets.begin(); iter != renderBuckets.end(); ++iter)
	{
		RenderBucket& renderBucket = *(iter->second);

		if (renderBucket.size())
		{
			std::sort(renderBucket.begin(), renderBucket.end(), [](const RenderQueueItem& lhs, const RenderQueueItem& rhs)
			{
				return lhs.SortKey < rhs.SortKey;
			});

			for (auto renderableIter = renderBucket.begin(); renderableIter != renderBucket.end(); ++renderableIter)
			{
				renderableIter->Renderable->Render();
			}
		}
		
	}
}

void SceneManager::ClearScene()
{
	// clear all scene node
	for (auto iter = mAllSceneNodes.begin(); iter != mAllSceneNodes.end(); ++iter)
	{
		SAFE_DELETE(*iter);
	}
	mAllSceneNodes.clear();

	// clear all scene object
	for (auto iIter = mSceneObjectCollections.begin(); iIter != mSceneObjectCollections.end(); ++iIter)
	{
		for (auto jIter = iIter->second.begin(); jIter != iIter->second.end(); ++jIter)
		{
			SAFE_DELETE(*jIter);
		}
	}
	mSceneObjectCollections.clear();


	// lights has delete by mSceneObjectCollections
	mAllSceneLights.clear();
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