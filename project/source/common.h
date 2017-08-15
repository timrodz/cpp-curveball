/* SIE CONFIDENTIAL
 PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2016 Sony Interactive Entertainment Inc.
 * All Rights Reserved.
 */

#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <sampleutil.h>
#include <user_service.h>

namespace vecmath = sce::Vectormath::Simd::Aos;
namespace ssg = sce::SampleUtil::Graphics;
namespace ssi = sce::SampleUtil::Input;
namespace ssd = sce::SampleUtil::Debug;
namespace sss = sce::SampleUtil::System;

// Color
const vecmath::Vector4 g_white      = vecmath::Vector4(1.0, 1.0, 1.0, 1.0);
const vecmath::Vector4 g_white_gray = vecmath::Vector4(0.7, 0.7, 0.7, 1.0);
const vecmath::Vector4 g_gray       = vecmath::Vector4(0.4, 0.4, 0.4, 1.0);
const vecmath::Vector4 g_red		= vecmath::Vector4(1.0, 0.0, 0.0, 1.0);
const vecmath::Vector4 g_green		= vecmath::Vector4(0.0, 1.0, 0.0, 1.0);
const vecmath::Vector4 g_blue		= vecmath::Vector4(0.0, 0.0, 1.0, 1.0);
const vecmath::Vector4 g_orange		= vecmath::Vector4(1.0, 0.5, 0.0, 1.0);
const vecmath::Vector4 g_pink		= vecmath::Vector4(1.0, 192.0 / 255.0, 203.0 / 255.0, 1.0);
const vecmath::Vector4 g_black		= vecmath::Vector4(0.0, 0.0, 0.0, 1.0);
const vecmath::Vector4 g_yellow		= vecmath::Vector4(1.0, 1.0, 0.0, 1.0);

const ssi::Button g_allButtons = (ssi::Button)( ssi::kButtonL3 | ssi::kButtonR3 | ssi::kButtonOptions | ssi::kButtonUp
									| ssi::kButtonRight | ssi::kButtonDown | ssi::kButtonLeft | ssi::kButtonL2
									| ssi::kButtonR2 | ssi::kButtonL1 | ssi::kButtonR1 | ssi::kButtonTriangle
									| ssi::kButtonCircle | ssi::kButtonCross | ssi::kButtonSquare | ssi::kButtonTouchPad);

class ResourceManager
{
public:
	enum Name 
	{
		TEX_BOY,
		TEX_DUCK,

		TEX_ENUM_MAX,
	};
	
private:
	ssg::Texture *m_textures[TEX_ENUM_MAX];

public:
	ResourceManager() 
	{
		for (int i = 0; i < TEX_ENUM_MAX; i++) {
			m_textures[i] = NULL;
		}
	}

	int initialize(ssg::GraphicsContext *graphicsContext)
	{
		struct { Name name; const char* path; } tbl[] = { 
			{ TEX_BOY, "/app0/data/graphics/model/boy/preview.png" },
			{ TEX_DUCK, "/app0/data/graphics/model/duck/preview.png" }
		};

		int ret = 0;
		(void)ret;

		const int tblSize = sizeof(tbl) / sizeof(tbl[0]);
		for (int i = 0; i < tblSize; i++) {
			ret = graphicsContext->createTextureFromFile(&(m_textures[tbl[i].name]), tbl[i].path);
			SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
			if (ret < SCE_OK) { break; }
		}

		return ret;
	}

	void terminate()
	{
		for (int i = 0; i < TEX_ENUM_MAX; i++) {
			if (m_textures[i] != NULL) {
				sce::SampleUtil::destroy(m_textures[i]);
				m_textures[i] = NULL;
			}
		}
	}

	ssg::Texture* getTexture(Name name)
	{
		return m_textures[name];
	}

};

struct UserInfo
{
	UserInfo(SceUserServiceUserId _userId, const char *_userName, const SceUserServiceUserColor _userColor) 
		: userId(_userId)
		, userName(_userName)
		, padContext(NULL)
	{
		setColor(_userColor);
	}

	~UserInfo()
	{
	}

	SceUserServiceUserId userId;
	std::string userName;
	vecmath::Vector4 userColor;
	ssi::PadContext *padContext;

	int initialize()
	{
		//pad open
		int ret = createPadContext(&padContext, userId, 0, 0, NULL);
		SCE_SAMPLE_UTIL_ASSERT(ret == SCE_OK);
		return ret;
	}

	int finalize()
	{
		//pad close
		if (padContext != NULL){
			sce::SampleUtil::destroy(padContext);
		}
		return SCE_OK;
	}

	int update()
	{
		return padContext->update();
	}

	int setColor(SceUserServiceUserColor _userColor)
	{
		switch (_userColor) {
		case SCE_USER_SERVICE_USER_COLOR_BLUE:
			userColor = g_blue;
			break;
		case SCE_USER_SERVICE_USER_COLOR_RED:
			userColor = g_red;
			break;
		case SCE_USER_SERVICE_USER_COLOR_GREEN:
			userColor = g_green;
			break;
		case SCE_USER_SERVICE_USER_COLOR_PINK:
			userColor = g_pink;
			break;
		default:
			break;
		}
		return SCE_OK;
	}
};

class UserInfoCollection
{
private:
	std::vector<UserInfo*> m_list;
public:
	void push_back(UserInfo *info)
	{
		m_list.push_back(info);
	}

	std::vector<UserInfo*>::iterator begin()
	{
		return m_list.begin();
	}

	std::vector<UserInfo*>::iterator end()
	{
		return m_list.end();
	}

	std::vector<UserInfo*>::iterator erase(std::vector<UserInfo*>::iterator it)
	{
		return m_list.erase(it);
	}

	void clear()
	{
		m_list.clear();
	}

	bool empty()
	{
		return m_list.empty();
	}

	UserInfo* get(SceUserServiceUserId userId)
	{
		std::vector<UserInfo*>::iterator it;
		for (it = m_list.begin(); it != m_list.end(); it++) {
			if ((*it)->userId == userId) {
				return *it;
			}
		}
		return NULL;
	}
};

enum EventId
{
	EVENT_ID_NONE,
	EVENT_ID_GO_FORWARD,
	EVENT_ID_GO_BACKWARD,
};

enum SceneId
{
	SCENE_ID_TITLE,
	SCENE_ID_SELECT_PLAYERS,
	SCENE_ID_GAME_MAIN,
};

class SceneBase
{
public:
	SceneBase() : m_initialized(false)
	{
	}

	~SceneBase()
	{
	}

	virtual int initialize() 
	{
		if (m_initialized) { return -1; } 
		m_initialized = true;
		return SCE_OK;
	}

	virtual void terminate()
	{
		m_initialized = false;
	}

	virtual SceneId getSceneId() = 0;
	virtual EventId getEvent() = 0;

	virtual int update(UserInfoCollection *usersInfo) = 0;
	virtual void render(ssg::GraphicsContext *graphicsContext, ssg::SpriteRenderer *spriteRenderer, ResourceManager *resourceManager) = 0;

	bool isInitialized()
	{
		return m_initialized;
	}

private:
	bool m_initialized;

protected:
	void drawDebugStringCenter(
		ssg::GraphicsContext *graphicsContext, 
		ssg::SpriteRenderer *spriteRenderer,
		float verticalPosition,
		float fontHeight,
		vecmath::Vector4_arg colorCoeff,
		const char *format, ... )
	{
		char buffer[128];
		int stringLength;
		va_list arg;
		va_start(arg, format);
		stringLength = vsnprintf(buffer, sizeof(buffer), format, arg);
		buffer[sizeof(buffer)-1] = '\0';
		va_end(arg);
		if (stringLength <= 0) { return; }

		float charWidth = spriteRenderer->getWidthOfDebugChar(fontHeight);
		float stringWidth = charWidth * stringLength;

		spriteRenderer->drawDebugStringf(graphicsContext, 
			vecmath::Vector2((1.0f - stringWidth)/2.0f, verticalPosition), fontHeight, colorCoeff, buffer);
	}
};

enum PlayerIndex {
	PLAYER_INDEX_1,
	PLAYER_INDEX_2,
	PLAYER_INDEX_MAX,
};

struct PlayersIds
{
	PlayersIds()
	{
		clear();
	}

	void clear()
	{
		for (int i = 0; i < PLAYER_INDEX_MAX; i++) {
			player[i] = SCE_USER_SERVICE_USER_ID_INVALID;
		}
	}
	SceUserServiceUserId player[PLAYER_INDEX_MAX];
};
