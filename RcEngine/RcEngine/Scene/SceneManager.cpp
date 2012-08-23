#include <Scene/SceneManager.h>
#include <Scene/SceneNode.h>
#include <Core/Exception.h>
#include <Core/Context.h>

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

}