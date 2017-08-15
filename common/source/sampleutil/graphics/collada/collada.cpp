/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <graphics/collada/collada_util.h>
#include <graphics/collada/dae/dae.h>
#include <stdarg.h>
#include "collada_scene.h"
#include "collada_internal.h"
#include "sampleutil_internal.h"

using namespace sce::SampleUtil::Graphics;
using namespace sce::SampleUtil::Graphics::Collada::Internal::Util;
using namespace sce::SampleUtil::Graphics::Collada::Internal::controller;
namespace ssgc = sce::SampleUtil::Graphics::Collada;
namespace ssgci = sce::SampleUtil::Graphics::Collada::Internal;

int ssgci::ColladaDataImpl::initialize(const char* daePath, 
								 ssgc::ColladaLoader *loaderConfig,
								 float framePeriod)
{
	int ret;

	if(daePath == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(loaderConfig == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}


	Dae::Collada *daeCollada = new Dae::Collada();

	ret = daeCollada->load(daePath, loaderConfig);
	if(ret != 0){
		delete daeCollada;
		return ret;
	}

	ret = initialize(daeCollada, daePath, loaderConfig, framePeriod);
	daeCollada->dispose();
	delete daeCollada;

	if(ret != SCE_OK){
		return ret;
	}

	return ret;
}



int ssgci::ColladaDataImpl::initialize(ssgc::Dae::Collada *collada, 
								 const char *_daePath, 
								 sce::SampleUtil::Graphics::Collada::ColladaLoader *_loaderConfig, 
								 float framePeriod
								 )
{
	int ret;
	if(collada == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(_daePath == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(_loaderConfig == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	std::string daePath(_daePath);
	m_daePath = daePath;
	sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig =  _loaderConfig;
	m_loaderConfig = loaderConfig;

	ret = m_libraryImages.initialize(_loaderConfig, m_daePath, collada->m_libraryImages);
	if(ret < SCE_OK){
		finalize();
		return ret;
	}

	Internal::LibraryEffects *libraryEffects = new Internal::LibraryEffects;
	libraryEffects->initialize(_loaderConfig, collada->m_libraryEffects, this);

	m_libraryMaterials.initialize(collada->m_libraryMaterials, libraryEffects);

	if(libraryEffects != NULL){
		delete libraryEffects;
		libraryEffects = NULL;
	}

	Internal::geometry::LibraryGeometries *libraryGeometries = new Internal::geometry::LibraryGeometries;
	ret = libraryGeometries->initialize(collada->m_libraryGeometries, loaderConfig);
	if(ret < 0){
		finalize();
		return ret;
	}

	LibraryControllers *libraryControllers = new LibraryControllers;
	libraryControllers->initialize(loaderConfig, collada->m_libraryControllers, libraryGeometries);


	ret = m_libraryMeshes.initialize(loaderConfig, libraryControllers, libraryGeometries);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	if(libraryGeometries != NULL){
		libraryGeometries->finalize(loaderConfig);
		delete libraryGeometries;
		libraryGeometries = NULL;
	}
	if(libraryControllers != NULL){
		libraryControllers->dispose(loaderConfig);
		delete libraryControllers;
		libraryControllers = NULL;
	}


	m_libraryAnimations = new Internal::LibraryTransformAnimations;
	m_libraryAnimations->initialize(collada->m_libraryAnimations);


	std::pair<float,float> timeRange = m_libraryAnimations->getTimeRange();


	m_visualScene = new VisualSceneImpl;
	m_visualScene->initialize(_loaderConfig, 
		collada->m_libraryVisualScenes->m_visualScenes.at(0),
		this,
		&m_librarySkeletons,
		m_libraryAnimations);


	m_animation.initialize(m_libraryAnimations, m_visualScene, framePeriod);

	return 0;
}

const ssgc::Image *ssgci::ColladaDataImpl::getImageById(std::string id) const
{
	return m_libraryImages.findImageById(id);
}
const ssgc::Image *ssgci::ColladaDataImpl::getImageByIndex(uint32_t index) const
{
	if(m_libraryImages.m_numImages <= index){
		return NULL;
	}
	return &m_libraryImages.m_images[index];
}

ssgc::Image *ssgci::ColladaDataImpl::getImageByIndex(uint32_t index) 
{
	if(m_libraryImages.m_numImages <= index){
		return NULL;
	}
	return &m_libraryImages.m_images[index];
}


uint32_t ssgci::ColladaDataImpl::getNumImages(void)
{
	return m_libraryImages.m_numImages;
}


const ssgc::Material *ssgci::ColladaDataImpl::getMaterialById(std::string id) const
{
	return m_libraryMaterials.getMaterial(id);
}

const ssgc::Material *ssgci::ColladaDataImpl::getMaterialByIndex(uint32_t index) const
{
	if(index >= m_libraryMaterials.m_numMaterials)
	{
		return NULL;
	}
	return &m_libraryMaterials.m_materials[index];
}
uint32_t ssgci::ColladaDataImpl::getNumMaterials(void) const
{
	return m_libraryMaterials.m_numMaterials;
}

int ssgci::ColladaDataImpl::finalize(void)
{
	int ret;

	m_animation.finalize();

	if(m_visualScene != NULL){
		m_visualScene->finalize();
		delete m_visualScene;
		m_visualScene = NULL;
	}

	for(uint32_t i=0; i<m_librarySkeletons.size(); i++){
		Internal::Skeleton *skel = m_librarySkeletons.at(i);
		delete skel;
	}
	m_librarySkeletons.clear();

	if(m_libraryAnimations != NULL){
		delete m_libraryAnimations;
		m_libraryAnimations =NULL;
	}

	ret = m_libraryMeshes.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);


	m_libraryMaterials.finalize();
	m_libraryImages.finalize();
	(void)ret;
	return SCE_OK;
}

ssgci::ColladaDataImpl::ColladaDataImpl(void)
{
	m_libraryAnimations = NULL;
	m_visualScene = NULL;
	m_loaderConfig = NULL;
	m_visualScene = NULL;

}

ssgci::ColladaDataImpl::~ColladaDataImpl(void)
{
	finalize();
}


const Collada::Mesh *ssgci::ColladaDataImpl::getMeshByControllerId(std::string controllerId) const
{
	return m_libraryMeshes.findMeshByControllerId(controllerId);
}
const Collada::Mesh *ssgci::ColladaDataImpl::getMeshByGeometryId(std::string geometryId) const
{
	return m_libraryMeshes.findMeshByGeometryId(geometryId);
}
const Collada::Mesh *ssgci::ColladaDataImpl::getMeshByIndex(uint32_t index) const
{
	if(m_libraryMeshes.m_meshes.size() <= index){
		return NULL;
	}
	return m_libraryMeshes.m_meshes.at(index);
}
uint32_t ssgci::ColladaDataImpl::getNumMeshes(void) const
{
	return m_libraryMeshes.m_meshes.size();
}

const Collada::Animation* ssgci::ColladaDataImpl::getAnimation(void) const
{
	return &m_animation;
}




/* -------------------------------------------------------- */

int Collada::Internal::LibraryEffects::initialize(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, 
										Dae::effect::LibraryEffects *libraryEffects, 
										ColladaData *collada)
{
	for(unsigned int i=0; i<libraryEffects->m_effects.size(); i++){
		m_effects.push_back(new Effect(loaderConfig, libraryEffects->m_effects.at(i), collada));
	}
	return SCE_OK;
}
Collada::Internal::Effect *Collada::Internal::LibraryEffects::findEffectById(std::string id)
{
	for(unsigned int i=0; i<m_effects.size(); i++){
		if(m_effects.at(i)->m_id == id){
			return m_effects.at(i);
		}
	}
	return NULL;
}

int Collada::Internal::LibraryEffects::finalize(void)
{
	for(unsigned int i=0; i<m_effects.size(); i++){
		m_effects.at(i)->finalize();
		delete m_effects.at(i);
	}
	m_effects.clear();
	return SCE_OK;
}

Collada::Internal::LibraryEffects::~LibraryEffects(void)
{
	finalize();
}




int Collada::Internal::LibraryMaterials::initialize(Dae::material::LibraryMaterials *libraryMaterials, 
												   Internal::LibraryEffects *libraryEffects)
{
	m_numMaterials = libraryMaterials->m_materials.size();
	m_materials = new MaterialImpl[m_numMaterials];
	for(unsigned int i=0; i<m_numMaterials; i++){
		m_materials[i].initialize( libraryMaterials->m_materials.at(i), 
			libraryEffects
			);
	}
	return SCE_OK;
}

Collada::Material *Collada::Internal::LibraryMaterials::getMaterial(std::string id) const
{
	for(unsigned int i=0; i<m_numMaterials; i++){
		if(m_materials[i].getId() == id){
			return &m_materials[i];
		}
	}
	return NULL;
}

int Collada::Internal::LibraryMaterials::finalize(void)
{
	if(m_materials != NULL){
		for(unsigned int i=0; i<m_numMaterials; i++){
			m_materials[i].finalize();
		}
		delete [] m_materials;
	}

	m_numMaterials = 0;
	m_materials = NULL;
	return SCE_OK;
}


Collada::Internal::LibraryMaterials::LibraryMaterials(void)
{
	m_numMaterials = 0;
	m_materials = NULL;
}

Collada::Internal::LibraryMaterials::~LibraryMaterials(void)
{
	finalize();
}

void Collada::Internal::LibraryMaterials::serialize(Internal::Serializer *serializer)
{
	printf("[LibraryMaterials]\n");
	printf("  numMaterials = %d\n", m_numMaterials);
	for(uint32_t i=0; i<m_numMaterials; i++){
		m_materials[i].serialize(serializer);
	}
}

void Collada::Internal::LibraryMeshes::serialize(Internal::Serializer *serializer)
{
	printf("[LibraryMaterials]\n");
	printf("  numMeshes = %d\n", (int)m_meshes.size());
	for(uint32_t i=0; i<m_meshes.size(); i++){
		m_meshes.at(i)->serialize(serializer);
	}
}


int Collada::Internal::LibraryImages::initialize(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, 
							  const std::string daePath, 
							  Dae::Image::LibraryImages *libraryImages)
{
	m_dummyTexture = dynamic_cast<ColladaLoaderImpl*>(loaderConfig)->m_resourceManager->getBuiltinResource()->m_dummyTexture;

	//m_dummyImage.m_texture = m_dummyTexture;

	m_numImages = libraryImages->m_images.size();
	if(m_numImages  == 0){
		m_images = NULL;
		return SCE_OK;
	}

	m_images = new Image[m_numImages];
	for(uint32_t i=0; i<m_numImages; i++){
		int ret = m_images[i].initialize(loaderConfig, daePath, 
			//&colladaLibraryImages->getImages()[i],
			libraryImages->m_images.at(i)
			);
		if(ret < SCE_OK){
			finalize();
			return ret;
		}
	}

	return SCE_OK;
}

int Collada::Internal::LibraryImages::finalize(void)
{
	if(m_images != NULL){
		for(uint32_t i=0; i<m_numImages; i++){
			m_images[i].finalize();
		}
		delete [] m_images;
		m_images = NULL;
		m_numImages = 0;
	}
	return SCE_OK;

}

sce::SampleUtil::Graphics::Texture *Collada::Internal::LibraryImages::getTexture(std::string id)
{
	Image *img = findImageById(id);
	if(img == NULL){
		return NULL;
	}else{
		return img->getTexture();
	}
}

void Collada::Internal::LibraryImages::serialize(Internal::Serializer *serializer)
{
	printf("Serialize LibraryImages\n");
	printf("  numImages=%d\n", m_numImages);
	for(uint32_t i=0; i<m_numImages; i++){
		m_images[i].serialize(serializer);
	}
}

/*
struct Header
{
	char magic[16];


};
*/


int ssgci::ColladaDataImpl::serialize(const char *filename)
{
	Internal::Serializer serializer;
	int ret = serializer.initialize(filename);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_libraryImages.serialize(&serializer);
	m_libraryMaterials.serialize(&serializer);
	m_libraryMeshes.serialize(&serializer);

	ret = serializer.finalize();
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	return SCE_OK;
}
int ssgci::ColladaDataImpl::deserialize(const char *filename)
{
	return SCE_OK;
}


ssgc::ColladaLoader  *ssgci::ColladaDataImpl::getLoader() const 
{
	return m_loaderConfig;
}

const ssgc::VisualScene *ssgci::ColladaDataImpl::getVisualScene(void) const
{
	return m_visualScene;
}
