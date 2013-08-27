#include <Scene/Light.h>
#include <Core/Exception.h>
#include <Scene/Node.h>
#include <Math/MathUtil.h>

namespace RcEngine {

Light::Light( const String& name )
	: SceneObject(name, SOT_Light, false), 
	  mLightType(LT_Point), mLightPosition(float3::Zero()),
	  mLightDirection(float3(0, 0, 1)), mLightColor(ColorRGBA::White),
	  mSpotOuter(Mathf::ToRadian(40.0f)), mSpotInner(Mathf::ToRadian(30.0f)), mSpotFalloff(1.0f), mRange(100000), mAttenuationConst(1.0f),
	  mAttenuationLinear(0.0f), mAttenuationQuad(0.0f), mDerivedPosition(float3::Zero()), 
	  mDerivedDirection(float3(0, 0, 1)), mDerivedTransformDirty(false)
{

}

Light::~Light()
{

}


void Light::SetPosition( const float3& pos )
{
	mLightPosition = pos;
	mDerivedTransformDirty = true;
}

void Light::SetLightColor( const ColorRGBA& color )
{
	mLightColor = color;
}


void Light::SetDirection( const float3& vec )
{
	mLightDirection = vec;
	mDerivedTransformDirty = true;
}

void Light::SetAttenuation( float range, float constant, float linear, float quadratic )
{
	mRange = range;
	mAttenuationConst = constant;
	mAttenuationLinear = linear;
	mAttenuationQuad = quadratic;
}

void Light::SetSpotlightRange( float innerAngleRadian, float outerAngleRadian, float falloff /*= 1.0*/ )
{
	if (mLightType != LT_SpotLight)
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "setSpotlightRange is only valid for spotlights.", "Light::SetSpotlightRange");
	}

	mSpotInner = innerAngleRadian;
	mSpotOuter = outerAngleRadian;
	mSpotFalloff = falloff;
}

void Light::SetSpotlightInnerAngle( float innerAngleRadian )
{
	mSpotInner = innerAngleRadian;
}

void Light::SetSpotlightOuterAngle( float outerAngleRadian )
{
	mSpotOuter = outerAngleRadian;
}

void Light::SetSpotlightFalloff( float val )
{
	mSpotFalloff = val;
}

void Light::SetSpotlightNearClipDistance( float nearClip )
{ 
	mSpotNearClip = nearClip;
}

void Light::SetLightType( LightType type )
{
	mLightType = type;
}

const float3& Light::GetDerivedPosition() const
{
	UpdateTransform();
	return mDerivedPosition;
}

const float3& Light::GetDerivedDirection() const
{
	UpdateTransform();
	return mDerivedDirection;
}

void Light::UpdateTransform() const
{
	if (mDerivedTransformDirty)
	{
		if (mParentNode)
		{
			// Ok, update with SceneNode we're attached to
			const Quaternionf& parentOrientation = mParentNode->GetWorldRotation();
			const float3& parentPosition = mParentNode->GetWorldPosition();
			mDerivedDirection = Transform(mLightDirection, parentOrientation);
			mDerivedPosition = Transform(mLightPosition, parentOrientation) + parentPosition;
		}
		else
		{
			mDerivedPosition = mLightPosition;
			mDerivedDirection = mLightDirection;
		}

		mDerivedTransformDirty = false;
	}
}

SceneObject* Light::FactoryFunc( const String& name, const NameValuePairList* params )
{
	return new Light(name);
}





}