#ifndef Mesh_h__
#define Mesh_h__

#include "Core/Prerequisites.h"
#include "Graphics/GraphicsCommon.h"
#include "Graphics/Renderable.h"
#include "Math/BoundingSphere.h"

namespace RcEngine
{
	namespace Render
	{
		using Math::BoundingSpheref;

		class _ApiExport Mesh : public Renderable
		{
		public:
			Mesh(const shared_ptr<Model>& model, const String& name);
			virtual ~Mesh();

			void AddVertexStream(const shared_ptr<GraphicsBuffer>& buffer, const shared_ptr<VertexDeclaration>& vd);
			void AddIndexStream(const shared_ptr<GraphicsBuffer>& buffer, IndexBufferType type);

			virtual const String& GetName() const;
			virtual Material& GetRenderMaterial() ;
			virtual RenderOperation& GetRenderOperation();
			virtual const BoundingSpheref& GetBoundingSphere() const;

			inline void SetStartVertexLocation(uint32 loc);
			inline uint32 GetStartVertexLocation() const;
			inline void SetBaseVertexLocation(uint32 loc);
			inline uint32 GetBaseVertexLocation() const;
			inline void SetStartIndexLocation(uint32 loc);
			inline uint32 GetStartIndexLocation() const;

		private:
			String mName;
			RenderOperation mRenderOperation;
			BoundingSpheref mBoundingSphere;
			uint32 mPrimitiveCount;

			shared_ptr<Material> mMaterial;
			weak_ptr<Model> mParentModel;

		};

		//////////////////////////////////////////////////////////////////////////
		void Mesh::SetStartVertexLocation( uint32 loc )
		{
			mRenderOperation.mStartVertexLocation = loc;
		}


		uint32 Mesh::GetStartVertexLocation() const
		{
			return mRenderOperation.mStartVertexLocation;
		}

		void Mesh::SetBaseVertexLocation( uint32 loc )
		{
			mRenderOperation.mBaseVertexLocation = loc;
		}

		uint32 Mesh::GetBaseVertexLocation() const
		{
			return mRenderOperation.mBaseVertexLocation;
		}

		void Mesh::SetStartIndexLocation( uint32 loc )
		{
			mRenderOperation.mStartIndexLocation = loc;
		}

		uint32 Mesh::GetStartIndexLocation() const
		{
			return mRenderOperation.mStartIndexLocation;
		}



	}
}

#endif // Mesh_h__
