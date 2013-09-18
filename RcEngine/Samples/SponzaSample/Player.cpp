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
#include <Input/InputSystem.h>

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
	mActorNode->SetScale(float3(3, 3, 3));
	mActorNode->SetRotation(QuaternionFromRotationYawPitchRoll(Mathf::ToRadian(-90.0f), 0.0f, 0.0f));
	mActorNode->AttachObject(mActorEntity);

	mAnimationPlayer = (SkinnedAnimationPlayer*)mActorEntity->GetAnimationPlayer();
	mAnimationState[Random] = mAnimationPlayer->GetClip("Take 001");
	mAnimationState[Random]->WrapMode = AnimationState::Wrap_Loop;


	String clipName = "Arthas/Walk.anim";
	shared_ptr<AnimationClip> clip = std::static_pointer_cast<AnimationClip>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Walk");
	mAnimationState[Walk] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Walk]->WrapMode = AnimationState::Wrap_Loop;

	clipName = "Arthas/Run.anim";
	clip = std::static_pointer_cast<AnimationClip>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Run");
	mAnimationState[Run] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Run]->WrapMode = AnimationState::Wrap_Loop;

	clipName = "Arthas/Fighting_Standby.anim";
	clip = std::static_pointer_cast<AnimationClip>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Fighting_Standby");
	mAnimationState[Fighting_Standby] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Fighting_Standby]->WrapMode = AnimationState::Wrap_Loop;

	clipName = "Arthas/Standby.anim";
	clip = std::static_pointer_cast<AnimationClip>(
	ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Standby");
	mAnimationState[Standby] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Standby]->WrapMode = AnimationState::Wrap_Loop;
	
	clipName = "Arthas/Casting.anim";
	clip = std::static_pointer_cast<AnimationClip>(
		ResourceManager::GetSingleton().GetResourceByName(RT_Animation, clipName, "General"));
	clip->Load();
	clip->SetClipName("Casting");
	mAnimationState[Casting] = mAnimationPlayer->AddClip(clip);
	mAnimationState[Casting]->WrapMode = AnimationState::Wrap_Loop;

	//mAnimationState[Standby]->Play();

	//mAnimationPlayer->PlayClip("Take 001");
	mAnimationPlayer->PlayClip("Walk");
	//mAnimationPlayer->PlayClip("Run");
	//mAnimationPlayer->PlayClip("Fighting_Standby");
	//mAnimationPlayer->PlayClip("Standby");
	//mAnimationPlayer->PlayClip("Casting");
}

void Player::Update( float dt )
{
	if ( InputSystem::GetSingleton().KeyDown(KC_LeftShift) )
	{	
		mAnimationState[Walk]->Stop();
		mAnimationState[Run]->Play();
	}
	else
	{
		mAnimationState[Run]->Stop();
		mAnimationState[Walk]->Play();
	}
}
