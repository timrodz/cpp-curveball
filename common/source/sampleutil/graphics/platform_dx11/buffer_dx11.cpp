/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS


#include <sampleutil/graphics/buffer.h>
#include "buffer_internal_dx11.h"
#include "loader_internal_dx11.h"
#include "../image.h"
#include "util/util.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

static uint32_t convertBindFlags(uint32_t bindFlags)
{
	UINT        dx11BindFlag = 0;
	dx11BindFlag |= (bindFlags & ssg::kBufferBindFlagShaderResource  ) ? D3D11_BIND_SHADER_RESOURCE  : 0;
	dx11BindFlag |= (bindFlags & ssg::kBufferBindFlagRenderTarget    ) ? D3D11_BIND_RENDER_TARGET    : 0;
	dx11BindFlag |= (bindFlags & ssg::kBufferBindFlagDepthStencil    ) ? D3D11_BIND_DEPTH_STENCIL    : 0;
	dx11BindFlag |= (bindFlags & ssg::kBufferBindFlagVertexBuffer    ) ? D3D11_BIND_VERTEX_BUFFER    : 0;
	dx11BindFlag |= (bindFlags & ssg::kBufferBindFlagIndexBuffer     ) ? D3D11_BIND_INDEX_BUFFER     : 0;
	dx11BindFlag |= (bindFlags & ssg::kBufferBindFlagConstantBuffer  ) ? D3D11_BIND_CONSTANT_BUFFER  : 0;
	dx11BindFlag |= (bindFlags & ssg::kBufferBindFlagUnorderedAccess ) ? D3D11_BIND_UNORDERED_ACCESS : 0;
	//printf("bindFlag %#x -> %#X\n", bindFlags, dx11BindFlag);
	return dx11BindFlag;

}

static D3D11_USAGE convertUsage(ssg::BufferAccessMode accessMode)
{
	switch(accessMode)
	{
	case ssg::kBufferAccessModeGpuReadWrite:	   { return D3D11_USAGE_DEFAULT; }
	case ssg::kBufferAccessModeGpuReadOnly:	   { return D3D11_USAGE_IMMUTABLE; }
	case ssg::kBufferAccessModeGpuReadCpuWrite: { return D3D11_USAGE_DYNAMIC; }
	}
	return D3D11_USAGE_DEFAULT;
}

static UINT convertCpuAccessFlags(ssg::BufferAccessMode accessMode)
{
	switch(accessMode)
	{
	case ssg::kBufferAccessModeGpuReadWrite:	   { return 0; }
	case ssg::kBufferAccessModeGpuReadOnly:	   { return 0; }
	case ssg::kBufferAccessModeGpuReadCpuWrite:   { return D3D11_CPU_ACCESS_WRITE; }
	}
	return 0;
}

ssgi::BufferImpl::BufferImpl(void)
{
	m_buffer = NULL;
	m_loader = NULL;
}

int ssgi::BufferImpl::initialize(GraphicsLoaderImpl *loader, uint32_t size, 
	ssg::BufferAccessMode    accessMode,
	uint32_t            bufferBindFlags,
	size_t              elementSize,
	uint32_t bufferAlign)
{

	D3D11_BUFFER_DESC vbDesc;
	uint32_t d = size%16;
	if(d > 0){
		size += (16-d);
	}
	vbDesc.ByteWidth               = size;
	vbDesc.Usage                   = convertUsage(accessMode);
	vbDesc.BindFlags               = convertBindFlags(bufferBindFlags);
	vbDesc.CPUAccessFlags          = convertCpuAccessFlags(accessMode) ;
	if(elementSize > 0){
		vbDesc.MiscFlags           = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		vbDesc.StructureByteStride = elementSize;
	}else{
		vbDesc.MiscFlags      = 0;
		vbDesc.StructureByteStride = 0;
	}
		
	ID3D11Buffer *buffer;
	HRESULT hr = loader->getDevice()->CreateBuffer( &vbDesc, NULL, &buffer );
	if( hr != S_OK ){
		printf("size=%d, hr = %#x\n", size, hr); 
		return -1;
	}
	m_buffer = buffer;
	m_loader = loader;

	m_size = size;
	m_accessMode = accessMode;
	m_bufferBindFlags = bufferBindFlags;
	m_elementSize = elementSize;
	
	return SCE_OK;
}


int ssgi::BufferImpl::finalize(void)
{
	if(m_buffer != NULL){
		m_buffer->Release();
		m_buffer = NULL;
	}
	m_loader = NULL;
	return 0;
}

void *ssgi::BufferImpl::beginWrite(void)
{
	ID3D11DeviceContext *context;
	ssgi::GraphicsLoaderImpl* loader = dynamic_cast<ssgi::GraphicsLoaderImpl*>(m_loader);
	if(loader == NULL){
		printf("loader=%p, m_loader=%p\n", loader, m_loader);
	}
	loader->m_mutex.lock();
	//printf("lock by %#x\n", GetCurrentThreadId());
	loader->getDevice()->GetImmediateContext(&context);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( hr != S_OK ){
		//printf("unlock by %#x\n", GetCurrentThreadId());
		loader->m_mutex.unlock();
		return NULL;
	}

	return mappedResource.pData;
}

void ssgi::BufferImpl::endWrite(void)
{
	ID3D11DeviceContext *context;
	ssgi::GraphicsLoaderImpl* loader = dynamic_cast<ssgi::GraphicsLoaderImpl*>(m_loader);

	loader->getDevice()->GetImmediateContext(&context);
	
	context->Unmap(m_buffer, 0);

	//printf("unlock by %#x\n", GetCurrentThreadId());
	loader->m_mutex.unlock();
}

ssgi::BufferImpl::~BufferImpl(void)
{
	finalize();
}

/* ----------------------------------------------------------------------------------------- */



/* ----------------------------------------------------------------------------------------- */


int ssgi::VertexBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t size)
{
	return m_dataBuffer.initialize(loader, size, 
								   kBufferAccessModeGpuReadCpuWrite,
								   kBufferBindFlagVertexBuffer, 0);
}
 
int ssgi::IndexBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t size)
{
	return m_dataBuffer.initialize(loader, size, 
								   kBufferAccessModeGpuReadCpuWrite,
								   kBufferBindFlagIndexBuffer, 0);
}

int ssgi::UniformBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t size)
{
	SCE_SAMPLE_UTIL_ASSERT(loader != NULL);

	int ret =m_dataBuffer.initialize(loader, size, 
								   kBufferAccessModeGpuReadCpuWrite,
								   kBufferBindFlagConstantBuffer, 0);


	m_dataBuffer.beginWrite();
	m_dataBuffer.endWrite();
	return ret;
			
			
}

int ssgi::StructuredBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t stride, uint32_t numElements)
{
	int ret = m_dataBuffer.initialize(loader, stride * numElements, 
								   kBufferAccessModeGpuReadCpuWrite,
								   kBufferBindFlagShaderResource,
								   stride);
	if(ret != SCE_OK){
		return ret;
	}

	if(m_srv != NULL){
		m_srv->Release();
		m_srv = NULL;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format                    = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension             = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement     = 0;
	desc.BufferEx.NumElements      = numElements;
	desc.BufferEx.Flags            = 0;

	HRESULT hr = loader->getDevice()->CreateShaderResourceView( m_dataBuffer.getResource(), &desc, &m_srv);
	SCE_SAMPLE_UTIL_ASSERT(hr >= 0);
	
	return SCE_OK;
}

int ssgi::RwStructuredBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t stride, uint32_t numElements)
{
	int ret = m_dataBuffer.initialize(loader, stride * numElements, 
		                              kBufferAccessModeGpuReadWrite,
									  kBufferBindFlagUnorderedAccess,
									  stride);
	if(ret != SCE_OK){
		return ret;
	}

	if(m_uav != NULL){
		m_uav->Release();
		m_uav = NULL;
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Format                    = DXGI_FORMAT_UNKNOWN;
	desc.ViewDimension             = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement     = 0;
	desc.Buffer.NumElements      = numElements;
	desc.Buffer.Flags            = 0;

	HRESULT hr = loader->getDevice()->CreateUnorderedAccessView( m_dataBuffer.getResource(), &desc, &m_uav);
	SCE_SAMPLE_UTIL_ASSERT(hr >= 0);
	
	return SCE_OK;
}


//-------------------------------------------------------------------------------------------------------
// Texture Buffer
//-------------------------------------------------------------------------------------------------------

ssgi::Texture2dBufferImpl::Texture2dBufferImpl(void)
{
	m_freeTexture = false;
}
ssgi::Texture2dBufferImpl::~Texture2dBufferImpl(void)
{
	finalize();
}

int ssgi::Texture2dBufferImpl::finalize(void)
{
	if(m_freeTexture){
		if(m_buffer != NULL){
			m_buffer->Release();
			m_buffer = NULL;
		}
		m_loader = NULL;
		m_freeTexture = false;
		
	}else{
	}
	return SCE_OK;
}

void *ssgi::Texture2dBufferImpl::beginWrite(void)
{
	ID3D11DeviceContext *context;
	dynamic_cast<Dx11::GraphicsLoaderDx11*>(m_loader)->getDevice()->GetImmediateContext(&context);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if( hr != S_OK ){
		return NULL;
	}
	return mappedResource.pData;
}

void ssgi::Texture2dBufferImpl::endWrite(void)
{
	ID3D11DeviceContext *context;
	dynamic_cast<Dx11::GraphicsLoaderDx11*>(m_loader)->getDevice()->GetImmediateContext(&context);

	context->Unmap(m_buffer, 0);
}

ssg::GraphicsLoader* ssgi::Texture2dBufferImpl::getLoader(void) const
{
	return m_loader;
}


DXGI_SAMPLE_DESC ssgi::getSampleDesc(ssgi::GraphicsLoaderImpl *loader, ssg::MultisampleMode multisampleMode)
{
	DXGI_SAMPLE_DESC MSAA;
	MSAA.Count = 1;
	MSAA.Quality = 0;
	if(multisampleMode == ssg::kMultisampleNone){
		return MSAA;
	}

	uint32_t max = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	if(multisampleMode == ssg::kMultisample4x){
		max = 4;
	}
	if(multisampleMode == ssg::kMultisample2x){
		max = 2;
	}
	for(int i=0; i <= max; i++){
		UINT Quality;
		if SUCCEEDED(loader->getDevice()->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, i, &Quality)){
			if(0 < Quality){
				MSAA.Count = i;
				MSAA.Quality = Quality - 1;
			}
		}
	}
	return MSAA;
}


int ssgi::Texture2dBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader,
									ssg::BufferFormat format,
									uint32_t width,
									uint32_t height,
									uint32_t mipCount,
									ssg::BufferAccessMode accessMode,
									uint32_t bindFlags,
									ssg::MultisampleMode multiSampleMode
									)
{
	m_format = format;
	m_accessMode = accessMode;
	m_bufferBindFlags = bindFlags;
	m_multiSampleMode = multiSampleMode;
	
	DXGI_FORMAT dx11Format;
	switch(format)
	{
	case kBufferFormatX8X8X8X8 : { dx11Format = DXGI_FORMAT_R8G8B8A8_TYPELESS;        break ; }
	case kBufferFormatX16      : { dx11Format = DXGI_FORMAT_R16_TYPELESS;             break ; }
	case kBufferFormatX32X8X24 : { dx11Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS; break ; }
	case kBufferFormatX32      : { dx11Format = DXGI_FORMAT_R32_TYPELESS;             break ; }
	case kBufferFormatX24X8    : { dx11Format = DXGI_FORMAT_R24G8_TYPELESS;           break ; }
	case kBufferFormatX8       : { dx11Format = DXGI_FORMAT_R8_TYPELESS;           break ; }
	default : { abort(); }
	}
	
	UINT        dx11BindFlag = convertBindFlags(bindFlags);

	D3D11_USAGE dx11Usage          = convertUsage(accessMode);
	UINT        dx11CpuAccessFlags = convertCpuAccessFlags(accessMode);

	m_loader = loader;
	D3D11_TEXTURE2D_DESC DSDesc;
	DSDesc.Width              = width;
	DSDesc.Height             = height;
	DSDesc.MipLevels          = (mipCount==0) ? 1 : mipCount;
	DSDesc.ArraySize          = 1;
	DSDesc.Format             = dx11Format;
	DSDesc.SampleDesc         = getSampleDesc(loader, multiSampleMode);
	DSDesc.BindFlags          = dx11BindFlag;
	DSDesc.CPUAccessFlags     = dx11CpuAccessFlags;
	DSDesc.Usage              = dx11Usage;
	DSDesc.MiscFlags          = 0;

	//printf("Format = %d, BindFlag=%d, Usage=%d, CPUAccess=%d DSDesc.SampleDesc%d,%d, miplevel=%d\n", 
	//	DSDesc.Format, DSDesc.BindFlags, DSDesc.Usage, DSDesc.CPUAccessFlags, DSDesc.SampleDesc.Count, DSDesc.SampleDesc.Quality, DSDesc.MipLevels);
	ID3D11Texture2D *texBuffer = NULL;
	HRESULT hr = loader->getDevice()->CreateTexture2D( &DSDesc, NULL, &texBuffer );
	if(FAILED(hr)){
		SCE_SAMPLE_UTIL_ASSERT(hr == 0);
		return -1;

	}
	m_buffer = texBuffer;
	m_freeTexture = true;
	return SCE_OK;
}


uint32_t ssgi::Texture2dBufferImpl::getWidth(void) const
{
	ID3D11Texture2D *tex2D = (ID3D11Texture2D *)m_buffer;
	D3D11_TEXTURE2D_DESC texDesc;
	tex2D->GetDesc(&texDesc);
	return texDesc.Width;
}

uint32_t ssgi::Texture2dBufferImpl::getHeight(void) const
{
	ID3D11Texture2D *tex2D = (ID3D11Texture2D *)m_buffer;
	D3D11_TEXTURE2D_DESC texDesc;
	tex2D->GetDesc(&texDesc);
	return texDesc.Height;
}

uint32_t ssgi::Texture2dBufferImpl::getStride(void) const
{
	return 0;
}

DXGI_FORMAT ssgi::Texture2dBufferImpl::getDxgiFormat(void) const
{
	ID3D11Texture2D *tex2D = (ID3D11Texture2D *)m_buffer;
	if(tex2D == NULL){
		return DXGI_FORMAT_UNKNOWN;
	}
	D3D11_TEXTURE2D_DESC texDesc;
	tex2D->GetDesc(&texDesc);
	return texDesc.Format;
}

uint32_t ssgi::Texture2dBufferImpl::getMipCount(void) const
{
	ID3D11Texture2D *tex2D = (ID3D11Texture2D *)m_buffer;
	if(tex2D == NULL){
		return 0;
	}
	D3D11_TEXTURE2D_DESC texDesc;
	tex2D->GetDesc(&texDesc);
	return texDesc.MipLevels;
}

uint32_t ssgi::Texture2dBufferImpl::getD3d11BindFlag(void) const
{
	ID3D11Texture2D *tex2D = (ID3D11Texture2D *)m_buffer;
	if(tex2D == NULL){
		return 0;
	}
	D3D11_TEXTURE2D_DESC texDesc;
	tex2D->GetDesc(&texDesc);
	return texDesc.BindFlags;
}




int  ssgi::Texture2dBufferImpl::initializeFromExistentTexture(GraphicsLoaderImpl *loader, ID3D11Texture2D * textureBuffer)
{
	m_loader = loader;
	m_buffer = textureBuffer;
	m_freeTexture = false;
	return SCE_OK;
}


bool  ssgi::Texture2dBufferImpl::isMultiSample(void) const
{
	ID3D11Texture2D *tex2D = (ID3D11Texture2D *)m_buffer;
	if(tex2D == NULL){
		return false;
	}
	D3D11_TEXTURE2D_DESC texDesc;
	tex2D->GetDesc(&texDesc);
	//printf("isMultiSample(%d,%d)\n", texDesc.SampleDesc.Count,	texDesc.SampleDesc.Quality);
	return texDesc.SampleDesc.Count >= 2;
}



#endif /* _SCE_TARGET_OS_WINDOWS */
