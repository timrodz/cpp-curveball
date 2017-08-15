/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/graphics.h>
#include "../graphics_internal.h"
#include "../constant_internal.h"


namespace ssg = sce::SampleUtil::Graphics;
namespace ssi = sce::SampleUtil::Impl;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

#pragma comment( lib, "SceSysmodule_stub" )
#pragma comment( lib, "SceGxm_stub" )
#pragma comment( lib, "SceDisplay_stub" )
#pragma comment( lib, "ScePerf_stub" )
#pragma comment( lib, "SceRazorCapture_stub_weak" )

int ssgi::GraphicsLoaderImpl::initialize(void)
{
	return initialize(NULL);
}

int ssgi::GraphicsLoaderImpl::initialize(SceGxmShaderPatcher *patcher)
{
	int ret;
	ret = m_lpddrUcRwHeap.initialize("SampleGraphUtil", 1*1024*1024, SCE_KERNEL_MEMBLOCK_TYPE_USER_RW_UNCACHE);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	if(ret != SCE_OK){
		return ret;
	}

	ret = m_cdramRwHeap.initialize("SampleGraphUtil", 32*1024*1024, SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	if(ret != SCE_OK){
		return ret;
	}

	ret = m_usseHeap.initialize("SampleGraphUtil", 1*1024*1024, SCE_KERNEL_MEMBLOCK_TYPE_USER_CDRAM_RW, false, true);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	if(ret != SCE_OK){
		return ret;
	}

	ret = m_jpegDecoder.initialize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	if(ret != SCE_OK){
		return ret;
	}

	if(patcher != NULL){
		m_destroyShaderPatcher = false;
		m_gxmShaderPatcher = patcher;
	}else{
		m_destroyShaderPatcher = true;
		m_gxmShaderPatcher = createGxmShaderPatcher();
	}

	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::finalize(void)
{
	int ret;
	ret = m_lpddrUcRwHeap.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = m_cdramRwHeap.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = m_usseHeap.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = m_jpegDecoder.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	if(m_destroyShaderPatcher){
		sceGxmShaderPatcherDestroy(m_gxmShaderPatcher);
		m_destroyShaderPatcher = false;
	}
	(void)ret;
	return SCE_OK;
}


static 	uint32_t getVertexAttributeRegBySemantic(SceGxmShaderPatcherId programId, SceGxmParameterSemantic semantic,	uint32_t index)
{
	const SceGxmProgram *program = sceGxmShaderPatcherGetProgramFromId( programId );
	const SceGxmProgramParameter *param = sceGxmProgramFindParameterBySemantic(program, semantic, index);
	return sceGxmProgramParameterGetResourceIndex( param );
}

static SceGxmVertexAttribute convertAttr(const ssgi::ProgramImpl* programId, const ssg::VertexAttribute &attr)
{
	SceGxmAttributeFormat format;
	switch(attr.format)
	{
	case ssg::kAttributeFormatU8: format  = 	SCE_GXM_ATTRIBUTE_FORMAT_U8; break; 
	case ssg::kAttributeFormatS8: format   = 	SCE_GXM_ATTRIBUTE_FORMAT_S8; break; 		
	case ssg::kAttributeFormatU16: format  = 	SCE_GXM_ATTRIBUTE_FORMAT_U16; break; 		
	case ssg::kAttributeFormatS16: format  = 	SCE_GXM_ATTRIBUTE_FORMAT_S16; break; 		
	case ssg::kAttributeFormatU8N: format  = 	SCE_GXM_ATTRIBUTE_FORMAT_U8N; break; 		
	case ssg::kAttributeFormatS8N: format  =		SCE_GXM_ATTRIBUTE_FORMAT_S8N; break; 		
	case ssg::kAttributeFormatU16N: format =		SCE_GXM_ATTRIBUTE_FORMAT_U16N; break; 		
	case ssg::kAttributeFormatS16N: format =		SCE_GXM_ATTRIBUTE_FORMAT_S16N; break; 		
	case ssg::kAttributeFormatF16: format  =		SCE_GXM_ATTRIBUTE_FORMAT_F16; break; 		
	case ssg::kAttributeFormatF32: format  = 	SCE_GXM_ATTRIBUTE_FORMAT_F32; break; 
	default: format = SCE_GXM_ATTRIBUTE_FORMAT_U8; break; 
	}
	SceGxmVertexAttribute ret;
	ret.streamIndex    = attr.streamIndex;
	ret.offset         = attr.offset;
	ret.format         = format;
	ret.componentCount = attr.componentCount;
	ret.regIndex       = getVertexAttributeRegBySemantic( *programId,  (SceGxmParameterSemantic)attr.semantic, attr.semanticIndex);

	return ret;
}


int ssgi::GraphicsLoaderImpl::createVertexProgram(ssg::VertexProgram **vertexProgram, const ssg::Program* iprogramId, 
										 const ssg::VertexAttribute *attrs, uint32_t numAttrs,
										 const ssg::VertexStream *streams, uint32_t numStreams)
{
	if(vertexProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(iprogramId == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if((attrs == NULL) && (numAttrs>0)){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if((streams == NULL) && (numStreams>0)){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	const ssgi::ProgramImpl *programId = dynamic_cast<const ssgi::ProgramImpl*>(iprogramId);
	SceGxmVertexAttribute *gxmAttr = (SceGxmVertexAttribute *)alloca(sizeof(SceGxmVertexAttribute) * numAttrs);
	for(uint32_t i=0; i<numAttrs; i++){
		gxmAttr[i] = convertAttr(programId, attrs[i]);
	}


	SceGxmVertexStream *vstreams = (SceGxmVertexStream *)alloca(sizeof(SceGxmVertexStream)*numStreams);
	for(uint32_t i=0; i<numStreams; i++){
		vstreams[i].stride = streams[i].stride;
		switch(streams[i].indexSource){
			case kIndexSourceIndex16Bit     : vstreams[i].indexSource= SCE_GXM_INDEX_SOURCE_INDEX_16BIT;break;
			case kIndexSourceIndex32Bit	: vstreams[i].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_32BIT;break;
			case kIndexSourceInstance16Bit	: vstreams[i].indexSource = SCE_GXM_INDEX_SOURCE_INSTANCE_16BIT;break;
			case kIndexSourceInstance32Bit	 : vstreams[i].indexSource= SCE_GXM_INDEX_SOURCE_INSTANCE_32BIT;break;
			default: vstreams[i].indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT; break;
		}
	}

	ssgi::VertexProgramImpl* v = new ssgi::VertexProgramImpl;
	v->m_gxmShaderPatcher = m_gxmShaderPatcher;

	int ret = sceGxmShaderPatcherCreateVertexProgram( m_gxmShaderPatcher, 
		*programId, 
		gxmAttr, numAttrs,
		vstreams, numStreams, 
		&v->m_gxmVertexProgram);
	v->m_programId = programId;
	*vertexProgram = v;

	return ret;

}


int  ssgi::GraphicsLoaderImpl::createFragmentProgram(ssg::FragmentProgram **fragmentProgram, 
												const ssg::Program* programId, 
												const ssg::Program* vertexProgramId, 
												const ssg::BlendInfo *blendInfo)
{
	return createFragmentProgramGxm(fragmentProgram, 
		programId, 
		 vertexProgramId, 
		 blendInfo,
		 SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4,
		 SCE_GXM_OUTPUT_REGISTER_FORMAT_HALF4);
}


int  ssgi::GraphicsLoaderImpl::createFragmentProgramGxm(ssg::FragmentProgram **_fragmentProgram, 
												const ssg::Program* iprogramId, 
												const ssg::Program* ivertexProgramId, 
												const ssg::BlendInfo *blendInfo,
												SceGxmOutputRegisterFormat outputFormat32BIT,
												SceGxmOutputRegisterFormat outputFormat64BIT)
{
	if((_fragmentProgram == NULL) || (iprogramId == NULL) || (ivertexProgramId == NULL)){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	int returnCode;

	ssgi::FragmentProgramImpl *fragmentProgram = new ssgi::FragmentProgramImpl;

	const ssgi::ProgramImpl* programId       = dynamic_cast<const ssgi::ProgramImpl*>(iprogramId);
	const ssgi::ProgramImpl* vertexProgramId = dynamic_cast<const ssgi::ProgramImpl*>(ivertexProgramId);

	fragmentProgram->m_gxmShaderPatcher = m_gxmShaderPatcher;
	fragmentProgram->m_vertexProgram = sceGxmShaderPatcherGetProgramFromId(*vertexProgramId);
	fragmentProgram->m_programId = programId;


	SceGxmBlendInfo *bi;
	SceGxmBlendInfo _bi;
	if(blendInfo != NULL){
		_bi = ssgi::convertBlendInfo(*blendInfo);
		bi = &_bi;
	}else{
		bi = NULL;
	}


	SceGxmShaderPatcher *p = m_gxmShaderPatcher;
	const ProgramImpl* fpid = programId;
	const SceGxmProgram *vp = fragmentProgram->m_vertexProgram;

	returnCode = sceGxmShaderPatcherCreateFragmentProgram(p, *fpid, outputFormat32BIT, SCE_GXM_MULTISAMPLE_NONE, bi,vp, &fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT][SCE_GXM_MULTISAMPLE_NONE] );
	if(returnCode == SCE_GXM_ERROR_UNSUPPORTED){ fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT][SCE_GXM_MULTISAMPLE_NONE] = NULL; }
	else if(returnCode != SCE_OK){ sce::SampleUtil::destroy(static_cast<ssi::ResourceImpl*>(fragmentProgram)); return returnCode; }

	returnCode = sceGxmShaderPatcherCreateFragmentProgram(p, *fpid, outputFormat32BIT, SCE_GXM_MULTISAMPLE_2X,   bi,vp, &fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT][SCE_GXM_MULTISAMPLE_2X] );
	if(returnCode == SCE_GXM_ERROR_UNSUPPORTED){ fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT][SCE_GXM_MULTISAMPLE_2X] = NULL; }
	else if(returnCode != SCE_OK){ sce::SampleUtil::destroy(static_cast<ssi::ResourceImpl*>(fragmentProgram)); return returnCode; }

	returnCode = sceGxmShaderPatcherCreateFragmentProgram(p, *fpid, outputFormat32BIT, SCE_GXM_MULTISAMPLE_4X,   bi,vp, &fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT][SCE_GXM_MULTISAMPLE_4X] );
	if(returnCode == SCE_GXM_ERROR_UNSUPPORTED){ fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT][SCE_GXM_MULTISAMPLE_4X] = NULL; }
	else if(returnCode != SCE_OK){ sce::SampleUtil::destroy(static_cast<ssi::ResourceImpl*>(fragmentProgram)); return returnCode; }

	returnCode = sceGxmShaderPatcherCreateFragmentProgram(p, *fpid, outputFormat64BIT, SCE_GXM_MULTISAMPLE_NONE, bi,vp, &fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_64BIT][SCE_GXM_MULTISAMPLE_NONE] );
	if(returnCode == SCE_GXM_ERROR_UNSUPPORTED){ fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_64BIT][SCE_GXM_MULTISAMPLE_NONE] = NULL; }
	else if(returnCode != SCE_OK){ sce::SampleUtil::destroy(static_cast<ssi::ResourceImpl*>(fragmentProgram)); return returnCode; }

	returnCode = sceGxmShaderPatcherCreateFragmentProgram(p, *fpid, outputFormat64BIT, SCE_GXM_MULTISAMPLE_2X,   bi,vp, &fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_64BIT][SCE_GXM_MULTISAMPLE_2X] );
	if(returnCode == SCE_GXM_ERROR_UNSUPPORTED){ fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_64BIT][SCE_GXM_MULTISAMPLE_2X] = NULL; }
	else if(returnCode != SCE_OK){ sce::SampleUtil::destroy(static_cast<ssi::ResourceImpl*>(fragmentProgram)); return returnCode; }

	returnCode = sceGxmShaderPatcherCreateFragmentProgram(p, *fpid, outputFormat64BIT, SCE_GXM_MULTISAMPLE_4X,   bi,vp, &fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_64BIT][SCE_GXM_MULTISAMPLE_4X] );
	if(returnCode == SCE_GXM_ERROR_UNSUPPORTED){ fragmentProgram->m_gxmFragmentPrograms[SCE_GXM_OUTPUT_REGISTER_SIZE_64BIT][SCE_GXM_MULTISAMPLE_4X] = NULL; }
	else if(returnCode != SCE_OK){ sce::SampleUtil::destroy(static_cast<ssi::ResourceImpl*>(fragmentProgram)); return returnCode; }

	*_fragmentProgram = fragmentProgram;

	return returnCode;
}

int ssgi::GraphicsLoaderImpl::cloneVertexProgram(ssg::VertexProgram **_vertexProgram, const ssg::VertexProgram *_from)
{
	if((_vertexProgram == NULL) || (_from == NULL)){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	const ssgi::VertexProgramImpl *from = dynamic_cast<const ssgi::VertexProgramImpl *>(_from);
	ssgi::VertexProgramImpl *vertexProgram = new ssgi::VertexProgramImpl;
	*vertexProgram = *from;
	int ret = sceGxmShaderPatcherAddRefVertexProgram(m_gxmShaderPatcher, vertexProgram->m_gxmVertexProgram);
	*_vertexProgram = vertexProgram;
	return ret;
}

int ssgi::GraphicsLoaderImpl::cloneFragmentProgram(ssg::FragmentProgram **_fragmentProgram, const ssg::FragmentProgram *_from)
{
	if((_fragmentProgram == NULL) || (_from == NULL)){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	const ssgi::FragmentProgramImpl *from = dynamic_cast<const ssgi::FragmentProgramImpl *>(_from);
	ssgi::FragmentProgramImpl *fragmentProgram = new ssgi::FragmentProgramImpl;
	*fragmentProgram = *from;
	for(uint32_t i=0; i<2; i++){
		for(uint32_t j=0; j<3; j++){
			if(fragmentProgram->m_gxmFragmentPrograms[i][j] != NULL){
				int ret = sceGxmShaderPatcherAddRefFragmentProgram( m_gxmShaderPatcher, fragmentProgram->m_gxmFragmentPrograms[i][j]);
				SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
			}
		}
	}
	*_fragmentProgram = fragmentProgram;
	return SCE_OK;
}



SceGxmShaderPatcherId ssgi::GraphicsLoaderImpl::_registerProgram(const SceGxmProgram* program)
{
	sce::SampleUtil::Map<const SceGxmProgram *, ProgramInfo2>::Type::iterator it;
	it = m_programInfoMap.find(program);
	if(it != m_programInfoMap.end()){
		// found
		ProgramInfo2 info = it->second;
		info.count++;
		m_programInfoMap[program] = info;
		return info.patcherId;
	}

	SceGxmShaderPatcherId patcherId;

	int ret = sceGxmShaderPatcherRegisterProgram( m_gxmShaderPatcher, (const SceGxmProgram *)program, &patcherId);
	if(ret < 0){
		return NULL;
	}else{
		ProgramInfo2 info;
		info.count = 1;
		info.patcherId = patcherId;
		info.program = program;
		m_programInfoMap[program] = info;
		return patcherId;
	}

}

int ssgi::GraphicsLoaderImpl::_referProgram(const SceGxmProgram* program)
{
	sce::SampleUtil::Map<const SceGxmProgram *, ProgramInfo2>::Type::iterator it;
	it = m_programInfoMap.find(program);
	if(it == m_programInfoMap.end()){
		// not found
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	ProgramInfo2 info = it->second;
	SCE_SAMPLE_UTIL_ASSERT(info.count > 0);
	info.count++;
	m_programInfoMap[program] = info;
	return SCE_OK;

}

int ssgi::GraphicsLoaderImpl::_unregisterProgram(const SceGxmProgram* program)
{
	sce::SampleUtil::Map<const SceGxmProgram *, ProgramInfo2>::Type::iterator it;
	it = m_programInfoMap.find(program);
	if(it == m_programInfoMap.end()){
		// not found
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	ProgramInfo2 info = it->second;
	SCE_SAMPLE_UTIL_ASSERT(info.count > 0);
	info.count--;
	if(info.count == 0){
		m_programInfoMap.erase(program);
		int ret = sceGxmShaderPatcherUnregisterProgram( m_gxmShaderPatcher, info.patcherId );
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		return SCE_OK;
	}else{
		m_programInfoMap[program] = info;
		return SCE_OK;
	}

}


int ssgi::GraphicsLoaderImpl::registerVertexProgram(ssg::Program** outProgram, const void *program, uint32_t size, ShaderFormat shaderFormat)
{
	if(outProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(sceGxmProgramGetType((const SceGxmProgram *)program) != SCE_GXM_VERTEX_PROGRAM){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	SceGxmShaderPatcherId patcherId = _registerProgram((const SceGxmProgram *)program);
	if(patcherId == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	*outProgram = new ssgi::ProgramImpl(patcherId, this);
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::registerFragmentProgram(ssg::Program** outProgram, const void *program, uint32_t size, ShaderFormat shaderFormat)
{
	if(outProgram == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(sceGxmProgramGetType((const SceGxmProgram *)program) != SCE_GXM_FRAGMENT_PROGRAM){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	SceGxmShaderPatcherId patcherId = _registerProgram((const SceGxmProgram *)program);
	if(patcherId == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	*outProgram = new ssgi::ProgramImpl(patcherId, this);
	return SCE_OK;
}

int ssgi::GraphicsLoaderImpl::unregisterProgram(ssg::Program* program)
{
	if(program == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	delete program;
	return SCE_OK;
}


void *ssgi::GraphicsLoaderImpl::patcherHostAlloc(void *userData, uint32_t size)
{

	return malloc(size);
}
void ssgi::GraphicsLoaderImpl::patcherHostFree(void *userData, void *mem)
{
	::free(mem);
}
void *ssgi::GraphicsLoaderImpl::patcherBufferAlloc(void *userData, uint32_t size)
{

	ssgi::GraphicsLoaderImpl *c = (GraphicsLoaderImpl *)userData;
	return c->m_cdramRwHeap.malloc(size);
}
void ssgi::GraphicsLoaderImpl::patcherBufferFree(void *userData, void *pMem)
{
	ssgi::GraphicsLoaderImpl *c = (GraphicsLoaderImpl *)userData;
	c->m_cdramRwHeap.free(pMem);
}
void *ssgi::GraphicsLoaderImpl::patcherVertexUsseAlloc(void *userData, uint32_t size, uint32_t *usseOffset)
{
	ssgi::GraphicsLoaderImpl *c = (GraphicsLoaderImpl *)userData;
	void *p = c->m_usseHeap.memalign(SCE_GXM_USSE_ALIGNMENT, size);
	*usseOffset = c->m_usseHeap.convertToVertexUsseOffset(p);
	return p;
}
void ssgi::GraphicsLoaderImpl::patcherVertexUsseFree(void *userData, void *pMem)
{
	ssgi::GraphicsLoaderImpl *c = (GraphicsLoaderImpl *)userData;
	c->m_usseHeap.free(pMem);
}
void *ssgi::GraphicsLoaderImpl::patcherFragmentUsseAlloc(void *userData, uint32_t size, uint32_t *usseOffset)
{
	GraphicsLoaderImpl *c = (GraphicsLoaderImpl *)userData;
	void *p = c->m_usseHeap.memalign(SCE_GXM_USSE_ALIGNMENT, size);
	*usseOffset = c->m_usseHeap.convertToFragmentUsseOffset(p);
	return p;
}
void ssgi::GraphicsLoaderImpl::patcherFragmentUsseFree(void *userData, void *pMem)
{
	GraphicsLoaderImpl *c = (GraphicsLoaderImpl *)userData;
	c->m_usseHeap.free(pMem);
}


SceGxmShaderPatcher *ssgi::GraphicsLoaderImpl::createGxmShaderPatcher(void)
{
	SceGxmShaderPatcherParams params;
	memset(&params, 0, sizeof(SceGxmShaderPatcherParams));
	params.userData					= this;
	params.hostAllocCallback		= &patcherHostAlloc;
	params.hostFreeCallback			= &patcherHostFree;
	params.bufferAllocCallback		= &patcherBufferAlloc;
	params.bufferFreeCallback		= &patcherBufferFree;
	params.bufferMem				= NULL;
	params.bufferMemSize			= NULL;
	params.vertexUsseAllocCallback	= &patcherVertexUsseAlloc;
	params.vertexUsseFreeCallback	= &patcherVertexUsseFree;
	params.vertexUsseMem			= NULL;
	params.vertexUsseMemSize		= NULL;
	params.vertexUsseOffset			= NULL;
	params.fragmentUsseAllocCallback= &patcherFragmentUsseAlloc;
	params.fragmentUsseFreeCallback	= &patcherFragmentUsseFree;
	params.fragmentUsseMem			= NULL;
	params.fragmentUsseMemSize		= NULL;
	params.fragmentUsseOffset		= NULL;

	SceGxmShaderPatcher *shaderPatcher;

	int ret = sceGxmShaderPatcherCreate(&params, &shaderPatcher);
	if(ret != SCE_OK){
		return NULL;
	}
	return shaderPatcher;
}




SceGxmShaderPatcher *ssgi::GraphicsLoaderImpl::getGxmShaderPatcher(void) const
{
	return m_gxmShaderPatcher;
}


uint32_t ssgi::GraphicsLoaderImpl::convertToVertexUsseOffset(void *ptr) const
{
	return m_usseHeap.convertToVertexUsseOffset(ptr);
}

uint32_t ssgi::GraphicsLoaderImpl::convertToFragmentUsseOffset(void *ptr) const
{
	return m_usseHeap.convertToFragmentUsseOffset(ptr);
}

void *ssgi::GraphicsLoaderImpl::memalign(MemType memtype, uint32_t boundary, uint32_t size)
{
	if(memtype == MEMTYPE_CDRAM){
		return m_cdramRwHeap.memalign(boundary, size);
	}else if(memtype == MEMTYPE_USSE){
		return m_usseHeap.memalign(boundary, size);
	}else{
		return m_lpddrUcRwHeap.memalign(boundary, size);
	}
}

void ssgi::GraphicsLoaderImpl::free(void *ptr)
{
	if(m_usseHeap.isInside(ptr)){
		m_usseHeap.free(ptr);
	}else if(m_lpddrUcRwHeap.isInside(ptr)){
		m_lpddrUcRwHeap.free(ptr);
	}else{
		m_cdramRwHeap.free(ptr);
	}
}

void ssgi::GraphicsLoaderImpl::getMemoryUsage(MemoryUsage *usage)
{
	if(usage == NULL){
		return;
	}

	Heap::Usage usseUsage;
	Heap::Usage lpddrUsage;
	Heap::Usage cdramUsage;

	m_usseHeap.getUsage(&usseUsage);
	m_lpddrUcRwHeap.getUsage(&lpddrUsage);
	m_cdramRwHeap.getUsage(&cdramUsage);


	usage->usse.totalSize                = usseUsage.totalSize;
	usage->usse.maxTotalSize             = usseUsage.maxTotalSize;
	usage->usse.memblockTotalSize        = usseUsage.memblockTotalSize;
	usage->lpddrUcRw.totalSize           = lpddrUsage.totalSize;
	usage->lpddrUcRw.maxTotalSize        = lpddrUsage.maxTotalSize;
	usage->lpddrUcRw.memblockTotalSize   = lpddrUsage.memblockTotalSize;
	usage->cdramRw.totalSize             = cdramUsage.totalSize;
	usage->cdramRw.maxTotalSize          = cdramUsage.maxTotalSize;
	usage->cdramRw.memblockTotalSize     = cdramUsage.memblockTotalSize;

}

ssgi::_JpegDecoder *ssgi::GraphicsLoaderImpl::getJpegDecoder()
{
	return &m_jpegDecoder;
}

int ssgi::GraphicsLoaderImpl::createGxmRenderTarget(
					RenderTarget **outRenderTarget, 
					uint32_t width,
					uint32_t height,
					SceGxmColorSurfaceType type,
					SceGxmColorFormat format,
					SceGxmOutputRegisterSize outputRegisterSize,

					uint32_t renderTargetFlags,
					uint16_t scenesPerFrame,
					SceGxmMultisampleMode msaaMode,
					uint32_t multisampleLocations,

					SceGxmSyncObject*  vertexSyncObject,
					SceGxmSyncObject*  fragmentSyncObject)
{
	RenderTargetImpl *rt = new RenderTargetImpl();
	int ret = rt->initialize(this, width, height,
		type,
		format,
		outputRegisterSize,

		renderTargetFlags,
		scenesPerFrame,
		msaaMode,
		multisampleLocations,

		vertexSyncObject,
		fragmentSyncObject);
	if(ret != SCE_OK){
		delete rt;
		return ret;
	}
	*outRenderTarget = rt;
	return SCE_OK;
}


int ssgi::GraphicsLoaderImpl::createGxmDepthStencilSurface(DepthStencilSurface **outDepthStencilSurface, 
					uint32_t width,
					uint32_t height, 
					SceGxmDepthStencilFormat fort,
					SceGxmDepthStencilSurfaceType type,
					SceGxmMultisampleMode multisampleMode,
					bool enableForceStore,
					bool enableForceLoad
					)
{
	DepthStencilSurfaceImpl *ds = new DepthStencilSurfaceImpl();
	int ret = ds->initialize(this, width, height,
		fort,
		type,
		multisampleMode,
		enableForceStore,
		enableForceLoad
		);
	if(ret != SCE_OK){
		delete ds;
		return ret;
	}
	*outDepthStencilSurface = ds;
	return SCE_OK;
}


int ssgi::GraphicsLoaderImpl::createTextureGxmLinear(
					Texture **outTexture,
					SceGxmTextureFormat texFormat,
					uint32_t width,
					uint32_t height,
					uint32_t mipCount,
					uint32_t bufferAlign)
{
	TextureImpl *texture = new TextureImpl();
	int ret = texture->initializeGxmLinear(this, texFormat, width, height, mipCount, bufferAlign);
	if(ret < SCE_OK){
		SCE_SAMPLE_UTIL_ASSERT(0);
		delete texture;
		return ret;
	}
	*outTexture = texture;
	return ret;
}

int ssgi::GraphicsLoaderImpl::createTextureGxmLinearStrided(
					Texture **outTexture,
					SceGxmTextureFormat texFormat,
					uint32_t width,
					uint32_t height,
					uint32_t byteStride,
					uint32_t bufferAlign)
{
	TextureImpl *texture = new TextureImpl();
	int ret = texture->initializeGxmLinearStrided(this, texFormat, width, height, byteStride, bufferAlign);
	if(ret < SCE_OK){
		SCE_SAMPLE_UTIL_ASSERT(0);
		delete texture;
		return ret;
	}
	*outTexture = texture;
	return ret;
}


int ssgi::GraphicsLoaderImpl::createTextureGxmTiled(
	Texture **outTexture,
	SceGxmTextureFormat texFormat,
	uint32_t width,
	uint32_t height,
	uint32_t mipCount,
	uint32_t bufferAlign)
{
	TextureImpl *texture = new TextureImpl();
	int ret = texture->initializeGxmTiled(this, texFormat, width, height, mipCount, bufferAlign);
	if(ret < SCE_OK){
		SCE_SAMPLE_UTIL_ASSERT(0);
		delete texture;
		return ret;
	}
	*outTexture = texture;
	return ret;
}

int ssgi::GraphicsLoaderImpl::createTextureGxmSwizzled(
	Texture **outTexture,
	SceGxmTextureFormat texFormat,
	uint32_t width,
	uint32_t height,
	uint32_t mipCount,
	uint32_t bufferAlign)
{
	TextureImpl *texture = new TextureImpl();
	int ret = texture->initializeGxmSwizzled(this, texFormat, width, height, mipCount, bufferAlign);
	if(ret < SCE_OK){
		SCE_SAMPLE_UTIL_ASSERT(0);
		delete texture;
		return ret;
	}
	*outTexture = texture;
	return ret;
}

int ssgi::GraphicsLoaderImpl::createTextureGxmCube(
	Texture **outTexture,
	SceGxmTextureFormat texFormat,
	uint32_t width,
	uint32_t height,
	uint32_t mipCount,
	uint32_t bufferAlign)
{
	TextureImpl *texture = new TextureImpl();
	int ret = texture->initializeGxmCube(this, texFormat, width, height, mipCount, bufferAlign);
	if(ret < SCE_OK){
		SCE_SAMPLE_UTIL_ASSERT(0);
		delete texture;
		return ret;
	}
	*outTexture = texture;
	return ret;
}

ssg::Gxm::GraphicsLoaderGxm *ssg::Gxm::GraphicsLoaderGxm::downcast(ssg::GraphicsLoader *loader)
{
	return dynamic_cast<ssg::Gxm::GraphicsLoaderGxm *>(loader);
}
const ssg::Gxm::GraphicsLoaderGxm *ssg::Gxm::GraphicsLoaderGxm::downcast(const ssg::GraphicsLoader *loader)
{
	return dynamic_cast<const ssg::Gxm::GraphicsLoaderGxm *>(loader);
}


#endif /* _SCE_TARGET_OS_PSP2 */


