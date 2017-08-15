/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _COLLADA_TRANSFORM_H
#define _COLLADA_TRANSFORM_H

#include <sampleutil/graphics.h>
#include <vectormath.h>
#include <libdbg.h>
#include <string>


#ifdef __psp2__
#  include <sceconst.h>
#endif

#ifndef M_PI
#define M_PI SCE_MATH_PI
#endif



namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{

			namespace Collada
			{
				namespace Internal
				{

					/*! @brief ノードの変換要素  */
					class TransformationElement
					{
					public:
						typedef enum { TYPE_MATRIX, TYPE_ROTATE, TYPE_TRANSLATE, TYPE_SCALE, TYPE_SKEW } TransformationType;

						virtual TransformationType getType(void) const = 0;
						virtual sce::Vectormath::Simd::Aos::Matrix4 asMatrix(void) = 0;
						virtual TransformationElement* duplicate(void) = 0;
						virtual std::string getSid(void) = 0;
						virtual void setElem(int col, int row, float value)
						{
							(void)col;
							(void)row;
							(void)value;
							SCE_SAMPLE_UTIL_ASSERT(0);
						}
						virtual void setAngle(float value)
						{
							(void)value;
							SCE_SAMPLE_UTIL_ASSERT(0);
						}
						virtual void setVector(sce::Vectormath::Simd::Aos::Vector3_arg v)
						{
							(void)v;
							SCE_SAMPLE_UTIL_ASSERT(0);
						}
						virtual void setVectorX(float v)
						{
							(void)v;
							SCE_SAMPLE_UTIL_ASSERT(0);
						}
						virtual void setVectorY(float v)
						{
							(void)v;
							SCE_SAMPLE_UTIL_ASSERT(0);
						}
						virtual void setVectorZ(float v)
						{
							(void)v;
							SCE_SAMPLE_UTIL_ASSERT(0);
						}

						virtual void setMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg matrix)
						{
							(void)matrix;
							SCE_SAMPLE_UTIL_ASSERT(0);
						}
						virtual ~TransformationElement(void){}

					};

					/*! @brief マトリックスタイプの変換要素  */
					struct Matrix : public TransformationElement
					{
						std::string m_sid;

						sce::Vectormath::Simd::Aos::Matrix4 m_matrix;


						Matrix(void){}
						Matrix(sce::Vectormath::Simd::Aos::Matrix4_arg matrix)
						{
							m_matrix = matrix;
						}

						virtual TransformationType getType(void) const  { return TYPE_MATRIX; }

						virtual sce::Vectormath::Simd::Aos::Matrix4 asMatrix(void)
						{
							return m_matrix;
						}

						virtual TransformationElement* duplicate(void)
						{
							Matrix *ret = new Matrix;
							ret->m_sid = m_sid;
							ret->m_matrix = m_matrix;
							return ret;
						}
						virtual std::string getSid(void)
						{
							return m_sid;
						}
						virtual void setElem(int col, int row, float value)
						{
							m_matrix.setElem(col, row, value);
						}

						virtual void setMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg matrix)
						{
							m_matrix = matrix;
						}


						virtual ~Matrix(void){}

					};

					/*! @brief 回転タイプの変換要素  */
					struct Rotate : public TransformationElement
					{
						std::string m_sid;

						sce::Vectormath::Simd::Aos::Vector3 m_axis;
						float m_angle_degree;

						virtual TransformationType getType(void) const  { return TYPE_ROTATE; }
						virtual std::string getSid(void)
						{
							return m_sid;
						}
						virtual sce::Vectormath::Simd::Aos::Matrix4 asMatrix(void)
						{
							return sce::Vectormath::Simd::Aos::Matrix4::rotation((float)((m_angle_degree / 180.0f) * M_PI), m_axis);
						}

						virtual TransformationElement* duplicate(void)
						{
							Rotate *ret = new Rotate;
							ret->m_sid = m_sid;
							ret->m_axis = m_axis;
							ret->m_angle_degree = m_angle_degree;
							return ret;
						}

						virtual void setAngle(float value)
						{
							m_angle_degree = value;
						}

						virtual ~Rotate(void){}

					};

					/*! @brief 平行移動タイプの変換要素  */
					struct Translate : public TransformationElement
					{
						sce::Vectormath::Simd::Aos::Vector3 m_offset;
						std::string m_sid;


						Translate(void)
						{
						}

						virtual TransformationType getType(void) const  { return TYPE_TRANSLATE; }
						virtual std::string getSid(void)
						{
							return m_sid;
						}
						virtual sce::Vectormath::Simd::Aos::Matrix4 asMatrix(void)
						{
							return sce::Vectormath::Simd::Aos::Matrix4::translation(m_offset);
						}

						virtual TransformationElement* duplicate(void)
						{
							Translate *ret = new Translate;

							ret->m_sid = m_sid;
							ret->m_offset = m_offset;
							return ret;
						}

						virtual void setVector(sce::Vectormath::Simd::Aos::Vector3_arg v)
						{
							m_offset = v;
						}
						virtual void setVectorX(float v)
						{
							m_offset.setX(v);
						}
						virtual void setVectorY(float v)
						{
							m_offset.setY(v);
						}
						virtual void setVectorZ(float v)
						{
							m_offset.setZ(v);
						}


						virtual ~Translate(void) {}

					};

					/*! @brief 拡大タイプの変換要素  */
					struct Scale  : public TransformationElement
					{
						std::string m_sid;
						sce::Vectormath::Simd::Aos::Vector3 m_scale;

						virtual TransformationType getType(void) const  { return TYPE_SCALE; }
						virtual std::string getSid(void)
						{
							return m_sid;
						}
						virtual sce::Vectormath::Simd::Aos::Matrix4 asMatrix(void)
						{
							return sce::Vectormath::Simd::Aos::Matrix4::scale(m_scale);
						}

						virtual TransformationElement* duplicate(void)
						{
							Scale *ret = new Scale;

							ret->m_sid = m_sid;
							ret->m_scale = m_scale;
							return ret;
						}

						virtual void setVector(sce::Vectormath::Simd::Aos::Vector3_arg v)
						{
							m_scale = v;
						}
						virtual void setVectorX(float v)
						{
							m_scale.setX(v);
						}
						virtual void setVectorY(float v)
						{
							m_scale.setY(v);
						}
						virtual void setVectorZ(float v)
						{
							m_scale.setZ(v);
						}


						virtual ~Scale(void) {}


					};

					struct Skew  : public TransformationElement
					{
						std::string m_sid;

						float m_angle_degree;
						sce::Vectormath::Simd::Aos::Vector3 m_vector_a;
						sce::Vectormath::Simd::Aos::Vector3 m_vector_b;
					public:

						virtual TransformationType getType(void) const  { return TYPE_SKEW; }
						virtual std::string getSid(void)
						{
							return m_sid;
						}
						virtual sce::Vectormath::Simd::Aos::Matrix4 asMatrix(void)
						{
							// TODO
							return sce::Vectormath::Simd::Aos::Matrix4::identity();
						}

						virtual TransformationElement* duplicate(void)
						{
							Skew *ret = new Skew;

							ret->m_sid = m_sid;
							ret->m_angle_degree = m_angle_degree;
							ret->m_vector_a = m_vector_a;
							ret->m_vector_b = m_vector_b;
							return ret; 
						}


						virtual ~Skew(void)
						{}
					};


				}
			}
		}
	}
}


#endif /* _COLLADA_TRANSFORM_H */