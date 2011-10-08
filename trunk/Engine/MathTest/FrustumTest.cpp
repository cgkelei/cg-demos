#include "ConvertBridge.h"

D3DXPLANE d3dFrustum[6];
Frustum3f myFrustum;



void BuildD3DViewFrustum(const D3DXMATRIX& viewProjection)
{

	// Left plane
	d3dFrustum[0].a = viewProjection._14 + viewProjection._11;
	d3dFrustum[0].b = viewProjection._24 + viewProjection._21;
	d3dFrustum[0].c = viewProjection._34 + viewProjection._31;
	d3dFrustum[0].d = viewProjection._44 + viewProjection._41;

	// Right plane
	d3dFrustum[1].a = viewProjection._14 - viewProjection._11;
	d3dFrustum[1].b = viewProjection._24 - viewProjection._21;
	d3dFrustum[1].c = viewProjection._34 - viewProjection._31;
	d3dFrustum[1].d = viewProjection._44 - viewProjection._41;

	// Top plane
	d3dFrustum[2].a = viewProjection._14 - viewProjection._12;
	d3dFrustum[2].b = viewProjection._24 - viewProjection._22;
	d3dFrustum[2].c = viewProjection._34 - viewProjection._32;
	d3dFrustum[2].d = viewProjection._44 - viewProjection._42;

	// Bottom plane
	d3dFrustum[3].a = viewProjection._14 + viewProjection._12;
	d3dFrustum[3].b = viewProjection._24 + viewProjection._22;
	d3dFrustum[3].c = viewProjection._34 + viewProjection._32;
	d3dFrustum[3].d = viewProjection._44 + viewProjection._42;

	// Near plane
	d3dFrustum[4].a = viewProjection._13;
	d3dFrustum[4].b = viewProjection._23;
	d3dFrustum[4].c = viewProjection._33;
	d3dFrustum[4].d = viewProjection._43;

	// Far plane
	d3dFrustum[5].a = viewProjection._14 - viewProjection._13;
	d3dFrustum[5].b = viewProjection._24 - viewProjection._23;
	d3dFrustum[5].c = viewProjection._34 - viewProjection._33;
	d3dFrustum[5].d = viewProjection._44 - viewProjection._43;

	//// Normalize planes
	//for ( int i = 0; i < 6; i++ )
	//{
	//	D3DXPlaneNormalize( &d3dFrustum[i], &d3dFrustum[i] );
	//}
}

void BuildMyViewFrustum(const Matrix4f& view, const Matrix4f& proj)
{
	myFrustum.Update(view, proj);
}

bool TestFrustum_T()
{
	Matrix4f myLookAtMatrixLH;
	MathUtil::CreateLookAtMatrixLH(myLookAtMatrixLH, Vector3f(0, 0, -10), Vector3f(0, 0, 0), Vector3f(0, 1, 0));

	Matrix4d myLookAtMatrixLHDouble;
	MathUtil::CreateLookAtMatrixLH(myLookAtMatrixLHDouble, Vector3d(0, 0, -10), Vector3d(0, 0, 0), Vector3d(0, 1, 0));

	float fovy = 60;
	float aspect = (float)4 / (float)3;
	float zNear = 1;
	float zFar = 1000;

	Matrix4f myMat;
	MathUtil::CreatePerspectiveFovLH(myMat, fovy, aspect, zNear, zFar);

	Matrix4d myMatDouble;
	MathUtil::CreatePerspectiveFovLH(myMatDouble, (double)fovy, (double)4/ (double)3, (double)zNear, (double)zFar);


	D3DXMATRIX d3dLookAtMatrixLH;
	D3DXVECTOR3 eye(0, 0, -10);
	D3DXVECTOR3 at(0, 0, 0);
	D3DXVECTOR3 up(0, 1, 0);
	D3DXMatrixLookAtLH(&d3dLookAtMatrixLH, &eye, &at, &up);


	D3DXMATRIX d3dMat;
	D3DXMatrixPerspectiveFovLH(&d3dMat, fovy, aspect, zNear, zFar);

	
	assert(d3dLookAtMatrixLH == myLookAtMatrixLH);
	assert(d3dMat == myMat);

	D3DXMATRIX viewProjection;
	viewProjection = d3dLookAtMatrixLH * d3dMat;
	
	Matrix4f myProj = myLookAtMatrixLH * myMat;


	Matrix4d myProjDouble = myLookAtMatrixLHDouble * myMatDouble;

	assert(myProj == viewProjection);

	BuildD3DViewFrustum(viewProjection);
	BuildMyViewFrustum(myLookAtMatrixLH, myMat);

	for (int i = 0; i < 6; i++)
	{
		if(d3dFrustum[i] != myFrustum.Planes[i])
			return false;
	}

	return true;
}

TEST(TestFrustum, HandleNoneZeroInput)
{
	EXPECT_EQ(true, TestFrustum_T());
}



