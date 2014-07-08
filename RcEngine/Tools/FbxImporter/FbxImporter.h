#pragma once

#include <fbxsdk.h>
#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/Skeleton.h>
#include <Graphics/VertexDeclaration.h>
#include <Math/MathUtil.h>
#include <Math/ColorRGBA.h>

using namespace RcEngine;

struct ExportSettings
{
	bool ExportSkeleton;
	bool ExportAnimation; 
	bool MergeScene;
	bool MergeWithSameMaterial; // Merge sub mesh with same material
	bool SwapWindOrder;

	ExportSettings()
		: SwapWindOrder(true),
		  ExportSkeleton(true),
		  ExportAnimation(true),
		  MergeScene(false),
		  MergeWithSameMaterial(false)
	{}
};

class FBXTransformer
{
public:
	FBXTransformer() {}

	void Initialize( FbxScene* pScene );

	void TransformMatrix( float4x4* pDestMatrix, const float4x4* pSrcMatrix ) const;
	void TransformPosition( float3* pDestPosition, const float3* pSrcPosition ) const;
	void TransformDirection( float3* pDestDirection, const float3* pSrcDirection ) const;

protected:
	bool  mMaxConversion; // Convert Z-Up to Y-Up
};

class FbxProcesser
{
public:
	struct MaterialData
	{
		String Name;
		float3 Ambient;
		float3 Diffuse;
		float3 Specular;
		float3 Emissive;
		float Power;

		unordered_map<String, String> Textures;
	};

	class BoneWeights
	{
	public:
		BoneWeights() {}

		void AddBoneWeight(int nBoneIndex, float fBoneWeight);

		void Validate();

		void Normalize();
	
		std::vector<std::pair<int, float>>& GetBoneWeights()  { return mBoneWeights; }

	private:
		std::vector<std::pair<int, float>> mBoneWeights;
	};

	struct Vertex
	{
		enum Flags
		{
			ePosition = 1 << 0,
			eBlendWeight =  1 << 1,
			eBlendIndices =  1 << 2,
			eNormal = 1 << 3,
			eTexcoord0 = 1 << 4,
			eTexcoord1 = 1 << 5,
			eTangent = 1 << 6,
			eBinormal = 1 << 7,	
		};

		friend bool operator< (const FbxProcesser::Vertex& lhs, const FbxProcesser::Vertex& rhs)
		{
			#define CMP(value, size)                    \
				for (int i = 0; i < 3; ++i) {             \
					if (lhs.value[i] < rhs.value[i]) return true;   \
					if (rhs.value[i] < lhs.value[i]) return false;  \
				}

			CMP(Position, 3);
			CMP(Normal, 3);
			CMP(Tex0, 2);
			CMP(Tex1, 2);

			#undef CMP

			return false;
		}


		float3 Position;
		float3 Normal;
		float3 Binormal;
		float3 Tangent;
		vector<uint32_t> BlendIndices;
		vector<float> BlendWeights;
		float2 Tex0;
		float2 Tex1;

		uint32_t Flag;
		size_t Index;	// keep track index in vertices
	};

	struct AnimationClipData
	{
		struct  KeyFrame
		{
			// The time offset from the start of the animation to this keyframe.
			float Time;

			float3 Translation;
			Quaternionf Rotation;
			float3 Scale;
		};

		struct  AnimationTrack
		{
			// Bone name
			String Name;
			vector<KeyFrame> KeyFrames;
		};

		float Duration;

		vector<AnimationTrack> mAnimationTracks;
	};

	struct AnimationData
	{
		unordered_map<String, AnimationClipData> AnimationClips;
	};

	struct MeshPartData
	{
		String Name;
		String MaterialName;

		BoundingBoxf Bound;

		vector<uint32_t> Indices;
		vector<Vertex> Vertices;

		vector<VertexElement> VertexDecl;
	};

	struct MeshData
	{
		MeshData() : MeshSkeleton(nullptr) {}

		String Name;
		BoundingBoxf Bound;
		vector<shared_ptr<MeshPartData> > MeshParts;
		shared_ptr<Skeleton> MeshSkeleton;
	};

public:
	FbxProcesser(void);
	~FbxProcesser(void);

	void RunCommand(const vector<String>& arguments);
		
public:
	void Initialize();
	bool LoadScene(const String& filename);

	void ProcessNode(FbxNode* pNode, FbxNodeAttribute::EType attriType);
	void ProcessSkeleton(FbxNode* pNode);
	void ProcessMesh(FbxNode* pNode);
	void ProcessSubDiv(FbxNode* pNode);

	shared_ptr<Skeleton> ProcessBoneWeights(FbxMesh* pMesh, std::vector<BoneWeights>& meshBoneWeights);
	void CalculateBindPose(Bone* bone, std::map<Bone*, FbxAMatrix>& bindPoseMap);
	
	void ProcessAnimation(FbxAnimStack* pStack, FbxNode* pNode, double fFrameRate, double fStart, double fStop);

	void ProcessScene();
	void CollectAnimations();
	void CollectMeshes();
	void CollectMaterials();
	void CollectSkeletons();

	void MergeScene();
	void MergeSubMeshWithSameMaterial();

	void BuildAndSaveXML();
	void BuildAndSaveBinary();	
	void BuildAndSaveMaterial();

	void ExportMaterial();

public:
	FbxManager* mFBXSdkManager;
	FbxScene* mFBXScene;
	bool mQuietMode;
	bool mMergeScene;
	String mSceneName;
	String mOutputPath;
	String mAnimationName;
	
	unordered_map<String, shared_ptr<Skeleton>> mSkeletons;
	unordered_map<String, AnimationData> mAnimations;

	unordered_map<String, FbxNode*> mBoneMap;

	vector<MaterialData> mMaterials;
	vector<shared_ptr<MeshData> > mSceneMeshes;

	FBXTransformer mFBXTransformer;
};


