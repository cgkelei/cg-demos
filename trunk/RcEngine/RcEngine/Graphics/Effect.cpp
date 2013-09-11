#include <Graphics/Effect.h>
#include <Graphics/EffectTechnique.h>
#include <Graphics/EffectPass.h>
#include <Graphics/ShaderProgram.h>
#include <Graphics/RenderDevice.h>
#include <Graphics/EffectParameter.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/Shader.h>
#include <Resource/ResourceManager.h>
#include <Core/Context.h>
#include <Core/Exception.h>
#include <Core/Utility.h>
#include <Core/XMLDom.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>

namespace {

using namespace RcEngine;

struct ShaderStage
{
	String Name;
	String Entry;
	String Stage;
	String Source;
	vector<String> Includes;
};

EffectParameter* CreateEffectParameter( const String& name, EffectParameterType type, bool array)
{
	switch(type)
	{
	case EPT_Boolean:
		{
			if (array)
			{
				return new EffectParameterBoolArray(name, type);
			}
			else
			{
				return new EffectParameterBool(name, type);
			}
		}
		break;
	case EPT_Float:
		{
			if (array)
			{
				return new EffectParameterFloatArray(name, type);
			}
			else
			{
				return new EffectParameterFloat(name, type);
			}
		}
		break;
	case EPT_Float2:
		{
			if (array)
			{
				return new EffectParameterVector2Array(name, type);
			}
			else
			{
				return new EffectParameterVector2(name, type);
			}
		}
		break;
	case EPT_Float3:
		{
			if (array)
			{
				return new EffectParameterVector3Array(name, type);
			}
			else
			{
				return new EffectParameterVector3(name, type);
			}
		}
		break;
	case EPT_Float4:
		{
			if (array)
			{
				return new EffectParameterVector4Array(name, type);
			}
			else
			{
				return new EffectParameterVector4(name, type);
			}
		}
		break;
	case EPT_Int:
		{
			if (array)
			{
				return new EffectParameterIntArray(name, type);
			}
			else
			{
				return new EffectParameterInt(name, type);
			}
		}
		break;
	case EPT_Int2:
		{
			if (array)
			{
				return new EffectParameterInt2Array(name, type);
			}
			else
			{
				return new EffectParameterInt2(name, type);
			}
		}
		break;
	case EPT_Int3:
		{
			if (array)
			{
				return new EffectParameterInt3Array(name, type);
			}
			else
			{
				return new EffectParameterInt3(name, type);
			}
		}
		break;
	case EPT_Int4:
		{
			if (array)
			{
				return new EffectParameterInt4Array(name, type);
			}
			else
			{
				return new EffectParameterInt4(name, type);
			}
		}
		break;
	case EPT_Matrix4x4:
		{
			if (array)
			{
				return new EffectParameterMatrixArray(name, type);
			}
			else
			{
				return new EffectParameterMatrix(name, type);
			}
		}
		break;
	case EPT_Texture1D:
	case EPT_Texture1DArray:
	case EPT_Texture2D:
	case EPT_Texture2DArray:
	case EPT_Texture3D:
	case EPT_Texture3DArray:
	case EPT_TextureCUBE:
	case EPT_TextureCUBEArray:
		{
			return new EffectParameterTexture(name, type);
		}
		break;
	default:
		return nullptr;
	}
}

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
		mDefs.insert(std::make_pair("AlwaysFail", CF_AlwaysFail));
		mDefs.insert(std::make_pair("AlwaysPass",  CF_AlwaysPass));
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

		return CF_AlwaysFail;
	}

private:
	unordered_map<String, uint32_t> mDefs;
};

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
			dsDesc.StencilReadMask = LexicalCast<uint16_t>(value);
		}
		else if (name == "StencilWriteMask")
		{
			dsDesc.StencilWriteMask = LexicalCast<uint16_t>(value);
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

shared_ptr<Shader> CompileShader(const ShaderStage& shaderStage, const vector<String>& defines, const vector<String>& values)
{
	shared_ptr<Shader> retVal;

	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	ShaderType shaderType;
	if (shaderStage.Stage == String("Vertex"))
	{
		shaderType = ST_Vertex;		
	}
	else if (shaderStage.Stage == String("Pixel"))
	{
		shaderType = ST_Pixel;
	}
	else if (shaderStage.Stage == String("Geometry"))
	{
		shaderType = ST_Geomerty;
	}

	retVal = factory.CreateShader(shaderType);

	//add include 
	for (size_t i = 0; i < shaderStage.Includes.size(); ++i)
	{
		retVal->AddInclude(shaderStage.Includes[i]);
	}

	// add macro
	for (size_t i = 0; i < defines.size(); ++i)
	{
		retVal->AddDefine(defines[i], values[i]);
	}

	
	if( !retVal->Compile(shaderStage.Source, shaderStage.Entry))
	{
		std::cout << "shader:" + shaderStage.Name << std::endl;
		std::cout << retVal->GetCompileInfo() << std::endl;
		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "shader:" + shaderStage.Name  + " compile error!",
			"Effect::LoadForm");
	}


	return retVal;
}

void CollectShaderMacro(const XMLNodePtr& node, vector<String>& defines, vector<String>& values)
{
	for (XMLNodePtr macroNode = node->FirstNode("Macro"); macroNode; macroNode = macroNode->NextSibling("Macro"))
	{
		String name = macroNode->Attribute("name")->ValueString();	
		String value = macroNode->AttributeString("value", "");
		defines.push_back(name);
		values.push_back(value);
	}

	RenderDeviceType rdType = Context::GetSingleton().GetRenderDevice().GetRenderDeviceType();
	
	// Deal with OpenGL and Direct3D texcoord Y reverse
	if (rdType == RD_Direct3D11)
	{
		defines.push_back("Direct3D");
		values.push_back("");
	}
}

}


namespace RcEngine {

Effect::Effect( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Effect, creator, handle, name, group)
{

}

Effect::~Effect()
{
	for (auto iter = mParameters.begin(); iter != mParameters.end(); ++iter)
	{
		delete (iter->second);
	}
	mParameters.clear();

	for (auto iter = mTechniques.begin(); iter != mTechniques.end(); ++iter)
	{
		delete *iter;
	}
	mTechniques.clear();
}

EffectTechnique* Effect::GetTechniqueByName(const String& techName)
{
	auto found = std::find_if(mTechniques.begin(), mTechniques.end(), 
		[&techName](EffectTechnique* param) { return param->GetTechniqueName() == techName; });

	if (found != mTechniques.end())
	{
		return *found;
	}

	return nullptr;
}

EffectTechnique* Effect::GetTechniqueByIndex(uint32_t index)
{
	assert(index >=0 && index < mTechniques.size());
	return mTechniques[index];
}

EffectParameter* Effect::GetParameterByName( const String& paraName ) const
{
	auto found = mParameters.find(paraName);

	if (found != mParameters.end())
	{
		return found->second;
	}

	return nullptr;
}

EffectParameter* Effect::AddOrGetShaderParameter(const String& name, EffectParameterType type, bool array)
{
	EffectParameter* effectParam;
	auto found = mParameters.find(name);
	if (found != mParameters.end())
	{
		effectParam = found->second;
	}
	else
	{
		effectParam = CreateEffectParameter(name, type, array);
		mParameters.insert(std::make_pair(name, effectParam));
	}
	return effectParam;
}


shared_ptr<Resource> Effect::Clone()
{
	printf("Clone effect: %s\n", mEffectName.c_str());

	shared_ptr<Effect> retVal = std::make_shared<Effect>(mCreator, mResourceHandle, mName, mGroup);
	
	retVal->mEffectName = mEffectName;

	// clone paremeters
	for (auto iter = mParameters.begin(); iter != mParameters.end(); ++iter)
	{
		retVal->mParameters[iter->first] = iter->second->Clone();
	}	

	// clone technique
	retVal->mTechniques.resize(mTechniques.size());
	for (size_t i = 0; i < mTechniques.size(); ++i)
	{
		retVal->mTechniques[i] = mTechniques[i]->Clone(*retVal);
	}

	retVal->SetLoadState(Resource::Loaded);

	return retVal;
}

void Effect::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	
	vector<String> effectFlags;
	
	//split the effect name to get effect file and flags
	std::istringstream iss(mName); 
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

	// store all shader this effect will use
	unordered_map<String, ShaderStage> shaderStages;

	XMLNodePtr shaderNode; 
	for (shaderNode = root->FirstNode("Shader");  shaderNode; shaderNode = shaderNode->NextSibling("Shader"))
	{
		String name = shaderNode->AttributeString("name", "");

		ShaderStage& shaderStage = shaderStages[name];

		shaderStage.Name = name;
		shaderStage.Stage = shaderNode->AttributeString("stage", "");	
		shaderStage.Entry = shaderNode->AttributeString("entry", "");

		// Parse shader include headers
		vector<String> includes;
		for (XMLNodePtr includeNode = shaderNode->FirstNode("Include");  includeNode; includeNode = includeNode->NextSibling("Include"))
		{
			String includeName = includeNode->Attribute("name")->ValueString();

			shared_ptr<Stream> streamPtr = FileSystem::GetSingleton().OpenStream(includeName, mGroup);
			Stream& includeStream = *streamPtr;

			XMLDoc includeDoc;
			XMLNodePtr includeSourceNode = includeDoc.Parse(includeStream)->FirstNode("ShaderSource")->FirstNode();

			if (includeSourceNode->NodeType() != XML_Node_CData)
				ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Effect shader include source(CDATA) does't exit", "Effect::LoadImpl");

			
			String includeSource = includeSourceNode->ValueString(); 
			shaderStage.Includes.push_back(includeSource);
		}

		// Parse shader source
		XMLNodePtr shaderSourceNode = shaderNode->FirstNode("ShaderSource")->FirstNode();
		if (shaderSourceNode->NodeType() == XML_Node_CData)
			shaderStage.Source =  shaderSourceNode->ValueString();	
		else
			ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, "Effect shader include source(CDATA) does't exit", "Effect::LoadImpl");	
	}

	// Parse effect technique
	XMLNodePtr technqueNode;
	for (technqueNode = root->FirstNode("Technique");  technqueNode; technqueNode = technqueNode->NextSibling("Technique"))
	{
		EffectTechnique* technique = new EffectTechnique;

		technique->mName = technqueNode->AttributeString("name", "");

		XMLNodePtr passNode;
		for (passNode = technqueNode->FirstNode("Pass");  passNode; passNode = passNode->NextSibling("Pass"))
		{
			EffectPass* pass = new EffectPass;
			pass->mName = passNode->AttributeString("name", "");

			DepthStencilStateDesc dsDesc;
			BlendStateDesc blendDesc;
			RasterizerStateDesc rasDesc;

			CollectRenderStates(passNode, dsDesc, blendDesc, rasDesc,  
				pass->mBlendColor, pass->mSampleMask, pass->mFrontStencilRef, pass->mBackStencilRef);

			pass->mDepthStencilState = factory.CreateDepthStencilState(dsDesc);
			pass->mBlendState = factory.CreateBlendState(blendDesc);
			pass->mRasterizerState = factory.CreateRasterizerState(rasDesc);

			shared_ptr<ShaderProgram> program = factory.CreateShaderProgram(*this);

			XMLNodePtr vertexNode = passNode->FirstNode("VertexShader");
			String vertexName = vertexNode->AttributeString("name", "");
			if (shaderStages.find(vertexName) != shaderStages.end())
			{
				vector<String> defines, values;
				CollectShaderMacro(vertexNode, defines, values);

				for (size_t i = 1; i < effectFlags.size(); ++i)
				{
					defines.push_back(effectFlags[i]);
					values.push_back(String(""));
				}
				program->AttachShader(CompileShader(shaderStages[vertexName], defines, values));
			}
			else
			{
				String err = "VS( " + vertexName + " ) not found!";
				ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, err, "Effect::Load");
			}

			XMLNodePtr pixelNode = passNode->FirstNode("PixelShader");
			String pixelName = pixelNode->AttributeString("name", "");
			if (shaderStages.find(pixelName) != shaderStages.end())
			{
				vector<String> defines, values;
				CollectShaderMacro(pixelNode, defines, values);
				for (size_t i = 1; i < effectFlags.size(); ++i)
				{
					defines.push_back(effectFlags[i]);
					values.push_back(String(""));
				}
				program->AttachShader(CompileShader(shaderStages[pixelName], defines, values));
			}
			else
			{
				String err = "PS( " + pixelName + " ) not found!";
				ENGINE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND, err, "Effect::Load");
			}

			if (!program->LinkProgram())
			{
				std::cout << program->GetLinkInfo() << std::endl;
				ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Effect error!",
					"Effect::LoadForm");
			}


			pass->mShaderProgram = program;
			technique->mPasses.push_back(pass);
		}

		mTechniques.push_back(technique);
	}
}

void Effect::UnloadImpl()
{

}

shared_ptr<Resource> Effect::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Effect>(creator,handle, name, group);
}


} // Namespace RcEngine