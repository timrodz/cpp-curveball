/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS

#include <kernel.h>
#include <sampleutil/graphics.h>
#include "util/util.h"
#include "constant_internal_gnm.h"
#include "loader_internal_gnm.h"
#include <libsysmodule.h>
//#include <texture_tool.h>

#pragma comment(lib,"libSceShaderBinary.a")
#pragma comment(lib,"libSceTextureTool.a")
#pragma comment(lib,"libSceSysmodule_stub_weak.a")

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

/*
static void mapMemory(void** system, uint32_t* systemSize, void** gpuShared, uint32_t* gpuSharedSize)
{
	SCE_GNM_ASSERT(system && systemSize && gpuShared && gpuSharedSize);
	//----------------------------------------------------------------------
	// Setup the memory pools:
#ifdef GNM_EARLY_LIMITATION
	const uint32_t	systemPoolSize	   = 1024 * 1024 * 256;
	const uint32_t	sharedGpuPoolSize  = 1024 * 1024 * 512;
#else // GNM_EARLY_LIMITATION
	const uint32_t	systemPoolSize	   = 1024 * 1024 * 1024;
	const uint32_t	sharedGpuPoolSize  = 1024 * 1024 * 192;
#endif // GNM_EARLY_LIMITATION

	const uint32_t  systemAlignment		= 2 * 1024 * 1024;
	const uint32_t  shaderGpuAlignment	= 2 * 1024 * 1024;

	off_t systemPoolOffset;		// TODO: store these value to be able to free them later.
	off_t sharedGpuPoolOffset;
	
	void*	systemPoolPtr	  = NULL;
	void*	sharedGpuPoolPtr  = NULL;

	uint32_t result = SCE_GNM_OK;

	int retSys = sceKernelAllocateDirectMemory(SCE_KERNEL_MAIN_DMEM_OFFSET,
		(SCE_KERNEL_MAIN_DMEM_OFFSET +
		SCE_KERNEL_MAIN_DMEM_SIZE),
		systemPoolSize,
		systemAlignment, // alignment
		SCE_KERNEL_WB_ONION_NONVOLATILE,
		&systemPoolOffset);
	if ( !retSys )
	{
		retSys = sceKernelMapDirectMemory(&systemPoolPtr,
			systemPoolSize,
			SCE_KERNEL_PROT_CPU_READ|SCE_KERNEL_PROT_CPU_WRITE|SCE_KERNEL_PROT_GPU_ALL,
			0,						//flags
			systemPoolOffset,
			systemAlignment);
	}
	if ( retSys )
	{
		fprintf(stderr, __FILE__"(%d) System Memory Allocation Failed (%d)\n", __LINE__, retSys);
		result = SCE_GNM_ERROR_FAILURE;
	}


	//


	int retShr = sceKernelAllocateDirectMemory(SCE_KERNEL_MAIN_DMEM_OFFSET,
		(SCE_KERNEL_MAIN_DMEM_OFFSET +
		SCE_KERNEL_MAIN_DMEM_SIZE),
		sharedGpuPoolSize,
		shaderGpuAlignment,	// alignment
		//SCE_KERNEL_WB_ONION_NONVOLATILE,
		SCE_KERNEL_UC_GARLIC_NONVOLATILE,
		&sharedGpuPoolOffset);

	if ( !retShr )
	{
		retShr = sceKernelMapDirectMemory(&sharedGpuPoolPtr,
			sharedGpuPoolSize,
			SCE_KERNEL_PROT_CPU_READ|SCE_KERNEL_PROT_CPU_WRITE|SCE_KERNEL_PROT_GPU_ALL,
			0,						//flags
			sharedGpuPoolOffset,
			shaderGpuAlignment);
	}
	if ( retShr )
	{
		fprintf(stderr, "sce::Gnm::Initialize Error: Shared Gpu Memory Allocation Failed (%d)\n", retShr);
		result = SCE_GNM_ERROR_FAILURE;
	}
	//

	if ( result == SCE_GNM_OK )
	{
		
		*gpuShared =  sharedGpuPoolPtr;
		*gpuSharedSize    = sharedGpuPoolSize;

		*system =        systemPoolPtr;
		*systemSize    = systemPoolSize;
	}
	
#ifndef DISABLE_MEMORY_CLEAR
	// clear all shared memory (including the memory we just used to clear private mem)
	// We clear to 0x7n because 0x7n7n7n7n is an obviously-invalid (Type-1) PM4 packet
	//memset(sce::Gnm::translateGpuToCpuAddress(system), 0x7d, systemSize);
	//memset(sce::Gnm::translateGpuToCpuAddress(gpuShared),  0x11, gpuSharedSize);
#endif // DISABLE_MEMORY_CLEAR

	//fprintf(stderr,"Shared System size    : 0x%010lX\n", static_cast<uint64_t>(systemPoolSize));
	//fprintf(stderr,"Shared System CPU Addr: 0x%010lX\n", uintptr_t(system));
	//fprintf(stderr,"Shared Video size     : 0x%010lX\n", static_cast<uint64_t>(sharedGpuPoolSize));
	//fprintf(stderr,"Shared Video CPU Addr : 0x%010lX\n", uintptr_t(gpuShared));
}
*/

ssgi::GraphicsLoaderImpl::MemoryBlock::MemoryBlock(void)
{
	m_systemPoolPtr = NULL;
	m_systemPoolSize = 0;
	m_systemPoolOffset = 0;
	
	m_gpuSharedPtr = NULL;
	m_gpuSharedSize = 0;
	m_gpuSharedOffset = 0;
}

int ssgi::GraphicsLoaderImpl::MemoryBlock::initialize(size_t systemHeapSize, size_t videoHeapSize)
{
	int ret;
	//----------------------------------------------------------------------
/*
	// Setup the memory pools:
#ifdef GNM_EARLY_LIMITATION
	m_systemPoolSize	   = 1024 * 1024 * 256;
	m_gpuSharedSize  = 1024 * 1024 * 512;
#else // GNM_EARLY_LIMITATION
	m_systemPoolSize	   = 1024 * 1024 * 1024;
	m_gpuSharedSize  = 1024 * 1024 * 1024;
#endif // GNM_EARLY_LIMITATION
	*/
	m_systemPoolSize   = systemHeapSize;
	m_gpuSharedSize    = videoHeapSize;

	const uint32_t  systemAlignment		= 2 * 1024 * 1024;
	const uint32_t  shaderGpuAlignment	= 2 * 1024 * 1024;

	ret = sceKernelAllocateDirectMemory(0,
		SCE_KERNEL_MAIN_DMEM_SIZE,
		m_systemPoolSize,
		systemAlignment, // alignment
		SCE_KERNEL_WB_ONION,
		&m_systemPoolOffset);
	if(ret != SCE_OK){
		finalize();
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}

	ret = sceKernelMapDirectMemory(&m_systemPoolPtr,
		m_systemPoolSize,
		SCE_KERNEL_PROT_CPU_READ|SCE_KERNEL_PROT_CPU_WRITE|SCE_KERNEL_PROT_GPU_ALL,
		0,						//flags
		m_systemPoolOffset,
		systemAlignment);
	if(ret != SCE_OK){
		finalize();
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	//

	ret = sceKernelAllocateDirectMemory(0,
		SCE_KERNEL_MAIN_DMEM_SIZE,
		m_gpuSharedSize,
		shaderGpuAlignment,	// alignment
		SCE_KERNEL_WC_GARLIC,
		&m_gpuSharedOffset);
	if(ret != SCE_OK){
		finalize();
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	ret = sceKernelMapDirectMemory(&m_gpuSharedPtr,
			m_gpuSharedSize,
			SCE_KERNEL_PROT_CPU_READ|SCE_KERNEL_PROT_CPU_WRITE|SCE_KERNEL_PROT_GPU_ALL,
			0,						//flags
			m_gpuSharedOffset,
			shaderGpuAlignment);
	if(ret != SCE_OK){
		finalize();
		return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
	}
	//

#ifndef DISABLE_MEMORY_CLEAR
	// clear all shared memory (including the memory we just used to clear private mem)
	// We clear to 0x7n because 0x7n7n7n7n is an obviously-invalid (Type-1) PM4 packet
	//memset(sce::Gnm::translateGpuToCpuAddress(system), 0x7d, systemSize);
	//memset(sce::Gnm::translateGpuToCpuAddress(gpuShared),  0x11, gpuSharedSize);
#endif // DISABLE_MEMORY_CLEAR

	//fprintf(stderr,"Shared System size    : 0x%010lX\n", static_cast<uint64_t>(systemPoolSize));
	//fprintf(stderr,"Shared System CPU Addr: 0x%010lX\n", uintptr_t(system));
	//fprintf(stderr,"Shared Video size     : 0x%010lX\n", static_cast<uint64_t>(sharedGpuPoolSize));
	//fprintf(stderr,"Shared Video CPU Addr : 0x%010lX\n", uintptr_t(gpuShared));
	return SCE_OK;
}
int ssgi::GraphicsLoaderImpl::MemoryBlock::finalize(void)
{
	if(m_systemPoolOffset != 0){
		sceKernelReleaseDirectMemory(m_systemPoolOffset, m_systemPoolSize);
		m_systemPoolPtr = NULL;
		m_systemPoolSize = 0;
		m_systemPoolOffset = 0;
	}
	if(m_gpuSharedOffset != 0){
		sceKernelReleaseDirectMemory(m_gpuSharedOffset, m_gpuSharedSize);
		m_gpuSharedPtr = NULL;
		m_gpuSharedSize = 0;
		m_gpuSharedOffset = 0;
	}
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::initialize(size_t systemHeapSize, size_t videoHeapSize)
{
	int ret = sceSysmoduleLoadModule(SCE_SYSMODULE_JPEG_DEC);
	if( ret != SCE_OK ){
		return ret;
	}

	ret = sceSysmoduleLoadModule(SCE_SYSMODULE_PNG_DEC);
	if( ret != SCE_OK ){
		return ret;
	}

	ret = m_memoryBlock.initialize(systemHeapSize, videoHeapSize);
	if(ret != SCE_OK){
		printf(__FILE__"(%d) Graphics memory initialization failed\n", __LINE__);
		return ret;
	}
	m_heap.initialize2(m_memoryBlock.m_systemPoolPtr, m_memoryBlock.m_systemPoolSize,
		m_memoryBlock.m_gpuSharedPtr, m_memoryBlock.m_gpuSharedSize);

	return SCE_OK;
}
int ssgi::GraphicsLoaderImpl::finalize(void)
{
	m_heap.finalize();
	m_memoryBlock.finalize();

	int ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_JPEG_DEC);
	if( ret != SCE_OK ){
		return ret;
	}
	ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_PNG_DEC);
	if( ret != SCE_OK ){
		return ret;
	}

	return SCE_OK;
}


int ssgi::GraphicsLoaderImpl::createVertexProgram(VertexProgram **outVertexProgram, const Program* programId,
	const VertexAttribute *attrs, uint32_t numAttrs,
	const VertexStream *streams, uint32_t numStreams)
{
	if(outVertexProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(programId == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	VertexProgramImpl *_vertexProgram = new VertexProgramImpl;
	_vertexProgram->m_streams = NULL;
	_vertexProgram->m_attributes = NULL;
	
	_vertexProgram->m_programId = programId;
	
	_vertexProgram->m_numStreams = numStreams;
	if(numStreams>0){
		_vertexProgram->m_streams = new VertexStream[numStreams];
		for(uint32_t i=0; i<numStreams; i++){
			_vertexProgram->m_streams[i] = streams[i];
		}
	}
	_vertexProgram->m_numAttrs = numAttrs;

	if(numAttrs>0){
		const ProgramImpl *pimpl = dynamic_cast<const ProgramImpl *>(programId);
		SCE_SAMPLE_UTIL_ASSERT(pimpl != NULL);

		_vertexProgram->m_attributes = new VertexProgramImpl::AttrParam[numAttrs];
		for(uint32_t i=0; i<_vertexProgram->m_numAttrs; i++){
			_vertexProgram->m_attributes[i].attr = attrs[i];
			const Parameter* param = pimpl->findParameterBySemantic(attrs[i].semantic, attrs[i].semanticIndex);
			if(param == NULL){
				_vertexProgram->m_attributes[i].reg  = -1;
			}else{
				_vertexProgram->m_attributes[i].reg  = param->getResourceIndex();
			}
		}
	}
	*outVertexProgram = _vertexProgram;
	return SCE_OK;
}


int ssgi::GraphicsLoaderImpl::releaseVertexProgram(VertexProgram *vertexProgram)
{
	delete vertexProgram;

	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::cloneVertexProgram(VertexProgram **vertexProgram, const VertexProgram *_from)
{
	if(vertexProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(_from == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	const VertexProgramImpl *from = dynamic_cast<const VertexProgramImpl *>(_from);
	VertexProgramImpl *v = new VertexProgramImpl;
	v->m_programId = from->m_programId;
	v->m_numAttrs  = from->m_numAttrs;
	v->m_attributes = new VertexProgramImpl::AttrParam[v->m_numAttrs];
	for(uint32_t i=0; i<v->m_numAttrs; i++){
		v->m_attributes[i] = from->m_attributes[i];
	}


	v->m_numStreams = from->m_numStreams;
	v->m_streams = new VertexStream[v->m_numStreams];
	for(uint32_t i=0; i<v->m_numStreams; i++){
		v->m_streams[i] = from->m_streams[i];
	}
	*vertexProgram = v;
	return SCE_OK;
}
int ssgi::GraphicsLoaderImpl::cloneFragmentProgram(FragmentProgram **fragmentProgram, const FragmentProgram *_from)
{
	if(fragmentProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(_from == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	const FragmentProgramImpl *from = dynamic_cast<const FragmentProgramImpl *>(_from);
	FragmentProgramImpl *f = new FragmentProgramImpl;
	f->m_programId  = from->m_programId;
	f->m_colorMask = from->m_colorMask;
	f->colorSrc = from->colorSrc;
	f->colorComb = from->colorComb;
	f->colorDest = from->colorDest;
	f->alphaSrc  = from->alphaSrc;
	f->alphaComb = from->alphaComb;
	f->alphaDest = from->alphaDest;
	
	*fragmentProgram = f;
	return SCE_OK;

}





int ssgi::GraphicsLoaderImpl::createFragmentProgram(FragmentProgram **outFragmentProgram, const Program* programId, const Program* vertexProgramId, const BlendInfo *blend)
{
	if(outFragmentProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(programId == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	FragmentProgramImpl *_fragmentProgram = new FragmentProgramImpl;
	_fragmentProgram->m_programId = programId;
	
	BlendInfo b;
	b.initializeAsNoBlend();
	if(blend != NULL){
		b = *blend;
	}
	if(b.colorFunc == kBlendFuncNone){
		b.colorDst = ssg::kBlendFactorZero;
		b.colorSrc = ssg::kBlendFactorOne;
		b.colorFunc = ssg::kBlendFuncAdd;
	}
	if(b.alphaFunc == kBlendFuncNone){
		b.alphaDst = ssg::kBlendFactorZero;
		b.alphaSrc = ssg::kBlendFactorOne;
		b.alphaFunc = ssg::kBlendFuncAdd;
	}

	_fragmentProgram->m_colorMask = 0;
	_fragmentProgram->m_colorMask |= (b.colorMask & kColorMaskR) ? (0x1<<0) : 0;
	_fragmentProgram->m_colorMask |= (b.colorMask & kColorMaskG) ? (0x1<<1) : 0;
	_fragmentProgram->m_colorMask |= (b.colorMask & kColorMaskB) ? (0x1<<2) : 0;
	_fragmentProgram->m_colorMask |= (b.colorMask & kColorMaskA) ? (0x1<<3) : 0;
	_fragmentProgram->colorSrc    = convertBlendOp  ((ssg::BlendFactor)b.colorSrc);
	_fragmentProgram->colorComb   = convertBlendFunc((ssg::BlendFunc)b.colorFunc);
	_fragmentProgram->colorDest   = convertBlendOp  ((ssg::BlendFactor)b.colorDst);
	_fragmentProgram->alphaSrc    = convertBlendOp  ((ssg::BlendFactor)b.alphaSrc);
	_fragmentProgram->alphaComb   = convertBlendFunc((ssg::BlendFunc)b.alphaFunc);
	_fragmentProgram->alphaDest   = convertBlendOp  ((ssg::BlendFactor)b.colorDst);

	*outFragmentProgram = _fragmentProgram;
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::createComputeProgram(ComputeProgram **outComputeProgram, const Program* programId)
{
	if(outComputeProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(programId == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	const ProgramImpl *p = dynamic_cast<const ProgramImpl *>(programId);
	const GnmProgram *gnm_program = p->m_program;
	if(gnm_program == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	if(gnm_program->m_type != GnmProgram::TYPE_COMPUTE){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	ComputeProgramImpl *_computeProgram = new ComputeProgramImpl;
	_computeProgram->m_programId = programId;
	_computeProgram->m_csShader  = gnm_program->m_csShader;
	*outComputeProgram = _computeProgram;
	return SCE_OK;
	
}

int ssgi::GraphicsLoaderImpl::releaseFragmentProgram(FragmentProgram *fragmentProgram)
{
	delete fragmentProgram;
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::registerVertexProgram(ssg::Program** outProgram, const void *binary, uint32_t programSize, ShaderFormat shaderFormat)
{
	if(outProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	*outProgram = registerProgramFromMemory(binary, programSize);
	if(*outProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	return SCE_OK;
}
int ssgi::GraphicsLoaderImpl::registerFragmentProgram(ssg::Program** outProgram, const void *binary, uint32_t programSize, ShaderFormat shaderFormat)
{
	if(outProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	*outProgram = registerProgramFromMemory(binary, programSize);
	if(*outProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::registerComputeProgram(ssg::Program** outProgram, const void *binary, uint32_t programSize, ShaderFormat shaderFormat)
{
	if(outProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	*outProgram = registerProgramFromMemory(binary, programSize);
	if(*outProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	return SCE_OK;
}


ssg::GnmProgram* ssgi::GraphicsLoaderImpl::_registerProgram(const void*  binary, uint32_t binarySize)
{
	if((binary == NULL) || (binarySize == 0)){
		return NULL;
	}
	sce::SampleUtil::Map<const void*, ProgramInfo2>::Type::iterator it;
	it = m_programInfoMap.find(binary);
	if(it != m_programInfoMap.end()){
		// found
		ProgramInfo2 info = it->second;
		info.count++;
		m_programInfoMap[binary] = info;
		return info.program;
	}

	GnmProgram *p = new GnmProgram;
	int ret = p->initialize(this, binary, binarySize);
	if(ret != SCE_OK){
		delete p;
		return NULL;
	}
	
	ProgramInfo2 info;
	info.count = 1;
	info.program = p;
	info.binary = binary;
	m_programInfoMap[binary] = info;
	return p;

}

int ssgi::GraphicsLoaderImpl::_referProgram(const void* binary)
{
	sce::SampleUtil::Map<const void*, ProgramInfo2>::Type::iterator it;
	it = m_programInfoMap.find(binary);
	if(it == m_programInfoMap.end()){
		// not found
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	ProgramInfo2 info = it->second;
	SCE_SAMPLE_UTIL_ASSERT(info.count > 0);
	info.count++;
	m_programInfoMap[binary] = info;
	return SCE_OK;

}

int ssgi::GraphicsLoaderImpl::_unregisterProgram(const void* binary)
{
	sce::SampleUtil::Map<const void *, ProgramInfo2>::Type::iterator it;
	it = m_programInfoMap.find(binary);
	if(it == m_programInfoMap.end()){
		// not found
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	ProgramInfo2 info = it->second;
	SCE_SAMPLE_UTIL_ASSERT(info.count > 0);
	info.count--;
	if(info.count == 0){
		m_programInfoMap.erase(binary);
		info.program->finalize();
		delete info.program;
		return SCE_OK;
	}else{
		m_programInfoMap[binary] = info;
		return SCE_OK;
	}

}





ssg::Program *ssgi::GraphicsLoaderImpl::registerProgramFromMemory(const void *binary, uint32_t binarySize)
{
	const GnmProgram *program = this->_registerProgram(binary, binarySize);
	if(program == NULL){
		return NULL;
	}

	return  new ProgramImpl(program, this);


}

int ssgi::GraphicsLoaderImpl::unregisterProgram(const ssg::Program *program)
{
	delete program;
	return SCE_OK;
}





ssg::Gnm::GraphicsLoaderGnm* ssg::Gnm::GraphicsLoaderGnm::cast(GraphicsLoader *from)
{
	return dynamic_cast<ssg::Gnm::GraphicsLoaderGnm*>(from);
}

const ssg::Gnm::GraphicsLoaderGnm* ssg::Gnm::GraphicsLoaderGnm::cast(const GraphicsLoader *from)
{
	return dynamic_cast<const ssg::Gnm::GraphicsLoaderGnm*>(from);
}



int ssgi::GraphicsLoaderImpl::createTexture(
	ssg::Texture **outTexture,
	TextureFormat texFormat,
	uint32_t width,
	uint32_t height,
	uint32_t mipCount)
{
	if(outTexture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	TextureImpl *t = new TextureImpl;
	int ret = t->initialize(this, width, height, mipCount, 
		texFormat);
	if(ret < SCE_OK){
		delete t;
		return ret;
	}
	*outTexture = t;
	return ret;
}


int ssgi::GraphicsLoaderImpl::createRenderTarget(RenderTarget **outRenderTarget,
	uint32_t width,
	uint32_t height)
{
	if(outRenderTarget == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssgi::RenderTargetImpl *r  = new ssgi::RenderTargetImpl;
	int ret = r->initialize(this, width, height,
		sce::Gnm::kDataFormatR8G8B8A8Unorm,
		sce::Gnm::kTileModeDisplay_LinearAligned,
		sce::Gnm::kNumSamples1, sce::Gnm::kNumFragments1, false , false);
	if(ret != SCE_OK){
		delete r;
		return ret;
	}
	*outRenderTarget = r;
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::createDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface,
	uint32_t width,
	uint32_t height)
{
	if(outDepthStencilSurface == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssgi::DepthStencilSurfaceImpl *d = new ssgi::DepthStencilSurfaceImpl;
	int ret = d->initialize(this, width, height, false, false);
	if(ret != SCE_OK){
		delete d;
		return ret;		
	}
	*outDepthStencilSurface = d;
	return SCE_OK;
}




#endif // _SCE_TARGET_OS_ORBIS
