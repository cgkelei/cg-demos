#include "Graphics/Model.h"

namespace RcEngine
{
	namespace Render
	{


		Model::Model(void)
		{
		}


		Model::~Model(void)
		{
		}

		const shared_ptr<Mesh>& Model::GetMesh( uint32 index ) const
		{
			return mMeshes[index];
		}

		uint32 Model::GetMeshesCount() const
		{
			return mMeshes.size();
		}

		const String& Model::GetName() const
		{
			return mName;
		}

		Material& Model::GetRenderMaterial()
		{
			return (*mMaterial);
		}

		RenderOperation& Model::GetRenderOperation()
		{
			return mRenderOperation;
		}

		const BoundingSpheref& Model::GetBoundingSphere() const
		{
			return mBoundingSphere;
		}

		void Model::OnRenderBegin()
		{

		}

		void Model::OnRenderEnd()
		{

		}

		bool Model::IsSkined() const
		{
			return false;
		}

	}
}