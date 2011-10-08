#include "ConvertBridge.h"


bool operator == (const D3DXMATRIX& d3dMat, const Matrix4f& myMat)
{
	for(int i = 0; i < 4; i++)
		for(int j = 0; j < 4; j++)
		{
			if( d3dMat(i, j) != myMat(i, j) )
				return false;
		}

		return true;
}

bool operator == (const Matrix4f& myMat, const D3DXMATRIX& d3dMat)
{
	return d3dMat == myMat;
}

//////////////////////////////////////////////////////////////////////////
bool operator == (const D3DXPLANE& d3dPlane, const Plane3f& myPlane)
{
	return d3dPlane.a == myPlane.Normal.X() &&
		d3dPlane.b == myPlane.Normal.Y() &&
		d3dPlane.c == myPlane.Normal.Z() &&
		d3dPlane.d == myPlane.Constant;

}

bool operator != (const D3DXPLANE& d3dPlane, const Plane3f& myPlane)
{
	return !(d3dPlane == myPlane);
}

