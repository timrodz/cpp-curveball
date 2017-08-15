/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_PROGRAM_INTERNAL_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_PROGRAM_INTERNAL_H

#include <gnm.h>
#include <gnmx.h>
#include <sampleutil/graphics.h>
#include "heap_gnm.h"
#include "sampleutil_internal.h"

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class GraphicsLoaderImpl;
				class GraphicsContextImpl;
				class GpuBufferState;
			}
			class GnmParameter : public Parameter
			{
			public:
				ParameterCategory m_category;
				std::string m_name;
				uint32_t m_size;
				uint32_t m_semanticIndex;
				ParameterSemantic m_semantic;
				uint32_t m_resourceIndex;
				uint32_t m_containerIndex;
				uint32_t m_componentCount;
				uint32_t m_offset;
				uint8_t	m_elementType; // PSSL_TYPE

				static 	ParameterSemantic getSemantic(std::string semantic);
				static std::string getParamName(const std::string sbparam);


				GnmParameter(void)
				{
					m_category = kParameterCategoryUnknown;
					m_semantic = kParameterSemanticNone;
					m_semanticIndex = 0;
					m_offset = 0;
					m_elementType = Shader::Binary::PsslType::kTypeTypeEnd;
					m_containerIndex = 0;
				}

				void initializeAsUniform(const sce::Shader::Binary::Element *e, uint32_t containerIndex);

				void initializeAsAttribute(const sce::Shader::Binary::Attribute *a);

				void initializeAsSampler(const sce::Shader::Binary::Buffer *b);

				void dump(void) const;
				uint32_t getBufferIndex(void) const;


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


			};


			class GnmProgram 
			{
			public:
				enum Type {
					TYPE_VERTEX,
					TYPE_PIXEL,
					TYPE_COMPUTE,
					TYPE_LOCAL,
					TYPE_HULL,
					TYPE_EXPORT,
					TYPE_GEOMETRY
				};

				Type m_type;

				union {
					Gnmx::VsShader *m_vsShader;
					Gnmx::PsShader *m_psShader;
					Gnmx::CsShader *m_csShader;
					Gnmx::LsShader *m_lsShader;
					Gnmx::HsShader *m_hsShader;
					Gnmx::EsShader *m_esShader;
					Gnmx::GsShader *m_gsShader;
				};

				Impl::MemBlock m_memblk;
				sce::Shader::Binary::Program m_shaderBinaryProgram;

				std::vector<GnmParameter> m_params;

				Impl::MemBlock m_fetchShader;
				uint32_t m_shaderModifier;
				
				uint32_t m_defaultUniformBufferSize;

				Impl::GraphicsLoaderImpl* m_loader;
				const void *m_binary;
				uint32_t m_binarySize;
			private:
				int initParams(void);
				int initializeVertexShader(Impl::GraphicsLoaderImpl* loader);
				int initializePixelShader(Impl::GraphicsLoaderImpl* loader);
				int initializeComputeShader(Impl::GraphicsLoaderImpl* loader);
			public:
				GnmProgram(void)
				{
					m_binary = NULL;
				}

				int initialize(Impl::GraphicsLoaderImpl* loader, const void *binary, uint32_t binarySize);

				int finalize(void);

				void dump(void)
				{
					printf(" -- dump params  ---\n");
					for(uint32_t i=0; i<m_params.size(); i++){
						m_params.at(i).dump();
					}
					printf(" -- dump params end ---\n");
				}

				bool isValid(void) const;

				uint32_t getDefaultBufferSize(void) const;

				void getAllParameters(std::vector<const Parameter*> &params) const;

				const Parameter* findParameterByName(const char* name) const;

				const Parameter* findParameterBySemantic(ParameterSemantic semantic, uint32_t semanticIndex) const;
				uint32_t getParameterCount(void) const;
				const Parameter* getParameter(uint32_t index) const;
			};

			class ProgramImpl : public Program, public sce::SampleUtil::Impl::ResourceImpl
			{
				friend class Impl::GraphicsContextImpl;
			public:
				const GnmProgram *m_program;
				Impl::GraphicsLoaderImpl *m_loader;
			public:
				virtual ~ProgramImpl(void);

				ProgramImpl(const GnmProgram *program, Impl::GraphicsLoaderImpl* loader);

				ProgramImpl(const ProgramImpl& from);
				ProgramImpl &operator=(const ProgramImpl &from);

				bool isValid(void) const;
				uint32_t getParameterCount(void) const;
				const Parameter* findParameterByName(const char *name) const;
				const Parameter* findParameterBySemantic(ParameterSemantic semantic, uint32_t semanticIndex) const;
				const Parameter* getParameter(uint32_t index) const;
				void getAllParameters(std::vector<const Parameter*> &params) const;
			   
				std::string getResourceType(void) const
				{
					return "Graphics::Program";
				}


			};

			namespace Impl
			{

				class VertexProgramImpl : public VertexProgram, public sce::SampleUtil::Impl::ResourceImpl
				{
					friend class Impl::GraphicsLoaderImpl;
					friend class Impl::GraphicsContextImpl;
					friend class Impl::GpuBufferState;
				public:
					VertexProgramImpl(void);

					virtual ~VertexProgramImpl(void);

					std::string getResourceType(void) const
					{
						return "Graphics::VertexProgram";
					}


				private:
					struct AttrParam
					{
						VertexAttribute attr;
						int32_t        reg;
					};
					const Program* m_programId;
					AttrParam  *m_attributes;
					uint32_t m_numAttrs;
					
					VertexStream *m_streams;
					uint32_t m_numStreams;
				};
				
				class FragmentProgramImpl : public FragmentProgram, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					const Program* m_programId;
					uint32_t                  m_colorMask;
					sce::Gnm::BlendMultiplier colorSrc;
					sce::Gnm::BlendFunc   colorComb;
					sce::Gnm::BlendMultiplier colorDest;
					sce::Gnm::BlendMultiplier alphaSrc;
					sce::Gnm::BlendFunc   alphaComb;
					sce::Gnm::BlendMultiplier alphaDest;

					virtual ~FragmentProgramImpl(void){}

					std::string getResourceType(void) const
					{
						return "Graphics::FragmentProgram";
					}

				};

				class ComputeProgramImpl : public ComputeProgram, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					const Program* m_programId;
					sce::Gnmx::CsShader *m_csShader;
					virtual ~ComputeProgramImpl(void){}
					std::string getResourceType(void) const
					{
						return "Graphics::ComputeProgram";
					}

				};
				
			}




		}
	}
}


#endif /* _SCE_SAMPLE_UTIL_GRAPHICS_PROGRAM_INTERNAL_H */


