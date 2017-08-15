/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "util.h"


namespace util = sce::SampleUtil::Internal::Util;

#if _SCE_TARGET_OS_WINDOWS

void util::checkForCgError( const char * situation, bool _exit )
{
	CGerror error;
	const char *string = cgGetLastErrorString(&error);

	if( error != CG_NO_ERROR )
	{
		printf(string);
		SCE_SAMPLE_UTIL_ASSERT(0);
		if( _exit )
			exit(1);
	}
}

#endif /* _SCE_TARGET_OS_WINDOWS */




std::string util::getExtention(const std::string &path)
{
	int p = path.find_last_of(".");
	if(p == path.npos){
		return "";
	}
	std::string ext = path.substr(p+1, path.npos);
	return ext;
}


void* util::malloc_and_load(const char* path, uint32_t *pSize)
{
	int ret;

	FILE *fp = fopen(path, "rb");
	if(fp == NULL){
		fprintf(stderr, "Error : Opening %s failed.\n", path);
		return NULL;
	}
	ret = fseek(fp, 0, SEEK_END);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);

	int32_t size = ftell(fp);
	SCE_SAMPLE_UTIL_ASSERT(size > 0);

	ret = fseek(fp, 0, SEEK_SET);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);

	void *elf = malloc(size);
	if(elf == NULL){
		fprintf(stderr, "Error : No memory. %s couldn't be loaded.\n", path);
		return NULL;
	}

	ret = fread(elf, 1, size, fp);
	SCE_SAMPLE_UTIL_ASSERT(ret == size);

	ret = fclose(fp);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);

	if(pSize != NULL){
		*pSize = size;
	}

	(void)ret;
	return elf;
}

int util::loadFile(void *fileImage, const char *filename, uint32_t *pSize, uint32_t maxFileSize)
{
	FILE *file = fopen(filename, "rb");
	if(file == NULL){
		return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
	}

	int ret = fseek(file, 0, SEEK_END);
	if(ret != 0){
		fclose(file);
		return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
	}
	uint32_t filesize = ftell(file);
	ret = fseek(file, 0, SEEK_SET);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	if(filesize > maxFileSize){
		printf("file too large (bufSize %d byte)\n", filesize);
		return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
	}

	ret = fread(fileImage, 1, filesize, file);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, filesize );

	fclose(file);

	if(pSize != NULL){
		*pSize = filesize;
	}

	return SCE_OK;
}

/* ----------------------------------------------------------------------------- */




#if _SCE_TARGET_OS_PSP2

util::Mutex::Mutex(const char* name)
{
	if(name == NULL){
		name = "AnonymousLock";
	}
	m_name = name;
	SceInt32 ret = sceKernelCreateLwMutex(
		&m_work,
		m_name.c_str(),
		SCE_KERNEL_LW_MUTEX_ATTR_TH_FIFO | SCE_KERNEL_LW_MUTEX_ATTR_RECURSIVE,
		0, //initCount
		NULL
		);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
}

int util::Mutex::lock(void)
{
	int ret = sceKernelLockLwMutex(&m_work, 1, NULL);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	return ret;
}

int util::Mutex::unlock(void)
{
	int ret = sceKernelUnlockLwMutex(&m_work, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	return ret;
}

util::Mutex::~Mutex(void)
{
	SceInt32 ret = sceKernelDeleteLwMutex(&m_work);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
}

#endif

#if _SCE_TARGET_OS_WINDOWS


util::Mutex::Mutex(SCE_SAMPLE_LPCTSTR name)
{
	m_mutexHandle = CreateMutex(NULL, FALSE, name);
	SCE_SAMPLE_UTIL_ASSERT(m_mutexHandle != NULL);
}

int util::Mutex::lock(void)
{
	DWORD ret = WaitForSingleObject(m_mutexHandle, INFINITE);
	SCE_SAMPLE_UTIL_ASSERT(ret == WAIT_OBJECT_0);
	return SCE_OK;
}

int util::Mutex::unlock(void)
{
	BOOL ret = ReleaseMutex(m_mutexHandle);
	SCE_SAMPLE_UTIL_ASSERT(ret);
	return SCE_OK;
}

util::Mutex::~Mutex(void)
{
	BOOL ret = CloseHandle(m_mutexHandle);
	SCE_SAMPLE_UTIL_ASSERT(ret);
}


#endif

#if _SCE_TARGET_OS_ORBIS


util::Mutex::Mutex(const char *name)
{
	scePthreadMutexattrInit(&m_mutexAttr);
	int ret = scePthreadMutexattrSettype(&m_mutexAttr, SCE_PTHREAD_MUTEX_RECURSIVE);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	ret = scePthreadMutexInit(&m_mutex,
		&m_mutexAttr,
		name);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
}

int util::Mutex::lock(void)
{
	return scePthreadMutexLock(&m_mutex);
}

int util::Mutex::unlock(void)
{
	return scePthreadMutexUnlock(&m_mutex);
}

util::Mutex::~Mutex(void)
{
	scePthreadMutexattrDestroy(&m_mutexAttr);
	scePthreadMutexDestroy(&m_mutex);
}

#endif


/* ----------------------------------------------------------------------------- */

util::FocusLock::FocusLock(util::Mutex *mutex)
{
	m_mutex = mutex;
	int ret = m_mutex->lock();
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
}

util::FocusLock::~FocusLock(void)
{
	int ret = m_mutex->unlock();
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
}

/* ----------------------------------------------------------------------------- */

util::Reader::Reader(void)
{
	m_file = NULL;
	m_size = 0;
	m_closeOnFinalize = false;
	m_endian = ENDIAN_LITTLE;
	m_data = NULL;
	m_dataOffset = 0;
}

int util::Reader::initializeFromFile(const char *filename)
{
	m_file = fopen(filename, "rb");
	if(m_file == NULL){
		return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
	}
	
	int ret = fseek(m_file, 0, SEEK_END);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);
	
	m_size = ftell(m_file);
	
	ret = fseek(m_file, 0, SEEK_SET);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);
	
	m_closeOnFinalize = true;
	
	return SCE_OK;
}

int util::Reader::initializeFromFileStream(FILE *file)
{
	m_file = file;
	
	int ret = fseek(m_file, 0, SEEK_END);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);
	
	m_size = ftell(m_file);
	
	ret = fseek(m_file, 0, SEEK_SET);
	SCE_SAMPLE_UTIL_ASSERT(ret == 0);
	
	m_closeOnFinalize = false;
	
	return SCE_OK;
}

int util::Reader::initializeFromMemory(const void *data, uint32_t size)
{
	m_data = (const uint8_t*)data;
	m_size = size;
	m_dataOffset = 0;
	return SCE_OK;
}


int util::Reader::finalize(void)
{
	if(m_file != NULL){
		if(m_closeOnFinalize){
			fclose(m_file);
		}
		m_file = NULL;
	}
	m_data = NULL;
	return SCE_OK;
}

int util::Reader::read(void *buffer, size_t size)
{
	if(m_file != NULL){
		int ret = fread(buffer, 1, size, m_file);
		if(ret != size){
			return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
		}
	}else{
		memcpy(buffer, &m_data[m_dataOffset], size);
		m_dataOffset += size;
		return SCE_OK;
	}
	return SCE_OK;
}

int util::Reader::readInt32(int32_t *val)
{
	int ret = read(val, sizeof(int32_t));
	if(ret == SCE_OK){
		fixEndian(val, sizeof(int32_t));
	}
	return ret;
}
int util::Reader::readUint32(uint32_t *val)
{
	int ret = read(val, sizeof(uint32_t));
	if(ret == SCE_OK){
		fixEndian(val, sizeof(uint32_t));
	}
	return ret;
}
int util::Reader::readUint16(uint16_t *val)
{
	int ret = read(val, sizeof(uint16_t));
	if(ret == SCE_OK){
		fixEndian(val, sizeof(uint16_t));
	}
	return ret;
}

int util::Reader::readUint8(uint8_t *val)
{
	int ret = read(val, sizeof(uint8_t));
	if(ret == SCE_OK){
		fixEndian(val, sizeof(uint8_t));
	}
	return ret;
}

int util::Reader::seek(long offset)
{
	if(m_file != NULL){
		int ret = fseek(m_file, offset, SEEK_SET);
		if(ret == 0){
			return SCE_OK;
		}else{
			return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
		}
	}else{
		m_dataOffset = offset;
	}
	return SCE_OK;
}
int util::Reader::skip(size_t size)
{
	if(m_file != NULL){
		int ret = fseek(m_file, size, SEEK_CUR);
		if(ret == 0){
			return SCE_OK;
		}else{
			return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
		}
	}else{
		m_dataOffset += size;
		return SCE_OK;
	}
}

uint32_t util::Reader::tell(void)
{
	if(m_file != NULL){
		return ftell(m_file);
	}else{
		return m_dataOffset;
	}
}

size_t util::Reader::getSize(void)
{
	return m_size;
}

util::Reader::~Reader(void)
{
	finalize();
}

void util::Reader::setEndian(Endian e)
{
	m_endian = e;
}

util::Reader::Endian util::Reader::getCpuEndian(void)
{
	union {
		int a;
		char c[4];
	} u;
	u.a = 1;
	return (u.c[0] == 1) ? ENDIAN_LITTLE : ENDIAN_BIG;
}
void util::Reader::fixEndian(void *data, uint32_t size)
{
	if(getCpuEndian() != m_endian){
		uint8_t *ud = (uint8_t*)data;
		for(uint32_t i=0; i<size/2; i++){
			uint8_t v1 = ud[i];
			uint8_t v2 = ud[size-1-i];
			ud[i] = v2;
			ud[size-1-i] = v1;
		}
	}
}


