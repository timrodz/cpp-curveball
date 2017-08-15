/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include <sampleutil/graphics.h>
#include "util/util.h"
#include "context_internal_dx11.h"
#include "constant_internal_dx11.h"

#include <malloc.h>

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace util = sce::SampleUtil::Internal::Util;

#pragma region GpuControl


int ssgi::GpuControl::initialize(
	sce::SampleUtil::Internal::Util::Mutex *mutex,
	ID3D11Device *device, ID3D11DeviceContext *context)
{
	m_device = device;
	m_context = context;
	m_depthStencilstate = NULL;
	m_mutex = mutex;

	m_currentDepthStencilDesc.DepthEnable                  = TRUE;
	m_currentDepthStencilDesc.DepthWriteMask               = D3D11_DEPTH_WRITE_MASK_ALL;
	m_currentDepthStencilDesc.DepthFunc                    = D3D11_COMPARISON_LESS_EQUAL;
	m_currentDepthStencilDesc.StencilEnable                = FALSE;
	m_currentDepthStencilDesc.StencilReadMask              = D3D11_DEFAULT_STENCIL_READ_MASK;
	m_currentDepthStencilDesc.StencilWriteMask             = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	m_currentDepthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
	m_currentDepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	m_currentDepthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
	m_currentDepthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;
	m_currentDepthStencilDesc.BackFace.StencilFailOp       = D3D11_STENCIL_OP_KEEP;
	m_currentDepthStencilDesc.BackFace.StencilDepthFailOp  = D3D11_STENCIL_OP_KEEP;
	m_currentDepthStencilDesc.BackFace.StencilPassOp       = D3D11_STENCIL_OP_KEEP;
	m_currentDepthStencilDesc.BackFace.StencilFunc         = D3D11_COMPARISON_ALWAYS;

	HRESULT hr = m_device->CreateDepthStencilState(&m_currentDepthStencilDesc, &m_depthStencilstate);
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED( hr ));

	{
		util::FocusLock flock(m_mutex);
		m_context->OMSetDepthStencilState(m_depthStencilstate, 0);
	}


	m_currentRasterizeDesc.FillMode              = D3D11_FILL_SOLID;
	m_currentRasterizeDesc.CullMode              = D3D11_CULL_NONE;
	m_currentRasterizeDesc.FrontCounterClockwise = FALSE;
	m_currentRasterizeDesc.DepthBias             = 0;
	m_currentRasterizeDesc.DepthBiasClamp        = 0;
	m_currentRasterizeDesc.SlopeScaledDepthBias  = 0;
	m_currentRasterizeDesc.ScissorEnable         = FALSE;
	m_currentRasterizeDesc.MultisampleEnable     = TRUE;//FALSE;
	m_currentRasterizeDesc.AntialiasedLineEnable = FALSE;

	hr = m_device->CreateRasterizerState( &m_currentRasterizeDesc, &m_rasterizerState );
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED( hr ));
	{
		util::FocusLock flock(m_mutex);
		m_context->RSSetState(m_rasterizerState);
	}



	return SCE_OK;
}



int ssgi::GpuControl::finalize(void)
{
	if(m_depthStencilstate != NULL){
		m_depthStencilstate->Release();
		m_depthStencilstate = NULL;
	}
	if(m_rasterizerState != NULL){
		m_rasterizerState->Release();
		m_rasterizerState = NULL;
	}
	return SCE_OK;
}



void ssgi::GpuControl::setDepthFunc(ssg::DepthFunc depthFunc)
{
	if(m_depthStencilstate != NULL){
		m_depthStencilstate->Release();
		m_depthStencilstate = NULL;
	}
	m_currentDepthStencilDesc.DepthFunc  = ssgi::convertDepthFunc(depthFunc);
	HRESULT hr = m_device->CreateDepthStencilState(&m_currentDepthStencilDesc,&m_depthStencilstate);
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED( hr ));
	SCE_SAMPLE_UTIL_ASSERT(m_depthStencilstate != NULL);
	{
		util::FocusLock flock(m_mutex);
		m_context->OMSetDepthStencilState(m_depthStencilstate, 0);
	}
}

void ssgi::GpuControl::setDepthWriteEnable(bool enable)
{
	if(m_depthStencilstate != NULL){
		m_depthStencilstate->Release();
		m_depthStencilstate = NULL;
	}
	m_currentDepthStencilDesc.DepthEnable  = enable;
	HRESULT hr = m_device->CreateDepthStencilState(&m_currentDepthStencilDesc,&m_depthStencilstate);
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED( hr ));
	{
		util::FocusLock flock(m_mutex);
		m_context->OMSetDepthStencilState(m_depthStencilstate, 0);
	}
}

void ssgi::GpuControl::setStencilFunc(
	uint8_t compareMask,
	uint8_t writeMask,
	ssg::StencilFunc frontFaceFunc,
	ssg::StencilOp   frontFaceStencilFail,
	ssg::StencilOp   frontFaceDepthFail,
	ssg::StencilOp   frontFaceDepthPass,
	ssg::StencilFunc backFaceFunc,
	ssg::StencilOp   backFaceStencilFail,
	ssg::StencilOp   backFaceDepthFail,
	ssg::StencilOp   backFaceDepthPass,
	uint8_t stencilRef
	)
{
	if(m_depthStencilstate != NULL){
		m_depthStencilstate->Release();
		m_depthStencilstate = NULL;
	}
	m_currentDepthStencilDesc.StencilEnable                = TRUE;
	m_currentDepthStencilDesc.StencilReadMask              = compareMask;
	m_currentDepthStencilDesc.StencilWriteMask             = writeMask;
	m_currentDepthStencilDesc.FrontFace.StencilFailOp      = ssgi::convertStencilOp(frontFaceStencilFail);
	m_currentDepthStencilDesc.FrontFace.StencilDepthFailOp = ssgi::convertStencilOp(frontFaceDepthFail);
	m_currentDepthStencilDesc.FrontFace.StencilPassOp      = ssgi::convertStencilOp(frontFaceDepthPass);
	m_currentDepthStencilDesc.FrontFace.StencilFunc        = ssgi::convertStencilFunc(frontFaceFunc);
	m_currentDepthStencilDesc.BackFace.StencilFailOp       = ssgi::convertStencilOp(backFaceStencilFail);
	m_currentDepthStencilDesc.BackFace.StencilDepthFailOp  = ssgi::convertStencilOp(backFaceDepthFail);
	m_currentDepthStencilDesc.BackFace.StencilPassOp       = ssgi::convertStencilOp(backFaceDepthPass);
	m_currentDepthStencilDesc.BackFace.StencilFunc         = ssgi::convertStencilFunc(backFaceFunc);

	HRESULT hr = m_device->CreateDepthStencilState(&m_currentDepthStencilDesc,&m_depthStencilstate);
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED( hr ));
	{
		util::FocusLock flock(m_mutex);
		m_context->OMSetDepthStencilState(m_depthStencilstate, 0);
	}
}

void ssgi::GpuControl::setFillMode(bool isFill)
{
	m_currentRasterizeDesc.FillMode = isFill ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;

	if(m_rasterizerState != NULL){
		m_rasterizerState->Release();
	}
	{
		util::FocusLock flock(m_mutex);
		HRESULT hr = m_device->CreateRasterizerState( &m_currentRasterizeDesc, &m_rasterizerState );
		SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED( hr ));

		m_context->RSSetState(m_rasterizerState);
	}
}

void ssgi::GpuControl::setCullMode(ssg::CullMode cullMode)
{
	m_currentRasterizeDesc.CullMode = ssgi::convertCullMode(cullMode);

	if(m_rasterizerState != NULL){
		m_rasterizerState->Release();
	}

	{
		util::FocusLock flock(m_mutex);
		HRESULT hr = m_device->CreateRasterizerState( &m_currentRasterizeDesc, &m_rasterizerState );
		SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED( hr ));

		m_context->RSSetState(m_rasterizerState);
	}
}

void ssgi::GpuControl::setRegionClip(uint32_t topLeftX,
									uint32_t topLeftY,
									uint32_t width,
									uint32_t height)
{
	m_currentRasterizeDesc.ScissorEnable = TRUE;
	if(m_rasterizerState != NULL){
		m_rasterizerState->Release();
	}
	HRESULT hr = m_device->CreateRasterizerState( &m_currentRasterizeDesc, &m_rasterizerState );
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED( hr ));
	m_context->RSSetState(m_rasterizerState);

	D3D11_RECT rects[1];
	rects[0].left   = topLeftX;
	rects[0].right  = topLeftX + width;
	rects[0].top    = topLeftY;
	rects[0].bottom = topLeftY + height;

	{
		util::FocusLock flock(m_mutex);
		m_context->RSSetScissorRects(1, rects);
	}

}

void ssgi::GpuControl::setViewPort(uint32_t topLeftX,
								  uint32_t topLeftY,
								  uint32_t width,
								  uint32_t height,
								  float minDepth,
								  float maxDepth)
{
	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width    = (FLOAT)width;
	vp.Height   = (FLOAT)height;
	vp.MinDepth = minDepth;
	vp.MaxDepth = maxDepth;
	vp.TopLeftX = (FLOAT)topLeftX;
	vp.TopLeftY = (FLOAT)topLeftY;

	{
		util::FocusLock flock(m_mutex);
		m_context->RSSetViewports( 1, &vp );
	}
}

void ssgi::GpuControl::setLineWidth(uint32_t width)
{
	return;
}
void ssgi::GpuControl::setPointSize(uint16_t,uint16_t)
{
	return;
}


#pragma endregion 

// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------

#pragma region GpuBufferState

int ssgi::GpuBufferState::initialize(sce::SampleUtil::Internal::Util::Mutex *mutex, ID3D11DeviceContext *context)
{
	m_context = context;
	m_mutex   = mutex;
	for(uint32_t i=0; i<D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT; i++){
		m_vertexBuffers[i] = NULL;
	}
	for(uint32_t i=0; i<D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; i++){
		m_vertexParameterBuffers[i] = NULL;
		m_fragmentParameterBuffers[i] = NULL;
	}

	return SCE_OK;
}

int ssgi::GpuBufferState::setVertexBuffer(ssg::VertexBuffer *vertexBuffer, uint32_t streamIndex)
{
	if(streamIndex >= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT){
		return -1;
	}
	m_vertexBuffers[streamIndex] = dynamic_cast<VertexBufferImpl*>(vertexBuffer)->getBuffer();
	return SCE_OK;
}

void ssgi::GpuBufferState::setUpVertexBuffers(const ssgi::VertexProgramImpl *vertexProgram)
{
	if(vertexProgram == NULL){
		return;
	}
	util::FocusLock flock(m_mutex);
	for(uint32_t i=0; i<vertexProgram->numStreams; i++){
		if(m_vertexBuffers[i] != NULL){
			m_context->IASetVertexBuffers(i, 1,
				&m_vertexBuffers[i],
				&vertexProgram->strides[i],
				&vertexProgram->offsets[i]);
		}
	}

}

void ssgi::GpuBufferState::setUpUniformBuffers(void)
{
	util::FocusLock flock(m_mutex);
	for(uint32_t i=0; i<D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT; i++){
		if(m_vertexParameterBuffers[i] != NULL){
			ID3D11Buffer *buf = m_vertexParameterBuffers[i]->getBuffer();
			m_context->VSSetConstantBuffers(
				i,
				1,
				&buf
				);
		}
		if(m_fragmentParameterBuffers[i] != NULL){
			ID3D11Buffer *buf = m_fragmentParameterBuffers[i]->getBuffer();
			m_context->PSSetConstantBuffers(
				i,
				1,
				&buf
				);
		}
	}
}

int ssgi::GpuBufferState::setUniformBuffer(ssg::ProgramType programType, ssg::UniformBuffer *buffer, uint32_t bufferIndex)
{
	UniformBufferImpl *buf = dynamic_cast<UniformBufferImpl *>(buffer);
	if(programType == kProgramTypeVertex){
		m_vertexParameterBuffers[bufferIndex] = buf;
	}else{
		m_fragmentParameterBuffers[bufferIndex] = buf;
	}
	return SCE_OK;
}


int ssgi::GpuBufferState::setTexture(ssg::ProgramType programType, ssg::Texture *texture, uint32_t index)
{
	util::FocusLock flock(m_mutex);
	ssgi::TextureImpl *t = dynamic_cast<ssgi::TextureImpl*>(texture);
	ID3D11ShaderResourceView *view = NULL;
	ID3D11SamplerState *samplerState = NULL;
	if(t != NULL){
		view = t->getD3D11ShaderResourceView();
	}
	if(t != NULL){
		samplerState = t->getD3D11SamplerState();
	}
	switch(programType)
		{
		case kProgramTypeVertex:
			{
				if(view != NULL){
					m_context->VSSetShaderResources(index, 1, &view);
				}
				if(samplerState != NULL){
					m_context->VSSetSamplers(index, 1, &samplerState);
				}
				break;
			}
		case kProgramTypeFragment:
			{
				if(view != NULL){
					m_context->PSSetShaderResources(index, 1, &view);
				}
				if(samplerState != NULL){
					m_context->PSSetSamplers(index, 1, &samplerState);
				}
				break;
			}
		case kProgramTypeCompute:
			{
				if(view != NULL){
					m_context->CSSetShaderResources(index, 1, &view);
				}
				if(samplerState != NULL){
					m_context->CSSetSamplers(index, 1, &samplerState);
				}
				break;
			}
		default:
			break;
		}
	return 0;
}

int ssgi::GpuBufferState::setResource(ssg::ProgramType programType, ssg::Texture *buffer, uint32_t index)
{
	return setTexture(programType, buffer, index);
}
int ssgi::GpuBufferState::setResource(ssg::ProgramType programType, ssg::StructuredBuffer *structuredBuffer, uint32_t index)
{
	util::FocusLock flock(m_mutex);
	ssgi::StructuredBufferImpl *sb = dynamic_cast<ssgi::StructuredBufferImpl *>(structuredBuffer);
	if(sb == NULL){
		return SCE_OK;
	}
	ID3D11ShaderResourceView *view = sb->getD3D11ShaderResourceView();
	if(view == NULL){
		return SCE_OK;
	}

	switch(programType)
		{
		case kProgramTypeVertex:
			{
				m_context->VSSetShaderResources(index, 1, &view);
				break;
			}
		case kProgramTypeFragment:
			{
				m_context->PSSetShaderResources(index, 1, &view);
				break;
			}
		case kProgramTypeCompute:
			{
				m_context->CSSetShaderResources(index, 1, &view);
				break;
			}
		default:
			break;
		}
	return 0;

	return SCE_OK;
}


int ssgi::GpuBufferState::setRwResource(ssg::ProgramType programType, ssg::RwStructuredBuffer *structuredBuffer, uint32_t index)
{
	util::FocusLock flock(m_mutex);
	ssgi::RwStructuredBufferImpl *sb = dynamic_cast<ssgi::RwStructuredBufferImpl *>(structuredBuffer);
	if(sb == NULL){
		return SCE_OK;
	}
	ID3D11UnorderedAccessView *view = sb->getD3D11UnorderedAccessView();
	if(view == NULL){
		return SCE_OK;
	}

	switch(programType)
		{
		case kProgramTypeCompute:
			{
				m_context->CSSetUnorderedAccessViews(index, 1, &view, NULL);
				break;
			}
		default:
			break;
		}
	return SCE_OK;
}


#pragma endregion

// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------

ssgi::GraphicsContextImpl::GraphicsContextImpl(void)
{
	int ret;
	m_pDevice = NULL;
	m_driverType        = D3D_DRIVER_TYPE_NULL;
	memset(&m_state, 0, sizeof(State));
}

int ssgi::GraphicsContextImpl::initialize(uint32_t width, uint32_t height, ssg::GraphicsContextOption* option)
{
	int ret;
	HRESULT hr = S_OK;

	MultisampleMode multisampleMode;
	uint32_t numFrameBuffer;
 
	if (option == NULL)	{
		multisampleMode = m_defaultMultisampleMode;
		numFrameBuffer = m_defaultNumFrameBuffer;
	}else{
		multisampleMode = option->multisampleMode;
		numFrameBuffer = option->numFrameBuffer;
	}

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		//D3D_DRIVER_TYPE_REFERENCE,
	};

	UINT numDriverTypes = sizeof( driverTypes ) / sizeof( driverTypes[0] );

	//D3D_FEATURE_LEVEL FeatureLevels     = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE( featureLevels );

	D3D_FEATURE_LEVEL *pFeatureLevel    = NULL;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		m_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDevice(NULL, m_driverType,	NULL,	createDeviceFlags,
			featureLevels,	numFeatureLevels,	D3D11_SDK_VERSION,	&m_pDevice,	pFeatureLevel,	&m_pDeviceContext);

		if( SUCCEEDED( hr ) ){
			break;
		}
	}
	if( FAILED(hr) ) {
		return hr;
	}

	m_loader.initialize(	m_pDevice);

	ret = m_frameBuffer.initialize(&m_loader, width, height, multisampleMode, numFrameBuffer);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = m_gpuControl.initialize(&m_loader.m_mutex, m_pDevice, m_pDeviceContext);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = m_gpuBufferState.initialize(&m_loader.m_mutex, m_pDeviceContext);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	// Setup the viewport
	setViewPort(0, 0, width, height, 0.0f, 1.0f);

	m_reservedVertexUniformBuffer = NULL;
	m_reservedFragmentUniformBuffer = NULL;
	return 0;
}

int ssgi::GraphicsContextImpl::finalize(void)
{
	int ret;
	m_frameBuffer.finalize();
	ret = m_gpuControl.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	if(m_pDevice != NULL){
		m_pDevice->Release();
		m_pDevice = NULL;
	}
	if(m_pDeviceContext != NULL){
		m_pDeviceContext->ClearState();
		m_pDeviceContext->Release();
		m_pDeviceContext = NULL;
	}

	return 0;
}

void ssgi::GraphicsContextImpl::clearRenderTarget(uint32_t color)
{
	uint8_t r = (color & 0xff000000) >> 24;
	uint8_t g = (color & 0x00ff0000) >> 16;
	uint8_t b = (color & 0x0000ff00) >> 8;
	uint8_t a = (color & 0x000000ff) >> 0;
	float ClearColor[4] = { r/255.0f, g/255.0f, b/255.0f, a/255.0f }; // RGBA

	util::FocusLock flock(&m_loader.m_mutex);
	m_pDeviceContext->ClearRenderTargetView(m_state.m_currentRenderTarget->getRenderTargetView(), ClearColor);
	m_pDeviceContext->ClearDepthStencilView( m_state.m_currentDepthStencilSurface->getDepthStencilView(),D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0 );
	util::checkForCgError("clearRenderTarget");
}

int ssgi::GraphicsContextImpl::draw(ssg::Primitive primitive, ssg::IndexBuffer *indexBuffer, uint32_t count)
{
	int ret;

	const VertexProgramImpl *vertexProgram   = m_state.m_vertexProgram;
	const FragmentProgramImpl *fragmentProgram = m_state.m_fragmentProgram;

	util::FocusLock flock(&m_loader.m_mutex);

	if(vertexProgram != NULL){
		m_pDeviceContext->IASetInputLayout(vertexProgram->m_layout);
		m_gpuBufferState.setUpVertexBuffers(vertexProgram);
		m_pDeviceContext->VSSetShader(vertexProgram->m_vertexShader, NULL, 0);
	}else{
		m_pDeviceContext->VSSetShader(NULL, NULL, 0);
	}

	if(m_reservedVertexUniformBuffer != NULL){
		m_reservedVertexUniformBuffer->endWrite();
		m_reservedVertexUniformBuffer = NULL;
	}

	if(m_reservedFragmentUniformBuffer != NULL){
		m_reservedFragmentUniformBuffer->endWrite();
		m_reservedFragmentUniformBuffer = NULL;
	}


	m_gpuBufferState.setUpUniformBuffers();

	if(fragmentProgram != NULL){
		m_pDeviceContext->PSSetShader(fragmentProgram->m_pixelShader, NULL, 0);
		m_pDeviceContext->OMSetBlendState(fragmentProgram->m_blendState, 0, 0xffffffff);
	}else{
		m_pDeviceContext->PSSetShader(NULL, NULL, 0);
	}

	const IndexBufferImpl *idxBuffer = dynamic_cast<const IndexBufferImpl *>(indexBuffer);

	m_pDeviceContext->IASetPrimitiveTopology(Impl::convertPrimitive(primitive));
	m_pDeviceContext->IASetIndexBuffer(idxBuffer->getBuffer(), DXGI_FORMAT_R16_UINT, 0);
	m_pDeviceContext->DrawIndexed(count,  0, 0);
	return SCE_OK;
}



ID3D11DeviceContext *ssgi::GraphicsContextImpl::getDeviceContext(void) const
{
	return m_pDeviceContext;
}


ID3D11Device *ssgi::GraphicsContextImpl::getDevice(void) const
{
	return m_pDevice;
}

HWND ssgi::GraphicsContextImpl::getWindowHandle(void) const
{
	return m_frameBuffer.getWindowHandle();
}

int ssgi::GraphicsContextImpl::setVertexProgram(ssg::VertexProgram *vertexProgram)
{
	m_state.m_vertexProgram = dynamic_cast<ssgi::VertexProgramImpl *>(vertexProgram);
	return SCE_OK;
}
int ssgi::GraphicsContextImpl::setFragmentProgram(ssg::FragmentProgram *fragmentProgram)
{
	m_state.m_fragmentProgram = dynamic_cast<ssgi::FragmentProgramImpl *>(fragmentProgram);
	return SCE_OK;
}


int ssgi::GraphicsContextImpl::beginScene(ssg::RenderTarget *iRenderTarget, ssg::DepthStencilSurface *idepthStencilSurface)
{
	util::FocusLock flock(&m_loader.m_mutex);

	RenderTargetImpl*        renderTarget        = dynamic_cast<RenderTargetImpl *>(iRenderTarget);
	DepthStencilSurfaceImpl* depthStencilSurface = dynamic_cast<DepthStencilSurfaceImpl*>(idepthStencilSurface);

	ID3D11RenderTargetView *rtViews[] = { renderTarget->getRenderTargetView() };
	m_pDeviceContext->OMSetRenderTargets( 1, rtViews, depthStencilSurface->getDepthStencilView() );
	SCE_SAMPLE_UTIL_ASSERT(depthStencilSurface->getDepthStencilView() != NULL);
	setViewPort(0, 0, 
		        renderTarget->getTexture()->getWidth(), 
				renderTarget->getTexture()->getHeight(), 
				0.0f, 1.0f);
	setRegionClip(0, 0, renderTarget->getTexture()->getWidth(), renderTarget->getTexture()->getHeight());

	m_state.m_currentRenderTarget        = renderTarget;
	m_state.m_currentDepthStencilSurface = depthStencilSurface;
	return 0;
}

int ssgi::GraphicsContextImpl::endScene(void)
{
	util::FocusLock flock(&m_loader.m_mutex);
	ID3D11RenderTargetView *r = NULL;
	m_pDeviceContext->OMSetRenderTargets( 1, &r, NULL );
	m_state.m_currentRenderTarget        = NULL;
	m_state.m_currentDepthStencilSurface = NULL;
	return 0;
}

int ssgi::GraphicsContextImpl::setComputeProgram(ssg::ComputeProgram *computeProgram)
{
	ssgi::ComputeProgramImpl *ci = dynamic_cast<ssgi::ComputeProgramImpl*>(computeProgram);
	m_pDeviceContext->CSSetShader(ci->m_computeShader, NULL, 0);
	return SCE_OK;
}

int ssgi::GraphicsContextImpl::dispatch(uint32_t threadGroupX,uint32_t threadGroupY,uint32_t threadGroupZ)
{
	m_pDeviceContext->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
	return SCE_OK;
}



int ssg::createGraphicsContext(GraphicsContext **outGraphicsContext, uint32_t width, uint32_t height,
							   GraphicsContextOption* option)
{
	ssgi::GraphicsContextImpl *c = new ssgi::GraphicsContextImpl();

	int ret = c->initialize(width, height, option);
	if(ret < SCE_OK){
		return ret;
	}
	*outGraphicsContext = c;
	return ret;
}


void *ssgi::GraphicsContextImpl::reserveDefaultUniformBuffer(ssg::ProgramType programType)
{
	if(programType == kProgramTypeVertex){
		if(m_state.m_vertexProgram == NULL){
			return NULL;
		}
		m_reservedVertexUniformBuffer = m_state.m_vertexProgram->m_programId->getDefaultUniformBuffer();
		m_gpuBufferState.setUniformBuffer(kProgramTypeVertex, m_reservedVertexUniformBuffer, 0);
		void *ret = (m_reservedVertexUniformBuffer == NULL) ? NULL : m_reservedVertexUniformBuffer->beginWrite();
		return ret;
	}else{
		if(m_state.m_fragmentProgram == NULL){
			return NULL;
		}
		m_reservedFragmentUniformBuffer = m_state.m_fragmentProgram->m_programId->getDefaultUniformBuffer();
		m_gpuBufferState.setUniformBuffer(kProgramTypeFragment, m_reservedFragmentUniformBuffer, 0);
		return (m_reservedFragmentUniformBuffer == NULL) ? NULL : m_reservedFragmentUniformBuffer->beginWrite();
	}
}


int ssgi::GraphicsContextImpl::readBuffer(void *dst, const ssg::BufferInterface* buffer, uint64_t offset, uint64_t numBytes)
{
	if(dst == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(buffer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

#if 1
	const RwStructuredBufferImpl *bufferImpl = dynamic_cast<const RwStructuredBufferImpl *>(buffer);
	ID3D11Resource *resource = bufferImpl->getBuffer();
#else
	BufferImpl *bufferImpl = dynamic_cast<BufferImpl *>(buffer);
	if(bufferImpl == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	ID3D11Resource *resource = bufferImpl->m_buffer;
#endif
	ID3D11Device *device = getDevice();



	D3D11_RESOURCE_DIMENSION dim;
	resource->GetType(&dim);

	switch(dim)
	{
	case D3D11_RESOURCE_DIMENSION_BUFFER:
		{
			ID3D11Buffer *buf = (ID3D11Buffer *)resource;
			D3D11_BUFFER_DESC desc;
			buf->GetDesc(&desc);
			desc.Usage          = D3D11_USAGE_STAGING;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.BindFlags      = 0;
			desc.MiscFlags      = 0;
			ID3D11Buffer *readBuffer;
			HRESULT hr = device->CreateBuffer(&desc, NULL, &readBuffer);
			if( FAILED(hr) ){
				SCE_SAMPLE_UTIL_ASSERT(0);
				return -1;
			}

			m_pDeviceContext->CopyResource(readBuffer, resource);

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			hr = m_pDeviceContext->Map(readBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
			if( FAILED(hr) ){
				SCE_SAMPLE_UTIL_ASSERT(0);
				readBuffer->Release();
				return -1;
			}
			void *data = mappedResource.pData;
			uint8_t *dst8 = (uint8_t*)dst;
			memcpy(dst8 + offset, data, numBytes); // todo: size check.
			m_pDeviceContext->Unmap(readBuffer, 0);
			readBuffer->Release();
			break;
		}
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		{
			ID3D11Texture2D *tex = dynamic_cast<ID3D11Texture2D *>(resource);
			D3D11_TEXTURE2D_DESC desc;
			tex->GetDesc(&desc);
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

			ID3D11Texture2D *tex2D;
			HRESULT hr = device->CreateTexture2D(&desc, NULL, &tex2D);
			if( FAILED(hr) ){
				return -1;
			}
			m_pDeviceContext->CopyResource(tex2D, resource);

			D3D11_MAPPED_SUBRESOURCE mappedResource;
			hr = m_pDeviceContext->Map(tex2D, 0, D3D11_MAP_READ, 0, &mappedResource);
			if( FAILED(hr) ){
				tex2D->Release();
				return -1;
			}
			void *data = mappedResource.pData;
			uint32_t size = mappedResource.DepthPitch * mappedResource.RowPitch; // TODO:
			memcpy(dst, data, size);
			m_pDeviceContext->Unmap(tex2D, 0);
			tex2D->Release();
			break;
		}
		// unsupported yet.
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
	default:
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;

	}
	return SCE_OK;
}


#endif /* _SCE_TARGET_OS_WINDOWS */
