/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"
#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS
#include "util/util.h"
#include "program_internal_dx11.h"
#include "loader_internal_dx11.h"
#include "constant_internal_dx11.h"

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace util = sce::SampleUtil::Internal::Util;

#pragma comment( lib, "cg.lib" )
#pragma comment( lib, "cgD3D11.lib" )
#pragma comment( lib, "DXGUID.LIB" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "d3dcompiler.lib")


int ssgi::GraphicsLoaderImpl::initialize(void)
{
	return initialize(NULL);
}


int ssgi::GraphicsLoaderImpl::initialize(ID3D11Device *device,
										   CGprofile cgVertexProfile,
										   CGprofile cgFragmentProfile)
{
	m_device = device;
	m_cgContext=  cgCreateContext();
	//cgSetParameterSettingMode(m_cgContext, CG_DEFERRED_PARAMETER_SETTING );
	cgSetParameterSettingMode(m_cgContext, CG_IMMEDIATE_PARAMETER_SETTING );
	HRESULT hr = cgD3D11SetDevice( m_cgContext, device );
	util::checkForCgError( "setting Direct3D device", false );
	if( hr != S_OK ){
		return hr;
	}

	m_cgVertexProfile = cgVertexProfile;
	m_cgFragmentProfile = cgFragmentProfile;
	return SCE_OK;
}
int ssgi::GraphicsLoaderImpl::finalize(void)
{
	cgD3D11SetDevice( m_cgContext, NULL );
	cgDestroyContext( m_cgContext );
	return 0;

}

int ssgi::GraphicsLoaderImpl::registerVertexProgram(ssg::Program** outProgram, const void *program, uint32_t size, 
													ssg::ShaderFormat shaderFormat)
{
	if(shaderFormat == ssg::kShaderFormatDefault){
		shaderFormat = ssg::kShaderFormatCgBinary;
	}
	
	CGprogram cgProg = NULL;
	ID3DBlob *blob   = NULL;
	switch(shaderFormat)
		{
		case kShaderFormatCgBinary:
			{
				cgProg = cgCreateProgram(
										m_cgContext,              /* Cg runtime context */
										CG_OBJECT,  
										(const char*) program,    /* Name of file containing program */
										m_cgVertexProfile,        /* Profile */
										NULL,                     /* Entry function name */
										NULL);                    /* Pass optimal compiler options */
				util::checkForCgError("creating vertex program from file");
				
				HRESULT result = cgD3D11LoadProgram( cgProg, 0 );
				SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED(result));
				util::checkForCgError("loading vertex program");
				blob = cgD3D11GetCompiledProgram(cgProg );
				break;
			}
		case kShaderFormatCgSource:
			{
				cgProg = cgCreateProgram(
										m_cgContext,              /* Cg runtime context */
										CG_SOURCE,  
										(const char*) program,    /* Name of file containing program */
										m_cgVertexProfile,        /* Profile */
										NULL,                     /* Entry function name */
										NULL);                    /* Pass optimal compiler options */
				util::checkForCgError("creating vertex program from file");
				
				HRESULT result = cgD3D11LoadProgram( cgProg, 0 );
				SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED(result));
				util::checkForCgError("loading vertex program");
				blob = cgD3D11GetCompiledProgram(cgProg );
				break;

			}
			
		case kShaderFormatHlslSource:
			{
				
				HRESULT hr = D3DCompile(
					(const char*)program,//LPCVOID pSrcData,
					size,//SIZE_T SrcDataSize,
					NULL, //LPCSTR pSourceName,
					NULL, //CONST D3D10_SHADER_MACRO *pDefines,
					NULL, //LPD3D10INCLUDE pInclude,
					"main", //LPCSTR pEntrypoint,
					"vs_4_0", //LPCSTR pTarget,
					0, //UINT Flags1,
					0, //UINT Flags2,
					&blob, //LPD3D10BLOB *ppCode,
					NULL//LPD3D10BLOB *ppErrorMsgs
					);
				if(hr != S_OK){
					return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
				}
				break;
			}
		default:
			{
				return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
			}
		}

	

	*outProgram = new ProgramImpl(this, cgProg, blob);
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::registerFragmentProgram(ssg::Program** outProgram, const void *program, uint32_t size, 
													  ssg::ShaderFormat shaderFormat)
{
	if(shaderFormat == ssg::kShaderFormatDefault){
		shaderFormat = ssg::kShaderFormatCgBinary;
	}
	
	CGprogram cgProg = NULL;
	ID3DBlob *blob   = NULL;
	switch(shaderFormat)
		{
		case kShaderFormatCgBinary:
			{
				cgProg = cgCreateProgram(
										m_cgContext,              /* Cg runtime context */
										CG_OBJECT,  
										(const char*) program,    /* Name of file containing program */
										m_cgFragmentProfile,        /* Profile */
										NULL,                     /* Entry function name */
										NULL);                    /* Pass optimal compiler options */
				util::checkForCgError("creating fragment program from file");
				
				HRESULT result = cgD3D11LoadProgram( cgProg, 0 );
				SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED(result));
				util::checkForCgError("loading fragment program");
				blob = cgD3D11GetCompiledProgram(cgProg );
				break;
			}
		case kShaderFormatCgSource:
			{
				cgProg = cgCreateProgram(
										m_cgContext,              /* Cg runtime context */
										CG_SOURCE,  
										(const char*) program,    /* Name of file containing program */
										m_cgFragmentProfile,        /* Profile */
										NULL,                     /* Entry function name */
										NULL);                    /* Pass optimal compiler options */
				util::checkForCgError("creating fragment program from file");
				
				HRESULT result = cgD3D11LoadProgram( cgProg, 0 );
				SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED(result));
				util::checkForCgError("loading fragment program");
				blob = cgD3D11GetCompiledProgram(cgProg );
				break;
			}
			
		case kShaderFormatHlslSource:
			{
				HRESULT hr = D3DCompile(
					(const char*)program,//LPCVOID pSrcData,
					size,//SIZE_T SrcDataSize,
					NULL, //LPCSTR pSourceName,
					NULL, //CONST D3D10_SHADER_MACRO *pDefines,
					NULL, //LPD3D10INCLUDE pInclude,
					"main", //LPCSTR pEntrypoint,
					"ps_4_0", //LPCSTR pTarget,
					0, //UINT Flags1,
					0, //UINT Flags2,
					&blob, //LPD3D10BLOB *ppCode,
					NULL//LPD3D10BLOB *ppErrorMsgs
					);
				if(hr != S_OK){
					return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
				}
				break;
			}
		default:
			{
				return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
			}
		}

	

	*outProgram = new ProgramImpl(this, cgProg, blob);
	return SCE_OK;
}


int ssgi::GraphicsLoaderImpl::registerComputeProgram(ssg::Program** outProgram, const void *program, uint32_t size, 
													  ssg::ShaderFormat shaderFormat)
{
	if(shaderFormat == ssg::kShaderFormatDefault){
		shaderFormat = ssg::kShaderFormatHlslSource;
	}
	
	CGprogram cgProg = NULL;
	ID3DBlob *blob   = NULL;
	switch(shaderFormat)
		{
		case kShaderFormatCgBinary:
			{
				return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
			}
		case kShaderFormatCgSource:
			{
				return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
			}
			
		case kShaderFormatHlslSource:
			{
				HRESULT hr = D3DCompile(
					(const char*)program,//LPCVOID pSrcData,
					size,//SIZE_T SrcDataSize,
					NULL, //LPCSTR pSourceName,
					NULL, //CONST D3D10_SHADER_MACRO *pDefines,
					NULL, //LPD3D10INCLUDE pInclude,
					"main", //LPCSTR pEntrypoint,
					"cs_4_0", //LPCSTR pTarget,
					0, //UINT Flags1,
					0, //UINT Flags2,
					&blob, //LPD3D10BLOB *ppCode,
					NULL//LPD3D10BLOB *ppErrorMsgs
					);

				if(hr != S_OK){
					printf("hr = %#x\n", hr);
					return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
				}
				break;
			}
		default:
			{
				return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
			}
		}

	

	*outProgram = new ProgramImpl(this, cgProg, blob);
	return SCE_OK;
}

ssg::Program* ssgi::GraphicsLoaderImpl::registerVertexProgramFromSourceFile(const char *path, const char *entry)
{
	const char ** profileOpts = NULL;

	profileOpts = cgD3D11GetOptimalOptions(m_cgVertexProfile);
	util::checkForCgError("getting latest profile options");

	CGprogram myCgVertexProgram =
		cgCreateProgramFromFile(
		m_cgContext,              /* Cg runtime context */
		CG_SOURCE,                /* Program in human-readable form */
		path,                     /* Name of file containing program */
		m_cgVertexProfile,        /* Profile */
		entry,                    /* Entry function name */
		profileOpts);             /* Pass optimal compiler options */
	util::checkForCgError("creating vertex program from file");

	HRESULT result = cgD3D11LoadProgram( myCgVertexProgram, 0 );
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED(result));
	util::checkForCgError("loading vertex program");

	return new ProgramImpl(this, myCgVertexProgram, cgD3D11GetCompiledProgram( myCgVertexProgram ));
}

ssg::Program* ssgi::GraphicsLoaderImpl::registerFragmentProgramFromSourceFile(const char *path, const char *entry)
{
	const char ** profileOpts = NULL;

	profileOpts = cgD3D11GetOptimalOptions(m_cgFragmentProfile);
	util::checkForCgError("getting latest profile options");

	CGprogram myCgFragmentProgram = 
		cgCreateProgramFromFile(
		m_cgContext,                /* Cg runtime context */
		CG_SOURCE,                  /* Program in human-readable form */
		path,                       /* Name of file containing program */
		m_cgFragmentProfile,        /* Profile */
		entry,                      /* Entry function name */
		profileOpts);               /* Pass optimal compiler options */
	util::checkForCgError("creating fragment program from file");

	cgD3D11LoadProgram( myCgFragmentProgram, 0 );
	util::checkForCgError("loading fragment program");

	return new ProgramImpl(this, myCgFragmentProgram, cgD3D11GetCompiledProgram( myCgFragmentProgram ));
}

ssg::Program* ssgi::GraphicsLoaderImpl::registerVertexProgramFromSource(const char *program, const char *entry)
{
	const char ** profileOpts = NULL;

	profileOpts = cgD3D11GetOptimalOptions(m_cgVertexProfile);
	util::checkForCgError("getting latest profile options");

	CGprogram myCgVertexProgram =
		cgCreateProgram(
		m_cgContext,              /* Cg runtime context */
		CG_SOURCE,                /* Program in human-readable form */
		program,                  /* Name of file containing program */
		m_cgVertexProfile,        /* Profile */
		entry,                    /* Entry function name */
		profileOpts);             /* Pass optimal compiler options */
	util::checkForCgError("creating vertex program from file");

	HRESULT result = cgD3D11LoadProgram( myCgVertexProgram, 0 );
	SCE_SAMPLE_UTIL_ASSERT(SUCCEEDED(result));
	util::checkForCgError("loading vertex program");

	return new ProgramImpl(this, myCgVertexProgram, cgD3D11GetCompiledProgram( myCgVertexProgram ));
}

ssg::Program* ssgi::GraphicsLoaderImpl::registerFragmentProgramFromSource(const char *program, const char *entry)
{
	const char ** profileOpts = NULL;

	profileOpts = cgD3D11GetOptimalOptions(m_cgFragmentProfile);
	util::checkForCgError("getting latest profile options");

	CGprogram myCgFragmentProgram = 
		cgCreateProgram(
		m_cgContext,                /* Cg runtime context */
		CG_SOURCE,                  /* Program in human-readable form */
		program,                    /* Name of file containing program */
		m_cgFragmentProfile,        /* Profile */
		entry,                      /* Entry function name */
		profileOpts);               /* Pass optimal compiler options */
	util::checkForCgError("creating fragment program from file");

	cgD3D11LoadProgram( myCgFragmentProgram, 0 );
	util::checkForCgError("loading fragment program");

	return new ProgramImpl(this, myCgFragmentProgram, cgD3D11GetCompiledProgram( myCgFragmentProgram ));
}



ssg::Program* ssgi::GraphicsLoaderImpl::registerHlslComputetProgramFromSource(SCE_SAMPLE_LPCTSTR program, const char* entry)
{
	ID3D10Blob *pShader;
	wchar_t *wstr = new wchar_t[strlen(program)+1];
	size_t chars = 0;
	mbstowcs_s(&chars, wstr, strlen(program)+1, program, _TRUNCATE);
	HRESULT hr = D3DCompileFromFile(
		wstr, //in      LPCWSTR pFileName,
		NULL, //in_opt  const D3D_SHADER_MACRO pDefines,
		NULL, //in_opt  ID3DInclude pInclude,
		entry, //in      LPCSTR pEntrypoint,
		"cs_5_0", //in      LPCSTR pTarget,
		0, //in      UINT Flags1,
		0, //in      UINT Flags2,
		&pShader, //out     ID3DBlob ppCode,
		NULL//out_opt ID3DBlob ppErrorMsgs
		);
	delete [] wstr;

	ID3D11ComputeShader *computeShader;
	m_device->CreateComputeShader( pShader->GetBufferPointer(),
								   pShader->GetBufferSize(), 
								   NULL, //g_pPSClassLinkage, 
								   &computeShader );  
	ID3D11ShaderReflection* pReflector = NULL; 
	D3DReflect( pShader->GetBufferPointer(), 
				pShader->GetBufferSize(),
				IID_ID3D11ShaderReflection, 
				(void**) &pReflector);     
	
	return SCE_OK;
}





static DXGI_FORMAT getFormat(ssg::AttributeFormat format,uint32_t componentCount)
{
	switch(format)
	{
	case ssg::kAttributeFormatU8: 
		switch(componentCount)
		{
		case 1: 	return DXGI_FORMAT_R8_UINT;
		case 2:		return DXGI_FORMAT_R8G8_UINT;
		case 4:		return DXGI_FORMAT_R8G8B8A8_UINT;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	case ssg::kAttributeFormatS8 : 
		switch(componentCount)
		{
		case 1: 	return DXGI_FORMAT_R8_SINT;
		case 2:		return DXGI_FORMAT_R8G8_SINT;
		case 4:		return DXGI_FORMAT_R8G8B8A8_SINT;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	case ssg::kAttributeFormatU16:	
		switch(componentCount)
		{
		case 1: 	return DXGI_FORMAT_R16_UINT;
		case 2:		return DXGI_FORMAT_R16G16_UINT;
		case 4:		return DXGI_FORMAT_R16G16B16A16_UINT;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	case ssg::kAttributeFormatS16 :
		switch(componentCount)
		{
		case 1: 	return DXGI_FORMAT_R16_SINT;
		case 2:		return DXGI_FORMAT_R16G16_SINT;
		case 4:		return DXGI_FORMAT_R16G16B16A16_SINT;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	case ssg::kAttributeFormatU8N :
		switch(componentCount)
		{
		case 1: 	return DXGI_FORMAT_R8_UNORM;
		case 2:		return DXGI_FORMAT_R8G8_UNORM;
		case 4:		return DXGI_FORMAT_R8G8B8A8_UNORM;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	case ssg::kAttributeFormatS8N :
		switch(componentCount)
		{
		case 1: 	return DXGI_FORMAT_R8_SNORM;
		case 2:		return DXGI_FORMAT_R8G8_SNORM;
		case 4:		return DXGI_FORMAT_R8G8B8A8_SNORM;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	case ssg::kAttributeFormatU16N:
		switch(componentCount)
		{
		case 1: 	return DXGI_FORMAT_R16_UNORM;
		case 2:		return DXGI_FORMAT_R16G16_UNORM;
		case 4:		return DXGI_FORMAT_R16G16B16A16_UNORM;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	case ssg::kAttributeFormatS16N:
		switch(componentCount)
		{
		case 1: 	return DXGI_FORMAT_R16_SNORM;
		case 2:		return DXGI_FORMAT_R16G16_SNORM;
		case 4:		return DXGI_FORMAT_R16G16B16A16_SNORM;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	case ssg::kAttributeFormatF16 :
		switch(componentCount)
		{
		case 1:		return DXGI_FORMAT_R16_FLOAT;
		case 2:		return DXGI_FORMAT_R16G16_FLOAT;
		//case 3:		return DXGI_FORMAT_R16G16B16_FLOAT;
		case 4:		return DXGI_FORMAT_R16G16B16A16_FLOAT;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	case ssg::kAttributeFormatF32:	
		switch(componentCount)
		{
		case 1:		return DXGI_FORMAT_R32_FLOAT;
		case 2:		return DXGI_FORMAT_R32G32_FLOAT;
		case 3:		return DXGI_FORMAT_R32G32B32_FLOAT;
		case 4:		return DXGI_FORMAT_R32G32B32A32_FLOAT;
		default:	return DXGI_FORMAT_FORCE_UINT;
		};
	default: return DXGI_FORMAT_FORCE_UINT;
	}
}

static const char *getSemanticName(ssg::ParameterSemantic vattrSemantic)
{
	switch (vattrSemantic)
	{
	case ssg::kParameterSemanticNone         : return "NONE";
	case ssg::kParameterSemanticAttr         : return  "ATTR";        
	case ssg::kParameterSemanticBcol         : return  "BCOL";
	case ssg::kParameterSemanticBinormal     : return  "BINORMAL";
	case ssg::kParameterSemanticBlendindices : return  "BLENDINDICES";
	case ssg::kParameterSemanticBlendweight  : return  "BLENDWEIGHT";
	case ssg::kParameterSemanticColor        : return  "COLOR";
	case ssg::kParameterSemanticDiffuse      : return  "DIFFUSE";
	case ssg::kParameterSemanticFogcoord     : return  "FOGCOORD";
	case ssg::kParameterSemanticNormal       : return  "NORMAL";
	case ssg::kParameterSemanticPointsize    : return  "POINTSIZE";
	case ssg::kParameterSemanticPosition     : return  "POSITION";
	case ssg::kParameterSemanticSpecular     : return  "SPECULAR";
	case ssg::kParameterSemanticTangent      : return  "TANGENT";
	case ssg::kParameterSemanticTexcoord     : return  "TEXCOORD";
	default:		return "NONE";

	}
}



static D3D11_INPUT_ELEMENT_DESC convertAttr(const ssg::VertexAttribute &attr, const ssg::VertexStream *streams, uint32_t numStreams)
{

	//{ "POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, positionOffset,  D3D11_INPUT_PER_VERTEX_DATA, 0 },        
	//printf("slot=%d, offset=%d, format=%#x, count=%d\n",  attr.streamIndex, attr.offset, attr.format, attr.componentCount);

	D3D11_INPUT_ELEMENT_DESC desc;
	desc.SemanticName         = getSemanticName(attr.semantic);
	desc.SemanticIndex        = attr.semanticIndex;
	desc.Format               = getFormat(attr.format, attr.componentCount);
	if(desc.Format == DXGI_FORMAT_FORCE_UINT){
		desc.Format =DXGI_FORMAT_R32_FLOAT;// TODO work around;
	}
	desc.InputSlot            = attr.streamIndex;
	desc.AlignedByteOffset    = attr.offset;

	ssg::VertexStream strm = streams[attr.streamIndex];
	if((strm.indexSource == ssg::kIndexSourceIndex16Bit) || (strm.indexSource == ssg::kIndexSourceIndex32Bit)){
		desc.InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;
	}else{
		desc.InputSlotClass       = D3D11_INPUT_PER_INSTANCE_DATA;
		desc.InstanceDataStepRate = 1;
	}
	return desc;
}


int ssgi::GraphicsLoaderImpl::createVertexProgram(ssg::VertexProgram **_vertexProgram, const ssg::Program* _programId, 
											  const ssg::VertexAttribute *attrs, uint32_t numAttrs,
											  const ssg::VertexStream *streams, uint32_t numStreams)
{
	uint16_t *streamStrides = (uint16_t*)_malloca(sizeof(uint16_t)*numStreams);
	for(uint32_t i=0; i<numStreams; i++){
		streamStrides[i] = streams[i].stride;
	}

	D3D11_INPUT_ELEMENT_DESC *descs = (D3D11_INPUT_ELEMENT_DESC *)_malloca(sizeof(D3D11_INPUT_ELEMENT_DESC) * numAttrs);
	//D3D11_INPUT_ELEMENT_DESC *descs = (D3D11_INPUT_ELEMENT_DESC *)malloc(sizeof(D3D11_INPUT_ELEMENT_DESC) * numAttrs);
	for(uint32_t i=0; i<numAttrs; i++){
		descs[i] = convertAttr(attrs[i], streams, numStreams);
	}

	const Program *programId = _programId;

	ID3D10Blob * pVSBuf = dynamic_cast<const ssgi::ProgramImpl*>(programId)->m_pBlob;
	SCE_SAMPLE_UTIL_ASSERT(pVSBuf != NULL);
/*
	printf("create input layout %p, %d\n", pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize());
	printf("numAttrs=%d, p=%#x, size=%#x\n" , numAttrs, (uint32_t)pVSBuf->GetBufferPointer(), (uint32_t)pVSBuf->GetBufferSize());
	for(uint32_t i=0; i<numAttrs; i++){
		printf("---%d\n",i);
		printf("%s\n", descs[i].SemanticName);
		printf("%#x\n",  descs[i].SemanticIndex);
		printf("%#x\n",  descs[i].Format);
		printf("%#x\n",  descs[i].InputSlot);
		printf("%#x\n",  descs[i].AlignedByteOffset);
		printf("%#x\n",  descs[i].InputSlotClass);
		printf("%#x\n",  descs[i].InstanceDataStepRate);
	}
	*/
	ID3D11InputLayout *vlayout;
	HRESULT hr = m_device->CreateInputLayout( descs, numAttrs, pVSBuf->GetBufferPointer(), pVSBuf->GetBufferSize(), &vlayout );
	if(FAILED(hr)){
		printf("hr = %#x\n" , hr);
		printf("numAttrs=%d, p=%#x, size=%#x\n" , numAttrs, (uint32_t)pVSBuf->GetBufferPointer(), (uint32_t)pVSBuf->GetBufferSize());
		for(uint32_t i=0; i<numAttrs; i++){
			printf("%s\n", descs[i].SemanticName);
		}

		SCE_SAMPLE_UTIL_ASSERT(hr == SCE_OK);
		return -1;
	}

	ssgi::VertexProgramImpl *vertexProgram = new ssgi::VertexProgramImpl;

	vertexProgram->m_layout = vlayout;
	vertexProgram->m_programId = dynamic_cast<const ssgi::ProgramImpl*>(programId);

	hr = m_device->CreateVertexShader(
		vertexProgram->m_programId->m_pBlob->GetBufferPointer(),
		vertexProgram->m_programId->m_pBlob->GetBufferSize(),
		NULL,
		&vertexProgram->m_vertexShader);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(hr, SCE_OK);

	memset(vertexProgram->strides, 0, sizeof(vertexProgram->strides));
	memset(vertexProgram->offsets, 0, sizeof(vertexProgram->offsets));
	SCE_SAMPLE_UTIL_ASSERT(numStreams <= D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT);
	for(uint32_t i=0; i<numStreams; i++){
		vertexProgram->strides[i] = streamStrides[i];
	}
	vertexProgram->numStreams = numStreams;

	*_vertexProgram = vertexProgram;
	return SCE_OK;

}

int ssgi::GraphicsLoaderImpl::createFragmentProgram(ssg::FragmentProgram **_fragmentProgram, 
											   const ssg::Program* programId, 
											   const ssg::Program* vertexProgramId,
											   const ssg::BlendInfo *blendInfo)
{

	ssgi::FragmentProgramImpl *fragmentProgram = new ssgi::FragmentProgramImpl;

	fragmentProgram->m_programId = dynamic_cast<const ssgi::ProgramImpl*>(programId);

	HRESULT hr = m_device->CreatePixelShader(
		fragmentProgram->m_programId->m_pBlob->GetBufferPointer(),
		fragmentProgram->m_programId->m_pBlob->GetBufferSize(),
		NULL,
		&fragmentProgram->m_pixelShader);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(hr, SCE_OK);

	util::checkForCgError("loading fragment program");

	if(blendInfo != NULL){
		D3D11_BLEND_DESC bi = ssgi::convertBlendInfo(*blendInfo);
		m_device->CreateBlendState(&bi,  &fragmentProgram->m_blendState);
	}else{
		m_device->CreateBlendState(NULL,  &fragmentProgram->m_blendState);
	}
	*_fragmentProgram = fragmentProgram;
	return 0;
}

int ssgi::GraphicsLoaderImpl::createComputeProgram(ssg::ComputeProgram **_computeProgram, 
											       const ssg::Program* programId)
{

	ssgi::ComputeProgramImpl *computeProgram = new ssgi::ComputeProgramImpl;

	computeProgram->m_programId = dynamic_cast<const ssgi::ProgramImpl*>(programId);
	
	HRESULT hr = m_device->CreateComputeShader(
		computeProgram->m_programId->m_pBlob->GetBufferPointer(),
		computeProgram->m_programId->m_pBlob->GetBufferSize(),
		NULL,
		&computeProgram->m_computeShader);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(hr, SCE_OK);

	util::checkForCgError("loading fragment program");

	*_computeProgram = computeProgram;
	return 0;
}


int ssgi::GraphicsLoaderImpl::cloneVertexProgram(ssg::VertexProgram **vertexProgram, const ssg::VertexProgram *_from)
{
	const ssgi::VertexProgramImpl *from = dynamic_cast<const ssgi::VertexProgramImpl *>(_from);
	ssgi::VertexProgramImpl *v =	new ssgi::VertexProgramImpl;
	*v = *from;
	v->m_layout->AddRef();
	v->m_vertexShader->AddRef();
	*vertexProgram = v;

	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::cloneFragmentProgram(ssg::FragmentProgram **fragmentProgram, const ssg::FragmentProgram *_from)
{
	const ssgi::FragmentProgramImpl *from = dynamic_cast<const ssgi::FragmentProgramImpl *>(_from);
	ssgi::FragmentProgramImpl *f = new ssgi::FragmentProgramImpl;
	*f = *from;
	f->m_blendState->AddRef();
	f->m_pixelShader->AddRef();	
	*fragmentProgram = f;
	return SCE_OK;
}




ID3D11Device *ssgi::GraphicsLoaderImpl::getDevice(void) const
{
	return m_device;
}


int ssg::Dx11::createGraphicsLoader(GraphicsLoader **outGraphicsLoader, 
							  ID3D11Device *device,
							  CGprofile cgVertexProfile,
							  CGprofile cgFragmentProfile)
{
	ssgi::GraphicsLoaderImpl *loader = new ssgi::GraphicsLoaderImpl();
	int ret = loader->initialize(device,
							  cgVertexProfile,
							  cgFragmentProfile);
	if(ret != SCE_OK){
		delete loader;
		return ret;
	}
	*outGraphicsLoader = loader;
	return SCE_OK;

}




#endif /* _SCE_TARGET_OS_WINDOWS */
