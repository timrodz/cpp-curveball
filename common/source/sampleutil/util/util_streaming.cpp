/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "util.h"

namespace ssiu = sce::SampleUtil::Internal::Util;

#if _SCE_TARGET_OS_WINDOWS
#pragma comment( lib, "libfios2.lib" )
#endif 

// --------------------------------------------------------------------------

int ssiu::RoFile::open(const char *filePath)
{
	m_file = fopen(filePath, "r");
	if(m_file == NULL){
		return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
	}
	//setvbuf(m_file, &b, _IONBF, 1);
	
	return SCE_OK;
}

int ssiu::RoFile::seek(long offset)
{
	return fseek(m_file, offset, SEEK_SET);
}

long ssiu::RoFile::tell(void)
{
	return ftell(m_file);
}


int ssiu::RoFile::read(void *buff, size_t size, size_t *preadSize)
{
	size_t ret;
	size_t readSize = 0;
	size_t remain = size;
	while(remain > 0){
		ret = fread(buff, 1, remain, m_file);
		if((ret == 0) && (ferror(m_file)!=0)){
			//printf(__FILE__"(%d) LoopFile::read failed\n", __LINE__);
			return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
		}
		remain -= ret;
		readSize += ret;
		if(ret == 0){
			break;
		}
	}
	if(preadSize != NULL){
		*preadSize = readSize;
	}
	return 0;
}

int ssiu::RoFile::asyncRead(void *buff, size_t size)
{
	m_asyncReadSize = size;
	int ret =read(buff, size, NULL);
	return (ret>=0) ? SCE_OK : -1;
}

int ssiu::RoFile::cancelAsyncRead(void)
{
	return SCE_OK;
}

int ssiu::RoFile::waitAsyncRead(size_t *readSize)
{
	if(readSize != NULL){
		*readSize = m_asyncReadSize;
	}
	return SCE_OK;
}



int ssiu::RoFile::close(void)
{
	return fclose(m_file);
}


// --------------------------------------------------------------------------



int ssiu::Fios2RoFile::open(const char *filePath)
{
	m_op = SCE_FIOS_HANDLE_INVALID;
	return sceFiosFHOpenSync(
		NULL, //const SceFiosOpAttr *pAttr,
		&m_fileHandle,
		filePath,
		NULL // readonly, const SceFiosOpenParams *pOpenParams
		);
}

long ssiu::Fios2RoFile::seek(int offset)
{
	return sceFiosFHSeek(m_fileHandle, offset, SCE_FIOS_SEEK_SET);
}

long ssiu::Fios2RoFile::tell(void)
{
	return sceFiosFHTell(m_fileHandle);
}

int ssiu::Fios2RoFile::read(void *buff, uint32_t size, size_t *readSize)
{
	SceFiosSize ret = sceFiosFHReadSync(
		NULL, //const SceFiosOpAttr *pAttr,
		m_fileHandle,
		buff,
		size
		);
	if(ret >=0){
		if(readSize != NULL){
			*readSize = (size_t)ret;
		}
		return SCE_OK;
	}else{
		return (int)ret;
	}
}

int ssiu::Fios2RoFile::asyncRead(void *buff, uint32_t size)
{
	if(m_op != SCE_FIOS_HANDLE_INVALID){
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
	return SCE_OK;
}

int ssiu::Fios2RoFile::cancelAsyncRead(void)
{
	if(m_op == SCE_FIOS_HANDLE_INVALID){
		return -1;
	}
	sceFiosOpDelete(m_op);
	m_op = SCE_FIOS_HANDLE_INVALID;
	return SCE_OK;
}

int ssiu::Fios2RoFile::waitAsyncRead(size_t *readSize)
{
	if(m_op == SCE_FIOS_HANDLE_INVALID){
		return -1;
	}
	SceFiosSize ret = sceFiosOpSyncWaitForIO(m_op);
	m_op = SCE_FIOS_HANDLE_INVALID;
	if(readSize != NULL){
		*readSize = (size_t)ret;
	}
	return SCE_OK;
}


int ssiu::Fios2RoFile::close(void)
{
	return sceFiosFHCloseSync(NULL, m_fileHandle);
}


// --------------------------------------------------------------------------



	
int ssiu::LoopFile::open(const char *filePath, int loopNum,
						long dataOffset,
						size_t dataSize,
						long loopStartOffset,
						long loopEndOffset
	)
{
	int ret;

	m_dataOffset      = dataOffset;
	m_dataSize        = dataSize;
	m_loopStartOffset = loopStartOffset;
	m_loopEndOffset   = loopEndOffset;

	ret = m_file.open(filePath);
	if(ret < SCE_OK){
		return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
	}

	m_loopNum = loopNum;
	m_curentLoopCount = 0;

	long offset = m_file.seek(m_dataOffset);
	SCE_SAMPLE_UTIL_ASSERT(offset>=0);

	m_looped = false;
	return SCE_OK;
}


uint32_t ssiu::LoopFile::getNextReadSize(uint32_t size)
{
	int currentOffset = m_file.tell();

	bool reachEnd = (currentOffset + size) >= (m_dataOffset + m_dataSize);
	if(reachEnd){
		size = (m_dataOffset + m_dataSize) - currentOffset;
	}

	bool canLoop = (m_loopNum < 0) || (m_curentLoopCount < m_loopNum);
	bool reachLoopEnd = (currentOffset < m_loopEndOffset) && ((currentOffset + size) >= m_loopEndOffset);

	if(canLoop && reachLoopEnd){
		size = m_loopEndOffset - currentOffset;
	}
	return size;
}


int ssiu::LoopFile::read(void *buffer, uint32_t size, int *offsetOfData, int *loopFlag)
{
	if(loopFlag == NULL){
		return -1;
	}
	if(offsetOfData == NULL){
		return -1;
	}

	*loopFlag = 0;

	int currentOffset = m_file.tell();
	if(m_looped && (currentOffset == m_loopStartOffset)){
		*loopFlag |= STREAM_BUFFER_FLAG_LOOP_BEGIN;
	}

	size = getNextReadSize(size);

	*offsetOfData = currentOffset;


	int ret = m_file.read(buffer, size, NULL);

	m_looped = false;
	if(m_file.tell() == m_loopEndOffset){
		bool canLoop = (m_loopNum < 0) || (m_curentLoopCount < m_loopNum);
		if(canLoop){
			*loopFlag |= STREAM_BUFFER_FLAG_LOOP_END;
			int tmpret = m_file.seek(m_loopStartOffset);
			SCE_SAMPLE_UTIL_ASSERT(tmpret>=0);
			m_curentLoopCount++;
			m_looped = true;
		}
	}
	SCE_SAMPLE_UTIL_ASSERT(m_file.tell() < (m_dataOffset + m_dataSize));
	return ret;
}

int ssiu::LoopFile::asyncRead(void *buffer, uint32_t size, size_t *outReadRequestSize)
{
	m_asyncReadOffset = m_file.tell();

	size = getNextReadSize(size);
	if(size <= 0){
		return 0;
	}

	int ret = m_file.asyncRead(buffer, size);
	if(outReadRequestSize != NULL){
		*outReadRequestSize = size;
	}
	return (ret>=0) ? SCE_OK : -1;
}

int ssiu::LoopFile::waitAsyncRead(int *offsetOfData, int *loopFlag)
{
	size_t size=0;
	int ret = m_file.waitAsyncRead(&size);
	if(ret < SCE_OK){
		return ret;
	}
	*offsetOfData = m_asyncReadOffset;
	if(m_looped && (m_asyncReadOffset == m_loopStartOffset)){
		*loopFlag |= STREAM_BUFFER_FLAG_LOOP_BEGIN;
	}

	m_looped = false;
	if(m_file.tell() == m_loopEndOffset){
		bool canLoop = (m_loopNum < 0) || (m_curentLoopCount < m_loopNum);
		if(canLoop){
			*loopFlag |= STREAM_BUFFER_FLAG_LOOP_END;
			int tmpret = m_file.seek(m_loopStartOffset);
			SCE_SAMPLE_UTIL_ASSERT(tmpret>=0);
			m_curentLoopCount++;
			m_looped = true;
		}
	}
	SCE_SAMPLE_UTIL_ASSERT(m_file.tell() <= (m_dataOffset + m_dataSize));
	return size;

}


void ssiu::LoopFile::reset(void)
{
	int tmpret = m_file.seek(m_dataOffset);
	SCE_SAMPLE_UTIL_ASSERT(tmpret>=0);

	m_curentLoopCount = 0;
}

int ssiu::LoopFile::setPosition(int pos)
{
	int tmpret = m_file.seek(pos);
	SCE_SAMPLE_UTIL_ASSERT(tmpret>=0);
	return SCE_OK;
}

void ssiu::LoopFile::setLoop(int loopNum)
{
	m_loopNum = loopNum; 
	m_curentLoopCount = 0;
}

void ssiu::LoopFile::setLoop(int loopNum, int startPos, int size)
{
	m_loopNum = loopNum; 
	m_curentLoopCount = 0;

	m_loopStartOffset = startPos;
	m_loopEndOffset   = startPos + size;

	return ;
}

int ssiu::LoopFile::cancelAsyncRead(void)
{
	return m_file.cancelAsyncRead();
}


int ssiu::LoopFile::getCurrentOffset(void)
{
	return m_file.tell();
}

int ssiu::LoopFile::close(void)
{
	return m_file.close();
}




// --------------------------------------------------------------------------


ssiu::Stream::Stream(void)
{
	m_isOpen = false;
}
ssiu::Stream::~Stream(void)
{
	if(m_isOpen){
		close();
	}
}

int ssiu::Stream::open(const char *filePath, int loopNum, int align,
	size_t dataOffset,
	size_t dataSize,
	size_t loopStartOffset,
	size_t loopEndOffset
	)
{
	int ret;
	m_useAsyncIo = true;

	m_bufferSize = STREAM_BUFFER_SIZE;
	if((align > 0) && ((STREAM_BUFFER_SIZE % align) != 0)){
		if(align > STREAM_BUFFER_SIZE){
			//printf(__FILE__"(%d) play \n", __LINE__);
			return -1;
		}
		int n = STREAM_BUFFER_SIZE / align;
		m_bufferSize = align * n;
	} 

	ret = m_loopFile.open(filePath, loopNum,
		dataOffset,
		dataSize,
		loopStartOffset,
		loopEndOffset);


	if(ret < SCE_OK){
		//printf(__FILE__"(%d) play \n", __LINE__);
		return ret;
	}

	m_lastUsedBuffer = 0;
	m_loadingBufferIndex = -1;
	m_loadingRequestSize = 0;

	m_isOpen = true;
	return SCE_OK;
}


int ssiu::Stream::getStreamingData(void** buffer, int *dataFileOffset, char* bufferState)
{
	if(m_useAsyncIo){
		return getStreamingDataAsync(buffer, dataFileOffset, bufferState);
	}else{
		return getStreamingDataSync(buffer, dataFileOffset, bufferState);
	}
}

void ssiu::Stream::reset(void)
{
	if(m_useAsyncIo){
		cancelLoad();
	}
	m_loopFile.reset();
}

void ssiu::Stream::setLoop(int loopNum)
{
	if(m_useAsyncIo){
		cancelLoad();
	}
	m_loopFile.setLoop(loopNum);
}

int ssiu::Stream::close(void)
{
	if(m_useAsyncIo){
		cancelLoad();
	}
	int ret = m_loopFile.close();
	if(ret == SCE_OK){
		m_isOpen = false;
	}
	return ret;
}



int ssiu::Stream::getStreamingDataSync(void** buffer, int *dataFileOffset, char* bufferState)
{
	int ret;
	m_lastUsedBuffer = (m_lastUsedBuffer + 1) % STREAM_BUFFER_NUM;
	void *buf = m_streamBuffer[m_lastUsedBuffer];
	int loopState = 0;

	ret = m_loopFile.read(buf, m_bufferSize, dataFileOffset, &loopState);

	*buffer = buf;
	*bufferState = loopState;

	return ret;
}

void ssiu::Stream::kickNextLoad(int bufferIndex)
{
	SCE_SAMPLE_UTIL_ASSERT(m_loadingBufferIndex == -1);
	void *buf = m_streamBuffer[bufferIndex];
	int ret = m_loopFile.asyncRead(buf, m_bufferSize, &m_loadingRequestSize); // ret is request size
	if(ret >= 0){
		m_loadingBufferIndex = bufferIndex;
	}
}

int ssiu::Stream::waitLoad(int *dataFileOffset, char* bufferState)
{
	*dataFileOffset = 0;
	*bufferState    = 0;

	if (m_loadingBufferIndex != -1){
		m_lastUsedBuffer = m_loadingBufferIndex;
		int loopState = 0;
		int ret = m_loopFile.waitAsyncRead(dataFileOffset, &loopState); // ret is size result.
		*bufferState = loopState;
		m_loadingBufferIndex = -1;
		m_loadingRequestSize = 0;
		return ret;
	}else{
		return 0;
	}
}
void ssiu::Stream::cancelLoad(void)
{
	if(m_loadingRequestSize > 0){
		int ret = m_loopFile.cancelAsyncRead();
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_loadingBufferIndex = -1;
		m_loadingRequestSize = 0;
	}else{
	}
}

int ssiu::Stream::getStreamingDataAsync(void** buffer, int *dataFileOffset, char* bufferState)
{
	if(m_loadingBufferIndex == -1){
		int nextBuffer = (m_lastUsedBuffer + 1) % STREAM_BUFFER_NUM;
		kickNextLoad(nextBuffer);
	}

	int size = waitLoad(dataFileOffset, bufferState);
	void *buf = m_streamBuffer[m_lastUsedBuffer];

	*buffer = buf;

	int nextBuffer = (m_lastUsedBuffer + 1) % STREAM_BUFFER_NUM;
	kickNextLoad(nextBuffer);

	return size;
}





// --------------------------------------------------------------------------

