#include "Graphics/Material.h"
#include "Graphics/RenderDevice.h"
#include "Graphics/RenderFactory.h"
#include "Graphics/FrameBuffer.h"
#include "Graphics/Camera.h"
#include "Graphics/Effect.h"
#include "Graphics/EffectParameter.h"
#include "Content/MaterialContentLoader.h"
#include "Core/Context.h"
#include "Core/Exception.h"


namespace RcEngine 
{
	namespace 
	{
		using namespace Render;

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

			~EffectParamsUsageDefs()
			{
				Safe_Delete(msInstance);
			}

			static EffectParamsUsageDefs& GetInstance()
			{
				if (!msInstance)
				{
					msInstance = new EffectParamsUsageDefs();
				}
				return *msInstance;
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
			static EffectParamsUsageDefs* msInstance;
		};
		EffectParamsUsageDefs* EffectParamsUsageDefs::msInstance;

	}


	namespace Render
	{
		using Core::Exception;

		Material::Material(void)
		{
		}


		Material::~Material(void)
		{
			for(size_t i = 0; i < mCachedEffectParams.size(); ++i)
			{
				Safe_Delete(mCachedEffectParams[i]);
			}
		}

		void Material::ApplyMaterial()
		{
			Camera* camera = Core::Context::GetSingleton().GetRenderDevice().GetCurrentFrameBuffer()->GetCamera();

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

		MaterialParameter* Material::GetCustomParameter( EffectParameterUsage usage )
		{
			for (size_t i = 0; i < mCachedEffectParams.size(); ++i)
			{
				if (mCachedEffectParams[i]->Usage == usage)
				{
					return mCachedEffectParams[i];
				}
			}

			return nullptr;
		}

		MaterialParameter* Material::GetCustomParameter( const String& name )
		{
			for (size_t i = 0; i < mCachedEffectParams.size(); ++i)
			{
				if (mCachedEffectParams[i]->Name == name)
				{
					return mCachedEffectParams[i];
				}
			}

			return nullptr;
		}



		shared_ptr<Material> Material::LoadFrom( Stream& source )
		{
			RenderFactory&  factory = Core::Context::GetSingleton().GetRenderFactory();
			shared_ptr<Material> material = std::make_shared<Material>();

			XMLDoc doc;
			XMLNodePtr root = doc.Parse(source);

			// effect first
			XMLNodePtr effectNode = root->FirstNode("Effect");
			String effecName =  effectNode->AttributeString("name", "");
			String effectPath = effectNode->AttributeString("file", "");		

			material->mEffect = factory.CreateEffectFromFile(effecName, effectPath);
			material->mCurrentTechnique = material->mEffect->GetTechniqueByIndex(0);

			for (XMLNodePtr paramNode = root->FirstNode(); paramNode; paramNode = paramNode->NextSibling())
			{
				if (paramNode->NodeName() == "Parameter")
				{
					MaterialParameter* parameter = new MaterialParameter;
					parameter->Name = paramNode->AttributeString("name", "");
					parameter->EffectParam = material->mEffect->GetParameterByName(parameter->Name);
					parameter->Type = parameter->EffectParam->GetParameterType();
					String semantic =  paramNode->AttributeString("semantic", "");
					parameter->IsSemantic = semantic.length() > 0;

					if (parameter->IsSemantic)
					{
						parameter->Usage = EffectParamsUsageDefs::GetInstance().GetUsageType(semantic);
					}else
					{
						parameter->Usage = EPU_Unknown;
					}

					String value = paramNode->AttributeString("value", "");		

					if (parameter->Type == EPT_Sampler || parameter->Type == EPT_Texture2D ||
						parameter->Type == EPT_TextureCUBE || parameter->Type == EPT_Texture3D )
					{	
						if (!value.empty())
						{
							material->mTextures[parameter->Name] = factory.CreateTextureFromFile(value, 0);
						}
					}	
					material->mCachedEffectParams.push_back(parameter);
				}
			}

			return material;
		}

	}
}
