/* SIE CONFIDENTIAL
PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#include "stdafx.h"
#include <scebase_common.h>


#include "resource_manager.h"
#include "sampleutil_internal.h"

#include <scebase_common.h>

namespace ss = sce::SampleUtil;
namespace ssi = sce::SampleUtil::Internal;
namespace ssg = sce::SampleUtil::Graphics;

#if _SCE_TARGET_OS_PSP2


extern const char _binary_sampleutil_sprite2d_v_gxp_start[];
extern const char _binary_sampleutil_sprite2d_f_gxp_start[];
extern const char _binary_sampleutil_sprite2d_2_v_gxp_start[];
extern const char _binary_sampleutil_sprite2d_2_f_gxp_start[];
extern const char _binary_sampleutil_sprite2d_yuy2_v_gxp_start[];
extern const char _binary_sampleutil_sprite2d_yuy2_f_gxp_start[];
extern const char _binary_sampleutil_fill2d_v_gxp_start[];
extern const char _binary_sampleutil_fill2d_f_gxp_start[];
extern const char _binary_sampleutil_fill3d_v_gxp_start[];
extern const char _binary_sampleutil_fill3d_f_gxp_start[];
extern const char _binary_sampleutil_point_sprite2d_v_gxp_start[];
extern const char _binary_sampleutil_point_sprite2d_f_gxp_start[];
extern const char _binary_sampleutil_wireframe_v_gxp_start[];
extern const char _binary_sampleutil_wireframe_f_gxp_start[];

extern const char _binary_sampleutil_sprite2d_v_gxp_size[];
extern const char _binary_sampleutil_sprite2d_f_gxp_size[];
extern const char _binary_sampleutil_sprite2d_2_v_gxp_size[];
extern const char _binary_sampleutil_sprite2d_2_f_gxp_size[];
extern const char _binary_sampleutil_sprite2d_yuy2_v_gxp_size[];
extern const char _binary_sampleutil_sprite2d_yuy2_f_gxp_size[];
extern const char _binary_sampleutil_fill2d_v_gxp_size[];
extern const char _binary_sampleutil_fill2d_f_gxp_size[];
extern const char _binary_sampleutil_fill3d_v_gxp_size[];
extern const char _binary_sampleutil_fill3d_f_gxp_size[];
extern const char _binary_sampleutil_point_sprite2d_v_gxp_size[];
extern const char _binary_sampleutil_point_sprite2d_f_gxp_size[];
extern const char _binary_sampleutil_wireframe_v_gxp_size[];
extern const char _binary_sampleutil_wireframe_f_gxp_size[];

extern const char _binary_sampleutil_ascii_tga_start[];
extern const char _binary_sampleutil_ascii_tga_size[];

extern const char _binary_sampleutil_collada_skinning_phong_v_gxp_start[];
extern const char _binary_sampleutil_collada_skinning_phong_v_gxp_size[];
extern const char _binary_sampleutil_collada_skinning_phong_f_gxp_start[];
extern const char _binary_sampleutil_collada_skinning_phong_f_gxp_size[];

ssg::Program* ssi::Resource::loadVertexShader(ssg::GraphicsLoader *loader,
													 ssi::Resource::VertexShaderId shader)
{
	const void *binary = NULL;
	uint32_t size = 0;
	switch(shader)
	{
	case SPRITE_2D_V:
		{
			binary = _binary_sampleutil_sprite2d_v_gxp_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_v_gxp_size;
			break;
		}
	case SPRITE_2D_2_V:
		{
			binary = _binary_sampleutil_sprite2d_2_v_gxp_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_2_v_gxp_size;
			break;
		}
	case SPRITE_2D_YUY2_V:
		{
			binary = _binary_sampleutil_sprite2d_yuy2_v_gxp_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_yuy2_v_gxp_size;
			break;
		}
	case FILL_2D_V:
		{
			binary = _binary_sampleutil_fill2d_v_gxp_start;
			size = (uintptr_t)_binary_sampleutil_fill2d_v_gxp_size;
			break;
		}
	case FILL_3D_V:
		{
			binary = _binary_sampleutil_fill3d_v_gxp_start;
			size = (uintptr_t)_binary_sampleutil_fill3d_v_gxp_size;
			break;
		}
	case POINT_SPRITE_2D_V:
		{
			binary = _binary_sampleutil_point_sprite2d_v_gxp_start;
			size = (uintptr_t)_binary_sampleutil_point_sprite2d_v_gxp_size;
			break;
		}
	case COLLADA_SKINNING_PHONG_V:
		{
			binary = _binary_sampleutil_collada_skinning_phong_v_gxp_start;
			size = (uintptr_t)_binary_sampleutil_collada_skinning_phong_v_gxp_size;
			break;
		}
	case WIREFRAME_V:
		{
			binary = _binary_sampleutil_wireframe_v_gxp_start;
			size = (uintptr_t)_binary_sampleutil_wireframe_v_gxp_size;
			break;
		}
	default:
		break;
	}

	if(binary == NULL){
		return NULL;
	}

	ssg::Program* program = NULL;
	int ret = loader->registerVertexProgram(&program, binary, size);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	SCE_SAMPLE_UTIL_ASSERT(program != NULL);

	return program;
}


ssg::Program* ssi::Resource::loadFragmentShader(ssg::GraphicsLoader *loader,
													 ssi::Resource::FragmentShaderId shader)
{
	const void *binary = NULL;
	uint32_t size = 0;
	switch(shader)
	{
	case SPRITE_2D_F:
		{
			binary = _binary_sampleutil_sprite2d_f_gxp_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_f_gxp_size;
			break;
		}
	case SPRITE_2D_2_F:
		{
			binary = _binary_sampleutil_sprite2d_2_f_gxp_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_2_f_gxp_size;
			break;
		}
	case SPRITE_2D_YUY2_F:
		{
			binary = _binary_sampleutil_sprite2d_yuy2_f_gxp_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_yuy2_f_gxp_size;
			break;
		}
	case FILL_2D_F:
		{
			binary = _binary_sampleutil_fill2d_f_gxp_start;
			size = (uintptr_t)_binary_sampleutil_fill2d_f_gxp_size;
			break;
		}
	case FILL_3D_F:
		{
			binary = _binary_sampleutil_fill3d_f_gxp_start;
			size = (uintptr_t)_binary_sampleutil_fill3d_f_gxp_size;
			break;
		}
	case POINT_SPRITE_2D_F:
		{
			binary = _binary_sampleutil_point_sprite2d_f_gxp_start;
			size = (uintptr_t)_binary_sampleutil_point_sprite2d_f_gxp_size;
			break;
		}
	case COLLADA_SKINNING_PHONG_F:
		{
			binary = _binary_sampleutil_collada_skinning_phong_f_gxp_start;
			size = (uintptr_t)_binary_sampleutil_collada_skinning_phong_f_gxp_size;
			break;
		}
	case WIREFRAME_F:
		{
			binary = _binary_sampleutil_wireframe_f_gxp_start;
			size = (uintptr_t)_binary_sampleutil_wireframe_f_gxp_size;
			break;
		}
	default:
		break;
	}

	if(binary == NULL){
		return NULL;
	}

	ssg::Program* program = NULL;
	int ret = loader->registerFragmentProgram(&program, binary, size);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	SCE_SAMPLE_UTIL_ASSERT(program != NULL);

	return program;
}



ssg::Texture* ssi::Resource::loadTexture(ssg::GraphicsLoader *loader,
										  ssi::Resource::TextureId texture)
{
	const void *binary = NULL;
	uint32_t size = 0;

	switch(texture)
	{
	case ASCII:
		{
			binary = (const void *)_binary_sampleutil_ascii_tga_start;
			size = (uintptr_t)_binary_sampleutil_ascii_tga_size;
			break;
		}
	default:
		break;
	}

	if(binary == NULL){
		return NULL;
	}

	ssg::Texture* tex = NULL;
	int ret = loader->createTextureFromMemory(&tex, binary, size);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	return tex;
}






#endif /* _SCE_TARGET_OS_PSP2 */



#if _SCE_TARGET_OS_WINDOWS


#include <resource/resource.h>

static std::string _sampleutil_sprite2d_v_cg;
static std::string _sampleutil_sprite2d_f_cg;
static std::string _sampleutil_sprite2d_2_v_cg;
static std::string _sampleutil_sprite2d_2_f_cg;
static std::string _sampleutil_sprite2d_yuy2_v_cg;
static std::string _sampleutil_sprite2d_yuy2_f_cg;
static std::string _sampleutil_sprite2d_ycbcr_v_cg;
static std::string _sampleutil_sprite2d_ycbcr_f_cg;
static std::string _sampleutil_fill2d_v_cg;
static std::string _sampleutil_fill2d_f_cg;
static std::string _sampleutil_fill3d_v_cg;
static std::string _sampleutil_fill3d_f_cg;
static std::string _sampleutil_wireframe_v_cg;
static std::string _sampleutil_wireframe_f_cg;
static std::string _sampleutil_point_sprite2d_v_cg;
static std::string _sampleutil_point_sprite2d_f_cg;
static void *_sampleutil_ascii_tga_start;
static uint32_t _sampleutil_ascii_tga_size;

static std::string colladaShaderF;
static std::string colladaShaderV;

static std::string findAsString(HINSTANCE moduleId, LPTSTR id, LPTSTR type)
{
	HRSRC r = FindResource(moduleId, id, type);
	HGLOBAL hg = LoadResource(moduleId, r);
	return std::string( (const char*)LockResource(hg),  SizeofResource(moduleId, r));
}

static int getPtrSize(HINSTANCE moduleId, LPTSTR id, LPTSTR type, void **ptr, uint32_t *size)
{
	HRSRC r = FindResource(moduleId, id, type);
	HGLOBAL hg = LoadResource(moduleId, r);
	*ptr = LockResource(hg);
	*size = SizeofResource(moduleId, r);
	return SCE_OK;
}


void ssi::Resource::initialize(HINSTANCE moduleId)
{
	_sampleutil_sprite2d_v_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_SPRITE2D_SHADER_V, _TEXT("CG"));
	_sampleutil_sprite2d_f_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_SPRITE2D_SHADER_F, _TEXT("CG"));
	_sampleutil_sprite2d_2_v_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_SPRITE2D_SHADER_2_V, _TEXT("CG"));
	_sampleutil_sprite2d_2_f_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_SPRITE2D_SHADER_2_F, _TEXT("CG"));
	_sampleutil_sprite2d_yuy2_v_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_SPRITE2D_YUY2_SHADER_V, _TEXT("CG"));
	_sampleutil_sprite2d_yuy2_f_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_SPRITE2D_YUY2_SHADER_F, _TEXT("CG"));
	_sampleutil_sprite2d_ycbcr_v_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_SPRITE2D_YCBCR_SHADER_F, _TEXT("CG"));
	_sampleutil_sprite2d_ycbcr_f_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_SPRITE2D_YCBCR_SHADER_V, _TEXT("CG"));
	_sampleutil_fill2d_v_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_FILL2D_SHADER_V, _TEXT("CG"));
	_sampleutil_fill2d_f_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_FILL2D_SHADER_F, _TEXT("CG"));
	_sampleutil_fill3d_v_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_FILL3D_SHADER_V, _TEXT("CG"));
	_sampleutil_fill3d_f_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_FILL3D_SHADER_F, _TEXT("CG"));
	_sampleutil_wireframe_v_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_WIREFRAME_SHADER_V, _TEXT("CG"));
	_sampleutil_wireframe_f_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_WIREFRAME_SHADER_F, _TEXT("CG"));
	_sampleutil_point_sprite2d_v_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_POINT_SPRITE2D_SHADER_V, _TEXT("CG"));
	_sampleutil_point_sprite2d_f_cg = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_POINT_SPRITE2D_SHADER_F, _TEXT("CG"));
	colladaShaderF = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_COLLADA_SHADER_F, _TEXT("CG"));
	colladaShaderV = findAsString(moduleId, (SCE_SAMPLE_LPTSTR)IDR_CG_COLLADA_SHADER_V, _TEXT("CG"));

	
	getPtrSize(moduleId, (LPTSTR)IDR_TEXTURE_SAMPLE_ASCII, _TEXT("TGA"), &_sampleutil_ascii_tga_start, &_sampleutil_ascii_tga_size);

}

ssg::Program* ssi::Resource::loadVertexShader(ssg::GraphicsLoader *loader,
													 ssi::Resource::VertexShaderId shader)
{
	ssg::Dx11::GraphicsLoaderDx11 *gc = dynamic_cast<ssg::Dx11::GraphicsLoaderDx11 *>(loader);

	std::string *code = NULL;
	switch(shader)
	{
	case SPRITE_2D_V:
		{
			code = &_sampleutil_sprite2d_v_cg;
			break;
		}
	case SPRITE_2D_2_V:
		{
			code = &_sampleutil_sprite2d_2_v_cg;
			break;
		}
	case SPRITE_2D_YUY2_V:
		{
			code = &_sampleutil_sprite2d_yuy2_v_cg;
			break;
		}
	case SPRITE_2D_YCBCR_V:
		{
			code = &_sampleutil_sprite2d_ycbcr_v_cg;
			break;
		}
	case FILL_2D_V:
		{
			code = &_sampleutil_fill2d_v_cg;
			break;
		}
	case FILL_3D_V:
		{
			code = &_sampleutil_fill3d_v_cg;
			break;
		}
	case WIREFRAME_V:
		{
			code = &_sampleutil_wireframe_v_cg;
			break;
		}
	case POINT_SPRITE_2D_V:
		{
			code = &_sampleutil_point_sprite2d_v_cg;
			break;
		}
	case COLLADA_SKINNING_PHONG_V:
		{
			code = &colladaShaderV;
			break;
		}
	default:
		break;
	}

	if(code == NULL){
		return NULL;
	}

	ssg::Program* program = NULL;
	program = gc->registerVertexProgramFromSource(code->c_str(), "main");
	SCE_SAMPLE_UTIL_ASSERT(program != NULL);
	return program;
}


ssg::Program* ssi::Resource::loadFragmentShader(ssg::GraphicsLoader *loader,
													 ssi::Resource::FragmentShaderId shader)
{
	ssg::Dx11::GraphicsLoaderDx11 *gc = dynamic_cast<ssg::Dx11::GraphicsLoaderDx11 *>(loader);

	std::string *code = NULL;
	switch(shader)
	{
	case SPRITE_2D_F:
		{
			code = &_sampleutil_sprite2d_f_cg;
			break;
		}
	case SPRITE_2D_2_F:
		{
			code = &_sampleutil_sprite2d_2_f_cg;
			break;
		}
	case SPRITE_2D_YUY2_F:
		{
			code = &_sampleutil_sprite2d_yuy2_f_cg;
			break;
		}
	case SPRITE_2D_YCBCR_F:
		{
			code = &_sampleutil_sprite2d_ycbcr_f_cg;
			break;
		}
	case FILL_2D_F:
		{
			code = &_sampleutil_fill2d_f_cg;
			break;
		}
	case FILL_3D_F:
		{
			code = &_sampleutil_fill3d_f_cg;
			break;
		}
	case WIREFRAME_F:
		{
			code = &_sampleutil_wireframe_f_cg;
			break;
		}
	case POINT_SPRITE_2D_F:
		{
			code = &_sampleutil_point_sprite2d_f_cg;
			break;
		}
	case COLLADA_SKINNING_PHONG_F:
		{
			code = &colladaShaderF;
			break;
		}
	default:
		break;
	}

	if(code == NULL){
		return NULL;
	}

	ssg::Program* program = NULL;

	program = gc->registerFragmentProgramFromSource(code->c_str(), "main");
	SCE_SAMPLE_UTIL_ASSERT(program != NULL);
	return program;
}


ssg::Texture* ssi::Resource::loadTexture(ssg::GraphicsLoader *loader,
										  ssi::Resource::TextureId texture)
{
	const void *binary = NULL;
	uint32_t size = 0;

	switch(texture)
	{
	case ASCII:
		{
			binary = (const void *)_sampleutil_ascii_tga_start;
			size = (uintptr_t)_sampleutil_ascii_tga_size;
			break;
		}
	default:
		break;
	}

	if(binary == NULL){
		return NULL;
	}

	ssg::Texture* tex = NULL;
	int ret = loader->createTextureFromMemory(&tex, binary, size);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	return tex;
}







#endif /* _SCE_TARGET_OS_WINDOWS */


#if _SCE_TARGET_OS_ORBIS

extern const char _binary_sampleutil_sprite2d_v_sb_start[];
extern const char _binary_sampleutil_sprite2d_f_sb_start[];
extern const char _binary_sampleutil_sprite2d_2_v_sb_start[];
extern const char _binary_sampleutil_sprite2d_2_f_sb_start[];
extern const char _binary_sampleutil_sprite2d_yuy2_v_sb_start[];
extern const char _binary_sampleutil_sprite2d_yuy2_f_sb_start[];
extern const char _binary_sampleutil_sprite2d_ycbcr_v_sb_start[];
extern const char _binary_sampleutil_sprite2d_ycbcr_f_sb_start[];
extern const char _binary_sampleutil_fill2d_v_sb_start[];
extern const char _binary_sampleutil_fill2d_f_sb_start[];
extern const char _binary_sampleutil_fill3d_v_sb_start[];
extern const char _binary_sampleutil_fill3d_f_sb_start[];
extern const char _binary_sampleutil_wireframe_v_sb_start[];
extern const char _binary_sampleutil_wireframe_f_sb_start[];
extern const char _binary_sampleutil_point_sprite2d_v_sb_start[];
extern const char _binary_sampleutil_point_sprite2d_f_sb_start[];

extern const char _binary_sampleutil_sprite2d_v_sb_size[];
extern const char _binary_sampleutil_sprite2d_f_sb_size[];
extern const char _binary_sampleutil_sprite2d_2_v_sb_size[];
extern const char _binary_sampleutil_sprite2d_2_f_sb_size[];
extern const char _binary_sampleutil_sprite2d_yuy2_v_sb_size[];
extern const char _binary_sampleutil_sprite2d_yuy2_f_sb_size[];
extern const char _binary_sampleutil_sprite2d_ycbcr_v_sb_size[];
extern const char _binary_sampleutil_sprite2d_ycbcr_f_sb_size[];
extern const char _binary_sampleutil_fill2d_v_sb_size[];
extern const char _binary_sampleutil_fill2d_f_sb_size[];
extern const char _binary_sampleutil_fill3d_v_sb_size[];
extern const char _binary_sampleutil_fill3d_f_sb_size[];
extern const char _binary_sampleutil_wireframe_v_sb_size[];
extern const char _binary_sampleutil_wireframe_f_sb_size[];
extern const char _binary_sampleutil_point_sprite2d_v_sb_size[];
extern const char _binary_sampleutil_point_sprite2d_f_sb_size[];

extern const char _binary_sampleutil_ascii_tga_start[];
extern const char _binary_sampleutil_ascii_tga_size[];

extern const char _binary_sampleutil_collada_skinning_phong_v_sb_start[];
extern const char _binary_sampleutil_collada_skinning_phong_v_sb_size[];
extern const char _binary_sampleutil_collada_skinning_phong_f_sb_start[];
extern const char _binary_sampleutil_collada_skinning_phong_f_sb_size[];

ssg::Program* ssi::Resource::loadVertexShader(ssg::GraphicsLoader *loader,
													 ssi::Resource::VertexShaderId shader)
{
	const void *binary = NULL;
	uint32_t size = 0;
	switch(shader)
	{
	case SPRITE_2D_V:
		{
			binary = _binary_sampleutil_sprite2d_v_sb_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_v_sb_size;
			break;
		}
	case SPRITE_2D_2_V:
		{
			binary = _binary_sampleutil_sprite2d_2_v_sb_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_2_v_sb_size;
			break;
		}
	case SPRITE_2D_YUY2_V:
		{
			binary = _binary_sampleutil_sprite2d_yuy2_v_sb_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_yuy2_v_sb_size;
			break;
		}
	case SPRITE_2D_YCBCR_V:
		{
			binary = _binary_sampleutil_sprite2d_ycbcr_v_sb_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_ycbcr_v_sb_size;
			break;
		}
	case FILL_2D_V:
		{
			binary = _binary_sampleutil_fill2d_v_sb_start;
			size = (uintptr_t)_binary_sampleutil_fill2d_v_sb_size;
			break;
		}
	case FILL_3D_V:
		{
			binary = _binary_sampleutil_fill3d_v_sb_start;
			size = (uintptr_t)_binary_sampleutil_fill3d_v_sb_size;
			break;
		}
	case WIREFRAME_V:
		{
			binary = _binary_sampleutil_wireframe_v_sb_start;
			size = (uintptr_t)_binary_sampleutil_wireframe_v_sb_size;
			break;
		}
	case POINT_SPRITE_2D_V:
		{
			binary = _binary_sampleutil_point_sprite2d_v_sb_start;
			size = (uintptr_t)_binary_sampleutil_point_sprite2d_v_sb_size;
			break;
		}
	case COLLADA_SKINNING_PHONG_V:
		{
			binary = _binary_sampleutil_collada_skinning_phong_v_sb_start;
			size = (uintptr_t)_binary_sampleutil_collada_skinning_phong_v_sb_size;
			break;
		}
	default:
		break;
	}

	if(binary == NULL){
		return NULL;
	}

	ssg::Program* program = NULL;
	int ret = loader->registerVertexProgram(&program, binary, size);
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	SCE_SAMPLE_UTIL_ASSERT(program != NULL);

	return program;
}


ssg::Program* ssi::Resource::loadFragmentShader(ssg::GraphicsLoader *loader,
													 ssi::Resource::FragmentShaderId shader)
{
	const void *binary = NULL;
	uint32_t size = 0;
	switch(shader)
	{
	case SPRITE_2D_F:
		{
			binary = _binary_sampleutil_sprite2d_f_sb_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_f_sb_size;
			break;
		}
	case SPRITE_2D_2_F:
		{
			binary = _binary_sampleutil_sprite2d_2_f_sb_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_2_f_sb_size;
			break;
		}
	case SPRITE_2D_YUY2_F:
		{
			binary = _binary_sampleutil_sprite2d_yuy2_f_sb_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_yuy2_f_sb_size;
			break;
		}
	case SPRITE_2D_YCBCR_F:
		{
			binary = _binary_sampleutil_sprite2d_ycbcr_f_sb_start;
			size = (uintptr_t)_binary_sampleutil_sprite2d_ycbcr_f_sb_size;
			break;
		}
	case FILL_2D_F:
		{
			binary = _binary_sampleutil_fill2d_f_sb_start;
			size = (uintptr_t)_binary_sampleutil_fill2d_f_sb_size;
			break;
		}
	case FILL_3D_F:
		{
			binary = _binary_sampleutil_fill3d_f_sb_start;
			size = (uintptr_t)_binary_sampleutil_fill3d_f_sb_size;
			break;
		}
	case WIREFRAME_F:
		{
			binary = _binary_sampleutil_wireframe_f_sb_start;
			size = (uintptr_t)_binary_sampleutil_wireframe_f_sb_size;
			break;
		}
	case POINT_SPRITE_2D_F:
		{
			binary = _binary_sampleutil_point_sprite2d_f_sb_start;
			size = (uintptr_t)_binary_sampleutil_point_sprite2d_f_sb_size;
			break;
		}
	case COLLADA_SKINNING_PHONG_F:
		{
			binary = _binary_sampleutil_collada_skinning_phong_f_sb_start;
			size = (uintptr_t)_binary_sampleutil_collada_skinning_phong_f_sb_size;
			break;
		}
	default:
		break;
	}

	if(binary == NULL){
		return NULL;
	}

	ssg::Program* program = NULL;
	int ret = loader->registerFragmentProgram(&program, binary, size);
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	SCE_SAMPLE_UTIL_ASSERT(program != NULL);

	return program;
}



ssg::Texture* ssi::Resource::loadTexture(ssg::GraphicsLoader *loader,
										  ssi::Resource::TextureId texture)
{
	const void *binary = NULL;
	uint32_t size = 0;

	switch(texture)
	{
	case ASCII:
		{
			binary = (const void *)_binary_sampleutil_ascii_tga_start;
			size = (uintptr_t)_binary_sampleutil_ascii_tga_size;
			break;
		}
	default:
		break;
	}

	if(binary == NULL){
		return NULL;
	}

	ssg::Texture* tex = NULL;
	int ret = loader->createTextureFromMemory(&tex, binary, size);
	(void)ret;
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	return tex;
}



#endif // _SCE_TARGET_OS_ORBIS
