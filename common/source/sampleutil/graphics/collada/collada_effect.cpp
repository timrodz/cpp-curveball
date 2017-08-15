/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <sampleutil/graphics/collada.h>
#include "graphics/collada/collada_internal.h"
#include "dae/dae_effect.h"
#include "collada_material.h"
#ifdef __psp2__
#include <alloca.h>
#endif


namespace ssg = sce::SampleUtil::Graphics;
namespace ssgc = sce::SampleUtil::Graphics::Collada;
namespace ssgci = sce::SampleUtil::Graphics::Collada::Internal;





using namespace ssg;

void ssgc::EffectDataParameter::serialize(ssgci::Serializer *serializer)
{
	printf("            param %s : type=%s, ", m_name.c_str(), m_type == kTypeFloatArray ? "floatAry" : "image");
	if(m_type == kTypeFloatArray){
		for(uint32_t i=0; i<m_floatArraySize; i++){
			printf("%.2f ", m_floatArray[i]);
		}
		printf("\n");
	}else{
		printf("%s\n", m_imageId.c_str());
	}
}


void ssgci::EffectDataShaderImpl::serialize(ssgci::Serializer *serializer)
{
	printf("          shader : %s %s\n", m_shaderPath.c_str(), m_shaderEntry.c_str());
	for(uint32_t i=0; i<m_parameters.size(); i++){
		m_parameters.at(i).serialize(serializer);
	}
}

void ssgci::EffectDataImpl::serialize(ssgci::Serializer *serializer)
{
	printf("      EffectData\n");
	printf("        type : %s\n", m_profileCommon ? "profile_common" : "profile_cg");
	printf("        vertex\n");
	m_vertexShader.serialize(serializer);
	printf("        fragment\n");
	m_fragmentShader.serialize(serializer);

}




static int initSamp2d(ssg::Texture *& texture, 
					  Collada::Dae::effect::Sampler2D *sampler2d, Collada::Dae::effect::ProfileCg *profileCg,
					  Collada::ColladaData *collada,
					  ssgci::LibraryImages *images)
{
	ssg::Collada::Dae::effect::Surface *surface= profileCg->findNewParamBySid(sampler2d->m_source)->m_surface;
	texture = images->findImageById(surface->m_init_from)->getTexture();

	TextureAddrMode wrapS = ssg::kTextureAddrModeClamp;
	TextureAddrMode wrapT = ssg::kTextureAddrModeClamp;

	if(sampler2d->m_wrapS == "WRAP"){
		wrapS = ssg::kTextureAddrModeRepeat;
	}else if(sampler2d->m_wrapS == "MIRROR"){
		wrapS = ssg::kTextureAddrModeMirror;
	}else if(sampler2d->m_wrapS == "CLAMP"){
		wrapS = ssg::kTextureAddrModeClamp;
	}else if(sampler2d->m_wrapS == "MIRROR_ONCE"){
		wrapS = ssg::kTextureAddrModeMirrorClamp;
	}


	if(sampler2d->m_wrapT == "WRAP"){
		wrapT = ssg::kTextureAddrModeRepeat;
	}else if(sampler2d->m_wrapT == "MIRROR"){
		wrapT = ssg::kTextureAddrModeMirror;
	}else if(sampler2d->m_wrapT == "CLAMP"){
		wrapT = ssg::kTextureAddrModeClamp;
	}else if(sampler2d->m_wrapT == "MIRROR_ONCE"){
		wrapT = ssg::kTextureAddrModeClamp;
	}

	texture->setAddrMode(wrapS, wrapT);

/* TODO
	m_minfilter= sampler2d->m_minfilter;
	m_magfilter= sampler2d->m_magfilter;
	m_mipfilter= sampler2d->m_mipfilter;
*/
	return SCE_OK;
}

static int initializeParamValue(ssg::Effect::ParameterValue *paramValue, 
								ssg::Collada::Dae::effect::NewParam *newParam, 
								ssg::Collada::Dae::effect::ProfileCg *profileCg,
								Collada::ColladaData *collada, 
								ssgci::LibraryImages *images,
								std::string paramName,
								ssg::Collada::EffectDataShader *effectDataShader)
{
	switch(newParam->m_valueType)
	{
	case Collada::Dae::kValueSampler2d:
		{
			ssg::Texture *texture;
			initSamp2d(texture, newParam->m_sampler2D, profileCg, collada, images);
			paramValue->setTexture(texture);

			ssg::Collada::Dae::effect::Surface *surface= profileCg->findNewParamBySid(newParam->m_sampler2D->m_source)->m_surface;
			effectDataShader->addParameter(ssg::Collada::EffectDataParameter(paramName, surface->m_init_from));
			break;
		}
	default:
		{
			paramValue->setArrayDataF(newParam->m_floats, newParam->m_numFloats);
			effectDataShader->addParameter(ssg::Collada::EffectDataParameter(paramName, newParam->m_floats, newParam->m_numFloats));
		}

	}

	return SCE_OK;
}




static void initializeParamValueAsColor( ssg::Effect::ParameterValue *paramValue,
										Collada::Dae::effect::ProfileCommon* profileCommon, 
										Collada::Dae::effect::ColorOrTexture *cot)
{
	if(cot->m_isColor){
		paramValue->setArrayDataF(cot->m_rgba, 4);
	}else{
		float data[4] = { 0,0,0,0};
		paramValue->setArrayDataF(data, 4);
	}
}

static std::string getColorOrTextureImageId(Collada::Dae::effect::ColorOrTexture *cot,
											 Collada::Dae::effect::ProfileCommon* profileCommon)
{
	if(cot->m_isColor){
		SCE_SAMPLE_UTIL_ASSERT(0);
		return "";
	}else{
		const Collada::Dae::effect::NewParam *sampler = profileCommon->findNewParamBySid(cot->m_texture);
		std::string surfaceSid = sampler->m_sampler2D->m_source;
		const Collada::Dae::effect::NewParam *surface = profileCommon->findNewParamBySid(surfaceSid);
		return surface->m_surface->m_init_from;
	}

}

static int initializeParamValueAsTexture(ssg::Effect::ParameterValue *paramValue,
											  Collada::Dae::effect::ProfileCommon* profileCommon,  
											  Collada::Dae::effect::ColorOrTexture *cot, 
											  Collada::ColladaData *collada,
											  ssgci::LibraryImages *images,
											  ssgci::ResourceManager *resourceManager
											  )
{
	if(cot->m_isColor){
		paramValue->setTexture(resourceManager->m_builtinResource.m_dummyTexture);
	}else{
		const Collada::Dae::effect::NewParam *sampler = profileCommon->findNewParamBySid(cot->m_texture);
		std::string surfaceSid = sampler->m_sampler2D->m_source;
		const Collada::Dae::effect::NewParam *surface = profileCommon->findNewParamBySid(surfaceSid);

		ssg::Texture *texture = images->findImageById(surface->m_surface->m_init_from)->getTexture();
		paramValue->setTexture(texture);
		SCE_SAMPLE_UTIL_ASSERT(paramValue->getTexture() != NULL);

		TextureAddrMode wrapS = ssg::kTextureAddrModeClamp;
		TextureAddrMode wrapT = ssg::kTextureAddrModeClamp;

		if(sampler->m_sampler2D->m_wrapS == "WRAP"){
			wrapS = ssg::kTextureAddrModeRepeat;
		}else if(sampler->m_sampler2D->m_wrapS == "MIRROR"){
			wrapS = ssg::kTextureAddrModeMirror;
		}else if(sampler->m_sampler2D->m_wrapS == "CLAMP"){
			wrapS = ssg::kTextureAddrModeClamp;
		}else if(sampler->m_sampler2D->m_wrapS == "MIRROR_ONCE"){
			wrapS = ssg::kTextureAddrModeMirrorClamp;
		}


		if(sampler->m_sampler2D->m_wrapT == "WRAP"){
			wrapT = ssg::kTextureAddrModeRepeat;
		}else if(sampler->m_sampler2D->m_wrapT == "MIRROR"){
			wrapT = ssg::kTextureAddrModeMirror;
		}else if(sampler->m_sampler2D->m_wrapT == "CLAMP"){
			wrapT = ssg::kTextureAddrModeClamp;
		}else if(sampler->m_sampler2D->m_wrapT == "MIRROR_ONCE"){
			wrapT = ssg::kTextureAddrModeMirrorClamp;
		}

		texture->setAddrMode(wrapS, wrapT);

	}
	return SCE_OK;
}


static int initializeBlendFunc(ssg::BlendInfo *blendInfo, 
							   const Collada::Dae::effect::BlendFunc *blendFunc)
{
	if(blendFunc == NULL){
		return -1;
	}

	blendInfo->colorMask = ssg::kColorMaskAll;
	blendInfo->colorFunc = ssg::kBlendFuncAdd;
	blendInfo->alphaFunc = ssg::kBlendFuncAdd;
	if(blendFunc->m_src == "SRC_ALPHA"){
		blendInfo->colorSrc  = ssg::kBlendFactorSrcAlpha;
		blendInfo->alphaSrc  = ssg::kBlendFactorSrcAlpha;
	}else if(blendFunc->m_src == "ONE_MINUS_SRC_ALPHA"){
		blendInfo->colorSrc  = ssg::kBlendFactorOneMinusSrcAlpha;
		blendInfo->alphaSrc  = ssg::kBlendFactorOneMinusSrcAlpha;
	}else if(blendFunc->m_src == "ONE"){
		blendInfo->colorSrc  = ssg::kBlendFactorOne;
		blendInfo->alphaSrc  = ssg::kBlendFactorOne;
	}else{
		blendInfo->colorSrc  = ssg::kBlendFactorSrcAlpha;
		blendInfo->alphaSrc  = ssg::kBlendFactorSrcAlpha;
	}

	if(blendFunc->m_dest == "SRC_ALPHA"){
		blendInfo->colorDst  = ssg::kBlendFactorSrcAlpha;
		blendInfo->alphaDst  = ssg::kBlendFactorSrcAlpha;
	}if(blendFunc->m_dest == "ONE_MINUS_SRC_ALPHA"){
		blendInfo->colorDst  = ssg::kBlendFactorOneMinusSrcAlpha;
		blendInfo->alphaDst  = ssg::kBlendFactorOneMinusSrcAlpha;
	}else if(blendFunc->m_dest == "ONE"){
		blendInfo->colorDst  = ssg::kBlendFactorOne;
		blendInfo->alphaDst  = ssg::kBlendFactorOne;
	}else{
		blendInfo->colorDst  = ssg::kBlendFactorOneMinusSrcAlpha;
		blendInfo->alphaDst  = ssg::kBlendFactorOneMinusSrcAlpha;
	}
	return SCE_OK;
}


int ssgci::Effect::initializeProfileCg(ssgc::ColladaLoader *_loader,
									   ssgc::Dae::effect::ProfileCg* profileCg, 
									   ssgc::Dae::effect::Effect *effect,
									   ssgc::ColladaData *_collada)
{
	ssgci::ColladaLoaderImpl *loader = dynamic_cast<ssgci::ColladaLoaderImpl *>(_loader);
	ssgci::ColladaDataImpl *collada = dynamic_cast<ssgci::ColladaDataImpl *>(_collada);

	EffectDataShaderImpl vshader;
	EffectDataShaderImpl fshader;
	{
		Dae::effect::Shader *shader = profileCg->getShaderByStage("VERTEX");
		std::string source = profileCg->getSourceUrl(shader->m_name->m_source); 
		std::string shaderPath = ssgci::Util::getDir(loader->m_daePath) + source;

		//printf(__FILE__"(%d) vshader=%s\n", __LINE__, shaderPath.c_str());
		m_vertexProgramId = loader->m_resourceManager->loadVertexShader(shaderPath, shader->m_name->m_name);

		vshader = EffectDataShaderImpl(source, shader->m_name->m_name);
	}
	{
		Dae::effect::Shader *shader = profileCg->getShaderByStage("FRAGMENT");
		std::string source = profileCg->getSourceUrl(shader->m_name->m_source); 
		std::string shaderPath = ssgci::Util::getDir(loader->m_daePath) + source;
		m_fragmentProgramId = loader->m_resourceManager->loadFragmentShader(shaderPath, shader->m_name->m_name);

		fshader = EffectDataShaderImpl(source, shader->m_name->m_name);
	}
	m_loaderConfig = loader;
	m_effectData = EffectDataImpl(vshader, fshader);

	Dae::effect::BlendFunc *b = profileCg->m_technique->m_passes.at(0)->m_blendFunc;
	ssg::BlendInfo *blendInfo = NULL;

	ssg::BlendInfo tmpBlendInfo;
	if(b != NULL){
		int ret = initializeBlendFunc(&tmpBlendInfo, b);
		if(ret == SCE_OK){
			blendInfo = &tmpBlendInfo;
		}
	}

	int ret = ssg::Effect::EffectData::initialize(m_vertexProgramId, m_fragmentProgramId, blendInfo);
	if(ret != SCE_OK){
		return ret;
	}

	//printf(__FILE__"(%d) Effect::getNumAttributeParams()=%d\n", __LINE__, Effect::getNumAttributeParams());

	for(uint32_t i=0; i<getVertexShader()->getNumParams(); i++){
		Dae::effect::Bind *bind = profileCg->getShaderByStage("VERTEX")->findBindBySymbol(getVertexShader()->getParamByIndex(i)->getParameter()->getName());
		if(bind != NULL){
			ret = initializeParamValue(getVertexShader()->getParamByIndex(i)->getValue(), 
				                       profileCg->findNewParamBySid(bind->m_param->m_ref), 
									   profileCg, 
									   collada,
									   &collada->m_libraryImages,
									   getVertexShader()->getParamByIndex(i)->getParameter()->getName(),
									   m_effectData.getVertexShader());
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		}
	}
	for(uint32_t i=0; i<getFragmentShader()->getNumParams(); i++){
		Dae::effect::Bind *bind = profileCg->getShaderByStage("FRAGMENT")->findBindBySymbol(getFragmentShader()->getParamByIndex(i)->getParameter()->getName());
		if(bind != NULL){
			ret = initializeParamValue(getFragmentShader()->getParamByIndex(i)->getValue(), 
				                       profileCg->findNewParamBySid(bind->m_param->m_ref), 
									   profileCg, 
									   collada,
									   &collada->m_libraryImages,
									   getFragmentShader()->getParamByIndex(i)->getParameter()->getName(),
									   m_effectData.getFragmentShader());
			SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		}
	}

	return 0;
}


int ssgci::Effect::initializeProfileCommon(
	ssgc::ColladaLoader *_loaderConfig, 
	ssgc::Dae::effect::ProfileCommon* profileCommon, 
	ssgc::Dae::effect::Effect *effect,
	ssgc::ColladaData *_collada)

{
	ssgci::ColladaLoaderImpl *loaderConfig = dynamic_cast<ssgci::ColladaLoaderImpl *>(_loaderConfig);
	ssgci::ColladaDataImpl *collada = dynamic_cast<ssgci::ColladaDataImpl *>(_collada);

	const ssg::Program* vertexProgramId = loaderConfig->m_resourceManager->getBuiltinResource()->m_profileCommonPhongSkinningVertexProgramId;
	const ssg::Program* fragmentProgramId = loaderConfig->m_resourceManager->getBuiltinResource()->m_profileCommonPhongSkinningFragmentProgramId;

	int ret = ssg::Effect::EffectData::initialize(vertexProgramId, fragmentProgramId, NULL);
	if(ret != SCE_OK){
		return ret;
	}

	float light0Color[3] = { 0,0,0};

	Dae::effect::ColorOrTexture emission;
	Dae::effect::ColorOrTexture ambient;
	Dae::effect::ColorOrTexture diffuse;
	Dae::effect::ColorOrTexture specular;
	float shininess = 0.0f;

	if(profileCommon->m_technique->m_phong){
		Dae::effect::Phong *phong =  profileCommon->m_technique->m_phong;
		emission  = phong->m_emission;
		ambient   = phong->m_ambient;
		diffuse   = phong->m_diffuse;
		specular  = phong->m_specular;
		shininess = phong->m_shininess;
	}else if(profileCommon->m_technique->m_blinn){
		Dae::effect::Blinn *blinn =  profileCommon->m_technique->m_blinn;
		emission  = blinn->m_emission;
		ambient   = blinn->m_ambient;
		diffuse   = blinn->m_diffuse;
		specular  = blinn->m_specular;
		shininess = blinn->m_shininess;
	}else if(profileCommon->m_technique->m_lambert){
		Dae::effect::Lambert *lambert = profileCommon->m_technique->m_lambert;
		emission  = lambert->m_emission;
		ambient   = lambert->m_ambient;
		diffuse   = lambert->m_diffuse;
		specular  = lambert->m_specular;
	}

	getFragmentShader()->getParamByName("light0Color")->getValue()->setArrayDataF(light0Color, 3);
	initializeParamValueAsTexture(getFragmentShader()->getParamByName("emissionTexture")->getValue(), profileCommon, &emission, collada, &collada->m_libraryImages, loaderConfig->m_resourceManager);
	initializeParamValueAsTexture(getFragmentShader()->getParamByName("ambientTexture")->getValue(),  profileCommon, &ambient,  collada, &collada->m_libraryImages, loaderConfig->m_resourceManager);
	initializeParamValueAsTexture(getFragmentShader()->getParamByName("diffuseTexture")->getValue(),  profileCommon, &diffuse,  collada, &collada->m_libraryImages, loaderConfig->m_resourceManager);
	initializeParamValueAsTexture(getFragmentShader()->getParamByName("specularTexture")->getValue(), profileCommon, &specular, collada, &collada->m_libraryImages, loaderConfig->m_resourceManager);
	initializeParamValueAsColor(getFragmentShader()->getParamByName("emissionColor")->getValue(), profileCommon, &emission);
	initializeParamValueAsColor(getFragmentShader()->getParamByName("ambientColor")->getValue(), profileCommon, &ambient);
	initializeParamValueAsColor(getFragmentShader()->getParamByName("diffuseColor")->getValue(), profileCommon, &diffuse);
	initializeParamValueAsColor(getFragmentShader()->getParamByName("specularColor")->getValue(), profileCommon, &specular);
	getFragmentShader()->getParamByName("shininess")->getValue()->setArrayDataF(&shininess, 1);

	m_effectData = EffectDataImpl();

/*
	Dae::effect::ColorOrTexture emission;
	Dae::effect::ColorOrTexture ambient;
	Dae::effect::ColorOrTexture diffuse;
	Dae::effect::ColorOrTexture specular;
	float shininess = 0.0f;
*/
	if(emission.m_isColor){
		m_effectData.getFragmentShader()->addParameter(EffectDataParameter("emission", emission.m_rgba, 4));
	}else{
		std::string imgId = getColorOrTextureImageId(&emission, profileCommon);
		m_effectData.getFragmentShader()->addParameter(EffectDataParameter("emission", imgId));
	}
	if(ambient.m_isColor){
		m_effectData.getFragmentShader()->addParameter(EffectDataParameter("ambient", ambient.m_rgba, 4));
	}else{
		std::string imgId = getColorOrTextureImageId(&ambient, profileCommon);
		m_effectData.getFragmentShader()->addParameter(EffectDataParameter("ambient", imgId));
	}
	if(diffuse.m_isColor){
		m_effectData.getFragmentShader()->addParameter(EffectDataParameter("diffuse", diffuse.m_rgba, 4));
	}else{
		std::string imgId = getColorOrTextureImageId(&diffuse, profileCommon);
		m_effectData.getFragmentShader()->addParameter(EffectDataParameter("diffuse", imgId));
	}
	if(specular.m_isColor){
		m_effectData.getFragmentShader()->addParameter(EffectDataParameter("specular", specular.m_rgba, 4));
	}else{
		std::string imgId = getColorOrTextureImageId(&specular, profileCommon);
		m_effectData.getFragmentShader()->addParameter(EffectDataParameter("specular", imgId));
	}
	m_effectData.getFragmentShader()->addParameter(EffectDataParameter("shininess", &shininess, 1));

	//m_effectData.serialize(NULL);

	return 0;
}




ssgci::Effect::Effect(ssgc::ColladaLoader *loaderConfig, 
					  ssgc::Dae::effect::Effect* effect, 
					  ssgc::ColladaData *collada)
{
	m_loaderConfig = NULL;
	m_id = effect->m_id;


	m_vertexProgramId = NULL;
	m_fragmentProgramId = NULL;


	if(effect->m_profileCgs.size() > 0){
		initializeProfileCg(loaderConfig, effect->m_profileCgs.at(0), effect, collada);
	}else{
		initializeProfileCommon(loaderConfig, effect->m_profileCommons.at(0), effect, collada);
	}
	//m_paramManager = new SimpleRendererInternal::ProfileCgParameterManager(&m_vertexShader, &m_fragmentShader);

}

int ssg::Collada::Internal::Effect::finalize(void)
{
/*
	if(m_paramManager){
		delete m_paramManager;
		m_paramManager = NULL;
	}
*/
	ssg::Effect::EffectData::finalize();

	if(m_loaderConfig != NULL){
		if(m_vertexProgramId != NULL){
			m_loaderConfig->m_resourceManager->unloadVertexShader(m_vertexProgramId);
			m_vertexProgramId = NULL;
		}
		if(m_fragmentProgramId != NULL){
			//printf(__FILE__"(%d) check\n", __LINE__);
			m_loaderConfig->m_resourceManager->unloadFragmentShader(m_fragmentProgramId);	
			m_fragmentProgramId = NULL;
		}
		m_loaderConfig = NULL;
	}


	return 0;
}


sce::SampleUtil::Graphics::Collada::Internal::Effect::~Effect(void)
{
	finalize();
}
/*
void sce::SampleUtil::Graphics::Collada::Effect::copyFrom(const sce::SampleUtil::Graphics::Collada::Effect *from)
{
	m_id = from->m_id;
	m_loaderConfig = from->m_loaderConfig;

	if(from->m_vertexProgramId.isValid()){
		m_vertexShaderPath = from->m_vertexShaderPath;
		m_vertexShaderEntry = from->m_vertexShaderEntry;

		std::string shaderPath = Internal::Util::getDir(m_loaderConfig->m_daePath) + from->m_vertexShaderPath;
		m_vertexProgramId = m_loaderConfig->m_resourceManager.loadVertexShader(shaderPath, from->m_vertexShaderEntry);
	}
	if(from->m_fragmentProgramId.isValid()){
		m_fragmentShaderPath = from->m_fragmentShaderPath;
		m_fragmentShaderEntry = from->m_fragmentShaderEntry;

		std::string shaderPath = Internal::Util::getDir(m_loaderConfig->m_daePath) + from->m_fragmentShaderPath;
		m_fragmentProgramId = m_loaderConfig->m_resourceManager.loadFragmentShader(shaderPath, from->m_fragmentShaderEntry);
	}

	sce::SampleUtil::Graphics::Effect::Effect::copyFrom(from);
}
*/


void ssgci::MaterialImpl::initialize(sce::SampleUtil::Graphics::Collada::Dae::material::Material *material, 
									 ssgci::LibraryEffects *libraryEffects)
{

	m_id = material->m_id;
	m_effectId = Internal::Util::urlToId(material->m_instance_effect_url);
	Internal::Effect *effect = libraryEffects->findEffectById(m_effectId);
	m_effect.copyFrom(effect);

	m_effectData = effect->m_effectData;

	m_loaderConfig = effect->m_loaderConfig;
	if(effect->m_vertexProgramId != NULL){
		std::string vertexShaderPath = m_effectData.getVertexShader()->getPath();
		std::string vertexShaderEntry = m_effectData.getVertexShader()->getEntry();

		std::string shaderPath = Internal::Util::getDir(m_loaderConfig->m_daePath) + vertexShaderPath;
		m_vertexProgramId = m_loaderConfig->m_resourceManager->loadVertexShader(shaderPath, vertexShaderEntry);
	}
	if(effect->m_fragmentProgramId != NULL){
		std::string fragmentShaderPath = m_effectData.getFragmentShader()->getPath();
		std::string fragmentShaderEntry = m_effectData.getFragmentShader()->getEntry();

		std::string shaderPath = Internal::Util::getDir(m_loaderConfig->m_daePath) + fragmentShaderPath;
		m_fragmentProgramId = m_loaderConfig->m_resourceManager->loadFragmentShader(shaderPath, fragmentShaderEntry);
	}


}

void ssgci::MaterialImpl::finalize(void)
{
	if(m_loaderConfig != NULL){
		if(m_vertexProgramId != NULL){
			m_loaderConfig->m_resourceManager->unloadVertexShader(m_vertexProgramId);
			m_vertexProgramId = NULL;
		}
		if(m_fragmentProgramId != NULL){
			//printf(__FILE__"(%d) check\n", __LINE__);
			m_loaderConfig->m_resourceManager->unloadFragmentShader(m_fragmentProgramId);	
			m_fragmentProgramId = NULL;
		}
		m_loaderConfig = NULL;
	}

	m_id = "";
	m_effect.finalize();
}

void ssgci::MaterialImpl::serialize(ssgci::Serializer *serializer)
{
	printf("  Material\n");
	printf("    id : %s\n", m_id.c_str());
	printf("    effectId : %s\n", m_effectId.c_str());

	m_effectData.serialize(serializer);

}



void Collada::Internal::EffectInstance::applyNewParameterManager(ShaderParameterReference *instanceParameterManager)
{
	if(instanceParameterManager == NULL){
		return;
	}
	for(uint32_t i=0; i<getEffect()->getVertexShader()->getNumParams(); i++){
		const sce::SampleUtil::Graphics::Effect::EffectParameter *bu = getEffect()->getVertexShader()->getParamByIndex(i);

		sce::SampleUtil::Graphics::Effect::ParameterValue *v = instanceParameterManager->getParamValue(sce::SampleUtil::Graphics::kProgramTypeVertex, bu->getParameter());
		if(v != NULL){
			setParamValueReferenceByName(sce::SampleUtil::Graphics::kProgramTypeVertex,  bu->getParameter()->getName(), v);
		}
	}

	for(uint32_t i=0; i<getEffect()->getFragmentShader()->getNumParams(); i++){
		const sce::SampleUtil::Graphics::Effect::EffectParameter *bu = getEffect()->getFragmentShader()->getParamByIndex(i);
		sce::SampleUtil::Graphics::Effect::ParameterValue *v = instanceParameterManager->getParamValue(sce::SampleUtil::Graphics::kProgramTypeFragment, bu->getParameter());
		if(v != NULL){
			setParamValueReferenceByName(sce::SampleUtil::Graphics::kProgramTypeFragment,  bu->getParameter()->getName(), v);
		}
	}
}

sce::SampleUtil::Graphics::VertexAttribute Collada::Internal::EffectInstance::getVertexAttribute(const MeshPart *meshPart, const sce::SampleUtil::Graphics::Parameter *parameter)
{
	sce::SampleUtil::Graphics::VertexAttribute ret;
	sce::SampleUtil::Graphics::ParameterSemantic semantic = parameter->getSemantic();
	uint32_t semanticIndex  = parameter->getSemanticIndex();

	sce::SampleUtil::Graphics::ParameterSemantic s = semantic;
	int si = semanticIndex;

	//if(meshPart->hasSkin()){

		if(semantic == sce::SampleUtil::Graphics::kParameterSemanticTexcoord)
		{
			if(semanticIndex == 4){
				s = sce::SampleUtil::Graphics::kParameterSemanticBlendindices;
				si = 0;
			}
			if(semanticIndex == 5){
				s = sce::SampleUtil::Graphics::kParameterSemanticBlendindices;
				si = 1;
			}
			if(semanticIndex == 6){
				s = sce::SampleUtil::Graphics::kParameterSemanticBlendweight;
				si = 0;
			}
			if(semanticIndex == 7){
				s = sce::SampleUtil::Graphics::kParameterSemanticBlendweight;
				si = 1;
			}
		}
	//}
	ret = meshPart->getVertexAttribute(s, si);
	ret.semantic = semantic;
	ret.semanticIndex = semanticIndex;

	if(ret.componentCount == 0){
		ret.streamIndex = 1;
		ret.componentCount = parameter->getComponentCount() * parameter->getArraySize();
		if(s == sce::SampleUtil::Graphics::kParameterSemanticBlendindices){
			ret.offset = offsetof(Internal::DummyVertex, m_blendIndices);
		}else if(s == ssg::kParameterSemanticBlendweight){
			if(si == 0){
				ret.offset = offsetof(Internal::DummyVertex, m_blendWeight);
			}else{
				ret.offset = offsetof(Internal::DummyVertex, m_other);
			}
		}else{
			ret.offset = offsetof(Internal::DummyVertex, m_other);
		}
	}
	return ret;


}

int Collada::Internal::EffectInstance::initializeEffect(ssg::Collada::ColladaLoader* _loader,
									 const ssg::Effect::EffectData *effect,
									 const MeshPart *meshPart,
									 ShaderParameterReference **paramManagers, uint32_t numParamManagers)
{
	if(_loader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(effect == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(meshPart == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	int ret;
	ssgci::ColladaLoaderImpl *loader = dynamic_cast<ssgci::ColladaLoaderImpl *>(_loader);

	uint32_t numVertexAttributeParams = effect->getNumAttributeParams();

	ssg::VertexAttribute *vattr2 =(ssg::VertexAttribute*)alloca(sizeof(ssg::VertexAttribute)* numVertexAttributeParams);
	for(uint32_t i=0; i<numVertexAttributeParams; i++){
		const ssg::Parameter *parameter = effect->getAttributeParam(i);
		vattr2[i] = getVertexAttribute(meshPart, parameter);
	}
	
	ssg::VertexStream streamStrides[] = { 
		{ (uint16_t)meshPart->getStride(), 0 },
		{ 0, 1}
	};

	ret = ssg::Effect::EffectInstance::initialize(loader->getGraphicsLoader(), 
                                     effect, 
									 vattr2, numVertexAttributeParams,
									 streamStrides, 2);
	SCE_SAMPLE_UTIL_ASSERT_MSG( ret == SCE_OK, "ret=%#x\n", ret);

	/* -------------------------------------- */

	for(uint32_t i=0; i<getEffect()->getVertexShader()->getNumParams(); i++){
		const ssg::Effect::EffectParameter *bu = getEffect()->getVertexShader()->getParamByIndex(i);
		for(uint32_t j=0; j<numParamManagers; j++){
			ssg::Effect::ParameterValue *v = paramManagers[j]->getParamValue(ssg::kProgramTypeVertex, bu->getParameter());
			if(v != NULL){
				setParamValueReferenceByName(ssg::kProgramTypeVertex,  bu->getParameter()->getName(), v);
				break;
			}
		}
	}

	for(uint32_t i=0; i<getEffect()->getFragmentShader()->getNumParams(); i++){
		const ssg::Effect::EffectParameter *bu = getEffect()->getFragmentShader()->getParamByIndex(i);
		for(uint32_t j=0; j<numParamManagers; j++){
			ssg::Effect::ParameterValue *v = paramManagers[j]->getParamValue(ssg::kProgramTypeFragment, bu->getParameter());
			if(v != NULL){
				setParamValueReferenceByName(ssg::kProgramTypeFragment,  bu->getParameter()->getName(), v);
				break;
			}
		}
	}

	m_dummyStream               = loader->m_resourceManager->getBuiltinResource()->m_dummyVertexBuffer;
	//m_dummySkinningVertexBuffer = &loader->m_resourceManager.getBuiltinResource()->m_dummySkinningVertexBuffer;
	m_dummySkinMatrices         = loader->m_resourceManager->getBuiltinResource()->m_dummySkinMatrices;

	(void)ret;
	return SCE_OK;
}


Collada::Internal::EffectInstance::EffectInstance(void)
{
	m_loaderConfig = NULL;
	m_dummyStream = NULL;
	m_dummySkinMatrices = NULL;
	m_renderingStage = 0;

}
Collada::Internal::EffectInstance::~EffectInstance(void)
{
	finalize();
}


int Collada::Internal::EffectInstance::initialize(ssg::Collada::ColladaLoader *_loaderConfig , 
										const MeshPart *meshPart,
											   const ssg::Effect::EffectData *effect,
											   int renderingStage
							   )
{
	int ret;
	ssgci::ColladaLoaderImpl *loaderConfig = dynamic_cast<ssgci::ColladaLoaderImpl *>(_loaderConfig);

	ShaderParameterReference *paramManagers[] = { loaderConfig->m_globalUniformParamManager };

	ret = initializeEffect(loaderConfig,
		effect,
		meshPart,
		paramManagers, 1);

	SCE_SAMPLE_UTIL_ASSERT_MSG((ret == SCE_OK), "ret=%#x\n", ret);


	m_loaderConfig = loaderConfig;

	m_renderingStage = renderingStage;
	(void)ret;
	return SCE_OK;
}

int Collada::Internal::EffectInstance::finalize(void)
{
	ssg::Effect::EffectInstance::finalize();

	m_loaderConfig = NULL;
	m_dummyStream = NULL;
	m_dummySkinMatrices = NULL;
	m_renderingStage = 0;

	return SCE_OK;
}

void Collada::Internal::EffectInstance::cloneFrom(EffectInstance &from)
{
	ssg::Effect::EffectInstance::cloneFrom(from);
	m_loaderConfig = from.m_loaderConfig;
	m_dummyStream = from.m_dummyStream;
	m_dummySkinMatrices = from.m_dummySkinMatrices;
	m_renderingStage = from.m_renderingStage;
}



void Collada::Internal::EffectInstance::render(ssg::GraphicsContext *graphicsContext, 
	ssg::VertexBuffer *geometryStream,
	ssg::UniformBuffer *skinMatrices,
	ssg::IndexBuffer *indexBuffer,
	unsigned short numIndices,
	int renderingStage)
{
	if(m_renderingStage != renderingStage){
		return;
	}


	setup(graphicsContext, 
		geometryStream,
		skinMatrices);

	graphicsContext->draw(kPrimitiveTriangles, indexBuffer, numIndices);

}




void Collada::Internal::EffectInstance::setup(ssg::GraphicsContext *graphicsContext,
									ssg::VertexBuffer *vertexBuffer, 
									ssg::UniformBuffer *skinMatrices)
{
	ssg::VertexBuffer * buffers[] = { vertexBuffer,  m_dummyStream};
	apply(graphicsContext, buffers, 2);
	if(skinMatrices != NULL){
		// TODO
		int ret = graphicsContext->setUniformBuffer(ssg::kProgramTypeVertex, skinMatrices, 1);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		(void)ret;
	}else{
		int ret = graphicsContext->setUniformBuffer(ssg::kProgramTypeVertex, m_dummySkinMatrices, 1);
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		(void)ret;
	}
}




