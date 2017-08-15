/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#include <graphics/graphics_internal.h>
#include <graphics/collada/collada_util.h>
#include <graphics/collada/collada_internal.h>

#include "resource/resource_manager.h"

using namespace sce::SampleUtil::Graphics;
using namespace sce::SampleUtil::Graphics::Collada;
using namespace sce::Vectormath::Simd::Aos;

namespace ssi = sce::SampleUtil::Internal;
namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssgc = sce::SampleUtil::Graphics::Collada;
namespace ssgci = sce::SampleUtil::Graphics::Collada::Internal;


int ssgci::BuiltinResource::initialize(sce::SampleUtil::Graphics::GraphicsLoader *graphicsLoader)
{

	m_graphicsLoader  = graphicsLoader;

	m_graphicsLoader->createVertexBuffer(&m_dummyVertexBuffer, sizeof(DummyVertex));
	DummyVertex *dv = (DummyVertex *)m_dummyVertexBuffer->beginWrite();
	memset(dv, 0, sizeof(DummyVertex));
	dv->m_blendWeight[0] = 1.0;
	m_dummyVertexBuffer->endWrite();

	int ret = m_graphicsLoader->createUniformBuffer(&m_dummySkinMatrices, sizeof(Matrix4));
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	Matrix4 *sm = (Matrix4*)m_dummySkinMatrices->beginWrite();
	*sm = Matrix4::identity();
	m_dummySkinMatrices->endWrite();

	ret = m_graphicsLoader->createTexture(&m_dummyTexture,sce::SampleUtil::Graphics::kTextureFormatA8B8G8R8Unorm, 1, 1, 0);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	{
		uint32_t *dummyTextureData = (uint32_t*)m_dummyTexture->beginWrite();
		dummyTextureData[0] = 0x0;
		m_dummyTexture->endWrite();
	}
	ret = m_dummyTexture->setFilter(sce::SampleUtil::Graphics::kTextureFilterLinear, sce::SampleUtil::Graphics::kTextureFilterLinear, sce::SampleUtil::Graphics::kTextureMipFilterDisabled);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_profileCommonPhongSkinningVertexProgramId = ssi::Resource::loadVertexShader(m_graphicsLoader, ssi::Resource::COLLADA_SKINNING_PHONG_V);
	SCE_SAMPLE_UTIL_ASSERT(m_profileCommonPhongSkinningVertexProgramId != NULL);

	m_profileCommonPhongSkinningFragmentProgramId = ssi::Resource::loadFragmentShader(m_graphicsLoader, ssi::Resource::COLLADA_SKINNING_PHONG_F);
	SCE_SAMPLE_UTIL_ASSERT(m_profileCommonPhongSkinningVertexProgramId != NULL);

	return SCE_OK;
}


int ssgci::BuiltinResource::finalize(void)
{
	int ret;
	ret = sce::SampleUtil::destroy( m_profileCommonPhongSkinningVertexProgramId  );
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	ret = sce::SampleUtil::destroy( m_profileCommonPhongSkinningFragmentProgramId);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	sce::SampleUtil::destroy(m_dummyVertexBuffer);
	m_dummyVertexBuffer = NULL;
	sce::SampleUtil::destroy(m_dummySkinMatrices);
	m_dummySkinMatrices = NULL;

	sce::SampleUtil::destroy(m_dummyTexture);
	(void)ret;
	return SCE_OK;
}

#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

static std::string fixShaderPathFromCgToSb(std::string cgpath)
{
	size_t p = cgpath.find_last_of(".");
	if(p == cgpath.npos){
		return "";
	}
	return cgpath.substr(0, p) + ".sb";
}




sce::SampleUtil::Graphics::Program* ssgci::ResourceManager::loadVSInternal(std::string path, std::string entry)
{
	sce::SampleUtil::Graphics::GraphicsLoader *gc = m_graphicsLoader;

	std::string vertexShaderPath = fixShaderPathFromCgToSb( path);

	uint32_t size=0;
	void *binaryVertexProgram = Internal::Util::malloc_and_load( vertexShaderPath.c_str(), &size );

	sce::SampleUtil::Graphics::Program* vertexProgramId = NULL;
	int ret = gc->registerVertexProgram(&vertexProgramId, binaryVertexProgram, size);
	if(ret != SCE_OK){
		return NULL;
	}

	return vertexProgramId;
}

void ssgci::ResourceManager::unloadVSInternal(sce::SampleUtil::Graphics::Program* programId)
{
	const void* binary = dynamic_cast<const ssg::ProgramImpl*>(programId)->m_program->m_binary;
	sce::SampleUtil::destroy(programId);
	free((void*)binary);
}

sce::SampleUtil::Graphics::Program* ssgci::ResourceManager::loadFSInternal(std::string path, std::string entry)
{
	sce::SampleUtil::Graphics::GraphicsLoader *gc = m_graphicsLoader;
	std::string fragmentShaderPath = fixShaderPathFromCgToSb( path);

	uint32_t size=0;
	void *binaryFragmentProgram = Internal::Util::malloc_and_load( fragmentShaderPath.c_str(), &size );
	sce::SampleUtil::Graphics::Program* fragmentProgramId = NULL; 
	int ret = gc->registerFragmentProgram(&fragmentProgramId, binaryFragmentProgram, size);
	if(ret != SCE_OK){
		return NULL;
	}
	return fragmentProgramId;
}

void ssgci::ResourceManager::unloadFSInternal(sce::SampleUtil::Graphics::Program* programId)
{
	const void* binary = dynamic_cast<const ssg::ProgramImpl*>(programId)->m_program->m_binary;
	sce::SampleUtil::destroy(programId);
	free((void*)binary);
}

#else



sce::SampleUtil::Graphics::Program* ssgci::ResourceManager::loadVSInternal(std::string path, std::string entry)
{
#if _SCE_TARGET_OS_WINDOWS
	sce::SampleUtil::Graphics::Dx11::GraphicsLoaderDx11 *gc = dynamic_cast<sce::SampleUtil::Graphics::Dx11::GraphicsLoaderDx11*>(m_graphicsLoader);
	return gc->registerVertexProgramFromSourceFile(path.c_str(),  entry.c_str());
#endif /* _SCE_TARGET_OS_WINDOWS */


#if _SCE_TARGET_OS_PSP2
	sce::SampleUtil::Graphics::GraphicsLoader *gc = m_graphicsLoader;

	std::string vertexShaderPath = fixShaderPathFromCgToGxp( path);

	uint32_t size=0;
	const SceGxmProgram *binaryVertexProgram = (const SceGxmProgram *)Internal::Util::malloc_and_load( vertexShaderPath.c_str(), &size );

	sce::SampleUtil::Graphics::Program* vertexProgramId = NULL;
	int ret = gc->registerVertexProgram(&vertexProgramId, binaryVertexProgram, size);
	if(ret != SCE_OK){
		free((void*)binaryVertexProgram);
		return NULL;
	}

	return vertexProgramId;

#endif /* _SCE_TARGET_OS_PSP2 */
}

void ssgci::ResourceManager::unloadVSInternal(sce::SampleUtil::Graphics::Program* programId)
{
#ifndef __psp2__
	sce::SampleUtil::destroy(programId);
#else
	const SceGxmProgram *program = sce::SampleUtil::Graphics::Gxm::ProgramGxm::downcast(programId)->getGxmProgram();
	sce::SampleUtil::destroy(programId);
	//printf("### unload V : %#x\n",  (SceGxmShaderPatcherId)programId);
	free((void*)program);
#endif
}

sce::SampleUtil::Graphics::Program* ssgci::ResourceManager::loadFSInternal(std::string path, std::string entry)
{

#ifndef __psp2__
	sce::SampleUtil::Graphics::Dx11::GraphicsLoaderDx11 *gc = dynamic_cast<sce::SampleUtil::Graphics::Dx11::GraphicsLoaderDx11*>(m_graphicsLoader);
	return gc->registerFragmentProgramFromSourceFile(path.c_str(),  entry.c_str());//TOOD
#else
	sce::SampleUtil::Graphics::GraphicsLoader *gc = m_graphicsLoader;
	std::string fragmentShaderPath = fixShaderPathFromCgToGxp( path);

	uint32_t size=0;
	const SceGxmProgram *binaryFragmentProgram = (const SceGxmProgram *)Internal::Util::malloc_and_load( fragmentShaderPath.c_str(), &size );

	sce::SampleUtil::Graphics::Program* fragmentProgramId = NULL;
	int ret = gc->registerFragmentProgram(&fragmentProgramId, binaryFragmentProgram, size);
	if(ret != SCE_OK){
		free((void*)binaryFragmentProgram);
		return NULL;
	}
	//printf("### load F : %s, %#x\n", fragmentShaderPath.c_str(), (SceGxmShaderPatcherId)fragmentProgramId);
	return fragmentProgramId;

#endif
}

void ssgci::ResourceManager::unloadFSInternal(sce::SampleUtil::Graphics::Program* programId)
{
#ifndef __psp2__
	sce::SampleUtil::destroy(programId);
#else
	//printf("### unload F : %#x\n",(SceGxmShaderPatcherId)programId);

	const SceGxmProgram *program = sce::SampleUtil::Graphics::Gxm::ProgramGxm::downcast(programId)->getGxmProgram();
	sce::SampleUtil::destroy(programId);
	free((void*)program);
#endif
}




#endif


int ssgci::ResourceManager::initialize(sce::SampleUtil::Graphics::GraphicsLoader *graphicsLoader)
{
	m_graphicsLoader = graphicsLoader;

	return m_builtinResource.initialize(graphicsLoader);

}
int ssgci::ResourceManager::finalize(void)
{
	// TODO: check resources.
	return m_builtinResource.finalize();
}

sce::SampleUtil::Graphics::Program* ssgci::ResourceManager::loadVertexShader(std::string path, std::string entry)
{
	std::string id = path + "/" + entry;
	MapItr it = m_map.find(id);
	if(it == m_map.end()){
		const sce::SampleUtil::Graphics::Program* vertexProgramId = loadVSInternal(path, entry);
		Val v;
		v.count = 1;
		v.ext = Internal::Util::getExtention(path);
		v.val = vertexProgramId;
		m_map[id] = v;
		return (sce::SampleUtil::Graphics::Program*)v.val;
	}else{
		Val v = it->second;
		v.count++; 
		m_map[id] = v;
		return (sce::SampleUtil::Graphics::Program*)v.val;
	}
}

void ssgci::ResourceManager::unloadVertexShader(sce::SampleUtil::Graphics::Program* programId)
{
	for(MapItr it = m_map.begin(); it != m_map.end(); it++){
		if(it->second.val == programId){
			Val v = it->second;
			v.count--;

			if(v.count == 0){
				unloadVSInternal(programId);
				m_map.erase(it);
			}else{
				m_map[it->first] = v;
			}
			break;
		}
	}
}

sce::SampleUtil::Graphics::Program* ssgci::ResourceManager::loadFragmentShader(std::string path, std::string entry)
{

	std::string id = path + "/" + entry;
	MapItr it = m_map.find(id);
	if(it == m_map.end()){
		const sce::SampleUtil::Graphics::Program* fragmentProgramId = loadFSInternal(path, entry);
		Val v;
		v.count = 1;
		v.ext = Internal::Util::getExtention(path);
		v.val = fragmentProgramId;
		m_map[id] = v;
		return (sce::SampleUtil::Graphics::Program*)v.val;
	}else{
		Val v = it->second;
		v.count++; 
		m_map[id] = v;
		return (sce::SampleUtil::Graphics::Program*)v.val;
	}
}

void ssgci::ResourceManager::unloadFragmentShader(sce::SampleUtil::Graphics::Program* programId)
{
	for(MapItr it = m_map.begin(); it != m_map.end(); it++){
		if(it->second.val == programId){
			Val v = it->second;
			v.count--;
			if(v.count == 0){

				unloadFSInternal(programId);
				m_map.erase(it);
			}else{
				m_map[it->first] = v;
			}
			break;
		}
	}
}



sce::SampleUtil::Graphics::Texture* ssgci::ResourceManager::referTexture(const std::string &absPath)
{
	TextureMap::iterator it = m_textures.find(absPath);
	if(it == m_textures.end()){
		TextureCache c;
		int ret = m_graphicsLoader->createTextureFromFile(&c.m_texture, absPath.c_str());
		if(ret != SCE_OK){
			sce::SampleUtil::destroy(c.m_texture);
			c.m_texture = NULL;
			return NULL;
		}
		c.m_refCount = 1;
		m_textures[absPath] = c;
		return c.m_texture;
	}else{
		TextureCache c = it->second;
		c.m_refCount++;
		m_textures[it->first] = c;
		return c.m_texture;
	}
}

void ssgci::ResourceManager::decrReferenceTexture(const std::string &absPath)
{
	TextureMap::iterator it = m_textures.find(absPath);
	if(it != m_textures.end()){
		TextureCache c = it->second;
		c.m_refCount--;
		if(c.m_refCount == 0){
			sce::SampleUtil::destroy(c.m_texture);
			c.m_texture = NULL;
			m_textures.erase(absPath);
		}else{
			m_textures[it->first] = c;
		}
	}

}

// -------------------------------------------------------------------------------------------------



ssgci::DefaultShaderParameterManagerImpl::DefaultShaderParameterManagerImpl(ssgci::BuiltinResource *resource)
{
	m_world = sce::Vectormath::Simd::Aos::Matrix4::identity();
	m_bindShapeMatrix  = sce::Vectormath::Simd::Aos::Matrix4::identity();
	m_paramShadowViewProjection = sce::Vectormath::Simd::Aos::Matrix4::identity();

	m_light0Position = sce::Vectormath::Simd::Aos::Vector3(0, 100.0, 0); 
	m_light0Color    = sce::Vectormath::Simd::Aos::Vector3(1.0f);

	m_valueView                   .initializeAsArray(sce::SampleUtil::Graphics::kParameterTypeF32, 16, (float*)&m_paramView);
	m_valueViewInverse            .initializeAsArray(sce::SampleUtil::Graphics::kParameterTypeF32,16, (float*)&m_paramViewInverse);
	m_valueWorldViewProjection    .initializeAsArray(sce::SampleUtil::Graphics::kParameterTypeF32,16, (float*)&m_paramWorldViewProjection);
	m_valueWorld                  .initializeAsArray(sce::SampleUtil::Graphics::kParameterTypeF32,16, (float*)&m_paramWorld);
	m_valueWorldInverseTranspose  .initializeAsArray(sce::SampleUtil::Graphics::kParameterTypeF32,16, (float*)&m_paramWorldInverseTranspose);
	m_valuebindShapeMatrix        .initializeAsArray(sce::SampleUtil::Graphics::kParameterTypeF32,16, (float*)&m_bindShapeMatrix);
	m_valueLight0Position         .initializeAsArray(sce::SampleUtil::Graphics::kParameterTypeF32,3, (float*)&m_light0Position);
	m_valueLight0Color            .initializeAsArray(sce::SampleUtil::Graphics::kParameterTypeF32,3, (float*)&m_light0Color);

	m_valueShadowViewProjection   .initializeAsArray(sce::SampleUtil::Graphics::kParameterTypeF32, 16, (float*)&m_paramShadowViewProjection);
	m_valueShadowTex.initializeAsTexture(resource->m_dummyTexture);
}

ssgci::DefaultShaderParameterManagerImpl::~DefaultShaderParameterManagerImpl(void)
{
	m_valueView                   .finalize();
	m_valueViewInverse            .finalize();
	m_valueWorldViewProjection    .finalize();
	m_valueWorld                  .finalize();
	m_valueWorldInverseTranspose  .finalize();
	m_valuebindShapeMatrix        .finalize();
	m_valueLight0Position         .finalize();
	m_valueLight0Color            .finalize();

	m_valueShadowViewProjection   .finalize();
	m_valueShadowTex              .finalize();
}

sce::SampleUtil::Graphics::Effect::ParameterValue *ssgci::DefaultShaderParameterManagerImpl::getParamValue(sce::SampleUtil::Graphics::ProgramType type, 
																		 const sce::SampleUtil::Graphics::Parameter *parameter)
{
	if(parameter == NULL){
		return NULL;
	}

#if _SCE_TARGET_OS_ORBIS
	if(type > kProgramTypeCompute){
		return NULL;
	}
#else
	if(type > kProgramTypeFragment){
		return NULL;
	}
#endif
	const char *parameterName = parameter->getName();
	if(strcmp(parameterName, "ViewXf") == 0){
		return &m_valueView;
	}else if(strcmp(parameterName, "View") == 0){
		return &m_valueView;
	}else if(strcmp(parameterName, "mvpMatrix") == 0){
		return &m_valueWorldViewProjection;
	}else if(strcmp(parameterName, "WorldXf") == 0){
		return &m_valueWorldViewProjection;
	}else if(strcmp(parameterName, "WorldInverseTranspose") == 0){
		return &m_valueWorldInverseTranspose;
	}else if(strcmp(parameterName, "WorldViewProjection") == 0){
		return &m_valueWorldViewProjection;
	}else if(strcmp(parameterName, "World") == 0){
		return &m_valueWorld;
	}else if(strcmp(parameterName, "ViewInverse") == 0){
		return &m_valueViewInverse;
	}else if(strcmp(parameterName, "bindShapeMatrix") == 0){
		return &m_valuebindShapeMatrix;
	}else if(strcmp(parameterName, "BindShapeMatrix") == 0){
		return &m_valuebindShapeMatrix;
	}else if(strcmp(parameterName, "light0Position") == 0){
		return &m_valueLight0Position;
	}else if(strcmp(parameterName, "Light0Position") == 0){
		return &m_valueLight0Position;
	}else if(strcmp(parameterName, "light0Color") == 0){
		return &m_valueLight0Color;
	}else if(strcmp(parameterName, "Light0Color") == 0){
		return &m_valueLight0Color;
	}else if(strcmp(parameterName, "shadowViewProjMatrix") == 0){
		return &m_valueShadowViewProjection;
	}else if(strcmp(parameterName, "shadowTex") == 0){
		return &m_valueShadowTex;
	}
	return NULL;
}




void ssgci::DefaultShaderParameterManagerImpl::setLight(sce::Vectormath::Simd::Aos::Vector3_arg light0Position,
											 sce::Vectormath::Simd::Aos::Vector3_arg light0Color)
{
	m_light0Position = light0Position;
	m_light0Color  = light0Color;
	m_valueLight0Position         .setArrayDataF((const float*)&m_light0Position);
	m_valueLight0Color            .setArrayDataF((const float*)&m_light0Color);
}

void ssgci::DefaultShaderParameterManagerImpl::setWorldMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg world)
{
	m_world = world;
	updateMatrices();
}


void ssgci::DefaultShaderParameterManagerImpl::setViewMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg viewMatrix) 
{
	m_view =  viewMatrix;
	updateMatrices();
}

void ssgci::DefaultShaderParameterManagerImpl::setProjectionMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg projectionMatrix) 
{
	m_projection = projectionMatrix;
	updateMatrices();
}

void ssgci::DefaultShaderParameterManagerImpl::setShadowTex(Texture *texture)
{
	m_valueShadowTex.setTexture(texture);
}

void ssgci::DefaultShaderParameterManagerImpl::setShadowViewProjection(sce::Vectormath::Simd::Aos::Matrix4_arg shadowViewProjectionMatrix) 
{
	m_shadowViewProjection = shadowViewProjectionMatrix;
	updateMatrices();
}


void ssgci::DefaultShaderParameterManagerImpl::updateMatrices(void)
{
	m_paramView                  = transpose(m_view);
	m_paramViewInverse           = transpose(inverse(m_view));
	m_paramWorldViewProjection   = transpose(m_projection * m_view * m_world);
	m_paramWorld                 = transpose(m_world);
	m_paramWorldInverseTranspose = inverse(m_world);
	m_paramShadowViewProjection = transpose(m_shadowViewProjection * m_world);

	m_valueView                   .setArrayDataF((const float*)&m_paramView);
	m_valueViewInverse            .setArrayDataF((const float*)&m_paramViewInverse);
	m_valueWorldViewProjection    .setArrayDataF((const float*)&m_paramWorldViewProjection);
	m_valueWorld                  .setArrayDataF((const float*)&m_paramWorld);
	m_valueWorldInverseTranspose  .setArrayDataF((const float*)&m_paramWorldInverseTranspose);
	m_valueShadowViewProjection   .setArrayDataF((const float*)&m_paramShadowViewProjection);

}


// -------------------------------------------------------------------------------------------------



int ssgci::ColladaLoaderImpl::initialize(sce::SampleUtil::Graphics::GraphicsLoader *graphicsLoader, 
								   ShaderParameterReference *uniformParamManager,
								   MatrixChangeListener *matrixChangeListener)
{
	int ret;
	if(graphicsLoader == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	m_graphicsLoader = graphicsLoader;

	m_resourceManager = new Internal::ResourceManager;
	ret = m_resourceManager->initialize(m_graphicsLoader);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	m_useDefaultParamManager = uniformParamManager == NULL;
	m_defaultShaderParameterManager =  new ssgci::DefaultShaderParameterManagerImpl(&m_resourceManager->m_builtinResource);
	if(m_useDefaultParamManager){
		m_globalUniformParamManager =m_defaultShaderParameterManager;
	}else{
		m_globalUniformParamManager = uniformParamManager;
	}
	if(matrixChangeListener == NULL){
		m_matrixChangeListener = m_defaultShaderParameterManager;
	}else{
		m_matrixChangeListener = matrixChangeListener;
	}

	return ret;
}



int ssgci::ColladaLoaderImpl::finalize(void)
{
	int ret = 0;

	if(m_useDefaultParamManager){
		delete m_globalUniformParamManager;
		m_globalUniformParamManager = NULL;
	}
	if(m_resourceManager != NULL){
		ret = m_resourceManager->finalize();
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
		delete m_resourceManager;
		m_resourceManager = NULL;
	}

	return ret;
}


int ssgci::ColladaLoaderImpl::load(sce::SampleUtil::Graphics::Collada::ColladaData **outCollada, const char* daePath,
								   float framePeriod
)
{
	if(outCollada == NULL){
		return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_NULL_POINTER;
	}

	if(daePath == NULL){
		return SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ERROR_NULL_POINTER;
	}
	m_daePath = daePath;
	ColladaDataImpl *impl = new ColladaDataImpl;
	int ret = impl->initialize(daePath, this, framePeriod);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outCollada = impl;
	return SCE_OK;
}



DefaultShaderParameterManager *ssgci::ColladaLoaderImpl::getDefaultParams(void) const
{
	return m_defaultShaderParameterManager;
}

sce::SampleUtil::Graphics::GraphicsLoader *ssgci::ColladaLoaderImpl::getGraphicsLoader(void) const
{
	return m_graphicsLoader;
}

MatrixChangeListener *ssgci::ColladaLoaderImpl::getMatrixChangeListener(void) const 
{
	return m_matrixChangeListener;
}



int ssgc::createColladaLoader(ColladaLoader** outColladaLoder, 
											sce::SampleUtil::Graphics::GraphicsLoader *graphicsLoader,
											ShaderParameterReference *uniformParamManager,
											MatrixChangeListener *matrixChangeListener)
{
	if(outColladaLoder == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssgci::ColladaLoaderImpl *impl = new ssgci::ColladaLoaderImpl();
	int ret = impl->initialize(graphicsLoader, uniformParamManager, matrixChangeListener);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outColladaLoder = impl;
	return SCE_OK;
}
