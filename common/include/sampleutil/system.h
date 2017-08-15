/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _SCE_SAMPLE_UTIL_SYSTEM_H
#define _SCE_SAMPLE_UTIL_SYSTEM_H

#include <sampleutil/sampleutil_common.h>
#include <sampleutil/sampleutil_error.h>

#if defined(_SCE_TARGET_OS_WINDOWS) && _SCE_TARGET_OS_WINDOWS

   #if defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS
      #include <pad.h>
   #else/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */
      #include <windows.h>
      #ifndef DIRECTINPUT_VERSION
         #define DIRECTINPUT_VERSION 0x0800 /* DIRECTINPUT_HEADER_VERSION */
      #endif/*DIRECTINPUT_VERSION*/
   
      #include <dinput.h>
      #include <dinputd.h>
   #endif/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */
   
#endif//defined(_SCE_TARGET_OS_WINDOWS) && _SCE_TARGET_OS_WINDOWS


#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS

#include <user_service.h>
#include <save_data.h>
#include <rtc.h>
#define SCE_SAMPLE_UTIL_MAX_LOGIN_USERS SCE_USER_SERVICE_MAX_LOGIN_USERS
#define SCE_SAMPLE_UTIL_USER_ID_INVALID SCE_USER_SERVICE_USER_ID_INVALID
#define SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE SCE_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_TITLE_MAXSIZE		SCE_SAVE_DATA_TITLE_MAXSIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_SUBTITLE_MAXSIZE	SCE_SAVE_DATA_SUBTITLE_MAXSIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_DETAIL_MAXSIZE	SCE_SAVE_DATA_DETAIL_MAXSIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_TITLE_ID_DATA_SIZE	SCE_SAVE_DATA_TITLE_ID_DATA_SIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_FINGERPRINT_DATA_SIZE	SCE_SAVE_DATA_FINGERPRINT_DATA_SIZE

#endif/*defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS*/

#if defined(_SCE_TARGET_OS_PSP2) && _SCE_TARGET_OS_PSP2

#include <apputil.h>
#define SCE_SAMPLE_UTIL_MAX_LOGIN_USERS 1
#define SCE_SAMPLE_UTIL_USER_ID_INVALID -1
#define SCE_SAMPLE_UTIL_USER_ID_DEFAULT 0
#define SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE SCE_APPUTIL_MOUNTPOINT_DATA_MAXSIZE+8
#define SCE_SAMPLE_UTIL_SAVE_DATA_TITLE_MAXSIZE		SCE_APPUTIL_SAVEDATA_SLOT_TITLE_MAXSIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_SUBTITLE_MAXSIZE	SCE_APPUTIL_SAVEDATA_SLOT_SUBTITLE_MAXSIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_DETAIL_MAXSIZE	SCE_APPUTIL_SAVEDATA_SLOT_DETAIL_MAXSIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_ICON_PATH_MAXSIZE	SCE_APPUTIL_SAVEDATA_SLOT_ICON_PATH_MAXSIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_TITLE_ID_DATA_SIZE	SCE_APPUTIL_TITLE_ID_DATA_SIZE
#define SCE_SAMPLE_UTIL_SAVE_DATA_PASSCODE_DATA_SIZE	SCE_APPUTIL_PASSCODE_DATA_SIZE

#endif //defined(_SCE_TARGET_OS_PSP2) && _SCE_TARGET_OS_PSP2

#if defined(_SCE_TARGET_OS_WINDOWS) && _SCE_TARGET_OS_WINDOWS

#define SCE_SAMPLE_UTIL_MAX_LOGIN_USERS 4
#define SCE_SAMPLE_UTIL_USER_ID_INVALID NULL
#define SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE 32
#define SCE_SAMPLE_UTIL_SAVE_DATA_TITLE_MAXSIZE		64
#define SCE_SAMPLE_UTIL_SAVE_DATA_SUBTITLE_MAXSIZE	128
#define SCE_SAMPLE_UTIL_SAVE_DATA_DETAIL_MAXSIZE	512
#define SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_ICON_PATH_MAXSIZE	1024
#define SCE_SAMPLE_UTIL_SAVE_DATA_TITLE_ID_DATA_SIZE	10
#define SCE_SAMPLE_UTIL_SAVE_DATA_PASSCODE_DATA_SIZE	32

#endif //defined(_SCE_TARGET_OS_WINDOWS) && _SCE_TARGET_OS_WINDOWS

#define SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX	256

namespace sce
{
	namespace SampleUtil
	{

		/*!
		 * @~English
		 * @brief System-associated definitions 
		 * @details These are the System-associated definitions. 
		 * @~Japanese
		 * @brief System関連の定義 
		 * @details System関連の定義です。 
		 */
		namespace System
		{
			/*!
			 * @~English
			 * 
			 * @brief User ID type 
			 * @details This is the user ID type. 
			 * @~Japanese
			 * 
			 * @brief ユーザーIDの型 
			 * @details ユーザーIDの型です。 
			 */
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS
			typedef SceUserServiceUserId UserId;
			static const UserId kInvalidUserId = SCE_USER_SERVICE_USER_ID_INVALID;
            static const size_t kMaxUserNameLength = SCE_USER_SERVICE_MAX_USER_NAME_LENGTH;
            static const uint32_t kMaxLoginUsers   = SCE_USER_SERVICE_MAX_LOGIN_USERS;
#endif //defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS
#if defined _SCE_TARGET_OS_PSP2 && _SCE_TARGET_OS_PSP2
			typedef int UserId;
			static const UserId kInvalidUserId = -1;
			static const size_t kMaxUserNameLength = 16;
            static const uint32_t kMaxLoginUsers   = 1;
#endif //defined _SCE_TARGET_OS_PSP2 && _SCE_TARGET_OS_PSP2
#if defined _SCE_TARGET_OS_WINDOWS && _SCE_TARGET_OS_WINDOWS
#if defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS
			typedef SceUserServiceUserId UserId;
#else/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */
			typedef DIDEVICEINSTANCE *UserId;
#endif/* defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS */
			static const UserId kInvalidUserId = NULL;
			static const size_t kMaxUserNameLength = 16;
            static const uint32_t kMaxLoginUsers   = 1;
#endif //defined _SCE_TARGET_OS_WINDOWS &&  _SCE_TARGET_OS_WINDOWS

			/*!
			 * @~English
			 * @brief Structure that stores the user ID list 
			 * @details This structure stores the user ID list. 
			 * @~Japanese
			 * @brief ユーザーIDリストを格納する構造体 
			 * @details ユーザーIDリストを格納する構造体です。 
			 */
			struct UserIdList {
				/*!
				 * @~English
				 * @brief This is the user ID list. 
				 * @~Japanese
				 * @brief ユーザーIDリストです。 
				 */
				UserId userId[SCE_SAMPLE_UTIL_MAX_LOGIN_USERS];
			};

			/*!
			 * @~English
			 * @brief Structure that stores the user login status (logged in/logged out) change events 
			 * @details This structure stores the user login status (logged in/logged out) change events. 
			 * @~Japanese
			 * @brief ユーザーのログイン状態(ログイン/ログアウト)の変更イベントを格納する構造体 
			 * @details ユーザーのログイン状態(ログイン/ログアウト)の変更イベントを格納する構造体です。 
			 */
			struct UserLoginStatusChangedEvents {
				/*!
				 * @~English
				 * @brief User ID list of logged in users that joined the game. 
				 * @~Japanese
				 * @brief ログインしてゲームに参加したユーザーのユーザーIDリストです。 
				 */
				UserIdList joinedUserIdList;
				/*!
				 * @~English
				 * @brief User ID list of users that have logged out and left the game. 
				 * @~Japanese
				 * @brief ログアウトしてゲームから抜けたユーザーのユーザーIDリストです。 
				 */
				UserIdList leftUserIdList;
			};

			/*!
			 * @~English
			 * 
			 * @brief Enumerator type for user colors 
			 * @details This is the enumerator type for user colors. 
			 * @~Japanese
			 * 
			 * @brief ユーザーカラーの列挙型 
			 * @details ユーザーカラーの列挙型です。 
			 */
			enum UserColor
			{
				/*!
				 * @~English
				 * @brief Blue 
				 * @~Japanese
				 * @brief 青 
				 */
				kUserColorBlue = 0,
				/*!
				 * @~English
				 * @brief Red 
				 * @~Japanese
				 * @brief 赤 
				 */
				kUserColorRed = 1,
				/*!
				 * @~English
				 * @brief Green 
				 * @~Japanese
				 * @brief 緑 
				 */
				kUserColorGreen = 2,
				/*!
				 * @~English
				 * @brief Pink 
				 * @~Japanese
				 * @brief ピンク 
				 */
				kUserColorPink = 3
			};

			/*!
			 * @~English
			 * 
			 * @brief Class for managing user IDs 
			 * @details UserIdManager is a class for supporting the management of user IDs. 
			 * @~Japanese
			 * 
			 * @brief ユーザーIDを管理するためのクラス 
			 * @details UserIdManagerはユーザーIDの管理をサポートするためのクラスです。 
			 */
			class SCE_SAMPLE_UTIL_API UserIdManager : public Resource
			{
			public:
				/*!
				 * @~English
				 * @brief Destructor 
				 * @details This is a destructor. 
				 * @~Japanese
				 * @brief デストラクタ 
				 * @details デストラクタです。 
				 */
				virtual ~UserIdManager(void){}

				/*!
				 * @~English
				 * 
				 * @brief Update 
				 * @details This is the update function of UserIdManager. The UserIdManager user ID list updates are performed in this function. 
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * 
				 * @brief 更新 
				 * @details UserIdManagerの更新関数です。本関数内でUserIdManagerのユーザーIDリストの更新が行われます。 
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int update(void) = 0;

				/*!
				 * @~English
				 * 
				 * @brief Get user ID list of logged in users 
				 * @param list UserIdList pointer
				 * @details This gets the user ID list of the logged in users. 
				 * 
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * 
				 * @brief ログインしているユーザーのユーザーIDリストの取得 
				 * @param list UserIdListのポインタ
				 * @details ログインしているユーザーのユーザーIDリストを取得します。 
				 * 
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int getLoginUserIdList(UserIdList *list) = 0;

				/*!
				 * @~English
				 * 
				 * @brief Get user login status (logged in/logged out) change event for last update 
				 * @param events UserLoginStatusChangedEvents pointer
				 * @details This gets the user login status (logged in/logged out) change event for the last update. 
				 * 
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * 
				 * @brief 最後に更新した際のユーザーのログイン状態(ログイン/ログアウト)の変更イベントの取得 
				 * @param events UserLoginStatusChangedEventsのポインタ
				 * @details 最後に更新した際のユーザーのログイン状態(ログイン/ログアウト)の変更イベントを取得します。 
				 * 
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int getUserLoginStatusChangedEventsOfLastUpdate(UserLoginStatusChangedEvents *events) = 0;

				/*!
				 * @~English
				 * 
				 * @brief Obtains the user ID of the user that started the game 
				 * @param userId Pointer that stores user IDs
				 * @details Obtains the user ID of the user that started the game. In PlayStation(R)4, this function cannot be used with applications that have the InitialUserAlwaysLoggedIn flag set to disabled in param.sfo, and SCE_USER_SERVICE_ERROR_OPERATION_NOT_SUPPORTED will return. 
				 * 
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * 
				 * @brief ゲームを起動したユーザーのユーザーIDの取得 
				 * @param userId ユーザーIDを格納するポインタ
				 * @details ゲームを起動したユーザーのユーザーIDを取得します。PlayStation(R)4では、この関数はparam.sfoでInitialUserAlwaysLoggedInフラグを無効に設定したアプリケーションでは使用できず、SCE_USER_SERVICE_ERROR_OPERATION_NOT_SUPPORTEDが返されます。 
				 * 
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int getInitialUser(UserId *userId) = 0;

				/*!
				 * @~English
				 * 
				 * @brief Obtains the user name 
				 * @param userId User ID
				 * @param userName Pointer for the array to store the user name
				 * @param size Size of userName
				 * @details Gets the user name. 
				 * 
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * 
				 * @brief ユーザー名の取得 
				 * @param userId ユーザーID
				 * @param userName ユーザー名を格納する配列のポインタ
				 * @param size userNameのサイズ
				 * @details ユーザー名を取得します。 
				 * 
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int getUserName(const UserId userId, char *userName, const size_t size) = 0;

				/*!
				 * @~English
				 * 
				 * @brief Get user color 
				 * @param userId User ID
				 * @param userColor Pointer that stores user color
				 * @details This obtains the user color (color allocated by the system to a user on PlayStation(R)4). Regarding user colors on PlayStation(R)4, refer to "User Management Overview" document. 
				 * 
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * 
				 * @brief ユーザーカラーの取得 
				 * @param userId ユーザーID
				 * @param userColor ユーザーカラーを格納するポインタ
				 * @details ユーザーカラー（PlayStation(R)4においてシステムがユーザーに割り当てた色）を取得します。PlayStation(R)4におけるユーザーカラーについては「ユーザー管理 概要」ドキュメントを参照してください。 
				 * 
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int getUserColor(const UserId userId, UserColor *userColor) = 0;
			};

 			/*!
 			 * @~English
 			 * 
 			 * @brief Options structure for UserIdManager 
 			 * @details This is an options structure for UserIdManager. This is used by specifying it to the option argument of createUserIdManager(). 
 			 * @~Japanese
 			 * 
 			 * @brief UserIdManagerのオプション構造体 
 			 * @details UserIdManagerのオプション構造体です。createUserIdManager()の引数optionに指定することで利用します。 
 			 */
			typedef struct UserIdManagerOption{
			} UserIdManagerOption;

			/*!
			 * @~English
			 * 
			 * @brief Initialization function of UserIdManager 
			 * @param outUserIdManager Pointer to be returned by the generated UserIdManager.
			 * @param option This is the UserIdManagerOption structure.
			 * 
			 * @retval SCE_OK Success
			 * @retval (<0) Error code
			 * @details This is the initialization function of UserIdManager. Call this first when using UserIdManager. 
			 * @~Japanese
			 * 
			 * @brief UserIdManagerの初期化関数 
			 * @param outUserIdManager 生成されたUserIdManagerが返されるポインタ。
			 * @param option UserIdManagerOption構造体。
			 * 
			 * @retval SCE_OK 成功
			 * @retval (<0) エラーコード
			 * @details UserIdManagerの初期化関数です。UserIdManager利用時に最初に呼び出してください。 
			 */
			SCE_SAMPLE_UTIL_API int createUserIdManager(UserIdManager **outUserIdManager, UserIdManagerOption *option=NULL);

			/*!
			 * @~English
			 * 
			 * @brief Save data slot ID type 
			 * @details This is the save data slot ID type. 
			 * @~Japanese
			 * 
			 * @brief セーブデータのスロットIDの型 
			 * @details セーブデータのスロットIDの型です。 
			 */
			typedef uint32_t SaveDataSlotId;

			/*!
			 * @~English
			 * 
			 * @brief Save data slot status enumerated type 
			 * @details This is the save data slot status enumerated type. 
			 * @~Japanese
			 * 
			 * @brief セーブデータのスロットステータスの列挙型 
			 * @details セーブデータのスロットステータスの列挙型です。 
			 */
			enum SaveDataSlotStatus
			{
				/*!
				 * @~English
				 * @brief Normal status 
				 * @~Japanese
				 * @brief 通常状態 
				 */
				kSaveDataSlotStatusAvailable = 0,
				/*!
				 * @~English
				 * @brief Broken status 
				 * @~Japanese
				 * @brief 破損状態 
				 */
				kSaveDataSlotStatusBroken = 1
			};

			/*!
			 * @~English
			 * 
			 * @brief Save data slot search sort key enumerated type 
			 * @details This is the save data slot search sort key enumerated type. 
			 * @~Japanese
			 * 
			 * @brief セーブデータのスロット検索ソートキーの列挙型 
			 * @details セーブデータのスロット検索ソートキーの列挙型です。 
			 */
			enum SaveDataSlotSortKey
			{
				/*!
				 * @~English
				 * @brief Use the save data slot ID as the sort key 
				 * @~Japanese
				 * @brief セーブデータスロットIDをソートキーとする 
				 */
				kSaveDataSlotSortKeySlotId = 0,
				/*!
				 * @~English
				 * @brief Use the user parameter as the sort key 
				 * @~Japanese
				 * @brief ユーザーパラメータをソートキーとする 
				 */
				kSaveDataSlotSortKeyUserParam = 1,
				/*!
				 * @~English
				 * @brief Use the data size as the sort key 
				 * @~Japanese
				 * @brief データサイズをソートキーとする 
				 */
				kSaveDataSlotSortKeySize = 2,
				/*!
				 * @~English
				 * @brief Use the last modified date/time as the sort key 
				 * @~Japanese
				 * @brief 最終更新日時をソートキーとする 
				 */
				kSaveDataSlotSortKeyMtime = 3
			};

			/*!
			 * @~English
			 * 
			 * @brief Save data slot search sort order enumerated type 
			 * @details This is the save data slot search sort order enumerated type. 
			 * @~Japanese
			 * 
			 * @brief セーブデータのスロット検索ソート順の列挙型 
			 * @details セーブデータのスロット検索ソート順の列挙型です。 
			 */
			enum SaveDataSlotSortOrder
			{
				/*!
				 * @~English
				 * @brief Sort in ascending order 
				 * @~Japanese
				 * @brief 昇順でソートする 
				 */
				kSaveDataSlotSortOrderAscent = 0,
				/*!
				 * @~English
				 * @brief Sort in descending order 
				 * @~Japanese
				 * @brief 降順でソートする 
				 */
				kSaveDataSlotSortOrderDescent = 1
			};

			/*!
			 * @~English
			 * 
			 * @brief Save data save mode enumerated type 
			 * @details This is the save data save mode enumerated type. 
			 * @~Japanese
			 * 
			 * @brief セーブデータの保存モードの列挙型 
			 * @details セーブデータの保存モードの列挙型です。 
			 */
			enum SaveDataSaveMode
			{
				/*!
				 * @~English
				 * @brief File operation 
				 * @~Japanese
				 * @brief ファイル操作 
				 */
				kSaveDataSaveModeFile = 0,
				/*!
				 * @~English
				 * @brief File operation (truncate file with the specified offset and size) 
				 * @~Japanese
				 * @brief ファイル操作（指定したオフセットとサイズでファイルを切り詰める） 
				 */
				kSaveDataSaveModeFileTruncate = 1,
				/*!
				 * @~English
				 * @brief Directory operation 
				 * @~Japanese
				 * @brief ディレクトリ操作 
				 */
				kSaveDataSaveModeDirectory = 2
			};

#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS
			/*!
			 * @~English
			 * 
			 * @brief Save data icon structure 
			 * @details This structure is for saving/loading icon images for save data. 
			 * @details When saving, specify the buffer of the icon to save for buf, specify the size of the buffer for bufSize, and specify the size of the icon data in the buffer for dataSize. 
			 * @details When not saving an icon, specify NULL for buf. 
			 * @details When loading, prepare a buffer in an appropriate size in advance, then specify it to buf and bufSize. Stores the size of the loaded icon data in dataSize for normal termination. 
			 * @details When not loading an icon, specify NULL for buf. 
			 * @~Japanese
			 * 
			 * @brief セーブデータのアイコン構造体 
			 * @details セーブデータのアイコン画像をセーブ/ロードするための構造体です。 
			 * @details セーブ時には、bufにはセーブするアイコンのバッファを指定し、bufSizeにはそのバッファのサイズ、dataSizeにはバッファ中のアイコンデータのサイズを指定してください。 
			 * @details アイコンをセーブしない場合はbufにNULLを指定してください。 
			 * @details ロード時には、あらかじめ適切なサイズのバッファを用意しておき、bufとbufSizeで指定してください。正常終了した場合、dataSizeにはロードしたアイコンデータのサイズが格納されます。 
			 * @details アイコンをロードしない場合はbufにNULLを指定してください。 
			 */
			typedef struct SaveDataIcon{
				/*!
				 * @~English
				 * @brief Icon data buffer 
				 * @~Japanese
				 * @brief アイコンデータバッファ 
				 */
				void *buf;
				/*!
				 * @~English
				 * @brief Icon data buffer size 
				 * @~Japanese
				 * @brief アイコンデータバッファのサイズ 
				 */
				size_t bufSize;
				/*!
				 * @~English
				 * @brief Icon data size 
				 * @~Japanese
				 * @brief アイコンデータサイズ 
				 */
				size_t dataSize;
			} SaveDataIcon;
#endif

#if defined(_SCE_TARGET_OS_WINDOWS) && _SCE_TARGET_OS_WINDOWS
			typedef struct DateTime {
				unsigned short year;
				unsigned short month;
				unsigned short day;
				unsigned short hour;
				unsigned short minute;
				unsigned short second;
				unsigned int microsecond;
			} DateTime;
#endif

			/*!
			 * @~English
			 * 
			 * @brief Save data slot parameter structure 
			 * @details This is the save data slot parameter structure. 
			 * @~Japanese
			 * 
			 * @brief セーブデータのスロットパラメータの構造体 
			 * @details セーブデータのスロットパラメータの構造体です。 
			 */
			typedef struct SaveDataSlotParam{
				/*!
				 * @~English
				 * @brief Save data slot status 
				 * @~Japanese
				 * @brief セーブデータスロットステータス 
				 */
				SaveDataSlotStatus status;
				/*!
				 * @~English
				 * @brief Title name (terminated with NULL, UTF-8) 
				 * @~Japanese
				 * @brief タイトル名（NULL終端、UTF-8） 
				 */
				char title[SCE_SAMPLE_UTIL_SAVE_DATA_TITLE_MAXSIZE];
				/*!
				 * @~English
				 * @brief Sub-title name (terminated with NULL, UTF-8) 
				 * @~Japanese
				 * @brief サブタイトル名（NULL終端、UTF-8） 
				 */
				char subTitle[SCE_SAMPLE_UTIL_SAVE_DATA_SUBTITLE_MAXSIZE];
				/*!
				 * @~English
				 * @brief Detailed information (terminated with NULL, UTF-8) 
				 * @~Japanese
				 * @brief 詳細情報（NULL終端、UTF-8） 
				 */
				char detail[SCE_SAMPLE_UTIL_SAVE_DATA_DETAIL_MAXSIZE];
				/*!
				 * @~English
				 * @brief User parameter 
				 * @~Japanese
				 * @brief ユーザーパラメータ 
				 */
				uint32_t userParam;
#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief Save data icon 
				 * @~Japanese
				 * @brief セーブデータアイコン 
				 */
				SaveDataIcon icon;
				/*!
				 * @~English
				 * @brief Last modified date/time 
				 * @~Japanese
				 * @brief 最終更新日時 
				 */
				SceRtcDateTime mtime;
#elif defined(_SCE_TARGET_OS_PSP2) && _SCE_TARGET_OS_PSP2
				/*!
				 * @~English
				 * @brief Thumbnail icon path (terminated with NULL) 
				 * @~Japanese
				 * @brief サムネイルアイコンパス（NULL終端） 
				 */
				char iconPath[SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_ICON_PATH_MAXSIZE];
				/*!
				 * @~English
				 * @brief Last modified date/time 
				 * @~Japanese
				 * @brief 最終更新日時 
				 */
				SceDateTime mtime;
#elif defined(_SCE_TARGET_OS_WINDOWS) && _SCE_TARGET_OS_WINDOWS
				DateTime mtime;
#endif
			} SaveDataSlotParam;

			/*!
			 * @~English
			 * 
			 * @brief Save data slot search condition structure 
			 * @details This is the save data slot search condition structure. 
			 * @~Japanese
			 * 
			 * @brief セーブデータのスロット検索条件の構造体 
			 * @details セーブデータのスロット検索条件の構造体です。 
			 */
			typedef struct SaveDataSearchCond{
				/*!
				 * @~English
				 * @brief Search start save data slot ID 
				 * @~Japanese
				 * @brief 検索開始セーブデータスロットID 
				 */
				SaveDataSlotId from;
				/*!
				 * @~English
				 * @brief Search range 
				 * @~Japanese
				 * @brief 検索範囲 
				 */
				uint32_t range;
				/*!
				 * @~English
				 * @brief Save data slot search sort key 
				 * @~Japanese
				 * @brief セーブデータスロット検索ソートキー 
				 */
				SaveDataSlotSortKey key;
				/*!
				 * @~English
				 * @brief Save data slot search sort order 
				 * @~Japanese
				 * @brief セーブデータスロット検索ソート順 
				 */
				SaveDataSlotSortOrder order;
			} SaveDataSearchCond;

			/*!
			 * @~English
			 * 
			 * @brief Save data slot search result structure 
			 * @details This is the save data slot search result structure. For slotList, specify a pointer to the array to store the slot ID list for the search results. For slotListNum, specify the number of arrays specified for slotList. 
			 * @~Japanese
			 * 
			 * @brief セーブデータのスロット検索結果の構造体 
			 * @details セーブデータのスロット検索結果の構造体です。slotListには、検索結果のスロットIDリストを格納する配列のポインタを指定してください。slotListNumにはslotListに指定した配列の数を指定してください。 
			 */
			typedef struct SaveDataSearchResult{
				/*!
				 * @~English
				 * @brief Number of search hits 
				 * @~Japanese
				 * @brief 検索結果件数 
				 */
				uint32_t hitNum;
				/*!
				 * @~English
				 * @brief Save data slot ID list 
				 * @~Japanese
				 * @brief セーブデータスロットIDリスト 
				 */
				SaveDataSlotId *slotList;
				/*!
				 * @~English
				 * @brief Number of save data slot ID lists 
				 * @~Japanese
				 * @brief セーブデータスロットIDリスト数 
				 */
				uint32_t slotListNum;
			} SaveDataSearchResult;

			/*!
			 * @~English
			 * 
			 * @brief Structure for specifying target save data to save 
			 * @details This structure specifies the target save data to save. For dataPath, specify the data path of the target save data to save. With PlayStation(R)Vita, a path with a directory hierarchy up to three levels deep can be specified for dataPath. Relative paths using ".." are not valid. For buf, specify the address of the buffer where the content of the save data to save is stored. Accordingly, specify the size of the buffer for bufSize. For offset, specify the write offset position from the file start when saving the save data content specified with buf. When specifying kSaveDataSaveModeDirectory for mode, NULL must be set for buf and 0 must be set for both bufSize and offset. 
			 * @~Japanese
			 * 
			 * @brief 保存対象セーブデータ指定用構造体 
			 * @details 保存対象セーブデータを指定するための構造体です。dataPathには保存対象セーブデータのデータパスを指定してください。PlayStation(R)VitaではdataPathに指定できるパスのディレクトリ階層は3階層まで可能で、".."を用いた相対参照はできません。bufには、保存するセーブデータの内容が格納されているバッファのアドレスを指定してください。合わせてbufSizeにはそのサイズを指定します。offsetには、bufで指定したセーブデータ内容を保存する際のファイル先頭からの書き込みオフセット位置を指定します。modeにkSaveDataSaveModeDirectoryを指定する場合には、bufにはNULLを、bufSize, offsetにはそれぞれ0を設定する必要があります。 
			 */
			typedef struct DataSaveItem{
				/*!
				 * @~English
				 * @brief Data path of the target save data to save 
				 * @~Japanese
				 * @brief 保存対象セーブデータのデータパス 
				 */
				const char *dataPath;
				/*!
				 * @~English
				 * @brief Buffer where the target save data content to save is stored when saving a file 
				 * @~Japanese
				 * @brief ファイル保存の場合、保存対象とするセーブデータ内容が格納されているバッファ 
				 */
				const void *buf;
				/*!
				 * @~English
				 * @brief Size of the target save data to save when saving a file 
				 * @~Japanese
				 * @brief ファイル保存の場合、保存対象とするセーブデータのサイズ 
				 */
				size_t bufSize;
				/*!
				 * @~English
				 * @brief Write offset position from the file start for the target save data to save when saving a file 
				 * @~Japanese
				 * @brief ファイル保存の場合、保存対象セーブデータに対するファイル先頭からの書き込みオフセット位置 
				 */
				uint32_t offset;
				/*!
				 * @~English
				 * @brief Specify save mode for the target save data to save 
				 * @~Japanese
				 * @brief 保存対象とするセーブデータの保存モード指定 
				 */
				SaveDataSaveMode mode;
			} DataSaveItem;

			/*!
			 * @~English
			 * 
			 * @brief Save data mount point structure 
			 * @details This structure receives the save data mount point name. 
			 * @~Japanese
			 * 
			 * @brief セーブデータのマウントポイント構造体 
			 * @details セーブデータのマウントポイント名を受け取るための構造体です。 
			 */
			typedef struct SaveDataMountPoint{
				/*!
				 * @~English
				 * @brief Mount point name (terminated with NULL, UTF-8) 
				 * @~Japanese
				 * @brief マウントポイント名（NULL終端, UTF-8） 
				 */
				SCE_SAMPLE_TCHAR data[SCE_SAMPLE_UTIL_SAVE_DATA_MOUNT_POINT_DATA_MAXSIZE];
			} SaveDataMountPoint;

			/*!
			 * @~English
			 * 
			 * @brief Structure for specifying target save data to remove 
			 * @details This structure specifies the target save data to remove. For dataPath, specify the data path of the target save data to remove. 
			 * @~Japanese
			 * 
			 * @brief 削除対象セーブデータ指定用構造体 
			 * @details 削除対象セーブデータを指定するための構造体です。dataPathには削除対象セーブデータのデータパスを指定します。 
			 */
			typedef struct DataRemoveItem{
				/*!
				 * @~English
				 * @brief Data path of the target save data to remove 
				 * @~Japanese
				 * @brief 削除対象セーブデータのデータパス 
				 */
				const char *dataPath;
			} DataRemoveItem;

			/*!
			 * @~English
			 * 
			 * @brief Save data 
			 * 
			 * @details This class provides the save data feature. On PlayStation(R)4, save data is created per user ID. 
			 * @~Japanese
			 * 
			 * @brief セーブデータ 
			 * 
			 * @details セーブデータ機能を提供するクラスです。PlayStation(R)4では、ユーザーIDごとにセーブデータが作成されます。 
			 */
			class SCE_SAMPLE_UTIL_API SaveData : public Resource
			{
			public:
				/*!
				 * @~English
				 * @brief Create save data slot 
				 * 
				 * @details This creates a save data slot. 
				 * @param userId User ID. This parameter is ignored on all platforms other than PlayStation(R)4.
				 * @param slotId Save data slot ID (0 to SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1)
				 * @param param Save data slot parameter structure
				 * @param sizeKiB Data size (KiB). Specify 10240 or more for PlayStation(R)4. Moreover, this value is aligned to a multiple of 32 on PlayStation(R)4.
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータスロットを作成する 
				 * 
				 * @details セーブデータスロットを作成します。 
				 * @param userId ユーザーID。PlayStation(R)4以外ではこのパラメータは無視されます。
				 * @param slotId セーブデータスロットID（0～SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1）
				 * @param param セーブデータスロットパラメータ構造体
				 * @param sizeKiB データサイズ（単位：KiB）。PlayStation(R)4では10240以上を指定してください。また、PlayStation(R)4では32の倍数にアラインされます。
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int slotCreate(const UserId userId, const SaveDataSlotId slotId, const SaveDataSlotParam *param, const size_t sizeKiB) = 0;

				/*!
				 * @~English
				 * @brief Delete save data slot 
				 * 
				 * @details This deletes a save data slot. 
				 * @param userId User ID. This parameter is ignored on all platforms other than PlayStation(R)4.
				 * @param slotId Save data slot ID (0 to SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1)
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータスロットを削除する 
				 * 
				 * @details セーブデータスロットを削除します。 
				 * @param userId ユーザーID。PlayStation(R)4以外ではこのパラメータは無視されます。
				 * @param slotId セーブデータスロットID（0～SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1）
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int slotDelete(const UserId userId, const SaveDataSlotId slotId) = 0;

				/*!
				 * @~English
				 * @brief Set save data slot parameters 
				 * 
				 * @details This sets save data slot parameters. With PlayStation(R)4, when an error returns, some of the parameters may be set. 
				 * @param userId User ID. This parameter is ignored on all platforms other than PlayStation(R)4.
				 * @param slotId Save data slot ID (0 to SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1)
				 * @param param Save data slot parameter structure
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータスロットのパラメータを設定する 
				 * 
				 * @details セーブデータスロットのパラメータを設定します。PlayStation(R)4ではエラーが返った際に、パラメータが一部設定される場合があります。 
				 * @param userId ユーザーID。PlayStation(R)4以外ではこのパラメータは無視されます。
				 * @param slotId セーブデータスロットID（0～SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1）
				 * @param param セーブデータスロットパラメータ構造体
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int slotSetParam(const UserId userId, const SaveDataSlotId slotId, const SaveDataSlotParam *param) = 0;

				/*!
				 * @~English
				 * @brief Get save data slot parameters 
				 * 
				 * @details This gets save data slot parameters. 
				 * @param userId User ID. This parameter is ignored on all platforms other than PlayStation(R)4.
				 * @param slotId Save data slot ID (0 to SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1)
				 * @param outParam Save data slot parameter structure
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータスロットのパラメータを取得する 
				 * 
				 * @details セーブデータスロットのパラメータを取得します。 
				 * @param userId ユーザーID。PlayStation(R)4以外ではこのパラメータは無視されます。
				 * @param slotId セーブデータスロットID（0～SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1）
				 * @param outParam セーブデータスロットパラメータ構造体
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int slotGetParam(const UserId userId, const SaveDataSlotId slotId, SaveDataSlotParam *outParam) = 0;

				/*!
				 * @~English
				 * @brief Search for save data slot 
				 * 
				 * @details This searches for a save data slot. For the search processing results, the number of matching save data slot hits will be stored in the hitNum member of outResult, and a save data slot ID array will be stored in the slotList member of the argument outResult. 
				 * @param userId User ID. This parameter is ignored on all platforms other than PlayStation(R)4.
				 * @param cond Save data slot search condition structure
				 * @param outResult Save data slot search result structure
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータスロットを検索する 
				 * 
				 * @details セーブデータスロットを検索します。検索処理の結果として合致したセーブデータスロット件数がoutResultのhitNumメンバに、セーブデータスロットID配列が引数outResultのslotListメンバに格納されます。 
				 * @param userId ユーザーID。PlayStation(R)4以外ではこのパラメータは無視されます。
				 * @param cond セーブデータスロット検索条件構造体
				 * @param outResult セーブデータスロット検索結果構造体
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int slotSearch(const UserId userId, const SaveDataSearchCond *cond, SaveDataSearchResult *outResult) = 0;

				/*!
				 * @~English
				 * @brief Save save data 
				 * 
				 * @details This function saves save data specified in data in file or directory units. With PlayStation(R)Vita, 0 is stored in outRequiredSizeKiB and outRequiredSizeKiB returns for normal termination of the function. If an SCE_APPUTIL_ERROR_SAVEDATA_NO_SPACE_QUOTA error or SCE_APPUTIL_ERROR_SAVEDATA_NO_SPACE_FS error occurs, the size (KiB) insufficient for saving the save data specified for data and dataNum will be stored in outRequiredSizeKiB and outRequiredSizeKiB will return. With PlayStation(R)4, the value will not be stored in outRequiredSizeKiB. 
				 * @param userId User ID. This parameter is ignored on all platforms other than PlayStation(R)4.
				 * @param slotId Save data slot ID (0 to SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1)
				 * @param data DataSaveItem array pointer. Specify target data to save in file or directory units. The upper limit for total data file size on PlayStation(R)Vita is 1 MiB.
				 * @param dataNum Number of target data to save. For PlayStation(R)Vita, 1 to SCE_APPUTIL_SAVEDATA_DATA_MAXNUM can be specified.
				 * @param outRequiredSizeKiB Size lacking for saving the save data specified in data (units in KiB) (when an SCE_APPUTIL_ERROR_SAVEDATA_NO_SPACE_QUOTA error or SCE_APPUTIL_ERROR_SAVEDATA_NO_SPACE_FS error occurs on PlayStation(R)Vita)
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータを保存する 
				 * 
				 * @details data 指定したセーブデータをファイルまたはディレクトリ単位で保存します。PlayStation(R)VitaではoutRequiredSizeKiBに、関数が正常終了した場合に0が格納されて返り、SCE_APPUTIL_ERROR_SAVEDATA_NO_SPACE_QUOTAエラーまたはSCE_APPUTIL_ERROR_SAVEDATA_NO_SPACE_FSエラーが発生した場合にdata, dataNumに指定したセーブデータを保存するのに不足しているサイズ（単位：KiB）が格納されて返ります。PlayStation(R)4ではoutRequiredSizeKiBに、値は格納されません。 
				 * @param userId ユーザーID。PlayStation(R)4以外ではこのパラメータは無視されます。
				 * @param slotId セーブデータスロットID（0～SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1）
				 * @param data DataSaveItemの配列のポインタ。保存対象のデータをファイルまたはディレクトリ単位で指定。PlayStation(R)Vitaでは、データの合計ファイルサイズの上限は1MiB。
				 * @param dataNum 保存対象のデータの数。PlayStation(R)Vitaでは、1～SCE_APPUTIL_SAVEDATA_DATA_MAXNUMが指定可能です。
				 * @param outRequiredSizeKiB dataに指定したセーブデータを保存するにあたり不足しているサイズ（単位：KiB）（PlayStation(R)VitaにてSCE_APPUTIL_ERROR_SAVEDATA_NO_SPACE_QUOTAエラーまたはSCE_APPUTIL_ERROR_SAVEDATA_NO_SPACE_FSエラー発生時）
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int dataSave(const UserId userId, const SaveDataSlotId slotId, const DataSaveItem *data, const uint32_t dataNum, size_t *outRequiredSizeKiB=NULL) = 0;

#if defined(_SCE_TARGET_OS_PSP2) && _SCE_TARGET_OS_PSP2
				/*!
				 * @~English
				 * @brief Create save data slot and save save data atomically 
				 * 
				 * @details This function creates a save data slot based on the settings specified by slotParam and atomically saves save data specified in data in file or directory units. An error will return if the save data slot which has the ID specified for slotId already exists. 
				 * @param slotId Save data slot ID (0 to SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1)
				 * @param data DataSaveItem array pointer. Specify target data to save in file or directory units. The upper limit is 1 MiB for the data's total file size.
				 * @param dataNum Number of target data to save. 1 to SCE_APPUTIL_SAVEDATA_DATA_MAXNUM can be specified.
				 * @param slotParam Save data slot parameter structure. An error will return if NULL is specified.
				 * @param slotSizeKiB Data size of the save data slot (KiB).
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータスロットの作成とセーブデータの保存を不可分に行う 
				 * 
				 * @details slotParamで指定された設定でのセーブデータスロット作成と、 dataに指定したセーブデータのファイルまたはディレクトリ単位での保存を不可分に行います。slotIdで指定されたIDを持つセーブデータスロットがすでに存在する場合にはエラーが返ります。 
				 * @param slotId セーブデータスロットID（0～SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1）
				 * @param data DataSaveItemの配列のポインタ。保存対象のデータをファイルまたはディレクトリ単位で指定。データの合計ファイルサイズの上限は1MiB。
				 * @param dataNum 保存対象のデータの数。1～SCE_APPUTIL_SAVEDATA_DATA_MAXNUMが指定可能。
				 * @param slotParam セーブデータスロットパラメータ構造体。NULLの場合はエラーが返る。
				 * @param slotSizeKiB セーブデータスロットデータサイズ（単位：KiB）
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int atomicSlotCreateAndDataSave(const SaveDataSlotId slotId, const DataSaveItem *data, const uint32_t dataNum, const SaveDataSlotParam *slotParam, const size_t slotSizeKiB) = 0;

				/*!
				 * @~English
				 * @brief Save save data and set save data slot parameters atomically 
				 * 
				 * @details This saves the save data specified for data at the file or directory level and sets save data slot parameters specified for slotParam atomically. An error will return if the save data slot which has the ID specified for slotId does not exist. 
				 * @param slotId Save data slot ID (0 to SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1)
				 * @param data DataSaveItem array pointer. Specify target data to save in file or directory units. The upper limit is 1 MiB for the data's total file size.
				 * @param dataNum Number of target data to save. 1 to SCE_APPUTIL_SAVEDATA_DATA_MAXNUM can be specified.
				 * @param slotParam Save data slot parameter structure. An error will return if NULL is specified.
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータの保存とセーブデータスロットのパラメータ設定を不可分に行う 
				 * 
				 * @details dataに指定したセーブデータのファイルまたはディレクトリ単位での保存と、 slotParamに指定したセーブデータスロットのパラメータ設定を不可分に行います。slotIdで指定されたIDを持つセーブデータスロットが存在しない場合にはエラーが返ります。 
				 * @param slotId セーブデータスロットID（0～SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1）
				 * @param data DataSaveItemの配列のポインタ。保存対象のデータをファイルまたはディレクトリ単位で指定。データの合計ファイルサイズの上限は1MiB。
				 * @param dataNum 保存対象のデータの数。1～SCE_APPUTIL_SAVEDATA_DATA_MAXNUMが指定可能。
				 * @param slotParam セーブデータスロットパラメータ構造体。NULLの場合はエラーが返る。
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int atomicDataSaveAndSlotSetParam(const SaveDataSlotId slotId, const DataSaveItem *data, const uint32_t dataNum, const SaveDataSlotParam *slotParam) = 0;
#endif

				/*!
				 * @~English
				 * @brief Mount save data directory for loading 
				 * 
				 * @details This mounts a save data slot directory for loading. Use an obtained mount point name to load the save data. With PlayStation(R)Vita, only the mount point name for loading will be returned; actual mount processing will not be performed in this function. 
				 * @param userId User ID. This parameter is ignored on all platforms other than PlayStation(R)4.
				 * @param slotId Save data slot ID (0 to SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1)
				 * @param outMountPoint Storage destination for the allocated mount point name
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータディレクトリを読み込み用にマウントする 
				 * 
				 * @details セーブデータスロットのディレクトリを読み込み用にマウントします。取得したマウントポイント名を使用してセーブデータをロードしてください。PlayStation(R)Vitaでは、読み込み用のマウントポイント名を返すのみで、本関数内で実際にマウント処理は行われません。 
				 * @param userId ユーザーID。PlayStation(R)4以外ではこのパラメータは無視されます。
				 * @param slotId セーブデータスロットID（0～SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1）
				 * @param outMountPoint 割り当てられたマウントポイント名の格納先
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int dataLoadMount(const UserId userId, const SaveDataSlotId slotId, SaveDataMountPoint *outMountPoint) = 0;

				/*!
				 * @~English
				 * @brief Unmount save data directory 
				 * 
				 * @details This unmounts a save data slot directory mounted for loading. With PlayStation(R)Vita, actual unmount processing is not performed in this function. 
				 * @param mountPoint Mount point name
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータディレクトリをアンマウントする 
				 * 
				 * @details 読み込み用にマウントしたセーブデータスロットのディレクトリをアンマウントします。PlayStation(R)Vitaでは、本関数内で実際にアンマウント処理は行われません。 
				 * @param mountPoint マウントポイント名
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int dataLoadUmount(const SaveDataMountPoint *mountPoint) = 0;

				/*!
				 * @~English
				 * @brief Remove save data 
				 * 
				 * @details This removes the save data specified for data and dataNum at the file or directory level. This removes the file/directory only without deleting the save data slot. 
				 * @param userId User ID. This parameter is ignored on all platforms other than PlayStation(R)4.
				 * @param slotId Save data slot ID (0 to SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1)
				 * @param data DataRemoveItem array pointer (specify save data (file or directory level) to remove)
				 * @param dataNum Number of save data to remove. For PlayStation(R)Vita, 1 to SCE_APPUTIL_SAVEDATA_DATA_MAXNUM can be specified.
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータを削除する 
				 * 
				 * @details data, dataNumに指定したセーブデータをファイルまたはディレクトリ単位で削除します。セーブデータスロットは削除せずに、ファイル／ディレクトリのみを削除します。 
				 * @param userId ユーザーID。PlayStation(R)4以外ではこのパラメータは無視されます。
				 * @param slotId セーブデータスロットID（0～SCE_SAMPLE_UTIL_SAVE_DATA_SLOT_MAX-1）
				 * @param data DataRemoveItemの配列のポインタ（削除対象のセーブデータのデータ（ファイルまたはディレクトリ単位）の指定）
				 * @param dataNum 削除対象のセーブデータのデータ数。PlayStation(R)Vitaでは、1～SCE_APPUTIL_SAVEDATA_DATA_MAXNUMが指定可能です。
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int dataRemove(const UserId userId, const SaveDataSlotId slotId, const DataRemoveItem *data, const uint32_t dataNum) = 0;

#if defined(_SCE_TARGET_OS_ORBIS) && _SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief Gets number of blocks required for a save data mount 
				 * 
				 * @details This function obtains the number of blocks required for a save data mount. Call this function when SCE_SAVE_DATA_ERROR_NO_SPACE_FS returns from slotCreate(), slotSetParam(). dataSave() or dataRemove(). 
				 * @param outRequiredBlocks Pointer for storing required number of blocks
				 * @retval SCE_OK Success
				 * @retval (<0) Error code
				 * @~Japanese
				 * @brief セーブデータマウントのために必要なブロック数の取得 
				 * 
				 * @details セーブデータマウントのために必要なブロック数を取得します。slotCreate()、slotSetParam()、dataSave()、dataRemove()からSCE_SAVE_DATA_ERROR_NO_SPACE_FS が返った際に呼び出してください。 
				 * @param outRequiredBlocks 必要なブロック数を格納するポインタ
				 * @retval SCE_OK 成功
				 * @retval (<0) エラーコード
				 */
				virtual int getRequiredBlocksForSaveDataMount(SceSaveDataBlocks *outRequiredBlocks) = 0;
#endif
			};
			
 			/*!
 			 * @~English
 			 * 
 			 * @brief Structure for initializing SaveData 
 			 * @details This is the structure for initializing SaveData. This is used by specifying it to the option argument of createSaveData(). 
 			 * @~Japanese
 			 * 
 			 * @brief SaveDataの初期化用構造体 
 			 * @details SaveDataの初期化用構造体です。createSaveData()の引数optionに指定することで利用します。 
 			 */
			typedef struct SaveDataOption
			{
#if !_SCE_TARGET_OS_ORBIS
				/*!
				 * @~English
				 * @brief Title ID 
				 * @~Japanese
				 * @brief タイトルID 
				 */
				char titleId[SCE_SAMPLE_UTIL_SAVE_DATA_TITLE_ID_DATA_SIZE];
				/*!
				 * @~English
				 * @brief Passcode 
				 * @~Japanese
				 * @brief パスコード 
				 */
				char passCode[SCE_SAMPLE_UTIL_SAVE_DATA_PASSCODE_DATA_SIZE];
#endif
			} SaveDataOption;

			/*!
			 * @~English
			 * @brief Create save data class instance 
			 * 
			 * @details This creates a save data class instance. To delete the generated instance, use sce::SampleUtil::destroy(). When NULL is specified for option, the save data directory that is automatically mounted when the application starts will be accessed. When a value other than NULL is specified for option, the titleId save data directory specified for the SaveDataOption structure will be mounted. However, with PlayStation(R)Vita it will be mounted with read access. 
			 * @param outSaveData Pointer to which the generated save data class instance returns
			 * @param option SaveDataOption structure.
			 * @retval SCE_OK Success
			 * @retval (<0) Error code
			 * @~Japanese
			 * @brief セーブデータクラスのインスタンスを生成 
			 * 
			 * @details セーブデータクラスのインスタンスを生成します。生成されたインスタンスを削除する場合はsce::SampleUtil::destroy()を使用してください。optionにNULLを指定するとアプリケーション起動時に自動でマウントされるセーブデータディレクトリにアクセスします。optionにNULL以外を指定した場合、SaveDataOption構造体に指定されたtitleIdのセーブデータディレクトリをマウントします、ただしPlayStation(R)Vitaではリードアクセスでマウントします。 
			 * @param outSaveData 生成されるセーブデータクラスのインスタンスが返るポインタ
			 * @param option SaveDataOption構造体。
			 * @retval SCE_OK 成功
			 * @retval (<0) エラーコード
			 */
			SCE_SAMPLE_UTIL_API int createSaveData(SaveData **outSaveData, const SaveDataOption *option);
		}
	}
}

#endif /* _SCE_SAMPLE_UTIL_SYSTEM_H */
