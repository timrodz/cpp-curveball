/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_GEOMETRY_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_GEOMETRY_H

#include <sampleutil/graphics/collada/collada_base.h>
#include <string.h>
#include <string>


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
					class LibraryMeshes;
					class MeshImpl;
				}

				/*!
				 * @~English
				 * @brief Mesh part class 
				 * @details This is the mesh part class. 
				 * @~Japanese
				 * @brief メッシュパートクラス 
				 * @details メッシュパートクラスです。 
				 */
				class SCE_SAMPLE_UTIL_API MeshPart
				{
					friend class Internal::MeshImpl;
				public:
					/*!
					 * @~English
					 * @brief Gets the information of an attribute parameter 
					 * 
					 * @details This gets the information of the vertex buffer specified by a semantic and semantic index. streamIndex is set to 0. 
					 * @param semantic Semantic of an attribute parameter
					 * @param semanticIndex Semantic index of attribute parameters
					 * @return The information of an attribute parameter. If not found, a VertexAttribute in which everything is set to 0 will be returned. 
					 * @~Japanese
					 * @brief アトリビュートパラメータの情報の取得 
					 * 
					 * @details セマンティックとセマンティックインデックスで指定された頂点バッファの情報を取得します。streamIndexは0が設定されます。 
					 * @param semantic アトリビュートパラメータのセマンティック
					 * @param semanticIndex アトリビュートパラメータのセマンティックインデックス
					 * @return アトリビュートパラメータの情報。見つからなかった場合は、全て0が設定されたVertexAttributeが返ります。 
					 */
					Graphics::VertexAttribute getVertexAttribute(Graphics::ParameterSemantic semantic, int semanticIndex=0) const;
					/*!
					 * @~English
					 * @brief Gets the information of an attribute parameter 
					 * @details This gets the information of a vertex buffer. streamIndex is set to 0. 
					 * @param index Index
					 * @return The information of an attribute parameter. If index is equal to or larger than the number of attribute parameters, VertexAttribute in which everything is set to 0 will be returned. 
					 * @~Japanese
					 * @brief アトリビュートパラメータの情報の取得 
					 * @details 頂点バッファの情報を取得します。streamIndexは0が設定されます。 
					 * @param index インデックス
					 * @return アトリビュートパラメータの情報。indexがアトリビュートパラメータ数以上の場合は、全て0が設定されたVertexAttributeが返ります。 
					 */
					Graphics::VertexAttribute getVertexAttribute(uint32_t index) const;
					/*!
					 * @~English
					 * @brief Gets the number of attribute parameters. 
					 * @details This gets the number of attribute parameters. 
					 * @return Number of attribute parameters. 
					 * @~Japanese
					 * @brief アトリビュートパラメータ数の取得 
					 * @details アトリビュートパラメータ数を取得します。 
					 * @return アトリビュートパラメータ数。 
					 */
					uint32_t getNumVertexAttributes(void) const;
					/*!
					 * @~English
					 * @brief Gets the stride of vertex data. 
					 * @details This gets the stride of vertex data. 
					 * @return Stride of vertex data 
					 * @~Japanese
					 * @brief 頂点データのストライド取得 
					 * @details 頂点データのストライドを取得します。 
					 * @return 頂点データのストライド 
					 */
					uint32_t getStride(void) const;
					/*!
					 * @~English
					 * @brief Gets a vertex buffer. 
					 * @details This gets a vertex buffer. 
					 * @return Vertex buffer 
					 * @~Japanese
					 * @brief 頂点バッファの取得 
					 * @details 頂点バッファを取得します。 
					 * @return 頂点バッファ 
					 */
					Graphics::VertexBuffer *getVertexBuffer(void) const;
					/*!
					 * @~English
					 * @brief Gets an index buffer. 
					 * @details This gets an index buffer. 
					 * @return Index buffer. 
					 * @~Japanese
					 * @brief インデックスバッファの取得 
					 * @details インデックスバッファを取得します。 
					 * @return インデックスバッファ 
					 */
					Graphics::IndexBuffer *getIndexBuffer(void) const;
					/*!
					 * @~English
					 * @brief Gets the number of indices. 
					 * @details This gets the number of indices. 
					 * @return Number of indices 
					 * @~Japanese
					 * @brief インデックス数の取得 
					 * @details インデックス数を取得します。 
					 * @return インデックス数 
					 */
					uint32_t getNumIndices(void) const;
					/*!
					 * @~English
					 * @brief Gets the number of vertexes. 
					 * @details This gets the number of vertexes. 
					 * @return Number of vertexes 
					 * @~Japanese
					 * @brief 頂点数の取得 
					 * @details 頂点数を取得します。 
					 * @return 頂点数 
					 */
					uint32_t getNumVertices(void) const;
					/*!
					 * @~English
					 * @brief Returns the reference name of a material. 
					 * 
					 * @details This returns the reference name of a material to be used for drawing this mesh part. The association between the reference name of a material and the material is specified by InstanceMesh. 
					 * @return The reference name of a material 
					 * @~Japanese
					 * @brief マテリアルの参照名を返す 
					 * 
					 * @details このメッシュパートの描画に使用するマテリアルの参照名を返します。マテリアルの参照名とマテリアルの対応はInstanceMeshで指定されます。 
					 * @return マテリアルの参照名 
					 */
					std::string getMaterial(void) const;
					/*!
					 * @~English
					 * @brief Confirms whether a skin is included. 
					 * 
					 * @details This confirms whether a skin is included in this mesh part. 
					 * @return true will be returned if a skin is included, and false if not. 
					 * @~Japanese
					 * @brief スキンが含まれるかの確認 
					 * 
					 * @details このメッシュパートにスキンが含まれるか確認します。 
					 * @return スキンが含まれる場合はtrue、そうでない場合はfalseが返ります。 
					 */
					bool hasSkin(void) const;

					void serialize(Internal::Serializer *serializer);

				private:
					int initialize(ColladaLoader *loader,
						Internal::SkinMeshPart *skinMeshPart, 
						Internal::geometry::Triangles* triangles);
					int finalize(void);

					MeshPart(void);
					~MeshPart(void);
#if _SCE_TARGET_OS_WINDOWS
#pragma warning(push,3)
#pragma warning(disable: 4251)
#endif
					std::string m_material;
					Graphics::VertexBuffer *m_vertexBuffer;
					uint32_t m_numVertices;
					Graphics::IndexBuffer  *m_indexBuffer;
					uint32_t m_numIndices;
					Graphics::VertexAttribute *m_vertexAttributes;
					uint32_t m_numVertexAttributes;
					uint32_t m_stride;
#if _SCE_TARGET_OS_WINDOWS
#pragma warning(pop)
#endif
					bool m_hasSkin;
				};

				/*!
				 * @~English
				 * @brief Mesh class 
				 * @details This is the mesh class. 
				 * @~Japanese
				 * @brief メッシュクラス 
				 * @details メッシュクラスです。 
				 */
				class SCE_SAMPLE_UTIL_API Mesh
				{
				public:
					/*!
					 * @~English
					 * @brief Type of the name of a joint name array 
					 * 
					 * @details This is the type to indicate the type of the value of the joint name array which can be acquired by getJointNameArray(). 
					 * @~Japanese
					 * @brief ジョイント名配列の名前の種類 
					 * 
					 * @details getJointNameArray()で取得できるジョイント名配列の値の種類を示す型です。 
					 */
					enum JointNameType { 
						/*!
						 * @~English
						 * @brief getJointNameArray() is the array of the ID of a joint node 
						 * 
						 * @~Japanese
						 * @brief getJointNameArray()はジョイントノードのIDの列 
						 * 
						 */
						kJointNameTypeId,
						/*!
						 * @~English
						 * @brief getJointNameArray() is the array of the SID of a joint node 
						 * 
						 * @~Japanese
						 * @brief getJointNameArray()はジョイントノードのSIDの列 
						 * 
						 */
						kJointNameTypeSid 
					};

					/*!
					 * @~English
					 * @brief Confirms whether a skin is included in a mesh. 
					 * 
					 * @details This confirms whether a skin is included in a mesh. 
					 * @return  true will be returned if a skin is included, and false if not. 
					 * @~Japanese
					 * @brief メッシュにスキンが含まれるかの確認 
					 * 
					 * @details メッシュにスキンが含まれるか確認します。 
					 * @return  スキンが含まれる場合はtrue, そうでなければfalseが返ります。 
					 */
					virtual bool hasSkin(void) const = 0;

					/*!
					 * @~English
					 * @brief Gets a mesh part 
					 * @details This gets a mesh part. 
					 * @param index Index of mesh parts
					 * @return Mesh part. If index is equal to or larger than the number of mesh parts, NULL will be returned. 
					 * @~Japanese
					 * @brief メッシュパートの取得 
					 * @details メッシュパートを取得します。 
					 * @param index メッシュパートのインデックス
					 * @return メッシュパート。indexがメッシュパート数以上の場合はNULLが返ります。 
					 */
					virtual const MeshPart *getMeshPart(uint32_t index) const = 0;
					/*!
					 * @~English
					 * @brief Gets the number of mesh parts 
					 * @details This gets the number of mesh parts. 
					 * @return Number of mesh parts 
					 * @~Japanese
					 * @brief メッシュパート数の取得 
					 * @details メッシュパート数を取得します。 
					 * @return メッシュパート数 
					 */
					virtual uint32_t getNumMeshParts(void) const = 0;

					/*!
					 * @~English
					 * @brief Get the controller ID 
					 * @details This function obtains the controller ID. 
					 * @return Controller ID 
					 * @~Japanese
					 * @brief コントローラーIDの取得 
					 * @details コントローラーIDを取得します。 
					 * @return コントローラーID 
					 */
					virtual std::string getControllerId(void) const = 0;
					/*!
					 * @~English
					 * @brief Gets a geometry ID. 
					 * @details This gets a geometry ID. 
					 * @return Geometry ID 
					 * @~Japanese
					 * @brief ジオメトリIDの取得 
					 * @details ジオメトリIDを取得します。 
					 * @return ジオメトリID 
					 */
					virtual std::string getGeometryId(void) const = 0;

					/*!
					 * @~English
					 * @brief Gets the type of the name of a joint name array. 
					 * @details This gets the type of the name of a joint name array. 
					 * @return Type of the name of a joint name array 
					 * @~Japanese
					 * @brief ジョイント名配列の名前の種類の取得 
					 * @details ジョイント名配列の名前の種類を取得します。 
					 * @return ジョイント名配列の名前の種類 
					 */
					virtual JointNameType getJointNameType(void) const = 0;

					/*!
					 * @~English
					 * @brief Gets a joint count. 
					 * @details This gets a joint count. 
					 * @return Joint count 
					 * @~Japanese
					 * @brief ジョイント数の取得 
					 * @details ジョイント数を取得します。 
					 * @return ジョイント数 
					 */
					virtual uint32_t getNumJoints(void) const = 0;
					/*!
					 * @~English
					 * @brief Gets a joint name. 
					 * @details This gets a joint name. 
					 * @param jointIndex Joint index
					 * @return Joint name 
					 * @~Japanese
					 * @brief ジョイント名の取得 
					 * @details ジョイント名取得します。 
					 * @param jointIndex ジョイントのインデックス
					 * @return ジョイント名 
					 */
					virtual std::string getJointName(uint32_t jointIndex) const = 0;
					

					/*!
					 * @~English
					 * @brief Gets an inverse bind matrix. 
					 * @details This gets an inverse bind matrix. 
					 * @return Array of an inverse bind matrix 
					 * @~Japanese
					 * @brief インバースバインドマトリックス行列の取得 
					 * @details インバースバインドマトリックス行列を取得します。 
					 * @return インバースバインドマトリックスの配列 
					 */
					virtual const sce::Vectormath::Simd::Aos::Matrix4 *getInvBindMatrices(void) const = 0;
					/*!
					 * @~English
					 * @brief Gets a bind shape matrix. 
					 * @details This gets a bind shape matrix. 
					 * @return Bind shape matrix 
					 * @~Japanese
					 * @brief バインドシェイプマトリックスの取得 
					 * @details バインドシェイプマトリックスを取得します。 
					 * @return バインドシェイプマトリックス 
					 */
					virtual sce::Vectormath::Simd::Aos::Matrix4 getBindShapeMatrix(void) const = 0;

					virtual ~Mesh(void){}
				};

			}

		}
	}
}


#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_GEOMETRY_H */
