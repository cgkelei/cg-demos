#include "MathUtil.h"

namespace Engine
{

	void MathUtil::CreateLookAtMatrixLH( Matrix4f& out,const Vector3f& position, const Vector3f& target, const Vector3f& upVector )
	{
		Vector3f zaxis = target - position;
		zaxis.Normalize();

		Vector3f xaxis = upVector.crossProduct(zaxis);
		01667                 xaxis.normalize();
		01668 
			01669                 vector3df yaxis = zaxis.crossProduct(xaxis);
		01670 
			01671                 M[0] = (T)xaxis.X;
		01672                 M[1] = (T)yaxis.X;
		01673                 M[2] = (T)zaxis.X;
		01674                 M[3] = 0;
		01675 
			01676                 M[4] = (T)xaxis.Y;
		01677                 M[5] = (T)yaxis.Y;
		01678                 M[6] = (T)zaxis.Y;
		01679                 M[7] = 0;
		01680 
			01681                 M[8] = (T)xaxis.Z;
		01682                 M[9] = (T)yaxis.Z;
		01683                 M[10] = (T)zaxis.Z;
		01684                 M[11] = 0;
		01685 
			01686                 M[12] = (T)-xaxis.dotProduct(position);
		01687                 M[13] = (T)-yaxis.dotProduct(position);
		01688                 M[14] = (T)-zaxis.dotProduct(position);
		01689                 M[15] = 1;
	}	

	void MathUtil::CreateLookAtMatrixRH( Matrix4f& out, const Vector3f& position, const Vector3f& target, const Vector3f& upVector )
	{

	}

	void MathUtil::CreateMatrixPerspectiveFovLH( Matrix4f& out, float fovy, float aspect, float zNear, float zFar )
	{

	}

	void MathUtil::CreateMatrixPerspectiveFovRH( Matrix4f& out, float fovy, float aspect, float zNear, float zFar )
	{

	}

	void MathUtil::CreateMatrixPerspectiveLH( Matrix4f& out, float width, float height, float zNear, float zFar )
	{

	}

	void MathUtil::CreateMatrixPerspectiveRH( Matrix4f& out, float width, float height, float zNear, float zFar )
	{

	}

	void MathUtil::CreateRotationX( Matrix4f& out, float angle )
	{

	}

	void MathUtil::CreateRotationY( Matrix4f& out, float angle )
	{

	}

	void MathUtil::CreateRotationZ( Matrix4f& out, float angle )
	{

	}

	void MathUtil::CreateRotationAxis( Matrix4f& out, const Vector3f& axis, float angle )
	{

	}

	void MathUtil::CreateScaling( Matrix4f& out, float sX, float sY, float sZ )
	{

	}

	void MathUtil::CreateTranslation( Matrix4f& out, float x, float y, float z )
	{

	}

	void MathUtil::CreateTranslation( Matrix4f& out, float x, float y, float z )
	{

	}
}