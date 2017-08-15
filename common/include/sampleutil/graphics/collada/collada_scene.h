/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_SCENE_H
#define _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_SCENE_H

#include <sampleutil/graphics/collada/collada_base.h>

namespace sce
{
	namespace SampleUtil
	{
		namespace Graphics
		{

			namespace Collada
			{
				class Node;
				class VisualScene;

				namespace Internal
				{
					class EffectInstance;
					class NodeInternal;
					class ColladaDataImpl;
					class InstanceVisualSceneImpl;
					class AnimationPlayerImpl;
					class InstanceMeshImpl;
				}

				/*!
				 * @~English
				 * @brief Instance of a mesh part 
				 * @details This is the instance of a mesh part. 
				 * @~Japanese
				 * @brief メッシュパートのインスタンス 
				 * @details メッシュパートのインスタンスです。 
				 */
				class SCE_SAMPLE_UTIL_API InstanceMeshPart 
				{
				public:
					/*!
					 * @~English
					 * @brief Draws a mesh part 
					 * @details This draws a mesh part. 
					 * @param graphicsContext Graphics context
					 * @param skinMatrices Skin matrices. It is required only if a mesh has a skin.
					 * @param renderingStage Rendering stage. This draws only effect whose rendering stage matches.
					 * @~Japanese
					 * @brief メッシュパートの描画 
					 * @details メッシュパートを描画します。 
					 * @param graphicsContext グラフィックスコンテキスト
					 * @param skinMatrices スキン行列。メッシュがスキンを持つ場合のみ必要です。
					 * @param renderingStage レンダリングステージ。レンダリングステージが一致するエフェクトのみ描画します。
					 */
					virtual void draw(Graphics::GraphicsContext *graphicsContext,
						Graphics::UniformBuffer *skinMatrices,
						int renderingStage) = 0;

					/*!
					 * @~English
					 * @brief Changes the reference destination of a shader parameter. 
					 * @details This changes the reference destination of a parameter used for shader drawing in a scene. getParamValue() of reference is called to the shader parameters of all effects included in this mesh part, and the reference destination of a parameter to which a value other than NULL was returned is changed. 
					 * @param nodeParameterManager The reference destination of a shader parameter
					 * @~Japanese
					 * @brief シェーダパラメータの参照先の変更 
					 * @details シーン内のシェーダ描画に使われるパラメータの参照先を変更します。このメッシュパートに含まれるすべてのエフェクトのシェーダパラメータに対して、referenceのgetParamValue()が呼ばれ、NULL以外の値が返ったパラメータの参照先を変更します。 
					 * @param nodeParameterManager シェーダパラメータの参照先
					 */
					virtual void applyShaderParameterManager(ShaderParameterReference *nodeParameterManager) = 0;

					/*!
					 * @~English
					 * @brief Adds an effect. 
					 * @details This adds an effect. This is used for drawing only when renderingStage specified by draw matches. 
					 * @param effect The effect to be added
					 * @param renderingStage Rendering stage to draw
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief エフェクトの追加 
					 * @details エフェクトを追加します。drawで指定されるrenderingStageが一致したときのみ描画に使用されます。 
					 * @param effect 追加するエフェクト
					 * @param renderingStage 描画を行うレンダリングステージ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					virtual int addEffect(const Graphics::Effect::EffectData *effect,	int renderingStage) = 0;

					virtual ~InstanceMeshPart(void){}
				};

				/*!
				 * @~English
				 * @brief Instance of a mesh 
				 * @details This is the instance of a mesh. 
				 * @~Japanese
				 * @brief メッシュのインスタンス 
				 * @details メッシュのインスタンスです。 
				 */
				class SCE_SAMPLE_UTIL_API InstanceMesh
				{
				public:
					virtual ~InstanceMesh(void){}
					/*!
					 * @~English
					 * @brief Updates the skin matrix of a mesh. 
					 * @details This updates the skin matrix of a mesh. The world matrix of the node to be referenced needs to be updated before this function is called. 
					 * @~Japanese
					 * @brief メッシュのスキン行列の更新 
					 * @details メッシュのスキン行列を更新します。この関数が呼ばれる前に参照するノードのワールド行列が更新されている必要があります。 
					 */
					virtual void updateSkinMatrices(void) = 0;


					/*!
					 * @~English
					 * @brief Changes the reference destination of a shader parameter. 
					 * @details This changes the reference destination of a parameter used for shader drawing in a scene. getParamValue() of reference is called to the shader parameters of all mesh parts included in this mesh, and the reference destination of a parameter to which a value other than NULL was returned is changed. 
					 * @param nodeParameterManager The reference destination of a shader parameter
					 * @~Japanese
					 * @brief シェーダパラメータの参照先の変更 
					 * @details シーン内のシェーダ描画に使われるパラメータの参照先を変更します。このメッシュに含まれるすべてのメッシュパートのシェーダパラメータに対して、referenceのgetParamValue()が呼ばれ、NULL以外の値が返ったパラメータの参照先を変更します。 
					 * @param nodeParameterManager シェーダパラメータの参照先
					 */
					virtual void applyShaderParameterManager(ShaderParameterReference *nodeParameterManager) = 0;

					/*!
					 * @~English
					 * @brief Drawing 
					 * 
					 * @details This draws all mesh parts included in this mesh. 
					 * @param localToWorld The transformation matrix from the local coordinate of this mesh to the world coordinate
					 * @param graphicsContext Graphics context
					 * @param renderingStage Rendering stage. This draws only effect whose rendering stage matches.
					 * @~Japanese
					 * @brief 描画 
					 * 
					 * @details このメッシュに含まれるすべてのメッシュパートの描画を行います。 
					 * @param localToWorld このメッシュのローカル座標からワールド座標への変換行列
					 * @param graphicsContext グラフィックスコンテキスト
					 * @param renderingStage レンダリングステージ。レンダリングステージが一致するエフェクトのみ描画します。
					 */
					virtual void draw( sce::Vectormath::Simd::Aos::Matrix4_arg localToWorld, 
						Graphics::GraphicsContext *graphicsContext, 
						int renderingStage) = 0;

					/*!
					 * @~English
					 * @brief Gets the number of mesh parts 
					 * @details  This gets the number of mesh parts. 
					 * @return Number of mesh parts 
					 * @~Japanese
					 * @brief メッシュパート数の取得 
					 * @details  メッシュパート数を取得します。 
					 * @return メッシュパート数 
					 */
					virtual uint32_t getNumMeshParts(void) const = 0;

					/*!
					 * @~English
					 * @brief Gets a mesh part 
					 * @details This gets a mesh part. 
					 * @param index Index of mesh parts
					 * @return Mesh part. NULL in the case that index is equal to or larger than the number of mesh parts. 
					 * @~Japanese
					 * @brief メッシュパートの取得 
					 * @details メッシュパートを取得します。 
					 * @param index メッシュパートのインデックス
					 * @return メッシュパート。indexがメッシュパート数以上のときはNULL。 
					 */
					virtual InstanceMeshPart *getMeshPart(uint32_t index) = 0;

					/*!
					 * @~English
					 * @brief Gets the mesh which was the source. 
					 * @details This returns the mesh which was the generation source of this instance mesh. 
					 * @return Mesh 
					 * @~Japanese
					 * @brief 元となったメッシュの取得 
					 * @details このインスタンスメッシュの生成元となったメッシュを返します。 
					 * @return メッシュ 
					 */
					virtual const Mesh* getMesh(void) const = 0;

					/*!
					 * @~English
					 * @brief Adds an effect. 
					 * 
					 * @details This adds an effect to all mesh parts included in this mesh. This is used for drawing only when renderingStage specified by draw matches. 
					 * @param effect The effect to be added
					 * @param renderingStage Rendering stage to draw
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief エフェクトの追加 
					 * 
					 * @details このメッシュに含まれるすべてのメッシュパートにエフェクトを追加します。drawで指定されるrenderingStageが一致したときのみ描画に使用されます。 
					 * @param effect 追加するエフェクト
					 * @param renderingStage 描画を行うレンダリングステージ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					virtual int addEffect(const Graphics::Effect::EffectData *effect,	int renderingStage) = 0;

				};


				/*!
				 * @~English
				 * @brief Node of a scene 
				 * 
				 * @details This is the node of a scene. 
				 * 
				 * @~Japanese
				 * @brief シーンのノード 
				 * 
				 * @details シーン内のノードです。 
				 * 
				 */
				class SCE_SAMPLE_UTIL_API Node
				{
				public:
					/*!
					 * @~English
					 * @brief Returns the ID of a node. 
					 * @details This returns the ID of a node. 
					 * 
					 * @return ID of a node 
					 * @~Japanese
					 * @brief ノードのIDを返す 
					 * @details ノードのIDを返します。 
					 * 
					 * @return ノードのID 
					 */
					virtual std::string getId(void) const = 0;
					/*!
					 * @~English
					 * @brief Returns the SID of a node. 
					 * @details This returns the SID of a node. 
					 * @return SID of a node 
					 * @~Japanese
					 * @brief ノードのSIDを返す 
					 * @details ノードのSIDを返します。 
					 * @return ノードのSID 
					 */
					virtual std::string getSid(void) const = 0;


					/*!
					 * @~English
					 * @brief Searches the node with the SID specified from the subtree lower than this node 
					 * @details This searches the node with the SID specified from the subtree lower than this node. 
					 * @param sid SID of a node
					 * @return Node with the specified SID. NULL if not found. 
					 * @~Japanese
					 * @brief このノード以下のサブツリーの中から指定されたSIDを持つノードを検索する 
					 * @details このノード以下のサブツリーの中から指定されたSIDを持つノードを検索します。 
					 * @param sid ノードのSID
					 * @return 指定されたSIDを持つノード。見つからなかった場合はNULL。 
					 */
					virtual Node *findNodeBySid(std::string sid) = 0;
					/*!
					 * @~English
					 * @brief Searches the node with the ID specified from the subtree lower than this node 
					 * @details This searches the node with the ID specified from the subtree lower than this node. 
					 * @param id ID of a node
					 * @return Node with the specified ID. NULL if not found. 
					 * @~Japanese
					 * @brief このノード以下のサブツリーの中から指定されたIDを持つノードを検索する 
					 * @details このノード以下のサブツリーの中から指定されたIDを持つノードを検索します。 
					 * @param id ノードのID
					 * @return 指定されたIDを持つノード。見つからなかった場合はNULL。 
					 */
					virtual Node *findNodeById(std::string id) = 0;
					/*!
					 * @~English
					 * @brief Searches the node with the SID specified from the subtree lower than this node 
					 * @details This searches the node with the SID specified from the subtree lower than this node. 
					 * @param sid SID of a node
					 * @return Node with the specified SID. NULL if not found. 
					 * @~Japanese
					 * @brief このノード以下のサブツリーの中から指定されたSIDを持つノードを検索する 
					 * @details このノード以下のサブツリーの中から指定されたSIDを持つノードを検索します。 
					 * @param sid ノードのSID
					 * @return 指定されたSIDを持つノード。見つからなかった場合はNULL。 
					 */
					virtual const Node *findNodeBySid(std::string sid) const = 0;
					/*!
					 * @~English
					 * @brief Searches the node with the ID specified from the subtree lower than this node 
					 * @details This searches the node with the ID specified from the subtree lower than this node. 
					 * @param id ID of a node
					 * @return Node with the specified ID. NULL if not found. 
					 * @~Japanese
					 * @brief このノード以下のサブツリーの中から指定されたIDを持つノードを検索する 
					 * @details このノード以下のサブツリーの中から指定されたIDを持つノードを検索します。 
					 * @param id ノードのID
					 * @return 指定されたIDを持つノード。見つからなかった場合はNULL。 
					 */
					virtual const Node *findNodeById(std::string id) const = 0;

					/*!
					 * @~English
					 * @brief Returns the transformation matrix of this node. 
					 * @details This returns the transformation matrix of this node. 
					 * @return Transformation matrix of a node 
					 * @~Japanese
					 * @brief このノードの変換行列を返す 
					 * @details このノードの変換行列を返します。 
					 * @return ノードの変換行列 
					 */
					virtual sce::Vectormath::Simd::Aos::Matrix4 getTransformMatrix(void) const = 0;
					/*!
					 * @~English
					 * @brief Sets the transformation matrix of a node. 
					 * @details This sets the transformation matrix of a node. 
					 * @param matrix Transformation matrix of a node
					 * @~Japanese
					 * @brief ノードの変換行列を設定する 
					 * @details ノードの変換行列を設定します。 
					 * @param matrix ノードの変換行列
					 */
					virtual void setTransformMatrix(sce::Vectormath::Simd::Aos::Matrix4_arg matrix) = 0;
					/*!
					 * @~English
					 * @brief Returns the world matrix of a node with the root node as the starting point. 
					 * @details This returns the world matrix of a node with the root node as the starting point. 
					 * @return The world matrix of the world matrix node of a node 
					 * @~Japanese
					 * @brief ルートノードを起点としたノードのワールド行列を返す 
					 * @details ルートノードを起点としたノードのワールド行列を返します。 
					 * @return ノードのワールド行列ノードのワールド行列 
					 */
					virtual sce::Vectormath::Simd::Aos::Matrix4 getWorldMatrix(void) const = 0;
					/*!
					 * @~English
					 * @brief Returns the pointer to the world matrix of a node with the root node as the starting point. 
					 * @details This returns the pointer to the world matrix of a node with the root node as the starting point. 
					 * @return Pointer to the world matrix of the world matrix node of a node 
					 * @~Japanese
					 * @brief ルートノードを起点としたノードのワールド行列へのポインタを返す 
					 * @details ルートノードを起点としたノードのワールド行列へのポインタを返します。 
					 * @return ノードのワールド行列ノードのワールド行列へのポインタ 
					 */
					virtual const sce::Vectormath::Simd::Aos::Matrix4* getWorldMatrixPointer(void) const = 0;

					/*!
					 * @~English
					 * @brief Updates the world matrix of a node. 
					 * @details This updates the world matrix of a node. This calculates the world matrix of this node from the world matrix of the parent node and the transformation matrix of this node, and sets it. Also this updates the world matrix of a child node recursively. 
					 * @~Japanese
					 * @brief ノードのワールド行列を更新する 
					 * @details ノードのワールド行列を更新します。親ノードのワールド行列とこのノードの変換行列からこのノードのワールド行列を計算し、設定します。再帰的に子ノードのワールド行列の更新も行います。 
					 */
					virtual void updateWorldMatrix(void) = 0; 
					/*!
					 * @~English
					 * @brief Updates the skin matrices of the meshes under this node. 
					 * @details This updates the skin matrices of the meshes under this node. This updates the skin matrices of the meshes included in the subtrees under this node. The world matrix needs to be updated before this function is called. 
					 * @~Japanese
					 * @brief このノード以下のメッシュのスキン行列を更新する 
					 * @details このノード以下のメッシュのスキン行列を更新します。このノード以下のサブツリーに含まれるメッシュのスキン行列を更新します。この関数が呼ばれる前にワールド行列が更新されている必要があります。 
					 */
					virtual void updateSkinMatrices(void) = 0;

					/*!
					 * @~English
					 * @brief Draws the subtrees under this node. 
					 * @details This draws the subtrees under this node. 
					 * @param graphicsContext Graphics context
					 * @param matrix Transformation matrix. The product of matrix and the world matrix of a node is given to a shader as the world matrix.
					 * @param renderingStage Rendering stage. This draws only effect of the specified rendering stage.
					 * @~Japanese
					 * @brief このノード以下のサブツリーを描画する 
					 * @details このノード以下のサブツリーを描画します。 
					 * @param graphicsContext グラフィックスコンテキスト
					 * @param matrix 変換行列。matrixとノードのワールド行列の積がワールド行列としてシェーダに渡されます。
					 * @param renderingStage レンダリングステージ。指定したレンダリングステージのエフェクトのみ描画されます。
					 */
					virtual void draw(GraphicsContext *graphicsContext, 
						sce::Vectormath::Simd::Aos::Matrix4_arg matrix,
						int renderingStage) const = 0;

					/*!
					 * @~English
					 * @brief Changes the reference destination of a shader parameter. 
					 * 
					 * @details This changes the reference destination of a parameter used for shader drawing in a scene. getParamValue() of reference is called to all shader parameters in the subtrees under this node, and the reference destination of a parameter to which a value other than NULL was returned is changed. 
					 * @param nodeParameterManager The reference destination of a shader parameter
					 * @~Japanese
					 * @brief シェーダパラメータの参照先の変更 
					 * 
					 * @details シーン内のシェーダ描画に使われるパラメータの参照先を変更します。このノード以下のサブツリー内の全てのシェーダパラメータに対して、referenceのgetParamValue()が呼ばれ、NULL以外の値が返ったパラメータの参照先を変更します。 
					 * @param nodeParameterManager シェーダパラメータの参照先
					 */
					virtual void applyShaderParameterManager(ShaderParameterReference *nodeParameterManager) = 0;

					/*!
					 * @~English
					 * 
					 * @brief Gets an instance mesh. 
					 * @details This returns the instance mesh which this node has. 
					 * @param index Index of instance meshes.
					 * @return Instance mesh. NULL in the case that index larger than the number of instance meshes is specified. 
					 * @~Japanese
					 * 
					 * @brief インスタンスメッシュの取得 
					 * @details このノードが持つインスタンスメッシュを返します。 
					 * @param index インスタンスメッシュのインデックス。
					 * @return インスタンスメッシュ。インスタンスメッシュ数以上のindexが指定された場合はNULL。 
					 */
					virtual InstanceMesh* getInstanceMesh(uint32_t index) = 0;

					/*!
					 * @~English
					 * @brief Gets the number of instance meshes. 
					 * 
					 * @details This returns the number of instance meshes which this node has. 
					 * @return Number of instance meshes 
					 * @~Japanese
					 * @brief インスタンスメッシュ数の取得 
					 * 
					 * @details このノードが持つインスタンスメッシュ数を返します。 
					 * @return インスタンスメッシュ数 
					 */
					virtual uint32_t getNumInstanceMeshes(void) const = 0;

					/*!
					 * @~English
					 * @brief Gets the number of child nodes 
					 * @details This returns the number of child nodes. 
					 * @return Number of child nodes 
					 * @~Japanese
					 * @brief 子ノードの数の取得 
					 * @details 子ノードの数を返します。 
					 * @return 子ノードの数 
					 */
					virtual uint32_t getNumChildNodes(void) const = 0;

					/*!
					 * @~English
					 * @brief Returns a child node 
					 * @details This returns a child node. 
					 * @param index Index of child nodes.
					 * @return Child node. NULL in the case that index larger than the number of child nodes is specified. 
					 * @~Japanese
					 * @brief 子ノードを返す 
					 * @details 子ノードを返します。 
					 * @param index 子ノードのインデックス。
					 * @return 子ノード。子ノード数以上のindexが指定された場合はNULL。 
					 */
					virtual Node* getChildNode(uint32_t index) = 0;

					/*!
					 * @~English
					 * @brief Returns a child node 
					 * @details This returns a child node. 
					 * @param index Index of child nodes.
					 * @return Child node. NULL in the case that index larger than the number of child nodes is specified. 
					 * @~Japanese
					 * @brief 子ノードを返す 
					 * @details 子ノードを返します。 
					 * @param index 子ノードのインデックス。
					 * @return 子ノード。子ノード数以上のindexが指定された場合はNULL。 
					 */
					virtual const Node* getChildNode(uint32_t index) const = 0;

					/*!
					 * @~English
					 * @brief Adds an effect. 
					 * 
					 * @details This adds the specified effect to all meshes of the subtrees under this node. This is used for drawing only when renderingStage specified by draw matches. 
					 * @param effect The effect to be added
					 * @param renderingStage Rendering stage to draw
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief エフェクトの追加 
					 * 
					 * @details このノード以下のサブツリーの全てのメッシュに指定されたエフェクトを追加します。drawで指定されるrenderingStageが一致したときのみ描画に使用されます。 
					 * @param effect 追加するエフェクト
					 * @param renderingStage 描画を行うレンダリングステージ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					virtual int addEffect(const Graphics::Effect::EffectData *effect, int renderingStage) = 0;

					virtual ~Node(void){}
				};

				/*!
				 * @~English
				 * @brief Animation 
				 * @details This is the class to retain the node animation data of a scene. 
				 * 
				 * @~Japanese
				 * @brief アニメーション 
				 * @details シーンのノードアニメーションデータを保持するクラスです。 
				 * 
				 */
				class SCE_SAMPLE_UTIL_API Animation
				{
					friend class Internal::ColladaDataImpl;
					friend class Internal::AnimationPlayerImpl;
				public:
					/*!
					 * @~English
					 * @brief Returns the starting time of animation. 
					 * @details This returns the starting time of animation. 
					 * @return The starting time of animation 
					 * @~Japanese
					 * @brief アニメーションの開始時刻を返す 
					 * @details アニメーションの開始時刻を返します。 
					 * @return アニメーションの開始時刻 
					 */
					float getStartTime(void) const;

					/*!
					 * @~English
					 * @brief Returns the finish time of animation. 
					 * @details This returns the finish time of animation. 
					 * @return The finish time of animation 
					 * @~Japanese
					 * @brief アニメーションの終了時刻を返す 
					 * @details アニメーションの終了時刻を返します。 
					 * @return アニメーションの終了時刻 
					 */
					float getEndTime(void) const;

					/*!
					 * @~English
					 * @brief Returns the number of frames. 
					 * @details This returns the number of frames. 
					 * 
					 * @return Number of frames 
					 * @~Japanese
					 * @brief フレーム数を返す 
					 * @details フレーム数を返します。 
					 * 
					 * @return フレーム数 
					 */
					uint32_t getNumFrames(void) const;


					/*!
					 * @~English
					 * @brief Returns the number of animation nodes. 
					 * @details This returns the number of animation nodes. 
					 * 
					 * @return Number of animation nodes 
					 * @~Japanese
					 * @brief アニメーションノード数を返す 
					 * @details アニメーションノード数を返します。 
					 * 
					 * @return アニメーションノード数 
					 */
					uint32_t getNumAnimationNodes(void) const
					{
						return m_numNodeAnimations;
					}

					/*!
					 * @~English
					 * @brief Returns the animation node of the specified index. 
					 * @details This returns the animation node of the specified index. 
					 * 
					 * @param index Index
					 * @return Animation node 
					 * @~Japanese
					 * @brief 指定されたインデックスのアニメーションノードを返す 
					 * @details 指定されたインデックスのアニメーションノードを返します。 
					 * 
					 * @param index インデックス
					 * @return アニメーションノード 
					 */
					const NodeAnimation *getAnimationNode(uint32_t index) const
					{
						if(index >= m_numNodeAnimations){
							return NULL;
						}
						return &m_nodeAnimations[index];
					}




				private:
					Animation(void);
					~Animation(void);

					int initialize(Internal::LibraryTransformAnimations *libraryAnimations, 
						VisualScene *visualScene, 
						float framePeriod=0.0166666666666666);
					int finalize(void);	


					float m_framePeriod;
					float m_startTime;
					float m_endTime;
					NodeAnimation *m_nodeAnimations;
					uint32_t m_numNodeAnimations;
					const VisualScene *m_visualScene;
					uint32_t m_numFrames;


				};


				/*!
				 * @~English
				 * @brief Scene class 
				 * @details This is the scene class. 
				 * @~Japanese
				 * @brief シーンクラス 
				 * @details シーンクラスです。 
				 */
				class SCE_SAMPLE_UTIL_API VisualScene 
				{
				public:

					/*!
					 * @~English
					 * @brief Returns the ID of a scene. 
					 * @details This returns the ID of a scene. 
					 * @return ID 
					 * @~Japanese
					 * @brief シーンのIDを返す 
					 * @details シーンのIDを返します。 
					 * @return ID 
					 */
					virtual std::string  getId(void) const = 0; 
					/*!
					 * @~English
					 * @brief Returns the name of a scene. 
					 * @details This returns the name of a scene. 
					 * @return Name 
					 * @~Japanese
					 * @brief シーンの名前を返す 
					 * @details シーンの名前を返します。 
					 * @return 名前 
					 */
					virtual std::string  getName(void) const = 0; 

					/*!
					 * @~English
					 * @brief Returns the node with the specified ID 
					 * @details This returns the node with the specified ID. 
					 * @param id ID
					 * @return Node. NULL if not found. 
					 * @~Japanese
					 * @brief 指定されたIDを持つノードを返す 
					 * @details 指定されたIDを持つノードを返します。 
					 * @param id ID
					 * @return ノード。見つからなかった場合はNULL。 
					 */
					virtual Node *findNodeById(std::string id) = 0;
					/*!
					 * @~English
					 * @brief Returns the node with the specified SID 
					 * @details This returns the node with the specified SID. 
					 * @param sid SID
					 * @return Node. NULL if not found. 
					 * @~Japanese
					 * @brief 指定されたSIDを持つノードを返す 
					 * @details 指定されたSIDを持つノードを返します。 
					 * @param sid SID
					 * @return ノード。見つからなかった場合はNULL。 
					 */
					virtual Node *findNodeBySid(std::string sid) = 0;
					/*!
					 * @~English
					 * @brief Returns the node with the specified ID 
					 * @details This returns the node with the specified ID. 
					 * 
					 * @param id ID
					 * @return Node. NULL if not found. 
					 * @~Japanese
					 * @brief 指定されたIDを持つノードを返す 
					 * @details 指定されたIDを持つノードを返します。 
					 * 
					 * @param id ID
					 * @return ノード。見つからなかった場合はNULL。 
					 */
					virtual const Node *findNodeById(std::string id) const = 0;
					/*!
					 * @~English
					 * @brief Returns the node with the specified SID 
					 * @details This returns the node with the specified SID. 
					 * 
					 * @param sid SID
					 * @return Node. NULL if not found. 
					 * @~Japanese
					 * @brief 指定されたSIDを持つノードを返す 
					 * @details 指定されたSIDを持つノードを返します。 
					 * 
					 * @param sid SID
					 * @return ノード。見つからなかった場合はNULL。 
					 */
					virtual const Node *findNodeBySid(std::string sid) const = 0;

					/*!
					 * @~English
					 * @brief Returns the root node 
					 * @details This returns the root node. 
					 * 
					 * @return Root node 
					 * @~Japanese
					 * @brief ルートノードを返す 
					 * @details ルートノードを返します。 
					 * 
					 * @return ルートノード 
					 */
					virtual Node *getRootNode(void) = 0;

					/*!
					 * @~English
					 * @brief Returns the root node 
					 * @details This returns the root node. 
					 * 
					 * @return Root node 
					 * @~Japanese
					 * @brief ルートノードを返す 
					 * @details ルートノードを返します。 
					 * 
					 * @return ルートノード 
					 */
					virtual const Node *getRootNode(void) const = 0;

					virtual ~VisualScene(void){}

				};

				/*!
				 * @~English
				 * @brief Instance of a scene 
				 * @details This is the class to indicate the instance of a scene. 
				 * @~Japanese
				 * @brief シーンのインスタンス 
				 * @details シーンのインスタンスを表すクラスです。 
				 */
				class SCE_SAMPLE_UTIL_API InstanceVisualScene : public Resource
				{
				public:
					virtual ~InstanceVisualScene(void){}
					/*!
					 * @~English
					 * @brief Returns the root node 
					 * @details This returns the root node. 
					 * 
					 * @return Root node 
					 * @~Japanese
					 * @brief ルートノードを返す 
					 * @details ルートノードを返します。 
					 * 
					 * @return ルートノード 
					 */
					virtual Node *getRootNode(void) = 0;
					/*!
					 * @~English
					 * @brief Returns the root node 
					 * @details This returns the root node. 
					 * 
					 * @return Root node 
					 * @~Japanese
					 * @brief ルートノードを返す 
					 * @details ルートノードを返します。 
					 * 
					 * @return ルートノード 
					 */
					virtual const Node *getRootNode(void) const = 0; 

					/*!
					 * @~English
					 * @brief Draws a scene. 
					 * @details This draws a scene. 
					 * 
					 * @param graphicsContext Pointer to a graphics context
					 * @param matrix When drawing a node with Mesh, the product of the scene local World matrix of a node and this value is set in the shader parameter.
					 * @param renderingStage Rendering stage number. This draws only EffectInstance whose rendering stage matches this number. The rendering stage number of EffectInstance included in a collada file is initialized by 0.
					 * @~Japanese
					 * @brief シーンを描画する 
					 * @details シーンを描画します。 
					 * 
					 * @param graphicsContext グラフィックスコンテキストへのポインタ
					 * @param matrix Meshを持つノードの描画時に、ノードのシーンローカルのWorld行列にこの値をかけた値をWorld行列としてシェーダパラメータに設定します。
					 * @param renderingStage レンダリングスステージ番号。この番号とレンダリングステージが一致するEffectInstanceのみ描画します。colladaファイルに含まれるEffectInstanceのレンダリングステージ番号は0で初期化されます。
					 */
					virtual void draw(GraphicsContext *graphicsContext, 
						sce::Vectormath::Simd::Aos::Matrix4_arg matrix=sce::Vectormath::Simd::Aos::Matrix4::identity(),
						uint32_t renderingStage=0) const = 0;

					/*!
					 * @~English
					 * @brief Changes the reference destination of a shader parameter. 
					 * 
					 * @details This changes the reference destination of a parameter used for shader drawing in a scene. getParamValue() of reference is called to all parameters, and the reference destination of a parameter to which a value other than NULL was returned is changed. 
					 * 
					 * @param reference The reference destination of a shader parameter
					 * @~Japanese
					 * @brief シェーダパラメータの参照先の変更 
					 * 
					 * @details シーン内のシェーダ描画に使われるパラメータの参照先を変更します。全てのパラメータに対して、referenceのgetParamValue()が呼ばれ、NULL以外の値が返ったパラメータの参照先を変更します。 
					 * 
					 * @param reference シェーダパラメータの参照先
					 */
					virtual void applyParameterReference(ShaderParameterReference *reference) = 0;

					/*!
					 * @~English
					 * @brief Adds an effect. 
					 * 
					 * @details This adds the effect specified in the mesh in a scene. This is used for drawing only when renderingStage specified by draw matches. 
					 * @param effect The effect to be added
					 * @param renderingStage Rendering stage to draw
					 * @retval SCE_OK Success
					 * @retval <SCE_OK Error
					 * @~Japanese
					 * @brief エフェクトの追加 
					 * 
					 * @details シーン内のメッシュに指定されたエフェクトを追加します。drawで指定されるrenderingStageが一致したときのみ描画に使用されます。 
					 * @param effect 追加するエフェクト
					 * @param renderingStage 描画を行うレンダリングステージ
					 * @retval SCE_OK 成功
					 * @retval <SCE_OK エラー
					 */
					virtual int addEffect(const Graphics::Effect::EffectData *effect, int renderingStage) = 0;

					/*!
					 * @~English
					 * @brief Returns the node with the specified ID 
					 * 
					 * @details This returns the node with the specified ID. This returns NULL if not found. 
					 * @param id ID of a node
					 * @return Node 
					 * @~Japanese
					 * @brief 指定されたIDを持つノードを返す 
					 * 
					 * @details 指定されたIDを持つノードを返します。みつからなかった場合はNULLを返します。 
					 * @param id ノードのID
					 * @return ノード 
					 */
					virtual Node *findNodeById(std::string id) = 0;
					/*!
					 * @~English
					 * @brief Returns the node with the specified SID 
					 * 
					 * @details This returns the node with the specified SID. This returns NULL if not found. 
					 * @param sid SID of a node
					 * @return Node 
					 * @~Japanese
					 * @brief 指定されたSIDを持つノードを返す 
					 * 
					 * @details 指定されたSIDを持つノードを返します。みつからなかった場合はNULLを返します。 
					 * @param sid ノードのSID
					 * @return ノード 
					 */
					virtual Node *findNodeBySid(std::string sid) = 0;

					/*!
					 * @~English
					 * @brief Returns the node with the specified ID 
					 * 
					 * @details This returns the node with the specified ID. This returns NULL if not found. 
					 * @param id ID of a node
					 * @return Node 
					 * @~Japanese
					 * @brief 指定されたIDを持つノードを返す 
					 * 
					 * @details 指定されたIDを持つノードを返します。みつからなかった場合はNULLを返します。 
					 * @param id ノードのID
					 * @return ノード 
					 */
					virtual const Node *findNodeById(std::string id) const = 0;

					/*!
					 * @~English
					 * @brief Returns the node with the specified SID 
					 * 
					 * @details This returns the node with the specified SID. This returns NULL if not found. 
					 * @param sid SID of a node
					 * @return Node 
					 * @~Japanese
					 * @brief 指定されたSIDを持つノードを返す 
					 * 
					 * @details 指定されたSIDを持つノードを返します。みつからなかった場合はNULLを返します。 
					 * @param sid ノードのSID
					 * @return ノード 
					 */
					virtual const Node *findNodeBySid(std::string sid) const = 0;

					/*!
					 * @~English
					 * @brief Returns ViusalScene which was the source. 
					 * @details This returns ViusalScene which was the source. 
					 * 
					 * @return VisualScene 
					 * @~Japanese
					 * @brief 元となったViusalSceneを返す 
					 * @details 元となったViusalSceneを返します。 
					 * 
					 * @return VisualScene 
					 */
					virtual const VisualScene *getVisualScene(void) const = 0;
				};

				/*!
				 * @~English
				 * @brief Animation player 
				 * 
				 * @details This transfers the node of InstanceVisualScene according to the data of Animation and animate it. 
				 * @~Japanese
				 * @brief アニメーションプレイヤー 
				 * 
				 * @details InstanceVisualSceneのノードをAnimationのデータに従い移動させ、アニメーションさせます。 
				 */
				class SCE_SAMPLE_UTIL_API AnimationPlayer : public Resource
				{

				public:
					/*!
					 * @~English
					 * @brief Moves the node of a target scene to the position of the specified time. 
					 * 
					 * @details This changes the transform of the node of instanceVisualScene specified at the time of initialization to the value of the specified time of animation data. 
					 * @details If the time is out of range, the nearest valid time will be used. 
					 * 
					 * @param time Animation time
					 * @~Japanese
					 * @brief 対象のシーンのノードを指定された時刻の位置に移動する 
					 * 
					 * @details 初期化時に指定されたinstanceVisualSceneのノードのトランスフォームを、 アニメーションデータの指定された時刻の値に変更します。 
					 * @details 時刻が範囲外の場合、最も近い有効な時刻が使用されます。 
					 * 
					 * @param time アニメーション時刻
					 */
					virtual void setTime(float time) const = 0;

					/*!
					 * @~English
					 * @brief Moves the node of a target scene to the position of the specified frame. 
					 * 
					 * @details This changes the transform of the node of instanceVisualScene specified at the time of initialization to the value of the specified frame of animation data. 
					 * @details If the time is out of range, the nearest valid frame will be used. This function is synonymous with setTime (starting time + frame period * frame). 
					 * 
					 * @param frame Animation frame number
					 * @~Japanese
					 * @brief 対象のシーンのノードを指定されたフレームの位置に移動する 
					 * 
					 * @details 初期化時に指定されたinstanceVisualSceneのノードのトランスフォームを、 アニメーションデータの指定されたフレームの値に変更します。 
					 * @details 時刻が範囲外の場合、最も近い有効なフレームが使用されます。この関数はsetTime(開始時刻 + フレーム周期 * frame) と同義です。 
					 * 
					 * @param frame アニメーションフレーム番号
					 */
					virtual void setFrame(uint32_t frame) const = 0;

					virtual ~AnimationPlayer(void){}

					/*!
					 * @~English
					 * @brief Returns animation data. 
					 * 
					 * @details This returns the animation data used at the time of initialization. 
					 * @return Pointer to Animation. NULL if not found. 
					 * 
					 * @~Japanese
					 * @brief アニメーションデータを返す 
					 * 
					 * @details 初期化時に使用されたアニメーションデータを返します。 
					 * @return Animationへのポインタ。見つからなかった場合はNULL。 
					 * 
					 */
					virtual const Animation* getAnimation(void) const = 0;
				};


				/*!
				 * @~English
				 * @brief Generate InstanceVisualScene 
				 * 
				 * @details This generates an instance with the specified scene information as the source. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * @param outInstanceVisualScene Pointer to which the generated InstanceVisualScene returns
				 * @param visualScene Source scene
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief InstanceVisualSceneを生成する 
				 * 
				 * @details 指定されたシーン情報を元にインスタンスを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * @param outInstanceVisualScene 生成されたInstanceVisualSceneが返るポインタ
				 * @param visualScene 元となるシーン
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				SCE_SAMPLE_UTIL_API  int createInstanceVisualScene(InstanceVisualScene **outInstanceVisualScene, 
					const VisualScene *visualScene);

				/*!
				 * @~English
				 * @brief This generates an animation player. 
				 * 
				 * @details This generates an animation player. This changes the node of the scene specified in instanceVisualScene based on animation. VisualScene which was the source of instanceVisualScene needs to be the same as VisualScene used at the time of initialization of animation. To delete the generated instance, use sce::SampleUtil::destroy(). 
				 * 
				 * @param outAnimationPlayer Pointer returned by the animation player generated
				 * @param instanceVisualScene instanceVisualScene of an animation target
				 * @param animation Animation data
				 * @retval SCE_OK Success
				 * @retval <SCE_OK Error
				 * @~Japanese
				 * @brief アニメーションプレイヤーを生成します。 
				 * 
				 * @details アニメーションプレイヤーを生成します。instanceVisualSceneに指定されたシーンのノードをanimationに基づき変動させます。instanceVisualSceneの元となったVisualSceneと animationの初期化時に使用されたVisualSceneは同一である必要があります。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。 
				 * 
				 * @param outAnimationPlayer 生成されたアニメーションプレイヤーが返るポインタ
				 * @param instanceVisualScene アニメーション対象のInstanceVisualScene
				 * @param animation アニメーションデータ
				 * @retval SCE_OK 成功
				 * @retval <SCE_OK エラー
				 */
				SCE_SAMPLE_UTIL_API  int createAnimationPlayer(AnimationPlayer **outAnimationPlayer, 
					InstanceVisualScene *instanceVisualScene, 
					const Animation *animation);


			}
		}
	}
}


#endif /* _SCE_SAMPLE_UTIL_CPP_GRAPHICS_COLLADA_SCENE_H */
