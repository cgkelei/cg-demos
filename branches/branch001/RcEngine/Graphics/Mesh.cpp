#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

namespace RcEngine
{
	namespace Render
	{
		using Math::BoundingSpheref;

		Mesh::Mesh( const shared_ptr<Model>& model, const String& name )
			: mParentModel(model), mName(name)
		{
			mRenderOperation.SetPrimitiveType(PT_Triangle_List);
		}

		Mesh::~Mesh()
		{

		}

		const String& Mesh::GetName() const
		{
			return mName;
		}

		RenderOperation& Mesh::GetRenderOperation() 
		{	
			return mRenderOperation;
		}

		Material& Mesh::GetRenderMaterial()
		{
			return (*mMaterial);
		}

		const BoundingSpheref& Mesh::GetBoundingSphere() const
		{
			return mBoundingSphere;
		}

		void Mesh::AddVertexStream( const shared_ptr<GraphicsBuffer>& buffer, const shared_ptr<VertexDeclaration>& vd )
		{
			mRenderOperation.BindVertexStream(buffer, vd);
		}

		void Mesh::AddIndexStream( const shared_ptr<GraphicsBuffer>& buffer, IndexBufferType type )
		{
			mRenderOperation.BindIndexStream(buffer, type);
		}

		

		

	}
}