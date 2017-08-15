/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#include <stdio.h>
#include <vectormath.h>
#include <vector>
#include <string>
#include <sampleutil.h>
#include <user_service.h>
#include <system_service.h>
#include <libsysmodule.h>
#include "common.h"
#include "title_scene.h"
#include "select_players_scene.h"
#include "game_main_scene.h"

#include "AudioManager.h"

size_t sceLibcHeapSize = 256 * 1024 * 1024;

AudioManager am;

class Application : public sce::SampleUtil::SampleSkeleton
{
private:
	ResourceManager m_resourceManager;
	SceneBase *m_scene;
	UserInfoCollection m_usersInfo;

public:
	Application() 
		: sce::SampleUtil::SampleSkeleton()
		, m_resourceManager()
		, m_scene(NULL)
		, m_usersInfo()
	{}

	~Application() 
	{}

	virtual int initialize(void)
	{
		int ret = 0;
		(void)ret;

		//E Initialize SampleUtil. Graphics feature and SpriteRenderer feature of SampleUtil are enabled.
		//J SampleUtilの初期化。SampleUtilのGraphics機能とSpriteRenderer機能を有効化。
		ret = initializeUtil(kFunctionFlagGraphics | kFunctionFlagSpriteRenderer);
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
		if (ret < SCE_OK) { return ret; } 

		//E Initialize libraries.
		//J ライブラリの初期化
		ret = initializeLibrary();
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
		if (ret < SCE_OK) { return ret; } 

		//E Load texture data.
		//J テクスチャデータの読み込み
		ret = m_resourceManager.initialize(getGraphicsContext());
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
		if (ret < SCE_OK) { return ret; } 

		m_usersInfo.clear();

		ret = loadScene(SCENE_ID_TITLE, NULL);
		if (ret < SCE_OK) { return ret; } 

		return ret;
	}

	virtual int finalize(void)
	{
		int ret = 0;
		(void)ret;

		unloadScene();

		std::vector<UserInfo*>::iterator it = m_usersInfo.begin();
		while (it != m_usersInfo.end()) {
			ret = (*it)->finalize();
			SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
			delete (*it);
			it = m_usersInfo.erase(it);
		}

		m_resourceManager.terminate();

		terminateLibrary();

		ret = finalizeUtil();
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);

		return ret;
	}
	
	//E Update the application logic. Call updateUtil() at the beginning.
	//J アプリケーションのロジックの更新を行います。updateUtil()は冒頭で必ず呼び出します。
	virtual int update(void)
	{
		int ret = 0;
		(void)ret;

		//E Update SampleUtil
		//J SampleUtilの更新
		ret = updateUtil();
		if (ret != SCE_OK){
			return ret;
		}

		//E Get UserService events while it returns SCE_OK.
		//J SCE_OK が返ってくる間 UserService のイベントを取得する。
		SceUserServiceEvent event;
		for (;;) {
			ret = sceUserServiceGetEvent(&event);
			if (ret == SCE_OK) {
				//E Call event handler when UserService events can get.
				//J UserService のイベントが取得できたらイベントハンドラを呼び出す。
				userEventHandler(&event);
			} else {
				SCE_SAMPLE_UTIL_ASSERT(ret == SCE_USER_SERVICE_ERROR_NO_EVENT);
				break;
			}
		}

		//E Refresh User color on SystemService resume event.
		//J SystemService の resume event が来たらユーザーカラーを再取得する
		SceSystemServiceStatus systemStatus;
		SceSystemServiceEvent systemEvent;

		ret = sceSystemServiceGetStatus(&systemStatus);
		if ((ret == SCE_OK) && (systemStatus.eventNum > 0))	{
			for (int i = 0; i < systemStatus.eventNum; i++) {
				ret = sceSystemServiceReceiveEvent(&systemEvent);
				if (ret == SCE_OK) {
					if (systemEvent.eventType == SCE_SYSTEM_SERVICE_EVENT_ON_RESUME) {
						// This process was resumed.
						for (std::vector<UserInfo*>::iterator it = m_usersInfo.begin(); it != m_usersInfo.end(); it++) {
							SceUserServiceUserColor userColor;
							ret = sceUserServiceGetUserColor((*it)->userId, &userColor);
							if (ret == SCE_OK) {
								(*it)->setColor(userColor);
							}
						}
					}
				}
			}
		}

		for (std::vector<UserInfo*>::iterator it = m_usersInfo.begin(); it != m_usersInfo.end(); it++) {
			ret = (*it)->update();
			if (ret < SCE_OK){
				return ret;
			}
		}

		if (m_scene != NULL) {
			m_scene->update(&m_usersInfo);

			EventId eventId = m_scene->getEvent();
			if (eventId != EVENT_ID_NONE) {
				switch (m_scene->getSceneId())
				{
				case SCENE_ID_TITLE:
					if(eventId == EVENT_ID_GO_FORWARD){
						am.PlaySound("bugs_big_one");
						ret = loadScene(SCENE_ID_SELECT_PLAYERS, NULL);
					}
					break;
				case SCENE_ID_SELECT_PLAYERS:
					if(eventId == EVENT_ID_GO_FORWARD){
						PlayersIds ids = ((SelectPlayersScene*)m_scene)->getSelectedPlayers();
						ret = loadScene(SCENE_ID_GAME_MAIN, &ids);
					}else if(eventId == EVENT_ID_GO_BACKWARD){
						ret = loadScene(SCENE_ID_TITLE, NULL);
					}
					break;
				case SCENE_ID_GAME_MAIN:
					if(eventId == EVENT_ID_GO_FORWARD){
						ret = loadScene(SCENE_ID_TITLE, NULL);
					}
					break;
				default:
					break;
				}
				if (ret != SCE_OK){
					return ret;
				}
			}
		}

		return SCE_OK;
	}	

	virtual void render(void)
	{
		getGraphicsContext()->beginScene(getGraphicsContext()->getNextRenderTarget(),
			                             getGraphicsContext()->getDepthStencilSurface());
		getGraphicsContext()->clearRenderTarget(0x00000000);
		getGraphicsContext()->setDepthFunc(sce::SampleUtil::Graphics::kDepthFuncAlways);	// for drawDebugStringf

		if (m_scene != NULL) {
			m_scene->render(getGraphicsContext(), getSpriteRenderer(), &m_resourceManager);
		}

		getGraphicsContext()->endScene();
		getGraphicsContext()->flip(1);
	}

private:
	//E Handle UserService events.
	//J UserService のイベントをハンドルします。
	int userEventHandler(SceUserServiceEvent *event)
	{
		if (event == NULL) {
			return 0;
		}
		if (event->eventType == SCE_USER_SERVICE_EVENT_TYPE_LOGIN) {
			onLogin(event->userId);
		} else if (event->eventType == SCE_USER_SERVICE_EVENT_TYPE_LOGOUT) {
			onLogout(event->userId);
		}

		return 0;
	}

	//E Procedures on user login.
	//J ユーザーがログインした時の処理。
	int onLogin(SceUserServiceUserId userId)
	{
		int ret;
		(void)ret;
		//E Get local user name of the user.
		//E After user have linked to Sony Entertainment Network account, it is the same as Online ID .
		//J ユーザーのローカルユーザー名を取得する。
		//J Sony Entertainment Networkアカウントを登録すると、オンラインIDと同じものが設定されている。
		char userName[SCE_USER_SERVICE_MAX_USER_NAME_LENGTH + 1];
		memset(userName, 0, sizeof(userName));
		ret = sceUserServiceGetUserName(userId, userName, sizeof(userName));
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);

		SceUserServiceUserColor userColor;
		ret = sceUserServiceGetUserColor(userId, &userColor);
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);

		UserInfo *userInfo = new UserInfo(userId, userName, userColor);

		ret = userInfo->initialize();
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);

		m_usersInfo.push_back(userInfo);

		printf("user \"%s\" (0x%x) is logged in\n", userName, userId);

		return SCE_OK;
	}

	//E Procedures on user logout.
	//J ユーザーがログアウトした時の処理。
	int onLogout(SceUserServiceUserId userId)
	{
		int ret = 0;
		(void)ret;

		UserInfo *userInfo = NULL;
		for (std::vector<UserInfo*>::iterator it = m_usersInfo.begin(); it != m_usersInfo.end(); it++) {
			if ((*it)->userId == userId) {
				userInfo = *it;
				m_usersInfo.erase(it);
				break;
			}
		}

		printf("user \"%s\" (0x%x) is logged out\n", userInfo->userName.c_str(), userInfo->userId);

		ret = userInfo->finalize();
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);

		delete userInfo;

		return SCE_OK;
	}

	int initializeLibrary(void)
	{
		int ret = 0;

		ret = sceUserServiceInitialize(NULL);
		SCE_SAMPLE_UTIL_ASSERT(ret < SCE_OK);
		if (ret < SCE_OK) { return ret; }

		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_LOGIN_DIALOG);
		SCE_SAMPLE_UTIL_ASSERT(ret < SCE_OK);
		if (ret < SCE_OK) { return ret; }

		ret = sceSysmoduleLoadModule(SCE_SYSMODULE_LOGIN_SERVICE);
		SCE_SAMPLE_UTIL_ASSERT(ret < SCE_OK);
		if (ret < SCE_OK) { return ret; }


		return SCE_OK;
	}

	void terminateLibrary(void)
	{
		sceSysmoduleUnloadModule(SCE_SYSMODULE_LOGIN_SERVICE);
		sceSysmoduleUnloadModule(SCE_SYSMODULE_LOGIN_DIALOG);

		sceUserServiceTerminate();
	}

	void unloadScene()
	{
		if (m_scene != NULL){
			m_scene->terminate();
		}
		m_scene = NULL;
	}

	int loadScene(SceneId id, void *param)
	{
		unloadScene();

		int ret;
		switch (id)
		{
		case SCENE_ID_TITLE:
			m_scene = new TitleScene();
			break;
		case SCENE_ID_SELECT_PLAYERS:
			m_scene = new SelectPlayersScene();
			break;
		case SCENE_ID_GAME_MAIN:
			m_scene = new GameMainScene();
			ret = ((GameMainScene*)m_scene)->setPlayers(*((PlayersIds*)param));
			if (ret < SCE_OK) { return ret; } 
			break;
		default:
			break;
		}

		if (m_scene != NULL) {
			ret = m_scene->initialize();
			if (ret < SCE_OK) { return ret; } 
		}

		return SCE_OK;
	}
};

//E Instance definition of the application class
//J アプリケーションクラスのインスタンス定義
Application g_application;

int main(void)
{
	int ret = 0;
	(void)ret;

	ret = g_application.initialize();
	SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
	if (ret < SCE_OK) { return 0; }

	while (1) {
		ret = g_application.update();
		if (ret != SCE_OK){
			break;
		}
		g_application.render();
	}

	ret = g_application.finalize();
	SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);

	return 0;
}