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

		class _ApiExport FirstPersonCameraControler : public CameraControler
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
			FirstPersonCameraControler();
			~FirstPersonCameraControler();

			void AttachCamera(Camera* camera);

		private:
			void Move(float x, float y, float z);
			void Rotate(float yaw, float pitch, float roll);

		};
	}
}


#endif // CameraControler_h__