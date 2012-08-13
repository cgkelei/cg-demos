#ifndef CameraControler_h__
#define CameraControler_h__

#include "Core/Prerequisites.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"


namespace RcEngine
{
	namespace Render
	{
		/**
		 * Camera controller. Controller provides camera movement and rotation control using keyboard and mouse.
		 */
		class _ApiExport CameraControler 
		{
		protected:
			CameraControler();

		public:
			virtual ~CameraControler();

			virtual void AttachCamera(Camera* camera);
			void DetachCamera();

		protected:
			Camera* mAttachedCamera;

		};

		class _ApiExport FPSCameraControler : public CameraControler
		{
		public:
			enum Action
			{
				TurnLeftRight,
				TurnUpDown,
				RollLeft,
				RollRight,

				Forward,
				Backward,
				MoveLeft,
				MoveRight
			};

		public:
			FPSCameraControler();
			~FPSCameraControler();

			void AttachCamera(Camera* camera);

		protected:
			void Move(float x, float y, float z);
			void Rotate(float yaw, float pitch, float roll);
			void HandleMove(uint32_t action, bool value, float delta);
			void HandleRoatate(uint32_t action, float value, float delta);

		protected:
			Math::Quaternionf mCameraRot;
			float mCameraYawAngle;
			float mCameraPitchAngle;
			float mCameraRollAngle;
		};


		class _ApiExport ModelViewerCameraControler : public CameraControler
		{
		public:
			ModelViewerCameraControler();
			~ModelViewerCameraControler();

			void AttachCamera(Camera* camera);

		protected:
			class ArcBall
			{

			};


		};
	}
}


#endif // CameraControler_h__