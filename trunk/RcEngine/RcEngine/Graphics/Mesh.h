#ifndef Mesh_h__
#define Mesh_h__

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/Renderable.h"
#include "Math/BoundingSphere.h"

namespace RcEngine
{
	//Forward declaration 
	namespace Content
	{
		class MeshContentLoader;
		class MeshPartContentLoader;
	}

	namespace Render
	{
		//using
		using Content::MeshContentLoader;
		using Content::MeshPartContentLoader;
		using Math::BoundingSpheref;
		using Math::Matrix4f;


		class _ApiExport MeshPart : public Renderable
		{
		public:
			MeshPart(const String& name, const shared_ptr<Mesh>& mesh);

			const String& GetName() const								{ return mName; }
			const BoundingSpheref& GetBoundingSphere() const			{ return mBoundingSphere; }

			uint32_t GetVertexCount() const				{ return mVertexCount; }
			uint32_t GetPrimitiveCount() const			{ return mPrimitiveCount; }
			uint32_t GetIndexCount() const				{ return mIndexCount; }
			uint32_t GetStartIndex() const				{ return mStartIndex; }
			uint32_t GetVertexOffset() const			{ return mVertexOffset;}
			uint32_t GetMaterialID() const				{ return mMaterialID; }

			const shared_ptr<GraphicsBuffer>& GetVertexBuffer() const			{ return mVertexBuffer; }
			const shared_ptr<VertexDeclaration>& GetVertexDeclaration() const	{ return mVertexDecl;}

			const shared_ptr<GraphicsBuffer>& GetIndexBuffer()	const	{ return mIndexBuffer; }
			IndexBufferType GetIndexFormat() const						{ return mIndexFormat;  }
		
			const shared_ptr<RenderOperation>& GetRenderOperation() const	{ return mRenderOperation; }
			const shared_ptr<Material>& GetMaterial() const;

			void GetWorldTransforms(Math::Matrix4f* xform) const;
			uint32_t GetWorldTransformsCount() const;

			/** 
			 * Load a mesh part from MeshPartContentLoader
			 * @param mesh The mesh that this is part of.
			 * @param mploader loader contains mesh part data.
			 */
			static shared_ptr<MeshPart> LoadFrom( const shared_ptr<Mesh>& mesh, MeshPartContentLoader* mpLoader);
			
		private:
			weak_ptr<Mesh> mParentMesh;

			String mName;
			uint32_t mMaterialID;

			shared_ptr<GraphicsBuffer>    mVertexBuffer;
			shared_ptr<VertexDeclaration> mVertexDecl;

			shared_ptr<GraphicsBuffer> mIndexBuffer;
			IndexBufferType			   mIndexFormat;

			BoundingSpheref mBoundingSphere;
			
			uint32_t mVertexCount;
			uint32_t mPrimitiveCount; 
			uint32_t mIndexCount;
			uint32_t mStartIndex;
			uint32_t mVertexOffset; 
		};


		class _ApiExport Mesh
		{
		public:
			Mesh(const String& name, const BoundingSpheref& bs);
			virtual ~Mesh();

			const String& GetName() const								{ return mName; }
			const BoundingSpheref& GetBoundingSphere() const			{ return mBoundingSphere; }

			const Matrix4f& GetWorldMatrix() const						{ return mWorld; }
			void SetWorldMatrix(const Matrix4f& mat)					{ mWorld = mat; }

			const shared_ptr<Material>& GetMaterial(size_t i) const 	{ return mMaterials[i]; }

			/** 
			 * Load a mesh from loader which contains the mesh data.
			 */
			static shared_ptr<Mesh> LoadFrom(MeshContentLoader* loader);


		private:
			String mName;
			
			Math::BoundingSpheref mBoundingSphere;

			RenderOperation mRenderOperation;

			uint32_t mPrimitiveCount;
			uint32_t mVertexCount;

			vector<shared_ptr<MeshPart> > mMeshParts;  
			vector<shared_ptr<Material> > mMaterials;

			Math::Matrix4f mWorld;
		};
	}
}

#endif // Mesh_h__
