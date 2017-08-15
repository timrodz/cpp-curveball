/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ANIMATION_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ANIMATION_H

#include <sampleutil/graphics/collada/collada_base.h>
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
					struct LibraryTransformAnimations;
				}

				/*!
				 * @~English
				 * @brief Transform of a node in a scene 
				 * 
				 * @details This is the transform of a node in a scene. 
				 * @~Japanese
				 * @brief シーン内のノードのトランスフォーム 
				 * 
				 * @details シーン内のノードのトランスフォームです。 
				 */
				struct NodeTransform
				{
				public:
					/*!
					 * @~English
					 * @brief Scaling element of transform 
					 * 
					 * @~Japanese
					 * @brief トランスフォームのスケーリング成分 
					 * 
					 */
					sce::Vectormath::Simd::Aos::Vector3 scale;
					/*!
					 * @~English
					 * @brief Rotation element of transform 
					 * 
					 * @~Japanese
					 * @brief トランスフォームの回転成分 
					 * 
					 */
					sce::Vectormath::Simd::Aos::Quat    rotation;
					/*!
					 * @~English
					 * @brief Parallel displacement element of transform 
					 * 
					 * @~Japanese
					 * @brief トランスフォームの平行移動成分 
					 * 
					 */
					sce::Vectormath::Simd::Aos::Vector3 translation;
				};

				/*!
				 * @~English
				 * 
				 * @brief Animation data of one node 
				 * @details This is the animation data of one node. 
				 * @~Japanese
				 * 
				 * @brief ノード１つのアニメーションデータ 
				 * @details ノード１つのアニメーションデータです。 
				 */
				class NodeAnimation
				{
					friend class Animation;


				public:
					/*!
					 * @~English
					 * @return ID of the target node of animation 
					 * @brief Returns ID of target node of animation 
					 * 
					 * @details This returns the ID of the target node of animation. 
					 * @~Japanese
					 * @return アニメーションの対象ノードのID 
					 * @brief アニメーションの対象ノードのIDを返す 
					 * 
					 * @details アニメーションの対象ノードのIDを返します。 
					 */
					std::string getNodeId(void) const;
					/*!
					 * @~English
					 * @return Number of frames of animation 
					 * @brief Returns number of frames of animation 
					 * 
					 * @details This returns the number of frames of animation. 
					 * @~Japanese
					 * @return アニメーションのフレーム数 
					 * @brief アニメーションのフレーム数を返す 
					 * 
					 * @details アニメーションのフレーム数を返します。 
					 */
					uint32_t getNumFrames(void) const;
					/*!
					 * @~English
					 * @param frameIndex Frame number
					 * @return Transform of the specified frame 
					 * @brief Returns transform of specified frame 
					 * 
					 * @details This returns the transform of the specified frame. 
					 * @~Japanese
					 * @param frameIndex フレーム番号
					 * @return 指定したフレームのトランスフォ－ム 
					 * @brief 指定したフレームのトランスフォ－ムを返す 
					 * 
					 * @details 指定したフレームのトランスフォ－ムを返します。 
					 */
					NodeTransform getNodeTransform(uint32_t frameIndex) const;
					/*!
					 * @~English
					 * @param frameIndex Frame number
					 * @return The matrix of the specified frame 
					 * @brief Return matrix of specified frame 
					 * 
					 * @details This returns the matrix of the specified frame. 
					 * @~Japanese
					 * @param frameIndex フレーム番号
					 * @return 指定したフレームの行列 
					 * @brief 指定したフレームの行列を返す 
					 * 
					 * @details 指定したフレームの行列を返します。 
					 */
					sce::Vectormath::Simd::Aos::Matrix4 getMatrix(uint32_t frameIndex) const;

				private:
					std::string m_nodeId;
					uint32_t m_numFrames;

					NodeTransform *m_transforms;

					NodeAnimation(void);
					~NodeAnimation(void);
					int initialize(std::string nodeId, uint32_t numFrames);
					int finalize(void);
				};


			}

		}
	}
}
#endif /* SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_ANIMATION_H */
