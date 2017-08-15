/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#include <sampleutil/input/controller.h>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil_internal.h>

#include "pad_common.h"

/* ------------------------------------------------------------------------- */
namespace ssi  = sce::SampleUtil::Input;
namespace ssii = sce::SampleUtil::Input::Impl;

const float ssii::PadContextImpl::m_recipMaxByteAsFloat	= 1.0f / 255.0f;

/* ------------------------------------------------------------------------- */
ssii::PadContextImpl::PadContextImpl(void)
{
}

/* ------------------------------------------------------------------------- */
ssii::PadContextImpl::~PadContextImpl(void)
{
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextImpl::getData(ssi::PadData *data, uint32_t length) const
{
	if (data == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	length = (this->m_numCurrentPadData) < length ? this->m_numCurrentPadData : length;
	for (uint32_t i=0; i<length; i++) {
		data[i] = this->m_currentPadData[i];
	}

	return length;
}

int ssii::PadContextImpl::getPreviousUpdateData(ssi::PadData *data, uint32_t length) const
{
	if (data == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	length = (this->m_numPreviousPadData) < length ? this->m_numPreviousPadData : length;
	for (uint32_t i=0; i<length; i++) {
		data[i] = this->m_previousPadData[i];
	}

	return length;
}


int ssii::PadContextImpl::getPressedButtons(ssi::Button *pressedButton) const
{
	if (pressedButton == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	*pressedButton = (ssi::Button)this->m_pressedButtonData;

	return SCE_OK;
}

int ssii::PadContextImpl::getReleasedButtons(ssi::Button *releasedButton) const
{
	if (releasedButton == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	*releasedButton = (ssi::Button)this->m_releasedButtonData;

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
static bool _buttonBitPatternMatch(uint32_t buttonData, uint32_t bits, ssi::ButtonEventPattern pattern)
{
	if (pattern == ssi::kButtonEventPatternAny) {
		if ((buttonData & bits) != 0) {	
			return true;
		} else {
			return false;
		}
	} else if (pattern == ssi::kButtonEventPatternAll) {
		if ((buttonData & bits) == bits) {
			return true;
		} else {
			return false;
		}
	}

	return false;
}

/* ------------------------------------------------------------------------- */
bool ssii::PadContextImpl::isButtonDown(uint32_t buttons, ssi::ButtonEventPattern pattern) const
{
	return _buttonBitPatternMatch(this->m_currentPadData[0].buttons, buttons, pattern);
}

/* ------------------------------------------------------------------------- */
bool ssii::PadContextImpl::isButtonUp(uint32_t buttons, ssi::ButtonEventPattern pattern) const
{
	return !_buttonBitPatternMatch(this->m_currentPadData[0].buttons, buttons, pattern);
}

/* ------------------------------------------------------------------------- */
bool ssii::PadContextImpl::isButtonPressed(uint32_t buttons, ssi::ButtonEventPattern pattern) const
{
	return _buttonBitPatternMatch(this->m_pressedButtonData, buttons, pattern);
}

/* ------------------------------------------------------------------------- */
bool ssii::PadContextImpl::isButtonReleased(uint32_t buttons, ssi::ButtonEventPattern pattern) const
{
	return _buttonBitPatternMatch(this->m_releasedButtonData, buttons, pattern);
}

/* ------------------------------------------------------------------------- */
const sce::Vectormath::Simd::Aos::Vector2& ssii::PadContextImpl::getLeftStick() const
{
	return this->m_leftStickValue;
}

/* ------------------------------------------------------------------------- */
const sce::Vectormath::Simd::Aos::Vector2& ssii::PadContextImpl::getRightStick() const
{
	return this->m_rightStickValue;
}

/* ------------------------------------------------------------------------- */
void ssii::PadContextImpl::setRightAnalogStickDeadZone(float rightAnalogStickDeadZone)
{
	this->m_rightAnalogStickDeadZone = rightAnalogStickDeadZone;
}

/* ------------------------------------------------------------------------- */
void ssii::PadContextImpl::setLeftAnalogStickDeadZone(float leftAnalogStickDeadZone)
{
	this->m_leftAnalogStickDeadZone = leftAnalogStickDeadZone;
}

/* ------------------------------------------------------------------------- */
float ssii::PadContextImpl::getDefaultLeftAnalogStickDeadZone(void)
{
	return this->m_defaultLeftAnalogStickDeadZone;
}

/* ------------------------------------------------------------------------- */
float ssii::PadContextImpl::getDefaultRightAnalogStickDeadZone(void)
{
	return this->m_defaultRightAnalogStickDeadZone;
}

/* ------------------------------------------------------------------------- */
sce::SampleUtil::System::UserId ssii::PadContextImpl::getUserId(void)
{
	return this->m_userId;
}

/* ------------------------------------------------------------------------- */
int32_t ssii::PadContextImpl::getType(void)
{
	return this->m_type;
}

/* ------------------------------------------------------------------------- */
int32_t ssii::PadContextImpl::getIndex(void)
{
	return this->m_index;
}


#if _SCE_TARGET_OS_WINDOWS || _SCE_TARGET_OS_PSP2
#if !(defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS)

/* ------------------------------------------------------------------------- */
int ssii::PadContextImpl::initialize(
	const sce::SampleUtil::System::UserId userId, 
	const int32_t type, 
	const int32_t index, 
	const ssi::PadContextOption* pOption)
{
	(void)pOption;
	int ret = SCE_OK;

	if (pOption == NULL) {
		m_numPadData = kDefaultNumPadDataBufs;
	} else {
		if (pOption->numBufs < 1 || kMaxNumPadDataBufs < pOption->numBufs) {
			return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
		}
		m_numPadData = pOption->numBufs;
	}

	this->m_userId  = userId;
	this->m_type    = type;
	this->m_index   = index;

	m_currentPadData = (ssi::PadData *)malloc(sizeof(ssi::PadData) * m_numPadData);
	if (m_currentPadData == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}

	m_previousPadData = (ssi::PadData *)malloc(sizeof(ssi::PadData) * m_numPadData);
	if (m_previousPadData == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}

	for (uint32_t i = 0; i < m_numPadData; i++) {
		memset((void*)&m_currentPadData[i],  0, sizeof(m_currentPadData[i]) );
		memset((void*)&m_previousPadData[i], 0, sizeof(m_previousPadData[i]));
	}

	m_defaultRightAnalogStickDeadZone = 0.25;
	m_defaultLeftAnalogStickDeadZone  = 0.25;

	this->m_leftAnalogStickDeadZone  = this->m_defaultLeftAnalogStickDeadZone;
	this->m_rightAnalogStickDeadZone = this->m_defaultRightAnalogStickDeadZone;

	/* ------------ Initialize Controller ------------ */
	ret = initializeController(pOption);
	if (ret != SCE_OK) {
		return SCE_OK;
	}

	/* ------------ Initialize Motion ------------ */
	ret = initializeMotion(pOption);
	if (ret != SCE_OK) {
		return SCE_OK;
	}

	/* ------------ Initialize Touch ------------ */
	ret = initializeTouchDevice(pOption);
	if (ret != SCE_OK) {
		return SCE_OK;
	}

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextImpl::finalize(void)
{
	int ret = SCE_OK;

	/* ------------ Finalize Controller ------------ */
	ret = finalizeController();
	if (ret != SCE_OK) {
		return SCE_OK;
	}

	/* ------------ Finalize Motion ------------ */
	ret = finalizeMotion();
	if (ret != SCE_OK) {
		return SCE_OK;
	}

	/* ------------ Finalize Touch ------------ */
	ret = finalizeTouchDevice();
	if (ret != SCE_OK) {
		return SCE_OK;
	}

	return SCE_OK;
}

/* ------------------------------------------------------------------------- */
int ssii::PadContextImpl::update(void)
{
	/* ------------ swap prev / current ------------ */
	ssi::PadData *swap = this->m_previousPadData;
	this->m_previousPadData = this->m_currentPadData;
	this->m_currentPadData  = swap;

	/* ------------ Update Controller ------------ */
	updateController(this->m_currentPadData, this->m_numPadData);
	
	/* ------------ Update Motion ------------ */
	updateMotion(this->m_currentPadData, this->m_numPadData);

	/* ------------ Update Touch ------------ */
	updateTouchDevice(this->m_currentPadData, this->m_numPadData);

	/* ------------ Check Pressed/Released ------------ */
	m_pressedButtonData  = ( m_currentPadData[0].buttons & ~m_previousPadData[0].buttons);
	m_releasedButtonData = (~m_currentPadData[0].buttons &  m_previousPadData[0].buttons);

	/* ------------ Update Stick Position ------------ */
	float X, Y;

	// Right Stick
	{
		X = (float)((int32_t)this->m_currentPadData[0].rx * 2 - 255) * this->m_recipMaxByteAsFloat;
		Y = (float)((int32_t)this->m_currentPadData[0].ry * 2 - 255) * this->m_recipMaxByteAsFloat;

		X = fabsf(X) < this->m_rightAnalogStickDeadZone ? 0 : X;
		Y = fabsf(Y) < this->m_rightAnalogStickDeadZone ? 0 : Y;

		this->m_rightStickValue.setX(X);
		this->m_rightStickValue.setY(Y);
	}

	// Left Stick
	{
		X = (float)((int32_t)this->m_currentPadData[0].lx * 2 - 255) * this->m_recipMaxByteAsFloat;
		Y = (float)((int32_t)this->m_currentPadData[0].ly * 2 - 255) * this->m_recipMaxByteAsFloat;

		X = fabsf(X) < this->m_leftAnalogStickDeadZone ? 0 : X;
		Y = fabsf(Y) < this->m_leftAnalogStickDeadZone ? 0 : Y;

		this->m_leftStickValue.setX(X);
		this->m_leftStickValue.setY(Y);
	}

	return SCE_OK;
}

#endif/* !(defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) */
#endif /*_SCE_TARGET_OS_WINDOWS || _SCE_TARGET_OS_PSP2*/

/* ------------------------------------------------------------------------- */
int ssi::createPadContext(sce::SampleUtil::Input::PadContext **outPadContext,
						  const sce::SampleUtil::System::UserId userId,
						  const int32_t type,
						  const int32_t index,
						  const PadContextOption* option)
{
	int ret = SCE_OK;

#if _SCE_TARGET_OS_ORBIS
	ret = ssii::createOrbisPadContext(outPadContext, userId, type, index, option);
#endif/*_SCE_TARGET_OS_ORBIS*/

#if _SCE_TARGET_OS_PSP2
	ret = ssii::createVitaPadContext(outPadContext, userId, type, index, option);
#endif/*_SCE_TARGET_OS_PSP2*/

#if _SCE_TARGET_OS_WINDOWS
	ret = ssii::createWindowsPadContext(outPadContext, userId, type, index, option);
#endif/*_SCE_TARGET_OS_WINDOWS*/

	return ret;
}
