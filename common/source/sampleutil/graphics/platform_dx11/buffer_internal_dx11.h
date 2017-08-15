/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_BUFFER_INTERNAL_DX11_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_BUFFER_INTERNAL_DX11_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include <sampleutil/graphics.h>
#include <dxgi.h>
#include <D3D11.h>
#include <DXGI.h>
#include <D3D11Shader.h>
#include <D3Dcompiler.h>
#include <sampleutil_internal.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				// -----------------------------------------------------------------------------
				// Buffer
				// -----------------------------------------------------------------------------


				class GraphicsLoaderImpl;

				class BufferImpl : public Buffer, public sce::SampleUtil::Impl::ResourceImpl
				{
					friend class GraphicsLoaderImpl;
					friend class GraphicsContextImpl;
					friend class Effect::EffectParameter;
					friend class Effect::EffectInstance;
				public:
					ID3D11Resource *m_buffer;
					GraphicsLoader *m_loader;

					uint32_t            m_size;
					size_t              m_elementSize;
					BufferAccessMode    m_accessMode;
					uint32_t            m_bufferBindFlags;

					

					BufferImpl(void);
					int initialize(GraphicsLoaderImpl *loader, uint32_t size, 
								   BufferAccessMode    accessMode,
								   uint32_t            bufferBindFlags,
								   size_t              elementSize,
								   uint32_t bufferAlign=16);

					int finalize(void);
					std::string getResourceType(void) const
					{
						return "Graphics::Buffer";
					}

				public:

					ID3D11Resource *getResource(void) const
					{
						return m_buffer;
					}

					virtual void *beginWrite(void);
					virtual void endWrite(void);

					virtual GraphicsLoader* getLoader(void) const;
					virtual ~BufferImpl(void);

					uint32_t getBindFlags(void) const { return m_bufferBindFlags; }
					BufferAccessMode getAccessMode(void) const { return m_accessMode; }
					size_t getElementSize(void) const { return m_elementSize; }

					size_t getSize(void) const { return m_size; }
					BufferDimension getDimension(void) const { return kBufferDimensionBuffer; }
				};



				


				class Texture2dBufferImpl : public Texture2dBuffer, public sce::SampleUtil::Impl::ResourceImpl
				{
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

					int initializeFromExistentTexture(GraphicsLoaderImpl *loader, ID3D11Texture2D * DSBuffer);

					uint32_t getWidth(void) const;
					uint32_t getHeight(void) const;
					uint32_t getStride(void) const;
					uint32_t getMipCount(void) const;

					BufferFormat getFormat(void) const { return m_format;}
					MultisampleMode getMultiSampleMode(void) const { return m_multiSampleMode; }
					uint32_t getBindFlags(void) const { return m_bufferBindFlags; }
					BufferAccessMode getAccessMode(void) const { return m_accessMode; }

					DXGI_FORMAT getDxgiFormat(void) const;
					uint32_t getD3d11BindFlag(void) const;
					bool isMultiSample(void) const;

					
					void *beginWrite(void);
					void endWrite(void);
					GraphicsLoader* getLoader(void) const;
				 
					ID3D11Resource *getResource(void) { return m_buffer; }

					int finalize(void);
					BufferDimension getDimension(void) const { return kBufferDimensionTexture2D; }

					std::string getResourceType(void) const
					{
						return "Graphics::Texture2dBuffer";
					}

				protected:
					bool m_freeTexture;
					BufferFormat m_format;
					BufferAccessMode m_accessMode;
					uint32_t m_bufferBindFlags;
					MultisampleMode m_multiSampleMode;
					
					ID3D11Resource *m_buffer;
					GraphicsLoader *m_loader;

				};


				// -----------------------------------------------------------------------------
				// Resource View
				// -----------------------------------------------------------------------------




				class IndexBufferImpl : public IndexBuffer, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					IndexBufferImpl(void){}

					int initialize(GraphicsLoaderImpl *loader, uint32_t size);

					ID3D11Buffer *getBuffer(void) const { return (ID3D11Buffer *)m_dataBuffer.getResource();	}
					void *beginWrite(void)        { return m_dataBuffer.beginWrite(); }
					void endWrite(void)           { m_dataBuffer.endWrite(); }
					 GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
					~IndexBufferImpl(void){	finalize();	}
					int finalize(void){ return m_dataBuffer.finalize(); }

					Buffer *getBaseBuffer(void) { return &m_dataBuffer; }
					BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					std::string getResourceType(void) const
					{
						return "Graphics::IndexBuffer";
					}

				private:
					BufferImpl m_dataBuffer;
					
				};

				class VertexBufferImpl : public VertexBuffer, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					VertexBufferImpl(void){}

					int initialize(GraphicsLoaderImpl *loader, uint32_t size);

					ID3D11Buffer *getBuffer(void) const { return (ID3D11Buffer *)m_dataBuffer.getResource();	}
					void *beginWrite(void)        { return m_dataBuffer.beginWrite(); }
					void endWrite(void)           { m_dataBuffer.endWrite(); }
					GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
					~VertexBufferImpl(void){	finalize();	}
					int finalize(void){ return m_dataBuffer.finalize(); }
					Buffer *getBaseBuffer(void) { return &m_dataBuffer; }
					BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					std::string getResourceType(void) const
					{
						return "Graphics::VertexBuffer";
					}

				private:
					BufferImpl m_dataBuffer;
				};

				class UniformBufferImpl : public UniformBuffer, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					UniformBufferImpl(void){}

					int initialize(GraphicsLoaderImpl *loader, uint32_t size);

					ID3D11Buffer *getBuffer(void) const { return (ID3D11Buffer *)m_dataBuffer.getResource();	}
					virtual void *beginWrite(void)        { return m_dataBuffer.beginWrite(); }
					virtual void endWrite(void)           { m_dataBuffer.endWrite(); }
					virtual GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
					virtual ~UniformBufferImpl(void){	finalize();	}
					int finalize(void){ return m_dataBuffer.finalize(); }
					Buffer *getBaseBuffer(void) { return &m_dataBuffer; }
					BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					std::string getResourceType(void) const
					{
						return "Graphics::UniformBuffer";
					}

				private:
					BufferImpl m_dataBuffer;
				};

				class StructuredBufferImpl : public StructuredBuffer, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					StructuredBufferImpl(void)
					{
						m_srv = NULL;
					}

					int initialize(GraphicsLoaderImpl *loader, uint32_t stride, uint32_t numElements);

					ID3D11Buffer *getBuffer(void) const { return (ID3D11Buffer *)m_dataBuffer.getResource();	}
					virtual void *beginWrite(void)        { return m_dataBuffer.beginWrite(); }
					virtual void endWrite(void)           { m_dataBuffer.endWrite(); }
					virtual GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
					virtual ~StructuredBufferImpl(void){	finalize();	}
					int finalize(void){ return m_dataBuffer.finalize(); }

					ID3D11ShaderResourceView *getD3D11ShaderResourceView(void) const { return m_srv; }

					Buffer *getBaseBuffer(void) { return &m_dataBuffer; }
					BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					std::string getResourceType(void) const
					{
						return "Graphics::StructuredBuffer";
					}

				private:
					BufferImpl m_dataBuffer;
					ID3D11ShaderResourceView *m_srv;
				};

				class RwStructuredBufferImpl : public RwStructuredBuffer, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:	
					RwStructuredBufferImpl(void)
					{
						m_uav = 0;
					}

					int initialize(GraphicsLoaderImpl *loader, uint32_t stride, uint32_t numElements);

					ID3D11Buffer *getBuffer(void) const { return (ID3D11Buffer *)m_dataBuffer.getResource();	}
					virtual void *beginWrite(void)        { return m_dataBuffer.beginWrite(); }
					virtual void endWrite(void)           { m_dataBuffer.endWrite(); }
					virtual GraphicsLoader* getLoader(void) const { return m_dataBuffer.getLoader();}
					virtual ~RwStructuredBufferImpl(void){	finalize();	}
					int finalize(void){ return m_dataBuffer.finalize(); }
					
					ID3D11UnorderedAccessView *getD3D11UnorderedAccessView(void) const { return m_uav; }

					Buffer *getBaseBuffer(void) { return &m_dataBuffer; }
					BufferDimension getDimension(void) const { return m_dataBuffer.getDimension(); }
					std::string getResourceType(void) const
					{
						return "Graphics::RwStructuredBuffer";
					}

				private:
					BufferImpl m_dataBuffer;
					ID3D11UnorderedAccessView *m_uav;
				};

				


				class SamplerState
				{
				private:
					D3D11_SAMPLER_DESC m_samplerDesc;
					ID3D11SamplerState *m_samplerState;
					GraphicsLoaderImpl *m_loader;
				public:
					SamplerState(void);

					int initialize(GraphicsLoaderImpl *loader);

					int setFilter(TextureFilter minFilter,
								  TextureFilter magFilter,
								  TextureMipFilter mipFilter);

					int setAddrMode(TextureAddrMode uAddrMode, 
									TextureAddrMode vAddrMode);
					
					ID3D11SamplerState *getID3D11SamplerState(void) const;

					~SamplerState(void);
				};


				class TextureImpl : public Texture, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					Texture2dBufferImpl *m_textureBuffer;
					ID3D11ShaderResourceView *m_srv;

					SamplerState m_samplerState;
					
					int initSrv(DXGI_FORMAT srvFormat);
					bool m_freeTextureBuffer;
					
					int initializeFromDx11Texture(GraphicsLoaderImpl *loader, ID3D11Texture2D *texture);
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
								   uint32_t mipCount,
								   BufferAccessMode bufferAccessMode,
								   uint32_t bindFlag// Texture2dBufferImpl::BindFlag 
								   ); 


					int initializeFromExistentTextureBuffer(GraphicsLoaderImpl *loader, Texture2dBufferImpl *textureBuffer, 
															DXGI_FORMAT shaderResourceViewFormat=DXGI_FORMAT_UNKNOWN);
					
					uint32_t getWidth(void) const;

					uint32_t getHeight(void) const;

					uint32_t getStride(void) const;

					int setFilter(
						TextureFilter minFilter,
						TextureFilter magFilter,
						TextureMipFilter mipFilter);

					int setAddrMode(TextureAddrMode uAddrMode, TextureAddrMode vAddrMode);

					int finalize(void);



					ID3D11ShaderResourceView *getD3D11ShaderResourceView(void) const
					{
						return m_srv;
					}

					ID3D11SamplerState *getD3D11SamplerState(void) const
					{
						return m_samplerState.getID3D11SamplerState();
					}

					void *beginWrite(void)        { return m_textureBuffer->beginWrite(); }
					void endWrite(void)           { m_textureBuffer->endWrite(); }
					GraphicsLoader* getLoader(void) const { return m_textureBuffer->getLoader();}
					BufferDimension getDimension(void) const { return m_textureBuffer->getDimension(); }

					std::string getResourceType(void) const
					{
						return "Graphics::Texture";
					}

				};

				class RenderTargetImpl : public RenderTarget, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					TextureImpl m_texture;
					ID3D11RenderTargetView *pRenderTargetView;
					uint32_t m_width;
					uint32_t m_height;
					bool m_freeOnFinalize;
				public:
					RenderTargetImpl(void)
					{
						m_freeOnFinalize = false;
					}

					~RenderTargetImpl(void)
					{
						finalize();
					}

					int initialize(GraphicsLoaderImpl *loader, 
								   uint32_t width, 
								   uint32_t height, 
								   TextureFormat format=kTextureFormatA8B8G8R8Unorm);

					int initializeFromTextureBuffer(GraphicsLoaderImpl *loader, Texture2dBufferImpl* textureBuffer);

					int finalize(void);
					Texture* getTexture(void)
					{
						return &m_texture;
					}

					virtual uint32_t getWidth(void) const
					{
						return m_width;
					}

					virtual uint32_t getHeight(void) const
					{
						return m_height;
					}

					ID3D11RenderTargetView *getRenderTargetView(void) const
					{
						return pRenderTargetView;
					}
					std::string getResourceType(void) const
					{
						return "Graphics::RenderTarget";
					}


				};

				class DepthStencilSurfaceImpl : public DepthStencilSurface, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					ID3D11DepthStencilView *pDepthStencilView;
					Texture2dBufferImpl m_textureBuffer;
					TextureImpl m_depthTexture;
					TextureImpl m_stencilTexture;
				public:

					DepthStencilSurfaceImpl(void)
					{
						pDepthStencilView = NULL;
					}

					~DepthStencilSurfaceImpl(void)
					{
						finalize();
					}

					int initialize(GraphicsLoaderImpl *loader, 
								   uint32_t width, 
								   uint32_t height, 
								   DXGI_FORMAT format,
								   MultisampleMode multiSampleMode);

					int finalize(void);

					TextureImpl *getTexture(void)
					{
						return &m_depthTexture;
					}
					ID3D11DepthStencilView *getDepthStencilView(void) const 
					{
						return pDepthStencilView;
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


#endif // _SCE_TARGET_OS_WINDOWS
#endif // _SCE_SAMPLE_UTIL_GRAPHICS_BUFFER_INTERNAL_DX11_H
