#include "TestApp.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/Texture.h"
#include "Graphics/Effect.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectPass.h"
#include "Graphics/EffectParameter.h"
#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/Camera.h"
#include "Graphics/CameraControler.h"
#include "Graphics/SimpleGeometry.h"
#include "Graphics/Animation.h"
#include "Math/ColorRGBA.h"
#include "Math/MathUtil.h"
#include "Core/Context.h"
#include "Math/BoundingSphere.h"
#include "IO/FileStream.h"
#include "MainApp/Window.h"
#include "Input/InputSystem.h"
#include "Input/InputDevice.h"
#include "Scene/SceneNode.h"
#include "Scene/Entity.h"
#include "Scene/SceneManager.h"
#include "Graphics/Sky.h"
#include "Graphics/SimpleGeometry.h"
#include <D3DX10Math.h>
#include "Core/XMLDom.h"


#pragma comment(lib, "D3DX10.lib")


TestApp::TestApp(void)
{
}


TestApp::~TestApp(void)
{
}

void TestApp::Initialize()
{
	Camera* camera = RcEngine::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();
	
	Vector3f up(0, 1, 0);
	camera->SetViewParams(Vector3f(0, 0, 200), Vector3f(0, 0, 0), up);
	camera->SetProjectionParams(Mathf::PI/4, (float)mSettings.Width / (float)mSettings.Height, 1.0f, 10000.0f );

	mCameraControler = new FPSCameraControler;
	//mCameraControler = new ModelViewerCameraControler();
	//mCameraControler->SetWindowSize(GetMainWindow()->GetWidth(), GetMainWindow()->GetHeight());
	mCameraControler->AttachCamera(camera);
}

void TestApp::LoadContent()
{
	RenderFactory* factory = RcEngine::Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager* sceneManager = Context::GetSingleton().GetSceneManagerPtr();
	
	
	Entity* dwarfEntity = sceneManager->CreateEntity("dwarf", "../Media/Mesh/Dwarf/Dwarf.mdl");
	SceneNode* dwarfNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	dwarfNode->SetPosition(Vector3f(0, 0, 0));
	dwarfNode->SetScale(Vector3f(50, 50, 50));
	dwarfNode->AttachObject(dwarfEntity);

	/*FileStream animationFile;
	animationFile.Open("E:/dude.anim", FILE_READ);
	shared_ptr<Animation> animation = Animation::LoadFrom(animationFile);*/

	
	/*Entity* dudeEntity = sceneManager->CreateEntity("Dude", "../Media/Mesh/Dude/Dude.mdl");
	SceneNode* dudeNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf");
	dudeNode->SetPosition(Vector3f(0, 0, 0));
	dudeNode->AttachObject(dudeEntity);*/
	
	
	//shared_ptr<Material> wireMat = factory->CreateMaterialFromFile("WireFrame", "../Media/Materials/WireFrame.material.xml");

	//Entity* dwarfEntity1 = sceneManager->CreateEntity("Dwarf1", "E:/Code/RcEngine/Tools/MeshImporter/Dude.mdl");
	//dwarfEntity1->SetMaterial(mDwarfMaterial);

	//SceneNode* dwarfNode1 = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf1");
	//dwarfNode1->SetPosition(Vector3f(50, 0, 0));
	//dwarfNode1->AttachObject(dwarfEntity1);

	//Entity* dwarfEntity2 = sceneManager->CreateEntity("Dwarf2", "E:/Code/RcEngine/Tools/MeshImporter/Dude.mdl");
	//dwarfEntity2->SetMaterial(mDwarfMaterial);

	//SceneNode* dwarfNode2 = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Dwarf1");
	//dwarfNode2->SetPosition(Vector3f(-50, 0, 0));
	//dwarfNode2->AttachObject(dwarfEntity2);


	//Entity* plane = sceneManager->CreateEntity("Plane", "../Media/Mesh/Dwarf/Plane.mdl");;
	//plane->SetMaterial(wireMat);
	//SceneNode* planeNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Plane");
	//planeNode->AttachObject(plane);

	//Entity* teapot = sceneManager->CreateEntity("Plane", "../Media/Mesh/Dwarf/Teapot.mdl");;
	//teapot->SetMaterial(mDwarfMaterial);
	//SceneNode* teapotNode = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Plane", Vector3f(-50, 0, 0));
	//teapotNode->AttachObject(teapot);

	//BoundingSpheref sphere = dwarfEntity->GetBoundingSphere();
	//float radus = sphere.Radius;

	//Entity* modelSphereEntity = sceneManager->CreateEntity("ModelBoudningSphere", "E:/Code/RcEngine/Tools/MeshImporter/sphere.mdl");
	//modelSphereEntity->SetMaterial(wireMat);
	//float loacalRadius = modelSphereEntity->GetBoundingSphere().Radius;
	//SceneNode* modelSphere = sceneManager->GetRootSceneNode()->CreateChildSceneNode("ModelBoudningSphere");
	//modelSphere->SetPosition(sphere.Center);
	//modelSphere->SetScale(Vector3f(sphere.Radius/loacalRadius, sphere.Radius/loacalRadius, sphere.Radius/loacalRadius));
	//modelSphere->AttachObject(modelSphereEntity);
		

	/*shared_ptr<Skeleton> skeleton = dwarfEntity->GetSkeleton();
	vector<Bone*> bones = skeleton->GetBones();

	Matrix4f world = bones[1]->GetWorldTransform();*/

	//FileStream fisle;
	//fisle.Open("Skeleton.xml", FILE_READ);
	//shared_ptr<Skeleton> skeleton = Skeleton::LoadFrom(fisle, 0);


	//FileStream file;
	//file.Open("CollisionSpheres.xml", FILE_READ);
	//XMLDoc collisions;
	//XMLNodePtr root = collisions.Parse(file);
	//XMLNodePtr content = root->FirstNode("Asset");

	//for (XMLNodePtr item = content->FirstNode("Item"); item; item = item->NextSibling("Item"))
	//{
	//	String boneName = item->FirstNode("BoneName")->ValueString();
	//	float radius = item->FirstNode("Radius")->ValueFloat();

	//	float offset = 0;
	//	XMLNodePtr offsetNode = item->FirstNode("Offset");
	//	if (offsetNode)
	//	{
	//		offset = offsetNode->ValueFloat();
	//	}


	//	Vector3f center = Vector3f(offset, 0, 0);
	//	BoundingSpheref sphere =  BoundingSpheref(center, radius);

	//	// Transform the BoundingSphere by its parent bone matrix,
	//	// and store the result into the boundingSpheres array.
	//	Bone* bone = skeleton->GetBone(boneName);

	//	Matrix4f trans = bone->GetWorldTransform();
	//	sphere = Transform(sphere, trans);

	//	Entity* entity = sceneManager->CreateEntity("Dwarf", "E:/Code/RcEngine/Tools/MeshImporter/sphere.mdl");
	//	entity->SetMaterial(wireMat);
	//	SceneNode* node = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Bounding");
	//	node->SetScale(sphere.Radius/2);
	//	node->SetPosition(sphere.Center);
	//	node->AttachObject(entity);
	//}

	//for (size_t i = 0; i < 3/*bones.size()*/; ++i)
	//{

	//	Vector3f pos = skeleton->GetBone(i)->GetWorldPosition();
	//	float radius = skeleton->GetBone(i)->mRadius;
	//	String name = skeleton->GetBone(i)->GetName();
	//	
	//	Entity* entity = sceneManager->CreateEntity("Dwarf", "E:/Code/RcEngine/Tools/MeshImporter/sphere.mdl");
	//	entity->SetMaterial(wireMat);
	//	SceneNode* node = sceneManager->GetRootSceneNode()->CreateChildSceneNode("Bounding", pos);

	//	node->SetScale(Vector3f(2, 2, 2));

	//	node->AttachObject(entity);
	//}

	//BoundingSpheref spehre = dwarfEntity->GetBoundingSphere();
	//dwarfNode->SetScale( Vector3f(10.0f / spehre.Radius, 10.0f / spehre.Radius, 10.0f / spehre.Radius) );
	//dwarfNode->AttachObject(dwarfEntity);


	//SceneNode* centerNode = dwarfNode->CreateChildSceneNode("Center", -spehre.Center);
	//centerNode->AttachObject(dwarfEntity);

	/*vector<String> skyTextures;
	skyTextures.push_back( String("../Media/Textures/left.dds"));
	skyTextures.push_back( String("../Media/Textures/right.dds"));
	skyTextures.push_back( String("../Media/Textures/up.dds"));
	skyTextures.push_back( String("../Media/Textures/down.dds"));
	skyTextures.push_back( String("../Media/Textures/front.dds"));
	skyTextures.push_back( String("../Media/Textures/back.dds"));

	auto texture = factory->CreateTextureArrayFromFile(skyTextures);*/

	// Sky 
	//sceneManager->CreateSkyBox(texture, false);
	//sceneManager->CreateSkyBox(factory->CreateTextureFromFile("../Media/Textures/MeadowTrail.dds", 0));
	//sceneManager->CreateSkyBox(factory->CreateTextureFromFile("../Media/Textures/grassenvmap1024.dds", 0), 5000.0f); 
}


void TestApp::UnloadContent()
{

}



void TestApp::Render()
{
	//RenderDevice* device = Context::GetSingleton().GetRenderDevicePtr();
	//device->GetCurrentFrameBuffer()->Clear(CF_Color | CF_Depth |CF_Stencil, 
	//	RcEngine::ColorRGBA(1.1f, 1.1f, 1.1f, 1.0f), 1.0f, 0);
	//device->GetCurrentFrameBuffer()->SwapBuffers();
}


void TestApp::Update( float deltaTime )
{
	/*static float degree = 0;
	degree += deltaTime * 1.0f;*/

	//SceneNode* dwarfNode = Context::GetSingleton().GetSceneManager().FindSceneNode("Dwarf");
	//dwarfNode->SetRotation(QuaternionFromRotationMatrix(mCameraControler->GetWorldMatrix()));
}


int32_t main()
{
	TestApp app;
	app.Create();
	app.RunGame();
	app.Release();
	return 0;
}