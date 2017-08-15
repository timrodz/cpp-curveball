/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS
#include "buffer_internal_dx11.h"
#include "loader_internal_dx11.h"
#include "constant_internal_dx11.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

#pragma region RenderTargetImpl

int ssgi::RenderTargetImpl::initialize(GraphicsLoaderImpl *loader, 
									   uint32_t width, 
									   uint32_t height, 
									   ssg::TextureFormat format)
{
	//printf("creating render target\n");
	DXGI_FORMAT rtFormat;
	switch(format)
		{
		case kTextureFormatA8B8G8R8Unorm:
			{
				rtFormat = DXGI_FORMAT_R8G8B8A8_TYPELESS;
				break;
			};
		default:
			abort();//TODO
		}

	//printf("creating texture for render target\n");
	int ret = m_texture.initialize(loader, format, width, height, 0, 
								   kBufferAccessModeGpuReadWrite,
								   //kBufferBindFlagRenderTarget);
								   kBufferBindFlagShaderResource | kBufferBindFlagRenderTarget);
	if(ret < SCE_OK){
		printf(__FILE__"(%d) ret=%#x\n", __LINE__, ret);
		return ret;
	}
	//printf("creating texture for render target ok\n");



	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
	ZeroMemory(&renderTargetDesc, sizeof(renderTargetDesc));
	renderTargetDesc.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderTargetDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetDesc.Texture2D.MipSlice = 0;

	HRESULT hr = loader->getDevice()->CreateRenderTargetView( m_texture.m_textureBuffer->getResource(), &renderTargetDesc, &pRenderTargetView );
	if( FAILED(hr) ) { 
		printf(__FILE__"(%d) hr=%#x\n", __LINE__, hr);
		return -1;
	}
	m_freeOnFinalize = true; 
	m_width  = width;
	m_height = height;
	return SCE_OK;
}

int ssgi::RenderTargetImpl::initializeFromTextureBuffer(GraphicsLoaderImpl *loader, Texture2dBufferImpl* textureBuffer)
{
	int ret = m_texture.initializeFromExistentTextureBuffer(loader, textureBuffer);
	if(ret < SCE_OK){
		printf(__FILE__"(%d) ret=%#x\n", __LINE__, ret);
		SCE_SAMPLE_UTIL_ASSERT(0);
		return ret;
	}


	DXGI_FORMAT rtFormat;
	switch(textureBuffer->getDxgiFormat())
		{
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
		case DXGI_FORMAT_R8G8B8A8_UNORM:
			{
				rtFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
				break;
			};
		default:
			{
				SCE_SAMPLE_UTIL_ASSERT(0);
				break;
			}
		}

	
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc;
	ZeroMemory(&renderTargetDesc, sizeof(renderTargetDesc));
	renderTargetDesc.Format             = rtFormat;
	if(textureBuffer->isMultiSample()){
		//printf(__FILE__"(%d) multi sample\n", __LINE__);
		renderTargetDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	}else{
		//printf(__FILE__"(%d) single sample\n", __LINE__);
		renderTargetDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetDesc.Texture2D.MipSlice = 0;
	}
	HRESULT hr = loader->getDevice()->CreateRenderTargetView( textureBuffer->getResource(), &renderTargetDesc, &pRenderTargetView );
	if( FAILED(hr) ) { 
		printf(__FILE__"(%d) hr=%#x\n", __LINE__, hr);
		SCE_SAMPLE_UTIL_ASSERT(0);
		return -1;
	}
	
	m_freeOnFinalize = true; 
	m_width  = textureBuffer->getWidth();
	m_height = textureBuffer->getHeight();
	return SCE_OK;
}


int ssgi::RenderTargetImpl::finalize(void)
{
	if(m_freeOnFinalize){
		if(pRenderTargetView != NULL){
			pRenderTargetView->Release();
			pRenderTargetView = NULL;
		}
		
		m_texture.finalize();
		m_freeOnFinalize = false;
	}
	return SCE_OK;
}

#pragma endregion // RenderTargetImpl

#pragma region DepthStencilSurfaceImpl

int ssgi::DepthStencilSurfaceImpl::initialize(GraphicsLoaderImpl *loader, 
											  uint32_t width, 
											  uint32_t height, 
											  DXGI_FORMAT format,
											  ssg::MultisampleMode multiSampleMode)
{
	BufferFormat textureBufferFormat;
	DXGI_FORMAT depthSrvFormat;
	DXGI_FORMAT stencilSrvFormat;

	switch(format)
	{
	case DXGI_FORMAT_D16_UNORM:
		textureBufferFormat = kBufferFormatX16;
		depthSrvFormat      = DXGI_FORMAT_R16_UNORM;
		stencilSrvFormat    = DXGI_FORMAT_UNKNOWN;
		break;
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
		textureBufferFormat = kBufferFormatX32X8X24;
		depthSrvFormat      = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		stencilSrvFormat    = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;
		break;
	case DXGI_FORMAT_D32_FLOAT:
		textureBufferFormat = kBufferFormatX32;
		depthSrvFormat      = DXGI_FORMAT_R32_FLOAT;
		stencilSrvFormat    = DXGI_FORMAT_UNKNOWN;
		break;
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		textureBufferFormat = kBufferFormatX24X8;
		depthSrvFormat      = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		stencilSrvFormat    = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
		break;
	default:
		return -1;
	}
	


	int ret = m_textureBuffer.initialize(loader,
										 textureBufferFormat,
										 width,
										 height,
										 0,
										 kBufferAccessModeGpuReadWrite,
										 kBufferBindFlagDepthStencil | kBufferBindFlagShaderResource,
										 multiSampleMode );
	if(ret != SCE_OK){
		SCE_SAMPLE_UTIL_ASSERT(0);
		return ret;
	}

	ret = m_depthTexture.initializeFromExistentTextureBuffer(loader, &m_textureBuffer, depthSrvFormat);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	if(stencilSrvFormat == DXGI_FORMAT_UNKNOWN){
		ret = m_stencilTexture.initializeFromExistentTextureBuffer(loader, &m_textureBuffer, stencilSrvFormat);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	}

	DXGI_SAMPLE_DESC sd = getSampleDesc(loader, multiSampleMode);

	D3D11_DEPTH_STENCIL_VIEW_DESC  dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	dsDesc.Format             = format;
	if(sd.Count > 1){
		dsDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	}else{
		dsDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Texture2D.MipSlice = 0;
	}
	HRESULT hr = loader->getDevice()->CreateDepthStencilView( m_textureBuffer.getResource(), &dsDesc, &pDepthStencilView);
	if( FAILED(hr) ) { 
		SCE_SAMPLE_UTIL_ASSERT(0);
		return -1;
	}

	return SCE_OK;
}

int ssgi::DepthStencilSurfaceImpl::finalize(void)
{
	m_depthTexture.finalize();
	m_stencilTexture.finalize();
	m_textureBuffer.finalize();
	if(pDepthStencilView != NULL){
	   pDepthStencilView->Release();
	   pDepthStencilView  = NULL;
	}
	return SCE_OK;
}


#pragma endregion // DepthStencilSurfaceImpl

#endif /* _SCE_TARGET_OS_WINDOWS */
