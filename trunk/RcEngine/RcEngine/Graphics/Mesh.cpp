#include "Graphics/Mesh.h"
#include "Graphics/GraphicBuffer.h"
#include "Graphics/RenderOperation.h"
#include "Graphics/Material.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/RenderJob.h"
#include "Content/MeshContentLoader.h"
#include "Content/MaterialContentLoader.h"
#include "Core/Context.h"


namespace RcEngine
{
	namespace Render
	{
		MeshPart::MeshPart( const String& name, const shared_ptr<Mesh>& mesh )
			:mName(name), mParentMesh(mesh)
		{
			mRenderOperation = std::make_shared<RenderOperation>();
		}



		const shared_ptr<Material>& MeshPart::GetMaterial() const
		{
			shared_ptr<Mesh> parent = mParentMesh.lock();
			return parent->GetMaterial(mMaterialID);
		}

		void MeshPart::GetWorldTransforms( Math::Matrix4f* xform ) const
		{
			uint32_t count = GetWorldTransformsCount();
			/*for (uint32_t i = 0; i < count; ++i)
			{

			}*/

			shared_ptr<Mesh> parent = mParentMesh.lock();
			xform[0] = parent->GetWorldMatrix();

		}

		uint32_t MeshPart::GetWorldTransformsCount() const
		{
			return 1;
		}

		shared_ptr<MeshPart> MeshPart::LoadFrom( const shared_ptr<Mesh>& mesh, MeshPartContentLoader* mpLoader )
		{
			if (!mesh || !mpLoader)
			{
				return nullptr;
			}

			RenderFactory& factory = Core::Context::GetSingleton().GetRenderFactory();

			shared_ptr<MeshPart> meshPart = std::make_shared<MeshPart>(mpLoader->Name, mesh);
			meshPart->mMaterialID = mpLoader->MaterialID;
			meshPart->mIndexCount = mpLoader->IndexCount;
			meshPart->mStartIndex = mpLoader->StartIndex;
			meshPart->mIndexFormat = mpLoader->IndexFormat;
			meshPart->mVertexOffset = mpLoader->VertexOffset;
			meshPart->mPrimitiveCount = mpLoader->IndexCount / 3;	// Only support triangles


			ElementInitData iInitData;
			iInitData.pData = mpLoader->IndexData;
			iInitData.rowPitch = mpLoader->IndexCount * ((mpLoader->IndexFormat == IBT_Bit16) ? 2 : 4);
			iInitData.slicePitch = 0;
			meshPart->mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

			meshPart->mRenderOperation->PrimitiveType = PT_Triangle_List;

			// add index buffer
			meshPart->mRenderOperation->BindIndexStream(meshPart->mIndexBuffer, meshPart->mIndexFormat);

			// add vertex buffer
			meshPart->mRenderOperation->BindVertexStream(meshPart->mVertexBuffer, meshPart->mVertexDecl);

			

			return meshPart;
		}

		
		//////////////////////////////////////////////////////////////////////////
		Mesh::Mesh( const String& name, const BoundingSpheref& bs )
			: mName(name), mBoundingSphere(bs)
		{

		}

		Mesh::~Mesh()
		{

		}


		shared_ptr<Mesh> Mesh::LoadFrom( MeshContentLoader*  meshLoader )
		{
			if (!meshLoader)
			{
				return nullptr;
			}

			shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meshLoader->Name, meshLoader->BoundingSphere);

			// Create materials
			vector<MaterialContentLoader*>& matLoaders = meshLoader->MaterialContentLoaders;
			for (size_t i = 0; i < matLoaders.size(); ++i)
			{
				shared_ptr<Material> material = Material::LoadFrom(matLoaders[i]);
				mesh->mMaterials.push_back(material);
			}

			vector<MeshPartContentLoader*>& mpLoaders = meshLoader->MeshPartContentLoaders;
			for (size_t i = 0; i < mpLoaders.size(); ++i)
			{
				shared_ptr<MeshPart> meshpart = MeshPart::LoadFrom(mesh, mpLoaders[i]);
				mesh->mMeshParts.push_back(meshpart);
			}
			return mesh;
		}	

	}
}