#include "Graphics/SimpleGeometry.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderJob.h"
#include "Graphics/Material.h"
#include "Graphics/EffectTechnique.h"
#include "Graphics/EffectParameter.h"
#include "Graphics/GraphicBuffer.h"
#include "Graphics/Effect.h"
#include "Graphics/Camera.h"
#include "Core/Context.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Content/MeshContentLoader.h"

namespace RcEngine 
{
	namespace Render
	{
		using namespace  Math;
	
		RenderableHelper::RenderableHelper( const String& name )
			: mName(name)
		{

		}

		RenderableHelper::~RenderableHelper()
		{

		}


		SimpleBox::SimpleBox(const String& name)
			: RenderableHelper(name), mRenderOperation( new RenderOperation )
		{
			RenderFactory& factory = Core::Context::GetSingleton().GetRenderFactory();

			mMaterial = factory.CreateMaterialFromFile("SimpleMat", "../Media/Materials/SimpleMat.xml");

			Vector3f vertives[8] = 
			{
				 Vector3f( -1.0f,  1.0f, -1.0f ),
				 Vector3f(  1.0f,  1.0f, -1.0f ),
				 Vector3f(  1.0f,  1.0f,  1.0f ),
				 Vector3f( -1.0f,  1.0f,  1.0f ),
				 Vector3f( -1.0f, -1.0f, -1.0f ),
				 Vector3f(  1.0f, -1.0f, -1.0f ), 
				 Vector3f(  1.0f, -1.0f,  1.0f ), 
				 Vector3f( -1.0f, -1.0f,  1.0f )
			};

			uint16_t indices[] = 
			{
				3,1,0, 2,1,3,
				0,5,4, 1,5,0,
				3,4,7, 0,4,3,
				1,6,5, 2,6,1,
				2,7,6, 3,7,2,
				6,4,5, 7,4,6,
			};

			ElementInitData vInitData;
			vInitData.pData = vertives;
			vInitData.rowPitch = sizeof(vertives);
			vInitData.slicePitch = 0;
			shared_ptr<GraphicsBuffer> vb = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

			VertexElement vdsc[] = {
				VertexElement(0, VEF_Vector3,  VEU_Position, 0),
			};
			shared_ptr<VertexDeclaration> decla = factory.CreateVertexDeclaration(vdsc, 1);

			ElementInitData iInitData;
			iInitData.pData = indices;
			iInitData.rowPitch = sizeof(indices);
			iInitData.slicePitch = 0;
			shared_ptr<GraphicsBuffer> ib = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);


			mRenderOperation->BaseVertexLocation = 0;
			mRenderOperation->UseIndex = true;
			mRenderOperation->IndexBuffer = ib;
			mRenderOperation->BindVertexStream(vb, decla);
			mRenderOperation->PrimitiveType = PT_Triangle_List;
			mRenderOperation->IndexType = IBT_Bit16;
			mRenderOperation->StartIndexLocation = 0;
			mRenderOperation->StartVertexLocation = 0;
		}

		SimpleBox::~SimpleBox()
		{

		}

		const shared_ptr<RenderOperation>& SimpleBox::GetRenderOperation() const
		{
			return mRenderOperation;
		}

	}

}




