/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_PLATFORM_DX11_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_PLATFORM_DX11_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS
//#pragma warning(disable:4251)

#include <sampleutil/sampleutil_common.h>

#define NOMINMAX
#include <dxgi.h>
#include <d3d11.h> 
#include <Cg/cg.h> 
#include <Cg/cgD3D11.h>
#include <assert.h>
#include <malloc.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Dx11
			{

				class GraphicsLoaderDx11 :  public GraphicsLoader
				{
				public:

					virtual Program* registerVertexProgramFromSourceFile(const char *path, const char *entry) = 0;
					virtual Program* registerFragmentProgramFromSourceFile(const char *path, const char *entry) = 0;
					virtual Program* registerVertexProgramFromSource(const char *programSource, const char *entry) = 0;
					virtual Program* registerFragmentProgramFromSource(const char *programSource, const char *entry) = 0;

					virtual ID3D11Device *getDevice(void) const = 0;

					virtual ~GraphicsLoaderDx11(void){}
				};

				class GraphicsContextDx11 :  public GraphicsLoaderDx11, public GraphicsContext
				{
				public:
					virtual int readBuffer(void *dst, const BufferInterface* from, uint64_t offset, uint64_t numBytes) = 0;
					virtual ~GraphicsContextDx11(void){}
				};

				class ParameterCg : public Parameter
				{
				public:
					static const ParameterCg *downcast(const Parameter* param);

					virtual ~ParameterCg(void){}
				};


				class ProgramCg : public Program
				{
				public:
					static const ProgramCg *downcast(const Program* param);

					virtual ~ProgramCg(void){}
				};
				
				int createGraphicsLoader(GraphicsLoader **outGraphicsLoader, 
					ID3D11Device *device,
					CGprofile cgVertexProfile=CG_PROFILE_VS_4_0,
					CGprofile cgFragmentProfile=CG_PROFILE_PS_4_0);
			}
		}
	}
}


#endif

#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_PLATFORM_DX11_H */


