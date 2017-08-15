/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_PROGRAM_INTERNAL_GXM_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_PROGRAM_INTERNAL_GXM_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/graphics.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class GraphicsLoaderImpl;

				class VertexProgramImpl : public VertexProgram, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					SceGxmShaderPatcher *m_gxmShaderPatcher;
					SceGxmVertexProgram*          m_gxmVertexProgram;
					const Program*                     m_programId;
					virtual ~VertexProgramImpl(void)
					{
						int ret = sceGxmShaderPatcherReleaseVertexProgram( m_gxmShaderPatcher, this->m_gxmVertexProgram);
						SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
					}
					std::string getResourceType(void) const
					{
						return "Graphics::VertexProgram";
					}

				};

				class FragmentProgramImpl : public FragmentProgram, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					SceGxmShaderPatcher *m_gxmShaderPatcher;
					const SceGxmProgram *        m_vertexProgram;
					const Program*                     m_programId;
					SceGxmFragmentProgram*        m_gxmFragmentPrograms[2][3];// SceGxmOutputRegisterSize * SceGxmMultisampleMode
					virtual ~FragmentProgramImpl(void)
					{
						for(uint32_t i=0; i<2; i++){
							for(uint32_t j=0; j<3; j++){
								if(this->m_gxmFragmentPrograms[i][j] != NULL){
									int ret = sceGxmShaderPatcherReleaseFragmentProgram( m_gxmShaderPatcher, this->m_gxmFragmentPrograms[i][j]);
									SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
								}
							}
						}
					}
					std::string getResourceType(void) const
					{
						return "Graphics::FragmentProgram";
					}

				};



				class ParameterImpl : public Gxm::ParameterGxm
				{
					const SceGxmProgramParameter* m_gxmParam;
				public:
					ParameterImpl(void) : m_gxmParam(NULL) {}
					ParameterImpl(const SceGxmProgramParameter* gxmParam)  : m_gxmParam(gxmParam){}
					operator const SceGxmProgramParameter*(void) const { return m_gxmParam;}
					virtual const SceGxmProgramParameter* getGxmParameter(void) const { return m_gxmParam;}

					ParameterSemantic getSemantic(void) const;

					uint32_t getComponentCount(void) const;
					uint32_t getArraySize(void) const;

					bool isUseDefaultBuf(void) const;
					uint32_t getResourceIndex(void) const;
					uint32_t getContainerIndex(void) const;
					uint32_t getSemanticIndex(void) const;
					int setUniformDataF(void *uniformBuffer, const float *sourceData) const;
					const char *getName(void) const;
					ParameterCategory getCategory(void) const;
					bool isValid(void) const;
					ParameterType getType(void) const;

				private:
					uint32_t getComponentCountAll(void) const;

					bool isSampler(void) const;
					bool isUniform(void) const;
					bool isAttribute(void) const;

				};



				class ProgramImpl : public Gxm::ProgramGxm, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					uint32_t m_numParameters;
					ParameterImpl *m_parameters;

				private:
					SceGxmShaderPatcherId m_patcherId;
					GraphicsLoaderImpl* m_loader;
				public:
					ProgramImpl(void);
					ProgramImpl(SceGxmShaderPatcherId patcherId, GraphicsLoaderImpl* loader);

					ProgramImpl(const ProgramImpl& from);
					ProgramImpl &operator=(const ProgramImpl &from);

					~ProgramImpl(void);

					SceGxmShaderPatcherId getPatcherId(void) const
					{
						return m_patcherId; 
					}

					operator SceGxmShaderPatcherId(void){ return m_patcherId; }
					operator SceGxmShaderPatcherId(void) const { return m_patcherId; }
					bool isValid(void) const{ return m_patcherId != NULL; }

					virtual const SceGxmProgram *getGxmProgram(void) const
					{
						return sceGxmShaderPatcherGetProgramFromId(m_patcherId);
					}
					virtual SceGxmShaderPatcherId getGxmShaderPatcherId(void) const 
					{ 
						return m_patcherId; 
					}
					uint32_t getParameterCount(void) const;
					const ParameterImpl* findParameterByName(const char *name) const;
					const ParameterImpl* findParameterBySemantic(ParameterSemantic semantic, uint32_t semanticIndex) const;
					const ParameterImpl* getParameter(uint32_t index) const;
					void getAllParameters(std::vector<const Parameter*> &params) const;
					std::string getResourceType(void) const
					{
						return "Graphics::Program";
					}

				private:
					void _getAllParameters(std::vector<ParameterImpl> &params) const;
				};

			}
		}
	}
}



#endif // _SCE_TARGET_OS_PSP2
#endif //_SCE_SAMPLE_UTIL_GRAPHICS_PROGRAM_INTERNAL_GXM_H
