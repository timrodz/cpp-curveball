/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */



#include <fios2.h>
#include <ngs2.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Audio
		{
			namespace Internal
			{
				inline static void dumpSceNgs2WaveformBlock(const SceNgs2WaveformBlock *aBlock, uint32_t n)
				{
					for(uint32_t i=0; i<n;i++){
						printf("Of=%#x,sz=%#x,rp=%#x,ss=%#x, ns=%#x,r=%#x,U=%#lx\n",
							aBlock[i].dataOffset,
							aBlock[i].dataSize,
							aBlock[i].numRepeats,
							aBlock[i].numSkipSamples,
							aBlock[i].numSamples,
							aBlock[i].reserved,
							aBlock[i].userData);
						
					}
				}

				struct Block 
				{
					SceNgs2WaveformBlock waveformBlock;
					void *m_data;
					uint32_t waveformBlockFlag;
				};


				class Fios2AsyncFile
				{
				private:
					SceFiosFH m_fileHandle;
					SceFiosOp m_op;
				public:
					int open(const char *filePath)
					{
						m_op = SCE_FIOS_HANDLE_INVALID;
						int ret = sceFiosFHOpenSync(
							NULL, //const SceFiosOpAttr *pAttr,
							&m_fileHandle, 
							filePath, 
							NULL // readonly, const SceFiosOpenParams *pOpenParams
							);
						//printf("ret=%#x\n", ret);
						return ret;
					}

					int seek(int offset)
					{
						SceFiosOffset ret = sceFiosFHSeek(m_fileHandle, offset, SCE_FIOS_SEEK_SET);
						if(ret < 0){
							return -1;
						}
						return SCE_OK;
					}

					int64_t tell(void)
					{
						return sceFiosFHTell(m_fileHandle);
					}

					size_t read(void *buff, uint32_t size)
					{
						return sceFiosFHReadSync(
							NULL, //const SceFiosOpAttr *pAttr,
							m_fileHandle, 
							buff, 
							size
							);
					}

					int asyncRead(void *buff, uint32_t size)
					{
						if(m_op != SCE_FIOS_HANDLE_INVALID){
							//SCE_SAMPLE_UTIL_ASSERT(0);
							return -1;
						}
						m_op = sceFiosFHRead(
							NULL, //const SceFiosOpAttr *pAttr,
							m_fileHandle, 
							buff, 
							size
							);
						if(m_op <= 0){
							printf("Error: sceFiosFHRead failed. sceFiosInitialize may not be called.\n");
							return m_op;
						}
						SCE_SAMPLE_UTIL_ASSERT(m_op != SCE_FIOS_HANDLE_INVALID);
						return SCE_OK;
					}

					int asyncRead(void *dest, size_t offset, size_t size)
					{
						off_t ret = seek(offset);
						if(ret < SCE_OK){
							return ret;
						}
						ret = asyncRead(dest, size);
						//printf(__FILE__"(%d) ret=%#x\n", __LINE__, ret);
						return ret;
					}

					int cancelAsyncRead(void)
					{
						if(m_op == SCE_FIOS_HANDLE_INVALID){
							return -1;
						}
						sceFiosOpDelete(m_op);
						m_op = SCE_FIOS_HANDLE_INVALID;
						return SCE_OK;
					}

					int waitAsyncRead(size_t *readSize)
					{
						if(m_op == SCE_FIOS_HANDLE_INVALID){
							return -1;
						}
						SceFiosSize ret = sceFiosOpSyncWaitForIO(m_op);
						if(readSize != NULL){
							*readSize = ret;
						}
						m_op = SCE_FIOS_HANDLE_INVALID;
						return SCE_OK;
					}


					int close(void)
					{
						return sceFiosFHCloseSync(NULL, m_fileHandle);
					}

				};


				class FakeAsyncFile
				{
				private:
					FILE *m_file;
					int retOfRead;
					bool m_hasAsyncRead;
				public:
					int open(const char* path);
					int asyncRead(void *dest, size_t offset, size_t size);
					int waitAsyncRead(size_t *readSize);
					int close(void);
				};

#ifndef USE_FAKE_ASYNC
				typedef Fios2AsyncFile AsyncFile;
#else
				typedef FakeAsyncFile AsyncFile;
#endif
				

				class WaveReader
				{
				private:
					AsyncFile m_file;
					const SceNgs2WaveformBlock *m_waveformBlocks;
					uint32_t m_numBlocks;

					uint32_t m_currentBlockIndex;
					uint32_t m_currentOffsetInBlock;
					uint32_t m_loopCount;
					bool m_hasAsyncRead;

					bool canLoop(void)
					{
						if(m_currentBlockIndex >= m_numBlocks){
							return false;
						}
						return m_waveformBlocks[m_currentBlockIndex].numRepeats > m_loopCount;
					}
					bool hasNextBlock(void)
					{
						return m_currentBlockIndex < (m_numBlocks-1);
					}
					void fixState(void);

				public:
					int initialize(const char* path,
						const SceNgs2WaveformBlock *waveformBlocks,
						uint32_t numBlocks);

					int finalize(void);

					int beginRead(Block *outBlock, void *dataBuffer, size_t databufferSize);
					int wait(void);

					bool isFinished(void);
					void reset(void);
				};					


				class Stream
				{
				public:
					static const uint32_t BUFFER_SIZE=16*1024;
				private:
					static const uint32_t NUM_BUFFERS=3;
					WaveReader m_waveReader;

					Block m_readingBlock;
					bool m_hasAsyncRead;
					char m_buffers[NUM_BUFFERS][BUFFER_SIZE];
					uint32_t m_nextBufferIndex;

					std::vector<Block> m_availableBlocks;
					std::vector<Block> m_usingBlocks;

					void kickNext(void);
				public:
					int initialize(const char* path, 
						const SceNgs2WaveformBlock *waveformBlocks,
						uint32_t numBlocks);

					int finalize(void);

					Block readNext(void);
					void reset(void);
					bool isFinished(void);

				};

			}

		}
	}
}