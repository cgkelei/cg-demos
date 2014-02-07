#pragma once

#include <fbxsdk.h>
#include <Core/Prerequisites.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/Skeleton.h>
#include <Graphics/VertexElement.h>
#include <Math/MathUtil.h>
#include <Math/ColorRGBA.h>

using namespace RcEngine;

enum AxisSystem 
{
	Axis_OpenGL,
	Axis_DirectX,
};

struct ExportSettings
{
	bool ExportSkeleton;
	bool ExportAnimation; 
	bool MergeScene;
	AxisSystem AxisSystem;

	ExportSettings()
		: ExportSkeleton(true),
		  ExportAnimation(true),
		  MergeScene(false),
		  AxisSystem(Axis_DirectX)
	{}
};

//class FBXTransformer
//{
//public:
//	FBXTransformer()
//		: mUnitScale( 1.0f )
//	{ }
//
//	void Initialize( FbxScene* pScene );
//
//	void TransformMatrix( float4x4* pDestMatrix, const float4x4* pSrcMatrix ) const;
//	void TransformPosition( float3* pDestPosition, const float3* pSrcPosition ) const;
//	void TransformDirection( float3* pDestDirection, const float3* pSrcDirection ) const;
//	float TransformLength( float inputLength ) const;
//
//	// Sets unit scale for exporting all geometry - works with characters too.
//	void SetUnitScale( const float fScale )	{ mUnitScale = fScale; }
//
//protected:
//	float mUnitScale;
//	bool  m3dMaxConversion;
//};

class FbxProcesser
{
public:
	struct MaterialData
	{
		enum
		{
			MAT_AMBIENT_COLOR = 0x1,
			MAT_DIFFUSE_COLOR = 0x2, 
			MAT_SPECULAR_COLOR = 0x4, 
			MAT_EMISSIVE_COLOR = 0x8, 
			MAT_POWER_COLOR = 0x10,
		};

		MaterialData() 
			: MaterialFlags(0) { }

		String Name;
		float3 Ambient;
		float3 Diffuse;
		float3 Specular;
		float3 Emissive;
		float Power;

		uint32_t MaterialFlags;

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

	#pragma pack(push, 1)
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
	#pragma pack(pop)

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
	
	void ProcessAnimation(FbxAnimStack* pStack, FbxNode* pNode, double fFrameRate, double fStart, double fStop);

	void ProcessScene();
	void CollectAnimations();
	void CollectMeshes();
	void CollectMaterials();
	void CollectSkeletons();

	void MergeScene();

	void BuildAndSaveXML();
	void BuildAndSaveBinary();	

	void ExportMaterial();

private:
	FbxManager* mFBXSdkManager;
	FbxScene* mFBXScene;
	bool mQuietMode;
	bool mMergeScene;
	String mSceneName;
	String mOutputPath;

	unordered_map<String, shared_ptr<Skeleton>> mSkeletons;
	unordered_map<String, AnimationData> mAnimations;

	vector<MaterialData> mMaterials;
	vector<shared_ptr<MeshData> > mSceneMeshes;

	//FBXTransformer mFBXTransformer;
};


