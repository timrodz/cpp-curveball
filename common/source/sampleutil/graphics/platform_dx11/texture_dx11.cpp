/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include <sampleutil/graphics.h>
#include "buffer_internal_dx11.h"
#include "loader_internal_dx11.h"
#include "constant_internal_dx11.h"
#include <gxt_conversion.h>

#include "../image.h"
namespace ssg  = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace util = sce::SampleUtil::Internal::Util;

//-------------------------------------------------------------------------------------------------------
// Sampler state
//-------------------------------------------------------------------------------------------------------

ssgi::SamplerState::SamplerState(void)
{
	memset(&m_samplerDesc, 0, sizeof(m_samplerDesc));
	m_samplerState = NULL;
}


ssgi::SamplerState::~SamplerState(void)
{
	if(m_samplerState != NULL){
		m_samplerState->Release();
		m_samplerState = NULL;
	}
}

int ssgi::SamplerState::initialize(ssgi::GraphicsLoaderImpl *loader)
{
	memset(&m_samplerDesc, 0, sizeof(m_samplerDesc));
	m_samplerState = NULL;
	m_loader = loader;

	m_samplerDesc.Filter         = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	m_samplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_samplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_samplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
	m_samplerDesc.MaxAnisotropy  = 16;
	m_samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	m_samplerDesc.MaxLOD         = FLT_MAX;
	m_samplerDesc.MinLOD         = -FLT_MAX;

	HRESULT hr = loader->getDevice()->CreateSamplerState(&m_samplerDesc, &m_samplerState);
	return hr;
}

int ssgi::SamplerState::setFilter(ssg::TextureFilter minFilter,
								  ssg::TextureFilter magFilter,
								  ssg::TextureMipFilter mipFilter)
{

	m_samplerDesc.Filter = ssgi::convertTextureFilter(minFilter, magFilter, mipFilter);
	if(m_samplerState != NULL){
		m_samplerState->Release();
		m_samplerState = NULL;
	}
	return m_loader->getDevice()->CreateSamplerState(&m_samplerDesc, &m_samplerState);
}

int ssgi::SamplerState::setAddrMode(ssg::TextureAddrMode uAddrMode, 
									ssg::TextureAddrMode vAddrMode)
{
	m_samplerDesc.AddressU = ssgi::convertTextureAddrMode(uAddrMode);
	m_samplerDesc.AddressV = ssgi::convertTextureAddrMode(vAddrMode);
	if(m_samplerState != NULL){
		m_samplerState->Release();
		m_samplerState = NULL;
	}
	return m_loader->getDevice()->CreateSamplerState(&m_samplerDesc, &m_samplerState);
}

ID3D11SamplerState *ssgi::SamplerState::getID3D11SamplerState(void) const
{
	return m_samplerState; 
}


/* -------------------------------------------------------------- */

ssgi::TextureImpl::TextureImpl(void)
{
	m_srv = NULL;
	m_textureBuffer = NULL;
	m_freeTextureBuffer = false;
}

int ssgi::TextureImpl::initialize(ssgi::GraphicsLoaderImpl *loader, 
								  ssg::TextureFormat texFormat,
								  uint32_t width,
								  uint32_t height,
								  uint32_t mipCount,
								  ssg::BufferAccessMode accessMode,
								  uint32_t bindFlags)
{
	BufferFormat format;
	switch(texFormat)
		{
		case ssg::kTextureFormatA8B8G8R8Unorm:
			{
				format = kBufferFormatX8X8X8X8;
				break;
			}
		case ssg::kTextureFormatR8Unorm:
			{
				format = kBufferFormatX8;
				break;
			}
		};

	m_textureBuffer = new Texture2dBufferImpl();
	if(m_textureBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}
	m_freeTextureBuffer = true;

	int ret = m_textureBuffer->initialize(loader, format,
										 width, height, mipCount, 
										 accessMode, 
										 bindFlags,
										 kMultisampleNone);
	if(ret != SCE_OK){
		delete m_textureBuffer;
		m_textureBuffer = NULL;
		return ret;
	}
	
	DXGI_FORMAT dxFormat = m_textureBuffer->getDxgiFormat();
	dxFormat  = (dxFormat == DXGI_FORMAT_R8G8B8A8_TYPELESS) ? DXGI_FORMAT_R8G8B8A8_UNORM : dxFormat; //TODO
	dxFormat  = (dxFormat == DXGI_FORMAT_R8_TYPELESS) ? DXGI_FORMAT_R8_UNORM : dxFormat;
	initSrv(dxFormat);

	ret = m_samplerState.initialize(loader);
	if(ret != SCE_OK){
		return ret;
	}

	return SCE_OK;
}

int ssgi::TextureImpl::initializeFromDx11Texture(GraphicsLoaderImpl *loader, ID3D11Texture2D *texture)
{
	m_textureBuffer = new Texture2dBufferImpl();
	int ret = m_textureBuffer->initializeFromExistentTexture(loader, texture);
	if(ret != SCE_OK){
		delete m_textureBuffer ;
		return ret;
	}
	if(m_textureBuffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}
	m_freeTextureBuffer = false;
	
	DXGI_FORMAT dxFormat = m_textureBuffer->getDxgiFormat();
	dxFormat  = (dxFormat == DXGI_FORMAT_R8G8B8A8_TYPELESS) ? DXGI_FORMAT_R8G8B8A8_UNORM : dxFormat; //TODO
	dxFormat  = (dxFormat == DXGI_FORMAT_R8_TYPELESS) ? DXGI_FORMAT_R8_UNORM : dxFormat;
	initSrv(dxFormat);

	ret = m_samplerState.initialize(loader);
	if(ret != SCE_OK){
		return ret;
	}
	return SCE_OK;
}


static int loadTGAFromMemory(ID3D11Resource **texture, const void *fileimage, uint32_t size, ID3D11Device *loader)
{
	ssgi::TgaFile tgaFile;
	int ret = tgaFile.openFromMemory(fileimage, size);
	if(ret != SCE_OK){
		return ret;
	}

	uint8_t *imageData =(uint8_t*)_malloca(tgaFile.getWidth() * tgaFile.getHeight() * 4);
	if(imageData == NULL){
		printf("malloc failed \n");
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}
	SCE_SAMPLE_UTIL_ASSERT(imageData != NULL);


	ret = tgaFile.readData(imageData, 0);
	if(ret != SCE_OK){
		tgaFile.close();
		return ret;
	}

	D3D11_TEXTURE2D_DESC desc; 
	desc.Width              = tgaFile.getWidth();
	desc.Height             = tgaFile.getHeight();
	desc.MipLevels          = 1;
	desc.ArraySize          = 1; 
	desc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM; 
	desc.SampleDesc.Count   = 1; 
	desc.SampleDesc.Quality = 0; 
	desc.Usage              = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags          = D3D11_BIND_SHADER_RESOURCE; 
	desc.CPUAccessFlags     = 0;
	desc.MiscFlags          = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = imageData;
	data.SysMemPitch = tgaFile.getWidth() * 4;
	data.SysMemSlicePitch = 0;

	HRESULT result = loader->CreateTexture2D( &desc, &data, (ID3D11Texture2D **)texture); 
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED(result));

	ret = tgaFile.close();
	if(ret !=SCE_OK){
		return ret;
	}

	return SCE_OK;
}


int ssgi::TextureImpl::initializeFromFile(ssgi::GraphicsLoaderImpl *loader, const char *filename)
{
	uint32_t filesize = 0;
	void *fileimage = util::malloc_and_load(filename, &filesize);
	
	int ret = initializeFromMemory(loader, fileimage, filesize);

	free(fileimage);
	return ret;
}

int ssgi::TextureImpl::initSrv(DXGI_FORMAT srvFormat)
{
	if(m_srv != NULL){
		m_srv->Release();
		m_srv = NULL;
	}
	GraphicsLoaderImpl *loader = dynamic_cast<GraphicsLoaderImpl *>(m_textureBuffer->getLoader());
	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	desc.Format                    = srvFormat;
	desc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipLevels       = m_textureBuffer->getMipCount();
	desc.Texture2D.MostDetailedMip = 0;

	HRESULT hr = loader->getDevice()->CreateShaderResourceView(m_textureBuffer->getResource(), &desc, &m_srv);
	SCE_SAMPLE_UTIL_ASSERT(hr >= 0);
	
	return SCE_OK;
}

int ssgi::TextureImpl::initializeFromMemory(GraphicsLoaderImpl *loader, const void *image, uint32_t imageSize)
{
	ID3D11Resource *texture;
	int ret = loadTGAFromMemory(&texture, image, imageSize, loader->getDevice());
	if(ret == SCE_OK){
		ret = initializeFromDx11Texture(loader, (ID3D11Texture2D *)texture);
		if(ret != SCE_OK){
			texture->Release();
			return ret;
		}
		m_freeTextureBuffer = true;
		return SCE_OK;
	}
		
	/*------------------------------------------------------------------------------*/
	ImageFile imageFile;
	ret = imageFile.openFromMemory(image, imageSize);
	if(ret != SCE_OK){
		return ret;
	}
	uint32_t mipCount = imageFile.getMipCount();
	ret = initialize(loader, ssg::kTextureFormatA8B8G8R8Unorm, imageFile.getWidth(), imageFile.getHeight(), 
					 (mipCount == 0) ? 1 : mipCount, ssg::kBufferAccessModeGpuReadCpuWrite, ssg::kBufferBindFlagShaderResource);
	if(ret != SCE_OK){
		imageFile.close();
		return SCE_OK;
	}
	void *ptr = beginWrite();
	ret = imageFile.readData(ptr);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	endWrite();
	
	imageFile.close();
	return SCE_OK;

}


int ssgi::TextureImpl::initializeFromExistentTextureBuffer(GraphicsLoaderImpl *loader, 
														   Texture2dBufferImpl *textureBuffer, 
														   DXGI_FORMAT shaderResourceViewFormat)
{
	m_textureBuffer = textureBuffer;
	m_freeTextureBuffer = false;

	if(m_textureBuffer->getD3d11BindFlag() & D3D11_BIND_SHADER_RESOURCE){

		if(shaderResourceViewFormat == DXGI_FORMAT_UNKNOWN){
			DXGI_FORMAT texFormat = m_textureBuffer->getDxgiFormat();
			switch(texFormat){
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:
				{ 
					shaderResourceViewFormat = DXGI_FORMAT_R8G8B8A8_UNORM; 
					break ; 
				}
			case DXGI_FORMAT_R16_TYPELESS:
				{ 
					shaderResourceViewFormat = DXGI_FORMAT_R16_UNORM; 
					break ; 
				}
			default:
				shaderResourceViewFormat = texFormat;
				break;
			}
		}
		//printf(__FILE__"(%d) shaderResourceViewFormat=%#x\n", __LINE__, shaderResourceViewFormat);
		initSrv(shaderResourceViewFormat);
	}

	int ret = m_samplerState.initialize(loader);
	if(ret != SCE_OK){
		return ret;
	}
	return SCE_OK;
}



int ssgi::TextureImpl::setFilter(ssg::TextureFilter minFilter,	ssg::TextureFilter magFilter,	ssg::TextureMipFilter mipFilter)
{
	return m_samplerState.setFilter(minFilter, magFilter, mipFilter);
}

int ssgi::TextureImpl::setAddrMode(ssg::TextureAddrMode uAddrMode, ssg::TextureAddrMode vAddrMode)
{
	return m_samplerState.setAddrMode(uAddrMode, vAddrMode);
}



uint32_t ssgi::TextureImpl::getWidth(void) const
{
	return m_textureBuffer->getWidth();
}
uint32_t ssgi::TextureImpl::getHeight(void) const
{
	return m_textureBuffer->getHeight();
}
uint32_t ssgi::TextureImpl::getStride(void) const
{
	return m_textureBuffer->getStride();
}


int ssgi::TextureImpl::finalize(void)
{
	if(m_srv != NULL){
		m_srv->Release();
		m_srv = NULL;
	}
	if(m_freeTextureBuffer){
		if(m_textureBuffer != NULL){
			delete m_textureBuffer;
			m_textureBuffer = NULL;
		}
	}
	return SCE_OK;
}




#endif /* _SCE_TARGET_OS_WINDOWS */
