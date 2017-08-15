/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_BUFFER_INTERNAL_GXM_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_BUFFER_INTERNAL_GXM_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/graphics.h>
#include <sampleutil_internal.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class GraphicsLoaderImpl;

				class BufferImpl : public Buffer, public Gxm::BufferGxm, public sce::SampleUtil::Impl::ResourceImpl
				{
					friend class GraphicsLoaderImpl;
					friend class GraphicsContextImpl;
					friend class Effect::EffectParameter;
					friend class Effect::EffectInstance;
				protected:
					void *m_data;
					uint32_t m_size;
					GraphicsLoader *m_loader;

					size_t              m_elementSize;

					BufferAccessMode    m_accessMode;
					uint32_t            m_bufferBindFlags;
				public:
					BufferImpl(void);
					int initialize(GraphicsLoaderImpl *loader, uint32_t size, 
								   BufferAccessMode    accessMode,
								   uint32_t            bufferBindFlags,
								   size_t              elementSize,
								   uint32_t bufferAlign=16);

					void *getData(void);
					const void *getData(void) const;
					uint32_t getSize(void) const { return m_size; }
					void *beginWrite(void);
					void endWrite(void);
					GraphicsLoader* getLoader(void) const ;
					virtual ~BufferImpl(void);

					int finalize(void);

					uint32_t getBindFlags(void) const { return m_bufferBindFlags; }
					BufferAccessMode getAccessMode(void) const { return m_accessMode; }
					size_t getElementSize(void) const { return m_elementSize; }
					BufferDimension getDimension(void) const { return kBufferDimensionBuffer;} 

					std::string getResourceType(void) const
					{
						return "Graphics::Buffer";
					}
				};

				/*------------------------------------------------------------------------------------------------------------*/

				class Texture2dBufferImpl : public Texture2dBuffer, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					void *m_data;
					uint32_t m_size;
					GraphicsLoaderImpl *m_loader;

					uint32_t m_width;
					uint32_t m_height;
					uint32_t m_mipCount;
					uint32_t m_stride;
					SceGxmTextureType m_type;
					
					BufferFormat     m_format;
					BufferAccessMode m_accessMode;
					uint32_t         m_bufferBindFlags; /* BufferBindFlag */
					MultisampleMode  m_multiSampleMode;
					std::string getResourceType(void) const
					{
						return "Graphics::Texture2dBuffer";
					}
					bool m_freeOnFinalize;
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
								   MultisampleMode multiSampleMode,
								   uint32_t bufferAlign=16
								   );

					int initializeFromExistentBuffer(
						GraphicsLoaderImpl *loader,
						void *data,
						BufferFormat format,
						uint32_t width,
						uint32_t height,
						uint32_t mipCount,
						BufferAccessMode accessMode,
						uint32_t bufferBindFlags, /* BufferBindFlag */
						MultisampleMode multiSampleMode,
						uint32_t bufferAlign=16
						);

					int finalize(void);

					// format is initialized as kBufferFormatUnknown
					int initializeFromTexture(GraphicsLoaderImpl *loader,
											  SceGxmTexture *texture );
								   
					uint32_t getWidth(void) const;
					uint32_t getHeight(void) const;
					uint32_t getStride(void) const;
					uint32_t getMipCount(void) const;
					SceGxmTextureType getType(void) const {return m_type;}
	
					void *getData(void)            { return m_data; }
					const void *getData(void) const{ return m_data; }
					void *beginWrite(void)         { return m_data; }
					void endWrite(void)            { }
					GraphicsLoader* getLoader(void) const;

					uint32_t getBindFlags(void) const           { return m_bufferBindFlags; }
					BufferAccessMode getAccessMode(void) const  { return m_accessMode; }
					BufferFormat getFormat(void) const          { return m_format; }
					MultisampleMode getMultiSampleMode(void) const { return m_multiSampleMode; }
					BufferDimension getDimension(void) const { return kBufferDimensionTexture2D;}
				};


				/*------------------------------------------------------------------------------------------------------------*/


				class IndexBufferImpl : public IndexBuffer, public Gxm::BufferGxm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					IndexBufferImpl(void){}

					int initialize(GraphicsLoaderImpl *loader, uint32_t size);

					virtual void *beginWrite(void)        { return m_dataBuffer.beginWrite(); }
					virtual void endWrite(void)           { m_dataBuffer.endWrite(); }
					virtual GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
					virtual ~IndexBufferImpl(void){	finalize();	}
					int finalize(void){ return m_dataBuffer.finalize(); }

					void *getData(void){ return m_dataBuffer.getData(); }
					const void *getData(void) const{ return m_dataBuffer.getData(); }

					Buffer *getBaseBuffer(void) { return &m_dataBuffer; } 
					BufferDimension getDimension(void) const { return m_dataBuffer.getDimension();} 
					std::string getResourceType(void) const
					{
						return "Graphics::IndexBuffer";
					}

				private:
					BufferImpl m_dataBuffer;
					
				};

				class VertexBufferImpl : public VertexBuffer, public Gxm::BufferGxm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					VertexBufferImpl(void){}

					int initialize(GraphicsLoaderImpl *loader, uint32_t size);

					virtual void *beginWrite(void)        { return m_dataBuffer.beginWrite(); }
					virtual void endWrite(void)           { m_dataBuffer.endWrite(); }
					virtual GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
					virtual ~VertexBufferImpl(void){	finalize();	}
					int finalize(void){ return m_dataBuffer.finalize(); }

					void *getData(void){ return m_dataBuffer.getData(); }
					const void *getData(void) const{ return m_dataBuffer.getData(); }
					Buffer *getBaseBuffer(void) { return &m_dataBuffer; } 
					BufferDimension getDimension(void) const { return m_dataBuffer.getDimension();} 
					std::string getResourceType(void) const
					{
						return "Graphics::VertexBuffer";
					}

				private:
					BufferImpl m_dataBuffer;
				};

				class UniformBufferImpl : public UniformBuffer, public Gxm::BufferGxm, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					UniformBufferImpl(void){}

					int initialize(GraphicsLoaderImpl *loader, uint32_t size);

					virtual void *beginWrite(void)        { return m_dataBuffer.beginWrite(); }
					virtual void endWrite(void)           { m_dataBuffer.endWrite(); }
					virtual GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
					virtual ~UniformBufferImpl(void){	finalize();	}
					int finalize(void){ return m_dataBuffer.finalize(); }

					void *getData(void){ return m_dataBuffer.getData(); }
					const void *getData(void) const{ return m_dataBuffer.getData(); }
					Buffer *getBaseBuffer(void) { return &m_dataBuffer; } 
					BufferDimension getDimension(void) const { return m_dataBuffer.getDimension();} 
					std::string getResourceType(void) const
					{
						return "Graphics::UniformBuffer";
					}

				private:
					BufferImpl m_dataBuffer;
				};



				class TextureImpl : public Gxm::TextureGxm, public BufferInterface, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					Texture2dBufferImpl *m_buffer;
					SceGxmTexture m_texture;
					bool m_freeBufferOnFinalize;
				public:
					TextureImpl(void);

					virtual ~TextureImpl(void)
					{
						finalize();
					};
					int initializeFromFile(GraphicsLoaderImpl *loader, const char *filename);
					int initializeFromMemory(GraphicsLoaderImpl *loader, const void *image, uint32_t gxtImageSize);
					int initialize(GraphicsLoaderImpl *loader, 
								   TextureFormat texFormat, 
								   uint32_t width,
								   uint32_t height,
								   uint32_t mipCount);
					int initializeGxm(GraphicsLoaderImpl *loader, 
									  SceGxmTextureType type, SceGxmTextureFormat texFormat, 
									  uint32_t width, uint32_t height, uint32_t stride, uint32_t mipCount,
									  uint32_t bufferAlign=16);

					uint32_t getWidth(void) const;
					uint32_t getHeight(void) const;
					uint32_t getStride(void) const;

					int setFilter(TextureFilter minFilter,  TextureFilter magFilter,  TextureMipFilter mipFilter);
					int setAddrMode(TextureAddrMode uAddrMode, 	TextureAddrMode vAddrMode);

					int finalize(void);

					int initializeGxmLinear(
											GraphicsLoaderImpl *loader, 
											SceGxmTextureFormat texFormat,
											uint32_t width,
											uint32_t height,
											uint32_t mipCount,
											uint32_t bufferAlign=16);

					int initializeGxmLinearStrided(
												   GraphicsLoaderImpl *loader, 
												   SceGxmTextureFormat texFormat,
												   uint32_t width,
												   uint32_t height,
												   uint32_t byteStride,
												   uint32_t bufferAlign=16);

					int initializeGxmTiled(
										   GraphicsLoaderImpl *loader, 
										   SceGxmTextureFormat texFormat,
										   uint32_t width,
										   uint32_t height,
										   uint32_t mipCount,
										   uint32_t bufferAlign=16);

					int initializeGxmSwizzled(
											  GraphicsLoaderImpl *loader, 
											  SceGxmTextureFormat texFormat,
											  uint32_t width,
											  uint32_t height,
											  uint32_t mipCount,
											  uint32_t bufferAlign=16);

					int initializeGxmCube(
										  GraphicsLoaderImpl *loader, 
										  SceGxmTextureFormat texFormat,
										  uint32_t width,
										  uint32_t height,
										  uint32_t mipCount,
										  uint32_t bufferAlign=16);

					int initializeFromTexture2dBuffer(Texture2dBufferImpl *buffer);

					SceGxmTexture *getGxmTexture(void)
					{
						return &m_texture;
					}
					const SceGxmTexture *getGxmTexture(void) const
					{
						return &m_texture;
					}

					void *beginWrite(void)        { return m_buffer->beginWrite(); }
					void endWrite(void)           { m_buffer->endWrite(); }
					GraphicsLoader* getLoader(void) const { return m_buffer->getLoader();}
					void *getData(void) { return m_buffer->getData(); }
					BufferDimension getDimension(void) const { return m_buffer->getDimension();} 
					std::string getResourceType(void) const
					{
						return "Graphics::Texture";
					}

				};


				class RenderTargetImpl : public RenderTarget, public sce::SampleUtil::Impl::ResourceImpl
				{
					friend class GraphicsContextImpl;
				private:
					uint32_t                    m_flags;
					SceGxmRenderTarget          *m_renderTarget;
					const SceGxmValidRegion*   m_validRegion;
					SceGxmSyncObject*           m_vertexSyncObject;
					SceGxmSyncObject*           m_fragmentSyncObject;

					SceGxmColorSurface          m_colorSurface;
					TextureImpl    m_colorTexture;

					SceGxmOutputRegisterSize m_outputRegisterSize;
					SceGxmMultisampleMode    m_msaaMode;
				public:
					RenderTargetImpl(void)
					{
						m_renderTarget = NULL;
					}
					~RenderTargetImpl(void)
					{
						finalize();
					}

					int initialize(GraphicsLoaderImpl *loader, 
								   uint32_t width,
								   uint32_t height,
								   SceGxmColorSurfaceType type=SCE_GXM_COLOR_SURFACE_LINEAR,
								   SceGxmColorFormat format=SCE_GXM_COLOR_FORMAT_U8U8U8U8_ABGR,
								   SceGxmOutputRegisterSize outputRegisterSize = SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,

								   uint32_t renderTargetFlags=0,
								   uint16_t scenesPerFrame=1,
								   SceGxmMultisampleMode msaaMode=SCE_GXM_MULTISAMPLE_NONE,
								   uint32_t multisampleLocations=0,

								   SceGxmSyncObject*  vertexSyncObject=NULL,
								   SceGxmSyncObject*  fragmentSyncObject=NULL);

					int initializeInternal(void *buffer, 
						uint32_t width,
						uint32_t height,
						SceGxmColorSurfaceType type=SCE_GXM_COLOR_SURFACE_LINEAR,
						SceGxmColorFormat format=SCE_GXM_COLOR_FORMAT_U8U8U8U8_ABGR,
						SceGxmOutputRegisterSize outputRegisterSize = SCE_GXM_OUTPUT_REGISTER_SIZE_32BIT,
						
						uint32_t renderTargetFlags=0,
						uint16_t scenesPerFrame=1,
						SceGxmMultisampleMode msaaMode=SCE_GXM_MULTISAMPLE_NONE,
						uint32_t multisampleLocations=0,
						
						SceGxmSyncObject*  vertexSyncObject=NULL,
						SceGxmSyncObject*  fragmentSyncObject=NULL);


					virtual uint32_t getWidth(void) const;

					virtual uint32_t getHeight(void) const;


					int finalize(void);

					SceGxmRenderTarget *getGxmRenderTarget(void)
					{
						return m_renderTarget;
					}

					SceGxmColorSurface *getGxmColorSurface(void)
					{
						return &m_colorSurface;
					}

					virtual Texture    *getTexture(void)
					{
						return &m_colorTexture;
					}

					SceGxmSyncObject* getGxmVertexSyncObject(void)
					{
						return m_vertexSyncObject;
					}

					SceGxmSyncObject* getGxmFragmentSyncObject(void)
					{
						return m_fragmentSyncObject;
					}

					SceGxmOutputRegisterSize getGxmOutputRegisterSize(void) const
					{
						return m_outputRegisterSize;
					}

					SceGxmMultisampleMode  getGxmMultisampleMode(void) const
					{
						return m_msaaMode;
					}
					std::string getResourceType(void) const
					{
						return "Graphics::RenderTarget";
					}

				};

				class DepthStencilSurfaceImpl : public DepthStencilSurface, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					SceGxmDepthStencilSurface    m_depthStencilSurface;
					TextureImpl	 m_depthStencilTexture;
				public:
					DepthStencilSurfaceImpl(void){}

					~DepthStencilSurfaceImpl(void){}

					int initialize(
								   GraphicsLoaderImpl *loader, 
								   uint32_t width,
								   uint32_t height, 
								   SceGxmDepthStencilFormat fort = SCE_GXM_DEPTH_STENCIL_FORMAT_DF32,
								   SceGxmDepthStencilSurfaceType type = SCE_GXM_DEPTH_STENCIL_SURFACE_TILED,
								   SceGxmMultisampleMode multisampleMode=SCE_GXM_MULTISAMPLE_NONE,
								   bool enableForceStore=false,
								   bool enableForceLoad=false
								   );

					int finalize(void);

					SceGxmDepthStencilSurface* getGxmDepthStencilSurface(void)
					{
						return &m_depthStencilSurface;
					}

					virtual Texture *getTexture(void)
					{
						return &m_depthStencilTexture;
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


#endif // _SCE_TARGET_OS_PSP2
#endif //_SCE_SAMPLE_UTIL_GRAPHICS_BUFFER_INTERNAL_GXM_H
