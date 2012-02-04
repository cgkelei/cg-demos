#ifndef Model_h__
#define Model_h__

#include "Core/Prerequisites.h"
#include "Graphics/Renderable.h"
#include "Math/BoundingSphere.h"

namespace RcEngine
{
	namespace Render
	{
		using Math::BoundingSpheref;

		class _ApiExport Model : public Renderable
		{
		public:
			Model(void);
			~Model(void);

			virtual const String& GetName() const ;	
			virtual Material& GetRenderMaterial();
			virtual RenderOperation& GetRenderOperation();
			virtual const BoundingSpheref& GetBoundingSphere() const;
			
			virtual void OnRenderBegin();
			virtual void OnRenderEnd();
			
			virtual bool IsSkined() const; 

			template <typename ForwardIterator>
			void AssignMeshes(ForwardIterator first, ForwardIterator last);

			uint32 GetMeshesCount() const;
			const shared_ptr<Mesh>& GetMesh(uint32 index) const; 


		protected:
			void UpdateBoudingSphere();

		protected:		
			typedef std::vector<shared_ptr<Mesh> > MeshContainer;

			String mName;
			MeshContainer mMeshes;
			BoundingSpheref mBoundingSphere;

			RenderOperation mRenderOperation;
			shared_ptr<Material> mMaterial;
		};

		template <typename ForwardIterator>
		void Model::AssignMeshes( ForwardIterator first, ForwardIterator last )
		{
			mMeshes.assign(first, last);
			UpdateBoudingSphere();
		}

	}
}

#endif // Model_h__
