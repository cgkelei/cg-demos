#ifndef CameraControler_h__
#define CameraControler_h__

#include <Core/Prerequisites.h>
#include <Math/Vector.h>
#include <Math/Quaternion.h>
#include <Math/Matrix.h>


namespace RcEngine {
namespace Render {
		
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
	void HandleRoatate(uint32_t action, int32_t value, float delta);

protected:
	Math::Quaternionf mCameraRot;
	float mCameraYawAngle;
	float mCameraPitchAngle;
	float mCameraRollAngle;
};


class _ApiExport ArcBall
{
public:
	ArcBall();
	~ArcBall();

	void Reset();
	void SetOffset( int32_t offX, int32_t offY ); 

	void OnMove(int32_t mouseX, int32_t mouseY);
	void OnBegin(int32_t mouseX, int32_t mouseY);
	void OnEnd();

	bool IsDragging() const { return mDrag; }

	Math::Quaternionf GetRotation() const { return mQuatNow; }

private:
	Math::Vector3f ScreenToSphere(float screenX, float screenY);

private:
			
	int32_t mWindowWidth;		// arc ball's window width
	int32_t mWindowHeight;		// arc ball's window height

	Math::Vector<int32_t,2> mOffset;	// window offset, or upper-left corner of window		

	Math::Vector2f mCenter;		 // center of arc ball in screen coordinates
	float mRadius;              // arc ball's radius in screen coordinates
	
	float mRadiusTranslation;   // arc ball's radius for translating the target

	bool mConstrain;

	bool mDrag;

	Math::Quaternionf mQuatNow;
	Math::Quaternionf mQuatDown;

	Math::Vector2f mLastMousePoint;

	Math::Vector3f mDownPt;			// starting point of rotation arc on sphere
	Math::Vector3f mCurrentPt;		// current point of rotation arc on sphere


};

class _ApiExport ModelViewerCameraControler : public CameraControler
{
public:
	enum Action 
	{
		ModelView,
		CameraView,
		Turn
	};

public:
	ModelViewerCameraControler();
	~ModelViewerCameraControler();

	void AttachCamera(Camera* camera);

	/**
	 * Get the world matrix of model
	 */
	Math::Matrix4f GetWorldMatrix() const	{ return mWorld; }

protected:
	void HandleModelView(uint32_t action, bool value, float delta);
	void HadnleCameraView(uint32_t action, bool value, float delta);

protected:
	ArcBall mCameraArcBall;
	ArcBall mModelArcBall;
	Math::Quaternionf mCameraRot;
	Math::Matrix4f mWorld; // world matrix of model
};

} // Namespace Render
} // Namespace RcEngine

#endif // CameraControler_h__