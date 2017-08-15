/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#pragma once

#include "common.h"

#define GAME_BOARD_WIDTH			3
#define GAME_BOARD_HEIGHT			3

typedef void (*FrameTimerCallback)(void *arg, int param);

class FrameTimer
{
public:
	FrameTimer()
	{
		clear();
	}

	int set(FrameTimerCallback func, void *arg, int param, uint duration)
	{
		if (isWorking()) { return -1; }
		m_func = func;
		m_arg = arg;
		m_param = param;
		m_countdown = duration;
		return SCE_OK;
	}

	bool isWorking()
	{
		return (m_countdown != 0);
	}

	void clear()
	{
		m_func = NULL;
		m_arg = NULL;
		m_param = 0;
		m_countdown = 0;
	}

	void update()
	{
		if (m_countdown > 0) {
			m_countdown--;
			if (m_countdown == 0) {
				m_func(m_arg, m_param);
				clear();
			}
		}
	}

private:
	FrameTimerCallback m_func;
	void *m_arg;
	int m_param;
	uint32_t m_countdown;
};

class GameMainScene : public SceneBase
{
public:
	GameMainScene();
	~GameMainScene();

	int setPlayers(PlayersIds ids);

	virtual int initialize();
	virtual void terminate();

	virtual SceneId getSceneId()
	{
		return SCENE_ID_GAME_MAIN;
	}
	virtual EventId getEvent();

	virtual int update(UserInfoCollection *usersInfo);
	virtual void render(ssg::GraphicsContext *graphicsContext, ssg::SpriteRenderer *spriteRenderer, ResourceManager *resourceManager);

private:
	enum Mode {
		MODE_START,
		MODE_PLAYING,
		MODE_PLAYER_IS_LOGGED_OUT,
	};
	enum PieceType {
		PIECE_TYPE_EMPTY,
		PIECE_TYPE_PLAYER1,
		PIECE_TYPE_PLAYER2,
	};
	enum GameResult {
		GAME_RESULT_IN_PLAY		= -1,
		GAME_RESULT_DRAW		=  0,
		GAME_RESULT_PLAYER1_WIN,
		GAME_RESULT_PLAYER2_WIN,
	};

	Mode m_mode;
	bool m_shouldQuit;
	PlayerIndex m_currentPlayer;
	bool m_currentPlayerHasController;

	PlayersIds m_playersIds;
	std::string m_playersName[PLAYER_INDEX_MAX];
	vecmath::Vector4 m_playersColor[PLAYER_INDEX_MAX];

	PieceType m_gameBoard[GAME_BOARD_WIDTH][GAME_BOARD_HEIGHT];
	int m_cursorX;
	int m_cursorY;
	uint m_turnsCount;

	GameResult m_result;
	bool m_resultBoard[GAME_BOARD_WIDTH][GAME_BOARD_HEIGHT];

	uint m_frameCount;
	FrameTimer m_timer;

private:
	static void _timerCallback(void *arg, int param);
	void timerCallback(int param);

	float getBlink();
	bool isPlayersIdValid(PlayersIds ids) const;

	void updateGameMain(UserInfo *player);
	void setNextPlayer(PlayerIndex player);
	void clearResultBoard();
	GameResult findThreeMarksRow(PlayerIndex currentPlayer);
};
