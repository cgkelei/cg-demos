#include "Player.h"
#include <Resource/ResourceManager.h>
#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Scene/Entity.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/Material.h>
#include <Graphics/RenderQueue.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Graphics/CameraControler.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/AnimationState.h>
#include <Graphics/Animation.h>
#include <Graphics/SamplerState.h>
#include <IO/FileSystem.h>
#include <Core/Context.h>
#include <Math/MathUtil.h>
#include <Graphics/AnimationClip.h>

Player::Player()
{

}

Player::~Player()
{

}

void Player::LoadContent()
{
	RenderFactory* factory = Context::GetSingleton().GetRenderFactoryPtr();
	SceneManager& sceneMan = Context::GetSingleton().GetSceneManager();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	mActorEntity = sceneMan.CreateEntity("Arthas", "Arthas/Mesh_ArthasLichKing.mesh",  "Custom");
	mActorNode = sceneMan.GetRootSceneNode()->CreateChildSceneNode("Arthas");
	mActorNode->SetPosition(float3(0, 0, 0));
	//mActorNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(-90.0f), 0.0f, 0.0f));
	mActorNode->AttachObject(mActorEntity);

	mAnimationPlayer = (SkinnedAnimationPlayer*)mActorEntity->GetAnimationPlayer();
	mAnimationPlayer->GetClip("Take 001")->WrapMode = AnimationState::Wrap_Loop;

	String clipName = "Arthas/Walk.anim";
	shared_ptr<AnimationClip> clip = std::static_pointer_cast<AnimationClip>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Walk");
	mAnimationPlayer->AddClip(clip);
	mAnimationPlayer->GetClip("Walk")->WrapMode = AnimationState::Wrap_Loop;

	mAnimationPlayer->PlayClip("Take 001");

	//mAnimationPlayer->PlayClip("Walk");
}

void Player::Update( float dt )
{

}
