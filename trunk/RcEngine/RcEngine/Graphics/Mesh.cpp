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

		shared_ptr<MeshPart> MeshPart::LoadFrom( const shared_ptr<Mesh>& mesh, MeshPartContent* mpLoader )
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
			meshPart->mStartVertex = mpLoader->StartVertex;
			meshPart->mPrimitiveCount = mpLoader->IndexCount / 3;	// Only support triangles


			ElementInitData iInitData;
			iInitData.pData = &mpLoader->mFaces[0];
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

		void MeshPart::SetVertexBuffer( const shared_ptr<GraphicsBuffer>& vb, const shared_ptr<VertexDeclaration>& vd )
		{
			mVertexBuffer = vb;
			mVertexDecl = vd;
		}


		void MeshPart::SetIndexBuffer( const shared_ptr<GraphicsBuffer>& indexBuffer, IndexBufferType format )
		{
			mIndexBuffer = indexBuffer;
			mIndexFormat = format;
		}

		void MeshPart::Save( Stream& dest )
		{
			// write submesh name
			dest.WriteString(mName);

			// write vertices count
			dest.WriteUInt(mVertexCount);
			
			// write vertex size
			dest.WriteUInt(mVertexDecl->GetVertexSize());

			// write vertex declaration, element size
			dest.WriteUInt(mVertexDecl->GetElementCount());

			// write each vertex element
			const std::vector<VertexElement>& elements = mVertexDecl->GetElements();
			for (auto iter = elements.begin(); iter != elements.end(); ++iter)
			{
				const VertexElement& ve = *iter;
				dest.WriteUInt(ve.Offset);
				dest.WriteUInt(ve.Type);
				dest.WriteUInt(ve.Usage);
				dest.WriteUShort(ve.UsageIndex);
			}

			// write vertex buffer data
			uint32_t bufferSize = mVertexBuffer->GetBufferSize();
			void* data = mVertexBuffer->Map(0, bufferSize, BA_Read_Only);
			if (!data)
			{
				// Exception
			}
			dest.Write(data, bufferSize);
			mVertexBuffer->UnMap();

			// write index buffer
			bufferSize = mIndexBuffer->GetBufferSize();
			dest.WriteUInt(bufferSize);
			data = mIndexBuffer->Map(0, bufferSize, BA_Read_Only);
			if (data)
			{
				// Exception
			}
			dest.Write(data, bufferSize);
			mIndexBuffer->UnMap();
		}
		
		//////////////////////////////////////////////////////////////////////////
		Mesh::Mesh( const String& name, const BoundingSpheref& bs )
			: mName(name), mBoundingSphere(bs)
		{

		}

		Mesh::~Mesh()
		{

		}


		shared_ptr<Mesh> Mesh::LoadFrom( MeshContent*  meshLoader )
		{
			if (!meshLoader)
			{
				return nullptr;
			}

			shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meshLoader->Name, meshLoader->BoundingSphere);

			// Create materials
			vector<MaterialContent*>& matLoaders = meshLoader->MaterialContentLoaders;
			for (size_t i = 0; i < matLoaders.size(); ++i)
			{
				shared_ptr<Material> material = Material::LoadFrom(matLoaders[i]);
				mesh->mMaterials.push_back(material);
			}

			vector<MeshPartContent*>& mpLoaders = meshLoader->MeshPartContentLoaders;
			for (size_t i = 0; i < mpLoaders.size(); ++i)
			{
				shared_ptr<MeshPart> meshpart = MeshPart::LoadFrom(mesh, mpLoaders[i]);
				mesh->mMeshParts.push_back(meshpart);
			}
			return mesh;
		}	

		void Mesh::Load( Stream& source )
		{
			uint32_t subMeshCount = source.ReadUInt();
			for (uint32_t i = 0 ; i < subMeshCount; ++i)
			{
				String name = source.ReadString();
				shared_ptr<MeshPart> subMesh = std::make_shared<MeshPart>(name, )
			}
		}

		void Mesh::Save( Stream& dest )
		{
			// write submesh count
			dest.WriteUInt(mMeshParts.size());

			// write each submesh
			for (auto iter = mMeshParts.begin(); iter != mMeshParts.end(); ++iter)
			{
				(*iter)->Save(dest);
			}
		}
	}
}