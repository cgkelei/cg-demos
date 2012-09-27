#include <Graphics/SimpleGeometry.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderOperation.h>
#include <Graphics/Material.h>
#include <Graphics/EffectTechnique.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/GraphicBuffer.h>
#include <Graphics/Effect.h>
#include <Graphics/Camera.h>
#include <Scene/SceneNode.h>
#include <Core/Context.h>
#include <Math/Vector.h>
#include <Math/Matrix.h>


namespace RcEngine {

	
RenderableHelper::RenderableHelper( const String& name )
	: SceneObject(name), mRenderOperation(new RenderOperation)
{

}

RenderableHelper::~RenderableHelper()
{

}

void RenderableHelper::GetWorldTransforms( Matrix4f* xform ) const
{
	xform[0] = GetWorldTransform();
}

SimpleBox::SimpleBox(const String& name)
	: RenderableHelper(name)
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	//mMaterial = factory.CreateMaterialFromFile("SimpleMat", "../Media/Materials/SimpleMat.xml");

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
	mVertexBuffer= factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
	};
	mVertexDecl = factory.CreateVertexDeclaration(vdsc, 1);

	ElementInitData iInitData;
	iInitData.pData = indices;
	iInitData.rowPitch = sizeof(indices);
	iInitData.slicePitch = 0;
	mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);


	mRenderOperation->BaseVertexLocation = 0;
	mRenderOperation->UseIndex = true;
	mRenderOperation->IndexBuffer = mIndexBuffer;
	mRenderOperation->BindVertexStream(mVertexBuffer, mVertexDecl);
	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->IndexType = IBT_Bit16;
	mRenderOperation->StartIndexLocation = 0;
	mRenderOperation->StartVertexLocation = 0;
}

SimpleBox::~SimpleBox()
{

}


//------------------------------------------------------------------------------------------
SimpleTexturedQuad::SimpleTexturedQuad(const String& name)
	: RenderableHelper(name)
{
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	mMaterial = factory.CreateMaterialFromFile("SimpleTextured", "../Media/Materials/SimpleTextured.xml");

	struct PosNormalTexVertex
	{
		Vector3f Pos;
		Vector3f Normal;
		Vector2f Tex;
	};

	PosNormalTexVertex vertives[4] = 
	{
		{ Vector3f( -1.0f,  1.0f, 0.0f ), Vector3f(0.0f, 0.0f, 1.0f), Vector2f(0.0f, 1.0f) },
		{ Vector3f( 1.0f,  1.0f, 0.0f ), Vector3f(0.0f, 0.0f, 1.0f), Vector2f(1.0f, 1.0f) },
		{ Vector3f( 1.0f,  -1.0f, 0.0f ), Vector3f(0.0f, 0.0f, 1.0f), Vector2f(1.0f, 0.0f) },
		{ Vector3f( -1.0f,  -1.0f, 0.0f ), Vector3f(0.0f, 0.0f, 1.0f), Vector2f(0.0f, 0.0f) }
	};

	uint16_t indices[] = 
	{
		0, 3, 2, 0, 2, 1
	};

	ElementInitData vInitData;
	vInitData.pData = vertives;
	vInitData.rowPitch = sizeof(vertives);
	vInitData.slicePitch = 0;
	mVertexBuffer = factory.CreateVertexBuffer(BU_Static, 0, &vInitData);

	VertexElement vdsc[] = {
		VertexElement(0, VEF_Float3,  VEU_Position, 0),
		VertexElement(12, VEF_Float3,  VEU_Normal, 0),
		VertexElement(24, VEF_Float2,  VEU_TextureCoordinate, 0),
	};
	mVertexDecl = factory.CreateVertexDeclaration(vdsc, 3);

	ElementInitData iInitData;
	iInitData.pData = indices;
	iInitData.rowPitch = sizeof(indices);
	iInitData.slicePitch = 0;
	mIndexBuffer = factory.CreateIndexBuffer(BU_Static, 0, &iInitData);


	mRenderOperation->BaseVertexLocation = 0;
	mRenderOperation->UseIndex = true;
	mRenderOperation->IndexBuffer = mIndexBuffer;
	mRenderOperation->BindVertexStream(mVertexBuffer, mVertexDecl);
	mRenderOperation->PrimitiveType = PT_Triangle_List;
	mRenderOperation->IndexType = IBT_Bit16;
	mRenderOperation->StartIndexLocation = 0;
	mRenderOperation->StartVertexLocation = 0;
}

void SimpleTexturedQuad::SetDiffuseTexture( const shared_ptr<Texture>& diffuseMap )
{
	mMaterial->SetTexture("DiffuseMap", diffuseMap);
}

SimpleTexturedQuad::~SimpleTexturedQuad()
{

}

} // Namespace RcEngine




