/* SIE CONFIDENTIAL
PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#include "stdafx.h"
#include <scebase_common.h>

#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS

#include <scebase.h>
#include <shader/binary.h>
#include "heap_gnm.h"
#include <gnmx/shaderbinary.h>
namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;

#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstdlib> // For malloc -- EVIL
#include <cstring>

#include "loader_internal_gnm.h"

void ssgi::ReleaseShaderProgram(sce::Shader::Binary::Program *sbp)
{
	SCE_SAMPLE_UTIL_ASSERT(sbp);
	SCE_SAMPLE_UTIL_ASSERT(sbp->m_dataChunk);

	free(sbp->m_dataChunk);
	sbp->m_dataChunk = NULL;
}



struct ShaderFileInfo
{
	sce::Shader::Binary::Header shaderBinaryheader;
	sce::Gnmx::ShaderFileHeader shaderFileHeader;
	uintptr_t shader;
	void init(const void* fileImage)
	{
		uintptr_t ptr = (uintptr_t)fileImage;
		memcpy(&shaderBinaryheader, (const void*)ptr, sizeof(sce::Shader::Binary::Header));
		ptr += sizeof(sce::Shader::Binary::Header);

		memcpy(&shaderFileHeader, (const void*)ptr, sizeof(sce::Gnmx::ShaderFileHeader));
		ptr += sizeof(sce::Gnmx::ShaderFileHeader);
		shader = ptr;

		SCE_GNM_ASSERT(shaderFileHeader.m_fileHeader   == sce::Gnmx::kShaderFileHeaderId);
		SCE_GNM_ASSERT(shaderFileHeader.m_majorVersion == sce::Gnmx::kShaderMajorVersion);

	}
};

static uint64_t getShaderMemBlockSize(const sce::Gnmx::ShaderCommonData &commonData)
{
	return commonData.m_shaderSize + commonData.m_embeddedConstantBufferSizeInDQW*16;
}

int ssgi::loadAndAllocateVsShaderFromMemory(ssgi::GraphicsLoaderImpl *loader,
												  sce::Gnmx::VsShader* &vsShader,
                                                ssgi::MemBlock &memBlock,
												const void *sbFileContents, uint32_t sbFileSizeInBytes)
{
	ShaderFileInfo fileInfo;
	fileInfo.init(sbFileContents);
	// Validation:
	SCE_GNM_ASSERT(fileInfo.shaderFileHeader.m_type         == sce::Gnmx::kVertexShader);

	const sce::Gnmx::VsShader *vsb = (const sce::Gnmx::VsShader *)fileInfo.shader;

	// Allocate, and copy the shader code + immediate constant buffer in GPU memory:
	const uint32_t		 vsb_offset		= vsb->m_vsStageRegisters.m_spiShaderPgmLoVs;
	loader->getHeap()->allocateVideoMemoryBlock(&memBlock, getShaderMemBlockSize(vsb->m_common), sce::Gnm::kAlignmentOfShaderInBytes);
	memcpy(memBlock.m_ptr,  (void*)(fileInfo.shader + vsb_offset), getShaderMemBlockSize(vsb->m_common));

	Gnmx::VsShader  *vsbFinal = (Gnmx::VsShader*)malloc(vsb->computeSize());
	memcpy(vsbFinal, vsb, vsb->computeSize());
	vsbFinal->m_vsStageRegisters.m_spiShaderPgmLoVs = memBlock.getLoAddr256();
	vsbFinal->m_vsStageRegisters.m_spiShaderPgmHiVs = memBlock.getHiAddr256();

	vsShader = vsbFinal;

	return SCE_OK;
}

int ssgi::loadAndAllocatePsShaderFromMemory(ssgi::GraphicsLoaderImpl *loader,
											sce::Gnmx::PsShader* &psShader, 
	                                           ssgi::MemBlock &memBlock, 
											   const void *sbFileContents, uint32_t sbFileSizeInBytes)
{
	ShaderFileInfo fileInfo;
	fileInfo.init(sbFileContents);

	// Validation:
	SCE_GNM_ASSERT(fileInfo.shaderFileHeader.m_type         == Gnmx::kPixelShader);

	const sce::Gnmx::PsShader *psb = (const sce::Gnmx::PsShader *)fileInfo.shader;

	// Allocate, and copy the shader code + immediate constant buffer in GPU memory:
	const uint32_t		 psb_offset		= psb->m_psStageRegisters.m_spiShaderPgmLoPs;
	loader->getHeap()->allocateVideoMemoryBlock(&memBlock, getShaderMemBlockSize(psb->m_common), sce::Gnm::kAlignmentOfShaderInBytes);
	memcpy(memBlock.m_ptr,  (const char*)(fileInfo.shader + psb_offset), getShaderMemBlockSize(psb->m_common));

	sce::Gnmx::PsShader *psbFinal = (Gnmx::PsShader*)malloc(psb->computeSize());
	memcpy(psbFinal, psb, psb->computeSize());
	psbFinal->m_psStageRegisters.m_spiShaderPgmLoPs = memBlock.getLoAddr256();
	psbFinal->m_psStageRegisters.m_spiShaderPgmHiPs = memBlock.getHiAddr256();

	psShader = psbFinal;
	return SCE_OK;
}

int ssgi::loadAndAllocateCsShaderFromMemory(ssgi::GraphicsLoaderImpl *loader,
											sce::Gnmx::CsShader * &csShader, 
										   ssgi::MemBlock &memBlock, const void *sbFileContents, uint32_t sbFileSizeInBytes)
{
	ShaderFileInfo fileInfo;
	fileInfo.init(sbFileContents);

	// Validation:
	SCE_GNM_ASSERT(fileInfo.shaderFileHeader.m_type         == Gnmx::kComputeShader);

	const sce::Gnmx::CsShader *csb = (const sce::Gnmx::CsShader *)fileInfo.shader;

	// Allocate, and copy the shader code + immediate constant buffer in GPU memory:
	const uint32_t		 csb_offset		= csb->m_csStageRegisters.m_computePgmLo;
	loader->getHeap()->allocateVideoMemoryBlock(&memBlock, getShaderMemBlockSize(csb->m_common), sce::Gnm::kAlignmentOfShaderInBytes);
	memcpy(memBlock.m_ptr, (const void*)(fileInfo.shader + csb_offset), getShaderMemBlockSize(csb->m_common));


	sce::Gnmx::CsShader *csbFinal = (sce::Gnmx::CsShader*)malloc(csb->computeSize());
	memcpy(csbFinal, csb, csb->computeSize());
	csbFinal->m_csStageRegisters.m_computePgmLo = memBlock.getLoAddr256();
	csbFinal->m_csStageRegisters.m_computePgmHi = memBlock.getHiAddr256();

	csShader = csbFinal;

	return SCE_OK;
}

int ssgi::loadAndAllocateLsShaderFromMemory(ssgi::GraphicsLoaderImpl *loader,
											sce::Gnmx::LsShader *& lsShader, 
                                               	ssgi::MemBlock &memBlock, const void *sbFileContents, uint32_t sbFileSizeInBytes)
{
	ShaderFileInfo fileInfo;
	fileInfo.init(sbFileContents);

	// Validation:
	SCE_GNM_ASSERT(fileInfo.shaderFileHeader.m_type         == Gnmx::kLocalShader);

	const sce::Gnmx::LsShader *lsb = (const sce::Gnmx::LsShader *)fileInfo.shader;

	// Allocate, and copy the shader code + immediate constant buffer in GPU memory:
	const uint32_t		 lsb_offset		= lsb->m_lsStageRegisters.m_spiShaderPgmLoLs;
	loader->getHeap()->allocateVideoMemoryBlock(&memBlock, getShaderMemBlockSize(lsb->m_common), sce::Gnm::kAlignmentOfShaderInBytes);
	memcpy(memBlock.m_ptr, (const void*)(fileInfo.shader + lsb_offset), getShaderMemBlockSize(lsb->m_common));

	sce::Gnmx::LsShader *lsbFinal = (sce::Gnmx::LsShader*)malloc(lsb->computeSize());
	memcpy(lsbFinal, lsb, lsb->computeSize());
	lsbFinal->m_lsStageRegisters.m_spiShaderPgmLoLs = memBlock.getLoAddr256();
	lsbFinal->m_lsStageRegisters.m_spiShaderPgmHiLs = memBlock.getHiAddr256();

	lsShader = lsbFinal;

	return SCE_OK;
}

int ssgi::loadAndAllocateHsShaderFromMemory(ssgi::GraphicsLoaderImpl *loader,
											sce::Gnmx::HsShader *& hsShader, ssgi::MemBlock &memBlock, const void *sbFileContents, uint32_t sbFileSizeInBytes)
{
	ShaderFileInfo fileInfo;
	fileInfo.init(sbFileContents);

	// Validation:
	SCE_GNM_ASSERT(fileInfo.shaderFileHeader.m_type         == Gnmx::kHullShader);

	const Gnmx::HsShader *hsb = (const Gnmx::HsShader *)fileInfo.shader;

	// Allocate, and copy the shader code + immediate constant buffer in GPU memory:
	const uint32_t		 hsb_offset		= hsb->m_hsStageRegisters.m_spiShaderPgmLoHs;
	loader->getHeap()->allocateVideoMemoryBlock(&memBlock, getShaderMemBlockSize(hsb->m_common), sce::Gnm::kAlignmentOfShaderInBytes);
	memcpy(memBlock.m_ptr, (const void*)(fileInfo.shader + hsb_offset), getShaderMemBlockSize(hsb->m_common));

	Gnmx::HsShader *hsbFinal = (Gnmx::HsShader*)malloc(hsb->computeSize());
	memcpy(hsbFinal, hsb, hsb->computeSize());
	hsbFinal->m_hsStageRegisters.m_spiShaderPgmLoHs = memBlock.getLoAddr256();
	hsbFinal->m_hsStageRegisters.m_spiShaderPgmHiHs = memBlock.getHiAddr256();

	hsShader = hsbFinal;

	return SCE_OK;
}

int ssgi::loadAndAllocateEsShaderFromMemory(ssgi::GraphicsLoaderImpl *loader,
											sce::Gnmx::EsShader *&esShader, ssgi::MemBlock &memBlock, 
	                                                        const void *sbFileContents, uint32_t sbFileSizeInBytes)
{
	ShaderFileInfo fileInfo;
	fileInfo.init(sbFileContents);

	// Validation:
	SCE_GNM_ASSERT(fileInfo.shaderFileHeader.m_type         == Gnmx::kHullShader);

	const sce::Gnmx::EsShader *esb = (const sce::Gnmx::EsShader *)fileInfo.shader;

	// Allocate, and copy the shader code + immediate constant buffer in GPU memory:
	const uint32_t		 esb_offset		= esb->m_esStageRegisters.m_spiShaderPgmLoEs;
	loader->getHeap()->allocateVideoMemoryBlock(&memBlock, getShaderMemBlockSize(esb->m_common), sce::Gnm::kAlignmentOfShaderInBytes);
	memcpy(memBlock.m_ptr, (const void*)(fileInfo.shader + esb_offset), getShaderMemBlockSize(esb->m_common));

	sce::Gnmx::EsShader *esbFinal = (sce::Gnmx::EsShader*)malloc(esb->computeSize());
	memcpy(esbFinal, esb, esb->computeSize());
	esbFinal->m_esStageRegisters.m_spiShaderPgmLoEs = memBlock.getLoAddr256();
	esbFinal->m_esStageRegisters.m_spiShaderPgmHiEs = memBlock.getHiAddr256();

	esShader = esbFinal;

	return SCE_OK;
}

int ssgi::loadAndAllocateGsShaderFromMemory(ssgi::GraphicsLoaderImpl *loader,
											sce::Gnmx::GsShader *&gsShader, 
	                                  ssgi::MemBlock &gsMemBlock, 
									  ssgi::MemBlock &vsMemBlock, 
									  const void *sbFileContents, uint32_t sbFileSizeInBytes)
{
	ShaderFileInfo fileInfo;
	fileInfo.init(sbFileContents);

	// Validation:
	SCE_GNM_ASSERT(fileInfo.shaderFileHeader.m_type         == Gnmx::kGeometryShader);

	const sce::Gnmx::GsShader *gsb = (const sce::Gnmx::GsShader *)fileInfo.shader;

	// Allocate, and copy the shader code + immediate constant buffer in GPU memory:
	const uint32_t		gsb_offset		   = gsb->m_gsStageRegisters.m_spiShaderPgmLoGs;
	loader->getHeap()->allocateVideoMemoryBlock(&gsMemBlock, getShaderMemBlockSize(gsb->m_common), sce::Gnm::kAlignmentOfShaderInBytes);
	memcpy(gsMemBlock.m_ptr, (const void*)(fileInfo.shader + gsb_offset), getShaderMemBlockSize(gsb->m_common));


	const sce::Gnmx::VsShader *vsb	 = gsb->getCopyShader();
	// Allocate, and copy the shader code + immediate constant buffer in GPU memory:
	const uint32_t		vsb_offset		   = vsb->m_vsStageRegisters.m_spiShaderPgmLoVs;
	loader->getHeap()->allocateVideoMemoryBlock(&vsMemBlock, getShaderMemBlockSize(vsb->m_common), sce::Gnm::kAlignmentOfShaderInBytes);
	memcpy(vsMemBlock.m_ptr, (const void*)(fileInfo.shader + vsb_offset), getShaderMemBlockSize(vsb->m_common));


	sce::Gnmx::GsShader *gsbFinal = (sce::Gnmx::GsShader*)malloc(gsb->computeSize());
	memcpy(gsbFinal, gsb, gsb->computeSize());

	// Patch the regs value with the actual memory address:
	gsbFinal->m_gsStageRegisters.m_spiShaderPgmLoGs = gsMemBlock.getLoAddr256();
	gsbFinal->m_gsStageRegisters.m_spiShaderPgmHiGs = gsMemBlock.getHiAddr256();

	// Patch the regs value with the actual memory address:
	sce::Gnmx::VsShader *vsbFinal = (sce::Gnmx::VsShader *)gsbFinal->getCopyShader();
	vsbFinal->m_vsStageRegisters.m_spiShaderPgmLoVs = vsMemBlock.getLoAddr256();
	vsbFinal->m_vsStageRegisters.m_spiShaderPgmHiVs = vsMemBlock.getHiAddr256();

	gsShader = gsbFinal;

	return SCE_OK;
}


#endif// _SCE_TARGET_OS_ORBIS
