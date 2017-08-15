/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_H

#include <sampleutil/graphics.h>


/* --------------------------------------------------------------- */


#include <sampleutil/graphics/collada/collada_base.h>
#include <sampleutil/graphics/collada/collada_image.h>
#include <sampleutil/graphics/collada/collada_effect.h>
#include <sampleutil/graphics/collada/collada_geometry.h>
#include <sampleutil/graphics/collada/collada_animation.h>
#include <sampleutil/graphics/collada/collada_scene.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{

			namespace Collada
			{




				/*!
				 * @~English
				 * @brief Class of the Collada data 
				 * @details This is the class to handle the Collada data. 
				 * @~Japanese
				 * @brief Colladaデータのクラス 
				 * @details Colladaのデータを扱うためのクラスです。 
				 */
				class SCE_SAMPLE_UTIL_API ColladaData : public Resource
				{
				public:
					virtual ~ColladaData(void){}

					/*!
					 * @~English
					 * @return Pointer to ColladaLoader 
					 * @brief Returns ColladaLoader used for initialization. 
					 * 
					 * @details This returns ColladaLoader used for initialization. 
					 * @~Japanese
					 * @return ColladaLoaderへのポインタ 
					 * @brief 初期化に使用したColladaLoaderを返す 
					 * 
					 * @details 初期化に使用したColladaLoader を返します。 
					 */
					virtual ColladaLoader  *getLoader() const = 0;
					/*!
					 * @~English
					 * 
					 * @param id Image ID
					 * @return Pointer to Image. NULL if not found. 
					 * @brief Get image 
					 * 
					 * @details This gets an image. 
					 * @~Japanese
					 * 
					 * @param id 画像のID
					 * @return Imageへのポインタ。見つからなかった場合はNULL。 
					 * @brief 画像の取得 
					 * 
					 * @details 画像を取得します。 
					 */
					virtual const Image *getImageById(std::string id) const = 0;
					/*!
					 * @~English
					 * @param index Index of images. 0 or higher and lower than getNumImages().
					 * @return Pointer to Image. NULL if not found. 
					 * @brief Get image 
					 * 
					 * @details This gets an image. 
					 * @~Japanese
					 * @param index 画像のインデックス。0以上,getNumImages()未満。
					 * @return Imageへのポインタ。見つからなかった場合はNULL。 
					 * @brief 画像の取得 
					 * 
					 * @details 画像を取得します。 
					 */
					virtual const Image *getImageByIndex(uint32_t index) const = 0;

#if defined(_SCE_TARGET_OS_PSP2) && _SCE_TARGET_OS_PSP2
					/*!
					 * @~English
					 * @param index Index of images. 0 or higher and lower than getNumImages().
					 * @return Pointer to Image. NULL if not found. 
					 * @brief Get image 
					 * 
					 * @details This gets an image. 
					 * @~Japanese
					 * @param index 画像のインデックス。0以上,getNumImages()未満。
					 * @return Imageへのポインタ。見つからなかった場合はNULL。 
					 * @brief 画像の取得 
					 * 
					 * @details 画像を取得します。 
					 */
					virtual Image *getImageByIndex(uint32_t index) = 0;
#endif

					/*!
					 * @~English
					 * @return Number of images 
					 * @brief Get number of images 
					 * 
					 * @details Gets the number of images. 
					 * @~Japanese
					 * @return 画像数 
					 * @brief 画像数の取得 
					 * 
					 * @details 画像数を取得します。 
					 */
					virtual uint32_t getNumImages(void) = 0;

					/*!
					 * @~English
					 * @param id ID of a material
					 * @return Pointer to Material. NULL if not found. 
					 * @brief Get material 
					 * 
					 * @details This gets a material. 
					 * @~Japanese
					 * @param id マテリアルのID
					 * @return Materialへのポインタ。見つからなかった場合はNULL。 
					 * @brief マテリアルの取得 
					 * 
					 * @details マテリアルを取得します。 
					 */
					virtual const Material *getMaterialById(std::string id) const =0;
					/*!
					 * @~English
					 * @param index Index of materials. 0 or higher and lower than getNumMaterials().
					 * @return Pointer to Material. NULL if not found. 
					 * @brief Get material 
					 * 
					 * @details This gets a material. 
					 * @~Japanese
					 * @param index マテリアルのインデックス。0以上,getNumMaterials()未満。
					 * @return Materialへのポインタ。見つからなかった場合はNULL。 
					 * @brief マテリアルの取得 
					 * 
					 * @details マテリアルを取得します。 
					 */
					virtual const Material *getMaterialByIndex(uint32_t index) const =0;
					/*!
					 * @~English
					 * @return Number of materials 
					 * @brief Get number of materials 
					 * 
					 * @details This gets the number of materials. 
					 * @~Japanese
					 * @return マテリアル数 
					 * @brief マテリアル数の取得 
					 * 
					 * @details マテリアル数を取得します。 
					 */
					virtual uint32_t getNumMaterials(void) const= 0;

					/*!
					 * @~English
					 * @brief Get mesh by specifying the controller ID 
					 * @details This function obtains the mesh for the specified controller ID. 
					 * @param controllerId Mesh controller ID
					 * @return Pointer to Mesh. NULL if not found. 
					 * @~Japanese
					 * @brief コントローラーID指定でメッシュを取得する 
					 * @details コントローラーID指定でメッシュを取得します。 
					 * @param controllerId メッシュのコントローラーID
					 * @return Meshへのポインタ。見つからなかった場合はNULL。 
					 */
					virtual const Mesh *getMeshByControllerId(std::string controllerId) const=0;
					/*!
					 * @~English
					 * 
					 * @return Pointer to Mesh. NULL if not found. 
					 * @brief Gets a mesh by specifying a geometry ID. 
					 * 
					 * @param geometryId Geometry ID
					 * @details This gets a mesh by specifying a geometry ID. 
					 * @~Japanese
					 * 
					 * @return Meshへのポインタ。見つからなかった場合はNULL。 
					 * @brief ジオメトリID指定でメッシュを取得する 
					 * 
					 * @param geometryId ジオメトリID
					 * @details ジオメトリID指定でメッシュを取得します。 
					 */

					virtual const Mesh *getMeshByGeometryId(std::string geometryId) const=0;
					/*!
					 * @~English
					 * @param index Index of meshes. 0 or higher and lower than getNumMeshes().
					 * @return Pointer to Mesh. NULL if not found. 
					 * @brief Gets a mesh by specifying an index. 
					 * 
					 * @details This gets a mesh by specifying an index. 
					 * @~Japanese
					 * @param index メッシュのインデックス。0以上,getNumMeshes()未満。
					 * @return Meshへのポインタ。見つからなかった場合はNULL。 
					 * @brief インデックス指定でメッシュを取得する 
					 * 
					 * @details インデックス指定でメッシュを取得します。 
					 */
					virtual const Mesh *getMeshByIndex(uint32_t index) const=0;

					/*!
					 * @~English
					 * @return Number of meshes 
					 * @brief Gets the number of meshes 
					 * 
					 * @details This gets the number of meshes 
					 * @~Japanese
					 * @return メッシュ数 
					 * @brief メッシュ数を取得する 
					 * 
					 * @details メッシュ数を取得します。 
					 */
					virtual uint32_t getNumMeshes(void) const=0;

					/*!
					 * @~English
					 * @return Pointer to VisualScene. NULL if not found. 
					 * @brief Gets a scene. 
					 * 
					 * @details This gets a scene. 
					 * @~Japanese
					 * @return VisualSceneへのポインタ。見つからなかった場合はNULL。 
					 * @brief シーンを取得する 
					 * 
					 * @details シーンを取得します。 
					 */
					virtual const VisualScene *getVisualScene(void) const=0;

					/*!
					 * @~English
					 * @return Pointer to Animation. NULL if not found. 
					 * @brief Gets animation data. 
					 * 
					 * @details This gets animation data. 
					 * @~Japanese
					 * @return Animationへのポインタ。見つからなかった場合はNULL。 
					 * @brief アニメーションデータを取得する 
					 * 
					 * @details アニメーションデータを取得します。 
					 */
					virtual const Animation* getAnimation(void) const=0;
				};



			}


		}
	}
}



#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_H */
