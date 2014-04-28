#include <Graphics/Effect.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/GraphicsResource.h>
#include <Resource/ResourceManager.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <Core/Environment.h>
#include <Core/XMLDom.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>

namespace {

using namespace RcEngine;
//
//struct ShaderStage
//{
//	String Name;
//	String Entry;
//	String Stage;
//	String Source;
//	vector<String> Includes;
//};


bool StringToBool(const String& str)
{
	if (str == "true")
		return true;
	return false;
}

uint8_t ColorWriteMask(bool r, bool g, bool b, bool a)
{
	uint8_t mask = 0;
	if (r)	mask |= CWM_Red;
	if (g)	mask |= CWM_Green;
	if (b)	mask |= CWM_Blue;
	if (a)	mask |= CWM_Alpha;
	return mask;
}

class StateDescDefs
{
private:
	StateDescDefs()
	{
		mDefs.insert(std::make_pair("Never", CF_Never));
		mDefs.insert(std::make_pair("Always",  CF_Always));
		mDefs.insert(std::make_pair("Less", CF_Less));
		mDefs.insert(std::make_pair("LessEqual", CF_LessEqual));
		mDefs.insert(std::make_pair("Equal", CF_Equal));
		mDefs.insert(std::make_pair("NotEqual", CF_NotEqual));
		mDefs.insert(std::make_pair("GreaterEqual", CF_GreaterEqual));
		mDefs.insert(std::make_pair("Greater", CF_Greater));

		mDefs.insert(std::make_pair("Keep", SOP_Keep));
		mDefs.insert(std::make_pair("Zero",  SOP_Zero));
		mDefs.insert(std::make_pair("Replace", SOP_Replace));
		mDefs.insert(std::make_pair("Incr", SOP_Incr));
		mDefs.insert(std::make_pair("Decr", SOP_Decr));
		mDefs.insert(std::make_pair("Invert", SOP_Invert));
		mDefs.insert(std::make_pair("IncrWrap", SOP_Incr_Wrap));
		mDefs.insert(std::make_pair("DecrWrap", SOP_Decr_Wrap));

		mDefs.insert(std::make_pair("Solid", FM_Solid));
		mDefs.insert(std::make_pair("WireFrame",  FM_WireFrame));

		mDefs.insert(std::make_pair("None", CM_None));
		mDefs.insert(std::make_pair("Front", CM_Front));
		mDefs.insert(std::make_pair("Back", CM_Back));

		mDefs.insert(std::make_pair("Add", BOP_Add));
		mDefs.insert(std::make_pair("Sub", BOP_Sub));
		mDefs.insert(std::make_pair("RevSub", BOP_Rev_Sub));
		mDefs.insert(std::make_pair("Max", BOP_Max));
		mDefs.insert(std::make_pair("Min", BOP_Min));

		//mDefs.insert(std::make_pair("Zero",  ABF_Zero));
		mDefs.insert(std::make_pair("One", ABF_One));
		mDefs.insert(std::make_pair("SrcAlpha", ABF_Src_Alpha));
		mDefs.insert(std::make_pair("InvSrcAlpha", ABF_Inv_Src_Alpha));
		mDefs.insert(std::make_pair("DstAlpha", ABF_Dst_Alpha));
		mDefs.insert(std::make_pair("InvDestAlpha", ABF_Inv_Dst_Alpha));
		mDefs.insert(std::make_pair("SrcColor", ABF_Src_Color));
		mDefs.insert(std::make_pair("DestColor", ABF_Dst_Color));
		mDefs.insert(std::make_pair("InvSrcColor", ABF_Inv_Src_Color));
		mDefs.insert(std::make_pair("InvDestColor", ABF_Inv_Dst_Color));
		mDefs.insert(std::make_pair("SrcAlphaSat", ABF_Src_Alpha_Sat));
	}

public:
	static StateDescDefs& GetInstance()
	{
		static StateDescDefs singleton;
		return singleton;
	}

	uint32_t GetState(const String& str)
	{
		auto iter = mDefs.find(str);
		if (iter != mDefs.end())
		{
			return iter->second;
		}

		return CF_Always;
	}

private:
	unordered_map<String, uint32_t> mDefs;
};
//
void CollectRenderStates(XMLNodePtr passNode, DepthStencilStateDesc& dsDesc, BlendStateDesc& blendDesc, RasterizerStateDesc& rasDesc,
	ColorRGBA& blendFactor, uint32_t& sampleMask, uint16_t& frontStencilRef, uint16_t& backStencilRef)
{
	XMLNodePtr stateNode;
	for (stateNode = passNode->FirstNode("State");  stateNode; stateNode = stateNode->NextSibling("State"))
	{
		String name = stateNode->AttributeString("name", "");
		String value = stateNode->AttributeString("value", "");

		if (name == "DepthEnable")
		{
			dsDesc.DepthEnable = StringToBool(value);
		}
		else if (name == "DepthWriteMask")
		{
			dsDesc.DepthWriteMask = StringToBool(value);
		}
		else if (name == "DepthFunc")
		{
			dsDesc.DepthFunc = (CompareFunction)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "StencilEnable")
		{
			dsDesc.StencilEnable = StringToBool(value);
		}
		else if (name == "StencilReadMask")
		{
			dsDesc.StencilReadMask = LexicalCast<uint8_t>(value);
		}
		else if (name == "StencilWriteMask")
		{
			dsDesc.StencilWriteMask = LexicalCast<uint8_t>(value);
		}
		else if (name == "FrontStencilRef")
		{
			frontStencilRef = LexicalCast<uint16_t>(value);
		}
		else if (name == "BackStencilRef")
		{
			backStencilRef = LexicalCast<uint16_t>(value);
		}
		else if (name == "FrontStencilFailOp")
		{
			dsDesc.FrontStencilFailOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "FrontStencilPassOp")
		{
			dsDesc.FrontStencilPassOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "FrontStencilDepthFailOp")
		{
			dsDesc.FrontStencilDepthFailOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "FrontStencilFunc")
		{
			dsDesc.FrontStencilFunc = (CompareFunction)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BackStencilFailOp")
		{
			dsDesc.BackStencilFailOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BackStencilPassOp")
		{
			dsDesc.BackStencilPassOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BackStencilDepthFailOp")
		{
			dsDesc.BackStencilDepthFailOp = (StencilOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BackStencilFunc")
		{
			dsDesc.BackStencilFunc = (CompareFunction)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "AlphaToCoverageEnable")
		{
			blendDesc.AlphaToCoverageEnable = StringToBool(value);
		}
		else if (name == "IndependentBlendEnable")
		{
			blendDesc.IndependentBlendEnable  = StringToBool(value);
		}
		else if (name == "BlendEnable")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].BlendEnable = StringToBool(value);
		}
		else if (name == "SrcBlend")
		{	
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].SrcBlend = (AlphaBlendFactor)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "DestBlend")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].DestBlend = (AlphaBlendFactor)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BlendOp")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].BlendOp = (BlendOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "SrcBlendAlpha")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].SrcBlendAlpha = (AlphaBlendFactor)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "DestBlendAlpha")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].DestBlendAlpha = (AlphaBlendFactor)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "BlendOpAlpha")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			blendDesc.RenderTarget[index].BlendOpAlpha = (BlendOperation)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "ColorWriteMask")
		{
			uint32_t index = stateNode->AttributeUInt("index", 0);
			bool rEnable = StringToBool( stateNode->AttributeString("r", "true") );
			bool gEnable = StringToBool( stateNode->AttributeString("g", "true") );
			bool bEnable = StringToBool( stateNode->AttributeString("b", "true") );
			bool aEnable = StringToBool( stateNode->AttributeString("a", "true") );

			blendDesc.RenderTarget[index].ColorWriteMask = ColorWriteMask(rEnable, gEnable, bEnable, aEnable);
		}
		else if (name == "BlendFactor")
		{
			float r = stateNode->AttributeFloat("r", 0);
			float g = stateNode->AttributeFloat("g", 0);
			float b = stateNode->AttributeFloat("b", 0);
			float a = stateNode->AttributeFloat("a", 0);
			blendFactor = ColorRGBA(r,g,b,a);
		}
		else if (name == "SampleMask")
		{
			sampleMask = LexicalCast<uint32_t>(value);
		}
		else if (name == "FillMode")
		{
			rasDesc.PolygonFillMode = (FillMode)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "CullMode")
		{
			rasDesc.PolygonCullMode = (CullMode)StateDescDefs::GetInstance().GetState(value);
		}
		else if (name == "FrontCounterClockwise")
		{
			rasDesc.FrontCounterClockwise = StringToBool(value);
		}
		else if (name == "DepthBias")
		{
			rasDesc.DepthBias = LexicalCast<float>(value);
		}
		else if (name == "SlopeScaledDepthBias")
		{
			rasDesc.SlopeScaledDepthBias = LexicalCast<float>(value);
		}
		else if (name == "DepthClipEnable")
		{
			rasDesc.DepthClipEnable = StringToBool(value);
		}
		else if (name == "ScissorEnable")
		{
			rasDesc.ScissorEnable = StringToBool(value);
		}
		else if (name == "MultisampleEnable")
		{
			rasDesc.MultisampleEnable = StringToBool(value);
		}
		else
		{
			ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, name + ": Unknown effect state!",
				"CollectRenderStates");
		}
	}
}

//inline ShaderType ParseShaderStage(const String& stage)
//{
//	if (stage == "Vertex")
//		return ST_Vertex;
//	else if (stage == "Hull")
//		return ST_Hull;
//	else if (stage == "Domain")
//		return ST_Domain;
//	else if (stage == "Geometry")
//		return ST_Geomerty;
//	else if (stage == "Pixel")
//		return ST_Pixel;
//	else if (stage == "Compute")
//		return ST_Compute;
//}
//
//shared_ptr<Shader> CompileShader(const ShaderStage& shaderStage, const vector<String>& defines, const vector<String>& values)
//{
//	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
//
//	ShaderType shaderType = ParseShaderStage(shaderStage.Stage);
//
//
//	shared_ptr<Shader> retVal;
//
//	
//
//	
//
//
//
//
//
//
//
//	retVal = factory.CreateShader(shaderType);
//
//	//add include 
//	for (size_t i = 0; i < shaderStage.Includes.size(); ++i)
//	{
//		retVal->AddInclude(shaderStage.Includes[i]);
//	}
//
//	// add macro
//	for (size_t i = 0; i < defines.size(); ++i)
//	{
//		retVal->AddDefine(defines[i], values[i]);
//	}
//
//	
//	if( !retVal->Compile(shaderStage.Source, shaderStage.Entry))
//	{
//		std::cout << "shader:" + shaderStage.Name << std::endl;
//		std::cout << retVal->GetCompileInfo() << std::endl;
//
//		retVal->DumpSource(String("E:/" + shaderStage.Name).c_str());
//
//		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "shader:" + shaderStage.Name  + " compile error!",
//			"Effect::LoadForm");		
//	}
//
//
//	return retVal;
//}
//
void CollectShaderMacro(const XMLNodePtr& node, std::vector<ShaderMacro>& shaderMacros)
{
	for (XMLNodePtr macroNode = node->FirstNode("Macro"); macroNode; macroNode = macroNode->NextSibling("Macro"))
	{
		ShaderMacro macro;
		macro.Name = macroNode->Attribute("name")->ValueString();	
		macro.Definition = macroNode->AttributeString("value", "");
		shaderMacros.push_back(macro);
	}
}

}


namespace RcEngine {

Effect::Effect( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Effect, creator, handle, name, group), mCurrTechnique(nullptr)
{

}

Effect::~Effect()
{
	for (auto iter = mParameters.begin(); iter != mParameters.end(); ++iter)
		delete (iter->second);

	for (auto iter = mTechniques.begin(); iter != mTechniques.end(); ++iter)
		delete *iter;

	for (auto iter = mConstantBuffers.begin(); iter != mConstantBuffers.end(); ++iter)
		delete *iter;
}

EffectTechnique* Effect::GetTechniqueByIndex( uint32_t index ) const
{
	assert(index >=0 && index < mTechniques.size());
	return mTechniques[index];
}

EffectTechnique* Effect::GetTechniqueByName( const String& techName ) const
{
	for (EffectTechnique* technique : mTechniques)
	{
		if (technique->mName == techName)
			return technique;
	}

	return nullptr;
}

void Effect::SetCurrentTechnique( const String& techName )
{
	std::vector<EffectTechnique*>::const_iterator it;

	it = std::find_if(mTechniques.begin(), mTechniques.end(), [&](EffectTechnique* tech) { 
						return tech->GetTechniqueName() == techName;} );
	
	if (it == mTechniques.end())
		ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, techName + " deosn't exit in" + mEffectName, "Effect::SetCurrentTechnique");

	mCurrTechnique = *it;
}

void Effect::SetCurrentTechnique( uint32_t index )
{
	assert(index < mTechniques.size());
	mCurrTechnique = mTechniques[index];	
}

EffectParameter* Effect::GetParameterByName( const String& paraName ) const
{
	std::map<String, EffectParameter*>::const_iterator it;
	
	it = mParameters.find(paraName);
	return (it != mParameters.end()) ? it->second : nullptr;
}

EffectParameter* Effect::FetchSRVParameter( const String& name, EffectParameterType effectType )
{
	switch (effectType)
	{
	case EPT_Texture1D:
	case EPT_Texture2D:
	case EPT_Texture3D:
	case EPT_TextureCube:
	case EPT_Texture1DArray:
	case EPT_Texture2DArray:
	case EPT_Texture3DArray:
	case EPT_TextureCubeArray:
	case EPT_TextureBuffer:
	case EPT_StructureBuffer:
		{
			EffectParameter* srvParam;

			if (mParameters.find(name) == mParameters.end())
			{
				srvParam = new EffectSRVParameter(name, effectType);
				mParameters[name] = srvParam;
			}
			else
			{
				srvParam = mParameters[name];
				assert(srvParam->GetParameterType() == effectType);
			}

			return srvParam;
		}
		break;
	default:
		assert(false);
	}

	return NULL;
}

EffectParameter* Effect::FetchUAVParameter( const String& name, EffectParameterType effectType )
{
	switch (effectType)
	{
	case EPT_Texture1D:
	case EPT_Texture2D:
	case EPT_Texture3D:
	case EPT_TextureCube:
	case EPT_Texture1DArray:
	case EPT_Texture2DArray:
	case EPT_Texture3DArray:
	case EPT_TextureCubeArray:
	case EPT_TextureBuffer:
	case EPT_StructureBuffer:
		{
			EffectParameter* uavParam;

			if (mParameters.find(name) == mParameters.end())
			{
				uavParam = new EffectUAVParameter(name, effectType);
				mParameters[name] = uavParam;
			}
			else
			{
				uavParam = mParameters[name];
				assert(uavParam->GetParameterType() == effectType);
			}

			return uavParam;
		}
		break;
	default:
		assert(false);
	}

	return NULL;
}

EffectParameter* Effect::FetchUniformParameter( const String& name, EffectParameterType type, uint32_t elementSize )
{
	std::map<String, EffectParameter*>::const_iterator it = mParameters.find(name);
	
	EffectParameter* uniformParam;

	if (it != mParameters.end())
	{
		uniformParam = it->second;

		assert(uniformParam->GetParameterType() == type);
		assert(uniformParam->GetElementSize() == elementSize);

		return uniformParam;
	}

	// Not exit, create one
	switch(type)
	{
	case EPT_Boolean:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterBoolArray(name, type, elementSize);
			else 
				uniformParam = new EffectParameterBool(name, type);
		}
		break;
	case EPT_Float:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterFloatArray(name, type, elementSize);
			else
				uniformParam = new EffectParameterFloat(name, type);
		}
		break;
	case EPT_Float2:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterVector2Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterVector2(name, type);
		}
		break;
	case EPT_Float3:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterVector3Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterVector3(name, type);
		}
		break;
	case EPT_Float4:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterVector4Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterVector4(name, type);
		}
		break;
	case EPT_Int:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterIntArray(name, type, elementSize);
			else
				uniformParam = new EffectParameterInt(name, type);
		}
		break;
	case EPT_Int2:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterInt2Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterInt2(name, type);
		}
		break;
	case EPT_Int3:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterInt3Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterInt3(name, type);
		}
		break;
	case EPT_Int4:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterInt4Array(name, type, elementSize);
			else
				uniformParam = new EffectParameterInt4(name, type);
		}
		break;
	case EPT_Matrix4x4:
		{
			if (elementSize > 1)
				uniformParam = new EffectParameterMatrixArray(name, type, elementSize);
			else
				uniformParam = new EffectParameterMatrix(name, type);
		}
		break;
	default:
		assert(false);
	}

	mParameters[name] = uniformParam;

	return uniformParam;
}

EffectParameter* Effect::FetchSamplerParameter( const String& name )
{
	EffectParameter* samplerParam;

	if (mParameters.find(name) == mParameters.end())
	{
		samplerParam = new EffectSamplerParameter(name);
		mParameters[name] = samplerParam;
	}
	else
	{
		samplerParam = mParameters[name];
	}

	return samplerParam;
}

EffectConstantBuffer* Effect::FetchConstantBuffer( const String& name, uint32_t bufferSize )
{
	for (EffectConstantBuffer* buffer : mConstantBuffers)
	{
		if (buffer->GetName() == name && buffer->GetBufferSize() == bufferSize)
			return buffer;
	}

	EffectConstantBuffer* buffer = new EffectConstantBuffer(name, bufferSize);
	mConstantBuffers.push_back(buffer);

	return buffer;
}

EffectConstantBuffer* Effect::CreateConstantBuffer( const String& name, uint32_t bufferSize )
{
	EffectConstantBuffer* buffer = new EffectConstantBuffer(name, bufferSize);
	mConstantBuffers.push_back(buffer);

	return buffer;
}

void Effect::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory* factory = Environment::GetSingleton().GetRenderFactory();

	// effect flags used to build shader macro
	vector<String> effectFlags;

	//split the effect name to get effect file and flags
	StringStream iss(mResourceName); 
	do 
	{ 
		String sub; 
		iss >> sub; 
		if (!sub.empty())
			effectFlags.push_back(sub);	
	} while (iss); 

	shared_ptr<Stream> effectStream = fileSystem.OpenStream(effectFlags[0], mGroup);
	Stream& source = *effectStream;	

	XMLDoc doc;
	XMLNodePtr root = doc.Parse(source);

	// effect name 
	mEffectName = root->AttributeString("name", "");

	XMLNodePtr technqueNode, passNode, shaderNode;
	for (technqueNode = root->FirstNode("Technique");  technqueNode; technqueNode = technqueNode->NextSibling("Technique"))
	{
		EffectTechnique* technique = new EffectTechnique(*this);
	
		technique->mName = technqueNode->AttributeString("name", "");
	
		String filename, entryPoint;
		for (passNode = technqueNode->FirstNode("Pass");  passNode; passNode = passNode->NextSibling("Pass"))
		{
			EffectPass* pass = new EffectPass;
			pass->mName = passNode->AttributeString("name", "");
	
			DepthStencilStateDesc dsDesc;
			BlendStateDesc blendDesc;
			RasterizerStateDesc rasDesc;
	
			CollectRenderStates(passNode, dsDesc, blendDesc, rasDesc, pass->mBlendColor, pass->mSampleMask, pass->mFrontStencilRef, pass->mBackStencilRef);
	
			pass->mDepthStencilState = factory->CreateDepthStencilState(dsDesc);
			pass->mBlendState = factory->CreateBlendState(blendDesc);
			pass->mRasterizerState = factory->CreateRasterizerState(rasDesc);
			pass->mShaderPipeline = factory->CreateShaderPipeline(*this);

			static const String ShaderNodeNames[] = {"VertexShader", "HullShader", "DomainShader", "GeometryShader", "PixelShader", "ComputeShader"};
			for (uint32_t i = 0; i < ST_Count; ++i)
			{
				if (shaderNode = passNode->FirstNode(ShaderNodeNames[i]))
				{
					filename = shaderNode->AttributeString("file", "");
					entryPoint = shaderNode->AttributeString("entry", "");

					vector<ShaderMacro> shaderMacros;
					CollectShaderMacro(shaderNode, shaderMacros);

					for (size_t j = 1; j < effectFlags.size(); ++j)
					{
						ShaderMacro macro = { effectFlags[j], "" };
						shaderMacros.push_back(macro);
					}

					pass->mShaderPipeline->AttachShader(
						factory->LoadShaderFromFile(
						ShaderType(ST_Vertex + i),
						filename, 
						shaderMacros.empty() ? nullptr : &shaderMacros[0],
						shaderMacros.size(),
						entryPoint) );
				}
			}

			if (pass->mShaderPipeline->LinkPipeline() == false)
			{
				ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Effect error!", "Effect::LoadForm");
			}
			
			technique->mPasses.push_back(pass);	
		}

		mTechniques.push_back(technique);
	}
	
	mCurrTechnique = mTechniques.front();
}


//void Effect::LoadImpl()
//{
//	FileSystem& fileSystem = FileSystem::GetSingleton();
//	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
//	
//	vector<String> effectFlags;
//	
//	//split the effect name to get effect file and flags
//	std::istringstream iss(mResourceName); 
//	do 
//	{ 
//		String sub; 
//		iss >> sub; 
//		if (!sub.empty())
//			effectFlags.push_back(sub);	
//	} while (iss); 
//
//	shared_ptr<Stream> effectStream = fileSystem.OpenStream(effectFlags[0], mGroup);
//	Stream& source = *effectStream;	
//
//	XMLDoc doc;
//	XMLNodePtr root = doc.Parse(source);
//
//	// effect name 
//	mEffectName = root->AttributeString("name", "");
//
//	// store all shader this effect will use
//	unordered_map<String, ShaderStage> shaderStages;
//
//	XMLNodePtr shaderNode; 
//	for (shaderNode = root->FirstNode("Shader");  shaderNode; shaderNode = shaderNode->NextSibling("Shader"))
//	{
//		String name = shaderNode->AttributeString("name", "");
//
//		ShaderStage& shaderStage = shaderStages[name];
//
//		shaderStage.Name = name;
//		shaderStage.Stage = shaderNode->AttributeString("stage", "");	
//		shaderStage.Entry = shaderNode->AttributeString("entry", "");
//
//		// Parse shader include headers
//		vector<String> includes;
//		for (XMLNodePtr includeNode = shaderNode->FirstNode("Include");  includeNode; includeNode = includeNode->NextSibling("Include"))
//		{
//			String includeName = includeNode->Attribute("name")->ValueString();
//
//			shared_ptr<Stream> streamPtr = FileSystem::GetSingleton().OpenStream(includeName, mGroup);
//			Stream& includeStream = *streamPtr;
//
//			XMLDoc includeDoc;
//			XMLNodePtr includeSourceNode = includeDoc.Parse(includeStream)->FirstNode("ShaderSource")->FirstNode();
//
//			if (includeSourceNode->NodeType() != XML_Node_CData)
//				ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Effect shader include source(CDATA) does't exit", "Effect::LoadImpl");
//
//			
//			String includeSource = includeSourceNode->ValueString(); 
//			shaderStage.Includes.push_back(includeSource);
//		}
//
//		// Parse shader source
//		XMLNodePtr shaderSourceNode = shaderNode->FirstNode("ShaderSource")->FirstNode();
//		if (shaderSourceNode->NodeType() == XML_Node_CData)
//			shaderStage.Source =  shaderSourceNode->ValueString();	
//		else
//			ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Effect shader include source(CDATA) does't exit", "Effect::LoadImpl");	
//	}
//
//	// Parse effect technique
//	XMLNodePtr technqueNode;
//	for (technqueNode = root->FirstNode("Technique");  technqueNode; technqueNode = technqueNode->NextSibling("Technique"))
//	{
//		EffectTechnique* technique = new EffectTechnique(*this);
//
//		technique->mName = technqueNode->AttributeString("name", "");
//
//		XMLNodePtr passNode;
//		for (passNode = technqueNode->FirstNode("Pass");  passNode; passNode = passNode->NextSibling("Pass"))
//		{
//			EffectPass* pass = new EffectPass;
//			pass->mName = passNode->AttributeString("name", "");
//
//			DepthStencilStateDesc dsDesc;
//			BlendStateDesc blendDesc;
//			RasterizerStateDesc rasDesc;
//
//			CollectRenderStates(passNode, dsDesc, blendDesc, rasDesc,  
//				pass->mBlendColor, pass->mSampleMask, pass->mFrontStencilRef, pass->mBackStencilRef);
//
//			pass->mDepthStencilState = factory.CreateDepthStencilState(dsDesc);
//			pass->mBlendState = factory.CreateBlendState(blendDesc);
//			pass->mRasterizerState = factory.CreateRasterizerState(rasDesc);
//
//			shared_ptr<ShaderProgram> program = factory.CreateShaderProgram(*this);
//
//			// Vertex shader
//			XMLNodePtr vertexNode = passNode->FirstNode("VertexShader");
//			String vertexName = vertexNode->AttributeString("name", "");
//			if (shaderStages.find(vertexName) != shaderStages.end())
//			{
//				vector<ShaderMacro> shaderMacros;
//				CollectShaderMacro(vertexNode, shaderMacros);
//
//				for (size_t i = 1; i < effectFlags.size(); ++i)
//				{
//					shaderMacros.push_back(ShaderMacro(effectFlags[i], ""));
//				}
//
//				shared_ptr<Shader> vertexShader = factory.CreateShader(ST_Vertex,  )
//
//				//program->AttachShader(CompileShader(shaderStages[vertexName], defines, values));
//			}
//			else
//			{
//				String err = "VS( " + vertexName + " ) not found!";
//				ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, err, "Effect::Load");
//			}
//
//			// Geometry shader (optional) 
//			XMLNodePtr geometryNode = passNode->FirstNode("GeometryShader");
//			if (geometryNode)
//			{
//				String geometryName = geometryNode->AttributeString("name", "");
//				if (shaderStages.find(geometryName) != shaderStages.end())
//				{
//					vector<String> defines, values;
//					CollectShaderMacro(geometryNode, defines, values);
//					for (size_t i = 1; i < effectFlags.size(); ++i)
//					{
//						defines.push_back(effectFlags[i]);
//						values.push_back(String(""));
//					}
//					program->AttachShader(CompileShader(shaderStages[geometryName], defines, values));
//				}
//				else
//				{
//					String err = "PS( " + geometryName + " ) not found!";
//					ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, err, "Effect::Load");
//				}
//			}
//
//			// Pixel shader (optional) 
//			XMLNodePtr pixelNode = passNode->FirstNode("PixelShader");
//			if (pixelNode)
//			{
//				String pixelName = pixelNode->AttributeString("name", "");
//				if (shaderStages.find(pixelName) != shaderStages.end())
//				{
//					vector<String> defines, values;
//					CollectShaderMacro(pixelNode, defines, values);
//					for (size_t i = 1; i < effectFlags.size(); ++i)
//					{
//						defines.push_back(effectFlags[i]);
//						values.push_back(String(""));
//					}
//					program->AttachShader(CompileShader(shaderStages[pixelName], defines, values));
//				}
//				else
//				{
//					String err = "PS( " + pixelName + " ) not found!";
//					ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, err, "Effect::Load");
//				}
//			}
//			
//			if (!program->LinkProgram())
//			{
//				std::cout << program->GetLinkInfo() << std::endl;
//				ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Effect error!",
//					"Effect::LoadForm");
//			}
//
//
//			pass->mShaderProgram = program;
//			technique->mPasses.push_back(pass);
//		}
//
//		mTechniques.push_back(technique);
//	}
//
//	mCurrTechnique = mTechniques.front();
//}

void Effect::UnloadImpl()
{

}

shared_ptr<Resource> Effect::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Effect>(creator, handle, name, group);
}

//////////////////////////////////////////////////////////////////////////

EffectTechnique::EffectTechnique(Effect& effect)
	: mEffect(effect),
	  mValid(true)
{

}

EffectTechnique::~EffectTechnique()
{
	for (EffectPass* pass : mPasses)
		delete pass;
}

EffectPass* EffectTechnique::GetPassByName( const String& name ) const
{
	for (EffectPass* pass : mPasses)
	{
		if (pass->GetPassName() == name)
			return pass;
	}

	return nullptr;
}

EffectPass* EffectTechnique::GetPassByIndex( uint32_t index ) const
{
	if (index >= 0 && index < mPasses.size())
		return mPasses[index];

	return nullptr;
}


//////////////////////////////////////////////////////////////////////////
EffectPass::EffectPass( )
	: mFrontStencilRef(0),
	  mBackStencilRef(0),
	  mBlendColor(0, 0, 0, 0),
	  mSampleMask(0)
{

}


void EffectPass::BeginPass()
{
	RenderDevice* device = Environment::GetSingleton().GetRenderDevice();

	device->SetDepthStencilState(mDepthStencilState, mFrontStencilRef, mBackStencilRef);
	device->SetBlendState(mBlendState, mBlendColor, mSampleMask);
	device->SetRasterizerState(mRasterizerState);
	device->BindShaderPipeline(mShaderPipeline);
}

void EffectPass::EndPass()
{
	//Environment::GetSingleton().GetRHDevice()->BindShaderPipeline(nullptr);
}


} // Namespace RcEngine