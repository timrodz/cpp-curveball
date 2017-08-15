/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#include <scebase_common.h>
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#include <gnm.h>
#include <gnmx.h>
#include <gnf.h>
#include <stdio.h>
#include <string>
#include "util/util.h"
#include "image_gnm.h"

#include <sampleutil/graphics.h>
#include <sampleutil/sampleutil_error.h>

#pragma comment(lib, "libSceJpegDec_stub_weak.a")
#pragma comment(lib, "libScePngDec_stub_weak.a")
#pragma comment(lib, "libSceGnf.a")

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;


static uint32_t computeContentSize(const sce::Gnf::Contents *gnfContents)
{
	// compute the size of used bytes
	uint32_t headerSize = sizeof(sce::Gnf::Header) + sizeof(sce::Gnf::Contents) + gnfContents->m_numTextures*sizeof(sce::Gnm::Texture);
	// add the paddings
	uint32_t align = 1<<gnfContents->m_alignment; // actual alignment
	size_t mask = align-1;
	uint32_t missAligned = ( headerSize & mask ); // number of bytes after the alignemnet point
	if(missAligned) // if none zero we have to add paddings
	{
		headerSize += align- missAligned;
	}
	return headerSize-sizeof(sce::Gnf::Header);
}

#pragma region GnfFile

int ssgi::GnfFile::init(void)
{
	if(m_reader.getSize() < sizeof(sce::Gnf::Header)){
		printf(__FILE__"(%d) check\n", __LINE__);
		m_reader.finalize();
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}
		
	sce::Gnf::Header header;
	m_reader.read(&header, sizeof(header));
	if (header.m_magicNumber != sce::Gnf::kMagic){
		printf(__FILE__"(%d) check\n", __LINE__);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}
	if (header.m_contentsSize>sce::Gnf::kMaxContents){
		printf(__FILE__"(%d) check\n", __LINE__);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}
		
	sce::Gnf::Contents *contents = (sce::Gnf::Contents *)malloc(header.m_contentsSize);
		
	m_reader.read(contents, header.m_contentsSize);
	if(contents->m_version != sce::Gnf::kVersion ){
		printf(__FILE__"(%d) check\n", __LINE__);
		free(contents);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}
		
	if(contents->m_alignment>31){
		printf(__FILE__"(%d) check\n", __LINE__);
		free(contents);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}
	if( computeContentSize(contents) != header.m_contentsSize ){
		printf(__FILE__"(%d) check\n", __LINE__);
		free(contents);
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}
		
	sce::Gnm::SizeAlign pixelsSa = sce::Gnf::getTexturePixelsSize(contents, 0 /*textureIndex*/);
	m_sizeAlign = pixelsSa;
		
	uint8_t *pixels = (uint8_t *)malloc(pixelsSa.m_size);
		
	uint32_t textureIndex = 0;
	m_reader.seek(sizeof(sce::Gnf::Header) + header.m_contentsSize + getTexturePixelsByteOffset(contents, textureIndex));
	m_reader.read(pixels, pixelsSa.m_size);

	void *gpuBaseAddrs[] = { NULL };
	m_texture = *sce::Gnf::patchTextures(contents, 0,1, gpuBaseAddrs);
		
	free(contents);
	m_reader.finalize();
	m_data = pixels;
		
	return SCE_OK;
		
}

#pragma endregion


#pragma region JpegFile


#pragma endregion






/* ------------------------------------------------------------------------------------------------------------------------- */

static int padding( int width )
{
	int rem = 3 * width % 4;
	// dummy size
	return rem > 0 ? 4 - rem : 0;
}

#define PACKED					__attribute__((packed))


namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				typedef struct _BitmapFileHeader
				{
					uint16_t	bfType PACKED;
					uint32_t	bfSize PACKED;
					uint16_t	bfReserved1 PACKED;
					uint16_t	bfReserved2 PACKED;
					uint32_t	bfOffBits PACKED;
				} PACKED _BitmapFileHeader;

				// Bitmap Info Header
				typedef struct _BitmapInfoHeader
				{
					uint32_t biSize PACKED;
					int32_t			biWidth PACKED;
					int32_t			biHeight PACKED;
					uint16_t	biPlanes PACKED;
					uint16_t	biBitCount PACKED;
					uint32_t	biCompression PACKED;
					uint32_t	biSizeImage PACKED;
					int32_t			biXPixPerMeter PACKED;
					int32_t		biYPixPerMeter PACKED;
					uint32_t biClrUsed PACKED;
					uint32_t biClrImporant PACKED;
				} PACKED _BitmapInfoHeader;

				// Bitmap Header
				typedef struct _BitmapHeader
				{
					struct _BitmapFileHeader bmpFileHeader;
					struct _BitmapInfoHeader bmpInfoHeader;
				} PACKED _BitmapHeader;
			}
		}
	}
}



int ssgi::saveAsBmp(const char* path,
					int stride,
					int width,
					int height,
					const char * buf)
{
	if(path == NULL){
		printf("graphicsUtilSaveDisplayAsBmp: path is NULL\n");
		return -1;
	}

	int32_t returnCode = SCE_OK;

	// file open
	FILE *fp = fopen( path, "wb");
	if(fp == NULL){
		printf("failed to open %s\n", path);
		return -1;
	}
	int padsize = padding( width );
	int isize = ( 3 * width + padsize ) * height;
	int fsize = sizeof( struct ssgi::_BitmapHeader ) + isize;

	unsigned int lineBufferSize = ( 3 * width + padsize );

	char *lineBuffer = (char *)::malloc( lineBufferSize );
	SCE_SAMPLE_UTIL_ASSERT_MSG(lineBuffer, "graphicsUtilSaveDisplayAsBmp: Memory allocation for screenshot failed\n");
	if(!lineBuffer)
		{
			printf("graphicsUtilSaveDisplayAsBmp: Memory allocation for screenshot failed\n");
			return -1;
		}

	struct ssgi::_BitmapHeader bmpHeader;


	// BITMAPFILEHEADER
	bmpHeader.bmpFileHeader.bfType = 'B' + ('M'<<8);
	bmpHeader.bmpFileHeader.bfSize = fsize;
	bmpHeader.bmpFileHeader.bfReserved1 = 0;
	bmpHeader.bmpFileHeader.bfReserved2 = 0;
	bmpHeader.bmpFileHeader.bfOffBits = sizeof( struct ssgi::_BitmapHeader );



	// BITMAPINFOHEADER
	bmpHeader.bmpInfoHeader.biSize = sizeof( struct ssgi::_BitmapInfoHeader );
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


	// write out header
	{
		char *p = (char *) &bmpHeader;
		unsigned int remain = sizeof( struct ssgi::_BitmapHeader );
		while( remain > 0)
			{
				int ret = fwrite(p, 1, remain, fp);

				SCE_SAMPLE_UTIL_ASSERT_MSG(ret >= 0, "graphicsUtilSaveDisplayAsBmp:  sceIoWrite() 0x%08x\n", ret);
				if (ret < 0 || ret > remain) 
					{
						printf("graphicsUtilSaveDisplayAsBmp:  sceIoWrite() 0x%08x\n", ret);
						fclose(fp);
						::free(lineBuffer);
						return -1;
					}
				p += ret;
				remain -= ret;
			}
	}

	// write out pixel data
	for( int y=height-1 ; y>=0 ; y-- )
		{
			char *line = lineBuffer;
			for( int x=0 ; x<width ; x++ )
				{
					line[ x*3 + 0 ] = buf[ y*4*stride + x*4 + 0 ];
					line[ x*3 + 1 ] = buf[ y*4*stride + x*4 + 1 ];
					line[ x*3 + 2 ] = buf[ y*4*stride + x*4 + 2 ];
				}

			// dummy data
			for( int z=0 ; z<padsize ; z++ )
				{
					line[ width*3 + z ] = 0;
				}

			unsigned int remain = lineBufferSize;
			while( remain > 0)
				{
					int ret = fwrite(line, 1, remain, fp);
					if (ret < 0 || ret > remain) 
						{
							printf("graphicsUtilSaveDisplayAsBmp:  sceIoWrite() 0x%08x\n", ret);
							fclose(fp);
							::free(lineBuffer);
							return -1;
						}

					line += ret;
					remain -= ret;
				}
		}


	::free(lineBuffer);

	// file close
	fclose(fp);
	// done
	return returnCode;

}




#endif //defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS
