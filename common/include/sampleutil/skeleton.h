/* SIE CONFIDENTIAL
PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#ifndef _SCE_SAMPLE_UTIL_SKELETON_H
#define _SCE_SAMPLE_UTIL_SKELETON_H

#include <sampleutil/graphics.h>
#include <sampleutil/input.h>
#include <sampleutil/audio.h>
#include <sampleutil/system.h>
#include <sampleutil/sampleutil_common.h>
#include <fios2.h>


namespace sce
{
	namespace SampleUtil
	{
		/*!
		 * @~English
		 * @brief Sample skeleton 
		 * @details This is the sample skeleton which has a sample utility function. 
		 * @~Japanese
		 * @brief サンプルスケルトン 
		 * @details サンプルユーティリティの機能を持つサンプルスケルトンです。 
		 */
		class SCE_SAMPLE_UTIL_API SampleSkeleton
		{
		public:
			/*!
			 * @~English
			 * @brief Constructor 
			 * @details This is a constructor. 
			 * @~Japanese
			 * @brief コンストラクタ 
			 * @details コンストラクタです。 
			 */
			SampleSkeleton(void);

			/*!
			 * @~English
			 * @brief Destructor 
			 * @details This is a destructor. 
			 * @~Japanese
			 * @brief デストラクタ 
			 * @details デストラクタです。 
			 */
			virtual ~SampleSkeleton(void);

			// Pure virtuals - application dependent

			/*!
			 * @~English
			 * @brief Initialization 
			 * @details This function defines the initialization processing. This is overridden by an application class for each sample. 
			 * @retval SCE_OK Success
			 * @retval (<0) Error code
			 * @~Japanese
			 * @brief 初期化 
			 * @details 初期化処理を定義する関数です。サンプルごとにアプリケーションクラスでオーバーライドされます。 
			 * @retval SCE_OK 成功
			 * @retval (<0) エラーコード
			 */
			virtual int initialize(void) = 0;

			/*!
			 * @~English
			 * @brief Update 
			 * @details This function defines the update processing. This is overridden by an application class for each sample. 
			 * @retval SCE_OK Success
			 * @retval (<0) Error code
			 * @~Japanese
			 * @brief 更新 
			 * @details 更新処理を定義する関数です。サンプルごとにアプリケーションクラスでオーバーライドされます。 
			 * @retval SCE_OK 成功
			 * @retval (<0) エラーコード
			 */
			virtual int update(void) = 0;

			/*!
			 * @~English
			 * @brief Drawing 
			 * @details This function defines the render processing. This is overridden by an application class for each sample. 
			 * @~Japanese
			 * @brief 描画 
			 * @details 描画処理を定義する関数です。サンプルごとにアプリケーションクラスでオーバーライドされます。 
			 */
			virtual void render(void) = 0;

			/*!
			 * @~English
			 * @brief Termination processing 
			 * @details This function defines the termination processing. This is overridden by an application class for each sample. 
			 * @retval SCE_OK Success
			 * @retval (<0) Error code
			 * @~Japanese
			 * @brief 終了処理 
			 * @details 終了処理を定義する関数です。サンプルごとにアプリケーションクラスでオーバーライドされます。 
			 * @retval SCE_OK 成功
			 * @retval (<0) エラーコード
			 */
			virtual int finalize(void) = 0;

			/*!
			 * @~English
			 * 
			 * @brief The enum value of the flag of the function to be used in SampleSkeleton 
			 * @details Specify this for initializeUtil(). 
			 * @~Japanese
			 * 
			 * @brief SampleSkeletonにて使用する機能のフラグのenum値 
			 * @details initializeUtil()に指定します。 
			 */
			enum SampleSkeletonFunctionFlag {

				/*!
				 * @~English
				 * @brief Graphics function flag value 
				 * @~Japanese
				 * @brief グラフィックス機能フラグ値 
				 */
				kFunctionFlagGraphics = (1<<0),

				/*!
				 * @~English
				 * @brief 2D sprite rendering function flag value 
				 * @~Japanese
				 * @brief 2Dスプライト描画機能フラグ値 
				 */
				kFunctionFlagSpriteRenderer = (1<<1),
				/*!
				 * @~English
				 * @brief Audio function flag value 
				 * @~Japanese
				 * @brief オーディオ機能フラグ値 
				 */
				kFunctionFlagAudio = (1<<2),
				/*!
				 * @~English
				 * @brief FIOS2 function flag value 
				 * @~Japanese
				 * @brief FIOS2機能フラグ値 
				 */
				kFunctionFlagFios2 = (1<<3),
				/*!
				 * @~English
				 * @brief User ID manager flag value 
				 * @~Japanese
				 * @brief ユーザーIDマネージャーフラグ値 
				 */
				kFunctionFlagUserIdManager = (1<<4),

				/*!
				 * @~English
				 * @brief Controller function flag value of the initial user 
				 * @~Japanese
				 * @brief Initial userのコントローラー機能フラグ値 
				 */
				kFunctionFlagPadOfInitialUser = (1<<5),

				/*!
				 * @~English
				 * @brief Default function (graphics function, 2D sprite rendering function, user ID manager) flag value 
				 * @~Japanese
				 * @brief デフォルト機能 (グラフィックス機能、2Dスプライト描画機能、ユーザーIDマネージャー) フラグ値 
				 */
				kFunctionFlagDefault = (1<<0) |
										(1<<1) |
										(1<<4),
				/*!
				 * @~English
				 * @brief All function flag value 
				 * @~Japanese
				 * @brief 全機能フラグ値 
				 */
				kFunctionFlagAll = (1<<0) |
									(1<<1) |
									(1<<2) |
									(1<<3) |
									(1<<4) |
									(1<<5)
			};

			/*!
			 * @~English
			 * 
			 * @brief Sample skeleton option 
			 * @details This is the sample skeleton option. 
			 * @~Japanese
			 * 
			 * @brief サンプルスケルトンオプション 
			 * @details サンプルスケルトンオプションです。 
			 */
			struct SampleSkeletonOption
			{
			public:
				/*!
				 * @~English
				 * @brief Graphics option 
				 * @~Japanese
				 * @brief グラフィックスオプション 
				 */
				Graphics::GraphicsContextOption* graphicsOption;

				/*!
				 * @~English
				 * @brief Audio option 
				 * @~Japanese
				 * @brief オーディオオプション 
				 */
				Audio::AudioContextOption* audioOption;

				/*!
				 * @~English
				 * @brief FIOS2 option 
				 * @~Japanese
				 * @brief FIOS2オプション 
				 */
				SceFiosParams*			   fios2Option;

				/*!
				 * @~English
				 * @brief PadContext option 
				 * @~Japanese
				 * @brief PadContextオプション 
				 */
				Input::PadContextOption* padOption;

				/*!
				 * @~English
				 * @brief Constructor 
				 * @details This is a constructor. 
				 * @~Japanese
				 * @brief コンストラクタ 
				 * @details コンストラクタです。 
				 */
				SampleSkeletonOption(void);
			};

		protected:
			/*!
			 * @~English
			 * @brief Initializes a utility. 
			 * @details This initializes each function of the utility in accordance with functionFlags. For use of the 2D sprite rendering function (kFunctionFlagSpriteRenderer), the graphics function (kFunctionFlagGraphics) is required. For streaming with the audio function (kFunctionFlagAudio), the FIOS2 function (kFunctionFlagFios2) is required. To use the controller feature of the initial user (kFunctionFlagPadOfInitialUser), the user ID manager (kFunctionFlagUserIdManager) is required. The controller feature of the initial user (kFunctionFlagPadOfInitialUser) cannot be used with applications that have the InitialUserAlwaysLoggedIn flag set to disabled in param.sfo. When the InitialUserAlwaysLoggedIn flag is set to disabled in param.sfo and kFunctionFlagPadOfInitialUser is specified, SCE_USER_SERVICE_ERROR_OPERATION_NOT_SUPPORTED will return.  If displayHeight or displayWidth is smaller than zero, the resolution is set automatically.
			 * @param functionFlags Flag value for the function to use
			 * @param displayWidth Width of the main render target displayed on the display
			 * @param displayHeight Height of the main render target displayed on the display
			 * @param option Pointer to the SampleSkeletonOption structure. This is initialized by the default value if NULL is specified.
			 * @retval SCE_OK Success
			 * @retval (<0) Error code
			 * @~Japanese
			 * @brief ユーティリティの初期化 
			 * @details functionFlagsの指定に従い、ユーティリティの各機能の初期化を行います。2Dスプライト描画機能(kFunctionFlagSpriteRenderer)を使用するには、グラフィックス機能(kFunctionFlagGraphics)が必要です。オーディオ機能(kFunctionFlagAudio)のストリーミング再生には、FIOS2機能(kFunctionFlagFios2)が必要です。Initial userのコントローラー機能(kFunctionFlagPadOfInitialUser)を使用するには、ユーザーIDマネージャー(kFunctionFlagUserIdManager)が必要です。Initial userのコントローラー機能(kFunctionFlagPadOfInitialUser)は、param.sfoでInitialUserAlwaysLoggedInフラグを無効に設定したアプリケーションでは使用できません。param.sfoでInitialUserAlwaysLoggedInフラグを無効に設定し、kFunctionFlagPadOfInitialUserを指定すると、SCE_USER_SERVICE_ERROR_OPERATION_NOT_SUPPORTEDが返されます。 displayWidth, displayHeightのいずれかが負の値の場合解像度は自動的に選ばれます。
			 * @param functionFlags 使用する機能のフラグ値
			 * @param displayWidth ディスプレイに表示するメインレンダーターゲットの幅
			 * @param displayHeight ディスプレイに表示するメインレンダーターゲットの高さ
			 * @param option SampleSkeletonOption構造体のポインタ。NULLを指定するとデフォルトの値で初期化されます。
			 * @retval SCE_OK 成功
			 * @retval (<0) エラーコード
			 */
			int initializeUtil(uint32_t functionFlags=kFunctionFlagDefault,
				int32_t displayWidth=-1,
				int32_t displayHeight=-1,
				SampleSkeletonOption* option = NULL);

			/*!
			 * @~English
			 * @brief Termination processing of a utility. 
			 * @details This executes termination processing of a utility. 
			 * @retval SCE_OK Success
			 * @retval (<0) Error code
			 * @~Japanese
			 * @brief ユーティリティの終了処理 
			 * @details ユーティリティの終了処理を行います。 
			 * @retval SCE_OK 成功
			 * @retval (<0) エラーコード
			 */
			int finalizeUtil(void);

			/*!
			 * @~English
			 * @brief Update utility 
			 * @details This updates a utility. 
			 * @retval SCE_OK Success
			 * @retval (<0) Error code
			 * @~Japanese
			 * @brief ユーティリティの更新 
			 * @details ユーティリティを更新します。 
			 * @retval SCE_OK 成功
			 * @retval (<0) エラーコード
			 */
			int updateUtil(void);

			/*!
			 * @~English
			 * @brief Get graphics context 
			 * @details This gets a graphics context. If kFunctionFlagGraphics is not specified at the time of initialization, NULL will be returned. 
			 * @return Graphics context 
			 * @~Japanese
			 * @brief グラフィックスコンテキストの取得 
			 * @details グラフィックスコンテキストを取得します。初期化時にkFunctionFlagGraphicsが指定されていない場合、NULLが返ります。 
			 * @return グラフィックスコンテキスト 
			 */
			sce::SampleUtil::Graphics::GraphicsContext *getGraphicsContext(void);
			
			/*!
			 * @~English
			 * @brief Get 2D sprite rendering context 
			 * @details This gets a 2D sprite rendering context. If kFunctionFlagSpriteRenderer is not specified at the time of initialization, NULL will be returned. 
			 * @return 2D sprite rendering context 
			 * @~Japanese
			 * @brief 2Dスプライト描画コンテキストの取得 
			 * @details 2Dスプライト描画コンテキストを取得します。初期化時にkFunctionFlagSpriteRendererが指定されていない場合、NULLが返ります。 
			 * @return 2Dスプライト描画コンテキスト 
			 */
			sce::SampleUtil::Graphics::SpriteRenderer *getSpriteRenderer(void);

			/*!
			 * @~English
			 * @brief Get audio context 
			 * @details This gets an audio context. If kFunctionFlagAudio is not specified at the time of initialization, NULL will be returned. 
			 * @return Audio context 
			 * @~Japanese
			 * @brief オーディオコンテキストの取得 
			 * @details オーディオコンテキストを取得します。初期化時にkFunctionFlagAudioが指定されていない場合、NULLが返ります。 
			 * @return オーディオコンテキスト 
			 */
			sce::SampleUtil::Audio::AudioContext *getAudioContext(void);

			/*!
			 * @~English
			 * @brief Get user ID manager 
			 * @details This gets the user ID manager. If kFunctionFlagUserIdManager is not specified at the time of initialization, NULL will be returned. 
			 * @return User ID manager 
			 * @~Japanese
			 * @brief ユーザーIDマネージャーの取得 
			 * @details ユーザーIDマネージャーを取得します。初期化時にkFunctionFlagUserIdManagerが指定されていない場合、NULLが返ります。 
			 * @return ユーザーIDマネージャー 
			 */
			sce::SampleUtil::System::UserIdManager *getUserIdManager(void);

			/*!
			 * @~English
			 * @brief Get pad context of initial user 
			 * @details This gets the pad context of the initial user. If kFunctionFlagPadOfInitialUser is not specified at the time of initialization, NULL will be returned. 
			 * @return Pad context of initial user 
			 * @~Japanese
			 * @brief Initial userのパッドコンテキストの取得 
			 * @details Initial userのパッドコンテキストを取得します。初期化時にkFunctionFlagPadOfInitialUserが指定されていない場合、NULLが返ります。 
			 * @return Initial userのパッドコンテキスト 
			 */
			sce::SampleUtil::Input::PadContext *getPadContextOfInitialUser(void);

		private:
			uint32_t m_functionFlags;
			sce::SampleUtil::Graphics::GraphicsContext *m_graphicsContext;
			sce::SampleUtil::Graphics::SpriteRenderer *m_spriteRenderer;
			sce::SampleUtil::Audio::AudioContext *m_audioContext;
			sce::SampleUtil::System::UserIdManager *m_userIdManager;
			sce::SampleUtil::Input::PadContext *m_padContextOfInitialUser;
			
			unsigned char* m_pStorageForFios2;

			int32_t initializeFios2(SceFiosParams *params);

			int32_t finalizeFios2(void);
		};
	}
}

#endif /* _SCE_SAMPLE_UTIL_SKELETON_H */
