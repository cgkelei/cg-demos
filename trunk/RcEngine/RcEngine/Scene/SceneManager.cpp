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
#include <Core/Exception.h>
#include <Core/Context.h>
#include <IO/FileStream.h>

namespace RcEngine {

SceneManager::SceneManager()
	: mSceneRoot(nullptr), mSkyBox(nullptr), mSkyBoxNode(nullptr)
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
		Matrix4f mat = mSkyBoxNode->GetWorldTransform();
		mSkyBox->AddToRenderQueue(mRendeQueue);
	}
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


void SceneManager::CreateSkyBox( const shared_ptr<Texture>& frontTex, const shared_ptr<Texture>& backTex, const shared_ptr<Texture>& leftTex, const shared_ptr<Texture>& rightTex, const shared_ptr<Texture>& topTex, const shared_ptr<Texture>& bottomTex, float distance /*= 100.0f */ )
{
	if (mSkyBox)
	{
		Safe_Delete(mSkyBox);
	}

	if (!mSkyBoxNode)
	{
		mSkyBoxNode = static_cast<SceneNode*>(GetRootSceneNode()->CreateChildSceneNode("SkyBox"));
	}

	mSkyBox = new SkyBox(distance, false);
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	shared_ptr<Material> frontPlaneMat = factory.CreateMaterialFromFile("SkyNormal", "../Media/Materials/SkyNormal.material.xml");
	shared_ptr<Material> backPlaneMat = frontPlaneMat->Clone();
	shared_ptr<Material> leftPlaneMat = frontPlaneMat->Clone();
	shared_ptr<Material> rightPlaneMat = frontPlaneMat->Clone();
	shared_ptr<Material> topPlaneMat = frontPlaneMat->Clone();
	shared_ptr<Material> bottomPlaneMat = frontPlaneMat->Clone();

	frontPlaneMat->SetTexture("SkyPlaneMap", frontTex);
	mSkyBox->SetMaterial(SkyBox::Front, frontPlaneMat);

	backPlaneMat->SetTexture("SkyPlaneMap", backTex);
	mSkyBox->SetMaterial(SkyBox::Back, backPlaneMat);

	leftPlaneMat->SetTexture("SkyPlaneMap", leftTex);
	mSkyBox->SetMaterial(SkyBox::Left, leftPlaneMat);

	rightPlaneMat->SetTexture("SkyPlaneMap", rightTex);
	mSkyBox->SetMaterial(SkyBox::Right, rightPlaneMat);

	topPlaneMat->SetTexture("SkyPlaneMap", topTex);
	mSkyBox->SetMaterial(SkyBox::Up, topPlaneMat);

	bottomPlaneMat->SetTexture("SkyPlaneMap", bottomTex);
	mSkyBox->SetMaterial(SkyBox::Down, bottomPlaneMat);

	mSkyBoxNode->AttachObject(mSkyBox);
}

void SceneManager::CreateSkyBox( const shared_ptr<Texture>& cubicTex, float distance /*= 100.0f */ )
{
	if (mSkyBox)
	{
		Safe_Delete(mSkyBox);
	}

	if (!mSkyBoxNode)
	{
		mSkyBoxNode = static_cast<SceneNode*>(GetRootSceneNode()->CreateChildSceneNode("SkyBox"));
	}

	mSkyBox = new SkyBox(distance, true);
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	shared_ptr<Material> cubeMapMat = factory.CreateMaterialFromFile("SkyCubeMap", "../Media/Materials/SkyCubeMap.material.xml");
	cubeMapMat->SetTexture("SkyCubeMap", cubicTex);
	mSkyBox->SetMaterial(cubeMapMat);
	

	mSkyBoxNode->AttachObject(mSkyBox);
}

}