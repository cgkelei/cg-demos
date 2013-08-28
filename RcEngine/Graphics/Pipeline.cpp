#include <Graphics/Pipeline.h>
#include <Graphics/RenderFactory.h>
#include <Graphics/PixelFormat.h>
#include <Graphics/FrameBuffer.h>
#include <Graphics/RenderView.h>
#include <Graphics/RenderDevice.h>
#include <Resource/ResourceManager.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <Core/Context.h>
#include <Core/XMLDom.h>
#include <Core/Exception.h>

namespace {

using namespace RcEngine;

Attachment GetAttachment(const String& name)
{
	if (name == "Depth")
	{
		return ATT_DepthStencil;
	}
	else if (name == "Color0")
	{
		return ATT_Color0;
	}
	else if (name == "Color1")
	{
		return ATT_Color1;
	}
	else if (name == "Color2")
	{
		return ATT_Color2;
	}
	else if (name == "Color3")
	{
		return ATT_Color3;
	}
	else if (name == "Color4")
	{
		return ATT_Color4;
	}
	else
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_STATE, "Attachment Error.", "GetAttachment");
	}
}

}

namespace RcEngine {

shared_ptr<Resource> Pipeline::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<Pipeline>(creator, handle, name, group);
}

Pipeline::Pipeline(ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(RT_Pipeline, creator, handle, name, group)
{

}

Pipeline::~Pipeline()
{

}

void Pipeline::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();
	ResourceManager& resMan = ResourceManager::GetSingleton();

	shared_ptr<Stream> matStream = fileSystem.OpenStream(mName, mGroup);
	Stream& source = *matStream;	

	XMLDoc doc;
	XMLNodePtr root = doc.Parse(source);

	mPipelineName = root->AttributeString("name", "");


	XMLNodePtr setupNode = root->FirstNode("Setup");
	if (setupNode)
	{
		for (XMLNodePtr frameBufferNode = setupNode->FirstNode("FrameBuffer"); frameBufferNode; frameBufferNode = frameBufferNode->NextSibling("FrameBuffer"))
		{
			FrameBufferDesc fbDesc;

			fbDesc.Name =  frameBufferNode->AttributeString("name", "");
			fbDesc.Width = frameBufferNode->AttributeUInt("width", 0);
			fbDesc.Height = frameBufferNode->AttributeUInt("height", 0);

			for (XMLNodePtr targetNode = frameBufferNode->FirstNode("Target"); targetNode; targetNode = targetNode->NextSibling("Target"))
			{
				RenderTargetDesc rtDesc; 

				String attachment = targetNode->AttributeString("attach", "");
				String targetFormat = targetNode->AttributeString("format", "");	

				rtDesc.Attach = GetAttachment(attachment);
				rtDesc.Format = PixelFormatUtils::GetPixelFormat(targetFormat);
				rtDesc.SampleCount = targetNode->AttributeUInt("sampleCount", 1);
				rtDesc.SampleQuality = targetNode->AttributeUInt("sampleQuality", 0);

				fbDesc.RenderTargets.push_back(rtDesc);	
			}

			mFrameBuffers.push_back(fbDesc);
		}
	}

	CreateFrameBuffers();
}

void Pipeline::UnloadImpl()
{

}

void Pipeline::CreateFrameBuffers()
{
	RenderDevice& device = Context::GetSingleton().GetRenderDevice();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	for (size_t i = 0; i < mFrameBuffers.size(); ++i)
	{
		FrameBufferDesc& fbDesc = mFrameBuffers[i];

		fbDesc.FrameBuffer = factory.CreateFrameBuffer(fbDesc.Width, fbDesc.Height);
		
		// bind as current, so we can add render target
		device.BindFrameBuffer(fbDesc.FrameBuffer);

		for (size_t j = 0; j < fbDesc.RenderTargets.size(); ++j)
		{
			RenderTargetDesc& rtDesc = fbDesc.RenderTargets[j];

			shared_ptr<RenderView> view;

			if (rtDesc.Attach == ATT_DepthStencil)
			{
				rtDesc.RenderTexture = factory.CreateTexture2D(fbDesc.Width, fbDesc.Height, rtDesc.Format, 1, 0, rtDesc.SampleCount, rtDesc.SampleQuality, 0, nullptr);
				view = factory.CreateDepthStencilView(rtDesc.RenderTexture, 0, 0);
			}
			else
			{
				rtDesc.RenderTexture = factory.CreateTexture2D(fbDesc.Width, fbDesc.Height, rtDesc.Format, 1, 0, rtDesc.SampleCount, rtDesc.SampleQuality, 0, nullptr);
				view = factory.CreateRenderTargetView2D(rtDesc.RenderTexture, 0, 0);
			}

			fbDesc.FrameBuffer->Attach(rtDesc.Attach, view);
		}
	}
	
	// bind default screen buffer
	device.BindFrameBuffer(device.GetScreenFrameBuffer());
}

shared_ptr<FrameBuffer> Pipeline::GetFrameBuffer( const String& name )
{
	for (size_t i = 0; i < mFrameBuffers.size(); ++i)
	{
		FrameBufferDesc& fbDesc = mFrameBuffers[i];

		if (fbDesc.Name == name)
		{
			return fbDesc.FrameBuffer;
		}
	}

	return nullptr;
}

shared_ptr<Texture> Pipeline::GetRenderTexture( const String& frameName, Attachment attach )
{
	shared_ptr<Texture> retVal = nullptr;

	for (size_t i = 0; i < mFrameBuffers.size(); ++i)
	{
		FrameBufferDesc& fbDesc = mFrameBuffers[i];

		if (fbDesc.Name == frameName)
		{
			for (size_t j = 0; j < fbDesc.RenderTargets.size(); ++j)
			{
				if (fbDesc.RenderTargets[j].Attach == attach)
				{
					retVal = fbDesc.RenderTargets[j].RenderTexture;
				}
			}
		}
	}

	return retVal;
}

}
