/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_UTIL_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_UTIL_H
#include <scebase_common.h>


#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <string>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil/sampleutil_common.h>
#include <fios2.h>

#if _SCE_TARGET_OS_PSP2
#include <kernel.h>
#endif

#if _SCE_TARGET_OS_ORBIS
#include <kernel.h>
#endif
#if _SCE_TARGET_OS_WINDOWS
#include <Windows.h>
#include <Cg/cg.h> 
#endif


namespace sce
{
	namespace SampleUtil
	{
		namespace Internal
		{

			namespace Util
			{

#if _SCE_TARGET_OS_WINDOWS

				void checkForCgError( const char * situation, bool _exit = true );
#endif


                std::string getExtention(const std::string &path);


				void* malloc_and_load(const char* path, uint32_t *pSize);

				int loadFile(void *fileImage, const char *filename, uint32_t *pSize, uint32_t maxFileSize);

				static inline uint32_t ceil(uint32_t x, uint32_t a)
				{
					return (((x) + ((a) - 1)) & ~((a) - 1));
				}
				

			}
		}
	}
}


static inline uint32_t ceilPowersOf2(uint32_t x)	/* 4096 limit */
{
	uint32_t ret = 1;
	for(uint32_t i=0; i<12; i++){
		ret *=2;
		if(x <= ret){
			break;
		}
	}
	return ret;
}
static inline uint32_t maxUint32(uint32_t a, uint32_t b)
{
	return (a<b)?b:a;
}

namespace sce
{
	namespace SampleUtil
	{
		namespace Internal
		{

			namespace Util
			{


				class Mutex
				{
				private:
#if _SCE_TARGET_OS_PSP2
					std::string m_name;
					SceKernelLwMutexWork m_work;
#endif
#if _SCE_TARGET_OS_WINDOWS
                    HANDLE m_mutexHandle;
#endif
#if _SCE_TARGET_OS_ORBIS
					ScePthreadMutexattr m_mutexAttr;
					ScePthreadMutex m_mutex;
#endif
				public:
					Mutex(SCE_SAMPLE_LPCTSTR name=NULL);
					int lock(void);
					int unlock(void);
					~Mutex(void);
				};

				class FocusLock
				{
				private:
					Mutex *m_mutex;
				public:
					FocusLock(Mutex *mutex);

					~FocusLock(void);
				};



				class Reader
				{
				public:
					FILE *m_file;
					size_t m_size;
					const uint8_t *m_data;
					uint32_t m_dataOffset;

					bool m_closeOnFinalize;

					enum Endian { ENDIAN_BIG, ENDIAN_LITTLE };
					Endian m_endian;

					Reader(void);

					int initializeFromFile(const char *filename);

					int initializeFromFileStream(FILE *file);

					int initializeFromMemory(const void *data, uint32_t size);

					int finalize(void);

					int read(void *buffer, size_t size);

					int readInt32(int32_t *val);

					int readUint32(uint32_t *val);

					int readUint16(uint16_t *val);

					int readUint8(uint8_t *val);

					int seek(long offset);

					int skip(size_t size);

					uint32_t tell(void);

					size_t getSize(void);

					~Reader(void);

					void setEndian(Endian e);
				private:
					static Endian getCpuEndian(void);

					void fixEndian(void *data, uint32_t size);
				};

				class RoFile
				{
				private:
					FILE *m_file;
					//char b;
                    size_t m_asyncReadSize;
				public:
					int open(const char *filePath);

					int seek(long offset);

					long tell(void);

					int read(void *buff, size_t size, size_t *preadSize);

					int asyncRead(void *buff, size_t size);

					int cancelAsyncRead(void);

					int waitAsyncRead(size_t *readSize);

					int close(void);
				};

#if 1
				class Fios2RoFile
				{
				private:
					SceFiosFH m_fileHandle;
					SceFiosOp m_op;
				public:
					int open(const char *filePath);

					long seek(int offset);

					long tell(void);

					int read(void *buff, uint32_t size, size_t *readSize);

					int asyncRead(void *buff, uint32_t size);

					int cancelAsyncRead(void);

					int waitAsyncRead(size_t *readSize);

					int close(void);
				};
#else
				typedef RoFile Fios2RoFile;
#endif 


				static const int STREAM_BUFFER_FLAG_LOOP_BEGIN =	(1 << 0);
				static const int STREAM_BUFFER_FLAG_LOOP_END	=	(1 << 1);


				class LoopFile 
				{
				private:
					sce::SampleUtil::Internal::Util::Fios2RoFile m_file;
					
					long m_dataOffset;
					size_t m_dataSize;
					long m_loopStartOffset;
					long m_loopEndOffset;

					int32_t m_loopNum;
					int32_t m_curentLoopCount;

					int m_asyncReadOffset;
                    bool m_looped;
				public:
					int open(const char *filePath, int loopNum,
						long dataOffset,
						size_t dataSize,
						long loopStartOffset,
						long loopEndOffset
						);

					uint32_t getNextReadSize(uint32_t size);

					int read(void *buff, uint32_t size, int *offsetOfData, int *loopFlag);

					int asyncRead(void *buff, uint32_t size, size_t *outReadRequestSize);
					int waitAsyncRead(int *offsetOfData, int *loopFlag);
					int cancelAsyncRead(void);

					int getCurrentOffset(void);

					void reset(void);

					void setLoop(int loopNum);
					void setLoop(int loopNum, int startPos, int size);
					int setPosition(int pos);
					int close(void);
				};


				class Stream 
				{
				public:
					static const int 	STREAM_BUFFER_SIZE		=	(2 * 1024);

				private:
					static const int 	STREAM_BUFFER_NUM	=		3;

					short m_streamBuffer[STREAM_BUFFER_NUM][STREAM_BUFFER_SIZE/sizeof(short)];
					uint32_t m_bufferSize;
					int     m_lastUsedBuffer;
					int32_t m_loadingBufferIndex;
					size_t     m_loadingRequestSize;

					sce::SampleUtil::Internal::Util::LoopFile m_loopFile;
					bool m_useAsyncIo;
					bool m_isOpen;
				public:
					Stream(void);

					~Stream(void);

					int open(const char *filePath, int loopNum, int align,
						size_t dataOffset,  size_t dataSize,  size_t loopStartOffset,	size_t loopEndOffset);

					int getStreamingData(void** buffer, int *dataFileOffset, char* bufferState);

					void reset(void);

					void setLoop(int loopNum);

					int close(void);

				private:
					int getStreamingDataSync(void** buffer, int *dataFileOffset, char* bufferState);

					void kickNextLoad(int bufferIndex);

					int waitLoad(int *dataFileOffset, char* bufferState);

					void cancelLoad(void);

					int getStreamingDataAsync(void** buffer, int *dataFileOffset, char* bufferState);
				};

			}
		}
	}
}


#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_UTIL_H */
