/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#include "title_scene.h"

TitleScene::TitleScene()
	: m_isButtonPressed(false)
{
}

TitleScene::~TitleScene()
{
}

int TitleScene::initialize()
{
	int ret = SceneBase::initialize();
	SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
	if (ret < SCE_OK) { return ret; } 

	m_isButtonPressed = false;

	return SCE_OK;
}

void TitleScene::terminate()
{
	SceneBase::terminate();
}

EventId TitleScene::getEvent()
{
	return m_isButtonPressed ? EVENT_ID_GO_FORWARD : EVENT_ID_NONE;
}

int TitleScene::update(UserInfoCollection *usersInfo)
{
	//E Read button state
	//J ボタン押下を検出
	for (std::vector<UserInfo*>::iterator it = usersInfo->begin(); it != usersInfo->end(); it++) {
		m_isButtonPressed |= (*it)->padContext->isButtonPressed(g_allButtons, ssi::kButtonEventPatternAny);
	}

	return SCE_OK;
}

void TitleScene::render(ssg::GraphicsContext *graphicsContext, ssg::SpriteRenderer *spriteRenderer, ResourceManager *resourceManager)
{
	spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.271f, 0.193f), 0.15f, g_gray, "Tic-tac-toe");
	spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.268f, 0.190f), 0.15f, g_orange, "Tic-tac-toe");

	spriteRenderer->drawTexture(graphicsContext, vecmath::Vector2(0.163, 0.175), vecmath::Vector2(0.130, 0.173), resourceManager->getTexture(ResourceManager::TEX_BOY));
	spriteRenderer->drawTexture(graphicsContext, vecmath::Vector2(0.713, 0.175), vecmath::Vector2(0.130, 0.173), resourceManager->getTexture(ResourceManager::TEX_DUCK));

	drawDebugStringCenter(graphicsContext, spriteRenderer, 0.49f, 0.04f, g_orange, "Press any key to start");

	spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.188f, 0.76f), 0.03f, g_white, "Note:");
	spriteRenderer->drawDebugStringf(graphicsContext, vecmath::Vector2(0.188f, 0.80f), 0.03f, g_white, "To study this application, please connect just one controller on your PS4.");
}
