#include "Graphics/CameraControler.h"
#include "Graphics/Camera.h"
#include "Math/MathUtil.h"
#include "Input/InputSystem.h"
#include "Input/InputDevice.h"
#include "Core/Context.h"

namespace RcEngine
{
	namespace Render
	{
		using namespace Math;
		using namespace Input;

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

			InputSystem* inputSystem = Core::Context::GetSingleton().GetInputSystemPtr();
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

			const Matrix4f& viewMatrix = camera->GetViewMatrix();
			/*Matrix4f invViewMatrix = MatrixInverse(viewMatrix);	

			Vector4f camPos = invViewMatrix.GetRow(3);
			Quaternionf camRot = QuaternionFromRotationMatrix(invViewMatrix);

			// The axis basis vectors and camera position are stored inside the 
			// position matrix in the 4 rows of the camera's world matrix.
			// To figure out the yaw/pitch of the camera, we just need the Z basis vector
			Vector4f zBais = invViewMatrix.GetRow(2);

			mCameraYawAngle = std::atan2(zBais.X(), zBais.Z());
			float len = std::sqrt(zBais.X()* zBais.X() + zBais.Z()* zBais.Z());
			mCameraPitchAngle = -std::atan2(zBais.Y(), len);

			mCameraYawAngle = ToDegree(mCameraYawAngle);
			mCameraPitchAngle = ToDegree(mCameraPitchAngle);*/
	
			Quaternionf quat;
			Vector3f scale, pos;
			MatrixDecompose(scale, quat, pos, viewMatrix);

			mCameraRot = QuaternionInverse(quat);
			QuaternionToRotationYawPitchRoll(mCameraYawAngle, mCameraPitchAngle, mCameraRollAngle, mCameraRot);
		}

		void FPSCameraControler::Move( float x, float y, float z )
		{
			Vector3f newEyePos = mAttachedCamera->GetPosition() + Transform(Vector3f(x,y,z), mCameraRot);
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

		void FPSCameraControler::HandleRoatate( uint32_t action, float value, float dt)
		{
			if (action == TurnLeftRight)
			{
				Rotate(-value * dt, 0 , 0);
			}
			else if (action == TurnUpDown)
			{
				Rotate(0, value * dt, 0);
			}

			
		}

		void FPSCameraControler::Rotate( float yaw, float pitch, float roll )
		{
			mCameraYawAngle += yaw * 0.05f;
			mCameraPitchAngle += pitch * 0.05f;

			mCameraPitchAngle = (std::max)( -Math::PI / 2.0f, mCameraPitchAngle );
			mCameraPitchAngle = (std::min)( +Math::PI / 2.0f, mCameraPitchAngle );

			mCameraRot = QuaternionFromRotationYawPitchRoll(mCameraYawAngle, mCameraPitchAngle, 0.0f);

			// Transform vectors based on camera's rotation matrix
			Vector3f vWorldUp, vWorldAhead;
			Vector3f vLocalUp = Vector3f( 0, 1, 0 );
			Vector3f vLocalAhead = Vector3f( 0, 0, 1 );
			vWorldUp = Transform(vLocalUp, mCameraRot );
			vWorldAhead = Transform( vLocalAhead, mCameraRot );

			mAttachedCamera->SetViewParams(mAttachedCamera->GetPosition(), mAttachedCamera->GetPosition() + vWorldAhead, vWorldUp);
		}

	}
}