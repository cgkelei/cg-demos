#include <Graphics/Texture.h>
#include <Graphics/RenderFactory.h>
#include <Resource/ResourceManager.h>
#include <IO/FileSystem.h>
#include <IO/FileStream.h>
#include <Core/Context.h>

namespace RcEngine {

Texture::Texture( TextureType type, PixelFormat format, uint32_t numMipMaps, uint32_t sampleCount, uint32_t sampleQuality, uint32_t accessHint )
	: mType(type), mFormat(format), mSampleCount(sampleCount), mSampleQuality(sampleQuality), mAccessHint(accessHint)
{

}

Texture::~Texture()
{

}

TextureResource::TextureResource( uint32_t resType, ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
	: Resource(resType, creator, handle, name, group), mTexture(nullptr)
{

}

TextureResource::~TextureResource()
{

}

void TextureResource::LoadImpl()
{
	FileSystem& fileSystem = FileSystem::GetSingleton();
	RenderFactory& factory = Context::GetSingleton().GetRenderFactory();

	String fullPath = fileSystem.Locate(mName, mGroup);

	mTexture = factory.CreateTextureFromFile(fullPath);
}

void TextureResource::UnloadImpl()
{

}

shared_ptr<Resource> TextureResource::FactoryFunc( ResourceManager* creator, ResourceHandle handle, const String& name, const String& group )
{
	return std::make_shared<TextureResource>(ResourceTypes::Texture, creator, handle, name, group);
}

void _ApiExport SaveTexture( const shared_ptr<Texture> texture, const String& texName )
{

}



} // Namespace RcEngine