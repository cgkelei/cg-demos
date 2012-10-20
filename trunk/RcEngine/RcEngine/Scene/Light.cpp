#include <Scene/Light.h>
#include <Core/Exception.h>
#include <Scene/Node.h>
#include <Math/MathUtil.h>

namespace RcEngine {

Light::Light( const String& name )
	: SceneObject(name), mDerivedTransformDirty(false)
{
	mType = SOT_Light;
	mRenderable = false;
}

Light::~Light()
{

}


void Light::SetPosition( const Vector3f& pos )
{
	mLightPosition = pos;
	mDerivedTransformDirty = true;
}


void Light::SetDirection( const Vector3f& vec )
{
	mLightDirection = vec;
	mDerivedTransformDirty = true;
}

void Light::SetLightDiffuse( const ColorRGBA& color )
{
	mLightDiffuse = color;
}


void Light::SetLightSpecular( const ColorRGBA& color )
{
	mLightSpecular = color;
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

const Vector3f& Light::GetDerivedPosition() const
{
	UpdateTransform();
	return mDerivedPosition;
}

const Vector3f& Light::GetDerivedDirection() const
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
			const Vector3f& parentPosition = mParentNode->GetWorldPosition();
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




}