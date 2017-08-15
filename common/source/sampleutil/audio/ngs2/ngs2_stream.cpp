/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"
#include <scebase_common.h>
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#include <sampleutil/sampleutil_common.h>
#include <sampleutil/sampleutil_error.h>
#include <algorithm>
#include "ngs2_internal.h"

#pragma comment(lib,"libSceFios2_stub_weak.a")

namespace ssa = sce::SampleUtil::Audio;
namespace ssai =sce::SampleUtil::Audio::Internal;

int ssai::FakeAsyncFile::open(const char* path)
{
	m_file = fopen(path, "rb");
	if(m_file == NULL){
		return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
	}
	m_hasAsyncRead = false;
	return SCE_OK;
}

int ssai::FakeAsyncFile::close(void)
{
	SCE_SAMPLE_UTIL_ASSERT(!m_hasAsyncRead);
	
	fclose(m_file);
	m_file = NULL;
	return SCE_OK;
}

int ssai::FakeAsyncFile::asyncRead(void *dest, size_t offset, size_t size)
{
	SCE_SAMPLE_UTIL_ASSERT(!m_hasAsyncRead);
	int iret = fseek(m_file, offset, SEEK_SET);
	(void)iret;
	SCE_SAMPLE_UTIL_ASSERT(iret == 0);
	int nread = fread(dest, 1, size, m_file);
	retOfRead = nread;
	m_hasAsyncRead = true;
	return SCE_OK;
}

int ssai::FakeAsyncFile::waitAsyncRead(size_t *readSize)
{
	SCE_SAMPLE_UTIL_ASSERT(m_hasAsyncRead);
	m_hasAsyncRead = false;
	if(readSize != NULL){
		*readSize = retOfRead;
	}
	return retOfRead;
}


/* ------------------------------------------------------ */

int ssai::WaveReader::initialize(const char* path,
	const SceNgs2WaveformBlock *waveformBlocks,
	uint32_t numBlocks)
{
	int ret = m_file.open(path);
	if(ret != SCE_OK){
		return ret;
	}
	m_waveformBlocks = waveformBlocks;
	m_numBlocks = numBlocks;
	m_hasAsyncRead = false;

	reset();
	return SCE_OK;
}

void ssai::WaveReader::fixState(void)
{
	if(m_currentOffsetInBlock >= m_waveformBlocks[m_currentBlockIndex].dataSize){
		if(canLoop()){
			m_loopCount++;
			m_currentOffsetInBlock = 0;
		}else if(hasNextBlock()){
			m_currentOffsetInBlock = 0;
			m_currentBlockIndex++;
		}
	}

}

int ssai::WaveReader::finalize(void)
{
	SCE_SAMPLE_UTIL_ASSERT(!m_hasAsyncRead);
	return m_file.close();
}

int ssai::WaveReader::beginRead(Block *outBlock, void *dataBuffer, size_t databufferSize)
{
	SCE_SAMPLE_UTIL_ASSERT(!m_hasAsyncRead);
	SCE_SAMPLE_UTIL_ASSERT(outBlock != NULL);

	memset(outBlock, 0, sizeof(Block));

	const SceNgs2WaveformBlock *block;
	uint32_t readSize = 0;
	while(1){
		block = &m_waveformBlocks[m_currentBlockIndex];
		uint32_t remain = block->dataSize - m_currentOffsetInBlock;
	
		uint32_t bufsize = databufferSize;
		readSize = std::min(remain, bufsize);
		if(readSize > 0){
			break;
		}

		fixState();
		if(isFinished()){
			return SCE_SAMPLE_UTIL_ERROR_INVALID_STATE;
		}
	}


	uint32_t currentFileOffset = block->dataOffset + m_currentOffsetInBlock;
	int iret = m_file.asyncRead(dataBuffer, currentFileOffset, readSize);
	if(iret < SCE_OK){
		fprintf(stderr, __FILE__"(%d) invalid read size : %#x\n", __LINE__, iret);
		m_currentBlockIndex = (m_numBlocks-1);
		m_currentOffsetInBlock = m_waveformBlocks[m_currentBlockIndex].dataSize;
		return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
	}

	outBlock->waveformBlock.dataOffset     = 0;
	outBlock->waveformBlock.dataSize       = readSize;
	outBlock->waveformBlock.numRepeats     = 0;
	outBlock->waveformBlock.numSamples     = block->numSamples;
	outBlock->waveformBlock.numSkipSamples = block->numSkipSamples;
	outBlock->waveformBlock.reserved       = 0;
	outBlock->waveformBlock.userData       = 0;
	outBlock->m_data                       = dataBuffer;
	outBlock->waveformBlockFlag            = 0;
	
	outBlock->waveformBlockFlag |= (m_currentOffsetInBlock == 0) ? 0 : SCE_NGS2_WAVEFORM_BLOCK_FLAG_ONLY_DATA;
	
	m_currentOffsetInBlock += readSize;

	fixState();
	if(!isFinished()){
		outBlock->waveformBlockFlag |= SCE_NGS2_WAVEFORM_BLOCK_FLAG_CONTINUOUS;
	}
	m_hasAsyncRead = true;
	return SCE_OK;
}

int ssai::WaveReader::wait(void)
{
	SCE_SAMPLE_UTIL_ASSERT(m_hasAsyncRead);
	m_hasAsyncRead = false;

	int iret = m_file.waitAsyncRead(NULL);
	if(iret < SCE_OK){
		fprintf(stderr, __FILE__"(%d) invalid read size\n", __LINE__);
		m_currentBlockIndex = (m_numBlocks-1);
		m_currentOffsetInBlock = m_waveformBlocks[m_currentBlockIndex].dataSize;
		return iret;
	}

	return SCE_OK;
}

bool ssai::WaveReader::isFinished(void)
{
	if(hasNextBlock()){
		return false;
	}
	if(canLoop()){
		return false;
	}
	return true;
}
void ssai::WaveReader::reset(void)
{
	m_currentBlockIndex  = 0;
	m_currentOffsetInBlock = 0;
	m_loopCount = 0;
}

/* ----------------------------------------------------------------- */

int ssai::Stream::initialize(const char* path,
	const SceNgs2WaveformBlock *waveformBlocks,
	uint32_t numBlocks)
{
	int ret = m_waveReader.initialize(path, waveformBlocks, numBlocks);
	if(ret != SCE_OK){
		return ret;
	}
	m_hasAsyncRead = false;
	reset();
	return SCE_OK;
}

int ssai::Stream::finalize(void)
{
	if(m_hasAsyncRead){
		m_waveReader.wait();
		m_hasAsyncRead = false;
	}
	m_waveReader.finalize();
	return SCE_OK;
}

void ssai::Stream::reset(void)
{
	if(m_hasAsyncRead){
		m_waveReader.wait();
		m_hasAsyncRead = false;
	}
	m_waveReader.reset();
	m_nextBufferIndex = 0;


	kickNext();
}

void ssai::Stream::kickNext(void)
{
	SCE_SAMPLE_UTIL_ASSERT(!m_hasAsyncRead);
	if(m_waveReader.isFinished()){
		return;
	}
	m_hasAsyncRead = true;
	int ret = m_waveReader.beginRead(&m_readingBlock, m_buffers[m_nextBufferIndex], BUFFER_SIZE);
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
}

ssai::Block ssai::Stream::readNext(void)
{
	int ret;
	if(!m_hasAsyncRead){
		kickNext();
	}
	if(isFinished()){
		ssai::Block b;
		memset(&b, 0, sizeof(b));
		return b;
	}

	
	ret = m_waveReader.wait();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	m_hasAsyncRead = false;
	Block block = m_readingBlock;

	m_nextBufferIndex = (m_nextBufferIndex + 1) % NUM_BUFFERS;
	return block;
}
bool ssai::Stream::isFinished(void)
{
	if(m_hasAsyncRead){
		return false;
	}
	return m_waveReader.isFinished();
}

#endif // defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS