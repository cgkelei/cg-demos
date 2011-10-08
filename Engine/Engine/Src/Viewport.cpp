#include "Viewport.h"
#include "RenderTarget.h"
#include "Camera.h"
#include "Exception.h"

namespace Engine
{

	Viewport::Viewport( RenderTarget* target )
		: mTarget(target)
	{
		mCamera = new Camera();
	}


	Viewport::~Viewport(void)
	{
		SAFE_DELETE(mCamera);
	}

	void Viewport::Update()
	{
		if (mCamera)
		{
			// Tell Camera to render into me
			mCamera->RenderScreen(this);
		}
	}

	RenderTarget* Viewport::GetTarget() const
	{
		assert(mCamera != NULL);
		return mTarget;
	}

	Camera& Viewport::GetCamera()
	{
		assert(mCamera != NULL);
		return *mCamera;
	}

	void Viewport::UpdateDimensions( void )
	{
		mHeight = mTarget->GetHeight();
		mWidth = mTarget->GetWidth();
		mLeft = mTarget->GetLeft();
		mTop = mTarget->GetTop();

		if (mCamera) 
		{
		//	if (mCamera->GetAutoAspectRatio())
		//		mCamera->setAspectRatio((Real) mActWidth / (Real) mActHeight);


		}

	/*	LogMessageStream << "Viewport for camera '" << (mCamera != 0 ? mCamera->getName() : "NULL") << "'"
			<< ", actual dimensions "	<< std::ios::fixed << std::setprecision(2) 
			<< "L: " << mActLeft << " T: " << mActTop << " W: " << mActWidth << " H: " << mActHeight;
*/

	}
}