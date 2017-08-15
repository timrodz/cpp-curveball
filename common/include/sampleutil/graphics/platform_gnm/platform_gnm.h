/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_PLATFORM_GNM_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_PLATFORM_GNM_H


#include <scebase_common.h>
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#include <sampleutil/graphics/buffer.h>
#include <gnm.h>
#include <gnmx.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Gnm
			{
				class SCE_SAMPLE_UTIL_API BufferGnm : public BufferInterface
				{
				public:
					virtual ~BufferGnm(void){}

					/*!
					 * @~English
					 * @brief Returns the pointer to the data area of a buffer object 
					 * @details This returns the pointer to the data area of a buffer object. 
					 * 
					 * @return Pointer to the data area of a buffer object 
					 * @~Japanese
					 * @brief バッファオブジェクトのデータ領域へのポインタを返す 
					 * @details バッファオブジェクトのデータ領域へのポインタを返します。 
					 * 
					 * @return バッファオブジェクトのデータ領域へのポインタ 
					 */
					virtual void *getData(void) = 0;
					
					/*!
					 * @~English
					 * @brief Returns the pointer to the data area of a buffer object 
					 * @details This returns the pointer to the data area of a buffer object. 
					 * 
					 * @return Pointer to the data area of a buffer object 
					 * @~Japanese
					 * @brief バッファオブジェクトのデータ領域へのポインタを返す 
					 * @details バッファオブジェクトのデータ領域へのポインタを返します。 
					 * 
					 * @return バッファオブジェクトのデータ領域へのポインタ 
					 */
					virtual const void *getData(void) const = 0;
					
					/*!
					 * @~English
					 * @brief Returns the data area size of a buffer object 
					 * @details This returns the data area size of a buffer object. 
					 * 
					 * @return Data area size of a buffer object 
					 * @~Japanese
					 * @brief バッファオブジェクトのデータ領域のサイズを返す 
					 * @details バッファオブジェクトのデータ領域のサイズを返します。 
					 * 
					 * @return バッファオブジェクトのデータ領域のサイズ 
					 */
					virtual size_t  getSize(void) const = 0;

					/*!
					 * @~English
					 * @brief Returns the pointer to the data area of a buffer object 
					 * @details This returns the pointer to the data area of a buffer object. 
					 * 
					 * @return Pointer to the data area of a buffer object 
					 * @~Japanese
					 * @brief バッファオブジェクトのデータ領域へのポインタを返す 
					 * @details バッファオブジェクトのデータ領域へのポインタを返します。 
					 * 
					 * @return バッファオブジェクトのデータ領域へのポインタ 
					 */
					static BufferGnm *cast(BufferInterface *buffer);
					/*!
					 * @~English
					 * @brief Returns the pointer to the data area of a buffer object 
					 * @details This returns the pointer to the data area of a buffer object. 
					 * 
					 * @return Pointer to the data area of a buffer object 
					 * @~Japanese
					 * @brief バッファオブジェクトのデータ領域へのポインタを返す 
					 * @details バッファオブジェクトのデータ領域へのポインタを返します。 
					 * 
					 * @return バッファオブジェクトのデータ領域へのポインタ 
					 */
                    static const BufferGnm *cast(const BufferInterface *buffer);


				};
				class SCE_SAMPLE_UTIL_API RenderTargetGnm : public RenderTarget
				{
				public:
					
					virtual sce::Gnm::RenderTarget* getGnmRenderTarget(void) = 0;
					virtual ~RenderTargetGnm(void){}
				};

				class SCE_SAMPLE_UTIL_API GraphicsLoaderGnm : public GraphicsLoader
				{
				public:
					static GraphicsLoaderGnm* cast(GraphicsLoader *from);
					static const GraphicsLoaderGnm* cast(const GraphicsLoader *from);

					virtual void* memalignSystemSharedMemory(size_t boundary, size_t size) = 0;
					virtual void  freeSystemSharedMemory(void* ptr) = 0;
					virtual void* memalignVideoSharedMemory(size_t boundary, size_t size) = 0;
					virtual void  freeVideoSharedMemory(void* ptr) = 0;

					virtual ~GraphicsLoaderGnm(void){}
				};
				class SCE_SAMPLE_UTIL_API GraphicsContextGnm : public GraphicsContext, public GraphicsLoaderGnm
                {
                public:
					static GraphicsContextGnm* cast(GraphicsContext *from);
                    static const GraphicsContextGnm* cast(const GraphicsContext *from);
                    virtual ~GraphicsContextGnm(void){}

				    virtual int drawIndexAuto(Primitive primitive, uint32_t count) = 0;
                    virtual sce::Gnmx::GfxContext* getGfxContext(void) = 0;


                    virtual int readBuffer(void *dst, const BufferInterface* from, uint64_t offset, uint64_t numBytes) = 0;

					virtual int32_t getVideoOutHandle(void) const = 0;
					virtual int submitOnly(void) = 0;
					virtual int prepareNextFrame(void) = 0;
                };
			}
		}
	}
}


#endif // defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS
#endif // _SCE_SAMPLE_UTIL_GRAPHICS_PLATFORM_GNM_H
