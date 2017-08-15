/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include <sampleutil/graphics.h>
#include "context_internal_dx11.h"
#include "constant_internal_dx11.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;


static LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message ) 
	{
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			PostQuitMessage( 0 );
			exit(0);
			break;
		}
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		exit(0);
		break;

	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}

ssgi::FrameBufferImpl::FrameBufferImpl(void)
{
	m_windowHandle = NULL;
	m_swapChain = NULL;

	m_renderTargetTextureBuffer = NULL;
	m_depthTargetTextureBuffer = NULL;

	m_frameBufferRenderTarget = NULL;
	m_depthStencilSurface = NULL;
}


int ssgi::FrameBufferImpl::initialize(ssgi::GraphicsLoaderImpl *loader, uint32_t width, uint32_t height, 
											   ssg::MultisampleMode multisampleMode,
										   uint32_t numBuffer)
{
	HRESULT hr;
	static const TCHAR * myProgramName             = __TEXT("Sample Util");

	HINSTANCE instanceHandle = GetModuleHandle(NULL); 

	// Register class
	WNDCLASSEX windowClass;
	windowClass.cbSize         = sizeof(WNDCLASSEX);
	windowClass.style          = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc    = WndProc;
	windowClass.cbClsExtra     = 0;
	windowClass.cbWndExtra     = 0;
	windowClass.hInstance      = instanceHandle;
	windowClass.hIcon          = NULL;
	windowClass.hCursor        = LoadCursor( NULL, IDC_ARROW );
	windowClass.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
	windowClass.lpszMenuName   = NULL;
	windowClass.lpszClassName  = myProgramName;
	windowClass.hIconSm        = NULL;

	if( !RegisterClassEx( &windowClass ) ) {
		return E_FAIL;
	}

	// Create window    
	RECT windowRect = { 0, 0, width, height };
	AdjustWindowRect( &windowRect, WS_OVERLAPPEDWINDOW, FALSE );
	m_windowHandle = CreateWindow( myProgramName,          // Class name
		myProgramName,          // Window name
		WS_OVERLAPPEDWINDOW,    // Style
		CW_USEDEFAULT,          // X position
		CW_USEDEFAULT,          // Y position
		windowRect.right - windowRect.left,     // Width
		windowRect.bottom - windowRect.top,     // Height
		NULL,                   // Parent HWND
		NULL,                   // Menu
		instanceHandle,         // Instance
		NULL                    // Param
		);
	if( !m_windowHandle ){
		return E_FAIL;
	}

	ShowWindow(m_windowHandle, SW_SHOWNORMAL );
	m_displayDeviceContext = GetDC(m_windowHandle);

	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	ZeroMemory( &swapchainDesc, sizeof(swapchainDesc) );
	swapchainDesc.BufferCount                        = numBuffer;
	swapchainDesc.BufferDesc.Width                   = width;
	swapchainDesc.BufferDesc.Height                  = height;
	swapchainDesc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferDesc.RefreshRate.Numerator   = 60;
	swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapchainDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.OutputWindow                       = m_windowHandle;
	swapchainDesc.SampleDesc                         = getSampleDesc(loader, multisampleMode);
	swapchainDesc.Windowed                           = TRUE;

	IDXGIDevice * pDXGIDevice = NULL; 
	hr = loader->getDevice()->QueryInterface(__uuidof(IDXGIDevice), (void **)&pDXGIDevice);        
	IDXGIAdapter * pDXGIAdapter = NULL; 
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);  
	IDXGIFactory * pIDXGIFactory = NULL; 
	pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);       

	hr = pIDXGIFactory->CreateSwapChain(
		loader->getDevice(), 
		&swapchainDesc,
		&m_swapChain
		);
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED(hr));

	//--------------------------------------------------------------------------------------------------------------
	// initialize render target
	//--------------------------------------------------------------------------------------------------------------
	ID3D11Texture2D *pBuffer;
	hr = m_swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBuffer );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(hr, S_OK);

	m_renderTargetTextureBuffer = new Texture2dBufferImpl;
	m_renderTargetTextureBuffer->initializeFromExistentTexture(loader, pBuffer);
	

	m_frameBufferRenderTarget   = new RenderTargetImpl;
	int ret = m_frameBufferRenderTarget->initializeFromTextureBuffer(loader, m_renderTargetTextureBuffer);
	if(ret != SCE_OK){
		finalize();
		return ret;
	}
	
	//--------------------------------------------------------------------------------------------------------------
	// initialize depth target
	//--------------------------------------------------------------------------------------------------------------

	m_depthStencilSurface = new DepthStencilSurfaceImpl;
	if(m_depthStencilSurface == NULL){
		SCE_SAMPLE_UTIL_ASSERT(0);
		return -1;
	}
	
	ret = m_depthStencilSurface->initialize(loader, 
											width, 
											height, DXGI_FORMAT_D24_UNORM_S8_UINT, multisampleMode);
	if(ret != SCE_OK){
		finalize();
		SCE_SAMPLE_UTIL_ASSERT(0);
		return ret;
	}

	return SCE_OK;
}

int ssgi::FrameBufferImpl::finalize(void)
{
	if( m_swapChain ) {
		m_swapChain->Release();
		m_swapChain = NULL;
	}

	if(m_frameBufferRenderTarget != NULL){
		delete m_frameBufferRenderTarget;
		m_frameBufferRenderTarget = NULL;
	}
	if(m_depthStencilSurface != NULL){
		delete m_depthStencilSurface;
		m_depthStencilSurface = NULL;
	}

	if(m_renderTargetTextureBuffer != NULL){
		delete m_renderTargetTextureBuffer;
		m_renderTargetTextureBuffer = NULL;
	}
	if(m_depthTargetTextureBuffer != NULL){
		delete m_depthTargetTextureBuffer;
		m_depthTargetTextureBuffer = NULL;
	}


	return SCE_OK;
}


ssg::RenderTarget *ssgi::FrameBufferImpl::getNextRenderTarget(sce::SampleUtil::Graphics::GraphicsContext::FrameBufferSide side)
{
	(void)side;
	return m_frameBufferRenderTarget;
}

ssg::DepthStencilSurface *ssgi::FrameBufferImpl::getDepthStencilSurface(void)
{
	return m_depthStencilSurface;
}

int ssgi::FrameBufferImpl::flip(uint32_t numVSync)
{
	MSG msg = { 0 };
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
	{
		if(msg.message ==WM_QUIT){
			exit(0);
			return -1;
		}
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	/*
	static int i=0;
	i = (i+1) % (60*10);
	DXGI_MODE_DESC modeDesc;
	memset(&modeDesc, 0, sizeof(modeDesc));
	modeDesc.Width  = (i<60*5) ? 400 : 600; //width;
	modeDesc.Height = (i<60*5) ? 400 : 600; //height;
	modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	modeDesc.RefreshRate.Numerator   = 60;
	modeDesc.RefreshRate.Denominator = 1;
	m_swapChain->ResizeTarget(&modeDesc);
	*/

	m_swapChain->Present( numVSync, 0 );


	return 0;

}


HWND ssgi::FrameBufferImpl::getWindowHandle(void) const
{
	return m_windowHandle;
}

HDC ssgi::FrameBufferImpl::getDisplayDeviceContext(void) const
{
	return m_displayDeviceContext;
}


#endif /* _SCE_TARGET_OS_WINDOWS */
