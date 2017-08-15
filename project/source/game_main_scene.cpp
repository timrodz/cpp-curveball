/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#include <pad.h>
#include <libsysmodule.h>
#include <login_service.h>
#include "game_main_scene.h"

#define CURSOR_DEFAULT_X_POSITION	1
#define CURSOR_DEFAULT_Y_POSITION	1
#define MAX_TURNS_COUNT				9

#define ABS(x)						(((x) >= 0) ? (x) : (-(x)))

GameMainScene::GameMainScene()
	: m_mode(MODE_START)
	, m_shouldQuit(false)
	, m_currentPlayer(PLAYER_INDEX_1)
	, m_currentPlayerHasController(false)
	, m_cursorX(0)
	, m_cursorY(0)
	, m_turnsCount(0)
	, m_result(GAME_RESULT_IN_PLAY)
	, m_frameCount(0)
{
	memset(m_gameBoard, 0x00, sizeof(m_gameBoard));
	memset(m_resultBoard, 0x00, sizeof(m_resultBoard));
}

GameMainScene::~GameMainScene()
{}

int GameMainScene::setPlayers(PlayersIds ids)
{
	if (isInitialized()) { return -1; }
	if (!isPlayersIdValid(ids)) { return -1; }
	m_playersIds = ids;
	return SCE_OK;
}

int GameMainScene::initialize()
{
	int ret = SceneBase::initialize();
	SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
	if (ret < SCE_OK) { return ret; } 

	if(!isPlayersIdValid(m_playersIds)) { return -1; }

	m_mode = MODE_START;
	m_shouldQuit = false;
	m_currentPlayer = PLAYER_INDEX_1;
	m_currentPlayerHasController = false;

	for (int i = 0; i < PLAYER_INDEX_MAX; i++) {
		m_playersName[i] = "";
		m_playersColor[i] = g_black;
	}

	for (int y = 0; y < GAME_BOARD_HEIGHT; y++) {
		for (int x = 0; x < GAME_BOARD_WIDTH; x++) {
			m_gameBoard[x][y] = PIECE_TYPE_EMPTY;
		}
	}

	m_cursorX = CURSOR_DEFAULT_X_POSITION;
	m_cursorY = CURSOR_DEFAULT_Y_POSITION;
	m_turnsCount = 0;

	m_result = GAME_RESULT_IN_PLAY;
	clearResultBoard();

	m_frameCount = 0;
	m_timer.set(GameMainScene::_timerCallback, this, 0, 120);

	ret = scePadInit();
	if (ret < SCE_OK) { return ret; }

	ret = sceLoginServiceInitialize();
	printf("sceLoginServiceInitialize() ret=0x%x\n", ret);
	if (ret < SCE_OK) { return ret; }

	return SCE_OK;
}

void GameMainScene::terminate()
{
	int ret = sceSysmoduleIsLoaded(SCE_SYSMODULE_LOGIN_SERVICE);
	if (ret == SCE_SYSMODULE_LOADED) {
		ret = sceLoginServiceTerminate();
		printf("sceLoginServiceTerminate() ret=0x%x\n", ret);
	}
	SceneBase::terminate();
}

EventId GameMainScene::getEvent()
{
	return m_shouldQuit ? EVENT_ID_GO_FORWARD : EVENT_ID_NONE;
}

int GameMainScene::update(UserInfoCollection *usersInfo)
{
	m_frameCount++;
	m_timer.update();

	if (m_shouldQuit) { return SCE_OK; }

	int ret;
	bool allPlayersAreLoggedIn = true;
	for (int i = 0; i < PLAYER_INDEX_MAX; i++) {
		UserInfo *info = usersInfo->get(m_playersIds.player[i]);
		if (info != NULL) {
			//E make a copy of name and color for render().
			//J render()での処理のため、名前と色をコピーしておく
			if (m_playersName[i].empty()) {
				m_playersName[i] = info->userName;
				m_playersColor[i] = info->userColor;
			}
		} else {
			allPlayersAreLoggedIn = false;
		}
	}

	if ((m_mode == MODE_START) || (m_mode == MODE_PLAYING)) {
		if (!allPlayersAreLoggedIn) {
			m_mode = MODE_PLAYER_IS_LOGGED_OUT;
			m_timer.set(GameMainScene::_timerCallback, this, 0, 240);
		}
	}

	if (m_mode == MODE_PLAYING) {
		bool isAnyonePressAnyButtons = false;
		for (std::vector<UserInfo*>::iterator it = usersInfo->begin(); it != usersInfo->end(); it++) {
			isAnyonePressAnyButtons |= (*it)->padContext->isButtonPressed(g_allButtons, ssi::kButtonEventPatternAny);
		}

		if (m_result == GAME_RESULT_IN_PLAY) {
			UserInfo *info = usersInfo->get(m_playersIds.player[m_currentPlayer]);
			m_currentPlayerHasController = false;
			if (info != NULL) {
				//E Check the connection status of a controller.
				//J コントローラーの接続状態を調べる
				ScePadControllerInformation padInfo;
				memset(&padInfo, 0, sizeof(ScePadControllerInformation));
				ret = scePadGetControllerInformation(info->padContext->getPadHandle(), &padInfo);
				if ((ret == SCE_OK) && padInfo.connected) {
					m_currentPlayerHasController = true; 
				}
			}
			if (m_currentPlayerHasController) {
				//E Main process of the game.
				//J ゲームのメイン処理
				updateGameMain(info);
			} else {
				//E Return to the title screen if cancel button is pressed.
				//J キャンセルボタンが押されたらタイトル画面に戻る
				bool buttonPressed = false;
				for (std::vector<UserInfo*>::iterator it = usersInfo->begin(); it != usersInfo->end(); it++) {
					buttonPressed |= (*it)->padContext->isButtonPressed(ssi::kButtonCircle, ssi::kButtonEventPatternAny);
				}
				if (isAnyonePressAnyButtons) {
					m_shouldQuit = true;
				}
			}
		} else {
			//E Return to the title screen if any button is pressed.
			//J いずれかのボタンが押されたらタイトル画面に戻る
			bool buttonPressed = false;
			for (std::vector<UserInfo*>::iterator it = usersInfo->begin(); it != usersInfo->end(); it++) {
				buttonPressed |= (*it)->padContext->isButtonPressed(g_allButtons, ssi::kButtonEventPatternAny);
			}
			if (buttonPressed) {
				m_shouldQuit = true;
			}
		}
	}

	return SCE_OK;
}

void GameMainScene::render(ssg::GraphicsContext *graphicsContext, ssg::SpriteRenderer *spriteRenderer, ResourceManager *resourceManager)
{
	switch (m_mode)
	{
	case MODE_START:
		drawDebugStringCenter(graphicsContext, spriteRenderer, 0.49f, 0.04f, g_orange, "Game start");
		break;
	case MODE_PLAYING:
		if (m_currentPlayerHasController) {
			//E Render the game board.
			//J 盤面を描画
			for (int y = 0; y < GAME_BOARD_HEIGHT; y++) {
				for (int x = 0; x < GAME_BOARD_WIDTH; x++) {
					spriteRenderer->fillRect(graphicsContext,
							vecmath::Vector2(0.510 + x * 0.115, 0.240 + y * 0.204), 
							vecmath::Vector2(0.111, 0.197), 
							g_gray);
					if (m_gameBoard[x][y] != PIECE_TYPE_EMPTY) {
						ResourceManager::Name tex = (m_gameBoard[x][y] == PIECE_TYPE_PLAYER1)
							? ResourceManager::TEX_DUCK : ResourceManager::TEX_BOY;
						spriteRenderer->drawTexture(graphicsContext, 
							vecmath::Vector2(0.513 + x * 0.115, 0.269 + y * 0.204), 
							vecmath::Vector2(0.104, 0.139),
							resourceManager->getTexture(tex));
					}
				}
			}

			//E Render the information of players.
			//J Player情報を描画
			for (int i = 0; i < PLAYER_INDEX_MAX; i++) {
				vecmath::Vector4 matColor = g_gray;
				matColor.setW(((m_result == GAME_RESULT_IN_PLAY) && (m_currentPlayer == i)) ? (getBlink() * 0.2 + 0.4) : 0.4);
				spriteRenderer->fillRect(graphicsContext, vecmath::Vector2(0.085, 0.316 + 0.160 * i), 
					vecmath::Vector2(0.339, 0.139), matColor);

				spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.120, 0.355 + 0.160 * i), 0.04,
					m_playersColor[i], "Player");
				spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.120, 0.390 + 0.160 * i), 0.04, 
					g_white, "name: [%s]", m_playersName[i].c_str());
				spriteRenderer->drawTexture(graphicsContext, vecmath::Vector2(0.178, 0.339 + 0.160 * i), 
					vecmath::Vector2(0.040, 0.055), 
					resourceManager->getTexture((i == PLAYER_INDEX_1) ? ResourceManager::TEX_DUCK : ResourceManager::TEX_BOY));
			}

			if (m_result == GAME_RESULT_IN_PLAY) {
				//E Render the blinking cursor.
				//J 点滅カーソルを描画
				vecmath::Vector4 blinkedColor = m_playersColor[m_currentPlayer];
				blinkedColor.setW(getBlink() * 0.3 + 0.1);
				spriteRenderer->fillRect(graphicsContext,
						vecmath::Vector2(0.510 + m_cursorX * 0.115, 0.240 + m_cursorY * 0.204), 
						vecmath::Vector2(0.111, 0.197), 
						blinkedColor);

				spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.091), 0.06, 
					g_white, "It's [%s]'s turn.", m_playersName[m_currentPlayer].c_str());

				spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.920), 0.03,
					g_white_gray, "x:enter, up/left/down/right:move cursor");
			} else {
				vecmath::Vector4 blinkedColor = g_white;
				blinkedColor.setW(getBlink() * 0.8 + 0.2);
				if (m_result == GAME_RESULT_DRAW) {
					spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.091), 0.06, 
						blinkedColor, "The game was drawn.");
				} else {
					int idx = (m_result == GAME_RESULT_PLAYER1_WIN) ? 0 : 1;
					spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.091), 0.06, 
						blinkedColor, "The winner is [%s].", m_playersName[idx].c_str());

					//E Blink a line.
					//J そろったところを点滅させる
					vecmath::Vector4 blinkedColor = g_yellow;
					blinkedColor.setW(getBlink() * 0.2 + 0.1);
					for (int y = 0; y < GAME_BOARD_HEIGHT; y++) {
						for (int x = 0; x < GAME_BOARD_WIDTH; x++) {
							if (m_resultBoard[x][y]) {
								spriteRenderer->fillRect(graphicsContext,
										vecmath::Vector2(0.510 + x * 0.115, 0.240 + y * 0.204), 
										vecmath::Vector2(0.111, 0.197), 
										blinkedColor);
							}
						}
					}
				}
				blinkedColor = g_white_gray;
				blinkedColor.setW(getBlink() * 0.8 + 0.2);
				spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.920), 0.03,
					blinkedColor, "Press any key");
			}
		} else {
			drawDebugStringCenter(graphicsContext, spriteRenderer, 0.49f, 0.04f, g_orange, "Please connect your controller, [%s].", m_playersName[m_currentPlayer].c_str());
			spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.050, 0.920), 0.03, g_white_gray, "o:go to title screen");
		}
		break;
	case MODE_PLAYER_IS_LOGGED_OUT:
		drawDebugStringCenter(graphicsContext, spriteRenderer, 0.41f, 0.08f, g_orange, "Game over");
		drawDebugStringCenter(graphicsContext, spriteRenderer, 0.56f, 0.04f, g_orange, "A player has been logged out.");
		break;
	default:
		break;
	}
}

void GameMainScene::_timerCallback(void *arg, int param)
{
	((GameMainScene*)arg)->timerCallback(param);
}

void GameMainScene::timerCallback(int param)
{
	if (m_mode == MODE_START) {
		m_mode = MODE_PLAYING;
		setNextPlayer(PLAYER_INDEX_1);
	} else if (m_mode == MODE_PLAYER_IS_LOGGED_OUT) {
		m_shouldQuit = true;
	}
}

float GameMainScene::getBlink()
{
	const float _DURATION = 75.0;
	return ABS(((m_frameCount % (int)_DURATION) * 2.0 / _DURATION) - 1.0);
}

bool GameMainScene::isPlayersIdValid(PlayersIds ids) const
{
	bool valid = true;
	for (int i = 0; i < PLAYER_INDEX_MAX; i++) {
		valid &= (ids.player[i] != SCE_USER_SERVICE_USER_ID_INVALID);
	}
	return valid;
}

void GameMainScene::updateGameMain(UserInfo* player)
{
	bool crossPressed = player->padContext->isButtonReleased(ssi::kButtonCross, ssi::kButtonEventPatternAll);
	bool rightPressed = player->padContext->isButtonPressed(ssi::kButtonRight, ssi::kButtonEventPatternAll);
	bool leftPressed  = player->padContext->isButtonPressed(ssi::kButtonLeft, ssi::kButtonEventPatternAll);
	bool upPressed    = player->padContext->isButtonPressed(ssi::kButtonUp, ssi::kButtonEventPatternAll);
	bool downPressed  = player->padContext->isButtonPressed(ssi::kButtonDown, ssi::kButtonEventPatternAll);

	if (crossPressed) {
		if (m_gameBoard[m_cursorX][m_cursorY] == PIECE_TYPE_EMPTY) {
			m_gameBoard[m_cursorX][m_cursorY]
				= (m_currentPlayer == PLAYER_INDEX_1) ? PIECE_TYPE_PLAYER1 : PIECE_TYPE_PLAYER2;
			m_turnsCount++;
			//J 3つ揃った個所がないかチェックする
			//E Search for a row that consist entirely of same mark.
			m_result = findThreeMarksRow(m_currentPlayer);
			if (m_result == GAME_RESULT_IN_PLAY) {
				PlayerIndex nextPlayer = (m_currentPlayer == PLAYER_INDEX_1) ? PLAYER_INDEX_2 : PLAYER_INDEX_1;
				setNextPlayer(nextPlayer);
			}
		}
	} else {
		//J カーソルを移動する
		//E Move the cursor.
		m_cursorX += (rightPressed ? 1 : 0) - (leftPressed ? 1 : 0);
		m_cursorX = MAX(MIN(m_cursorX, GAME_BOARD_WIDTH - 1), 0);
		m_cursorY += (downPressed ? 1 : 0) - (upPressed ? 1 : 0);
		m_cursorY = MAX(MIN(m_cursorY, GAME_BOARD_HEIGHT - 1), 0);
	}
}

void GameMainScene::setNextPlayer(PlayerIndex player)
{
	m_currentPlayer = player;

	//E Assign a controller to next player.
	//J 次のプレイヤーに対してデバイスの割り当てを行う
	SceLoginServiceDeviceRequestList list;
	sceLoginServiceDeviceRequestListParamInitialize(&list);
	list.request[0].userId = m_playersIds.player[m_currentPlayer];
	list.request[0].devices = SCE_LOGIN_SERVICE_USER_ASSIGNED_DEVICE_TYPE_PAD_STANDARD;

	int ret = sceLoginServiceRequestDevices(&list);
	printf("sceLoginServiceRequestDevices(%s) ret=0x%x\n", m_playersName[m_currentPlayer].c_str(), ret);
}

void GameMainScene::clearResultBoard()
{
	for (int y = 0; y < GAME_BOARD_HEIGHT; y++) {
		for (int x = 0; x < GAME_BOARD_WIDTH; x++) {
			m_resultBoard[x][y] = false;
		}
	}
}

GameMainScene::GameResult GameMainScene::findThreeMarksRow(PlayerIndex currentPlayer)
{
	PieceType targetPiece = (currentPlayer == PLAYER_INDEX_1) ? PIECE_TYPE_PLAYER1 : PIECE_TYPE_PLAYER2;
	struct { int startX; int startY; int progressX; int progressY; } tbl[] = {
		{ 0, 0, 1, 0 }, { 0, 1, 1, 0 }, { 0, 2, 1, 0 },		//E Horizontal direction
		{ 0, 0, 0, 1 }, { 1, 0, 0, 1 }, { 2, 0, 0, 1 },		//E Vertical direction
		{ 0, 0, 1, 1 }, { 2, 0,-1, 1 } };					//E Diagonal direction
	int tblSize = sizeof(tbl) / sizeof(tbl[0]);
	for (int i = 0; i < tblSize; i++) {
		clearResultBoard();
		bool doesMatch = true;
		int x = tbl[i].startX;
		int y = tbl[i].startY;
		for (int j = 0; j < 3; j++) {
			doesMatch &= (m_gameBoard[x][y] == targetPiece);
			m_resultBoard[x][y] = true;
			x += tbl[i].progressX;
			y += tbl[i].progressY;
		}
		if (doesMatch) {
			return (currentPlayer == PLAYER_INDEX_1) ? GAME_RESULT_PLAYER1_WIN : GAME_RESULT_PLAYER2_WIN;
		}
	}
	clearResultBoard();
	return (m_turnsCount >= MAX_TURNS_COUNT) ? GAME_RESULT_DRAW : GAME_RESULT_IN_PLAY;
}