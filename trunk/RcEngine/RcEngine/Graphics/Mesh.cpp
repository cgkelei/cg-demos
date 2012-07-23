#include "Graphics/Mesh.h"
#include "Graphics/GraphicBuffer.h"
#include "Graphics/RenderOperation.h"
#include "Graphics/Material.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/RenderJob.h"
#include "Content/MeshContentLoader.h"
#include "Content/MaterialContentLoader.h"
#include "Core/Context.h"
#include "Core/Exception.h"


namespace RcEngine
{
	namespace Render
	{
		MeshPart::MeshPart( const String& name, const shared_ptr<Mesh>& mesh )
			:mName(name), mParentMesh(mesh)
		{
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


		//shared_ptr<MeshPart> MeshPart::LoadFrom( const shared_ptr<Mesh>& mesh, MeshPartContent* mpLoader )
		//{
		//	if (!mesh || !mpLoader)
		//	{
		//		return nullptr;
		//	}

		//	RenderFactory& factory = Core::Context::GetSingleton().GetRenderFactory();

		//	shared_ptr<MeshPart> meshPart = std::make_shared<MeshPart>(mpLoader->Name, mesh);
		//	meshPart->mMaterialID = mpLoader->MaterialID;
		//	meshPart->mIndexCount = mpLoader->IndexCount;
		//	meshPart->mStartIndex = mpLoader->StartIndex;
		//	meshPart->mIndexFormat = mpLoader->IndexFormat;
		//	meshPart->mStartVertex = mpLoader->StartVertex;
		//	meshPart->mPrimitiveCount = mpLoader->IndexCount / 3;	// Only support triangles


		//	ElementInitData iInitData;
		//	iInitData.pData = &mpLoader->mFaces[0];
		//	iInitData.rowPitch = mpLoader->IndexCount * ((mpLoader->IndexFormat == IBT_Bit16) ? 2 : 4);
		//	iInitData.slicePitch = 0;
		//	meshPart->mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);

		//	meshPart->mRenderOperation->PrimitiveType = PT_Triangle_List;

		//	// add index buffer
		//	meshPart->mRenderOperation->BindIndexStream(meshPart->mIndexBuffer, meshPart->mIndexFormat);

		//	// add vertex buffer
		//	meshPart->mRenderOperation->BindVertexStream(meshPart->mVertexBuffer, meshPart->mVertexDecl);

		//	

		//	return meshPart;
		//}

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


		void MeshPart::Save( const shared_ptr<MeshPart>& meshPart, Stream& dest )
		{
			// write submesh name
			dest.WriteString(meshPart->mName);

			// write vertices count
			dest.WriteUInt(meshPart->mVertexCount);

			// write vertex size
			dest.WriteUInt(meshPart->mVertexDecl->GetVertexSize());

			// write vertex declaration, element size
			dest.WriteUInt(meshPart->mVertexDecl->GetElementCount());

			// write each vertex element
			const std::vector<VertexElement>& elements = meshPart->mVertexDecl->GetElements();
			for (auto iter = elements.begin(); iter != elements.end(); ++iter)
			{
				const VertexElement& ve = *iter;
				dest.WriteUInt(ve.Offset);
				dest.WriteUInt(ve.Type);
				dest.WriteUInt(ve.Usage);
				dest.WriteUShort(ve.UsageIndex);
			}

			// write vertex buffer data
			uint32_t bufferSize = meshPart->mVertexBuffer->GetBufferSize();
			void* data = meshPart->mVertexBuffer->Map(0, bufferSize, BA_Read_Only);
			if (!data)
			{
				// Exception
			}
			dest.Write(data, bufferSize);
			meshPart->mVertexBuffer->UnMap();

			// write triangles count
			dest.WriteUInt(meshPart->mPrimitiveCount);
			bufferSize = meshPart->mIndexBuffer->GetBufferSize();
			data = meshPart->mIndexBuffer->Map(0, bufferSize, BA_Read_Only);
			if (data)
			{
				// Exception
			}
			dest.Write(data, bufferSize);
			meshPart->mIndexBuffer->UnMap();
		}

		shared_ptr<MeshPart> MeshPart::Load( const shared_ptr<Mesh>& mesh, Stream& source )
		{
			String name = source.ReadString();

			// result meshpart
			shared_ptr<MeshPart> meshPart( new MeshPart(name, mesh) );

			// read vertex count
			uint32_t vertexCount = source.ReadUInt();
			uint32_t vertexSize =  source.ReadUInt();

			// read vertex declaration
			uint32_t veCount = source.ReadUInt();
			vector<VertexElement> elements;
			elements.resize(veCount);

			for (size_t i = 0; i < veCount; ++i)
			{
				elements[i].Offset = source.ReadUInt();
				elements[i].Type =  static_cast<VertexElementFormat>(source.ReadUInt());
				elements[i].Usage =  static_cast<VertexElementUsage>(source.ReadUInt());
				elements[i].UsageIndex = source.ReadUShort();
			}

			RenderFactory& factory = Core::Context::GetSingleton().GetRenderFactory();
			
			// create vertex declaration
			meshPart->mVertexDecl = factory.CreateVertexDeclaration(elements);
		
			if (vertexSize != meshPart->mVertexDecl->GetVertexSize())
			{
				ENGINE_EXCEPT(Core::Exception::ERR_RT_ASSERTION_FAILED, "Vertex size in model is not same \
					as the vertex declaration", "MeshPart::Load");
			}

			uint32_t vertexBufferSize = vertexSize * vertexCount;
			
			// create vertex buffer
			ElementInitData vInitData;
			vInitData.pData = NULL;
			vInitData.rowPitch = vertexBufferSize;
			vInitData.slicePitch = 0;
			meshPart->mVertexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);
			void* data = meshPart->mVertexBuffer->Map(0, vertexBufferSize, BA_Write_Only);
			source.Read(data, vertexBufferSize);
			meshPart->mVertexBuffer->UnMap();

			meshPart->mStartVertex = 0;
			meshPart->mVertexCount = vertexCount;
			
			uint32_t triangleCount = source.ReadUInt();
			uint32_t indexBufferSize = sizeof(uint32_t) * 3 * triangleCount;		// Only support 32 bit index
			// create index buffer
			ElementInitData iInitData;
			iInitData.pData = NULL;
			iInitData.rowPitch = indexBufferSize;
			iInitData.slicePitch = 0;
			meshPart->mIndexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &iInitData);
			data = meshPart->mIndexBuffer->Map(0, indexBufferSize, BA_Write_Only);
			source.Read(data, indexBufferSize);
			meshPart->mIndexBuffer->UnMap();
			
			meshPart->mPrimitiveCount = triangleCount;
			meshPart->mIndexFormat = IBT_Bit32;
			meshPart->mStartIndex = 0;
			meshPart->mIndexCount = triangleCount * 3;

			// set render operation
			meshPart->mRenderOperation->BaseVertexLocation = 0;
			meshPart->mRenderOperation->UseIndex = true;
			meshPart->mRenderOperation->IndexBuffer = meshPart->mIndexBuffer;
			meshPart->mRenderOperation->BindVertexStream(meshPart->mVertexBuffer, meshPart->mVertexDecl);
			meshPart->mRenderOperation->PrimitiveType = PT_Triangle_List;
			meshPart->mRenderOperation->IndexType = IBT_Bit32;
			meshPart->mRenderOperation->StartIndexLocation = 0;
			meshPart->mRenderOperation->StartVertexLocation = 0;

			return meshPart;	
		}

	

		//////////////////////////////////////////////////////////////////////////
		Mesh::Mesh( const String& name )
			: mName(name)
		{

		}

		Mesh::~Mesh()
		{

		}


		//shared_ptr<Mesh> Mesh::LoadFrom( MeshContent*  meshLoader )
		//{
		//	if (!meshLoader)
		//	{
		//		return nullptr;
		//	}

		//	shared_ptr<Mesh> mesh = std::make_shared<Mesh>(meshLoader->Name, meshLoader->BoundingSphere);

		//	// Create materials
		//	vector<MaterialContent*>& matLoaders = meshLoader->MaterialContentLoaders;
		//	for (size_t i = 0; i < matLoaders.size(); ++i)
		//	{
		//		shared_ptr<Material> material = Material::LoadFrom(matLoaders[i]);
		//		mesh->mMaterials.push_back(material);
		//	}

		//	vector<MeshPartContent*>& mpLoaders = meshLoader->MeshPartContentLoaders;
		//	for (size_t i = 0; i < mpLoaders.size(); ++i)
		//	{
		//		shared_ptr<MeshPart> meshpart = MeshPart::LoadFrom(mesh, mpLoaders[i]);
		//		mesh->mMeshParts.push_back(meshpart);
		//	}
		//	return mesh;
		//}	

		 void Mesh::SetMaterial( const shared_ptr<Material>& mat )
		 {
			 mMaterials.clear();
			 mMaterials.push_back(mat);

			 for (auto iter = mMeshParts.begin(); iter != mMeshParts.end(); ++iter)
			 {
				 (*iter)->SetMaterialID(0);
			 }
		 }

		 void Mesh::SetMaterial( size_t meshPartIndex, const shared_ptr<Material>& mat )
		 {
			 mMaterials.push_back(mat);
			 size_t matID = mMaterials.size() - 1;

			 mMeshParts[meshPartIndex]->SetMaterialID(matID);
		 }

		 shared_ptr<Mesh> Mesh::Load( Stream& source )
		 {
			 String meshName = source.ReadString();

			 // result mesh
			 shared_ptr<Mesh> mesh( new Mesh(meshName) );

			 uint32_t subMeshCount = source.ReadUInt();
			 for (uint32_t i = 0 ; i < subMeshCount; ++i)
			 {
				 shared_ptr<MeshPart> subMesh = MeshPart::Load(mesh, source);
				 mesh->mMeshParts.push_back(subMesh);
			 }

			 return mesh;
		 }

		 void Mesh::Save( const shared_ptr<Mesh>& mesh, Stream& dest )
		 {
			 // write mesh name
			 dest.WriteString(mesh->mName);

			 // write submesh count
			 dest.WriteUInt(mesh->mMeshParts.size());

			 // write each submesh
			 for (auto iter = mesh->mMeshParts.begin(); iter != mesh->mMeshParts.end(); ++iter)
			 {
				 MeshPart::Save((*iter), dest);
			 }
		 }

	}
}