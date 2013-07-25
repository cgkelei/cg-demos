#include <Graphics/CameraControler.h>
#include <Graphics/Camera.h>
#include <Math/MathUtil.h>
#include <Input/InputSystem.h>
#include <Input/InputDevice.h>
#include <Core/Context.h>
#include <MainApp/Application.h>
#include <MainApp/Window.h>

namespace RcEngine {

/************************************************************************/
/*                                                                      */
/************************************************************************/
CameraControler::CameraControler()
	: mAttachedCamera(nullptr)
{

}

CameraControler::~CameraControler()
{

}

void CameraControler::AttachCamera( Camera* camera )
{
	mAttachedCamera = camera;
}

void CameraControler::DetachCamera()
{
	mAttachedCamera = nullptr;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
FPSCameraControler::FPSCameraControler()
{
	InputState states[] =
	{
		InputState(KC_W, Forward),
		InputState(KC_S, Backward),
		InputState(KC_A, MoveLeft),
		InputState(KC_D, MoveRight),
		InputState(KC_UpArrow, Forward),
		InputState(KC_DownArrow, Backward),
		InputState(KC_LeftArrow, MoveLeft),
		InputState(KC_RightArrow, MoveRight),
		InputState(KC_Q, RollLeft),
		InputState(KC_E, RollRight)
	};

	InputRange ranges[] =
	{
		InputRange(MS_XDelta, TurnLeftRight),
		InputRange(MS_YDelta, TurnUpDown),
	};

	InputSystem* inputSystem = Context::GetSingleton().GetInputSystemPtr();
	if (inputSystem)
	{
		inputSystem->AddState(states, states+ sizeof(states)/ sizeof(InputState));
		inputSystem->AddStateHandler(Forward, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleMove));
		inputSystem->AddStateHandler(Backward, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleMove));
		inputSystem->AddStateHandler(MoveLeft, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleMove));
		inputSystem->AddStateHandler(MoveRight, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleMove));
				
		inputSystem->AddRange(ranges, ranges+ sizeof(ranges)/ sizeof(InputRange));
		inputSystem->AddRangeHandler(TurnLeftRight, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleRoatate));
		inputSystem->AddRangeHandler(TurnUpDown, fastdelegate::MakeDelegate(this, &FPSCameraControler::HandleRoatate));
	}
			
}

FPSCameraControler::~FPSCameraControler()
{

}

void FPSCameraControler::AttachCamera( Camera* camera )
{
	CameraControler::AttachCamera(camera);		

	const float4x4& viewMatrix = camera->GetViewMatrix();
			
	/*float4x4 invViewMatrix = MatrixInverse(viewMatrix);	

	float4 camPos = invViewMatrix.GetRow(3);
	Quaternionf camRot = QuaternionFromRotationMatrix(invViewMatrix);

	// The axis basis vectors and camera position are stored inside the 
	// position matrix in the 4 rows of the camera's world matrix.
	// To figure out the yaw/pitch of the camera, we just need the Z basis vector
	float4 zBais = invViewMatrix.GetRow(2);

	mCameraYawAngle = std::atan2(zBais.X(), zBais.Z());
	float len = std::sqrt(zBais.X()* zBais.X() + zBais.Z()* zBais.Z());
	mCameraPitchAngle = -std::atan2(zBais.Y(), len);

	mCameraYawAngle = ToDegree(mCameraYawAngle);
	mCameraPitchAngle = ToDegree(mCameraPitchAngle);*/
	
	Quaternionf quat;
	float3 scale, pos;
	MatrixDecompose(scale, quat, pos, viewMatrix);

	mCameraRot = QuaternionInverse(quat);
	QuaternionToRotationYawPitchRoll(mCameraYawAngle, mCameraPitchAngle, mCameraRollAngle, mCameraRot);
}

void FPSCameraControler::Move( float x, float y, float z )
{
	float3 newEyePos = mAttachedCamera->GetPosition() + Transform(float3(x,y,z), mCameraRot);
	mAttachedCamera->SetViewParams(newEyePos, newEyePos + mAttachedCamera->GetView(), mAttachedCamera->GetUp());
}

void FPSCameraControler::HandleMove( uint32_t action, bool value, float delta)
{
	if (value)
	{
	switch(action)
		{
		case MoveLeft:
			Move(-100 *delta, 0, 0);
			break;
		case MoveRight:
			Move(100 *delta, 0, 0);
			break;
		case Forward:
			Move(0, 0, 100*delta);
			break;
		case Backward:
			Move(0, 0, -100 *delta);
			break;
		}
	}

}

void FPSCameraControler::HandleRoatate( uint32_t action, int32_t value, float dt)
{
	if (action == TurnLeftRight)
	{
		Rotate(value * 15 * dt, 0 , 0);
	}
	else if (action == TurnUpDown)
	{
		Rotate(0, value * 15 * dt, 0);
	}
	
	Context::GetSingleton().GetApplication().GetMainWindow()->ForceCursorToCenter();		
}

void FPSCameraControler::Rotate( float yaw, float pitch, float roll )
{
	mCameraYawAngle += yaw * 0.05f;
	mCameraPitchAngle += pitch * 0.05f;

	mCameraPitchAngle = (std::max)( -Mathf::PI / 2.0f, mCameraPitchAngle );
	mCameraPitchAngle = (std::min)( +Mathf::PI / 2.0f, mCameraPitchAngle );

	mCameraRot = QuaternionFromRotationYawPitchRoll(mCameraYawAngle, mCameraPitchAngle, 0.0f);

	// Transform vectors based on camera's rotation matrix
	float3 vWorldUp, vWorldAhead;
	float3 vLocalUp = float3( 0, 1, 0 );
	float3 vLocalAhead = float3( 0, 0, 1 );
	vWorldUp = Transform(vLocalUp, mCameraRot );
	vWorldAhead = Transform( vLocalAhead, mCameraRot );

	mAttachedCamera->SetViewParams(mAttachedCamera->GetPosition(), mAttachedCamera->GetPosition() + vWorldAhead, vWorldUp);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
ArcBall::ArcBall()
{
	Reset();
	SetNoConstraintAxis();
	mCenter.X() = mCenter.Y() = 0;
	mRadius = 1.0f;
}

ArcBall::ArcBall( int32_t windowWidth, int32_t windowHeight )
	: mWindowWidth(windowWidth), mWindowHeight(windowHeight)
{
	Reset();
	SetNoConstraintAxis();
	mCenter = float2( (float)windowWidth/2, (float)windowHeight/2 );
	mRadius = (std::min)( (float)windowWidth/2, (float)windowHeight/2 );
}

ArcBall::~ArcBall()
{

}

void ArcBall::Reset()
{
	mQuatDown.MakeIdentity();
	mQuatNow.MakeIdentity();

	mDrag = false;
}


float3 ArcBall::ScreenToSphere( float screenX, float screenY )
{
	// Scale to screen
	float3 result;

	// map to [-1, 1]
	result.X() = -(screenX - mCenter.X()) / mRadius;
	result.Y() = -(screenY - mCenter.Y()) / mRadius;
	result.Z() = 0.0f;
	
	float mag = LengthSquared(result);

	if( mag > 1.0f )
	{
		result.Normalize();
	}
	else
	{
		result.Z() = sqrtf( 1.0f - mag );
	}

	return result;
}

void ArcBall::OnMove( int32_t mouseX, int32_t mouseY )
{
	if( mDrag )
	{
		mCurrentPt = ScreenToSphere((float)mouseX, (float)mouseY);

		// Calculate quaternion form start and end point
		float3 part = Cross(mDownPt, mCurrentPt);
		float dot = Dot(mDownPt, mCurrentPt);

		mQuatNow = mQuatDown * Quaternionf(dot, part.X(), part.Y(), part.Z());
	}
}

void ArcBall::OnBegin( int32_t mouseX, int32_t mouseY )
{
	// Only enter the drag state if the click falls inside the click rectangle.
	if( mouseX >= 0 && mouseX < mWindowWidth &&
		mouseY >= 0 && mouseY < mWindowHeight )
	{
		mDrag = true;
		mQuatDown = mQuatNow;
		mDownPt = ScreenToSphere((float)mouseX, (float)mouseY);
	}
}

void ArcBall::OnEnd()
{
	mDrag = false;
}

void ArcBall::SetWindowSize( int32_t windowWidth, int32_t windowHeight )
{
	mWindowWidth = windowWidth;
	mWindowHeight = windowHeight;

	// Make the radius of the arcball slightly smaller than the smalest dimension of the screen.
	if (windowWidth > windowHeight)
		mRadius = (windowHeight * .95f) * 0.5f;
	else
		mRadius = (windowWidth * .95f) * 0.5f;

	mCenter = float2( (float)mWindowWidth/2, (float)mWindowHeight/2 );
	
}

void ArcBall::SetCenterAndRadius( const float2& center, float radius )
{
	mCenter = center;
	mRadius = radius;
}


/************************************************************************/
/*                                                                      */
/************************************************************************/
ModelViewerCameraControler::ModelViewerCameraControler()
{
	mWorld.MakeIdentity();
	mCameraRot.MakeIdentity();

	InputState states[] = 
	{
		InputState(MS_LeftButton, CameraView),
		InputState(MS_RightButton, ModelView)
	};

	InputSystem* inputSystem = Context::GetSingleton().GetInputSystemPtr();
	inputSystem->AddState(states, states+ sizeof(states)/ sizeof(InputState));
	inputSystem->AddStateHandler(CameraView, fastdelegate::MakeDelegate(this, &ModelViewerCameraControler::HadnleCameraView));
	inputSystem->AddStateHandler(ModelView, fastdelegate::MakeDelegate(this, &ModelViewerCameraControler::HandleModelView));
}

ModelViewerCameraControler::~ModelViewerCameraControler()
{

}

void ModelViewerCameraControler::AttachCamera( Camera* camera )
{
	CameraControler::AttachCamera(camera);
}

void ModelViewerCameraControler::HadnleCameraView( uint32_t action, bool value, float delta )
{
	assert(action == CameraView);
	// Get mouse device to fetch position
	Mouse* mouse =	Context::GetSingleton().GetInputSystem().GetMouse();
	if (value == true)
	{
		if (mCameraArcBall.IsDragging() == false)
		{
			mCameraArcBall.OnBegin(mouse->X(), mouse->Y());
		}
		else
		{
			mCameraArcBall.OnMove(mouse->X(), mouse->Y());

			// Now need to update camera view matrix
			mCameraRot = QuaternionInverse(mCameraArcBall.GetRotation());

			// Transform vectors based on camera's rotation matrix
			float3 vWorldUp, vWorldAhead;
			float3 vLocalUp = float3( 0, 1, 0 );
			float3 vLocalAhead = float3( 0, 0, 1 );
			vWorldUp = Transform(vLocalUp, mCameraRot );
			vWorldAhead = Transform( vLocalAhead, mCameraRot );

			//mAttachedCamera->SetViewParams(mAttachedCamera->GetPosition(), mAttachedCamera->GetPosition() + vWorldAhead, vWorldUp);


		}
	}
	else
	{
		mCameraArcBall.OnEnd();
	}
}


void ModelViewerCameraControler::HandleModelView( uint32_t action, bool value, float delta )
{
	assert(action == ModelView);
	// Get mouse device to fetch position
	Mouse* mouse =	Context::GetSingleton().GetInputSystem().GetMouse();
	if (value == true)
	{
		if (mModelArcBall.IsDragging() == false)
		{
			mModelArcBall.OnBegin(mouse->X(), mouse->Y());
		}
		else
		{
			mModelArcBall.OnMove(mouse->X(), mouse->Y());
			mWorld = QuaternionToRotationMatrix( mModelArcBall.GetRotation() );
		}
	}
	else
	{
		if (mModelArcBall.IsDragging() == true)
			mModelArcBall.OnEnd();
	}

}

void ModelViewerCameraControler::SetWindowSize( int32_t width, int32_t height )
{
	mCameraArcBall.SetWindowSize(width, height);
	mModelArcBall.SetWindowSize(width, height);
}

void ModelViewerCameraControler::SetCenterAndRadius( const float2& center, float radis )
{
	mCameraArcBall.SetCenterAndRadius(center, radis);
	mModelArcBall.SetCenterAndRadius(center, radis);
}

} // Namespace RcEngine