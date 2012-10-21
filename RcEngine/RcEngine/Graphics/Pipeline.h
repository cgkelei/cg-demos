#include <Core/Prerequisites.h>
#include <Resource/Resource.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/GraphicsCommon.h>
#include <Graphics/Texture.h>
#include <Graphics/Material.h>
#include <Math/ColorRGBA.h>

namespace RcEngine {

class _ApiExport PipelineCmdParam
{
public:
	PipelineCmdParam();

	bool GetParamBoolean() const { return mValueBasic.ValueBoolean; }
	float GetParamFloat() const { return mValueBasic.ValueFloat; }
	int32_t GetParamInt() const { return mValueBasic.ValueInt; }
	uint32_t GetParamUInt() const { return mValueBasic.ValueUInt; }
	const String& GetParamString() const { return mValueString; }
	const ColorRGBA& GetParamColor() const { return mValueColor; }
	const shared_ptr<Resource> GetResource() const { return mValueResource; }
	
private:
	union BasicType
	{
		float ValueFloat;
		uint32_t ValueUInt;
		int32_t ValueInt;
		bool ValueBoolean;
	};

	BasicType mValueBasic;
	String mValueString;
	ColorRGBA mValueColor;
	shared_ptr<Resource> mValueResource;
};

class _ApiExport Pipeline : public Resource
{
	friend class Renderer;

public:
	struct RenderTargetDesc
	{
		RenderTargetDesc() : SampleCount(1), SampleQuality(0), RenderTexture(0) {}

		Attachment Attach;
		PixelFormat Format;
		uint32_t SampleCount;
		uint32_t SampleQuality;

		shared_ptr<Texture> RenderTexture;
	};

	struct FrameBufferDesc
	{
		String Name;
		uint32_t Width, Height;
		std::vector<RenderTargetDesc> RenderTargets;
		shared_ptr<FrameBuffer> FrameBuffer;
	};

	enum CommandFlag
	{
		BindFrameBuffer,
		ClearFrameBuffer,
		SwapBuffer,
		DrawGeometry,
		DrawOverlays,
		DrawQuad,
		DoForwardLightLoop,
		DoDeferredLightLoop,
		SetUniform
	};

	struct PipelineCommand
	{
		CommandFlag	Command;
		std::vector< PipelineCmdParam >  params;

		PipelineCommand( CommandFlag command )
		{
			Command = command;
		}
	};

	struct PipelineStage
	{
		String Name;
		shared_ptr<Material> MaterialLink;
		std::vector< PipelineCommand >  Commands;
		bool Enabled;

		PipelineStage() : MaterialLink( 0x0 ) {}
	};

public:
	Pipeline(uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);
	~Pipeline();

	shared_ptr<FrameBuffer> GetFrameBuffer(const String& name);
	shared_ptr<Texture> GetRenderTexture(const String& frameName, Attachment attach);



public:
	static shared_ptr<Resource> FactoryFunc(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group);

protected:
	void LoadImpl();
	void UnloadImpl();

	void CreateFrameBuffers();

private:
	String mPipelineName;
	vector<FrameBufferDesc> mFrameBuffers;
	vector<PipelineStage> mPipelineStages;
};




}