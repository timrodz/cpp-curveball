/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_BUFFER_INTERNAL_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_BUFFER_INTERNAL_H


#include <scebase_common.h>
#if _SCE_TARGET_OS_ORBIS

#include <sampleutil/graphics.h>
#include <gnm.h>
#include <gnmx.h>
#include <shader.h>
#include "heap_gnm.h"
#include "sampleutil_internal.h"


namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class GraphicsContextImpl;
				class GraphicsLoaderImpl;
				class ImageFile;
				
				class BufferImpl : public Buffer, public Gnm::BufferGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
					friend class Impl::GraphicsContextImpl;
					friend class RenderTargetImpl;
				public:
					MemBlock m_buffer;
					GraphicsLoaderImpl *m_loader;

					size_t              m_elementSize;
					BufferAccessMode    m_accessMode;
					uint32_t            m_bufferBindFlags;
				public:
					BufferImpl(void){}
					virtual ~BufferImpl(void)
					{
						finalize();
					}
					int initialize(GraphicsLoaderImpl *loader, uint32_t size, 
								   BufferAccessMode    accessMode,
								   uint32_t            bufferBindFlags,
								   size_t              elementSize,
								   uint32_t alignment=16 /* For SSE Data*/);
					
					int finalize(void);
					
					
					void *getData(void) { return m_buffer.m_ptr;}
					
					const void *getData(void) const { return m_buffer.m_ptr;}

					size_t getSize(void) const { return m_buffer.m_size; }
					
					void *beginWrite(void) { 
						_mm_sfence();
						return  m_buffer.m_ptr;  
					}
					
					void endWrite(void) { 
						_mm_sfence();
					}

					GraphicsLoader* getLoader(void) const;

					uint32_t getBindFlags(void) const { return m_bufferBindFlags; }
					BufferAccessMode getAccessMode(void) const { return m_accessMode; }
					size_t getElementSize(void) const { return m_elementSize; }
					BufferDimension getDimension(void) const { return sce::SampleUtil::Graphics::kBufferDimensionBuffer; }

					std::string getResourceType(void) const
					{
						return "Graphics::Buffer";
					}

				};


				/* ------------------------------------------------------------------------------------------------------------- */


				class Texture2dBufferImpl : public Texture2dBuffer, public Gnm::BufferGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					sce::Gnm::Texture m_texture;
					MemBlock m_buffer;
					GraphicsLoaderImpl *m_loader;

					uint32_t m_width;
					uint32_t m_height;
					uint32_t m_mipCount;
					BufferFormat m_format;
					MultisampleMode m_multiSampleMode;
					
					BufferAccessMode m_accessMode;
					uint32_t m_bufferBindFlags;
					bool m_freeBuffer;
				public:

					Texture2dBufferImpl(void);
					~Texture2dBufferImpl(void);

					int initialize(GraphicsLoaderImpl *loader,
								   BufferFormat format,
								   uint32_t width,
								   uint32_t height,
								   uint32_t mipCount,
								   BufferAccessMode accessMode,
								   uint32_t bufferBindFlags, /* BufferBindFlag */
								   MultisampleMode multiSampleMode
								   );
					int initialize(GraphicsLoaderImpl *loader,
								   BufferFormat format,
								   uint32_t width,
								   uint32_t height,
								   uint32_t mipCount,
								   BufferAccessMode accessMode,
								   uint32_t bufferBindFlags, /* BufferBindFlag */
								   MultisampleMode multiSampleMode,
								   void *buffer
								   );


					int initializeAsUnknownBuffer(GraphicsLoaderImpl *loader,
						size_t bufferSize,
						size_t alignment,
						uint32_t width,
						uint32_t height,
						uint32_t mipCount,

						BufferAccessMode accessMode,
						uint32_t bufferBindFlags, /* BufferBindFlag */
						MultisampleMode multiSampleMode
						);

					int finalize(void);
								   

					uint32_t getWidth(void) const { return m_width; }
					uint32_t getHeight(void) const{ return m_height; }
					uint32_t getStride(void) const { return m_texture.getPitch() *4; /*todo: 4 assumes rgba*/ }
					uint32_t getMipCount(void) const { return m_mipCount; }

					uint32_t getBindFlags(void) const { return m_bufferBindFlags; }
					BufferAccessMode getAccessMode(void) const { return m_accessMode; }
					BufferFormat getFormat(void) const { return m_format; }
					MultisampleMode getMultiSampleMode(void) const { return m_multiSampleMode; } 

					void *getData(void)            { return m_buffer.m_ptr; }
					const void *getData(void) const{ return m_buffer.m_ptr; }
					void *beginWrite(void){
						_mm_sfence();
						return m_buffer.m_ptr;
					}
					void endWrite(void)            
					{  
						_mm_sfence();
					}
					GraphicsLoader* getLoader(void) const;
					size_t getSize(void) { return m_buffer.m_size; }

					BufferDimension getDimension(void) const { return sce::SampleUtil::Graphics::kBufferDimensionTexture2D; }
                    size_t  getSize(void) const { return m_buffer.m_size; }

					std::string getResourceType(void) const
					{
						return "Graphics::Texture2dBuffer";
					}

				};



				/* ------------------------------------------------------------------------------------------------------------- */

				
				class IndexBufferImpl : public IndexBuffer, public Gnm::BufferGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					IndexBufferImpl(void){}
					virtual ~IndexBufferImpl(void)  {	finalize();	}

					int initialize(GraphicsLoaderImpl *loader, uint32_t size);

					void *beginWrite(void)          { 
						return m_dataBuffer.beginWrite(); 
					}
					void endWrite(void)             { 
						m_dataBuffer.endWrite(); 
					}
					GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
                    BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					size_t getSize(void) const    { return m_dataBuffer.getSize(); }
					void *getData(void)             { return m_dataBuffer.getData(); }
					const void *getData(void) const { return m_dataBuffer.getData(); }
					int finalize(void)              { return m_dataBuffer.finalize(); }

					Buffer *getBaseBuffer(void) { return &m_dataBuffer; }
					std::string getResourceType(void) const
					{
						return "Graphics::IndexBuffer";
					}

				private:
					BufferImpl m_dataBuffer;

				};

				class VertexBufferImpl : public VertexBuffer, public Gnm::BufferGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					VertexBufferImpl(void){}
					virtual ~VertexBufferImpl(void) {  finalize();}

					int initialize(GraphicsLoaderImpl *loader, uint32_t size);

					void *beginWrite(void)          {
						return m_dataBuffer.beginWrite(); 
					}
					void endWrite(void)             {
						m_dataBuffer.endWrite();
					}
					GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
                    BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					size_t getSize(void) const    {  return m_dataBuffer.getSize(); }
					void *getData(void)             { return m_dataBuffer.getData(); }
					const void *getData(void) const { return m_dataBuffer.getData(); }
					int finalize(void)              { return m_dataBuffer.finalize(); }
					
					Buffer *getBaseBuffer(void){ return &m_dataBuffer; }
					std::string getResourceType(void) const
					{
						return "Graphics::VertexBuffer";
					}

				private:
					BufferImpl m_dataBuffer;

				};

				class UniformBufferImpl : public UniformBuffer, public Gnm::BufferGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					UniformBufferImpl(void){}
					virtual ~UniformBufferImpl(void){	finalize();	}

					int initialize(GraphicsLoaderImpl *loader, uint32_t size);

					void *beginWrite(void)          { 
						return m_dataBuffer.beginWrite(); 
					}
					void endWrite(void)             { 
						m_dataBuffer.endWrite();
					}
					GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
                    BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					size_t getSize(void) const    {  return m_dataBuffer.getSize(); }
					void *getData(void)             { return m_dataBuffer.getData(); }
					const void *getData(void) const { return m_dataBuffer.getData(); }
					int finalize(void)              { return m_dataBuffer.finalize(); }

					Buffer *getBaseBuffer(void) { return &m_dataBuffer; }
					std::string getResourceType(void) const
					{
						return "Graphics::UniformBuffer";
					}

				private:
					BufferImpl m_dataBuffer;

				};

				class StructuredBufferImpl : public StructuredBuffer, public Gnm::BufferGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					StructuredBufferImpl(void){}
					virtual ~StructuredBufferImpl(void)	{	finalize();	}

					int initialize(GraphicsLoaderImpl *loader, uint32_t stride, uint32_t numElements)
					{
						int ret = m_dataBuffer.initialize(loader, stride * numElements,
														  kBufferAccessModeGpuReadCpuWrite,
														  kBufferBindFlagConstantBuffer,
														  stride);
						if(ret != SCE_OK){
							return ret;
						}
						m_gnmBuffer.initAsRegularBuffer( m_dataBuffer.getData(), stride, numElements);

						return SCE_OK;
					}

					void *beginWrite(void)          { 
						return m_dataBuffer.beginWrite(); 
					}
					void endWrite(void)             { m_dataBuffer.endWrite(); }
					GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
                    BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					size_t getSize(void) const    {  return m_dataBuffer.getSize(); }
					void *getData(void)             { return m_dataBuffer.getData(); }
					const void *getData(void) const { return m_dataBuffer.getData(); }
					int finalize(void)              { return m_dataBuffer.finalize(); }

					const sce::Gnm::Buffer *getGnmBuffer(void) const { return &m_gnmBuffer; }
					Buffer *getBaseBuffer(void) { return &m_dataBuffer; }
					std::string getResourceType(void) const
					{
						return "Graphics::StructuredBuffer";
					}

				private:
					sce::Gnm::Buffer m_gnmBuffer;
					BufferImpl m_dataBuffer;
					
				};

				class RwStructuredBufferImpl : public RwStructuredBuffer, public Gnm::BufferGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					RwStructuredBufferImpl(void){}
					virtual ~RwStructuredBufferImpl(void)
					{
						finalize();
					}

					int initialize(GraphicsLoaderImpl *loader, uint32_t stride, uint32_t numElements)
					{
						int ret = m_dataBuffer.initialize(loader, stride * numElements,
														  kBufferAccessModeGpuReadWrite,
														  kBufferBindFlagUnorderedAccess,
														  stride);
						if(ret != SCE_OK){
							return ret;
						}
						//printf("rw structured m_dataBuffer.getAddress() = %#lx\n", m_dataBuffer.getAddress());
						m_gnmBuffer.initAsRegularBuffer( m_dataBuffer.getData(), stride, numElements);

						//printf("rw structured m_dataBuffer.getAddress() = %#lx\n", this->getAddress());
						return SCE_OK;
					}

					void *beginWrite(void)          {
						return m_dataBuffer.beginWrite(); 
					}
					void endWrite(void)             { m_dataBuffer.endWrite(); }
					GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
                    BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					size_t getSize(void) const    {  return m_dataBuffer.getSize(); }
					void *getData(void)             { return m_dataBuffer.getData(); }
					const void *getData(void) const { return m_dataBuffer.getData(); }
					int finalize(void)              { return m_dataBuffer.finalize(); }

					const sce::Gnm::Buffer *getGnmBuffer(void) const { return &m_gnmBuffer; }
					Buffer *getBaseBuffer(void) { return &m_dataBuffer; }
					std::string getResourceType(void) const
					{
						return "Graphics::RwStructuredBuffer";
					}

				private:
					sce::Gnm::Buffer m_gnmBuffer;
					BufferImpl m_dataBuffer;

				};

				class TextureImpl : public Texture, public Gnm::BufferGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					Texture2dBufferImpl *m_textureBuffer;
					sce::Gnm::Texture m_texture;
					sce::Gnm::Sampler m_sampler;
					TextureImpl(void) { 
						m_textureBuffer = NULL;
						m_freeTexture = false;
					}
					virtual ~TextureImpl(void) { finalize(); }

					int initializeFromImageFile(GraphicsLoader *loader, ImageFile &imageFile);
					int initializeFromFile(GraphicsLoader *loader, const char *path);
					int initializeFromMemory(GraphicsLoader *loader, const void *data, uint32_t size);
					int initialize(GraphicsLoader *loader, uint32_t width, uint32_t height,   uint32_t mipCount,  TextureFormat format);
					int initializeFromBuffer(GraphicsLoader *loader, Texture2dBufferImpl *buffer, sce::Gnm::DataFormat format, sce::Gnm::TileMode tileMode=sce::Gnm::kTileModeDisplay_LinearAligned);

					int setAddrMode(TextureAddrMode uAddrMode,	TextureAddrMode vAddrMode);
					int setFilter(TextureFilter minFilter, TextureFilter magFilter,  TextureMipFilter mipFilter);

					uint32_t getWidth(void) const;
					uint32_t getHeight(void) const;
					uint32_t getStride(void) const;

					void *beginWrite(void)          {
						return m_textureBuffer->beginWrite(); 
					}
					void endWrite(void)             { m_textureBuffer->endWrite(); }
					GraphicsLoader* getLoader(void) const { return  m_textureBuffer->getLoader();}
					BufferDimension getDimension(void) const { return m_textureBuffer->getDimension(); }
					//size_t getSize(void)  const   {  return  m_textureBuffer->getSize(); }
					void *getData(void)             { return  m_textureBuffer->getData(); }
					const void *getData(void) const { return  m_textureBuffer->getData(); }
                   size_t  getSize(void) const { return m_textureBuffer->getSize(); }
					int finalize(void);
					std::string getResourceType(void) const
					{
						return "Graphics::Texture";
					}

				private:
					bool m_freeTexture;
				};


				class RenderTargetImpl : public Gnm::RenderTargetGnm, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					uint32_t m_width;
					uint32_t m_height;
					Texture2dBufferImpl *m_textureBuffer;
                    TextureImpl m_texture;
					bool m_freeTextureBuffer;
					bool m_initialized;
				public:
					RenderTargetImpl(void);

					sce::Gnm::RenderTarget m_renderTarget;
					GraphicsLoaderImpl *m_loader;
					int initialize(
								   GraphicsLoaderImpl *loader,
								   uint32_t width, uint32_t height,
								   sce::Gnm::DataFormat colorFormat, 
								   sce::Gnm::TileMode tileMode,
								   sce::Gnm::NumSamples numSamples, 
								   sce::Gnm::NumFragments numFragments,
								   bool cmask=true, bool fmask=true);

					int finalize(void);


					uint32_t getWidth(void) const;

					// TODO: Add standard code header comment here.
					uint32_t getHeight(void) const;

					Texture    *getTexture(void)
					{
						return &m_texture;
					}

					virtual ~RenderTargetImpl(void);

					sce::Gnm::RenderTarget* getGnmRenderTarget(void)
					{
						return &m_renderTarget;
					}
					std::string getResourceType(void) const
					{
						return "Graphics::RenderTarget";
					}

				};

				class DepthStencilSurfaceImpl : public DepthStencilSurface, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					sce::Gnm::DepthRenderTarget m_depthTarget;
					GraphicsLoaderImpl *m_loader;
					uint32_t m_width;
					uint32_t m_height;

					Texture2dBufferImpl *m_textureBuffer;
                    TextureImpl m_texture;
					bool m_freeTextureBuffer;

				public:
					int initialize(GraphicsLoaderImpl *loader,
								   uint32_t width, uint32_t height,
								   bool useStencil,
								   bool useHtile);

					// TODO: Add standard code header comment here.
					Texture *getTexture(void)
					{
						return &m_texture;
					}
					virtual ~DepthStencilSurfaceImpl(void);
					
					int finalize(void);

					sce::Gnm::DepthRenderTarget* getGnmDepthRenderTarget(void)
					{
						return &m_depthTarget;
					}

					std::string getResourceType(void) const
					{
						return "Graphics::DepthStencilSurface";
					}

				};

			}
		}
	}
}

#endif //_SCE_TARGET_OS_ORBIS

#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_BUFFER_INTERNAL_H */

