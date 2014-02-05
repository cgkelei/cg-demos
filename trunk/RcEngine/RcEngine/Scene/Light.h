#include <Core/Prerequisites.h>
#include <Scene/SceneObject.h>
#include <Math/Vector.h>

namespace RcEngine {

enum LightType
{
	LT_PointLight = 0,
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
	void SetLightColor(const float3& color);

	/** 
	 * How far light is emitted from the center of the object. Point/Spot light only.
	 */
	void SetRange(float range);
	
	/** 
	 * Light attenuation equation for point light only. 
	 * Spot lights attenuate linearly over their range. At the point of light, intensity is at max, 
	 * at the end of light's range, it is zero. Linear drop off in between.
	 */
	void SetAttenuation(float constant = 1.0f, float linear = 25.0f, float quadratic = 0.0f);

	void SetSpotAngle(float innerAngleRadian, float outerAngleRadian, float falloff = 1.0);
	void SetSpotInnerAngle(float innerAngleRadian);
	void SetSpotOuterAngle(float outerAngleRadian);
	void SetSpotFalloff(float exponent);
	

	//void SetSpotlightNearClipDistance(float nearClip);
	
	LightType GetLightType() const					{ return mLightType; }
	const float3& GetPosition() const				{ return mLightDirection; }
	const float3& GetDirection() const				{ return mLightPosition; }
	const float3& GetLightColor() const			    { return mLightColor; }
	const float3& GetAttenuation() const			{ return mAttenuation; }
	float GetRange () const						    { return mRange; }
	float GetSpotInnerAngle() const					{ return mSpotInnerAngle; }
	float GetSpotOuterAngle() const					{ return mSpotOuterAngle; }
	float GetSpotlightFalloff() const				{ return mSpotFalloff; }
	//float GetSpotlightNearClipDistance() const		{ return mSpotNearClip; }

	// Cascade shadow map cout
	uint32_t GetShadowCascades() const				{ return mShadowCascades; }
	void SetShadowCascades(uint32_t count)			{ mShadowCascades = count; }

	const float3& GetDerivedPosition() const;
	const float3& GetDerivedDirection() const;

private:
	void UpdateTransform() const;

public:
	static SceneObject* FactoryFunc(const String& name, const NameValuePairList* params);

protected:
	LightType mLightType;
	float3 mLightColor;
	float3 mLightDirection;
	float3 mLightPosition;
	float3 mAttenuation;
	
	float mRange;	
	
	float mSpotInnerAngle;
	float mSpotOuterAngle;
	float mSpotFalloff;

	float mSpotNearClip;

	mutable float3 mDerivedPosition;
	mutable float3 mDerivedDirection;

	mutable bool mDerivedTransformDirty;

	// for shadow map
	uint32_t mShadowCascades;
	float mShadowMapBais;
};

}