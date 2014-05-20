#include <Graphics/Material.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Camera.h>
#include <Graphics/Texture.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectTechnique.h>
#include <Graphics/DepthStencilState.h>
#include <Graphics/SamplerState.h>
#include <Graphics/BlendState.h>
#include <Graphics/RasterizerState.h>
#include <Graphics/RenderQueue.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Core/XMLDom.h>
#include <Core/Utility.h>
#include <Core/Exception.h>
#include <IO/FileSystem.h>
#include <IO/PathUtil.h>
#include <Resource/ResourceManager.h>

namespace {

using namespace RcEngine;

class EffectParamsUsageDefs
{
public:
	EffectParamsUsageDefs()
	{
		mDefs.insert(std::make_pair("WorldMatrix", EPU_WorldMatrix));
		mDefs.insert(std::make_pair("ViewMatrix",  EPU_ViewMatrix));
		mDefs.insert(std::make_pair("ProjectionMatrix", EPU_ProjectionMatrix));
		mDefs.insert(std::make_pair("WorldViewMatrix", EPU_WorldViewMatrix));
		mDefs.insert(std::make_pair("ViewProjectionMatrix", EPU_ViewProjectionMatrix));
		mDefs.insert(std::make_pair("WorldViewProjection", EPU_WorldViewProjection));
		mDefs.insert(std::make_pair("WorldInverseTranspose", EPU_WorldInverseTranspose));
		mDefs.insert(std::make_pair("WorldMatrixInverse", EPU_WorldMatrixInverse));
		mDefs.insert(std::make_pair("ViewMatrixInverse", EPU_ViewMatrixInverse));
		mDefs.insert(std::make_pair("ProjectionMatrixInverse", EPU_ProjectionMatrixInverse));
		mDefs.insert(std::make_pair("AmbientMaterialColor", EPU_Material_Ambient_Color));
		mDefs.insert(std::make_pair("DiffuseMaterialColor", EPU_Material_Diffuse_Color));
		mDefs.insert(std::make_pair("SpecularMaterialColor", EPU_Material_Specular_Color));
		mDefs.insert(std::make_pair("SpecularMaterialPower", EPU_Material_Power));
		mDefs.insert(std::make_pair("EmissiveMaterialColor", EPU_Material_Emissive_Color));
		mDefs.insert(std::make_pair("DiffuseMaterialMap", EPU_Material_Diffuse_Texture));
		mDefs.insert(std::make_pair("SpecularMaterialMap", EPU_Material_Specular_Texture));
		mDefs.insert(std::make_pair("NormalMaterialMap", EPU_Material_Normal_Texture));
		mDefs.insert(std::make_pair("LightColor", EPU_Light_Color));
		mDefs.insert(std::make_pair("LightFalloff", EPU_Light_Attenuation));
		mDefs.insert(std::make_pair("LightDirection", EPU_Light_Dir));
		mDefs.insert(std::make_pair("LightPosition", EPU_Light_Position));
	}

	static EffectParamsUsageDefs& GetInstance()
	{
		static EffectParamsUsageDefs singleton;
		return singleton;
	}

	EffectParameterUsage GetUsageType(const String& str)
	{
		unordered_map<String, EffectParameterUsage>::iterator iter = mDefs.find(str);
		if (iter != mDefs.end())
			return iter->second;

		return EPU_Unknown;
	}

private:
	unordered_map<String, EffectParameterUsage> mDefs;
};

class SamplerDefs
{
public:
	static SamplerDefs& GetSingleton() 
	{
		static SamplerDefs singleton;
		return singleton;
	}

	uint32_t GetSamplerState(const String& name)
	{
		auto iter = mDefs.find(name);
		if (iter != mDefs.end())
			return iter->second;

		return TF_Min_Mag_Mip_Point;
	}

private:
	SamplerDefs()
	{
		mDefs.insert(std::make_pair("Min_Mag_Map_Point", TF_Min_Mag_Mip_Point));
		mDefs.insert(std::make_pair("Min_Point_Map_Linear_Mip_Point", TF_Min_Mag_Point_Mip_Linear));
		mDefs.insert(std::make_pair("Min_Point_Mag_Linear_Mip_Point", TF_Min_Point_Mag_Linear_Mip_Point));
		mDefs.insert(std::make_pair("Min_Point_Mag_Mip_Linear", TF_Min_Point_Mag_Mip_Linear));
		mDefs.insert(std::make_pair("Min_Linear_Mag_Mip_Point", TF_Min_Linear_Mag_Mip_Point));
		mDefs.insert(std::make_pair("Min_Linear_Mag_Point_Mip_Linear", TF_Min_Linear_Mag_Point_Mip_Linear));
		mDefs.insert(std::make_pair("Min_Mag_Linear_Mip_Point", TF_Min_Mag_Linear_Mip_Point));
		mDefs.insert(std::make_pair("Min_Mag_Mip_Linear", TF_Min_Mag_Mip_Linear));
		mDefs.insert(std::make_pair("Anisotropic", TF_Anisotropic));

		mDefs.insert(std::make_pair("Wrap", TAM_Wrap));
		mDefs.insert(std::make_pair("Mirror", TAM_Mirror));
		mDefs.insert(std::make_pair("Clamp", TAM_Clamp));
		mDefs.insert(std::make_pair("Border", TAM_Border));
		mDefs.insert(std::make_pair("MirrorOnce", TAM_Mirror_Once));

		mDefs.insert(std::make_pair("Never", CF_Never));
		mDefs.insert(std::make_pair("Alway",  CF_Always));
		mDefs.insert(std::make_pair("Less", CF_Less));
		mDefs.insert(std::make_pair("LessEqual", CF_LessEqual));
		mDefs.insert(std::make_pair("Equal", CF_Equal));
		mDefs.insert(std::make_pair("NotEqual", CF_NotEqual));
		mDefs.insert(std::make_pair("GreaterEqual", CF_GreaterEqual));
		mDefs.insert(std::make_pair("Greater", CF_Greater));

		mDefs.insert(std::make_pair("VertexShader", ST_Vertex));
		mDefs.insert(std::make_pair("PixelShader", ST_Pixel));
	}

private:
	unordered_map<String, uint32_t> mDefs;
};

uint32_t GetRenderQueueBucket(const String& str) 
{
	if (str == "Overlay")
		return RenderQueue::BucketOverlay;
	else if (str == "Background")
		return RenderQueue::BucketBackground;
	else if (str == "Opaque")
		return RenderQueue::BucketOpaque;
	else if (str == "Transparent")
		return RenderQueue::BucketTransparent;
	else if (str == "Translucent")
		return RenderQueue::BucketTranslucent;
	else if (str == "Opaque")
		return RenderQueue::BucketTransparent;
	else 
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Undefined Queue Bucket", "GetQueueBucket");
}

}

namespace RcEngine {

Material::Material( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Material, creator, handle, name, group)
{
}


Material::~Material(void)
{
	for (MaterialParameter* param : mCachedEffectParams)
		delete param;

	mCachedEffectParams.clear();
}

EffectParameter* Material::GetCustomParameter( EffectParameterUsage usage ) const
{
	for (MaterialParameter* param : mCachedEffectParams)
	{
		if (param->Usage == usage)
			return param->EffectParam;
	}

	return nullptr;
}

EffectParameter* Material::GetCustomParameter( const String& name ) const
{
	for (MaterialParameter* param : mCachedEffectParams)
	{
		if (param->Name == name)
			return param->EffectParam;
	}
	
	return mEffect->GetParameterByName(name);
}

shared_ptr<Resource> Material::Clone()
{
	printf("Clone material: %s\n", mMaterialName.c_str());

	shared_ptr<Material> retVal = std::make_shared<Material>(mCreator, mResourceHandle, mResourceName, mGroup);

	retVal->mMaterialName = mMaterialName;
	
	retVal->mAmbient = mEmissive;
	retVal->mDiffuse = mDiffuse;
	retVal->mSpecular = mSpecular;
	retVal->mEmissive = mEmissive;
	retVal->mPower = mPower;

	retVal->mEffect = std::static_pointer_cast<Effect>(mEffect/*->Clone()*/);
	retVal->mTextures = mTextures;

	for (MaterialParameter* param : mCachedEffectParams)
	{
		MaterialParameter* paramCloned = new MaterialParameter;
		paramCloned->EffectParam = retVal->mEffect->GetParameterByName(param->Name);
		paramCloned->Usage = param->Usage;
		paramCloned->IsSemantic = param->IsSemantic;
		paramCloned->Name = param->Name;
		paramCloned->Type = param->Type;
		retVal->mCachedEffectParams.push_back(paramCloned);
	}

	retVal->SetLoadState(Resource::Loaded);

	return retVal;
}

void Material::SetTexture( const String& name, const shared_ptr<Texture>& texture )
{
	if (mTextures.find(name) != mTextures.end())
	{
		mTextures[name].Texture = texture;
		mEffect->GetParameterByName(name)->SetValue(mTextures[name]);
	}
	else
	{
		String errMsg = "Texture " + name + " not exit in Effect" + mEffect->GetResourceName();
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, errMsg, "Material::SetTexture");
	}
}

void Material::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	shared_ptr<Stream> matStream = fileSystem.OpenStream(mResourceName, mGroup);
	Stream& source = *matStream;	

	XMLDoc doc;
	XMLNodePtr root = doc.Parse(source);

	// material name 
	mMaterialName = root->AttributeString("name", "");

	// effect first
	XMLNodePtr effectNode = root->FirstNode("Effect");
	String effecFile =  effectNode->AttributeString("name", "");		// file name
	
	String parentDir = PathUtil::GetPath(mResourceName);
	String effectResGroup;

	// Test if a effect exits in the same group as material
	if( fileSystem.Exits(parentDir + effecFile, mGroup) )
	{
		effectResGroup = mGroup;
		effecFile = parentDir + effecFile;  // Add material resource directory
	}
	else
		effectResGroup = "General";
	
	/* Effect name is unique resource ID, but with the effect shader macro, we can define different effect
	 * with the same file. So the full effect name is the effect file string + shader macro. By this way, 
	 * we can distinction effects.
	 */
	String effectName = effecFile;

	vector<String> effectFlags;
	for (XMLNodePtr effectFlagNode = effectNode->FirstNode("Flag"); effectFlagNode; effectFlagNode = effectFlagNode->NextSibling("Flag"))
	{
		String flag = effectFlagNode->AttributeString("name", "");
		effectName += " " + flag;
		effectFlags.push_back(flag);
	}
	
	// load effect
	{
		shared_ptr<Resource> effectProtype = resMan.GetResourceByName(RT_Effect, effectName, effectResGroup);
		effectProtype->Load();

		// if use material animation, use effect clone
		//mEffect = std::static_pointer_cast<Effect>(effectProtype->Clone());
		mEffect = std::static_pointer_cast<Effect>(effectProtype);
	}	

	for (XMLNodePtr samplerNode = root->FirstNode("Sampler"); samplerNode; samplerNode = samplerNode->NextSibling("Sampler"))
	{
		SamplerStateDesc desc;
		
		String samplerName = samplerNode->AttributeString("name", "");

		for (XMLNodePtr stateNode = samplerNode->FirstNode("State"); stateNode; stateNode = stateNode->NextSibling("State"))
		{	
			String stateName = stateNode->AttributeString("name", "");
			if (stateName == "Filter")
			{
				String value = stateNode->Attribute("value")->ValueString();
				desc.Filter = (TextureFilter)SamplerDefs::GetSingleton().GetSamplerState(value);
			}
			else if (stateName == "AddressU")
			{
				String value = stateNode->Attribute("value")->ValueString();
				desc.AddressU = (TextureAddressMode)SamplerDefs::GetSingleton().GetSamplerState(value);
			}
			else if (stateName == "AddressV")
			{
				String value = stateNode->Attribute("value")->ValueString();
				desc.AddressV = (TextureAddressMode)SamplerDefs::GetSingleton().GetSamplerState(value);
			}
			else if (stateName == "AddressW")
			{
				String value = stateNode->Attribute("value")->ValueString();
				desc.AddressW = (TextureAddressMode)SamplerDefs::GetSingleton().GetSamplerState(value);
			}
			else if (stateName == "MaxAnisotropy")
			{
				uint32_t value = stateNode->Attribute("value")->ValueUInt();
				if (value < 1 || value > 16)
					ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "MaxAnisotropy range invalid, only[1, 16] supported!",  "Material::LoadImpl");
				desc.MaxAnisotropy = value;
			}
			else if (stateName == "MinLOD")
			{
				float value = stateNode->Attribute("value")->ValueFloat();
				desc.MinLOD = value;
			}
			else if (stateName == "MaxLOD")
			{
				float value = stateNode->Attribute("value")->ValueFloat();
				desc.MaxLOD = value;
			}
			else if (stateName == "MipLODBias")
			{
				float value = stateNode->Attribute("value")->ValueFloat();
				desc.MipLODBias = value;
			}
			else if (stateName == "ComparisonFunc")
			{
				String value = stateNode->Attribute("value")->ValueString();
				desc.ComparisonFunc = (CompareFunction)SamplerDefs::GetSingleton().GetSamplerState(value);
			}
			else if (stateName == "BorderColor")
			{
				float r = stateNode->Attribute("r")->ValueFloat();
				float g = stateNode->Attribute("g")->ValueFloat();
				float b = stateNode->Attribute("b")->ValueFloat();
				float a = stateNode->Attribute("a")->ValueFloat();
				desc.BorderColor = ColorRGBA(r,g,b,a);
			}
			else
			{
				ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Unknown sampler state: " + stateName, 
					"Material::LoadFrom");
			}
		}
		mSamplerStates.insert( std::make_pair(samplerName, factory.CreateSamplerState(desc)) );
	}

	for (XMLNodePtr paramNode = root->FirstNode("Parameter"); paramNode; paramNode = paramNode->NextSibling("Parameter"))
	{
		String paramName =paramNode->Attribute("name")->ValueString();
		EffectParameter* effectParam = mEffect->GetParameterByName(paramName);

		if (!effectParam)
		{
			continue;
			/*ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Material Parameter: " + parameter->Name + "doesn't exit in effect!",
				"Material::LoadFrom");*/
		}

		MaterialParameter* parameter = new MaterialParameter;
		parameter->Name = paramName;
		parameter->EffectParam = effectParam;
		parameter->Type = parameter->EffectParam->GetParameterType();
		String semantic =  paramNode->AttributeString("semantic", "");
		parameter->IsSemantic = semantic.length() > 0;

		if (parameter->IsSemantic)
			parameter->Usage = EffectParamsUsageDefs::GetInstance().GetUsageType(semantic);
		else
			parameter->Usage = EPU_Unknown;

		// texture type
		if (EPT_Texture1D <= parameter->Type && parameter->Type <= EPT_TextureCUBEArray)
		{	
			TextureLayer layer;

			String stageValue = paramNode->Attribute("stage")->ValueString();
			layer.Stage = (ShaderType)SamplerDefs::GetSingleton().GetSamplerState(stageValue);
			layer.TexUnit = paramNode->Attribute("texUnit")->ValueUInt();

			String samplerValue = paramNode->Attribute("sampler")->ValueString();
			if (mSamplerStates.find(samplerValue) == mSamplerStates.end())
				ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Sampler: " + samplerValue + "doesn't exit", "Material::LoadFrom");

			layer.Sampler = mSamplerStates[samplerValue];

			String texFile = paramNode->AttributeString("value", "");
			if (!texFile.empty())
			{
				String texturePath = parentDir + texFile;
				
				shared_ptr<TextureResource> textureRes;
				if (fileSystem.Exits(texturePath, mGroup))
				{
					textureRes = std::static_pointer_cast<TextureResource>(
						resMan.GetResourceByName(RT_Texture, texturePath, mGroup));
				}
				else 
				{
					textureRes = std::static_pointer_cast<TextureResource>(
						resMan.GetResourceByName(RT_Texture, texFile, "General"));

					textureRes->Load();
					layer.Texture = textureRes->GetTexture();
				}		

				if (textureRes)
				{
					textureRes->Load();
					layer.Texture = textureRes->GetTexture();				
				}			
			}

			mTextures[parameter->Name] = layer;
			SetTexture(paramName, layer.Texture);
		}

		// Material Color
		if (parameter->Usage == EPU_Material_Ambient_Color)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
				std::sscanf(value.c_str(), "%f %f %f", &mAmbient[0], &mAmbient[1], &mAmbient[2]);				
		}
		else if (parameter->Usage == EPU_Material_Diffuse_Color)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
				std::sscanf(value.c_str(), "%f %f %f", &mDiffuse[0], &mDiffuse[1], &mDiffuse[2]);		
		}
		else if (parameter->Usage == EPU_Material_Specular_Color)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
				std::sscanf(value.c_str(), "%f %f %f", &mSpecular[0], &mSpecular[1], &mSpecular[2]);		
		}
		else if (parameter->Usage == EPU_Material_Emissive_Color)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
				std::sscanf(value.c_str(), "%f %f %f", &mEmissive[0], &mEmissive[1], &mEmissive[2]);
		}
		else if (parameter->Usage == EPU_Material_Power)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
				std::sscanf(value.c_str(), "%f", &mPower);			
		}
			
		mCachedEffectParams.push_back(parameter);
	}

	// Parse render queue bucket
	if (root->FirstNode("Queue"))
	{
		String bucket = root->FirstNode("Queue")->AttributeString("name", "");
		mQueueBucket = GetRenderQueueBucket(bucket);
	}
}

void Material::UnloadImpl()
{

}

shared_ptr<Resource> Material::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Material>(creator, handle, name, group);
}

EffectTechnique* Material::GetCurrentTechnique() const
{
	return mEffect->GetCurrentTechnique();
}

void Material::SetCurrentTechnique( const String& techName )
{
	mEffect->SetCurrentTechnique(techName);
}

void Material::SetCurrentTechnique( uint32_t index )
{
	mEffect->SetCurrentTechnique(index);
}

void Material::ApplyMaterial( const float4x4& world )
{
	RenderDevice* renderDevice = Context::GetSingleton().GetRenderDevicePtr();
	const shared_ptr<Camera>& camera = renderDevice->GetCurrentFrameBuffer()->GetCamera();

	for(size_t i = 0; i < mCachedEffectParams.size(); ++i)
	{
		MaterialParameter* param = mCachedEffectParams[i];
		switch (param->Usage)
		{
		case EPU_WorldMatrix:
			{
				param->EffectParam->SetValue(world);
			}		
			break;
		case EPU_ViewMatrix:
			{
				param->EffectParam->SetValue(camera->GetViewMatrix());
			}			
			break;
		case EPU_ProjectionMatrix:
			{
				param->EffectParam->SetValue(camera->GetEngineProjMatrix());
			}	
			break;
		case EPU_WorldViewMatrix:
			{
				param->EffectParam->SetValue(world * camera->GetViewMatrix());
			}		
			break;
		case EPU_ViewProjectionMatrix:
			{
				param->EffectParam->SetValue(camera->GetViewMatrix() * camera->GetEngineProjMatrix());
			}			
			break;	
		case EPU_WorldViewProjection:
			{
				param->EffectParam->SetValue(world * camera->GetViewMatrix() * camera->GetEngineProjMatrix());
			}			
			break;
		case EPU_WorldInverseTranspose:
			{
				param->EffectParam->SetValue(world.Inverse().Transpose());
			}
			break;
		case EPU_WorldMatrixInverse:
			{
				param->EffectParam->SetValue(world.Inverse());
			}
			break;
		case EPU_ViewMatrixInverse:
			{
				param->EffectParam->SetValue(camera->GetInvViewMatrix());
			}
			break;
		case EPU_ProjectionMatrixInverse:
			{
				param->EffectParam->SetValue(camera->GetInvProjMatrix());
			}
			break;
		case EPU_Material_Ambient_Color:
			{
				param->EffectParam->SetValue(mAmbient);
			}
			break;
		case EPU_Material_Diffuse_Color:
			{
				param->EffectParam->SetValue(mDiffuse);
			}
			break;
		case EPU_Material_Specular_Color:
			{
				param->EffectParam->SetValue(mSpecular);
			}
			break;
		case EPU_Material_Power:
			{
				param->EffectParam->SetValue(mPower);
			}
			break;
		case EPU_Material_Diffuse_Texture:
		case EPU_Material_Specular_Texture:
		case EPU_Material_Normal_Texture:
			{
				param->EffectParam->SetValue(mTextures[param->Name]);
			}
			break;
	
		default:
			{

			}
		}
	}
}

} // Namespace RcEngine