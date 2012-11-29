#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>
#include <Math/ColorRGBA.h>
#include <Math/Vector.h>

namespace RcEngine {

enum LightType
{
	LT_Point = 0,
	LT_Directional,
	LT_SpotLight,
	LT_AreaLight,
};

class _ApiExport Light : public SceneObject
{
public:
	Light(const String& name);
	virtual ~Light();
	
	void SetLightType(LightType type);
	void SetPosition(const Vector3f& pos);
	void SetDirection(const Vector3f& vec);
	void SetLightDiffuse(const ColorRGBA& color);
	void SetLightSpecular(const ColorRGBA& color);
	void SetAttenuation(float range, float constant, float linear, float quadratic);
	void SetSpotlightRange(float innerAngleRadian, float outerAngleRadian, float falloff = 1.0);
	void SetSpotlightInnerAngle(float innerAngleRadian);
    void SetSpotlightOuterAngle(float outerAngleRadian);
	void SetSpotlightFalloff(float val);
	void SetSpotlightNearClipDistance(float nearClip);
	
	LightType GetLightType() const					{ return mLightType; }
	const Vector3f& GetPosition() const				{ return mLightDirection; }
	const Vector3f& GetDirection() const			{ return mLightPosition; }
	const ColorRGBA& GetLightDiffuse() const		{ return mLightDiffuse; }
	const ColorRGBA& GetLightSpecular() const		{ return mLightSpecular; }
	float GetAttenuationRange () const				{ return mRange; }
	float GetAttenuationConstant () const			{ return mAttenuationConst; }
	float GetAttenuationLinear () const				{ return mAttenuationLinear; }
	float GetAttenuationQuadric () const			{ return mAttenuationQuad; }
	float GetSpotlightInnerAngle() const			{ return mSpotInner; }
	float GetSpotlightOuterAngle() const			{ return mSpotOuter; }
	float GetSpotlightFalloff() const				{ return mSpotFalloff; }
	float GetSpotlightNearClipDistance() const		{ return mSpotNearClip; }


	const Vector3f& GetDerivedPosition() const;
	const Vector3f& GetDerivedDirection() const;

private:
	void UpdateTransform() const;

public:
	static SceneObject* FactoryFunc(const String& name, const NameValuePairList* params);

protected:
	LightType mLightType;
	ColorRGBA mLightDiffuse;
	ColorRGBA mLightSpecular;
	Vector3f mLightDirection;
	Vector3f mLightPosition;
	float mRange;
	float mAttenuationConst;
	float mAttenuationLinear;
	float mAttenuationQuad;
	float mSpotOuter;
	float mSpotInner;
	float mSpotFalloff;
	float mSpotNearClip;

	mutable Vector3f mDerivedPosition;
	mutable Vector3f mDerivedDirection;

	mutable bool mDerivedTransformDirty;
	
};

}