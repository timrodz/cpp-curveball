/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include "util/util.h"
#include <sampleutil/graphics/buffer.h>
#include <sampleutil/graphics/loader.h>
#include "graphics_internal.h"
#include "image.h"
namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace util = sce::SampleUtil::Internal::Util;

#if _SCE_TARGET_OS_PSP2
#include <gxt.h>
#include <scepng.h>
#include <scejpeg.h>
#pragma comment(lib, "ScePng")
#pragma comment(lib, "SceDeflt")
#pragma comment(lib, "SceJpeg_stub")
#endif /* _SCE_TARGET_OS_PSP2 */

#if _SCE_TARGET_OS_WINDOWS
#pragma comment(lib, "gxt_conversion")
#endif /* _SCE_TARGET_OS_WINDOWS */

#define ROUND_UP(x, a)	((((unsigned int)x)+((a)-1u))&(~((a)-1u)))

// ------------------------------------------------------------------------------
// TGA
// ------------------------------------------------------------------------------
#pragma region TGA
int ssgi::TgaFile::readInfo(void)
{
	uint8_t tgaHeader[12]={0,0,2,0,0,0,0,0,0,0,0,0};

	if( memcmp(tgaHeader, m_fileImage, sizeof(tgaHeader))!=0 ){
		// unsupported format 
		//printf("Unsupported texture format \n");
		//for (uint32_t i = 0; i < 12; i++){
		//	printf("%#x\n", m_fileImage[i]);
		//}
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}

	m_info.currentOffset = sizeof(tgaHeader);
	m_info.header = m_fileImage + m_info.currentOffset;
	m_info.currentOffset += 6;
	m_info.width  = m_info.header[1] * 256 + m_info.header[0];
	m_info.height = m_info.header[3] * 256 + m_info.header[2];
	uint32_t bpp = m_info.header[4];
	m_info.isUpToBottom = m_info.header[5] & 0x20;

	if((m_info.width  <=0) 
		||  (m_info.height <=0) 
		||  (bpp!=24 && bpp!=32))
	{
		//printf("Unsupported texture format \n");
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;                                                           
	}

	m_info.bytesPerPixel		   = bpp/8;
	m_info.bytesPerPixelInBuffer = 4;

	m_info.imageSize = m_info.width * m_info.height * m_info.bytesPerPixelInBuffer;

	return SCE_OK;
}


int ssgi::TgaFile::open(const char *path)
{
	m_filesize = 0;
	m_fileImage = (uint8_t*)util::malloc_and_load(path, &m_filesize);
	if(m_fileImage == NULL){
		return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
	}
	m_openFromMemory = false;

	int ret = readInfo();
	if(ret != SCE_OK){
		free(m_fileImage);
		m_fileImage = NULL;
		return ret;
	}
	return SCE_OK;
}

int ssgi::TgaFile::openFromMemory(const void *image, uint32_t size)
{
	m_filesize = size;
	m_fileImage = (uint8_t*)image;
	if(m_fileImage == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	int ret = readInfo();
	if(ret != SCE_OK){
		return ret;
	}
	m_openFromMemory = true;
	return SCE_OK;
	
}

uint32_t ssgi::TgaFile::getWidth(void) const
{
	return m_info.width;
}
uint32_t ssgi::TgaFile::getHeight(void) const
{
	return m_info.height;
}

int ssgi::TgaFile::readData(void *data, uint32_t strideInByte) const
{
	if(strideInByte == 0){
		strideInByte = m_info.width * 4;
	}
	uint8_t *imageData = (uint8_t *)data;
	size_t dataOffset = m_info.currentOffset;
	for(uint32_t y=0; y<m_info.height; y++){
		for(uint32_t x=0; x<m_info.width; x++){

			uint8_t b = m_fileImage[dataOffset];
			dataOffset ++;
			uint8_t g = m_fileImage[dataOffset];
			dataOffset ++;
			uint8_t r = m_fileImage[dataOffset];
			dataOffset ++;
			uint8_t a;
			if(m_info.bytesPerPixel == 3){
				a = 255;
			}else{
				a = m_fileImage[dataOffset];
				dataOffset++;
			}

			uint32_t y2 = m_info.isUpToBottom ? y : (m_info.height-1 - y);
			uint8_t *ptr = &imageData[x*4 + y2 * strideInByte];

			ptr[0] = r;
			ptr[1] = g;
			ptr[2] = b;
			ptr[3] = a;
		}
	}


	return SCE_OK;
}



int ssgi::TgaFile::close(void)
{
	if(!m_openFromMemory){
		if(m_fileImage != NULL){
			free(m_fileImage);
			m_fileImage = NULL;
		}
	}
	return SCE_OK;
}

#pragma endregion
// ------------------------------------------------------------------------------
// BMP
// ------------------------------------------------------------------------------


int ssgi::BmpFile::open(const char *path)
{
	int ret = m_reader.initializeFromFile(path);
	if(ret != SCE_OK){
		return ret;
	}
	m_reader.setEndian(m_reader.ENDIAN_LITTLE);

	ret = readHeader();
	if(ret != SCE_OK){
		m_reader.finalize();
	}

	return ret;
}


int ssgi::BmpFile::openFromMemory(const void* data, size_t size)
{
	int ret = m_reader.initializeFromMemory(data, size);
	if(ret != SCE_OK){
		return ret;
	}

	m_reader.setEndian(m_reader.ENDIAN_LITTLE);
	ret = readHeader();
	if(ret != SCE_OK){
		m_reader.finalize();
	}

	return ret;
}

int ssgi::BmpFile::readHeader(void)
{
	int ret;
	m_reader.seek(0);

	ret = m_reader.readUint16(&m_header.bmpFileHeader.bfType);      if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint32(&m_header.bmpFileHeader.bfSize);      if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint16(&m_header.bmpFileHeader.bfReserved1); if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint16(&m_header.bmpFileHeader.bfReserved2); if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint32(&m_header.bmpFileHeader.bfOffBits);   if(ret != SCE_OK){ return ret; }


	ret = m_reader.readUint32(&m_header.bmpInfoHeader.biSize);          if(ret != SCE_OK){ return ret; }
	ret = m_reader.readInt32(&m_header.bmpInfoHeader.biWidth);          if(ret != SCE_OK){ return ret; }
	ret = m_reader.readInt32(&m_header.bmpInfoHeader.biHeight);         if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint16(&m_header.bmpInfoHeader.biPlanes);        if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint16(&m_header.bmpInfoHeader.biBitCount);      if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint32(&m_header.bmpInfoHeader.biCompression);   if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint32(&m_header.bmpInfoHeader.biSizeImage);     if(ret != SCE_OK){ return ret; }
	ret = m_reader.readInt32(&m_header.bmpInfoHeader.biXPixPerMeter);   if(ret != SCE_OK){ return ret; }
	ret = m_reader.readInt32(&m_header.bmpInfoHeader.biYPixPerMeter);   if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint32(&m_header.bmpInfoHeader.biClrUsed);       if(ret != SCE_OK){ return ret; }
	ret = m_reader.readUint32(&m_header.bmpInfoHeader.biClrImporant);   if(ret != SCE_OK){ return ret; }
	

	if (m_header.bmpFileHeader.bfType != 0x4D42 || m_header.bmpInfoHeader.biSize != 40) {
		printf("unsupported BMP, bfType(0x%x), biSize(%u)\n", m_header.bmpFileHeader.bfType, m_header.bmpInfoHeader.biSize);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}

	if (m_header.bmpInfoHeader.biCompression != 0 || m_header.bmpInfoHeader.biPlanes != 1) {
		printf("unsupported BMP, biCompression(%u), biPlanes(%u)\n", m_header.bmpInfoHeader.biCompression, m_header.bmpInfoHeader.biPlanes);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}

	if (m_header.bmpInfoHeader.biBitCount != 8 && m_header.bmpInfoHeader.biBitCount != 24 && m_header.bmpInfoHeader.biBitCount != 32) {
		printf("unsupported biBitCount(%u)\n", m_header.bmpInfoHeader.biBitCount);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}

	
	return SCE_OK;
}

int ssgi::BmpFile::readData(void *data) 
{
	int ret;
	uint8_t *imageData = (uint8_t*)data;

	uint32_t	x, y, destOffset;
	uint32_t	biPitch = ROUND_UP(m_header.bmpInfoHeader.biWidth * 3, 4);
	uint32_t	alignedWidth = util::ceil(m_header.bmpInfoHeader.biWidth, 4);
	bool		bottomUp;

	int32_t biHeight = 	m_header.bmpInfoHeader.biHeight;
	if (biHeight < 0) {
		biHeight *= -1;
		bottomUp = false;
	} else {
		bottomUp = true;
	}

	if (m_header.bmpInfoHeader.biBitCount == 8) {
		m_reader.seek(14 + 40);
		ret = m_reader.read(m_palette, sizeof(m_palette));   if(ret != SCE_OK){ return ret; }

		biPitch = ROUND_UP(m_header.bmpInfoHeader.biWidth * 1, 4);
		for (y = 0; y < biHeight; y++) {
			m_reader.seek(14 + 40 + 256*4 + biPitch*y);

			if (bottomUp) {
				destOffset = (biHeight - 1 - y) * alignedWidth;
			} else {
				destOffset = y * alignedWidth;
			}

			for (x = 0; x < m_header.bmpInfoHeader.biWidth; x++) {
				uint8_t paletteOffset;
				ret = m_reader.readUint8(&paletteOffset); if(ret != SCE_OK){ return ret; }

				imageData[4 * (x + destOffset) + 3] = 255;	// A
				imageData[4 * (x + destOffset) + 2] = m_palette[paletteOffset*4 + 0];	// B
				imageData[4 * (x + destOffset) + 1] = m_palette[paletteOffset*4 + 1];	// G
				imageData[4 * (x + destOffset) + 0] = m_palette[paletteOffset*4 + 2];	// R
			}
		}
	}else if (m_header.bmpInfoHeader.biBitCount == 24) {
		biPitch = ROUND_UP(m_header.bmpInfoHeader.biWidth * 3, 4);
		for (y = 0; y < biHeight; y++) {
			m_reader.seek(14 + 40  + biPitch*y);
			if (bottomUp) {
				destOffset = (biHeight - 1 - y) * alignedWidth;
			} else {
				destOffset = y * alignedWidth;
			}
			for (x = 0; x < m_header.bmpInfoHeader.biWidth; x++) {
				imageData[4 * (x + destOffset) + 3] = 255;						// A
				ret = m_reader.readUint8(&imageData[4 * (x + destOffset) + 2]);	if(ret != SCE_OK){ return ret; }// B
				ret = m_reader.readUint8(&imageData[4 * (x + destOffset) + 1]);	if(ret != SCE_OK){ return ret; }// G
				ret = m_reader.readUint8(&imageData[4 * (x + destOffset) + 0]);	if(ret != SCE_OK){ return ret; }// R
			}
		}
	}else if(m_header.bmpInfoHeader.biBitCount == 32){
		biPitch = m_header.bmpInfoHeader.biWidth * 4;
		for (y = 0; y < biHeight; y++) {
			m_reader.seek(14 + 40  + biPitch*y);
			if (bottomUp) {
				destOffset = (biHeight - 1 - y) * alignedWidth;
			} else {
				destOffset = y * alignedWidth;
			}
			for (x = 0; x < m_header.bmpInfoHeader.biWidth; x++) {

				ret = m_reader.readUint8(&imageData[4 * (x + destOffset) + 2]);	if(ret != SCE_OK){ return ret; }// B
				ret = m_reader.readUint8(&imageData[4 * (x + destOffset) + 1]);	if(ret != SCE_OK){ return ret; }// G
				ret = m_reader.readUint8(&imageData[4 * (x + destOffset) + 0]);	if(ret != SCE_OK){ return ret; }// R
				ret = m_reader.readUint8(&imageData[4 * (x + destOffset) + 3]);	if(ret != SCE_OK){ return ret; }// A
			}
		}
	}
	return SCE_OK;
}


#if _SCE_TARGET_OS_PSP2

int loadBMPFromMemory(SceGxmTexture *texture, uint8_t *fileimage, size_t fileSize, ssgi::GraphicsLoaderImpl *loader)
{
	ssgi::BmpFile bmpFile;
	int ret = bmpFile.openFromMemory(fileimage, fileSize);
	if(ret != SCE_OK){
		return ret;
	}
	uint32_t bytesPerPixelInBuffer = 4;
	uint32_t alignedWidth = util::ceil(bmpFile.getWidth(), 4);
	uint32_t alignedImageSize = alignedWidth * bmpFile.getHeight() * bytesPerPixelInBuffer;
	uint8_t *alignedImageData = (uint8_t*)loader->memalign(loader->MEMTYPE_CDRAM, 16, alignedImageSize);

	ret = bmpFile.readData(alignedImageData);
	if(ret != SCE_OK){
		loader->free(alignedImageData);
		return ret;
	}

	ret = sceGxmTextureInitLinearStrided(texture, alignedImageData, SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_ABGR,
		                                 bmpFile.getWidth(), bmpFile.getHeight(), alignedWidth * bytesPerPixelInBuffer);

	return SCE_OK;
}





int loadGXTFromMemory( SceGxmTexture *texture, const void *gxtImage, uint32_t gxtImageSize, ssgi::GraphicsLoaderImpl *loader )
{
	if( sceGxtCheckData(gxtImage) != SCE_OK ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	const uint32_t dataSize = sceGxtGetDataSize(gxtImage);

	void *textureData = loader->memalign(loader->MEMTYPE_CDRAM, 16, dataSize);

	const void *dataSrc = sceGxtGetDataAddress(gxtImage);

	memcpy(textureData, dataSrc, dataSize);

	int ret = sceGxtInitTexture(texture, gxtImage, textureData, 0);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	return ret;
}











#endif



static int padding( int width )
{
	int rem = 3 * width % 4;
	// dummy size
	return rem > 0 ? 4 - rem : 0;
}

static void safe_write(const void *data, size_t size, FILE* fp)
{
	while(size > 0){
		int ret = fwrite(data, 1, size, fp);
		size -= ret;
	}
}




int ssgi::BmpFile::saveImage(
		const char*path,
		int stride,
		int width,
		int height,
		const char * buf)
{
	if(path == NULL){
		printf("graphicsUtilSaveDisplayAsBmp: path is NULL\n");
		return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
	}

	int32_t returnCode = SCE_OK;


	// file open
	FILE *fp = fopen(path, "wb");
	if(fp == NULL){
		return SCE_SAMPLE_UTIL_ERROR_FILE_OPEN;
	}

	int padsize = padding( width );
	int isize = ( 3 * width + padsize ) * height;
	int fsize = 14+40 + isize;

	unsigned int lineBufferSize = ( 3 * width + padsize );

	char *lineBuffer = (char *)::malloc( lineBufferSize );
	if(!lineBuffer){
		printf("graphicsUtilSaveDisplayAsBmp: Memory allocation for screenshot failed\n");
		return SCE_SAMPLE_UTIL_ERROR_OUT_OF_MEMORY;
	}

	Header bmpHeader;

	// BITMAPFILEHEADER
	bmpHeader.bmpFileHeader.bfType = 'B' + ('M'<<8);
	bmpHeader.bmpFileHeader.bfSize = fsize;
	bmpHeader.bmpFileHeader.bfReserved1 = 0;
	bmpHeader.bmpFileHeader.bfReserved2 = 0;
	bmpHeader.bmpFileHeader.bfOffBits = 14+40;
	
	fwrite(&bmpHeader.bmpFileHeader.bfType,      1, 2, fp);
	fwrite(&bmpHeader.bmpFileHeader.bfSize,      1, 4, fp);
	fwrite(&bmpHeader.bmpFileHeader.bfReserved1, 1, 2, fp);
	fwrite(&bmpHeader.bmpFileHeader.bfReserved2, 1, 2, fp);
	fwrite(&bmpHeader.bmpFileHeader.bfOffBits,   1, 4, fp);

	// BITMAPINFOHEADER
	bmpHeader.bmpInfoHeader.biSize = 40;
	bmpHeader.bmpInfoHeader.biWidth = width;
	bmpHeader.bmpInfoHeader.biHeight = height;
	bmpHeader.bmpInfoHeader.biPlanes = 1;
	bmpHeader.bmpInfoHeader.biBitCount = 24;
	bmpHeader.bmpInfoHeader.biCompression = 0;
	bmpHeader.bmpInfoHeader.biSizeImage = isize;
	bmpHeader.bmpInfoHeader.biXPixPerMeter = 0;
	bmpHeader.bmpInfoHeader.biYPixPerMeter = 0;
	bmpHeader.bmpInfoHeader.biClrUsed = 0;
	bmpHeader.bmpInfoHeader.biClrImporant = 0;

	fwrite(&bmpHeader.bmpInfoHeader.biSize,         1, 4, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biWidth ,      1, 4, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biHeight,      1, 4, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biPlanes ,     1, 2, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biBitCount ,   1, 2, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biCompression, 1, 4, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biSizeImage ,  1, 4, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biXPixPerMeter,1, 4, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biYPixPerMeter,1, 4, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biClrUsed ,    1, 4, fp);
	fwrite(&bmpHeader.bmpInfoHeader.biClrImporant ,1, 4, fp);

	SCE_SAMPLE_UTIL_ASSERT(ftell(fp) == 14+40);
	// write out pixel data
	int count=0;
	for( int y=height-1 ; y>=0 ; y-- )
	{
		char *line = lineBuffer;
		for( int x=0 ; x<width ; x++ ){
			line[ x*3 + 0 ] = buf[ y*4*stride + x*4 + 2 ]; //g
			line[ x*3 + 1 ] = buf[ y*4*stride + x*4 + 1 ]; //b 
			line[ x*3 + 2 ] = buf[ y*4*stride + x*4 + 0 ]; //r
		}
		
		// dummy data
		for( int z=0 ; z<padsize ; z++ )
		{
			line[ width*3 + z ] = 0;
		}
		
		safe_write(line, lineBufferSize, fp);
		count++;

		SCE_SAMPLE_UTIL_ASSERT(ftell(fp) == (14+40+lineBufferSize *count));
		if(ftell(fp) != (14+40+lineBufferSize *count)){
			printf("%d,%d\n", (int)ftell(fp), (int)(14+40+lineBufferSize *count));
		}
	}


	::free(lineBuffer);

	// file close
	//printf("count %d,%d\n", count, height);
	//printf("done %d,%d\n", (int)ftell(fp), fsize);

	fclose(fp);

	// done
	return returnCode;
}



#if _SCE_TARGET_OS_PSP2
int ssgi::ImageFile::open(ssgi::GraphicsLoaderImpl *loader, ssgi::_JpegDecoder *jpegDecoder, const char *path)
#elif _SCE_TARGET_OS_WINDOWS || _SCE_TARGET_OS_ORBIS
int ssgi::ImageFile::open(const char *path)
#endif
{
	std::string strpath = path;
	size_t p = strpath.find_last_of(".");
	if(p == strpath.npos){
		printf(__FILE__"(%d) no extension\n", __LINE__);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}
	std::string ext = util::getExtention(path);

	if((ext == "tga") || (ext == "TGA")){
		m_fileFormat = kTga;
		return m_tgaFile.open(path);
#if _SCE_TARGET_OS_PSP2
	}else if((ext == "gxt") || (ext == "GXT")){
		m_fileFormat = kGxt;
		return m_gxtFile.open(path);
	}else if((ext == "png") || (ext == "PNG")){
		m_fileFormat = kPng;
		return m_pngFile.open(path);
	}else if((ext == "jpg") || (ext == "JPG") || (ext == "jpeg") || (ext == "JPEG") || (ext == "jpe") || (ext == "JPE")){
		m_fileFormat = kJpg;
		return m_jpgFile.open(loader, jpegDecoder, path);
#endif /* _SCE_TARGET_OS_PSP2 */
#if _SCE_TARGET_OS_ORBIS
	}else if((ext == "gnf") || (ext == "GNF")){
		m_fileFormat = kGnf;
		return m_gnfFile.open(path);
	}else if((ext == "jpg") || (ext == "JPG") || (ext == "jpeg") || (ext == "JPEG") || (ext == "jpe") || (ext == "JPE")){
		m_fileFormat = kJpg;
		return m_jpgFile.open(path);
	}else if((ext == "png") || (ext == "PNG")){
		m_fileFormat = kPng;
		return m_pngFile.open(path);
#endif
	}else if((ext == "bmp") || (ext == "BMP")){
		m_fileFormat = kBmp;
		return m_bmpFile.open(path);
	}
	printf(__FILE__"(%d) unsupported format\n", __LINE__);
	return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;

}


#if _SCE_TARGET_OS_PSP2
int ssgi::ImageFile::openFromMemory(_JpegDecoder *jpegDecoder, const void *image, size_t imageSize)
#elif _SCE_TARGET_OS_WINDOWS || _SCE_TARGET_OS_ORBIS
int ssgi::ImageFile::openFromMemory(const void *image, size_t imageSize)
#endif
{
	const uint8_t *magic = (const uint8_t *)image;
	if((magic[0] == 0x42) && (magic[1] == 0x4d)){
		m_fileFormat = kBmp;
		return m_bmpFile.openFromMemory(image,imageSize);
	}

#if _SCE_TARGET_OS_PSP2
	if((magic[0] == 0x89) && (magic[1] == 0x50) && (magic[2] == 0x4e) && (magic[3] == 0x47)){
		m_fileFormat = kPng;
		return m_pngFile.openFromMemory(image,imageSize);
	}
	
	/*
	if((magic[0] == 0xff) && (magic[1] == 0xd8) && (magic[2] == 0xff) && 
	   ((magic[3] == 0xe0) || (magic[3] == 0xe1))){ // JFIF or EXIF
		m_fileFormat = kJpg;
		return m_jpgFile.openFromMemory(image,imageSize);
	}
	*/

	if((magic[0] == 'G') && (magic[1] == 'X') && (magic[2] == 'T') && (magic[3] == 0x0)){
		m_fileFormat = kGxt;
		return m_gxtFile.openFromMemory(image,imageSize);
	}
#endif

#if _SCE_TARGET_OS_ORBIS
	if((magic[0] == 0x89) && (magic[1] == 0x50) && (magic[2] == 0x4e) && (magic[3] == 0x47)){
		m_fileFormat = kPng;
		return m_pngFile.openFromMemory(image,imageSize);
	}
	
	if((magic[0] == 0xff) && (magic[1] == 0xd8) && (magic[2] == 0xff) && 
		((magic[3] == 0xe0) || (magic[3] == 0xe1))){ // JFIF or EXIF
		m_fileFormat = kJpg;
		return m_jpgFile.openFromMemory(image,imageSize);
	}
#endif

	// Tga has no magic word.
	m_fileFormat = kTga;
	return m_tgaFile.openFromMemory(image, imageSize);
}


int ssgi::ImageFile::close(void)
{
	switch(m_fileFormat){
	case kBmp: { return m_bmpFile.close(); }
	case kTga: { return m_tgaFile.close(); }
#if _SCE_TARGET_OS_PSP2
	case kGxt: { return m_gxtFile.close(); }
	case kPng: { return m_pngFile.close(); }
	case kJpg: { return m_jpgFile.close(); }
#endif /* _SCE_TARGET_OS_PSP2 */
#if _SCE_TARGET_OS_ORBIS
	case kJpg: { return m_jpgFile.close(); }
	case kPng: { return m_pngFile.close(); }
	case kGnf: { return m_gnfFile.close(); }
#endif 
	default: { return SCE_OK; }
	}
}

ssgi::ImageFile::FileFormat ssgi::ImageFile::getFileFormat(void) const
{
	return m_fileFormat;
}
uint32_t ssgi::ImageFile::getWidth(void) const
{
	switch(m_fileFormat){
	case kBmp: { return m_bmpFile.getWidth(); }
	case kTga: { return m_tgaFile.getWidth(); }
#if _SCE_TARGET_OS_PSP2
	case kGxt: { return m_gxtFile.getWidth(); }
	case kPng: { return m_pngFile.getWidth(); }
	case kJpg: { return m_jpgFile.getWidth(); }
#endif /* _SCE_TARGET_OS_PSP2 */
#if _SCE_TARGET_OS_ORBIS
	case kJpg: { return m_jpgFile.getWidth(); }
	case kPng: { return m_pngFile.getWidth(); }
	case kGnf: { return m_gnfFile.getWidth(); }
#endif 
	default: { abort(); return 0; }

	}
}

uint32_t ssgi::ImageFile::getHeight(void) const
{
	switch(m_fileFormat){
	case kBmp: { return m_bmpFile.getHeight(); }
	case kTga: { return m_tgaFile.getHeight(); }
#if _SCE_TARGET_OS_PSP2
	case kGxt: { return m_gxtFile.getHeight(); }
	case kPng: { return m_pngFile.getHeight(); }
	case kJpg: { return m_jpgFile.getHeight(); }
#endif /* _SCE_TARGET_OS_PSP2 */
#if _SCE_TARGET_OS_ORBIS
	case kJpg: { return m_jpgFile.getHeight(); }
	case kPng: { return m_pngFile.getHeight(); }
	case kGnf: { return m_gnfFile.getHeight(); }
#endif 
	default: { abort(); return 0;}
		
	}
}
uint32_t ssgi::ImageFile::getStride(void) const
{
	switch(m_fileFormat){
	case kBmp: { return m_bmpFile.getStride(); }
	case kTga: { return m_tgaFile.getStride(); }
#if _SCE_TARGET_OS_PSP2
	case kGxt: { return m_gxtFile.getStride(); }
	case kPng: { return m_pngFile.getStride(); }
	case kJpg: { return m_jpgFile.getStride(); }
#endif /* _SCE_TARGET_OS_PSP2 */
#if _SCE_TARGET_OS_ORBIS
	case kJpg: { return m_jpgFile.getStride(); }
	case kPng: { return m_pngFile.getStride(); }
	case kGnf: { return m_gnfFile.getStride(); }
#endif 
	default: { abort(); return 0; }

	}
}

uint32_t ssgi::ImageFile::getMipCount(void) const
{
	switch(m_fileFormat){
	case kBmp: { return m_bmpFile.getMipCount(); }
	case kTga: { return m_tgaFile.getMipCount(); }
#if _SCE_TARGET_OS_PSP2
	case kGxt: { return m_gxtFile.getMipCount(); }
	case kPng: { return m_pngFile.getMipCount(); }
	case kJpg: { return m_jpgFile.getMipCount(); }
#endif /* _SCE_TARGET_OS_PSP2 */
#if _SCE_TARGET_OS_ORBIS
	case kJpg: { return m_jpgFile.getMipCount(); }
	case kPng: { return m_pngFile.getMipCount(); }
	case kGnf: { return m_gnfFile.getMipCount(); }
#endif 
	default: { abort();  return 0;}

	}
}

#if _SCE_TARGET_OS_PSP2

SceGxmTextureType ssgi::ImageFile::getGxmType(void) const
{
	switch(m_fileFormat){
	case kBmp: { return m_bmpFile.getGxmType(); }
	case kTga: { return m_tgaFile.getGxmType(); }
	case kGxt: { return m_gxtFile.getGxmType(); }
	case kPng: { return m_pngFile.getGxmType(); }
	case kJpg: { return m_jpgFile.getGxmType(); }
	default: { SCE_SAMPLE_UTIL_ASSERT(0); }
	}
	return  SCE_GXM_TEXTURE_SWIZZLED ;
}

SceGxmTextureFormat ssgi::ImageFile::getGxmFormat(void) const
{
	switch(m_fileFormat){
	case kBmp: { return m_bmpFile.getGxmFormat(); }
	case kTga: { return m_tgaFile.getGxmFormat(); }
	case kGxt: { return m_gxtFile.getGxmFormat(); }
	case kPng: { return m_pngFile.getGxmFormat(); }
	case kJpg: { return m_jpgFile.getGxmFormat(); }
	default: { SCE_SAMPLE_UTIL_ASSERT(0); }
	}
	return SCE_GXM_TEXTURE_FORMAT_U8_000R;
}


#endif /* _SCE_TARGET_OS_PSP2 */

int ssgi::ImageFile::readData(void *buf)
{
	switch(m_fileFormat){
	case kBmp: { return m_bmpFile.readData(buf); }
	case kTga: { return m_tgaFile.readData(buf); }
#if _SCE_TARGET_OS_PSP2
	case kGxt: { return m_gxtFile.readData(buf); }
	case kPng: { return m_pngFile.readData(buf); }
	case kJpg: { return m_jpgFile.readData(buf); }
#endif
#if _SCE_TARGET_OS_ORBIS
	case kJpg: { return m_jpgFile.readData(buf); }
	case kPng: { return m_pngFile.readData(buf); }
	case kGnf: { return m_gnfFile.readData(buf); }
#endif 
	default: { SCE_SAMPLE_UTIL_ASSERT(0); return 0;}
	}
}


