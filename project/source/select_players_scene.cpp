/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#include <libsysmodule.h>
#include <login_dialog.h>
#include "select_players_scene.h"

#define NO_PLAYER_SELECTED_TEXT "press (x) to select"

SelectPlayersScene::SelectPlayersScene()
	: m_event(EVENT_ID_NONE)
	, m_cursorPosition(0)
	, m_isLoginDialog(false)
{
	m_playersIds.clear();
}

SelectPlayersScene::~SelectPlayersScene()
{}

int SelectPlayersScene::initialize()
{
	int ret = SceneBase::initialize();
	SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
	if (ret < SCE_OK) { return ret; } 

	//E Initialize LoginDialog library.
	//J LoginDialogライブラリを初期化
	ret = sceLoginDialogInitialize();
	printf("sceLoginDialogInitialize() ret=0x%x\n", ret);
	if (ret < SCE_OK) { return ret; }

	for (int i = 0; i < PLAYER_INDEX_MAX; i++) {
		m_playersName[i] = NO_PLAYER_SELECTED_TEXT;
	}
	m_playersIds.clear();
	m_event = EVENT_ID_NONE;
	m_cursorPosition = 0;
	m_isLoginDialog = false;

	return SCE_OK;
}

void SelectPlayersScene::terminate()
{
	int ret = sceSysmoduleIsLoaded(SCE_SYSMODULE_LOGIN_DIALOG);
	if (ret == SCE_SYSMODULE_LOADED) {
		ret = sceLoginDialogTerminate();
		printf("sceLoginDialogTerminate() ret=0x%x\n", ret);
	}
	SceneBase::terminate();
}

EventId SelectPlayersScene::getEvent()
{
	return m_event;
}

int SelectPlayersScene::update(UserInfoCollection *usersInfo)
{
	//E If a user has been logged out, then the user become unselected state.
	//J もしユーザーがログアウトしていたら選択を解除
	for (int i = 0; i < PLAYER_INDEX_MAX; i++) {
		if ((m_playersIds.player[i] != SCE_USER_SERVICE_USER_ID_INVALID)
			&& (usersInfo->get(m_playersIds.player[i]) == NULL)) {

			m_playersName[i] = NO_PLAYER_SELECTED_TEXT;
			m_playersIds.player[i] = SCE_USER_SERVICE_USER_ID_INVALID;
		}
	}
	if ((canStartGame() == false) && (m_cursorPosition == 2)) {
		m_cursorPosition = 0;
	}

	if (m_isLoginDialog == false)  {
		bool crossPressed = false;
		bool circlePressed = false;
		bool upPressed = false;
		bool downPressed = false;

		//E Read button state.
		//J ボタン押下を検出
		for (std::vector<UserInfo*>::iterator it = usersInfo->begin(); it != usersInfo->end(); it++) {
			crossPressed |= (*it)->padContext->isButtonPressed(ssi::kButtonCross, ssi::kButtonEventPatternAll);
			circlePressed |= (*it)->padContext->isButtonPressed(ssi::kButtonCircle, ssi::kButtonEventPatternAll);
			upPressed |= (*it)->padContext->isButtonPressed(ssi::kButtonUp, ssi::kButtonEventPatternAll);
			downPressed |= (*it)->padContext->isButtonPressed(ssi::kButtonDown, ssi::kButtonEventPatternAll);
		}

		if (upPressed && downPressed) {
			upPressed = false;
			downPressed = false;
		}
		if (upPressed && (m_cursorPosition > 0)) {
			m_cursorPosition--;
		}
		int maxLine = canStartGame() ? 2 : 1;
		if (downPressed && (m_cursorPosition < maxLine)) {
			m_cursorPosition++;
		}

		if (circlePressed) {
			m_event = EVENT_ID_GO_BACKWARD;
		} else if (crossPressed) {
			if ((m_cursorPosition == 0) || (m_cursorPosition == 1)) {
				SceUserServiceUserId myself = m_playersIds.player[(m_cursorPosition == 0) ? PLAYER_INDEX_1 : PLAYER_INDEX_2];
				SceUserServiceUserId opponent = m_playersIds.player[(m_cursorPosition == 0) ? PLAYER_INDEX_2 : PLAYER_INDEX_1];

				//E Open LoginDialog.
				//J LoginDialogを開く
				SceLoginDialogParam param;
				sceLoginDialogParamInitialize(&param);
				param.mode = SCE_LOGIN_DIALOG_MODE_ALL_USERS;
				param.excludeUsersFromLoginList[0] = opponent;
				param.excludeUsersFromLogoutList[0] = opponent;
				param.initialFocus = myself;

				int ret = sceLoginDialogOpen(&param);
				printf("sceLoginDialogOpen() ret=0x%x\n", ret);
				if (ret >= SCE_OK) {
					m_isLoginDialog = true;
				}
			} else if (m_cursorPosition == 2) {
				m_event = EVENT_ID_GO_FORWARD;
			}
		}
	}

	if (m_isLoginDialog == true) {
		SceLoginDialogStatus status = sceLoginDialogUpdateStatus();
		m_isLoginDialog = (status == SCE_LOGIN_DIALOG_STATUS_RUNNING);
		if (status == SCE_LOGIN_DIALOG_STATUS_FINISHED) {
			SceLoginDialogResult result;
			memset(&result, 0x00, sizeof(SceLoginDialogResult));
			int ret = sceLoginDialogGetResult(&result);
			if (ret == SCE_OK) {
				if (result.result == SCE_LOGIN_DIALOG_RESULT_OK) {
					int idx = (m_cursorPosition == 0) ? PLAYER_INDEX_1 : PLAYER_INDEX_2;
					m_playersIds.player[idx] = result.selectedUser;

					//E Get local user name of the user.
					//J ユーザーのローカルユーザー名を取得する。
					char userName[SCE_USER_SERVICE_MAX_USER_NAME_LENGTH + 1];
					memset(userName, 0, sizeof(userName));
					ret = sceUserServiceGetUserName(m_playersIds.player[idx], userName, sizeof(userName));
					if (ret != SCE_OK) { userName[0] = '\0'; }
					m_playersName[idx] = userName;

					printf("sceLoginDialogGetResult() result=%s, selectedUser=[%s]\n"
						, "SCE_LOGIN_DIALOG_RESULT_OK", m_playersName[idx].c_str());
				} else if (result.result == SCE_LOGIN_DIALOG_RESULT_USER_CANCELED) {
					printf("sceLoginDialogGetResult() result=%s\n" 
						, "SCE_LOGIN_DIALOG_RESULT_USER_CANCELED");
				}
			} else {
				printf("sceLoginDialogGetResult() ret=0x%x\n", ret);
			}
		}
	}

	return SCE_OK;
}

void SelectPlayersScene::render(ssg::GraphicsContext *graphicsContext, ssg::SpriteRenderer *spriteRenderer, ResourceManager *resourceManager)
{
	spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.050), 0.08, g_white, "Select Players");

	int drawingLine = 0;
	vecmath::Vector4 color = (drawingLine == m_cursorPosition) ? g_red : g_white;
	const char *cursor = (drawingLine == m_cursorPosition) ? ">" : " ";
	spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.210), 0.03, color, 
		"%s Player1: [%s]", cursor, m_playersName[0].c_str());

	drawingLine++;
	color = (drawingLine == m_cursorPosition) ? g_red : g_white;
	cursor = (drawingLine == m_cursorPosition) ? ">" : " ";
	spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.250), 0.03, color,
		"%s Player2: [%s]", cursor, m_playersName[1].c_str());

	drawingLine++;
	color = (drawingLine == m_cursorPosition) ? g_red : (canStartGame() ? g_white : g_gray);
	cursor = (drawingLine == m_cursorPosition) ? ">" : " ";
	spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.290), 0.03, color, 
		"%s Start Game", cursor);

	spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.920), 0.03, g_white_gray, 
		"o:back, x:enter, up and down:move focus");
}

PlayersIds SelectPlayersScene::getSelectedPlayers()
{
	return m_playersIds;
}

bool SelectPlayersScene::canStartGame()
{
	return ((m_playersIds.player[PLAYER_INDEX_1] != SCE_USER_SERVICE_USER_ID_INVALID)
			&& (m_playersIds.player[PLAYER_INDEX_2] != SCE_USER_SERVICE_USER_ID_INVALID));
}
