/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_IMAGE_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_IMAGE_H

#include <sampleutil/graphics/collada/collada_base.h>
#include <string.h>

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
					class LibraryImages;
					class ColladaLoaderImpl;
				}

				/*!
				 * @~English
				 * @brief Image class 
				 * @details This is the image class. 
				 * @~Japanese
				 * @brief イメージクラス 
				 * @details イメージクラスです。 
				 */
				class SCE_SAMPLE_UTIL_API Image
				{
					friend class Internal::LibraryImages;
				public:
					/*!
					 * @~English
					 * @brief Gets a texture 
					 * 
					 * @details This returns the texture of this image. 
					 * @return Texture 
					 * @~Japanese
					 * @brief テクスチャの取得 
					 * 
					 * @details このイメージのテクスチャを返します。 
					 * @return テクスチャ 
					 */
					sce::SampleUtil::Graphics::Texture *getTexture(void);

					/*!
					 * @~English
					 * @brief Gets a texture 
					 * 
					 * @details This returns the texture of this image. 
					 * @return Texture 
					 * @~Japanese
					 * @brief テクスチャの取得 
					 * 
					 * @details このイメージのテクスチャを返します。 
					 * @return テクスチャ 
					 */
					const sce::SampleUtil::Graphics::Texture *getTexture(void) const;

					/*!
					 * @~English
					 * @brief Gets an ID. 
					 * 
					 * @details This returns the ID of this image. 
					 * @return ID 
					 * @~Japanese
					 * @brief IDの取得 
					 * 
					 * @details このイメージのIDを返します。 
					 * @return ID 
					 */
					const std::string getId(void) const;

					/*!
					 * @~English
					 * @brief Gets a name. 
					 * 
					 * @details This returns the name of this image. 
					 * @return Name 
					 * @~Japanese
					 * @brief 名前の取得 
					 * 
					 * @details このイメージの名前を返します。 
					 * @return 名前 
					 */
					const std::string getName(void) const;

					/*!
					 * @~English
					 * @brief Absolute path of an image 
					 * 
					 * @details This returns the absolute path of this image. 
					 * @return Absolute path 
					 * @~Japanese
					 * @brief イメージの絶対パス 
					 * 
					 * @details このイメージの絶対パスを返します。 
					 * @return 絶対パス 
					 */
					const std::string getAbsolutePath(void) const;

					/*!
					 * @~English
					 * @brief Path of an image 
					 * 
					 * @details This returns the path of this image. The absolute path or the relative path from a collada file is returned. 
					 * @return Path 
					 * @~Japanese
					 * @brief イメージのパス 
					 * 
					 * @details このイメージのパスを返します。絶対パスもしくはcolladaファイルからの相対パスが返ります。 
					 * @return パス 
					 */
					const std::string getPath(void) const;


					void serialize(Internal::Serializer *serializer);
				private:
					Internal::ColladaLoaderImpl *m_loaderConfig;

#if _SCE_TARGET_OS_WINDOWS
#pragma warning(push,3)
#pragma warning(disable: 4251)
#endif
					std::string m_id;
					std::string m_name;
					std::string m_path;
					std::string m_absPath;
#if _SCE_TARGET_OS_WINDOWS
#pragma warning(pop)
#endif
					sce::SampleUtil::Graphics::Texture *m_texture;

					Image(void);

					int initialize(ColladaLoader *loaderConfig, 
						const std::string daePath, 
						const Dae::Image::Image *daeImage);
					int finalize(void);

					~Image(void);
				};


			}

		}
	}
}

#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_IMAGE_H */

