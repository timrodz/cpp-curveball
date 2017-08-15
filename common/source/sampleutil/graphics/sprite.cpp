/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"

#include <sampleutil_internal.h>
#include <vector>
#include <sampleutil/graphics/sprite.h>
#include <sampleutil/graphics/platform.h>
#include <sampleutil/sampleutil_common.h>
#include "graphics_internal.h"
#include "font_internal.h"

#include "resource/resource_manager.h"
#include "sprite_internal.h"




#pragma region SpriteRendererImpl

int ssgi::SpriteRendererImpl::initialize(ssg::GraphicsLoader *graphicsLoader)
{
	int ret;
	if(graphicsLoader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ret = m_drawingUtilResource.initialize(graphicsLoader);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_fillEffect.initialize(graphicsLoader, &m_drawingUtilResource);
	m_sprite2dEffect.initialize(graphicsLoader, &m_drawingUtilResource);
	m_sprite2dEffect2.initialize(graphicsLoader, &m_drawingUtilResource);
	m_sprite2dYuy2Effect.initialize(graphicsLoader, &m_drawingUtilResource);
#if _SCE_TARGET_OS_ORBIS
	m_sprite2dYcbcrEffect.initialize(graphicsLoader, &m_drawingUtilResource);
#endif // _SCE_TARGET_OS_ORBIS
	m_pointSprite2dEffect.initialize(graphicsLoader, &m_drawingUtilResource);


	m_asciiTexture = ssi::Resource::loadTexture(graphicsLoader, ssi::Resource::ASCII);
	SCE_SAMPLE_UTIL_ASSERT(m_asciiTexture != NULL);
	m_asciiTexture->setFilter(ssg::kTextureFilterPoint, ssg::kTextureFilterPoint, ssg::kTextureMipFilterDisabled);

	m_asciiTextureSize.setX((float)m_asciiTexture->getWidth());
	m_asciiTextureSize.setY((float)m_asciiTexture->getHeight());

	// TODO
	m_renderTargetSize = sce::Vectormath::Simd::Aos::Vector2(1920, 1080);

	m_stringBufferSize = 1024;
	m_stringBuffer = (char*)malloc(m_stringBufferSize);

	(void)ret;
	return SCE_OK;
}

void ssgi::SpriteRendererImpl::setRenderTargetSize( vm::Vector2_arg renderTargetSize )
{
	if((renderTargetSize.getX() <= 0.0f) || (renderTargetSize.getY() <= 0.0f)){
		return;
	}
	m_renderTargetSize = renderTargetSize;
}


int ssgi::SpriteRendererImpl::finalize(void)
{


	m_fillEffect.finalize();
	m_sprite2dYuy2Effect.finalize();
#if _SCE_TARGET_OS_ORBIS
	m_sprite2dYcbcrEffect.finalize();
#endif // _SCE_TARGET_OS_ORBIS
	m_sprite2dEffect2.finalize();
	m_sprite2dEffect.finalize();
	m_pointSprite2dEffect.finalize();
	m_drawingUtilResource.finalize();

	if(m_asciiTexture != NULL){
		sce::SampleUtil::destroy(m_asciiTexture);
		m_asciiTexture = NULL;
	}
	if(m_stringBuffer != NULL){
		free(m_stringBuffer);
		m_stringBuffer = NULL;
	}

	return SCE_OK;
}

int ssgi::SpriteRendererImpl::fillRect(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	vm::Vector4_arg rgba,
	float depth)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_fillEffect.fillRect(context,
		position, size, depth, rgba);
	return SCE_OK;
}

int ssgi::SpriteRendererImpl::drawRect(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	vm::Vector4_arg rgba,
	float depth, uint32_t width)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_fillEffect.drawRect(context,
		position, size, depth, rgba, width);
	return SCE_OK;
}

int ssgi::SpriteRendererImpl::fillOval(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	vm::Vector4_arg rgba,
	float depth)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_fillEffect.fillOval(context,
		position, size, depth, rgba);
	return SCE_OK;
}

int ssgi::SpriteRendererImpl::drawOval(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	vm::Vector4_arg rgba,
	float depth,
	uint32_t width)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_fillEffect.drawOval(context,
		position, size, depth, rgba, width);
	return SCE_OK;
}

int ssgi::SpriteRendererImpl::drawLine(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	begin,
	vm::Vector2_arg	end,
	vm::Vector4_arg rgba,
	float depth,
	uint32_t width)
{
	if( context == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	m_fillEffect.drawLine(context,
		begin, end, depth, rgba, width);

	return SCE_OK;
}



int ssgi::SpriteRendererImpl::drawTexture(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	ssg::Texture *texture,
	vm::Vector4_arg colorCoeff,
	float depth )
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(texture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}


	m_sprite2dEffect.draw(context,
		position,
		size,
		depth,
		vm::Vector2(0.0f),
		vm::Vector2(1.0f),
		colorCoeff,
		texture);
	return SCE_OK;
}

int ssgi::SpriteRendererImpl::drawTexture(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	vm::Vector2_arg	textureOffset,
	vm::Vector2_arg	sizeInTexture,
	ssg::Texture *texture,
	vm::Vector4_arg colorCoeff,
	float depth
	)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(texture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_sprite2dEffect.draw(context,
		position,
		size,
		depth,
		textureOffset,
		sizeInTexture,
		colorCoeff,
		texture);
	return SCE_OK;
}

int ssgi::SpriteRendererImpl::drawTextureYuy2(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	ssg::Texture *texture,
	vm::Vector4_arg colorCoeff,
	float depth )
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(texture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}


	m_sprite2dYuy2Effect.draw(context,
		position,
		size,
		depth,
		vm::Vector2(0.0f),
		vm::Vector2(1.0f),
		colorCoeff,
		texture);
	return SCE_OK;
}

int ssgi::SpriteRendererImpl::drawTextureYuy2(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	vm::Vector2_arg	textureOffset,
	vm::Vector2_arg	sizeInTexture,
	ssg::Texture *texture,
	vm::Vector4_arg colorCoeff,
	float depth
	)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(texture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_sprite2dYuy2Effect.draw(context,
		position,
		size,
		depth,
		textureOffset,
		sizeInTexture,
		colorCoeff,
		texture);
	return SCE_OK;
}

#if _SCE_TARGET_OS_ORBIS
int ssgi::SpriteRendererImpl::drawTextureYcbcr(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	ssg::Texture *yTexture,
	ssg::Texture *cbcrTexture,
	vm::Vector4_arg colorCoeff,
	float depth )
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(yTexture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(cbcrTexture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_sprite2dYcbcrEffect.draw(context,
		position,
		size,
		depth,
		vm::Vector2(0.0f),
		vm::Vector2(1.0f),
		colorCoeff,
		yTexture, cbcrTexture);
	return SCE_OK;
}

int ssgi::SpriteRendererImpl::drawTextureYcbcr(
	ssg::GraphicsContext *context,
	vm::Vector2_arg	position,
	vm::Vector2_arg	size,
	vm::Vector2_arg	textureOffset,
	vm::Vector2_arg	sizeInTexture,
	ssg::Texture *yTexture,
	ssg::Texture *cbcrTexture,
	vm::Vector4_arg colorCoeff,
	float depth
	)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(yTexture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(cbcrTexture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	m_sprite2dYcbcrEffect.draw(context,
		position,
		size,
		depth,
		textureOffset,
		sizeInTexture,
		colorCoeff,
		yTexture, cbcrTexture);
	return SCE_OK;
}
#endif // _SCE_TARGET_OS_ORBIS


int ssgi::SpriteRendererImpl::drawPoints(
	ssg::GraphicsContext *context,
	float psize,
	ssg::Texture *texture,
	ssg::VertexBuffer *vertices,
	ssg::IndexBuffer *indices,
	uint32_t numIndices,
	vm::Vector4_arg colorCoeff,
	float depth
	)
{
	if( context == NULL || texture == NULL || vertices == NULL || indices == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	m_pointSprite2dEffect.draw(context, 
		psize, depth, colorCoeff, texture, vertices, indices, numIndices);

	return SCE_OK;
}





static inline uint32_t ucs2stringLen(const uint16_t* ucs2Charcode)
{
	uint32_t len = 0;
	while( *ucs2Charcode ){
		len++;
		ucs2Charcode++;
	}
	return len;
}

#if _SCE_TARGET_OS_ORBIS
int ssgi::SpriteRendererImpl::drawString(
	ssg::GraphicsContext *context,
	ssg::Font *font,
	const char16_t* ucs2Charcode,
	vm::Vector2_arg position,
	vm::Vector4_arg colorCoeff,
	float depth,
	float scale
	)
{
	return drawString(
		context,
		font,
		(const uint16_t*)ucs2Charcode,
		position,
		colorCoeff,
		depth,
		scale
	);
}
#endif

int ssgi::SpriteRendererImpl::drawString(
	ssg::GraphicsContext *context,
	ssg::Font *font,
	const uint16_t* ucs2Charcode,
	vm::Vector2_arg position,
	vm::Vector4_arg colorCoeff,
	float depth,
	float scale
	)
{
	if( context == NULL || font == NULL || ucs2Charcode == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssg::RenderTarget *rt = context->getCurrentRenderTarget();
	vm::Vector2 rtSize(rt->getWidth(), rt->getHeight());


	FontImpl2 *fontImpl = dynamic_cast<FontImpl2*>(font);
	ssg::Texture *texture = fontImpl->getTexture();

	uint32_t length = ucs2stringLen(ucs2Charcode);
	if(length == 0){
		return SCE_OK;
	}

	VertexBuffer *vertexBuffer = NULL;
	IndexBuffer *indexBuffer = NULL;
	int ret = context->createVertexBuffer(&vertexBuffer, sizeof(Internal::_Sprite2dEffect2::Vertex) * 4 * length);
	if(ret != SCE_OK){
		return ret;
	}
	ret = context->createIndexBuffer(&indexBuffer, sizeof(uint16_t) * 6 * length);
	if(ret != SCE_OK){
		sce::SampleUtil::destroy(vertexBuffer);
		return ret;
	}
	
	uint16_t *indices = (uint16_t *)indexBuffer->beginWrite();
	Internal::_Sprite2dEffect2::Vertex *vertices = (Internal::_Sprite2dEffect2::Vertex*)vertexBuffer->beginWrite();

	uint32_t numVertices = 0;
	uint32_t numIndices = 0;

	vm::Vector2 posXY;
	float offsetX = 0.0f;
	const uint16_t* pChar = ucs2Charcode;
	while( *pChar ){
		uint16_t charCode = *pChar;

		FontCharInfo charInfo;
		ret = fontImpl->getCharInfo(&charInfo, charCode);
		if(ret != SCE_OK){
			break;
		}

		if( offsetX == 0.0f && charInfo.bearingX < 0.0f ){
			offsetX	-= charInfo.bearingX * scale;
		}

		posXY = position + vm::Vector2( offsetX + charInfo.bearingX * scale, 0.0f );
		if( ! charInfo.isBlank ){

			vm::Vector2 posInDisp = divPerElem( posXY, rtSize);
			vm::Vector2 sizeInDisp = divPerElem( vm::Vector2(charInfo.width * scale, fontImpl->getCharHeight() * scale), rtSize );
			vm::Vector2 posInTex = vm::Vector2(charInfo.u, charInfo.v);
			vm::Vector2 sizeInTex = divPerElem( vm::Vector2(charInfo.width,	fontImpl->getCharHeight() ), vm::Vector2(texture->getWidth(), texture->getHeight()) );
			{
				indices[numIndices+0] = numVertices+0;
				indices[numIndices+1] = numVertices+3;
				indices[numIndices+2] = numVertices+1;
				indices[numIndices+3] = numVertices+1;
				indices[numIndices+4] = numVertices+3;
				indices[numIndices+5] = numVertices+2;
			}
			{
				vertices[numVertices+0].position = posInDisp;
				vertices[numVertices+0].texcoord = posInTex;
				
				vertices[numVertices+1].position = vm::Vector2(posInDisp.getX() + sizeInDisp.getX(), posInDisp.getY()); 
				vertices[numVertices+1].texcoord = vm::Vector2(posInTex.getX() + sizeInTex.getX(), posInTex.getY());
				
				vertices[numVertices+2].position = posInDisp + sizeInDisp;
				vertices[numVertices+2].texcoord = posInTex + sizeInTex;
				
				vertices[numVertices+3].position = vm::Vector2(posInDisp.getX(), posInDisp.getY() +  + sizeInDisp.getY());
				vertices[numVertices+3].texcoord = vm::Vector2(posInTex.getX(), posInTex.getY() +  + sizeInTex.getY());

			}
			numIndices  += 6;
			numVertices += 4;
/*
			m_sprite2dEffect.draw(context, 
				posInDisp,
				sizeInDisp, 
				depth,
				posInTex, 
				sizeInTex,
				colorCoeff,
				texture);
				*/
		}

		offsetX	+= charInfo.advanceX * scale;
		pChar++;
	}


	indexBuffer->endWrite();
	vertexBuffer->endWrite();

	if(numIndices > 0){
		//printf("numIndices=%d\n", numIndices);
		m_sprite2dEffect2.draw(context,
			depth,
			colorCoeff,
			texture,
			vertexBuffer,
			indexBuffer,
			numIndices);
	}
	
	sce::SampleUtil::destroy(vertexBuffer);
	sce::SampleUtil::destroy(indexBuffer);


	//sce::SampleUtil::Impl::debugPrintfln("test draw texture\n");
/*
	drawTexture(context,
	vm::Vector2(0),
	vm::Vector2(1),texture);
	*/
	

	/*
	m_sprite2dEffect.draw(context,
		vm::Vector2(0),
		vm::Vector2(1),
		depth,
		vm::Vector2(0),
		vm::Vector2(1),
		colorCoeff,
		texture);
		*/
	return ret;
}

vm::Vector2 ssgi::SpriteRendererImpl::getStringTextureSize(
	ssg::Font *font,
	const uint16_t* ucs2Charcode,
	float scale
	)
{
	if( font == NULL || ucs2Charcode == NULL ){
		return vm::Vector2(0.0f);
	}
	FontImpl2 *fontImpl = dynamic_cast<FontImpl2*>(font);
	float width = 0.0f;
	float offsetX = 0.0f;
	const uint16_t* pChar = ucs2Charcode;
	while( *pChar ){
		uint16_t charCode = *pChar;
		//printf(__FILE__"(%d) check charCode=%c\n", __LINE__, charCode);

		FontCharInfo charInfo;
		int ret = fontImpl->getCharInfo(&charInfo, charCode);
		if(ret != SCE_OK){
			return vm::Vector2(0.0f);
		}

		if( offsetX == 0.0f && charInfo.bearingX < 0.0f ){
			offsetX	-= charInfo.bearingX;
		}

		width = offsetX + charInfo.width + charInfo.bearingX;
		offsetX	+= charInfo.advanceX;
		pChar++;
	}
	return vm::Vector2(width, fontImpl->getCharHeight()) * scale;
}

#if _SCE_TARGET_OS_ORBIS
float ssgi::SpriteRendererImpl::getAlphanumericCharactersHeight(
	ssg::Font *font,
	float scale
	)
{
	if( font == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	FontImpl2 *fontImpl = dynamic_cast<FontImpl2*>(font);
	return fontImpl->getAlphanumericCharactersHeight() * scale;
}

float ssgi::SpriteRendererImpl::getAlphanumericCharactersBaselineOffset(
	ssg::Font *font,
	float scale
	)
{
	if( font == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	FontImpl2 *fontImpl = dynamic_cast<FontImpl2*>(font);
	return fontImpl->getAlphanumericCharactersBaselineOffset() * scale;
}
#endif

int ssgi::SpriteRendererImpl::drawDebugString(
	ssg::GraphicsContext *context,
	vm::Vector2_arg position,
	float fontHeight,
	vm::Vector4_arg colorCoeff,
	vm::Vector4_arg backgroundColor,
	float depth,
	const char *string
	)
{
	if( context == NULL || string  == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssg::RenderTarget *rt = context->getCurrentRenderTarget();
	if(rt == NULL){
		return SCE_OK;
	}
	vm::Vector2 rtSize(rt->getWidth(), rt->getHeight());

	float displayRate = rtSize.getY() / rtSize.getX();

	//vm::Vector2 offset = vm::Vector2(displayRate * (fontHeight/2.0f) , 0);

	/* create string buffer */
	int length = strlen(string);

	/* bug 64083 */
	if(0 == length){
		return SCE_OK;
	}

	IndexBuffer  *rectIndex;
	VertexBuffer *rectVertices;
	vm::Vector2 fontSizeInTexture(32.0f,64.0f);

	const int vertexStep = 4;
	const int indexStep  = 6;

	m_drawingUtilResource.m_graphicsLoader->createVertexBuffer(&rectVertices, sizeof(ssg::Internal::TexturedVertex)*(4*length));
	m_drawingUtilResource.m_graphicsLoader->createIndexBuffer (&rectIndex,    sizeof(uint16_t)*6 * length);

	ssg::Internal::TexturedVertex *vertexData = (ssg::Internal::TexturedVertex*)rectVertices->beginWrite();
	uint16_t                  *indexData  = (uint16_t *)rectIndex->beginWrite();

	for(int i=0 ; i < length ; ++i){
		unsigned char c = (unsigned char)(string[i]);

		vm::Vector2 textureOffset( ((c>>0) & 0x0f)*32.0f, ((c>>4) & 0x0f)*64.0f);

		vm::Vector2 texCoord_start = divPerElem(textureOffset,     m_asciiTextureSize);
		vm::Vector2 texCoord_end   = texCoord_start + divPerElem(fontSizeInTexture, m_asciiTextureSize);

		vertexData[vertexStep*i + 0].position = vm::Vector2((float)i/(float)(length), 0.0);
		vertexData[vertexStep*i + 0].texcoord = vm::Vector2(texCoord_start.getX(), texCoord_start.getY());

		vertexData[vertexStep*i + 1].position = vm::Vector2((float)(i+1)/(float)(length), 0.0);
		vertexData[vertexStep*i + 1].texcoord = vm::Vector2(texCoord_end.getX(),   texCoord_start.getY());

		vertexData[vertexStep*i + 2].position = vm::Vector2((float)(i+1)/(float)(length), 1.0);
		vertexData[vertexStep*i + 2].texcoord = vm::Vector2(texCoord_end.getX(),   texCoord_end.getY()  );

		vertexData[vertexStep*i + 3].position = vm::Vector2((float)i/(float)(length), 1.0);
		vertexData[vertexStep*i + 3].texcoord = vm::Vector2(texCoord_start.getX(), texCoord_end.getY()  );


		indexData[indexStep*i + 0] = vertexStep*i + 0;
		indexData[indexStep*i + 1] = vertexStep*i + 3;
		indexData[indexStep*i + 2] = vertexStep*i + 1;
		indexData[indexStep*i + 3] = vertexStep*i + 1;
		indexData[indexStep*i + 4] = vertexStep*i + 3;
		indexData[indexStep*i + 5] = vertexStep*i + 2;
	}

	rectVertices->endWrite();
	rectIndex->endWrite();


	vm::Vector2 stringAreaSize(displayRate * (fontHeight/2.0f) * length, fontHeight);


	this->fillRect(
		context,
		position,
		stringAreaSize,
		backgroundColor,
		depth );

	VertexBuffer *buffers[] = { rectVertices };
	m_sprite2dEffect.drawWithVertexBuffer(
		context,
		position,
		stringAreaSize,
		depth,
		buffers,
		rectIndex, 6*length, 
		colorCoeff,
		m_asciiTexture);

	sce::SampleUtil::destroy(rectVertices);
	sce::SampleUtil::destroy(rectIndex);

	return SCE_OK;
}

int ssgi::SpriteRendererImpl::drawDebugString(
	ssg::GraphicsContext *context,
	vm::Vector2_arg position,
	float fontHeight,
	vm::Vector4_arg colorCoeff,
	const char *string
	)
{
	return this->drawDebugString(context, position, fontHeight, colorCoeff, vm::Vector4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, string);
}


int ssgi::SpriteRendererImpl::drawDebugStringf(
	ssg::GraphicsContext *context,
	vm::Vector2_arg position,
	float fontHeight,
	vm::Vector4_arg colorCoeff,
	vm::Vector4_arg backgroundColor,
	float depth,
	const char *format, ... )
{
	while(1){
		va_list ap;
		va_start(ap, format);
		int n= vsnprintf(m_stringBuffer, m_stringBufferSize, format, ap);
		va_end(ap);

		if((n>=0)&&((unsigned int)n<m_stringBufferSize)){
			drawDebugString(context, position, fontHeight, colorCoeff, backgroundColor, depth, m_stringBuffer);
			return SCE_OK;
		}

		m_stringBufferSize *= 2;
		m_stringBuffer = (char*)realloc(m_stringBuffer, m_stringBufferSize);
		if(m_stringBuffer == NULL){
			printf("Error while executing drawStringf\n");
			return SCE_SAMPLE_UTIL_ERROR_FATAL;
		}
	}
}

int ssgi::SpriteRendererImpl::drawDebugStringf(
	ssg::GraphicsContext *context,
	vm::Vector2_arg position,
	float fontHeight,
	vm::Vector4_arg colorCoeff,
	const char *format, ... )
{
	while(1){
		va_list ap;
		va_start(ap, format);
		int n= vsnprintf(m_stringBuffer, m_stringBufferSize, format, ap);
		va_end(ap);


		if((n>=0)&&(n<m_stringBufferSize)){
			drawDebugString(context, position, fontHeight, colorCoeff, vm::Vector4(0.0f, 0.0f, 0.0f, 0.0f), 1.0f, m_stringBuffer);
			return SCE_OK;
		}

		m_stringBufferSize *= 2;
		m_stringBuffer = (char*)realloc(m_stringBuffer, m_stringBufferSize);
		if(m_stringBuffer == NULL){
			printf("Error while executing drawStringf\n");
			return SCE_SAMPLE_UTIL_ERROR_FATAL;
		}
	}
}


float ssgi::SpriteRendererImpl::getWidthOfDebugChar(float charHeight)
{
	return (charHeight / 2.0f) * (m_renderTargetSize.getY() / m_renderTargetSize.getX());
}

#pragma endregion // SpriteRendererImpl



int ssg::createSpriteRenderer(SpriteRenderer* *outSpriteRenderer, ssg::GraphicsLoader *loader)
{
	if(outSpriteRenderer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	Impl::SpriteRendererImpl *s = new Impl::SpriteRendererImpl();
	int ret = s->initialize(loader);
	if(ret != SCE_OK){
		delete s;
		return ret;
	}
	*outSpriteRenderer = s;
	return ret;
}





/* -------------------------------------------------------------------------------------- */


#pragma region Fill3dEffect

void ssg::Internal::Fill3dEffect::createCubeVertex(
	ssg::VertexBuffer **outVertices,
	ssg::IndexBuffer **outIndices,
	ssg::GraphicsLoader *graphicsLoader)
{
	int ret;
	ssg::VertexBuffer *cubeVertices;
	ssg::IndexBuffer *cubeIndices;

	ret = graphicsLoader->createIndexBuffer(&cubeIndices, sizeof(uint16_t)*6*6);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = graphicsLoader->createVertexBuffer(&cubeVertices, sizeof(Vertex)*4*6);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	const float d = 0.5f;
	const float n = 1.0f;
	Vertex vertices[4*6] =
	{
		Vertex(vm::Vector3( -d,  d, -d ), vm::Vector3( 0.0f, n, 0.0f )),
		Vertex(vm::Vector3(  d,  d, -d ), vm::Vector3( 0.0f, n, 0.0f )),
		Vertex(vm::Vector3(  d,  d,  d ), vm::Vector3( 0.0f, n, 0.0f )),
		Vertex(vm::Vector3( -d,  d,  d ), vm::Vector3( 0.0f, n, 0.0f )),
		
		Vertex(vm::Vector3( -d, -d, -d ), vm::Vector3( 0.0f, -n, 0.0f )),
		Vertex(vm::Vector3(  d, -d, -d ), vm::Vector3( 0.0f, -n, 0.0f )),
		Vertex(vm::Vector3(  d, -d,  d ), vm::Vector3( 0.0f, -n, 0.0f )),
		Vertex(vm::Vector3( -d, -d,  d ), vm::Vector3( 0.0f, -n, 0.0f )),
		
		Vertex(vm::Vector3( -d, -d,  d ), vm::Vector3( -n, 0.0f, 0.0f )),
		Vertex(vm::Vector3( -d, -d, -d ), vm::Vector3( -n, 0.0f, 0.0f )),
		Vertex(vm::Vector3( -d,  d, -d ), vm::Vector3( -n, 0.0f, 0.0f )),
		Vertex(vm::Vector3( -d,  d,  d ), vm::Vector3( -n, 0.0f, 0.0f )),
		
		Vertex(vm::Vector3( d, -d,  d ), vm::Vector3( n, 0.0f, 0.0f )),
		Vertex(vm::Vector3( d, -d, -d ), vm::Vector3( n, 0.0f, 0.0f )),
		Vertex(vm::Vector3( d,  d, -d ), vm::Vector3( n, 0.0f, 0.0f )),
		Vertex(vm::Vector3( d,  d,  d ), vm::Vector3( n, 0.0f, 0.0f )),
		
		Vertex(vm::Vector3( -d, -d, -d ), vm::Vector3( 0.0f, 0.0f, -n )),
		Vertex(vm::Vector3(  d, -d, -d ), vm::Vector3( 0.0f, 0.0f, -n )),
		Vertex(vm::Vector3(  d,  d, -d ), vm::Vector3( 0.0f, 0.0f, -n )),
		Vertex(vm::Vector3( -d,  d, -d ), vm::Vector3( 0.0f, 0.0f, -n )),
		
		Vertex(vm::Vector3( -d, -d, d ), vm::Vector3( 0.0f, 0.0f, n )),
		Vertex(vm::Vector3(  d, -d, d ), vm::Vector3( 0.0f, 0.0f, n )),
		Vertex(vm::Vector3(  d,  d, d ), vm::Vector3( 0.0f, 0.0f, n )),
		Vertex(vm::Vector3( -d,  d, d ), vm::Vector3( 0.0f, 0.0f, n ))
	};
	uint16_t indices[6*6] =
	{
		3,1,0,
		2,1,3,
		
		6,4,5,
		7,4,6,
		
		11,9,8,
		10,9,11,
		
		14,12,13,
		15,12,14,
		
		19,17,16,
		18,17,19,
		
		22,20,21,
		23,20,22
	};
	Vertex *vbuf = (Vertex *)cubeVertices->beginWrite();
	uint16_t *ibuf = (uint16_t*)cubeIndices->beginWrite();
	
	memcpy(vbuf, vertices, sizeof(vertices));
	memcpy(ibuf, indices, sizeof(indices));
	
	cubeIndices->endWrite();
	cubeVertices->endWrite();

	*outVertices = cubeVertices;
	*outIndices  = cubeIndices;
	(void)ret;
}

static vm::Vector3 sphereVetex(int latIndex, int longIndex, float radius, int latitudeSubdiv, int longitudeSubdiv)
{
	const float rdt = M_PI * 2.0f / (float)longitudeSubdiv;
	const float rds = M_PI / (float)latitudeSubdiv;

	if (latIndex == 0) {
		return vm::Vector3(radius, 0.0f, 0.0f);
	} else if (latIndex == latitudeSubdiv) {
		return vm::Vector3(-radius, 0.0f, 0.0f);
	} else {
		float r = radius * sin(rds * latIndex);
		return vm::Vector3(
			radius * cos(rds*(latIndex)),
			r * sin(rdt * longIndex), 
			r * cos(rdt * longIndex));
	}
}

static vm::Vector3 sphereNormal(int latIndex, int longIndex, int latitudeSubdiv, int longitudeSubdiv)
{
	const float rdt = M_PI * 2.0f / (float)longitudeSubdiv;
	const float rds = M_PI / (float)latitudeSubdiv;
	float r = sin(rds * latIndex);
	return vm::Vector3(
		cos(rds*(latIndex)),
		r * sin(rdt * longIndex),
		r * cos(rdt * longIndex));
}


static int sphereIndex(int latIndex, int longIndex, int latitudeSubdiv, int longitudeSubdiv)
{
	if(latIndex==0){
		return 0;
	}else if(latIndex==latitudeSubdiv){
		return longitudeSubdiv * (latitudeSubdiv-1)  + 1;
	}else{
		return longitudeSubdiv * (latIndex-1) + longIndex + 1;
	}
}




void ssg::Internal::Fill3dEffect::createSphereVertex(
	ssg::VertexBuffer **outVertices,
	ssg::IndexBuffer **outIndices,
	int *outNumSphereIndices,
	ssg::GraphicsLoader *graphicsLoader,
	int longitudeSubdiv, 
	int latitudeSubdiv)
{
	int ret;
	ssg::VertexBuffer *sphereVertices;
	ssg::IndexBuffer *sphereIndices;


	const int numSphereVertex = longitudeSubdiv * (latitudeSubdiv - 1) + 2;
	const int numTriangles = longitudeSubdiv * 2 + longitudeSubdiv*2*(latitudeSubdiv - 2);
	const int numSphereIndex = numTriangles * 3;



	ret = graphicsLoader->createIndexBuffer(&sphereIndices, sizeof(uint16_t)*numSphereIndex);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = graphicsLoader->createVertexBuffer(&sphereVertices, sizeof(Vertex)*numSphereVertex);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	Vertex *vbuf = (Vertex *)sphereVertices->beginWrite();
	uint16_t *ibuf = (uint16_t*)sphereIndices->beginWrite();
	
	const float radius = 0.5f;

	int triangleIndex=0;
	for (int j=0; j<latitudeSubdiv+1; j++) {
		for (int i = 0; i < longitudeSubdiv; i++) {
			vbuf[sphereIndex(j, i, latitudeSubdiv, longitudeSubdiv)] = 
				Vertex(sphereVetex(j, i, radius, latitudeSubdiv, longitudeSubdiv),
				sphereNormal(j, i, latitudeSubdiv, longitudeSubdiv));

			if(j<latitudeSubdiv){

				int tri1_1 = sphereIndex(j,   i, latitudeSubdiv, longitudeSubdiv);
				int tri1_2 = sphereIndex(j,   (i+1)%longitudeSubdiv, latitudeSubdiv, longitudeSubdiv);
				int tri1_3 = sphereIndex(j+1, i, latitudeSubdiv, longitudeSubdiv);
				if((tri1_1!=tri1_2) && (tri1_2!=tri1_3) && (tri1_3!=tri1_1)){
					ibuf[triangleIndex*3+0] = tri1_1;
					ibuf[triangleIndex*3+1] = tri1_2;
					ibuf[triangleIndex*3+2] = tri1_3;
					triangleIndex++;
				}

				int tri2_1 = sphereIndex(j,   (i+1)%longitudeSubdiv, latitudeSubdiv, longitudeSubdiv);
				int tri2_2 = sphereIndex(j+1, (i+1)%longitudeSubdiv, latitudeSubdiv, longitudeSubdiv);
				int tri2_3 = sphereIndex(j+1, i, latitudeSubdiv, longitudeSubdiv);
				if((tri2_1!=tri2_2) && (tri2_2!=tri2_3) && (tri2_3!=tri2_1)){
					ibuf[triangleIndex*3+0] = tri2_1;
					ibuf[triangleIndex*3+1] = tri2_2;
					ibuf[triangleIndex*3+2] = tri2_3;
					triangleIndex++;
				}
			}
		}
	}

	sphereIndices->endWrite();
	sphereVertices->endWrite();
	*outVertices = sphereVertices;
	*outIndices  = sphereIndices;
	*outNumSphereIndices = numSphereIndex;
	(void)ret;
}

// xPos == 0 -> left center
// xPos == 1 -> left plain
// xPos == 2 -> body left side
// xPos == 3 -> body right side
// xPos == 4 -> right plain
// xPos == 5 -> right center
static vm::Vector3 cylinderVetex(int xPos, int longIndex, int longitudeSubdiv)
{
	float dr = 2.0f * M_PI / longitudeSubdiv;
	switch(xPos)
	{
	case 0: return vm::Vector3(-0.5f, 0.0f, 0.0f);
	case 1: return vm::Vector3(-0.5f, 0.5f*cos(dr*longIndex),  0.5f*sin(dr*longIndex));
	case 2: return vm::Vector3(-0.5f, 0.5f*cos(dr*longIndex),  0.5f*sin(dr*longIndex));
	case 3: return vm::Vector3(0.5f, 0.5f*cos(dr*longIndex),  0.5f*sin(dr*longIndex));
	case 4: return vm::Vector3(0.5f, 0.5f*cos(dr*longIndex),  0.5f*sin(dr*longIndex));
	case 5: return vm::Vector3(0.5f, 0.0f, 0.0f);
	default: abort(); return vm::Vector3(0.0f, 0.0f, 0.0f);
	}
}

static vm::Vector3 cylinderNormal(int xPos, int longIndex, int longitudeSubdiv)
{
	float dr = 2.0f * M_PI / longitudeSubdiv;
	switch(xPos)
	{
	case 0: return vm::Vector3(-1.0f, 0.0f, 0.0f);
	case 1: return vm::Vector3(-1.0f, 0.0f, 0.0f);
	case 2: return vm::Vector3(0.0f, cos(dr*longIndex),  sin(dr*longIndex));
	case 3: return vm::Vector3(0.0f, cos(dr*longIndex),  sin(dr*longIndex));
	case 4: return vm::Vector3(1.0f, 0.0f, 0.0f);
	case 5: return vm::Vector3(1.0f, 0.0f, 0.0f);
	default: abort();return vm::Vector3(0.0f, 0.0f, 0.0f);
	}
}


static int cylinderIndex(int xPos, int longIndex, int longitudeSubdiv)
{
	switch(xPos)
	{
	case 0: return 0;
	case 1: return 1+longIndex;
	case 2: return 1+longitudeSubdiv+longIndex;
	case 3: return 1+2*longitudeSubdiv+longIndex;
	case 4: return 1+3*longitudeSubdiv+longIndex;
	case 5: return 1+4*longitudeSubdiv;
	default: abort(); return 0;
	}
}


void ssg::Internal::Fill3dEffect::createCylinderVertex(
	ssg::VertexBuffer **outVertices,
	ssg::IndexBuffer **outIndices,
	int *outNumCylinderIndices,
	ssg::GraphicsLoader *graphicsLoader,
	int cylinderSubdiv)
{
	int ret;
	ssg::VertexBuffer *cylinderVertices;
	ssg::IndexBuffer *cylinderIndices;

	ret = graphicsLoader->createIndexBuffer(&cylinderIndices, sizeof(uint16_t)*cylinderSubdiv*4*3);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	*outNumCylinderIndices = cylinderSubdiv*4*3;

	ret = graphicsLoader->createVertexBuffer(&cylinderVertices, sizeof(Vertex)*(cylinderSubdiv*4+2));
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);



	Vertex *vbuf = (Vertex *)cylinderVertices->beginWrite();
	uint16_t *ibuf = (uint16_t*)cylinderIndices->beginWrite();


	for(int xpos=0; xpos<=5; xpos++){
		for(int i=0; i<cylinderSubdiv; i++){
			vbuf[cylinderIndex(xpos, i, cylinderSubdiv)] = Vertex(cylinderVetex(xpos, i, cylinderSubdiv), cylinderNormal(xpos, i, cylinderSubdiv));
		}
	}

	int triangleIndex=0;
	// left plain
	for(int i=0; i<cylinderSubdiv; i++){
		ibuf[triangleIndex*3+0] = cylinderIndex(0,i, cylinderSubdiv);
		ibuf[triangleIndex*3+1] = cylinderIndex(1,i, cylinderSubdiv);
		ibuf[triangleIndex*3+2] = cylinderIndex(1,(i+1)%cylinderSubdiv, cylinderSubdiv);
		triangleIndex++;
	}
	// right plain
	for(int i=0; i<cylinderSubdiv; i++){
		ibuf[triangleIndex*3+0] = cylinderIndex(5,i, cylinderSubdiv);
		ibuf[triangleIndex*3+1] = cylinderIndex(4,(i+1)%cylinderSubdiv, cylinderSubdiv);
		ibuf[triangleIndex*3+2] = cylinderIndex(4,i, cylinderSubdiv);
		triangleIndex++;
	}
	// bpdy plain
	for(int i=0; i<cylinderSubdiv; i++){
		ibuf[triangleIndex*3+0] = cylinderIndex(2,i, cylinderSubdiv);
		ibuf[triangleIndex*3+1] = cylinderIndex(3,i, cylinderSubdiv);
		ibuf[triangleIndex*3+2] = cylinderIndex(2,(i+1)%cylinderSubdiv, cylinderSubdiv);
		triangleIndex++;
		ibuf[triangleIndex*3+0] = cylinderIndex(2,(i+1)%cylinderSubdiv, cylinderSubdiv);
		ibuf[triangleIndex*3+1] = cylinderIndex(3,i, cylinderSubdiv);
		ibuf[triangleIndex*3+2] = cylinderIndex(3,(i+1)%cylinderSubdiv, cylinderSubdiv);
		triangleIndex++;
	}
	cylinderIndices->endWrite();
	cylinderVertices->endWrite();


	*outVertices = cylinderVertices;
	*outIndices  = cylinderIndices;
	(void)ret;
}





void ssg::Internal::Fill3dEffect::initialize(GraphicsLoader *graphicsLoader, 
	const Object3dRendererResource *resource)
{
	int ret;
	m_graphicsLoader = graphicsLoader;

	BlendInfo blendInfo;
	blendInfo.initializeAsAlphaBlend();

	m_effect.initialize(
		resource->m_fill3dVId,
		resource->m_fill3dFId,
		&blendInfo);

	// -----------------------------------------------------------
	// set up vertex stream
	// -----------------------------------------------------------

	VertexAttribute vattr[2];
	VertexStream vertexStreams[1];

	vattr[0].streamIndex = 0;
	vattr[0].offset = 0; 
	vattr[0].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[0].componentCount = 3;
	vattr[0].semantic = sce::SampleUtil::Graphics::kParameterSemanticPosition;
	vattr[0].semanticIndex = 0;

	vattr[1].streamIndex = 0;
	vattr[1].offset = sizeof(vm::Vector3); 
	vattr[1].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[1].componentCount = 3;
	vattr[1].semantic = sce::SampleUtil::Graphics::kParameterSemanticNormal;
	vattr[1].semanticIndex = 0;

	vertexStreams[0].stride     = sizeof(Vertex);
	vertexStreams[0].indexSource = sce::SampleUtil::Graphics::kIndexSourceIndex16Bit;

	ret = EffectInstance::initialize(graphicsLoader, 
		&m_effect,
		vattr, 2, vertexStreams, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);


	// -----------------------------------------------------------
	// set up param
	// -----------------------------------------------------------

	m_worldInverseTranspose = m_effect.getVertexShader()->getParamByName("WorldInverseTranspose");
	SCE_SAMPLE_UTIL_ASSERT( m_worldInverseTranspose != NULL);
	m_worldViewProjection   = m_effect.getVertexShader()->getParamByName("WorldViewProjection");
	SCE_SAMPLE_UTIL_ASSERT( m_worldViewProjection != NULL);
	m_world                 = m_effect.getVertexShader()->getParamByName("World");
	SCE_SAMPLE_UTIL_ASSERT( m_world != NULL);
	m_viewInverse           = m_effect.getVertexShader()->getParamByName("ViewInverse");
	SCE_SAMPLE_UTIL_ASSERT( m_viewInverse != NULL);
	m_lightPosition         = m_effect.getVertexShader()->getParamByName("lightPosition");
	SCE_SAMPLE_UTIL_ASSERT( m_lightPosition != NULL);

	m_color                 = m_effect.getFragmentShader()->getParamByName("color");
	SCE_SAMPLE_UTIL_ASSERT( m_color != NULL);
	m_ambient               = m_effect.getFragmentShader()->getParamByName("ambient");
	SCE_SAMPLE_UTIL_ASSERT( m_ambient != NULL);
	m_shininess             = m_effect.getFragmentShader()->getParamByName("shininess");
	SCE_SAMPLE_UTIL_ASSERT( m_shininess != NULL);

	// -----------------------------------------------------------

	createCubeVertex(&m_cubeVertices, &m_cubeIndices, graphicsLoader);
	createSphereVertex(&m_sphereVertices, &m_sphereIndices, &m_numSphereIndices, graphicsLoader, m_sphereSubdiv, m_sphereSubdiv2);
	createCylinderVertex(&m_cylinderVertices, &m_cylinderIndices, &m_numCylinderIndices, graphicsLoader, m_cylinderSubdiv);
	(void)ret;
}


void ssg::Internal::Fill3dEffect::finalize(void)
{
	int ret;

	if(m_cubeVertices != NULL){
		sce::SampleUtil::destroy(m_cubeVertices);
		m_cubeVertices = NULL;
	}
	if(m_cubeIndices != NULL){
		sce::SampleUtil::destroy(m_cubeIndices);
		m_cubeIndices = NULL;
	}
	if(m_sphereVertices != NULL){
		sce::SampleUtil::destroy(m_sphereVertices);
		m_sphereVertices = NULL;
	}
	if(m_sphereIndices != NULL){
		sce::SampleUtil::destroy(m_sphereIndices);
		m_sphereIndices = NULL;
	}
	if(m_cylinderVertices != NULL){
		sce::SampleUtil::destroy(m_cylinderVertices);
		m_cylinderVertices = NULL;
	}
	if(m_cylinderIndices != NULL){
		sce::SampleUtil::destroy(m_cylinderIndices);
		m_cylinderIndices = NULL;
	}

	ret = EffectInstance::finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	m_effect.finalize();
	(void)ret;

}

void ssg::Internal::Fill3dEffect::setUp(
	GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector3 lightPosition,
	sce::Vectormath::Simd::Aos::Vector4 color,
	float ambient,
	float shininess,
	VertexBuffer *vertexBuffer)
{
	vm::Matrix4 paramWorldInverseTransPose = orthoInverse(world);
	vm::Matrix4 paramWorldViewProjection   = transpose(projection * view * world);
	vm::Matrix4 paramWorld                 = transpose(world);
	vm::Matrix4 paramViewInverse           = transpose(orthoInverse(view));

	m_worldInverseTranspose->getValue()->setArrayDataF((const float*) &paramWorldInverseTransPose );
	m_worldViewProjection->getValue()->setArrayDataF((const float*) &paramWorldViewProjection );
	m_world->getValue()->setArrayDataF((const float*) &paramWorld );
	m_viewInverse->getValue()->setArrayDataF((const float*) &paramViewInverse);

	m_lightPosition->getValue()->setArrayDataF((const float*) &lightPosition);
	m_color->getValue()->setArrayDataF((const float*) &color);
	m_ambient->getValue()->setArrayDataF((const float*) &ambient);
	m_shininess->getValue()->setArrayDataF((const float*) &shininess);

	apply(context, &vertexBuffer, 1);
}

void ssg::Internal::Fill3dEffect::drawCube(
	GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector3 lightPosition,
	sce::Vectormath::Simd::Aos::Vector4 color,
	float ambient,
	float shininess)
{
	context->setFillMode(true);
	context->setCullMode(ssg::kCullCw);

	setUp(
		context,
		world,
		view,
		projection,
		lightPosition,
		color,
		ambient,
		shininess,
		m_cubeVertices);
	context->draw(kPrimitiveTriangles, m_cubeIndices, 36 );
}


void ssg::Internal::Fill3dEffect::drawSphere(
	GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector3 lightPosition,
	sce::Vectormath::Simd::Aos::Vector4 color,
	float ambient,
	float shininess)
{
	context->setFillMode(true);
	context->setCullMode(ssg::kCullCw);

	setUp(
		context,
		world,
		view,
		projection,
		lightPosition,
		color,
		ambient,
		shininess,
		m_sphereVertices);
	context->draw(kPrimitiveTriangles, m_sphereIndices, m_numSphereIndices );
}

void ssg::Internal::Fill3dEffect::drawCylinder(
	GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector3 lightPosition,
	sce::Vectormath::Simd::Aos::Vector4 color,
	float ambient,
	float shininess)
{
	context->setFillMode(true);
	context->setCullMode(ssg::kCullCw);

	setUp(
		context,
		world,
		view,
		projection,
		lightPosition,
		color,
		ambient,
		shininess,
		m_cylinderVertices);
	context->draw(kPrimitiveTriangles, m_cylinderIndices, m_numCylinderIndices );
}


#pragma endregion


#pragma region WireframeEffect

void ssg::Internal::WireframeEffect::createCubeVertex(
	VertexBuffer **outVertices,
	IndexBuffer **outIndices,
	GraphicsLoader *graphicsLoader)
{
	int ret;
	VertexBuffer *cubeVertices = NULL;
	IndexBuffer *cubeIndices = NULL;

	ret = graphicsLoader->createIndexBuffer(&cubeIndices, sizeof(uint16_t)*24);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = graphicsLoader->createVertexBuffer(&cubeVertices, sizeof(vm::Vector3)*8);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	{
/*
  3----2
 /|   /|
0----1 |
| 7--|-6
|/   |/
4----5
*/
		const float d = 0.5f;
		vm::Vector3 vertices[8] = 
		{
			vm::Vector3( -d,  d, -d ),
			vm::Vector3(  d,  d, -d ),
			vm::Vector3(  d,  d,  d ),
			vm::Vector3( -d,  d,  d ),

			vm::Vector3( -d,  -d, -d ),
			vm::Vector3(  d,  -d, -d ),
			vm::Vector3(  d,  -d,  d ),
			vm::Vector3( -d,  -d,  d )
		};
		uint16_t indices[24] =
		{
			0,1,    1,2,     2,3,     3,0,  // top
			0,4,    1,5,     2,6,     3,7,  // vertical
			4,5,    5,6,     6,7,     7,4
		};
		vm::Vector3 *vbuf = (vm::Vector3 *)cubeVertices->beginWrite();
		uint16_t *ibuf = (uint16_t*)cubeIndices->beginWrite();

		memcpy(vbuf, vertices, sizeof(vertices));
		memcpy(ibuf, indices, sizeof(indices));		
		
		cubeIndices->endWrite();
		cubeVertices->endWrite();
	}
	*outVertices = cubeVertices;
	*outIndices = cubeIndices;

	(void)ret;
}



void ssg::Internal::WireframeEffect::createSphereVertex(
	VertexBuffer **outVertices,
	IndexBuffer **outIndices,
	GraphicsLoader *graphicsLoader,
	int longitudeSubdiv, 
	int latitudeSubdiv)
{
	int ret;
	VertexBuffer *sphereVertices;
	IndexBuffer *sphereIndices;

	const int numSphereVertex = longitudeSubdiv * (latitudeSubdiv - 1) + 2;
	const int numSphereIndex = longitudeSubdiv * 4 * (latitudeSubdiv - 1) + longitudeSubdiv * 2;

	ret = graphicsLoader->createIndexBuffer(&sphereIndices, sizeof(uint16_t)*numSphereIndex);
	SCE_SAMPLE_UTIL_ASSERT( ret == SCE_OK );

	ret = graphicsLoader->createVertexBuffer(&sphereVertices, sizeof(vm::Vector3)*numSphereVertex);
	SCE_SAMPLE_UTIL_ASSERT( ret == SCE_OK );

	vm::Vector3 *sphere_vbuf = (vm::Vector3 *)sphereVertices->beginWrite();
	uint16_t *sphere_ibuf = (uint16_t*)sphereIndices->beginWrite();
	
	const float sphere_r = 0.5f;

	int lineIndex=0;
	for (int j=0; j<latitudeSubdiv+1; j++) {
		for (int i = 0; i < longitudeSubdiv; i++) {
			sphere_vbuf[sphereIndex(j, i, latitudeSubdiv, longitudeSubdiv)] = sphereVetex(j, i, sphere_r, latitudeSubdiv, longitudeSubdiv);
			if(j<latitudeSubdiv){
				int line1 = sphereIndex(j,   i, latitudeSubdiv, longitudeSubdiv);
				int line2 = sphereIndex(j+1, i, latitudeSubdiv, longitudeSubdiv);
				if(line1 != line2){
					sphere_ibuf[lineIndex*2]   = line1;
					sphere_ibuf[lineIndex*2+1] = line2;
					lineIndex++;
				}
			}
			{
				int line1 = sphereIndex(j, i, latitudeSubdiv, longitudeSubdiv);
				int line2 = sphereIndex(j, (i+1)%longitudeSubdiv, latitudeSubdiv, longitudeSubdiv);
				if(line1 != line2){
					sphere_ibuf[lineIndex*2]   = line1;
					sphere_ibuf[lineIndex*2+1] = line2;
					lineIndex++;
				}
			}
		}
	}
	sphereIndices->endWrite();
	sphereVertices->endWrite();

	*outVertices = sphereVertices;
	*outIndices = sphereIndices;
	(void)ret;
}


void ssg::Internal::WireframeEffect::createCylinderVertex(
	VertexBuffer **outVertices,
	IndexBuffer **outIndices,
	GraphicsLoader *graphicsLoader,
	int cylinderSubdiv)
{
	int ret;
	//Cylinder
	float vx, vy, vz;
	const int numCylinderVertex = cylinderSubdiv * 2 + 2;
	const int numCylinderIndex = cylinderSubdiv * 10;

	VertexBuffer *cylinderVertices = NULL;
	IndexBuffer *cylinderIndices = NULL;

	ret = graphicsLoader->createIndexBuffer(&cylinderIndices, sizeof(uint16_t)*numCylinderIndex);
	SCE_SAMPLE_UTIL_ASSERT( ret == SCE_OK );

	ret = graphicsLoader->createVertexBuffer(&cylinderVertices, sizeof(vm::Vector3)*numCylinderVertex);
	SCE_SAMPLE_UTIL_ASSERT( ret == SCE_OK );

	vm::Vector3 *cylinder_vbuf = (vm::Vector3 *)cylinderVertices->beginWrite();
	uint16_t *cylinder_ibuf = (uint16_t*)cylinderIndices->beginWrite();

	float r = 0.5f; 
	const float rd = M_PI * 2.0f / (float)cylinderSubdiv;
	const float half_height = 0.5f;
	
	for (int j = 0, k = 0; j < 2; j++) {
		vx = half_height * ((j == 0) ? 1 : -1);
		cylinder_vbuf[k++] = vm::Vector3(vx, 0.0f, 0.0f);
		for(int i = 0; i < cylinderSubdiv; i++) {
			vy = r * sin(rd * i);
			vz = r * cos(rd * i);
			cylinder_vbuf[k++] = vm::Vector3(vx, vy, vz);
		}
	}
	int k = 0;
	for (int j = 0, c = 0; j < 2; j++) {
		if (j == 0) c = 0;
		else c = cylinderSubdiv+1;
		for(int i = 0; i < cylinderSubdiv; i++) {
			cylinder_ibuf[k++] = c;
			cylinder_ibuf[k++] = c + i + 1;
			cylinder_ibuf[k++] = c + i + 1;
			if(i == cylinderSubdiv - 1) {
				cylinder_ibuf[k++] = c + 1;
			} else {
				cylinder_ibuf[k++] = c + i + 2;
			}
		}
	}
	for(int i = 0; i < cylinderSubdiv; i++) {
		cylinder_ibuf[k++] = i + 1;
		cylinder_ibuf[k++] = i + cylinderSubdiv + 2;
	}
	
	cylinderIndices->endWrite();
	cylinderVertices->endWrite();

	*outVertices = cylinderVertices;
	*outIndices = cylinderIndices;
	(void)ret;
}



void ssg::Internal::WireframeEffect::initialize(GraphicsLoader *graphicsLoader, 
	const Object3dRendererResource *resource)
{
	int ret;
	m_graphicsLoader = graphicsLoader;

	BlendInfo blendInfo;
	blendInfo.initializeAsAlphaBlend();

	m_effect.initialize(
		resource->m_wireframeVId,
		resource->m_wireframeFId,
		&blendInfo);

	// -----------------------------------------------------------
	// set up vertex stream
	// -----------------------------------------------------------

	VertexAttribute vattr[1];
	VertexStream vertexStreams[1];

	vattr[0].streamIndex = 0;
	vattr[0].offset = 0; 
	vattr[0].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[0].componentCount = 3;
	vattr[0].semantic = sce::SampleUtil::Graphics::kParameterSemanticPosition;
	vattr[0].semanticIndex = 0;

	vertexStreams[0].stride     = sizeof(vm::Vector3);
	vertexStreams[0].indexSource = sce::SampleUtil::Graphics::kIndexSourceIndex16Bit;

	ret = EffectInstance::initialize(graphicsLoader, 
		&m_effect,
		vattr, 1, vertexStreams, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);


	// -----------------------------------------------------------
	// set up param
	// -----------------------------------------------------------

	m_worldViewProjection   = m_effect.getVertexShader()->getParamByName("WorldViewProjection");
	SCE_SAMPLE_UTIL_ASSERT( m_worldViewProjection != NULL);
	m_color                 = m_effect.getFragmentShader()->getParamByName("color");
	SCE_SAMPLE_UTIL_ASSERT( m_color != NULL);
	// -----------------------------------------------------------

	ret = graphicsLoader->createIndexBuffer(&m_lineIndices, sizeof(uint16_t)*2);
	SCE_SAMPLE_UTIL_ASSERT( ret == SCE_OK );

	ret = graphicsLoader->createVertexBuffer(&m_lineVertices, sizeof(vm::Vector3)*2);
	SCE_SAMPLE_UTIL_ASSERT( ret == SCE_OK );

	uint16_t indices_line[2] = { 0,1 };

	uint16_t *line_ibuf = (uint16_t*)m_lineIndices->beginWrite();
	memcpy(line_ibuf, indices_line, sizeof(indices_line));		
		
	m_lineIndices->endWrite();
	
	vm::Vector3 *line_vbuf = (vm::Vector3 *)m_lineVertices->beginWrite();
	line_vbuf[0] = vm::Vector3(0.0f, 0.0f, 0.0f);
	line_vbuf[1] = vm::Vector3(1.0f, 0.0f, 0.0f);
	m_lineVertices->endWrite();


	createCubeVertex(&m_cubeVertices, &m_cubeIndices, graphicsLoader);
	createSphereVertex(&m_sphereVertices, &m_sphereIndices, graphicsLoader, m_sphereSubdiv, m_sphereSubdiv2);
	createCylinderVertex(&m_cylinderVertices,&m_cylinderIndices,graphicsLoader,	m_cylinderSubdiv);

	(void)ret;
}


void ssg::Internal::WireframeEffect::finalize(void)
{
	int ret;

	if(m_lineVertices != NULL){
		sce::SampleUtil::destroy(m_lineVertices);
		m_lineVertices = NULL;
	}
	if(m_lineIndices != NULL){
		sce::SampleUtil::destroy(m_lineIndices);
		m_lineIndices = NULL;
	}
	if(m_cubeVertices != NULL){
		sce::SampleUtil::destroy(m_cubeVertices);
		m_cubeVertices = NULL;
	}
	if(m_cubeIndices != NULL){
		sce::SampleUtil::destroy(m_cubeIndices);
		m_cubeIndices = NULL;
	}
	if(m_sphereVertices != NULL){
		sce::SampleUtil::destroy(m_sphereVertices);
		m_sphereVertices = NULL;
	}
	if(m_sphereIndices != NULL){
		sce::SampleUtil::destroy(m_sphereIndices);
		m_sphereIndices = NULL;
	}
	if(m_cylinderVertices != NULL){
		sce::SampleUtil::destroy(m_cylinderVertices);
		m_cylinderVertices = NULL;
	}
	if(m_cylinderIndices != NULL){
		sce::SampleUtil::destroy(m_cylinderIndices);
		m_cylinderIndices = NULL;
	}
	
	ret = EffectInstance::finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	m_effect.finalize();
	(void)ret;

}

void ssg::Internal::WireframeEffect::setUp(
	GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector4 color,
	ssg::VertexBuffer *vertexBuffer)
{
	vm::Matrix4 paramWorldViewProjection   = transpose(projection * view * world);

	m_worldViewProjection->getValue()->setArrayDataF((const float*) &paramWorldViewProjection );
	m_color->getValue()->setArrayDataF((const float*) &color);

	apply(context, &vertexBuffer, 1);
}

void ssg::Internal::WireframeEffect::drawLine(
	GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector3 begin,
	sce::Vectormath::Simd::Aos::Vector3 end,
	sce::Vectormath::Simd::Aos::Vector4 color,
	uint32_t lineWidth)
{
	context->setFillMode(false);
	context->setLineWidth(lineWidth);
	context->setCullMode(ssg::kCullNone);
	
	const sce::Vectormath::Simd::Aos::Vector3 v = end - begin;
	const float lensqr = lengthSqr(v);
	if (lensqr < 0.000001f) return;
	const sce::Vectormath::Simd::Aos::Vector3 nv = normalize(v);
	const sce::Vectormath::Simd::Aos::Vector3 xv = sce::Vectormath::Simd::Aos::Vector3(1.0f, 0.0f, 0.0f);
	const sce::Vectormath::Simd::Aos::Quat q = normalize(sce::Vectormath::Simd::Aos::Quat(cross(xv, nv), 1.0+dot(xv, nv)));
	sce::Vectormath::Simd::Aos::Matrix4 world = sce::Vectormath::Simd::Aos::Matrix4::identity();
	world.setTranslation(begin);
	world.setUpper3x3(sce::Vectormath::Simd::Aos::Matrix3(q));
	world = world * world.scale(sce::Vectormath::Simd::Aos::Vector3(sqrtf(lensqr)));

	setUp(
		context,
		world,
		view,
		projection,
		color,
		m_lineVertices);
	context->draw(kPrimitiveLines, m_lineIndices, 2);
}

void ssg::Internal::WireframeEffect::drawCube(
	GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector4 color,
	uint32_t lineWidth)
{
	context->setFillMode(false);
	context->setLineWidth(lineWidth);
	context->setCullMode(ssg::kCullNone);
	setUp(
		context,
		world,
		view,
		projection,
		color,
		m_cubeVertices);
	context->draw(kPrimitiveLines, m_cubeIndices, 8 + 8 + 8);
}

void ssg::Internal::WireframeEffect::drawSphere(
	GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector4 color,
	uint32_t lineWidth)
{
	context->setFillMode(false);
	context->setLineWidth(lineWidth);
	context->setCullMode(ssg::kCullNone);

	setUp(
		context,
		world,
		view,
		projection,
		color,
		m_sphereVertices);
	context->draw(kPrimitiveLines, m_sphereIndices, m_sphereSubdiv * 4 * (m_sphereSubdiv2 - 1) + m_sphereSubdiv * 2);
}

void ssg::Internal::WireframeEffect::drawCylinder(
	GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector4 color,
	uint32_t lineWidth)
{
	context->setFillMode(false);
	context->setLineWidth(lineWidth);
	context->setCullMode(ssg::kCullNone);

	setUp(
		context,
		world,
		view,
		projection,
		color,
		m_cylinderVertices);
	context->draw(kPrimitiveLines, m_cylinderIndices, m_cylinderSubdiv * 10);
}

#pragma endregion




int ssg::Internal::Object3dRendererResource::initialize(ssg::GraphicsLoader  *graphicsLoader)
{
	if(graphicsLoader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_graphicsLoader = graphicsLoader;

	m_fill3dVId = ssi::Resource::loadVertexShader(m_graphicsLoader, ssi::Resource::FILL_3D_V);
	SCE_SAMPLE_UTIL_ASSERT(m_fill3dVId != NULL);

	m_fill3dFId = ssi::Resource::loadFragmentShader(m_graphicsLoader, ssi::Resource::FILL_3D_F);
	SCE_SAMPLE_UTIL_ASSERT(m_fill3dFId != NULL);

	m_wireframeVId = ssi::Resource::loadVertexShader(m_graphicsLoader, ssi::Resource::WIREFRAME_V);
	SCE_SAMPLE_UTIL_ASSERT(m_wireframeVId != NULL);

	m_wireframeFId = ssi::Resource::loadFragmentShader(m_graphicsLoader, ssi::Resource::WIREFRAME_F);
	SCE_SAMPLE_UTIL_ASSERT(m_wireframeFId != NULL);


	return SCE_OK;
}

int ssg::Internal::Object3dRendererResource::finalize(void)
{
	int ret;
	if(m_fill3dVId != NULL){
		ret = sce::SampleUtil::destroy( m_fill3dVId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_fill3dVId = NULL;
	}
	if(m_fill3dFId != NULL){
		ret = sce::SampleUtil::destroy( m_fill3dFId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_fill3dFId = NULL;
	}
	if(m_wireframeVId != NULL){
		ret = sce::SampleUtil::destroy( m_wireframeVId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_wireframeVId = NULL;
	}
	if(m_wireframeFId != NULL){
		ret = sce::SampleUtil::destroy( m_wireframeFId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_wireframeFId = NULL;
	}
	(void)ret;
	return SCE_OK;
}

#pragma region Object3dRendererImpl

int ssgi::Object3dRendererImpl::initialize(ssg::GraphicsLoader *graphicsLoader)
{
	int ret;
	ret = m_resource.initialize(graphicsLoader);
	if(ret != SCE_OK){
		return ret;
	}
	m_fill3dEffect.initialize(graphicsLoader, &m_resource);
	m_wireframeEffect.initialize(graphicsLoader, &m_resource);

	return SCE_OK;
}

int ssgi::Object3dRendererImpl::finalize(void)
{
	int ret;
	m_fill3dEffect.finalize();
	m_wireframeEffect.finalize();

	ret = m_resource.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	return ret;
}




int ssg::createObject3dRenderer(Object3dRenderer** outObject3dRenderer, GraphicsLoader *loader)
{
	if(outObject3dRenderer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}
	int ret = SCE_OK;
	Impl::Object3dRendererImpl *o = new Impl::Object3dRendererImpl;
	ret = o->initialize(loader);
	if(ret != SCE_OK){
		delete o;
		return ret;
	}
	*outObject3dRenderer = o;
	return ret;
}


int ssgi::Object3dRendererImpl::fillCube(
	ssg::GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector3 lightPosition,
	sce::Vectormath::Simd::Aos::Vector4 color,
	float ambient,
	float shininess)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_fill3dEffect.drawCube(
	context,
	world,
	view,
	projection,
	lightPosition,
	color,
	ambient,
	shininess);
	return SCE_OK;
}


int ssgi::Object3dRendererImpl::drawLine(
	ssg::GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector3 begin,
	sce::Vectormath::Simd::Aos::Vector3 end,
	sce::Vectormath::Simd::Aos::Vector4 color,
	uint32_t lineWidth)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_wireframeEffect.drawLine(
	context,
	view,
	projection,
	begin,
	end,
	color,
	lineWidth);
	return SCE_OK;
}

int ssgi::Object3dRendererImpl::drawCube(
	ssg::GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector4 color,
	uint32_t lineWidth)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_wireframeEffect.drawCube(
	context,
	world,
	view,
	projection,
	color,
	lineWidth);
	return SCE_OK;
}

int ssgi::Object3dRendererImpl::drawSphere(
	ssg::GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector4 color,
	uint32_t lineWidth)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_wireframeEffect.drawSphere(
	context,
	world,
	view,
	projection,
	color,
	lineWidth);
	return SCE_OK;
}

int ssgi::Object3dRendererImpl::fillSphere(
	ssg::GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector3 lightPosition,
	sce::Vectormath::Simd::Aos::Vector4 color,
	float ambient,
	float shininess)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_fill3dEffect.drawSphere(
	context,
	world,
	view,
	projection,
	lightPosition,
	color,
	ambient,
	shininess);
	return SCE_OK;
}

int ssgi::Object3dRendererImpl::fillCylinder(
	ssg::GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector3 lightPosition,
	sce::Vectormath::Simd::Aos::Vector4 color,
	float ambient,
	float shininess)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_fill3dEffect.drawCylinder(
	context,
	world,
	view,
	projection,
	lightPosition,
	color,
	ambient,
	shininess);
	return SCE_OK;
}



int ssgi::Object3dRendererImpl::drawCylinder(
	ssg::GraphicsContext *context,
	sce::Vectormath::Simd::Aos::Matrix4_arg world,
	sce::Vectormath::Simd::Aos::Matrix4_arg view,
	sce::Vectormath::Simd::Aos::Matrix4_arg projection,
	sce::Vectormath::Simd::Aos::Vector4 color,
	uint32_t lineWidth)
{
	if(context == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_wireframeEffect.drawCylinder(
	context,
	world,
	view,
	projection,
	color,
	lineWidth);
	return SCE_OK;
}

#pragma endregion // Object3dRendererImpl