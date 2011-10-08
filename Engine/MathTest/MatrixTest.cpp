#include "ConvertBridge.h"


//////////////////////////////////////////////////////////////////////////
bool MatrixMultiply_T()
{
	D3DXMATRIX d3dMat1;
	D3DXMATRIX d3dMat2;
	D3DXMATRIX d3dResult;

	Matrix4f myMat1;
	Matrix4f myMat2;
	Matrix4f myResult;

	for(int iRow = 0; iRow < 4; iRow ++)
		for(int iCol = 0; iCol < 4; iCol++)
		{
			float value = (float)(iRow * 4 + iCol);
			d3dMat1(iRow, iCol) = myMat1(iRow, iCol) = value;
			d3dMat2(iRow, iCol) = myMat2(iRow, iCol) = 16 - value;
		}

	assert(d3dMat1 == myMat1);
	assert(d3dMat2 == myMat2);

	d3dResult = d3dMat1 * d3dMat2;
	myResult = myMat1 * myMat2;

	return d3dResult == myResult;
}

TEST(TestMatrixMultiply, HandleNoneZeroInput)
{
	EXPECT_EQ(true, MatrixMultiply_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreateLookAtMatrixLH_T()
{
	Matrix4f myLookAtMatrixLH;
	MathUtil::CreateLookAtMatrixLH(myLookAtMatrixLH, Vector3f(0, 0, -10), Vector3f(0, 0, 0), Vector3f(0, 1, 0));

	D3DXMATRIX d3dLookAtMatrixLH;
	D3DXVECTOR3 eye(0, 0, -10);
	D3DXVECTOR3 at(0, 0, 0);
	D3DXVECTOR3 up(0, 1, 0);
	D3DXMatrixLookAtLH(&d3dLookAtMatrixLH, &eye, &at, &up);

	return myLookAtMatrixLH == d3dLookAtMatrixLH;
}

TEST(TestCreateLookAtMatrixLH, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreateLookAtMatrixLH_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreateLookAtMatrixRH_T()
{
	Matrix4f myLookAtMatrixRH;
	MathUtil::CreateLookAtMatrixRH(myLookAtMatrixRH, Vector3f(0, 0, -10), Vector3f(0, 0, 0), Vector3f(0, 1, 0));

	D3DXMATRIX d3dLookAtMatrixRH;
	D3DXVECTOR3 eye(0, 0, -10);
	D3DXVECTOR3 at(0, 0, 0);
	D3DXVECTOR3 up(0, 1, 0);
	D3DXMatrixLookAtRH(&d3dLookAtMatrixRH, &eye, &at, &up);

	return myLookAtMatrixRH == d3dLookAtMatrixRH;
};

TEST(TestCreateLookAtMatrixRH, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreateLookAtMatrixRH_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreatePerspectiveFovLH_T()
{
	float fovy = 60;
	float aspect = (float)4 / (float)3;
	float zNear = 1;
	float zFar = 100;

	Matrix4f myMat;
	MathUtil::CreatePerspectiveFovLH(myMat, fovy, aspect, zNear, zFar);

	D3DXMATRIX d3dMat;
	D3DXMatrixPerspectiveFovLH(&d3dMat, fovy, aspect, zNear, zFar);

	return myMat == d3dMat;
}


TEST(TestCreatePerspectiveFovLH, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreatePerspectiveFovLH_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreatePerspectiveFovRH_T()
{
	float fovy = 60;
	float aspect = (float)4 / (float)3;
	float zNear = 1;
	float zFar = 100;

	Matrix4f myMat;
	MathUtil::CreatePerspectiveFovRH(myMat, fovy, aspect, zNear, zFar);

	D3DXMATRIX d3dMat;
	D3DXMatrixPerspectiveFovRH(&d3dMat, fovy, aspect, zNear, zFar);

	return myMat == d3dMat;
}

TEST(TestCreatePerspectiveFovRH, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreatePerspectiveFovRH_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreatePerspectiveLH_T()
{
	float width = 800;
	float height = 600;
	float zNear = 1;
	float zFar = 100;

	Matrix4f myMat;
	MathUtil::CreatePerspectiveLH(myMat, width, height, zNear, zFar);

	D3DXMATRIX d3dMat;
	D3DXMatrixPerspectiveLH(&d3dMat, width, height, zNear, zFar);

	return myMat == d3dMat;
}

TEST(TestCreatePerspectiveLH, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreatePerspectiveLH_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreatePerspectiveRH_T()
{
	float width = 800;
	float height = 600;
	float zNear = 1;
	float zFar = 100;

	Matrix4f myMat;
	MathUtil::CreatePerspectiveRH(myMat, width, height, zNear, zFar);

	D3DXMATRIX d3dMat;
	D3DXMatrixPerspectiveRH(&d3dMat, width, height, zNear, zFar);

	return myMat == d3dMat;
}

TEST(TestCreatePerspectiveRH, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreatePerspectiveRH_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreateRotationX_T()
{
	float angle = 67;

	Matrix4f myMat;
	MathUtil::CreateRotationX(myMat, angle);

	D3DXMATRIX d3dMat;
	D3DXMatrixRotationX(&d3dMat, angle);

	return myMat == d3dMat;
}

TEST(TestCreateRotationX, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreateRotationX_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreateRotationY_T()
{
	float angle = 67;

	Matrix4f myMat;
	MathUtil::CreateRotationY(myMat, angle);

	D3DXMATRIX d3dMat;
	D3DXMatrixRotationY(&d3dMat, angle);

	return myMat == d3dMat;
}


TEST(TestCreateRotationY, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreateRotationY_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreateRotationZ_T()
{
	float angle = 67;

	Matrix4f myMat;
	MathUtil::CreateRotationZ(myMat, angle);

	D3DXMATRIX d3dMat;
	D3DXMatrixRotationZ(&d3dMat, angle);

	return myMat == d3dMat;
}

TEST(TestCreateRotationZ, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreateRotationZ_T());
}

//////////////////////////////////////////////////////////////////////////
bool CreateScaling_T()
{
	float sX = 2;
	float sY = 3;
	float sZ = 4;

	Matrix4f myMat;
	MathUtil::CreateScaling(myMat, sX, sY, sZ);

	D3DXMATRIX d3dMat;
	D3DXMatrixScaling(&d3dMat, sX, sY, sZ);

	return myMat == d3dMat;
}

TEST(TestCreateScaling, HandleNoneZeroInput)
{
	EXPECT_EQ(true, CreateScaling_T());
}
