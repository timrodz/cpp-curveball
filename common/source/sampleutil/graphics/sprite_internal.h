/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_SPRITE_INTERNAL_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_SPRITE_INTERNAL_H
#include <sampleutil_internal.h>
#include <vectormath.h>

#include "resource/resource_manager.h"


namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Internal
			{
				struct TexturedVertex
				{
					sce::Vectormath::Simd::Aos::Vector2 position;
					sce::Vectormath::Simd::Aos::Vector2 texcoord;
				};

				struct _DrawingUtilResource
				{
					GraphicsLoader  *m_graphicsLoader;
					Program* m_sprite2dVId;
					Program* m_sprite2dFId;
					Program* m_sprite2d2VId;
					Program* m_sprite2d2FId;
					Program* m_sprite2dYuy2VId;
					Program* m_sprite2dYuy2FId;
#if _SCE_TARGET_OS_ORBIS
					Program* m_sprite2dYcbcrVId;
					Program* m_sprite2dYcbcrFId;
#endif // _SCE_TARGET_OS_ORBIS
					Program* m_fill2dVId;
					Program* m_fill2dFId;
					Program* m_pointSprite2dVId;
					Program* m_pointSprite2dFId;

					_DrawingUtilResource(void)
					{
						m_graphicsLoader = NULL;
						m_sprite2dVId = NULL;
						m_sprite2dFId = NULL;
						m_sprite2d2VId = NULL;
						m_sprite2d2FId = NULL;
						m_sprite2dYuy2VId = NULL;
						m_sprite2dYuy2FId = NULL;
#if _SCE_TARGET_OS_ORBIS
						m_sprite2dYcbcrVId = NULL;
						m_sprite2dYcbcrFId = NULL;
#endif // _SCE_TARGET_OS_ORBIS
						m_fill2dVId = NULL;
						m_fill2dFId = NULL;
						m_pointSprite2dVId = NULL;
						m_pointSprite2dFId = NULL;
					}
					~_DrawingUtilResource(void)
					{
						finalize();
					}

					int initialize(GraphicsLoader  *graphicsLoader);

					int finalize(void);
				};



				struct _FillEffect : public Effect::EffectInstance
				{
					Effect::EffectData m_effect;
					GraphicsLoader *m_graphicsLoader;

					Effect::EffectParameter *m_offsetParam;
					Effect::EffectParameter *m_sizeParam;
					Effect::EffectParameter *m_depthParam;
					Effect::EffectParameter *m_color;

					VertexBuffer *m_rectVertices;
					IndexBuffer *m_fillRectIndices;
					IndexBuffer *m_drawRectIndices;

					VertexBuffer *m_ovalVertices;
					IndexBuffer *m_fillOvalIndices;
					IndexBuffer *m_drawOvalIndices;

					VertexBuffer *m_lineVertices;
					IndexBuffer *m_lineIndices;


					_FillEffect(void)
					{
						m_graphicsLoader = NULL;
						m_rectVertices = NULL;
						m_fillRectIndices = NULL;
						m_drawRectIndices = NULL;

						m_ovalVertices = NULL;
						m_fillOvalIndices = NULL;
						m_drawOvalIndices = NULL;

						m_lineVertices = NULL;
						m_lineIndices = NULL;

					}
					~_FillEffect(void)
					{
						finalize();
					}

					void initialize(
						GraphicsLoader *graphicsLoader, 
						const _DrawingUtilResource *drawingUtilResource);
					void finalize(void);

					void setUp(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	offset,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						float depth,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba,
						VertexBuffer **vertexBuffer, uint32_t numVertexBuffers);

					void drawRect(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	offset,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						float depth,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba,
						uint32_t width = 1);

					void fillRect(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	offset,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						float depth,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba);

					void drawOval(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	offset,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						float depth,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba,
						uint32_t width = 1);

					void fillOval(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	offset,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						float depth,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba);

					void drawLine(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg begin,
						sce::Vectormath::Simd::Aos::Vector2_arg end,
						float depth,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba,
						uint32_t width = 1);

				};

				struct _Sprite2dEffect : public Effect::EffectInstance
				{
					Effect::EffectData m_effect;
					GraphicsLoader *m_graphicsLoader;

					Effect::EffectParameter* m_displayOffset;
					Effect::EffectParameter* m_sizeInDisplay; 
					Effect::EffectParameter* m_textureOffset;
					Effect::EffectParameter* m_sizeInTexture;

					Effect::EffectParameter* m_colorCoeff;
					Effect::EffectParameter* m_tex;

					Effect::EffectParameter* m_depthParam;

					VertexBuffer *m_rectVertices;
					IndexBuffer *m_rectIndices;

					_Sprite2dEffect(void)
					{
						m_graphicsLoader = NULL;
						m_rectVertices = NULL;
						m_rectIndices = NULL;
					}
					~_Sprite2dEffect(void)
					{
						finalize();
					}

					void initialize(GraphicsLoader *graphicsLoader, 
						const _DrawingUtilResource *drawingUtilResource);
					void finalize(void);

					void draw(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg displayOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInDisplay,
						float depth,
						sce::Vectormath::Simd::Aos::Vector2_arg	textureOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInTexture,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
						Texture *texture);

					void drawWithVertexBuffer(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg   displayOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInDisplay,
						float depth,
						sce::SampleUtil::Graphics::VertexBuffer *vBuffers[],
						sce::SampleUtil::Graphics::IndexBuffer  *iBuffer,
						const int nIndices, 
						sce::Vectormath::Simd::Aos::Vector4_arg   colorCoeff,
						Texture *texture);
				};


				struct _Sprite2dEffect2 : public Effect::EffectInstance
				{
					Effect::EffectData m_effect;
					GraphicsLoader *m_graphicsLoader;

					Effect::EffectParameter* m_colorCoeff;
					Effect::EffectParameter* m_tex;
					Effect::EffectParameter* m_depthParam;

					_Sprite2dEffect2(void)
					{
						m_graphicsLoader = NULL;
						m_colorCoeff = NULL;
						m_tex = NULL;
						m_depthParam = NULL;
					}
					~_Sprite2dEffect2(void)
					{
						finalize();
					}

					void initialize(GraphicsLoader *graphicsLoader, const _DrawingUtilResource *drawingUtilResource);
					void finalize(void);

					struct Vertex
					{
						sce::Vectormath::Simd::Aos::Vector2 position;
						sce::Vectormath::Simd::Aos::Vector2 texcoord;
					};

					void draw(GraphicsContext *context,
						float depth,
						sce::Vectormath::Simd::Aos::Vector4_arg   colorCoeff,
						Texture *texture,
						VertexBuffer *vertexBuffer,
						IndexBuffer *indexBuffer, 
						uint16_t numIndices);
				};


				struct _Sprite2dYuy2Effect : public Effect::EffectInstance
				{
					Effect::EffectData m_effect;
					GraphicsLoader *m_graphicsLoader;

					Effect::EffectParameter* m_displayOffset;
					Effect::EffectParameter* m_sizeInDisplay; 
					Effect::EffectParameter* m_textureOffset;
					Effect::EffectParameter* m_sizeInTexture;

					Effect::EffectParameter* m_colorCoeff;
					Effect::EffectParameter* m_texSize;
					Effect::EffectParameter* m_tex;

					Effect::EffectParameter* m_depthParam;

					VertexBuffer *m_rectVertices;
					IndexBuffer *m_rectIndices;

					_Sprite2dYuy2Effect(void)
					{
						m_graphicsLoader = NULL;
						m_rectVertices = NULL;
						m_rectIndices = NULL;
					}
					~_Sprite2dYuy2Effect(void)
					{
						finalize();
					}

					void initialize(GraphicsLoader *graphicsLoader, 
						const _DrawingUtilResource *drawingUtilResource);
					void finalize(void);

					void draw(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg displayOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInDisplay,
						float depth,
						sce::Vectormath::Simd::Aos::Vector2_arg	textureOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInTexture,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
						Texture *texture);
				};


#if _SCE_TARGET_OS_ORBIS
				struct _Sprite2dYcbcrEffect : public Effect::EffectInstance
				{
					Effect::EffectData m_effect;
					GraphicsLoader *m_graphicsLoader;

					Effect::EffectParameter* m_displayOffset;
					Effect::EffectParameter* m_sizeInDisplay; 
					Effect::EffectParameter* m_textureOffset;
					Effect::EffectParameter* m_sizeInTexture;
					Effect::EffectParameter* m_depthParam;

					Effect::EffectParameter* m_yTex;
					Effect::EffectParameter* m_cbcrTex;

					VertexBuffer *m_rectVertices;
					IndexBuffer *m_rectIndices;

					_Sprite2dYcbcrEffect(void)
					{
						m_graphicsLoader = NULL;
						m_rectVertices = NULL;
						m_rectIndices = NULL;
					}
					~_Sprite2dYcbcrEffect(void)
					{
						finalize();
					}

					void initialize(GraphicsLoader *graphicsLoader, 
						const _DrawingUtilResource *drawingUtilResource);
					void finalize(void);

					void draw(GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg displayOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInDisplay,
						float depth,
						sce::Vectormath::Simd::Aos::Vector2_arg	textureOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInTexture,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
						Texture *yTexture, Texture *cbcrTexture);
				};
#endif // _SCE_TARGET_OS_ORBIS


				struct _PointSprite2dEffect : public Effect::EffectInstance
				{
					Effect::EffectData m_effect;
					GraphicsLoader *m_graphicsLoader;

					Effect::EffectParameter *m_psize;
					Effect::EffectParameter *m_colorCoeff;
					Effect::EffectParameter *m_tex;
					Effect::EffectParameter *m_depth;

					_PointSprite2dEffect(void){}
					~_PointSprite2dEffect(void){}

					void initialize(GraphicsLoader *graphicsLoader,
						const _DrawingUtilResource *drawingUtilResource);
					void finalize(void);

					void draw(GraphicsContext *context,
						float psize,
						float depth,
						sce::Vectormath::Simd::Aos::Vector4_arg   colorCoeff,
						Texture *texture,
						VertexBuffer *vertices,
						IndexBuffer *indices, 
						uint32_t numIndices

						);
				};

				struct Object3dRendererResource
				{
					GraphicsLoader  *m_graphicsLoader;
					Program* m_fill3dVId;
					Program* m_fill3dFId;
					Program* m_wireframeVId;
					Program* m_wireframeFId;

					Object3dRendererResource(void)
					{
						m_graphicsLoader = NULL;
						m_fill3dVId = NULL;
						m_fill3dFId = NULL;
						m_wireframeVId = NULL;
						m_wireframeFId = NULL;
					}
					~Object3dRendererResource(void)
					{
						finalize();
					}

					int initialize(GraphicsLoader  *graphicsLoader);

					int finalize(void);
				};

				struct Fill3dEffect: public Effect::EffectInstance
				{
					struct Vertex
					{
						sce::Vectormath::Simd::Aos::Vector3 position;
						sce::Vectormath::Simd::Aos::Vector3 normal;
						Vertex(void){}
						Vertex(sce::Vectormath::Simd::Aos::Vector3_arg p,
							sce::Vectormath::Simd::Aos::Vector3_arg n)
							: position(p), normal(n)
						{  }
					};
					Effect::EffectData m_effect;
					GraphicsLoader *m_graphicsLoader;

	                Effect::EffectParameter *m_worldInverseTranspose;
					Effect::EffectParameter *m_worldViewProjection;
					Effect::EffectParameter *m_world;
					Effect::EffectParameter *m_viewInverse;
					Effect::EffectParameter *m_lightPosition;
					Effect::EffectParameter *m_color;
					Effect::EffectParameter *m_ambient;
					Effect::EffectParameter *m_shininess;
					
					VertexBuffer *m_cubeVertices;
					IndexBuffer *m_cubeIndices;

					VertexBuffer *m_sphereVertices;
					IndexBuffer *m_sphereIndices;
					int m_numSphereIndices;

					VertexBuffer *m_cylinderVertices;
					IndexBuffer *m_cylinderIndices;
					int m_numCylinderIndices;

					int m_sphereSubdiv;
					int m_sphereSubdiv2;
					int m_cylinderSubdiv;

					Fill3dEffect(void)
					{
						m_graphicsLoader        = NULL;
						
						m_worldInverseTranspose = NULL;
						m_worldViewProjection   = NULL;
						m_world                 = NULL;
						m_viewInverse           = NULL;
						m_lightPosition         = NULL;
						m_color                 = NULL;
						m_ambient               = NULL;
						m_shininess             = NULL;
						
						m_cubeVertices          = NULL;
						m_cubeIndices           = NULL;

						m_sphereSubdiv   = 12;
						m_sphereSubdiv2  =  6;
						m_cylinderSubdiv = 12;

					}
					~Fill3dEffect(void)
					{
						finalize();
					}

					void initialize(GraphicsLoader *graphicsLoader,
						const Object3dRendererResource *resource);
					void finalize(void);

					void setUp(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector3 lightPosition,
						sce::Vectormath::Simd::Aos::Vector4 color,
						float ambient,
						float shininess,
						VertexBuffer *vertexBuffer);

					void drawCube(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector3 lightPosition,
						sce::Vectormath::Simd::Aos::Vector4 color,
						float ambient,
						float shininess);

					void drawSphere(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector3 lightPosition,
						sce::Vectormath::Simd::Aos::Vector4 color,
						float ambient,
						float shininess);

					void drawCylinder(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector3 lightPosition,
						sce::Vectormath::Simd::Aos::Vector4 color,
						float ambient,
						float shininess);

				private:
					static void createCubeVertex(
						VertexBuffer **outVertices,
						IndexBuffer **outIndices,
						GraphicsLoader *graphicsLoader);
					static void createSphereVertex(
						VertexBuffer **outVertices,
						IndexBuffer **outIndices,
						int *outNumSphereIndices,
						GraphicsLoader *graphicsLoader,
						int longitudeSubdiv,
						int latitudeSubdiv);
					static void createCylinderVertex(
						VertexBuffer **outVertices,
						IndexBuffer **outIndices,
						int *outNumCylinderIndices,
						GraphicsLoader *graphicsLoader,
						int cylinderSubdiv);
				};

				struct WireframeEffect: public Effect::EffectInstance
				{
					Effect::EffectData m_effect;
					GraphicsLoader *m_graphicsLoader;

					Effect::EffectParameter *m_worldViewProjection;
					Effect::EffectParameter *m_color;
					
					VertexBuffer *m_lineVertices;
					IndexBuffer *m_lineIndices;

					VertexBuffer *m_cubeVertices;
					IndexBuffer *m_cubeIndices;

					VertexBuffer *m_sphereVertices;
					IndexBuffer *m_sphereIndices;

					VertexBuffer *m_cylinderVertices;
					IndexBuffer *m_cylinderIndices;
					
					int m_sphereSubdiv;
					int m_sphereSubdiv2;
					int m_cylinderSubdiv;

					WireframeEffect(void)
					{
						m_graphicsLoader = NULL;
						
						m_worldViewProjection = NULL;
						m_color = NULL;

						m_lineVertices = NULL;
						m_lineIndices = NULL;

						m_cubeVertices = NULL;
						m_cubeIndices = NULL;

						m_sphereVertices = NULL;
						m_sphereIndices = NULL;

						m_cylinderVertices = NULL;
						m_cylinderIndices = NULL;

						m_sphereSubdiv   = 12;
						m_sphereSubdiv2  =  6;
						m_cylinderSubdiv = 12;
					}
					~WireframeEffect(void)
					{
						finalize();
					}

					void initialize(GraphicsLoader *graphicsLoader,
						const Object3dRendererResource *resource);
					void finalize(void);

					void setUp(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector4 color,
						VertexBuffer *vertexBuffer);

					void drawLine(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector3 begin,
						sce::Vectormath::Simd::Aos::Vector3 end,
						sce::Vectormath::Simd::Aos::Vector4 color,
						uint32_t lineWidth
						);
						
					void drawCube(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector4 color,
						uint32_t lineWidth
						);

					void drawSphere(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector4 color,
						uint32_t lineWidth
						);

					void drawCylinder(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector4 color,
						uint32_t lineWidth
						);
				private:
					static void createCubeVertex(
						VertexBuffer **outVertices,
						IndexBuffer **outIndices,
						GraphicsLoader *graphicsLoader);

					static void createSphereVertex(
						VertexBuffer **outVertices,
						IndexBuffer **outIndices,
						GraphicsLoader *graphicsLoader,
						int sphereSubdiv,
						int sphereSubdiv2);

					static void createCylinderVertex(
						VertexBuffer **outVertices,
						IndexBuffer **outIndices,
						GraphicsLoader *graphicsLoader,
						int cylinderSubdiv);

				};

			}

			namespace Impl
			{
				class  SpriteRendererImpl : public SpriteRenderer, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					SpriteRendererImpl(void)
					{
						m_asciiTexture = NULL;
						m_stringBuffer = NULL;
					}
					~SpriteRendererImpl(void)
					{
						finalize();
					}

					Internal::_DrawingUtilResource m_drawingUtilResource;

					Internal::_FillEffect m_fillEffect;
					Internal::_Sprite2dEffect m_sprite2dEffect;
					Internal::_Sprite2dEffect2 m_sprite2dEffect2;
					Internal::_Sprite2dYuy2Effect m_sprite2dYuy2Effect;
#if _SCE_TARGET_OS_ORBIS
					Internal::_Sprite2dYcbcrEffect m_sprite2dYcbcrEffect;
#endif // _SCE_TARGET_OS_ORBIS
					Internal::_PointSprite2dEffect m_pointSprite2dEffect;
					sce::Vectormath::Simd::Aos::Vector2 m_renderTargetSize;

				private:
					sce::SampleUtil::Graphics::Texture *m_asciiTexture;
					sce::Vectormath::Simd::Aos::Vector2 m_asciiTextureSize;

					char *m_stringBuffer;
					uint32_t m_stringBufferSize;

				public:
					int initialize(GraphicsLoader *graphicsLoader);
					void setRenderTargetSize( sce::Vectormath::Simd::Aos::Vector2_arg renderTargetSize );
					int finalize(void);

					int fillRect(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f);

					int drawRect(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f,
						uint32_t width = 1);

					int fillOval(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f);

					int drawOval(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f,
						uint32_t width = 1);
					int drawLine(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	begin,
						sce::Vectormath::Simd::Aos::Vector2_arg	end,
						sce::Vectormath::Simd::Aos::Vector4_arg rgba = sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f,
						uint32_t width = 1);

					int drawTexture(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						Texture *texture,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f);

					int drawTexture(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						sce::Vectormath::Simd::Aos::Vector2_arg	textureOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInTexture,
						Texture *texture, 
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f
						);

					int drawTextureYuy2(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						Texture *texture,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f);

					int drawTextureYuy2(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						sce::Vectormath::Simd::Aos::Vector2_arg	textureOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInTexture,
						Texture *texture, 
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f
						);

#if _SCE_TARGET_OS_ORBIS
					int drawTextureYcbcr(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						Texture *yTexture,
						Texture *cbcrTexture,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f);

					int drawTextureYcbcr(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg	position,
						sce::Vectormath::Simd::Aos::Vector2_arg	size,
						sce::Vectormath::Simd::Aos::Vector2_arg	textureOffset,
						sce::Vectormath::Simd::Aos::Vector2_arg	sizeInTexture,
						Texture *yTexture,
						Texture *cbcrTexture,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f
						);
#endif // _SCE_TARGET_OS_ORBIS

					int drawPoints(
						GraphicsContext *context,
						float psize,
						Texture *texture,
						VertexBuffer *vertices,
						IndexBuffer *indices, 
						uint32_t numIndices,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff=sce::Vectormath::Simd::Aos::Vector4(1.0f),
						float depth=0.0f
						);


					int drawString(
						GraphicsContext *context,
						Font *font,
						const uint16_t* ucs2Charcode,
						sce::Vectormath::Simd::Aos::Vector2_arg position,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
						float depth=0.0f,
						float scale=1.0f
						);
#if _SCE_TARGET_OS_ORBIS
					int drawString(
						GraphicsContext *context,
						Font *font,
						const char16_t* ucs2Charcode,
						sce::Vectormath::Simd::Aos::Vector2_arg position,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
						float depth=0.0f,
						float scale=1.0f
						);
#endif					


					sce::Vectormath::Simd::Aos::Vector2 getStringTextureSize(
						Font *font,
						const uint16_t* ucs2Charcode,
						float scale=1.0f
						);

#if _SCE_TARGET_OS_ORBIS
					float getAlphanumericCharactersHeight(
						Font *font,
						float scale=1.0f
						);

					float getAlphanumericCharactersBaselineOffset(
						Font *font,
						float scale=1.0f
						);
#endif					

					int drawDebugString(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg position,
						float fontHeight,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
						sce::Vectormath::Simd::Aos::Vector4_arg backgroundColor,
						float depth,
						const char *string);

					int drawDebugString(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg position,
						float fontHeight,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
						const char *string);

					int drawDebugStringf(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg position,
						float fontHeight,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
						sce::Vectormath::Simd::Aos::Vector4_arg backgroundColor,
						float depth,
						const char *format, ... );

					int drawDebugStringf(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Vector2_arg position,
						float fontHeight,
						sce::Vectormath::Simd::Aos::Vector4_arg colorCoeff,
						const char *format, ... );

					float getWidthOfDebugChar(float charHeight);

					std::string getResourceType(void) const
					{
						return "Graphics::SpriteRenderer";
					}
                };

				class Object3dRendererImpl: public Object3dRenderer, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					Internal::Object3dRendererResource m_resource;
					Internal::Fill3dEffect    m_fill3dEffect;
					Internal::WireframeEffect m_wireframeEffect;
				public:
					Object3dRendererImpl(void)
					{

					}

					~Object3dRendererImpl(void)
					{
						finalize();
					}

					int initialize(GraphicsLoader *graphicsLoader);
					int finalize(void);

					int fillCube(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector3 lightPosition,
						sce::Vectormath::Simd::Aos::Vector4 color,
						float ambient,
						float shininess);
					
					int drawLine(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector3 begin,
						sce::Vectormath::Simd::Aos::Vector3 end,
						sce::Vectormath::Simd::Aos::Vector4 color,
						uint32_t lineWidth);
						
					int drawCube(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector4 color,
						uint32_t lineWidth);

					int drawSphere(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector4 color,
						uint32_t lineWidth);

					int fillSphere(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector3 lightPosition,				  
						sce::Vectormath::Simd::Aos::Vector4 color,
						float ambient,
						float shininess);

					int drawCylinder(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector4 color,
						uint32_t lineWidth);

					int fillCylinder(
						GraphicsContext *context,
						sce::Vectormath::Simd::Aos::Matrix4_arg world,
						sce::Vectormath::Simd::Aos::Matrix4_arg view,
						sce::Vectormath::Simd::Aos::Matrix4_arg projection,
						sce::Vectormath::Simd::Aos::Vector3 lightPosition,				  
						sce::Vectormath::Simd::Aos::Vector4 color,
						float ambient,
						float shininess);

					std::string getResourceType(void) const
					{
						return "Graphics::SpriteRenderer";
					}

				};
			}
		}
	}
}

namespace ssi = sce::SampleUtil::Internal;
namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace vm = sce::Vectormath::Simd::Aos;

#endif // _SCE_SAMPLE_UTIL_GRAPHICS_SPRITE_INTERNAL_H