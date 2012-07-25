#include "Graphics/CameraControler.h"
#include "Graphics/Camera.h"
#include "Math/MathUtil.h"

namespace RcEngine
{
	namespace Render
	{
		using namespace Math;

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


		FirstPersonCameraControler::FirstPersonCameraControler()
		{

		}

		FirstPersonCameraControler::~FirstPersonCameraControler()
		{

		}

		void FirstPersonCameraControler::AttachCamera( Camera* camera )
		{
			CameraControler::AttachCamera(camera);

			Matrix4f viewMat = mAttachedCamera->GetViewMatrix();

			Quaternionf quat = QuaternionFromRotationYawPitchRoll(1.0f, 1.0f, 1.0f);
			
		}

	}
}