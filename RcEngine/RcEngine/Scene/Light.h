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
	void SetPosition(const float3& pos);
	void SetDirection(const float3& vec);
	void SetLightColor(const ColorRGBA& color);
	void SetAttenuation(float range, float constant, float linear, float quadratic);
	void SetSpotlightRange(float innerAngleRadian, float outerAngleRadian, float falloff = 1.0);
	void SetSpotlightInnerAngle(float innerAngleRadian);
    void SetSpotlightOuterAngle(float outerAngleRadian);
	void SetSpotlightFalloff(float val);
	void SetSpotlightNearClipDistance(float nearClip);
	
	LightType GetLightType() const					{ return mLightType; }
	const float3& GetPosition() const				{ return mLightDirection; }
	const float3& GetDirection() const				{ return mLightPosition; }
	const ColorRGBA& GetLightColor() const			{ return mLightColor; }
	float GetAttenuationRange () const				{ return mRange; }
	float GetAttenuationConstant () const			{ return mAttenuationConst; }
	float GetAttenuationLinear () const				{ return mAttenuationLinear; }
	float GetAttenuationQuadric () const			{ return mAttenuationQuad; }
	float GetSpotlightInnerAngle() const			{ return mSpotInner; }
	float GetSpotlightOuterAngle() const			{ return mSpotOuter; }
	float GetSpotlightFalloff() const				{ return mSpotFalloff; }
	float GetSpotlightNearClipDistance() const		{ return mSpotNearClip; }

	const float3& GetDerivedPosition() const;
	const float3& GetDerivedDirection() const;

private:
	void UpdateTransform() const;

public:
	static SceneObject* FactoryFunc(const String& name, const NameValuePairList* params);

protected:
	LightType mLightType;
	ColorRGBA mLightColor;
	float3 mLightDirection;
	float3 mLightPosition;
	float mRange;
	float mAttenuationConst;
	float mAttenuationLinear;
	float mAttenuationQuad;
	float mSpotOuter;
	float mSpotInner;
	float mSpotFalloff;
	float mSpotNearClip;

	mutable float3 mDerivedPosition;
	mutable float3 mDerivedDirection;

	mutable bool mDerivedTransformDirty;
	
};

}