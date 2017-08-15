/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_IMAGE_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_IMAGE_H 1

#include <sampleutil/graphics.h>
#include "util/util.h"
#if _SCE_TARGET_OS_PSP2
#include "platform_gxm/texture_util_gxm.h"
#include <scejpeg.h>
#endif

#if _SCE_TARGET_OS_ORBIS
#include <jpeg_dec.h>
#include <png_dec.h>
#endif

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{

				class TgaFile
				{
				private:

					struct TgaInfo
					{
						const uint8_t			*header;
						uint32_t		width;
						uint32_t		height;
						unsigned int	currentOffset;
						uint32_t		bytesPerPixel;
						uint32_t		bytesPerPixelInBuffer;
						uint32_t		imageSize;
						int				isUpToBottom;
					};

					TgaInfo m_info;
					uint8_t *m_fileImage;
					uint32_t m_filesize;
					bool m_openFromMemory;

					int readInfo(void);

				public:

					TgaFile(void)
					{
						m_fileImage = NULL;
					}


					int open(const char *path);
					int openFromMemory(const void *image, uint32_t size);

					uint32_t getWidth(void) const;
					uint32_t getHeight(void) const;
					uint32_t getStride(void) const {
						return 0;
					}
					uint32_t getMipCount(void) const
					{
						return 0;
					}

#if _SCE_TARGET_OS_PSP2
					SceGxmTextureType getGxmType(void) const
					{
						return SCE_GXM_TEXTURE_LINEAR ;
					}
					SceGxmTextureFormat getGxmFormat(void) const
					{
						return SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_ABGR;
					}
#endif

					int readData(void *data, uint32_t strideInByte) const;
					int readData(void *imageData) const
					{
						return readData(imageData, 4 * getWidth());
						   
					}					

					int close(void);

				};


				class BmpFile
				{
				private:
					struct FileHeader
					{
						uint16_t bfType;
						uint32_t bfSize;
						uint16_t bfReserved1;
						uint16_t bfReserved2;
						uint32_t bfOffBits;
					};
	
					// Bitmap Info Header
					struct InfoHeader
					{
						uint32_t	biSize;
						int32_t		biWidth;
						int32_t		biHeight;
						uint16_t	biPlanes;
						uint16_t	biBitCount;
						uint32_t	biCompression;
						uint32_t	biSizeImage;
						int32_t		biXPixPerMeter;
						int32_t		biYPixPerMeter;
						uint32_t	biClrUsed;
						uint32_t	biClrImporant;
					};
	
					// Bitmap Header
					struct Header
					{
						struct FileHeader bmpFileHeader;
						struct InfoHeader bmpInfoHeader;
					};
	
					Internal::Util::Reader m_reader;
					Header m_header;

					uint8_t m_palette[256*4];

					int readHeader(void);

				public:
	
					int open(const char *path);
					int openFromMemory(const void* data, size_t size);

					int readData(void *data) ;

					uint32_t getWidth(void) const
					{
						return (uint32_t)m_header.bmpInfoHeader.biWidth;
					}
					uint32_t getHeight(void) const
					{
						return (uint32_t)m_header.bmpInfoHeader.biHeight;
					}

					uint32_t getStride(void) const 
					{
						return 0;
					}
					uint32_t getMipCount(void) const
					{
						return 0;
					}
#if _SCE_TARGET_OS_PSP2
					SceGxmTextureType getGxmType(void) const
					{
						return SCE_GXM_TEXTURE_LINEAR ;
					}
					SceGxmTextureFormat getGxmFormat(void) const
					{
						return SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_ABGR;
					}

#endif


					int close(void)
					{
						return m_reader.finalize();
					}

					static int saveImage(
										 const char*path,
										 int stride,
										 int width,
										 int height,
										 const char * buf);

				};
#if _SCE_TARGET_OS_PSP2
				
				
				class GxtFile
				{
				public:
					SceGxmTexture m_texture;
					bool m_freeImage;
					void *m_image;
					
					GxtFile(void);
					
					~GxtFile(void);
					
					int openFromMemory(const void* gxtImage, size_t gxtImageSize);
					
					int open(const char * filename);
					
					int close(void);

					uint32_t getWidth(void) const
					{
						return sceGxmTextureGetWidth(&m_texture);
					}
					uint32_t getHeight(void) const
					{
						return sceGxmTextureGetHeight(&m_texture);
					}

					uint32_t getStride(void) const 
					{
						return sceGxmTextureGetStride(&m_texture);
					}

					uint32_t getMipCount(void) const
					{
						return sceGxmTextureGetMipmapCount(&m_texture);
					}

					SceGxmTextureType getGxmType(void) const
					{
						return sceGxmTextureGetType(&m_texture);
					}
					SceGxmTextureFormat getGxmFormat(void) const
					{
						return sceGxmTextureGetFormat(&m_texture);
					}

					int readData(void *data) const
					{
						size_t size = getTextureSize(getGxmType(), getGxmFormat(), getWidth(), getHeight(), getStride(), getMipCount());
						memcpy(data, sceGxmTextureGetData(&m_texture), size);
						return SCE_OK;
					}
					
					
				};
				
				
				class PngFile
				{
				private:
					int m_width;
					int m_height;
					int m_format;
					int m_streamFormat;
					uint32_t *m_imageData;
				public:
					PngFile(void);
					
					uint32_t getWidth(void) const;
					uint32_t getHeight(void) const;
					uint32_t getStride(void) const
					{
						return getWidth() * 4;
					}
					uint32_t getMipCount(void) const
					{
						return 0;
					}
					SceGxmTextureType getGxmType(void) const
					{
						return SCE_GXM_TEXTURE_LINEAR_STRIDED;
					}

					SceGxmTextureFormat getGxmFormat(void) const
					{
						return SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_ABGR;
					}
					
					int open(const char *filename);
					int openFromMemory(const void *image, uint32_t size);


					int close(void);
					
					int readData(void *data, uint32_t strideInByte) const;
					int readData(void *data) const
					{
						return readData(data, getWidth() *4);
					}
				};
				
				class _JpegDecoder;
				class GraphicsLoaderImpl;
				
				class JpegFile
				{
				private:
					GraphicsLoaderImpl *m_loader;
					_JpegDecoder *m_jpegDecoder;
					void *m_fileImage;
					size_t m_fileImageSize;

					SceJpegOutputInfo m_jpegOutInfo;
					int m_decodeMode;

					std::string m_path;
					
					struct DecodeBuffer
					{
						void *ycbcr;
						size_t ycbcrSize;
						void *tmpRgb;
						size_t tmpRgbSize;
					};
					int allocateDecodeBuffer(DecodeBuffer *buf);
					int freeDecodeBuffer(DecodeBuffer *buf);

				public:
					uint32_t getWidth(void) const;
					uint32_t getHeight(void) const;
					uint32_t getStride(void) const;
					uint32_t getMipCount(void) const
					{
						return 0;
					}

					JpegFile(void);
					int open(sce::SampleUtil::Graphics::Impl::GraphicsLoaderImpl *loader, sce::SampleUtil::Graphics::Impl::_JpegDecoder *jpegDecoder, const char *path);
					
					int readData(void *imageData);

					SceGxmTextureType getGxmType(void) const
					{
						return SCE_GXM_TEXTURE_LINEAR_STRIDED;
					}
					SceGxmTextureFormat getGxmFormat(void) const
					{
						return SCE_GXM_TEXTURE_FORMAT_U8U8U8U8_ABGR;
					}


					int close(void);
				};

#endif /* _SCE_TARGET_OS_PSP2 */

#if  _SCE_TARGET_OS_ORBIS 

				class GnfFile
				{
				private:
					sce::SampleUtil::Internal::Util::Reader m_reader;
					sce::Gnm::Texture m_texture;
					sce::Gnm::SizeAlign m_sizeAlign;
					void *m_data;

					int init(void);
				public:
					GnfFile(void)
					{
						m_data = NULL;
					}

					sce::Gnm::Texture getGnmTexture(void) const 
                    {
                       return m_texture;
                    }
					sce::Gnm::SizeAlign getGnmSizeAlign(void) const
                    { 
                       return m_sizeAlign;
					}
					int open(const char *path)
					{
						int ret = m_reader.initializeFromFile(path);
						if(ret != SCE_OK){
							return ret;
						}
						return init();
					}
					int openFromMemory(const void *image, uint32_t size)
					{
						int ret = m_reader.initializeFromMemory(image, size);
						if(ret != SCE_OK){
							return ret;
						}
						return init();
					}
	
					uint32_t getWidth(void) const
					{
						return m_texture.getWidth();
					}
					uint32_t getHeight(void) const
					{
						return m_texture.getHeight();
					}
					uint32_t getStride(void) const {
						return 0;
					}
					uint32_t getMipCount(void) const
					{
						return m_texture.getLastMipLevel()+1;
					}
	
					int readData(void *imageData) const
					{
						memcpy(imageData, m_data, getGnmSize());
						return SCE_OK;
		
					}					
	
					int close(void)
					{
						if(m_data != NULL){
							free(m_data);
							m_data = NULL;
						}
						return m_reader.finalize();
					}


					size_t getGnmAlign(void) const
					{
						return m_sizeAlign.m_align;
					}
					size_t getGnmSize(void) const
					{
						return m_sizeAlign.m_size;
					}
				};
				


				class JpegFile
				{
				private:
					const void *m_image;
					uint32_t m_imageSize;
					bool m_freeOnExit;
					SceJpegDecImageInfo		m_imageInfo;
					SceJpegDecParseParam	m_parseParam;
					sce::Gnm::SizeAlign m_sizeAlign;
					sce::Gnm::Texture m_texture;
					int init(void)
					{

						memset(&m_parseParam, 0, sizeof(m_parseParam));
						m_parseParam.jpegMemAddr = m_image;
						m_parseParam.jpegMemSize = m_imageSize;
						m_parseParam.decodeMode = SCE_JPEG_DEC_DECODE_MODE_NORMAL;
						m_parseParam.downScale = 1;

						int ret = sceJpegDecParseHeader(&m_parseParam, &m_imageInfo);
						if (ret < 0){
							return ret;
						}
						if (m_imageInfo.colorSpace == SCE_JPEG_DEC_COLOR_SPACE_UNKNOWN) {
							return -1;
						}

						sce::Gnm::TextureSpec spec;
						spec.init();
						spec.m_textureType = sce::Gnm::kTextureType2d;
						spec.m_width = m_imageInfo.imageWidth;
						spec.m_height = m_imageInfo.imageHeight;
						spec.m_depth = 1;
						spec.m_pitch = 0;
						spec.m_numMipLevels = 1;
						spec.m_numSlices = 1;
						spec.m_format = sce::Gnm::kDataFormatR8G8B8A8Unorm;
						spec.m_tileModeHint = sce::Gnm::kTileModeDisplay_LinearAligned;
						spec.m_minGpuMode = sce::Gnm::kGpuModeBase;
						spec.m_numFragments = sce::Gnm::kNumFragments1;
						int32_t status = m_texture.init(&spec);
						if (status != SCE_GNM_OK) {
							return -1;
						}
						m_sizeAlign = m_texture.getSizeAlign();
						return SCE_OK;
					}
				public:
					JpegFile(void)
					{
						m_image = NULL;
						m_freeOnExit = false;
					}

					int open(const char *path)
					{
						uint32_t size;
						m_image = sce::SampleUtil::Internal::Util::malloc_and_load(path, &size);
						if(m_image == NULL){
							return -1;
						}
						m_imageSize = size;
						m_freeOnExit = true;
						return init();
					}
					int openFromMemory(const void *image, uint32_t size)
					{
						if(image == NULL){
							return -1;
						}
						m_image = image;
						m_imageSize = size;
						m_freeOnExit = false;
						return init();
					}
	
					uint32_t getWidth(void) const
					{
						return m_texture.getWidth();
					}
					uint32_t getHeight(void) const
					{
						return m_texture.getHeight();
					}
					uint32_t getStride(void) const {
						return m_texture.getPitch() * 4;
					}
					uint32_t getMipCount(void) const
					{
						return 1;
					}
	
					int readData(void *decodedImage) const
					{
						// query size
						SceJpegDecCreateParam	createParam;
						memset(&createParam, 0, sizeof(createParam));
						createParam.thisSize		= sizeof(createParam);
						createParam.attribute		= m_imageInfo.suitableCscAttribute;
						createParam.maxImageWidth	= m_imageInfo.outputImageWidth;
						int ret = sceJpegDecQueryMemorySize(&createParam);
						if(ret < 0){
							return ret;
						}
						// alloc memory
						size_t decoderMemorySize = ret;
						void *decoderMemory = malloc(decoderMemorySize);

						void *coefficientMemory = (m_imageInfo.coefficientMemSize > 0) ? 
							malloc(m_imageInfo.coefficientMemSize) : NULL;

						SceJpegDecHandle handle;
						ret = sceJpegDecCreate(&createParam, decoderMemory, decoderMemorySize, &handle);
						if (ret < 0) {
							if(decoderMemory != NULL){free(decoderMemory);	}
							if(coefficientMemory != NULL){	free(coefficientMemory);}
							return ret;
						}
						// decode
						SceJpegDecDecodeParam	decodeParam;
						memset(&decodeParam, 0, sizeof(decodeParam));
						decodeParam.jpegMemAddr	= m_image;
						decodeParam.jpegMemSize	= m_imageSize;
						decodeParam.imageMemAddr= decodedImage;
						decodeParam.imageMemSize= m_sizeAlign.m_size;
						decodeParam.imagePitch	= m_texture.getPitch() *4;
						decodeParam.decodeMode	= m_parseParam.decodeMode;
						decodeParam.downScale	= m_parseParam.downScale;
						decodeParam.pixelFormat	= SCE_JPEG_DEC_PIXEL_FORMAT_R8G8B8A8;
						decodeParam.alphaValue	= 255;
						decodeParam.coefficientMemAddr	= coefficientMemory;
						decodeParam.coefficientMemSize	= m_imageInfo.coefficientMemSize;
						ret = sceJpegDecDecode(handle, &decodeParam, NULL);
						if(ret >= 0){
							ret = SCE_OK;
						}
						if(ret != SCE_OK){
							printf("Error : sceJpegDecDecode() => %#x\n", ret);
						}
						sceJpegDecDelete(handle);
						if(decoderMemory != NULL){free(decoderMemory);	}
						if(coefficientMemory != NULL){	free(coefficientMemory);}
						return ret;
					}					
	
					int close(void)
					{
						if(m_freeOnExit){
							free((void*)m_image);
							m_image = NULL;
							m_freeOnExit = false;
						}
						return SCE_OK;
					}
				};
				

				class PngFile
				{
				private:
					const void *m_image;
					uint32_t m_imageSize;
					bool m_freeOnExit;
					ScePngDecImageInfo		m_imageInfo;
					ScePngDecParseParam	m_parseParam;
					sce::Gnm::SizeAlign m_sizeAlign;
					sce::Gnm::Texture m_texture;
					int init(void)
					{
						
						memset(&m_parseParam, 0, sizeof(m_parseParam));
						m_parseParam.pngMemAddr	= m_image;
						m_parseParam.pngMemSize	= m_imageSize;
						int ret = scePngDecParseHeader(&m_parseParam, &m_imageInfo);
						if(ret < 0){
							return ret;
						}

						sce::Gnm::TextureSpec spec;
						spec.init();
						spec.m_textureType = sce::Gnm::kTextureType2d;
						spec.m_width = m_imageInfo.imageWidth;
						spec.m_height = m_imageInfo.imageHeight;
						spec.m_depth = 1;
						spec.m_pitch = 0;
						spec.m_numMipLevels = 1;
						spec.m_numSlices = 1;
						spec.m_format = sce::Gnm::kDataFormatR8G8B8A8Unorm;
						spec.m_tileModeHint = sce::Gnm::kTileModeDisplay_LinearAligned;
						spec.m_minGpuMode = sce::Gnm::kGpuModeBase;
						spec.m_numFragments = sce::Gnm::kNumFragments1;
						int32_t status = m_texture.init(&spec);
						if (status != 0) {
							return -1;
						}
						m_sizeAlign = m_texture.getSizeAlign();
						return SCE_OK;
					}
				public:
					PngFile(void)
					{
						m_image = NULL;
						m_freeOnExit = false;
					}

					int open(const char *path)
					{
						uint32_t size;
						m_image = sce::SampleUtil::Internal::Util::malloc_and_load(path, &size);
						if(m_image == NULL){
							return -1;
						}
						m_imageSize = size;
						m_freeOnExit = true;
						return init();
					}
					int openFromMemory(const void *image, uint32_t size)
					{
						if(image == NULL){
							return -1;
						}
						m_image = image;
						m_imageSize = size;
						m_freeOnExit = false;
						return init();
					}
	
					uint32_t getWidth(void) const
					{
						return m_texture.getWidth();
					}
					uint32_t getHeight(void) const
					{
						return m_texture.getHeight();
					}
					uint32_t getStride(void) const {
						return m_texture.getPitch() * 4;
					}
					uint32_t getMipCount(void) const
					{
						return 1;
					}
	
					int readData(void *decodedImage) const
					{
						// query size
						ScePngDecCreateParam	createParam;
						memset(&createParam, 0, sizeof(createParam));
						createParam.thisSize		= sizeof(createParam);
						createParam.attribute		= m_imageInfo.bitDepth >> 4;
						createParam.maxImageWidth	= m_imageInfo.imageWidth;
						int ret = scePngDecQueryMemorySize(&createParam);
						if(ret < 0){
							return ret;
						}
						// alloc memory
						size_t decoderMemorySize = ret;
						void *decoderMemory = malloc(decoderMemorySize);

						ScePngDecHandle handle;
						ret = scePngDecCreate(&createParam, decoderMemory, decoderMemorySize, &handle);
						if (ret < 0) {
							if(decoderMemory != NULL){free(decoderMemory);	}
							return ret;
						}
						// decode
						ScePngDecDecodeParam	decodeParam;
						memset(&decodeParam, 0, sizeof(decodeParam));
						decodeParam.pngMemAddr	= m_image;
						decodeParam.pngMemSize	= m_imageSize;
						decodeParam.imageMemAddr= decodedImage;
						decodeParam.imageMemSize= m_sizeAlign.m_size;
						decodeParam.imagePitch	= m_texture.getPitch() *4;
						decodeParam.pixelFormat	= SCE_JPEG_DEC_PIXEL_FORMAT_R8G8B8A8;
						decodeParam.alphaValue	= 255;
						ret = scePngDecDecode(handle, &decodeParam, NULL);
						if(ret >= 0){
							ret = SCE_OK;
						}
						if(ret != SCE_OK){
							printf("Error : sceJpegDecDecode() => %#x\n", ret);
						}
						scePngDecDelete(handle);
						if(decoderMemory != NULL){free(decoderMemory);	}
						return ret;
					}					
	
					int close(void)
					{
						if(m_freeOnExit){
							free((void*)m_image);
							m_image = NULL;
							m_freeOnExit = false;
						}
						return SCE_OK;
					}
				};

#if 0
				class TextureToolFile
				{
				private:
					sce::TextureTool::GnmTextureGen *m_loadedImage;
					sce::TextureTool::Image *m_srcImage;
					sce::Gnm::Texture m_texture;
					sce::Gnm::SizeAlign m_sizeAlign;
				public:
					TextureToolFile(void)
					{
						m_loadedImage = NULL;
						m_srcImage    = NULL;
					}

					int open(const char *path)
					{
						m_loadedImage = sce::TextureTool::loadImage( path, sce::TextureTool::ImageReader::kFileTypeJpg);
						if(m_loadedImage == NULL){
							return -1;
						}
						m_srcImage = NULL;
						if(m_loadedImage->getType() == sce::TextureTool::k2DImageMipped){
							m_srcImage = (static_cast<sce::TextureTool::MippedImage*> (m_loadedImage))->getImage(0); // get the mip 0
						}else{
							SCE_GNM_ASSERT(m_loadedImage->getType() == sce::TextureTool::k2DImage);
							m_srcImage = (static_cast<sce::TextureTool::Image*> (m_loadedImage));
						}
						m_sizeAlign = m_texture.initAs2d(m_srcImage->getWidth(), m_srcImage->getHeight(), 1,
							sce::Gnm::kDataFormatR8G8B8A8Unorm,
							sce::Gnm::kTileModeDisplay_LinearAligned, sce::Gnm::kNumSamples1);

						return SCE_OK;
					}

					uint32_t getWidth(void) const
					{
						return m_texture.getWidth();
					}
					uint32_t getHeight(void) const
					{
						return m_texture.getHeight();
					}
					uint32_t getStride(void) const 
					{
						return m_texture.getPitch();
					}
					uint32_t getMipCount(void) const
					{
						return 1;
					}
	
					int readData(void *decodedImage) const
					{
						return m_srcImage->fillSurface((uint8_t*)decodedImage, m_sizeAlign.m_size, sce::Gnm::kTileModeDisplay_LinearAligned, sce::Gnm::kDataFormatR8G8B8A8Unorm);
					}					
	
					int close(void)
					{
						if(m_loadedImage != NULL){
							sce::TextureTool::deleteImage(m_loadedImage);
							m_loadedImage = NULL;
						}
						m_srcImage = NULL;
						return SCE_OK;
					}
				};
#endif


#endif /* _SCE_TARGET_OS_ORBIS */


				class ImageFile
				{
				public:
					enum FileFormat
						{
							kBmp, kTga, kGxt, kPng, kJpg, kGnf
						};
				private:
					TgaFile m_tgaFile;
					BmpFile m_bmpFile;
#if _SCE_TARGET_OS_PSP2
					GxtFile m_gxtFile;
					PngFile m_pngFile;
					JpegFile m_jpgFile;
#endif /* _SCE_TARGET_OS_PSP2 */

#if _SCE_TARGET_OS_ORBIS
					GnfFile m_gnfFile;
					JpegFile m_jpgFile;
					PngFile m_pngFile;
#endif // _SCE_TARGET_OS_ORBIS
					FileFormat m_fileFormat;
				public:
#if _SCE_TARGET_OS_PSP2
					int open(GraphicsLoaderImpl *loader, _JpegDecoder *jpegDecoder, const char *path);
					int openFromMemory(_JpegDecoder *jpegDecoder, const void *image, size_t imageSize);

					SceGxmTextureType getGxmType(void) const;
					SceGxmTextureFormat getGxmFormat(void) const;

#endif /* _SCE_TARGET_OS_PSP2 */
#if _SCE_TARGET_OS_WINDOWS || _SCE_TARGET_OS_ORBIS
					int open(const char *path);
					int openFromMemory(const void *image, size_t imageSize);
#endif // _SCE_TARGET_OS_WINDOWS

					int close(void);
					FileFormat getFileFormat(void) const;
					uint32_t getWidth(void) const;
					uint32_t getHeight(void) const;
					uint32_t getStride(void) const;
					uint32_t getMipCount(void) const;
					int readData(void* buffer); 

#if _SCE_TARGET_OS_ORBIS
                    GnfFile *getGnf(void)
                    {
					  return (m_fileFormat == kGnf) ? &m_gnfFile : NULL;
                    }
					size_t getGnmAlign(void) const;
					size_t getGnmSize(void) const;
					sce::Gnm::DataFormat getGnmDataFormat(void) const;
#endif

				};

			}
		}
	}
}


#if _SCE_TARGET_OS_PSP2


#endif /* _SCE_TARGET_OS_PSP2 */


#if _SCE_TARGET_OS_WINDOWS

	int _sceSampleUtilLoadImage(ID3D11Resource **texrure, const char* filename, ID3D11Device *loader);

#endif /* _SCE_TARGET_OS_WINDOWS */

#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_IMAGE_H */
