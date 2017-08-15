/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#include "stdafx.h"

#include "sprite_internal.h"

#pragma region _DrawingUtilResource

int ssg::Internal::_DrawingUtilResource::initialize(ssg::GraphicsLoader  *graphicsLoader)
{
	if(graphicsLoader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_graphicsLoader = graphicsLoader;

	m_sprite2dVId = ssi::Resource::loadVertexShader(m_graphicsLoader, ssi::Resource::SPRITE_2D_V);
	SCE_SAMPLE_UTIL_ASSERT(m_sprite2dVId != NULL);

	m_sprite2dFId = ssi::Resource::loadFragmentShader(m_graphicsLoader, ssi::Resource::SPRITE_2D_F);
	SCE_SAMPLE_UTIL_ASSERT(m_sprite2dFId != NULL);

	m_sprite2d2VId = ssi::Resource::loadVertexShader(m_graphicsLoader, ssi::Resource::SPRITE_2D_2_V);
	SCE_SAMPLE_UTIL_ASSERT(m_sprite2d2VId != NULL);

	m_sprite2d2FId = ssi::Resource::loadFragmentShader(m_graphicsLoader, ssi::Resource::SPRITE_2D_2_F);
	SCE_SAMPLE_UTIL_ASSERT(m_sprite2d2FId != NULL);

	m_sprite2dYuy2VId = ssi::Resource::loadVertexShader(m_graphicsLoader, ssi::Resource::SPRITE_2D_YUY2_V);
	SCE_SAMPLE_UTIL_ASSERT(m_sprite2dYuy2VId != NULL);

	m_sprite2dYuy2FId = ssi::Resource::loadFragmentShader(m_graphicsLoader, ssi::Resource::SPRITE_2D_YUY2_F);
	SCE_SAMPLE_UTIL_ASSERT(m_sprite2dYuy2FId != NULL);

#if _SCE_TARGET_OS_ORBIS
	m_sprite2dYcbcrVId = ssi::Resource::loadVertexShader(m_graphicsLoader, ssi::Resource::SPRITE_2D_YCBCR_V);
	SCE_SAMPLE_UTIL_ASSERT(m_sprite2dYcbcrVId != NULL);

	m_sprite2dYcbcrFId = ssi::Resource::loadFragmentShader(m_graphicsLoader, ssi::Resource::SPRITE_2D_YCBCR_F);
	SCE_SAMPLE_UTIL_ASSERT(m_sprite2dYcbcrFId != NULL);
#endif // _SCE_TARGET_OS_ORBIS

	m_fill2dVId = ssi::Resource::loadVertexShader(m_graphicsLoader, ssi::Resource::FILL_2D_V);
	SCE_SAMPLE_UTIL_ASSERT(m_fill2dVId != NULL);

	m_fill2dFId = ssi::Resource::loadFragmentShader(m_graphicsLoader, ssi::Resource::FILL_2D_F);
	SCE_SAMPLE_UTIL_ASSERT(m_fill2dVId != NULL);

	m_pointSprite2dVId = ssi::Resource::loadVertexShader(m_graphicsLoader, ssi::Resource::POINT_SPRITE_2D_V);
	SCE_SAMPLE_UTIL_ASSERT(m_fill2dVId != NULL);

	m_pointSprite2dFId = ssi::Resource::loadFragmentShader(m_graphicsLoader, ssi::Resource::POINT_SPRITE_2D_F);
	SCE_SAMPLE_UTIL_ASSERT(m_fill2dVId != NULL);

	return SCE_OK;
}

int ssg::Internal::_DrawingUtilResource::finalize(void)
{
	int ret;
	if(m_sprite2dVId != NULL){
		ret = sce::SampleUtil::destroy( m_sprite2dVId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_sprite2dVId = NULL;
	}
	if(m_sprite2dFId != NULL){
		ret = sce::SampleUtil::destroy(m_sprite2dFId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_sprite2dFId = NULL;
	}
	if(m_sprite2d2VId != NULL){
		ret = sce::SampleUtil::destroy( m_sprite2d2VId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_sprite2d2VId = NULL;
	}
	if(m_sprite2d2FId != NULL){
		ret = sce::SampleUtil::destroy(m_sprite2d2FId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_sprite2d2FId = NULL;
	}
	if(m_sprite2dYuy2VId != NULL){
		ret = sce::SampleUtil::destroy( m_sprite2dYuy2VId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_sprite2dYuy2VId = NULL;
	}
	if(m_sprite2dYuy2FId != NULL){
		ret = sce::SampleUtil::destroy(m_sprite2dYuy2FId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_sprite2dYuy2FId = NULL;
	}
#if _SCE_TARGET_OS_ORBIS
	if(m_sprite2dYcbcrVId != NULL){
		ret = sce::SampleUtil::destroy( m_sprite2dYcbcrVId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_sprite2dYcbcrVId = NULL;
	}
	if(m_sprite2dYcbcrFId != NULL){
		ret = sce::SampleUtil::destroy(m_sprite2dYcbcrFId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_sprite2dYcbcrFId = NULL;
	}
#endif // _SCE_TARGET_OS_ORBIS
	if(m_fill2dVId != NULL){
		ret = sce::SampleUtil::destroy(m_fill2dVId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_fill2dVId = NULL;
	}
	if(m_fill2dFId != NULL){
		ret = sce::SampleUtil::destroy(m_fill2dFId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_fill2dFId = NULL;
	}
	if(m_pointSprite2dVId != NULL){
		ret = sce::SampleUtil::destroy(m_pointSprite2dVId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_pointSprite2dVId = NULL;
	}
	if(m_pointSprite2dFId != NULL){
		ret = sce::SampleUtil::destroy(m_pointSprite2dFId);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		m_pointSprite2dFId = NULL;
	}
	(void)ret;
	return SCE_OK;
}

#pragma endregion

#pragma region _FillEffect


void ssg::Internal::_FillEffect::initialize(GraphicsLoader *graphicsLoader, 
	const _DrawingUtilResource *drawingUtilResource)
{
	int ret;
	m_graphicsLoader = graphicsLoader;

	BlendInfo blendInfo;
	blendInfo.initializeAsAlphaBlend();
	//blendInfo.initializeAsNoBlend();

	m_effect.initialize(
		drawingUtilResource->m_fill2dVId,
		drawingUtilResource->m_fill2dFId,
		&blendInfo);

	VertexAttribute vattr[1];
	VertexStream vertexStreams[1];

	vattr[0].streamIndex = 0;
	vattr[0].offset = 0; 
	vattr[0].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[0].componentCount = 2;
	vattr[0].semantic = sce::SampleUtil::Graphics::kParameterSemanticPosition;
	vattr[0].semanticIndex = 0;

	vertexStreams[0].stride     = sizeof(vm::Vector2);
	vertexStreams[0].indexSource = sce::SampleUtil::Graphics::kIndexSourceIndex16Bit;


	ret = EffectInstance::initialize(graphicsLoader, 
		&m_effect,
		vattr, 1, vertexStreams, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_offsetParam = m_effect.getVertexShader()->getParamByName("offset");
	SCE_SAMPLE_UTIL_ASSERT( m_offsetParam != NULL );
	m_sizeParam   = m_effect.getVertexShader()->getParamByName("size");
	SCE_SAMPLE_UTIL_ASSERT( m_sizeParam != NULL );
	m_depthParam   = m_effect.getVertexShader()->getParamByName("depth");
	SCE_SAMPLE_UTIL_ASSERT( m_depthParam != NULL );
	m_color       = m_effect.getFragmentShader()->getParamByName("color");
	SCE_SAMPLE_UTIL_ASSERT( m_color != NULL );

	ret = graphicsLoader->createIndexBuffer(&m_fillRectIndices, sizeof(uint16_t)*6);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		uint16_t *data = (uint16_t *)m_fillRectIndices->beginWrite();
		data[0] = 0;
		data[1] = 3;
		data[2] = 1;
		data[3] = 1;
		data[4] = 3;
		data[5] = 2;
		m_fillRectIndices->endWrite();
	}

	ret = graphicsLoader->createIndexBuffer(&m_drawRectIndices, sizeof(uint16_t)*8);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		uint16_t *data = (uint16_t *)m_drawRectIndices->beginWrite();
		data[0] = 0;
		data[1] = 1;
		data[2] = 1;
		data[3] = 2;
		data[4] = 2;
		data[5] = 3;
		data[6] = 3;
		data[7] = 0;
		m_drawRectIndices->endWrite();
	}


	ret = graphicsLoader->createVertexBuffer(&m_rectVertices, sizeof(vm::Vector2)*4);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		vm::Vector2 *data = (vm::Vector2 *)m_rectVertices->beginWrite();
		data[0] = vm::Vector2(0.0, 0.0);
		data[1] = vm::Vector2(1.0, 0.0);
		data[2] = vm::Vector2(1.0, 1.0);
		data[3] = vm::Vector2(0.0, 1.0);
		m_rectVertices->endWrite();
	}


	ret = graphicsLoader->createVertexBuffer(&m_ovalVertices, sizeof(vm::Vector2)*37);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	ret = graphicsLoader->createIndexBuffer(&m_fillOvalIndices, sizeof(uint16_t)*36*3);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	ret = graphicsLoader->createIndexBuffer(&m_drawOvalIndices, sizeof(uint16_t)*36*2);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	{
		vm::Vector2 *ovalV = (vm::Vector2*)m_ovalVertices->beginWrite();
		uint16_t *fillI = (uint16_t*)m_fillOvalIndices->beginWrite();
		uint16_t *drawI = (uint16_t*)m_drawOvalIndices->beginWrite();

		for(uint32_t i=0; i<36; i++){
			float rad = (float)(i * 2.0 *M_PI / 36.0);
			ovalV[i] = vm::Vector2(0.5, 0.5) + 0.5 * vm::Vector2(cos(rad), sin(rad));

			fillI[3*i+0] = (i+0)%36;
			fillI[3*i+1] = (i+1)%36;
			fillI[3*i+2] = 36;

			drawI[2*i+0] = (i+0)%36;
			drawI[2*i+1] = (i+1)%36;
		}
		ovalV[36]  = vm::Vector2(0.5, 0.5);

		m_ovalVertices->endWrite();
		m_fillOvalIndices->endWrite();
		m_drawOvalIndices->endWrite();

	}

	ret = graphicsLoader->createIndexBuffer(&m_lineIndices, sizeof(uint16_t)*2);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		uint16_t *data = (uint16_t *)m_lineIndices->beginWrite();
		data[0] = 0;
		data[1] = 1;
		m_lineIndices->endWrite();
	}

	ret = graphicsLoader->createVertexBuffer(&m_lineVertices,  sizeof(vm::Vector2)*2);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		vm::Vector2 *data = (vm::Vector2 *)m_lineVertices->beginWrite();
		data[0] = vm::Vector2(0.0, 0.0);
		data[1] = vm::Vector2(1.0, 1.0);
		m_lineVertices->endWrite();
	}
	(void)ret;

}


void ssg::Internal::_FillEffect::finalize(void)
{
	int ret;
	if(m_lineIndices != NULL){
		sce::SampleUtil::destroy(m_lineIndices);
		m_lineIndices = NULL;
	}
	if(m_lineVertices != NULL){
		sce::SampleUtil::destroy(m_lineVertices);
		m_lineVertices = NULL;
	}
	if(m_rectVertices != NULL){
		sce::SampleUtil::destroy(m_rectVertices);
		m_rectVertices = NULL;
	}
	if(m_ovalVertices != NULL){
		sce::SampleUtil::destroy(m_ovalVertices);
		m_ovalVertices = NULL;
	}
	if(m_fillRectIndices != NULL){
		sce::SampleUtil::destroy(m_fillRectIndices);
		m_fillRectIndices = NULL;
	}
	if(m_fillOvalIndices != NULL){
		sce::SampleUtil::destroy(m_fillOvalIndices);
		m_fillOvalIndices = NULL;
	}
	if(m_drawRectIndices != NULL){
		sce::SampleUtil::destroy(m_drawRectIndices);
		m_drawRectIndices= NULL;
	}
	if(m_drawOvalIndices != NULL){
		sce::SampleUtil::destroy(m_drawOvalIndices);
		m_drawOvalIndices = NULL;
	}

	ret = EffectInstance::finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	m_effect.finalize();
	(void)ret;
}

void ssg::Internal::_FillEffect::setUp(GraphicsContext *context,
	vm::Vector2_arg	offset,
	vm::Vector2_arg	size,
	float depth,
	vm::Vector4_arg rgba,
	VertexBuffer **vertexBuffer, uint32_t numVertexBuffers)
{

	m_offsetParam->getValue()->setArrayDataF((const float*)&offset, 2);
	m_sizeParam->getValue()->setArrayDataF((const float*)&size, 2);
	m_depthParam->getValue()->setArrayDataF((const float*)&depth, 1);
	m_color->getValue()->setArrayDataF((const float*)&rgba, 4);

	apply(context, vertexBuffer, numVertexBuffers);
}


void ssg::Internal::_FillEffect::fillRect(GraphicsContext *context,
	vm::Vector2_arg	offset,
	vm::Vector2_arg	size,
	float depth,
	vm::Vector4_arg rgba)
{
	context->setFillMode(true);

	VertexBuffer* buffers[] = { m_rectVertices };
	setUp(context,
		offset,
		size,
		depth,
		rgba, buffers, 1);

	context->draw(kPrimitiveTriangles, m_fillRectIndices, 6 );

}

void ssg::Internal::_FillEffect::drawRect(GraphicsContext *context,
	vm::Vector2_arg	offset,
	vm::Vector2_arg	size,
	float depth,
	vm::Vector4_arg rgba,
	uint32_t width)
{
	int ret;

	context->setFillMode(false);
	context->setLineWidth(width);

	VertexBuffer* buffers[] = {m_rectVertices };
	setUp(context,
		offset,
		size,
		depth,
		rgba, buffers, 1);

	ret = context->draw(kPrimitiveLines, m_drawRectIndices, 8 );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	(void)ret;
}

void ssg::Internal::_FillEffect::fillOval(GraphicsContext *context,
	vm::Vector2_arg	offset,
	vm::Vector2_arg	size,
	float depth,
	vm::Vector4_arg rgba)
{
	int ret;

	context->setFillMode(true);

	VertexBuffer* buffers[] = {m_ovalVertices };
	setUp(context,
		offset,
		size,
		depth,
		rgba, buffers, 1);

	ret = context->draw(kPrimitiveTriangles, m_fillOvalIndices, 36*3 );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	(void)ret;
}

void ssg::Internal::_FillEffect::drawOval(GraphicsContext *context,
	vm::Vector2_arg	offset,
	vm::Vector2_arg	size,
	float depth,
	vm::Vector4_arg rgba,
	uint32_t width)
{
	int ret;
	context->setFillMode(false);
	context->setLineWidth(width);

	VertexBuffer* buffers[] = { m_ovalVertices };
	setUp(context,
		offset,
		size,
		depth,
		rgba,
		buffers, 1);

	ret = context->draw(kPrimitiveLines, m_drawOvalIndices, 36*2 );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	(void)ret;
}

void ssg::Internal::_FillEffect::drawLine(GraphicsContext *context,
	vm::Vector2_arg begin,
	vm::Vector2_arg end,
	float depth,
	vm::Vector4_arg rgba,
	uint32_t width)
{
	int ret;

	context->setFillMode(false);

	VertexBuffer* buffers[] = {m_lineVertices };
	setUp(context,
		begin,
		(end - begin),
		depth,
		rgba, buffers, 1);

	context->setLineWidth(width);

	ret = context->draw(kPrimitiveLines, m_lineIndices, 2);

	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	(void)ret;
}

#pragma endregion


#pragma region _Sprite2dEffect

void ssg::Internal::_Sprite2dEffect::initialize(ssg::GraphicsLoader *graphicsLoader, const ssg::Internal::_DrawingUtilResource *drawingUtilResource)
{
	int ret;

	BlendInfo blendInfo;
	blendInfo.initializeAsAlphaBlend();

	m_graphicsLoader = graphicsLoader;
	m_effect.initialize(
		drawingUtilResource->m_sprite2dVId,
		drawingUtilResource->m_sprite2dFId,
		&blendInfo);
	VertexAttribute vattr[2];
	VertexStream vertexStreams[1];

	vattr[0].streamIndex = 0;
	vattr[0].offset = 0; 
	vattr[0].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[0].componentCount = 2;
	vattr[0].semantic = sce::SampleUtil::Graphics::kParameterSemanticPosition;
	vattr[0].semanticIndex = 0;

	vattr[1].streamIndex = 0;
	vattr[1].offset = sizeof(vm::Vector2); 
	vattr[1].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[1].componentCount = 2;
	vattr[1].semantic = sce::SampleUtil::Graphics::kParameterSemanticTexcoord;
	vattr[1].semanticIndex = 0;

	vertexStreams[0].stride     = sizeof(vm::Vector2) * 2;
	vertexStreams[0].indexSource =  sce::SampleUtil::Graphics::kIndexSourceIndex16Bit;

	ret = EffectInstance::initialize(graphicsLoader, 
		&m_effect,
		vattr, 2, vertexStreams, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);


	m_displayOffset = m_effect.getVertexShader()->getParamByName("displayOffset");
	m_sizeInDisplay = m_effect.getVertexShader()->getParamByName("sizeInDisplay");
	m_textureOffset = m_effect.getVertexShader()->getParamByName("textureOffset");
	m_sizeInTexture = m_effect.getVertexShader()->getParamByName("sizeInTexture");
	m_depthParam    = m_effect.getVertexShader()->getParamByName("depth");

	m_colorCoeff      = m_effect.getFragmentShader()->getParamByName("colorCoeff");
	m_tex             = m_effect.getFragmentShader()->getParamByName("tex");

	ret = m_graphicsLoader->createIndexBuffer(&m_rectIndices, sizeof(uint16_t)*6);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		uint16_t *data = (uint16_t *)m_rectIndices->beginWrite();
		data[0] = 0;
		data[1] = 3;
		data[2] = 1;
		data[3] = 1;
		data[4] = 3;
		data[5] = 2;
		m_rectIndices->endWrite();
	}

	ret = m_graphicsLoader->createVertexBuffer(&m_rectVertices, sizeof(TexturedVertex)*4);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		TexturedVertex *data = (TexturedVertex*)m_rectVertices->beginWrite();

		data[0].position = vm::Vector2(0.0, 0.0);
		data[0].texcoord = vm::Vector2(0.0, 0.0);

		data[1].position = vm::Vector2(1.0, 0.0);
		data[1].texcoord = vm::Vector2(1.0, 0.0);

		data[2].position = vm::Vector2(1.0, 1.0);
		data[2].texcoord = vm::Vector2(1.0, 1.0);

		data[3].position = vm::Vector2(0.0, 1.0);
		data[3].texcoord = vm::Vector2(0.0, 1.0);

		m_rectVertices->endWrite();
	}
	(void)ret;
}

void ssg::Internal::_Sprite2dEffect::finalize(void)
{
	int ret;
	if(m_rectVertices != NULL){
		sce::SampleUtil::destroy(m_rectVertices);
		m_rectVertices = NULL;
	}
	if(m_rectIndices != NULL){
		sce::SampleUtil::destroy(m_rectIndices);
		m_rectIndices = NULL;
	}

	ret = EffectInstance::finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	m_effect.finalize();
	(void)ret;
}


void ssg::Internal::_Sprite2dEffect::draw(GraphicsContext *context,
	vm::Vector2_arg   displayOffset,
	vm::Vector2_arg	sizeInDisplay, 
	float depth,
	vm::Vector2_arg	textureOffset,
	vm::Vector2_arg	sizeInTexture,
	vm::Vector4_arg   colorCoeff,
	Texture *texture)
{
	m_displayOffset->getValue()->setArrayDataF((float*)&displayOffset, 2);
	m_sizeInDisplay->getValue()->setArrayDataF((float*)&sizeInDisplay, 2);
	m_textureOffset->getValue()->setArrayDataF((float*)&textureOffset, 2);
	m_sizeInTexture->getValue()->setArrayDataF((float*)&sizeInTexture, 2);
	m_depthParam->getValue()->setArrayDataF(&depth, 1);

	m_colorCoeff->getValue()->setArrayDataF((float*)&colorCoeff, 4);
	m_tex->getValue()->setTexture(texture);

	VertexBuffer *buffers[] = { m_rectVertices };
	apply(context, buffers, 1);

	int ret;
	context->setFillMode(true);

	ret = context->draw( kPrimitiveTriangles, m_rectIndices, 6 );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	(void)ret;

}



void ssg::Internal::_Sprite2dEffect::drawWithVertexBuffer(ssg::GraphicsContext *context,
	vm::Vector2_arg   displayOffset,
	vm::Vector2_arg	sizeInDisplay, 
	float depth,
	ssg::VertexBuffer *vBuffers[],
	ssg::IndexBuffer  *iBuffer,
	const int nIndices, 
	vm::Vector4_arg   colorCoeff,
	ssg::Texture *texture)
{
	float _textureOffset[] = {0.0f, 0.0f};
	float _sizeInTexture[] = {1.0f, 1.0f};

	m_displayOffset->getValue()->setArrayDataF((float*)&displayOffset, 2);
	m_sizeInDisplay->getValue()->setArrayDataF((float*)&sizeInDisplay, 2);
	m_textureOffset->getValue()->setArrayDataF((float*)_textureOffset, 2);
	m_sizeInTexture->getValue()->setArrayDataF((float*)_sizeInTexture, 2);

	m_depthParam->getValue()->setArrayDataF(&depth, 1);

	m_colorCoeff->getValue()->setArrayDataF((float*)&colorCoeff, 4);
	m_tex->getValue()->setTexture(texture);

	apply(context, vBuffers, 1);

	int ret;
	context->setFillMode(true);

	ret = context->draw( kPrimitiveTriangles, iBuffer, nIndices);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	(void)ret;
}

#pragma endregion

#pragma region _Sprite2dEffect2

void ssg::Internal::_Sprite2dEffect2::initialize(ssg::GraphicsLoader *graphicsLoader, const ssg::Internal::_DrawingUtilResource *drawingUtilResource)
{
	int ret;

	BlendInfo blendInfo;
	blendInfo.initializeAsAlphaBlend();

	m_graphicsLoader = graphicsLoader;
	m_effect.initialize(
		drawingUtilResource->m_sprite2d2VId,
		drawingUtilResource->m_sprite2d2FId,
		&blendInfo);
	VertexAttribute vattr[2];
	VertexStream vertexStreams[1];

	vattr[0].streamIndex = 0;
	vattr[0].offset = 0; 
	vattr[0].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[0].componentCount = 2;
	vattr[0].semantic = sce::SampleUtil::Graphics::kParameterSemanticPosition;
	vattr[0].semanticIndex = 0;

	vattr[1].streamIndex = 0;
	vattr[1].offset = sizeof(vm::Vector2); 
	vattr[1].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[1].componentCount = 2;
	vattr[1].semantic = sce::SampleUtil::Graphics::kParameterSemanticTexcoord;
	vattr[1].semanticIndex = 0;

	vertexStreams[0].stride     = sizeof(vm::Vector2) * 2;
	vertexStreams[0].indexSource =  sce::SampleUtil::Graphics::kIndexSourceIndex16Bit;

	ret = EffectInstance::initialize(graphicsLoader, 
		&m_effect,
		vattr, 2, vertexStreams, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_depthParam    = m_effect.getVertexShader()->getParamByName("depth");
	m_colorCoeff      = m_effect.getFragmentShader()->getParamByName("colorCoeff");
	m_tex             = m_effect.getFragmentShader()->getParamByName("tex");

	(void)ret;
}

void ssg::Internal::_Sprite2dEffect2::finalize(void)
{
	int ret;
	ret = EffectInstance::finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	m_effect.finalize();
	(void)ret;
}


void ssg::Internal::_Sprite2dEffect2::draw(ssg::GraphicsContext *context,
	float depth,
	vm::Vector4_arg   colorCoeff,
	Texture *texture,
	VertexBuffer *vertexBuffer,
	IndexBuffer *indexBuffer, uint16_t numIndices)
{
	m_depthParam->getValue()->setArrayDataF(&depth, 1);
	m_colorCoeff->getValue()->setArrayDataF((float*)&colorCoeff, 4);
	m_tex->getValue()->setTexture(texture);

	VertexBuffer *buffers[] = { vertexBuffer };
	apply(context, buffers, 1);

	int ret;
	context->setFillMode(true);

	ret = context->draw( kPrimitiveTriangles, indexBuffer, numIndices);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	(void)ret;
}

#pragma endregion


#pragma region _Sprite2dYuy2Effect

void ssg::Internal::_Sprite2dYuy2Effect::initialize(ssg::GraphicsLoader *graphicsLoader, const ssg::Internal::_DrawingUtilResource *drawingUtilResource)
{
	int ret;

	BlendInfo blendInfo;
	blendInfo.initializeAsNoBlend();

	m_graphicsLoader = graphicsLoader;
	m_effect.initialize(
		drawingUtilResource->m_sprite2dYuy2VId,
		drawingUtilResource->m_sprite2dYuy2FId,
		&blendInfo);
	VertexAttribute vattr[2];
	VertexStream vertexStreams[1];

	vattr[0].streamIndex = 0;
	vattr[0].offset = 0; 
	vattr[0].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[0].componentCount = 2;
	vattr[0].semantic = sce::SampleUtil::Graphics::kParameterSemanticPosition;
	vattr[0].semanticIndex = 0;

	vattr[1].streamIndex = 0;
	vattr[1].offset = sizeof(vm::Vector2); 
	vattr[1].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[1].componentCount = 2;
	vattr[1].semantic = sce::SampleUtil::Graphics::kParameterSemanticTexcoord;
	vattr[1].semanticIndex = 0;

	vertexStreams[0].stride     = sizeof(vm::Vector2) * 2;
	vertexStreams[0].indexSource =  sce::SampleUtil::Graphics::kIndexSourceIndex16Bit;

	ret = EffectInstance::initialize(graphicsLoader, 
		&m_effect,
		vattr, 2, vertexStreams, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);


	m_displayOffset = m_effect.getVertexShader()->getParamByName("displayOffset");
	m_sizeInDisplay = m_effect.getVertexShader()->getParamByName("sizeInDisplay");
	m_textureOffset = m_effect.getVertexShader()->getParamByName("textureOffset");
	m_sizeInTexture = m_effect.getVertexShader()->getParamByName("sizeInTexture");
	m_depthParam    = m_effect.getVertexShader()->getParamByName("depth");

	m_colorCoeff      = m_effect.getFragmentShader()->getParamByName("colorCoeff");
	m_tex             = m_effect.getFragmentShader()->getParamByName("tex");
	m_texSize         = m_effect.getFragmentShader()->getParamByName("texSize");

	ret = m_graphicsLoader->createIndexBuffer(&m_rectIndices, sizeof(uint16_t)*6);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		uint16_t *data = (uint16_t *)m_rectIndices->beginWrite();
		data[0] = 0;
		data[1] = 3;
		data[2] = 1;
		data[3] = 1;
		data[4] = 3;
		data[5] = 2;
		m_rectIndices->endWrite();
	}

	ret = m_graphicsLoader->createVertexBuffer(&m_rectVertices, sizeof(TexturedVertex)*4);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		TexturedVertex *data = (TexturedVertex*)m_rectVertices->beginWrite();

		data[0].position = vm::Vector2(0.0, 0.0);
		data[0].texcoord = vm::Vector2(0.0, 0.0);

		data[1].position = vm::Vector2(1.0, 0.0);
		data[1].texcoord = vm::Vector2(1.0, 0.0);

		data[2].position = vm::Vector2(1.0, 1.0);
		data[2].texcoord = vm::Vector2(1.0, 1.0);

		data[3].position = vm::Vector2(0.0, 1.0);
		data[3].texcoord = vm::Vector2(0.0, 1.0);

		m_rectVertices->endWrite();
	}
	(void)ret;
}

void ssg::Internal::_Sprite2dYuy2Effect::finalize(void)
{
	int ret;
	if(m_rectVertices != NULL){
		sce::SampleUtil::destroy(m_rectVertices);
		m_rectVertices = NULL;
	}
	if(m_rectIndices != NULL){
		sce::SampleUtil::destroy(m_rectIndices);
		m_rectIndices = NULL;
	}

	ret = EffectInstance::finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	m_effect.finalize();
	(void)ret;
}


void ssg::Internal::_Sprite2dYuy2Effect::draw(ssg::GraphicsContext *context,
	vm::Vector2_arg   displayOffset,
	vm::Vector2_arg	sizeInDisplay, 
	float depth,
	vm::Vector2_arg	textureOffset,
	vm::Vector2_arg	sizeInTexture,
	vm::Vector4_arg   colorCoeff,
	ssg::Texture *texture)
{
	m_displayOffset->getValue()->setArrayDataF((float*)&displayOffset, 2);
	m_sizeInDisplay->getValue()->setArrayDataF((float*)&sizeInDisplay, 2);
	m_textureOffset->getValue()->setArrayDataF((float*)&textureOffset, 2);
	m_sizeInTexture->getValue()->setArrayDataF((float*)&sizeInTexture, 2);
	m_depthParam->getValue()->setArrayDataF(&depth, 1);

	m_colorCoeff->getValue()->setArrayDataF((float*)&colorCoeff, 4);
	vm::Vector2 texSize(texture->getWidth(), texture->getHeight());
	m_texSize->getValue()->setArrayDataF((float*)&texSize);
	m_tex->getValue()->setTexture(texture);

	VertexBuffer *buffers[] = { m_rectVertices };
	apply(context, buffers, 1);

	int ret;
	context->setFillMode(true);

	ret = context->draw( kPrimitiveTriangles, m_rectIndices, 6 );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	(void)ret;
}

#pragma endregion


#if _SCE_TARGET_OS_ORBIS

#pragma region _Sprite2dYcbcrEffect

void ssg::Internal::_Sprite2dYcbcrEffect::initialize(ssg::GraphicsLoader *graphicsLoader, const ssg::Internal::_DrawingUtilResource *drawingUtilResource)
{
	int ret;

	BlendInfo blendInfo;
	blendInfo.initializeAsNoBlend();

	m_graphicsLoader = graphicsLoader;
	m_effect.initialize(
		drawingUtilResource->m_sprite2dYuy2VId,
//		drawingUtilResource->m_sprite2dYcbcrVId,
		drawingUtilResource->m_sprite2dYcbcrFId,
		&blendInfo);
	VertexAttribute vattr[2];
	VertexStream vertexStreams[1];

	vattr[0].streamIndex = 0;
	vattr[0].offset = 0; 
	vattr[0].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[0].componentCount = 2;
	vattr[0].semantic = sce::SampleUtil::Graphics::kParameterSemanticPosition;
	vattr[0].semanticIndex = 0;

	vattr[1].streamIndex = 0;
	vattr[1].offset = sizeof(vm::Vector2); 
	vattr[1].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[1].componentCount = 2;
	vattr[1].semantic = sce::SampleUtil::Graphics::kParameterSemanticTexcoord;
	vattr[1].semanticIndex = 0;

	vertexStreams[0].stride     = sizeof(vm::Vector2) * 2;
	vertexStreams[0].indexSource =  sce::SampleUtil::Graphics::kIndexSourceIndex16Bit;

	ret = EffectInstance::initialize(graphicsLoader, 
		&m_effect,
		vattr, 2, vertexStreams, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_displayOffset = m_effect.getVertexShader()->getParamByName("displayOffset");
	m_sizeInDisplay = m_effect.getVertexShader()->getParamByName("sizeInDisplay");
	m_textureOffset = m_effect.getVertexShader()->getParamByName("textureOffset");
	m_sizeInTexture = m_effect.getVertexShader()->getParamByName("sizeInTexture");
	m_depthParam    = m_effect.getVertexShader()->getParamByName("depth");

	m_yTex            = m_effect.getFragmentShader()->getParamByName("yTexture");
	m_cbcrTex         = m_effect.getFragmentShader()->getParamByName("cbcrTexture");

	ret = m_graphicsLoader->createIndexBuffer(&m_rectIndices, sizeof(uint16_t)*6);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		uint16_t *data = (uint16_t *)m_rectIndices->beginWrite();
		data[0] = 0;
		data[1] = 3;
		data[2] = 1;
		data[3] = 1;
		data[4] = 3;
		data[5] = 2;
		m_rectIndices->endWrite();
	}

	ret = m_graphicsLoader->createVertexBuffer(&m_rectVertices, sizeof(TexturedVertex)*4);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		TexturedVertex *data = (TexturedVertex*)m_rectVertices->beginWrite();

		data[0].position = vm::Vector2(0.0, 0.0);
		data[0].texcoord = vm::Vector2(0.0, 0.0);

		data[1].position = vm::Vector2(1.0, 0.0);
		data[1].texcoord = vm::Vector2(1.0, 0.0);

		data[2].position = vm::Vector2(1.0, 1.0);
		data[2].texcoord = vm::Vector2(1.0, 1.0);

		data[3].position = vm::Vector2(0.0, 1.0);
		data[3].texcoord = vm::Vector2(0.0, 1.0);

		m_rectVertices->endWrite();
	}
	(void)ret;
}

void ssg::Internal::_Sprite2dYcbcrEffect::finalize(void)
{
	int ret;
	if(m_rectVertices != NULL){
		sce::SampleUtil::destroy(m_rectVertices);
		m_rectVertices = NULL;
	}
	if(m_rectIndices != NULL){
		sce::SampleUtil::destroy(m_rectIndices);
		m_rectIndices = NULL;
	}

	ret = EffectInstance::finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	m_effect.finalize();
	(void)ret;
}


void ssg::Internal::_Sprite2dYcbcrEffect::draw(ssg::GraphicsContext *context,
	vm::Vector2_arg   displayOffset,
	vm::Vector2_arg	sizeInDisplay, 
	float depth,
	vm::Vector2_arg	textureOffset,
	vm::Vector2_arg	sizeInTexture,
	vm::Vector4_arg   colorCoeff,
	ssg::Texture *yTexture, ssg::Texture *cbcrTexture)
{
	m_displayOffset->getValue()->setArrayDataF((float*)&displayOffset, 2);
	m_sizeInDisplay->getValue()->setArrayDataF((float*)&sizeInDisplay, 2);
	m_textureOffset->getValue()->setArrayDataF((float*)&textureOffset, 2);
	m_sizeInTexture->getValue()->setArrayDataF((float*)&sizeInTexture, 2);
	m_depthParam->getValue()->setArrayDataF(&depth, 1);

	m_yTex->getValue()->setTexture(yTexture);
	m_cbcrTex->getValue()->setTexture(cbcrTexture);

	VertexBuffer *buffers[] = { m_rectVertices };
	apply(context, buffers, 1);

	int ret;
	context->setFillMode(true);

	ret = context->draw( kPrimitiveTriangles, m_rectIndices, 6 );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	(void)ret;
}

#pragma endregion

#endif // _SCE_TARGET_OS_ORBIS


#pragma region _PointSprite2dEffect

void ssg::Internal::_PointSprite2dEffect::initialize(ssg::GraphicsLoader *graphicsLoader, const ssg::Internal::_DrawingUtilResource *drawingUtilResource)
{
	int ret;
	m_graphicsLoader = graphicsLoader;

	BlendInfo blendInfo;
	blendInfo.initializeAsAddBlend();

	m_effect.initialize(
		drawingUtilResource->m_pointSprite2dVId,
		drawingUtilResource->m_pointSprite2dFId,
		&blendInfo);

	VertexAttribute vattr[1];
	VertexStream vertexStreams[1];

	vattr[0].streamIndex = 0;
	vattr[0].offset = 0; 
	vattr[0].format = sce::SampleUtil::Graphics::kAttributeFormatF32;
	vattr[0].componentCount = 2;
	vattr[0].semantic = sce::SampleUtil::Graphics::kParameterSemanticPosition;
	vattr[0].semanticIndex = 0;

	vertexStreams[0].stride     = sizeof(vm::Vector2);
	vertexStreams[0].indexSource = kIndexSourceIndex16Bit;

	ret = EffectInstance::initialize(graphicsLoader, 
		&m_effect,
		vattr, 1,
		vertexStreams, 1);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);


	m_psize      = m_effect.getVertexShader()->getParamByName("psize");
	SCE_SAMPLE_UTIL_ASSERT( m_psize != NULL );
	m_depth        = m_effect.getVertexShader()->getParamByName("depth");
	SCE_SAMPLE_UTIL_ASSERT( m_depth != NULL );
	m_colorCoeff = m_effect.getFragmentShader()->getParamByName("colorCoeff");
	SCE_SAMPLE_UTIL_ASSERT( m_colorCoeff != NULL );
	m_tex        = m_effect.getFragmentShader()->getParamByName("tex");
	SCE_SAMPLE_UTIL_ASSERT( m_tex != NULL );

	(void)ret;
	return;
}

void ssg::Internal::_PointSprite2dEffect::finalize(void)
{
	int ret;

	ret = EffectInstance::finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	ret = m_effect.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	(void)ret;
}


void ssg::Internal::_PointSprite2dEffect::draw(ssg::GraphicsContext *context,
	float psize,
	float depth,
	vm::Vector4_arg   colorCoeff,
	Texture *texture,
	VertexBuffer *vertices,
	IndexBuffer *indices, 
	uint32_t numIndices
	)
{

	m_psize->getValue()->setArrayDataF(&psize, 1);
	m_colorCoeff->getValue()->setArrayDataF((float*)&colorCoeff, 4);
	m_tex->getValue()->setTexture(texture);
	m_depth->getValue()->setArrayDataF(&depth, 1);

	VertexBuffer *vertexBuffers[] = { vertices };
	apply(context, vertexBuffers, 1);

	int ret;
	context->setFillMode(true);
	//sceGxmSetFrontPolygonMode(context->getGxmContext(), SCE_GXM_POLYGON_MODE_POINT_01UV);

	ret = context->draw( kPrimitivePoints, indices, numIndices );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	(void)ret;
}

#pragma endregion

