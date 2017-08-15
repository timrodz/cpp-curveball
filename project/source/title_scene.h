/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#pragma once

#include "common.h"

class TitleScene : public SceneBase
{
public:
	TitleScene();
	~TitleScene();

	virtual int initialize();
	virtual void terminate();

	virtual SceneId getSceneId()
	{
		return SCENE_ID_TITLE;
	}
	virtual EventId getEvent();

	virtual int update(UserInfoCollection *usersInfo);
	virtual void render(ssg::GraphicsContext *graphicsContext, ssg::SpriteRenderer *spriteRenderer, ResourceManager *resourceManager);

private:
	bool m_isButtonPressed;
};
