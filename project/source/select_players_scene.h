/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#pragma once

#include "common.h"

class SelectPlayersScene : public SceneBase
{
public:
	SelectPlayersScene();
	~SelectPlayersScene();

	virtual int initialize();
	virtual void terminate();

	virtual SceneId getSceneId()
	{
		return SCENE_ID_SELECT_PLAYERS;
	}
	virtual EventId getEvent();

	virtual int update(UserInfoCollection *usersInfo);
	virtual void render(ssg::GraphicsContext *graphicsContext, ssg::SpriteRenderer *spriteRenderer, ResourceManager *resourceManager);

	PlayersIds getSelectedPlayers();

private:
	std::string m_playersName[PLAYER_INDEX_MAX];
	PlayersIds m_playersIds;
	EventId m_event;
	int m_cursorPosition;
	bool m_isLoginDialog;

private:
	bool canStartGame();
};
