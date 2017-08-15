/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/
#ifndef _SCE_SAMPLE_UTIL_GRAPHICS_PROGRAM_INTERNAL_DX11_H
#define _SCE_SAMPLE_UTIL_GRAPHICS_PROGRAM_INTERNAL_DX11_H

#include <scebase_common.h>
#if _SCE_TARGET_OS_WINDOWS

#include <sampleutil/graphics.h>
#include <sampleutil_internal.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{
			namespace Impl
			{
				class UniformBufferImpl;
				// -----------------------------------------------------------------------------
				// Program
				// -----------------------------------------------------------------------------
				class ParameterImpl : 	public Dx11::ParameterCg
				{
				private:
					std::string       m_name;
					ParameterSemantic m_semantic;
					uint32_t          m_semanticIndex;
					uint32_t          m_componentCount;
					uint32_t          m_arraySize;
					ParameterCategory m_category;
					ParameterType     m_type;
					uint32_t          m_containerIndex;
					uint32_t          m_resourceIndex;
				public:
					ParameterImpl(void) : m_category(kParameterCategoryUnknown) {}

					ParameterImpl(std::string       m_name,
								  ParameterSemantic m_semantic,
								  uint32_t          m_semanticIndex,
								  uint32_t          m_componentCount,
								  uint32_t          m_arraySize,
								  ParameterCategory m_category,
								  ParameterType     m_type,
								  uint32_t          m_containerIndex,
								  uint32_t          m_resourceIndex);

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


				class ProgramImpl : public Dx11::ProgramCg, public sce::SampleUtil::Impl::ResourceImpl
				{
				private:
					GraphicsLoader *m_loader;
					uint32_t m_numParameters;
					ParameterImpl *m_parameters;
					CGprogram m_cgProgram;
					UniformBufferImpl *m_defaultUniformBuffer;
					uint32_t m_defaultUniformBufferSize;
				public:
					ID3D10Blob* m_pBlob;

					ProgramImpl(void);
					ProgramImpl(GraphicsLoader *loader, CGprogram cgProgram, ID3D10Blob* pBlob);
					ProgramImpl(const ProgramImpl& from);
					~ProgramImpl(void);

					bool isValid(void) const{ return m_pBlob != NULL; } 

					uint32_t getParameterCount(void) const;
					const ParameterImpl* findParameterByName(const char *name) const;
					const ParameterImpl* findParameterBySemantic(ParameterSemantic semantic, uint32_t semanticIndex) const;
					const ParameterImpl* getParameter(uint32_t index) const;
					void getAllParameters(std::vector<const Parameter*> &params) const;

					UniformBufferImpl	*getDefaultUniformBuffer(void) const
					{
						return m_defaultUniformBuffer;
					}

					std::string getResourceType(void) const
					{
						return "Graphics::Program";
					}


				private:
					void _getAllParameters(std::vector<ParameterImpl> &params) const;
				};

				class VertexProgramImpl : public VertexProgram, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					VertexProgramImpl(void)
					{
						m_programId = NULL;
						m_layout    = NULL;
						m_vertexShader = NULL;
					}
					const ProgramImpl* m_programId;
					ID3D11VertexShader *m_vertexShader;
					ID3D11InputLayout *m_layout;
					uint32_t strides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]; 
					uint32_t offsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT]; 
					uint32_t numStreams;
					
					virtual ~VertexProgramImpl(void)
					{
						if(m_layout != NULL){
						   m_layout->Release();
						   m_layout = NULL;
						}
						if(m_vertexShader != NULL){
							m_vertexShader->Release();
							m_vertexShader = NULL;
						}
					}

					std::string getResourceType(void) const
					{
						return "Graphics::VertexProgram";
					}

				};

				class FragmentProgramImpl : public FragmentProgram, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					FragmentProgramImpl(void)
					{
						m_programId = NULL;
						m_blendState = NULL;
						m_pixelShader = NULL;
					}

					const ProgramImpl* m_programId;
					ID3D11BlendState *m_blendState;
					ID3D11PixelShader *m_pixelShader;
					virtual ~FragmentProgramImpl(void)
					{
						if(m_blendState != NULL){
							m_blendState->Release();
							m_blendState = NULL;
						}
						if(m_pixelShader != NULL){
							m_pixelShader->Release();
							m_pixelShader = NULL;
						}
					}
					std::string getResourceType(void) const
					{
						return "Graphics::FragmentProgram";
					}

				};

				class ComputeProgramImpl : public ComputeProgram, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					ComputeProgramImpl(void)
					{
						m_programId = NULL;
						m_computeShader = NULL;
					}

					const ProgramImpl* m_programId;
					ID3D11ComputeShader *m_computeShader;
					virtual ~ComputeProgramImpl(void)
					{
						if(m_computeShader != NULL){
							m_computeShader->Release();
							m_computeShader = NULL;
						}
					}
					std::string getResourceType(void) const
					{
						return "Graphics::ComputeProgram";
					}

				};

			}
		}
	}
}


#endif // _SCE_TARGET_OS_WINDOWS
#endif // _SCE_SAMPLE_UTIL_GRAPHICS_PROGRAM_INTERNAL_DX11_H
