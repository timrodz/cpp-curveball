/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <sampleutil/graphics/collada.h>
#include "graphics/collada/collada_internal.h"
#include <graphics/collada/collada_util.h>
#include <graphics/collada/dae/dae.h>

using namespace sce::SampleUtil::Graphics;
namespace ssg = sce::SampleUtil::Graphics;
namespace ssgc = sce::SampleUtil::Graphics::Collada;
namespace ssgci = sce::SampleUtil::Graphics::Collada::Internal;

int Collada::Image::initialize(sce::SampleUtil::Graphics::Collada::ColladaLoader *_loaderConfig, 
				       const std::string daePath, 
					   const Dae::Image::Image *daeImage)
{
	ssgci::ColladaLoaderImpl *loaderConfig = dynamic_cast<ssgci::ColladaLoaderImpl *>(_loaderConfig);

	m_path = daeImage->m_init_from;

	std::string texture_path;

	if(Internal::Util::isAbsolutePath(daeImage->m_init_from)){

		texture_path = daeImage->m_init_from;
	}else{

		std::string dir = Internal::Util::getDir(daePath);
		std::string init_from = daeImage->m_init_from;

		if(init_from.substr(0, 2)  == "./"){
			texture_path = dir + init_from.substr(2);
		}else{
			texture_path = dir + init_from;
		}
	}
	m_absPath = texture_path;
	m_texture = loaderConfig->m_resourceManager->referTexture(m_absPath);
	if(m_texture == NULL){
		return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
	}
	m_id = daeImage->m_id;
	m_name = daeImage->m_name;


	m_loaderConfig = loaderConfig;
	return 0;
}

int Collada::Image::finalize(void)
{
	if(m_texture != NULL){
		m_loaderConfig->m_resourceManager->decrReferenceTexture(m_absPath);
		m_texture = NULL;
	}
	return SCE_OK;
}


Collada::Image::Image(void)
{
	m_texture =NULL;
}

/*! このイメージのテクスチャを取得します */
sce::SampleUtil::Graphics::Texture *Collada::Image::getTexture(void)
{
	return m_texture;
}
/*! このイメージのテクスチャを取得します */
const sce::SampleUtil::Graphics::Texture *Collada::Image::getTexture(void) const
{
	return m_texture;
}

const std::string Collada::Image::getId(void) const
{
	return m_id;
}
const std::string Collada::Image::getName(void) const
{
	return m_name;
}

/*! get the absolute path of this iamge */
const std::string Collada::Image::getAbsolutePath(void) const
{
	return m_absPath;
}
const std::string Collada::Image::getPath(void) const
{
	return m_path;
}
Collada::Image::~Image(void)
{
	finalize();
}




void Collada::Image::serialize(Internal::Serializer *serializer)
{
	printf("  Serialize Image\n");
	printf("    id=%s, name=%s, path=%s\n", m_id.c_str(), m_name.c_str(), m_path.c_str());

}

