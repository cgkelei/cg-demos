#include <Graphics/Material.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/Camera.h>
#include <Graphics/Texture.h>
#include <Graphics/Effect.h>
#include <Graphics/EffectTechnique.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/DepthStencilState.h>
#include <Graphics/SamplerState.h>
#include <Graphics/BlendState.h>
#include <Graphics/RasterizerState.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Core/XMLDom.h>
#include <Core/Utility.h>
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
		mDefs.insert(std::make_pair("PowerMaterial", EPU_Material_Power));
		mDefs.insert(std::make_pair("EmissiveMaterialColor", EPU_Material_Emissive_Color));
		mDefs.insert(std::make_pair("DiffuseMaterialMap", EPU_Material_Diffuse_Texture));
		mDefs.insert(std::make_pair("SpecularMaterialMap", EPU_Material_Specular_Texture));
		mDefs.insert(std::make_pair("NormalMaterialMap", EPU_Material_Normal_Texture));
		mDefs.insert(std::make_pair("AmbientLight", EPU_Light_Ambient));
		mDefs.insert(std::make_pair("DiffuseLight", EPU_Light_Diffuse));
		mDefs.insert(std::make_pair("SpecularLight", EPU_Light_Specular));
		mDefs.insert(std::make_pair("DirLight", EPU_Light_Dir));
		mDefs.insert(std::make_pair("PositionLight", EPU_Light_Position));
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
		{
			return iter->second;
		}

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
		{
			return iter->second;
		}

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

		mDefs.insert(std::make_pair("AlwaysFail", CF_AlwaysFail));
		mDefs.insert(std::make_pair("AlwaysPass",  CF_AlwaysPass));
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

}

namespace RcEngine {

Material::Material( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Material, creator, handle, name, group), mTransparent(false), mCurrentTech(nullptr)
{
}


Material::~Material(void)
{
	for (MaterialParameter* param : mCachedEffectParams)
		delete param;

	mCachedEffectParams.clear();
}

void Material::ApplyMaterial()
{
	RenderDevice* renderDevice = Context::GetSingleton().GetRenderDevicePtr();
	Camera* camera = renderDevice->GetCurrentFrameBuffer()->GetCamera();

	for(size_t i = 0; i < mCachedEffectParams.size(); ++i)
	{
		MaterialParameter* param = mCachedEffectParams[i];
		switch (param->Usage)
		{
		case EPU_ViewMatrix:
			{
				param->EffectParam->SetValue(camera->GetViewMatrix());
			}
			break;
		case EPU_ViewProjectionMatrix:
			{
				param->EffectParam->SetValue(camera->GetViewMatrix() * camera->GetProjectionMatrix());
			}
			break;
		case EPU_Material_Ambient_Color:
			{
				float3 color(mAmbient.R(), mAmbient.G(), mAmbient.B());
				param->EffectParam->SetValue(color);
			}
			break;
		case EPU_Material_Diffuse_Color:
			{
				float3 color(mDiffuse.R(), mDiffuse.G(), mDiffuse.B());
				param->EffectParam->SetValue(color);
			}
			break;
		case EPU_Material_Specular_Color:
			{
				float4 color(mSpecular.R(), mSpecular.G(), mSpecular.B(), mSpecular.A());
				param->EffectParam->SetValue(color);
			}
			break;
		case EPU_Material_Power:
			{
				param->EffectParam->SetValue(mPower);
			}
			break;

		case EPU_Material_Diffuse_Texture:
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

	retVal->mEffect = std::static_pointer_cast<Effect>(mEffect->Clone());
	retVal->mCurrentTech = retVal->mEffect->GetTechniqueByName(mCurrentTech->GetTechniqueName());
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

void Material::SetTexture( const String& texUint, const shared_ptr<Texture>& texture )
{
	auto found = mTextures.find(texUint);
	if (found != mTextures.end())
	{
		found->second.Texture = texture;
	}
	else
	{
		ENGINE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Texture Unit: "+texUint +"doesn't exit!", "Material::SetTexture");
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

	/* Effect name is unique resource ID, but with the effect shader macro, we can define different effect
	 * with the same file. So the full effect name is the effect file string + shader macro. By this way, 
	 * we can distinction effects.
	 */
	String effectName = parentDir + effecFile;

	String effectResGroup = "General";
	// Test if a effect exits in the same group as material
	if( fileSystem.Exits(effecFile, mGroup) )
		effectResGroup = mGroup;

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
		mEffect = std::static_pointer_cast<Effect>(effectProtype->Clone());
		mCurrentTech = mEffect->GetTechniqueByIndex(0);  
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
				desc.MaxAnisotropy = value;
			}
			else if (stateName == "MinLod")
			{
				float value = stateNode->Attribute("value")->ValueFloat();
				desc.MinLOD = value;
			}
			else if (stateName == "MaxLod")
			{
				float value = stateNode->Attribute("value")->ValueFloat();
				desc.MaxLOD = value;
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
		if (parameter->Type == EPT_Texture1D || parameter->Type == EPT_Texture2D ||
			parameter->Type == EPT_TextureCUBE || parameter->Type == EPT_Texture3D ||
			parameter->Type == EPT_Texture1DArray || parameter->Type == EPT_Texture2DArray ||
			parameter->Type == EPT_Texture3DArray || parameter->Type == EPT_TextureCUBEArray)
		{	
			TextureLayer layer;

			String stageValue = paramNode->Attribute("stage")->ValueString();
			layer.Stage = (ShaderType)SamplerDefs::GetSingleton().GetSamplerState(stageValue);
			layer.TexUnit = paramNode->Attribute("texUnit")->ValueUInt();

			String samplerValue = paramNode->Attribute("sampler")->ValueString();
			auto samplerIter = mSamplerStates.find(samplerValue) ;

			if (samplerIter == mSamplerStates.end())
				ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Sampler: " + samplerValue + "doesn't exit", "Material::LoadFrom");

			layer.Sampler = samplerIter->second;

			String texFile = paramNode->AttributeString("value", "");
			if (!texFile.empty())
			{
				texFile = parentDir + texFile;

				shared_ptr<TextureResource> textureRes = std::static_pointer_cast<TextureResource>(
					resMan.GetResourceByName(RT_Texture, texFile, mGroup));
				textureRes->Load();

				layer.Texture = textureRes->GetTexture();
			}

			mTextures[parameter->Name] = layer;
		}

		// Material Color
		if (parameter->Usage == EPU_Material_Ambient_Color)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
			{
				std::stringstream sss;
				sss << value;
				sss >> mAmbient.R() >> mAmbient.G() >> mAmbient.B() >> mAmbient.A();
			}			
		}
		else if (parameter->Usage == EPU_Material_Diffuse_Color)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
			{
				std::stringstream sss;
				sss << value;
				sss >> mDiffuse.R() >> mDiffuse.G() >> mDiffuse.B() >> mDiffuse.A();
		
			}
		}
		else if (parameter->Usage == EPU_Material_Specular_Color)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
			{
				std::stringstream sss;
				sss << value;
				sss >> mSpecular.R() >> mSpecular.G() >> mSpecular.B() >> mSpecular.A();
			}
		}
		else if (parameter->Usage == EPU_Material_Emissive_Color)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
			{
				std::stringstream sss;
				sss << value;
				sss >> mEmissive.R() >> mEmissive.G() >> mEmissive.B() >> mEmissive.A();
			}
		}
		else if (parameter->Usage == EPU_Material_Power)
		{
			String value = paramNode->AttributeString("value", "");
			if (!value.empty())
			{
				mPower = LexicalCast<float>(value);
			}
			
		}
			
		mCachedEffectParams.push_back(parameter);
	}
}

void Material::UnloadImpl()
{

}

shared_ptr<Resource> Material::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Material>(creator, handle, name, group);
}

void Material::SetCurrentTechnique( const String& techName )
{
	EffectTechnique* tech = mEffect->GetTechniqueByName(techName);

	if (!tech)
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, techName + " deosn't exit in" + mEffect->GetResourceName(), "Material::SetCurrentTechnique");

	mCurrentTech = tech;
}

void Material::SetCurrentTechnique( uint32_t index )
{
	EffectTechnique* tech = mEffect->GetTechniqueByIndex(index);

	if (!tech)
	{
		String err = mEffect->GetResourceName() + " doesn't have" + std::to_string(index) + " technique";
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, err, "Material::SetCurrentTechnique");
	}

	mCurrentTech = tech;
}






} // Namespace RcEngine