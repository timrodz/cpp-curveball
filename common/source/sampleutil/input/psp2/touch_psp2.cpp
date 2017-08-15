/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <sampleutil/input/touch.h>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil/sampleutil_common.h>
#include <sampleutil_internal.h>

#if _SCE_TARGET_OS_PSP2

#include <libsysmodule.h>

#pragma comment(lib, "SceTouch_stub")
#pragma comment(lib, "SceSystemGesture_stub")

namespace sce
{
	namespace SampleUtil
	{
		namespace Input
		{
			namespace Impl
			{
				class TouchContextImpl : public TouchContext, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					TouchContextImpl();
					virtual ~TouchContextImpl()
					{
						finalize();
					}
					int initialize(const TouchContextOption* option=NULL);

					int finalize();

					int update(); 

					int setFunctionFlag( const TouchPort port, const uint32_t functionFlag );

					uint32_t getFunctionFlag( const TouchPort port ) const;
					int getRawData(const TouchPort port, SceTouchData* touchData, const uint32_t size ) const;

					int getPanelInfo(const TouchPort port, SceTouchPanelInfo* panelInfo ) const;

					int getNumberOfPrimitiveGestureEvents() const;

					int getPrimitiveGestureEvents( TouchPrimitiveGestureEvent* events, const uint32_t size ) const;

					int getNumberOfGestureEvents() const;

					int getGestureEvents( TouchGestureEvent* events, const uint32_t size ) const;

					std::string getResourceType(void) const
					{
						return "Input::TouchContext";
					}

				private:
					bool setFunctionFlagState( const TouchPort port, const uint32_t type, const uint32_t flag );
					bool isFunctionFlagEnable( const TouchPort port, const uint32_t type ) const;

				private:
					static const uint32_t					m_maxRawTouchDataBufferSize			= 64;
					static const uint32_t 					m_primitiveTouchEventBufferSize		= SCE_TOUCH_MAX_REPORT * SCE_TOUCH_PORT_MAX_NUM * 3;
					static const uint32_t					m_numOfGestureType					= 5;
					static const uint32_t					m_numOfFunctionFlag					= 7;

					TouchContextOption						m_option;

					SceSystemGestureType					m_systemGestureType[m_numOfGestureType];
					uint32_t								m_functionFlag[m_numOfFunctionFlag];

					int										m_numOfRawTouchData[SCE_TOUCH_PORT_MAX_NUM];
					SceTouchData*							m_rawTouchData[SCE_TOUCH_PORT_MAX_NUM];
					SceTouchPanelInfo						m_panelInfo[SCE_TOUCH_PORT_MAX_NUM];

					SceSystemGesturePrimitiveTouchEvent*	m_primitiveTouchGestureEvent;
					uint32_t								m_numOfprimitiveGestureEvent;
					uint32_t								m_primitiveTouchGestureEventSize;

					SceSystemGestureTouchRecognizer*		m_touchRecognizer;
					SceSystemGestureTouchEvent*				m_touchGestureEvent;
					uint32_t								m_numOfGestureEvent;
					uint32_t								m_touchGestureEventSize;

					uint32_t								m_currentFunctionFlag[SCE_TOUCH_PORT_MAX_NUM];

				};
			}
		}
	}
}

namespace ssi = sce::SampleUtil::Input;
namespace ssii = sce::SampleUtil::Input::Impl;

ssii::TouchContextImpl::TouchContextImpl()
{
	//initialize gesture type
	m_systemGestureType[0] = SCE_SYSTEM_GESTURE_TYPE_TAP;
	m_systemGestureType[1] = SCE_SYSTEM_GESTURE_TYPE_DRAG;
	m_systemGestureType[2] = SCE_SYSTEM_GESTURE_TYPE_TAP_AND_HOLD;
	m_systemGestureType[3] = SCE_SYSTEM_GESTURE_TYPE_PINCH_OUT_IN;
	m_systemGestureType[4] = SCE_SYSTEM_GESTURE_TYPE_ROTATION;

	//initialize function flag 
	m_functionFlag[0] = kTouchContextFunctionFlagTapGestureEvent;
	m_functionFlag[1] = kTouchContextFunctionFlagDragGestureEvent;
	m_functionFlag[2] = kTouchContextFunctionFlagTapAndHoldGestureEvent;
	m_functionFlag[3] = kTouchContextFunctionFlagPinchOutInGestureEvent;
	m_functionFlag[4] = kTouchContextFunctionFlagRotationGestureEvent;
	m_functionFlag[5] = kTouchContextFunctionFlagPrimitiveGestureEvent;
	m_functionFlag[6] = kTouchContextFunctionFlagRawData;
}

int ssii::TouchContextImpl::initialize(const ssi::TouchContextOption* option)
{
	int ret = SCE_OK;

	//clear current function flag
	memset(m_currentFunctionFlag, 0, sizeof(m_currentFunctionFlag ));

	//initialize touch context option
	if (option != NULL) {
		for (int p=0; p<SCE_TOUCH_PORT_MAX_NUM; p++) {
			m_option.numBufs[p] = (option->numBufs[p] < m_maxRawTouchDataBufferSize) ? option->numBufs[p] : m_maxRawTouchDataBufferSize;
		}
	}

	//libTouch
	for (int p=0; p<SCE_TOUCH_PORT_MAX_NUM; p++) {
		ret = sceTouchGetPanelInfo(p, &m_panelInfo[p]);
		if (ret < SCE_OK) {
			return ret;
		}

		if (m_option.numBufs[p] > 0) {
			m_rawTouchData[p] = (SceTouchData*)malloc( sizeof(SceTouchData) * m_option.numBufs[p] );
			if (m_rawTouchData[p] == NULL) {
				return SCE_SAMPLE_UTIL_ERROR_FATAL;
			}
			memset((void*)m_rawTouchData[p], 0, (sizeof(SceTouchData) * m_option.numBufs[p]) );
		} else {
			m_rawTouchData[p] = NULL;
		}
	}

	//libSystemGesture

	//load module
	ret = sceSysmoduleLoadModule(SCE_SYSMODULE_SYSTEM_GESTURE);
	if (ret < SCE_OK) {
		return ret;
	}

	//setup primitive touch gesture
	m_primitiveTouchGestureEventSize = (
		sizeof(SceSystemGesturePrimitiveTouchEvent)
		* (m_option.numBufs[SCE_TOUCH_PORT_FRONT]+m_option.numBufs[SCE_TOUCH_PORT_BACK]) * m_primitiveTouchEventBufferSize);
	m_primitiveTouchGestureEvent = (SceSystemGesturePrimitiveTouchEvent*)malloc( m_primitiveTouchGestureEventSize);

	if (m_primitiveTouchGestureEvent == NULL){
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}
	memset((void*)m_primitiveTouchGestureEvent, 0, m_primitiveTouchGestureEventSize);

	ret = sceSystemGestureInitializePrimitiveTouchRecognizer(NULL);
	if (ret < SCE_OK) {
		return ret;
	}

	//prepare buffer for touch gesture recognizers
	m_touchRecognizer = (SceSystemGestureTouchRecognizer*)malloc((sizeof(SceSystemGestureTouchRecognizer) * SCE_TOUCH_PORT_MAX_NUM * m_numOfGestureType));
	if (m_touchRecognizer == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}
	memset((void*)m_touchRecognizer, 0, ( sizeof(SceSystemGestureTouchRecognizer) * SCE_TOUCH_PORT_MAX_NUM * m_numOfGestureType));
	//setup touch gesture recognizers
	uint32_t indexRecognizer = 0;
	for (int p=0; p<SCE_TOUCH_PORT_MAX_NUM; p++) {
		for (int i=0; i<m_numOfGestureType; i++) {
			ret = sceSystemGestureCreateTouchRecognizer(&m_touchRecognizer[indexRecognizer], m_systemGestureType[i], p, NULL, NULL);
			if (ret < SCE_OK) {
				return ret;
			}
			indexRecognizer++;
		}
	}

	//prepare buffer for touch gesture events
	m_touchGestureEventSize = (sizeof(SceSystemGestureTouchEvent) * SCE_TOUCH_MAX_REPORT * m_numOfGestureType);
	m_touchGestureEvent = (SceSystemGestureTouchEvent*)malloc(m_touchGestureEventSize);
	if (m_touchGestureEvent == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_FATAL;
	}
	memset((void*)m_touchGestureEvent, 0, m_touchGestureEventSize);

	return ret;
}

int ssii::TouchContextImpl::finalize(void)
{
	int ret = SCE_OK;

	if (m_primitiveTouchGestureEvent) {
		free(m_primitiveTouchGestureEvent);
	}
	if (m_touchRecognizer) {
		free(m_touchRecognizer);
	}
	if (m_touchGestureEvent) {
		free(m_touchGestureEvent);
	}
	for (int p=0; p<SCE_TOUCH_PORT_MAX_NUM; p++) {
		if (m_rawTouchData[p]) {
			free( m_rawTouchData[p] );
		}
		sceTouchSetSamplingState( p, SCE_TOUCH_SAMPLING_STATE_STOP );
	}

	//load libSystemGesture
	ret = sceSysmoduleUnloadModule(SCE_SYSMODULE_SYSTEM_GESTURE);
	if (ret < SCE_OK) {
		return ret;
	}

	return ret;
}

int ssii::TouchContextImpl::setFunctionFlag(const TouchPort port, const uint32_t functionFlag)
{
	int ret = SCE_OK;

	if (m_rawTouchData[port] == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NO_RAW_DATA_BUFFER;
	}

	if (m_touchRecognizer == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NOT_INITIALIZED;
	}

	if (m_currentFunctionFlag[port] == functionFlag) {
		// FunctionFlag was not changed.
		return SCE_OK;
	}

	if (m_currentFunctionFlag[port] == 0) {
		ret = sceTouchSetSamplingState(port, SCE_TOUCH_SAMPLING_STATE_START);
		if (ret < SCE_OK) {
			return ret;
		}
	}

	setFunctionFlagState(port, kTouchContextFunctionFlagRawData, functionFlag);

	if (setFunctionFlagState(port, kTouchContextFunctionFlagPrimitiveGestureEvent, functionFlag)) {
		sceSystemGestureResetPrimitiveTouchRecognizer();
	}

	uint32_t indexRecognizer = (port * m_numOfGestureType);
	for (int i = 0 ; i < m_numOfGestureType ; i++) {
		if (setFunctionFlagState( port, m_functionFlag[i], functionFlag)) {
			sceSystemGestureResetTouchRecognizer( &m_touchRecognizer[indexRecognizer] );
		}
		indexRecognizer++;
	}

	if (m_currentFunctionFlag[port] == 0) {
		//		printf("******************** <<< Sampling : STOP >>> [%s] \n",(port==TOUCH_CONTEXT_PORT_FRONT)?"FRONT":"BACK");
		ret = sceTouchSetSamplingState( port, SCE_TOUCH_SAMPLING_STATE_STOP );
		if (ret < SCE_OK) {
			return ret;
		}
	}
	
	return ret;
}

uint32_t ssii::TouchContextImpl::getFunctionFlag(const ssi::TouchPort port) const
{
	return m_currentFunctionFlag[port];
}

int ssii::TouchContextImpl::update(void)
{
	if (m_primitiveTouchGestureEvent == NULL || m_touchRecognizer == NULL || m_touchGestureEvent == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NOT_INITIALIZED;
	}

	if (m_rawTouchData[kTouchPortFront] == NULL &&  m_rawTouchData[kTouchPortBack] == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NO_RAW_DATA_BUFFER;
	}

	int ret = SCE_OK;
	int maxNumFrame = 0;

	//libTouch
	for (int p=0; p<SCE_TOUCH_PORT_MAX_NUM; p++) {
		m_numOfRawTouchData[p] = 0;

		if (m_rawTouchData[p]) {
			memset((void*)m_rawTouchData[p], 0, (sizeof(SceTouchData) * m_option.numBufs[p]) );

			if (m_currentFunctionFlag[p]) {
				if (m_option.numBufs[p] > 0) {
					m_numOfRawTouchData[p] = sceTouchRead(p, m_rawTouchData[p], m_option.numBufs[p]);
					if (m_numOfRawTouchData[p] < SCE_OK) {
						return m_numOfRawTouchData[p];
					}

					if (m_numOfRawTouchData[p] > maxNumFrame) {
						maxNumFrame = m_numOfRawTouchData[p];
					}
				}
			}
		}
	}

	//libSystemGesture

	// reset buffer
	m_numOfprimitiveGestureEvent = 0;
	memset((void*)m_primitiveTouchGestureEvent, 0, m_primitiveTouchGestureEventSize);

	m_numOfGestureEvent = 0;
	memset((void*)m_touchGestureEvent, 0, m_touchGestureEventSize);

	uint32_t numOfEvent = 0;
	for (int f=0; f<maxNumFrame; f++) {
		//update primitive touch event
		ret = sceSystemGestureUpdatePrimitiveTouchRecognizer(
			( f < m_numOfRawTouchData[SCE_TOUCH_PORT_FRONT] ) ? &m_rawTouchData[SCE_TOUCH_PORT_FRONT][f] : NULL,
			( f < m_numOfRawTouchData[SCE_TOUCH_PORT_BACK] )  ? &m_rawTouchData[SCE_TOUCH_PORT_BACK][f]  : NULL);
		if (ret < SCE_OK) {
			return ret;
		}

		//store primitive touch gesture event data
		ret = sceSystemGestureGetPrimitiveTouchEvents(&m_primitiveTouchGestureEvent[m_numOfprimitiveGestureEvent],
													  m_primitiveTouchEventBufferSize,
													  &numOfEvent);
		if (ret < SCE_OK) {
			return ret;
		}
		m_numOfprimitiveGestureEvent += numOfEvent;

		uint32_t indexRecognizer = 0;
		for (int p=0; p<SCE_TOUCH_PORT_MAX_NUM; p++) {
			for (int i=0; i<m_numOfGestureType; i++) {
				if (isFunctionFlagEnable((( p == 0 ) ? kTouchPortFront : kTouchPortBack), m_functionFlag[i])){
					//update higher gesture
					ret = sceSystemGestureUpdateTouchRecognizer(&m_touchRecognizer[indexRecognizer]);
					if (ret < SCE_OK) {
						return ret;
					}

					//store higher touch gesture event data
					ret = sceSystemGestureGetTouchEvents(&m_touchRecognizer[indexRecognizer],
														 &m_touchGestureEvent[m_numOfGestureEvent],
														 SCE_TOUCH_MAX_REPORT,
														 &numOfEvent);
					if (ret < SCE_OK) {
						return ret;
					}
					m_numOfGestureEvent += numOfEvent;
				}
				indexRecognizer++;
			}
		}
	}

	return ret;
}

//data acquired via libTouch
int ssii::TouchContextImpl::getRawData(const TouchPort port, SceTouchData* touchData, const uint32_t size) const
{
	if( touchData == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if( m_rawTouchData[port] == NULL ){
		return SCE_SAMPLE_UTIL_ERROR_NO_RAW_DATA_BUFFER;
	}

	if( ! isFunctionFlagEnable( port, kTouchContextFunctionFlagRawData ) ){
		return SCE_SAMPLE_UTIL_ERROR_FUNCTION_NOT_ENABLED;
	}

	uint32_t copySize = ( m_numOfRawTouchData[port] < size ) ? m_numOfRawTouchData[port] : size;
	if( copySize > 0 ){
		memcpy( touchData, m_rawTouchData[port], ( sizeof(SceTouchData) * copySize ) );
	}

	return copySize;
}

//data acquired via libTouch
int ssii::TouchContextImpl::getPanelInfo(const TouchPort port, SceTouchPanelInfo* panelInfo) const
{
	if (panelInfo == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	memcpy( panelInfo, &m_panelInfo[port], sizeof(SceTouchPanelInfo) );

	return SCE_OK;
}


//data acquired via libSystemGesture
int ssii::TouchContextImpl::getNumberOfPrimitiveGestureEvents() const
{
	int ret = 0;

	if (isFunctionFlagEnable( kTouchPortFront, kTouchContextFunctionFlagPrimitiveGestureEvent) ||
		isFunctionFlagEnable( kTouchPortBack , kTouchContextFunctionFlagPrimitiveGestureEvent)) {
			ret = m_numOfprimitiveGestureEvent;
	} else {
		ret = SCE_SAMPLE_UTIL_ERROR_FUNCTION_NOT_ENABLED;
	}

	return ret;
}

int ssii::TouchContextImpl::getPrimitiveGestureEvents(TouchPrimitiveGestureEvent* events, const uint32_t size) const
{
	if (events == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if (m_primitiveTouchGestureEvent == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NOT_INITIALIZED;
	}

	if (isFunctionFlagEnable( kTouchPortFront, kTouchContextFunctionFlagPrimitiveGestureEvent) ||
		isFunctionFlagEnable( kTouchPortBack , kTouchContextFunctionFlagPrimitiveGestureEvent)) {

			uint32_t copySize = ( m_numOfprimitiveGestureEvent < size ) ? m_numOfprimitiveGestureEvent : size;
			for (int i = 0 ; i < copySize ; i++) {
				events[i].port = ( m_primitiveTouchGestureEvent[i].primitiveID & 0xFF00 ) ? kTouchPortBack : kTouchPortFront;
				memcpy( &events[i].event, &m_primitiveTouchGestureEvent[i], sizeof(SceSystemGesturePrimitiveTouchEvent) );
			}

	} else {
		return SCE_SAMPLE_UTIL_ERROR_FUNCTION_NOT_ENABLED;
	}

	return SCE_OK;
}

int ssii::TouchContextImpl::getNumberOfGestureEvents() const
{
	int ret = 0;

	uint32_t eventType =  ( kTouchContextFunctionFlagTapGestureEvent |
		kTouchContextFunctionFlagDragGestureEvent |
		kTouchContextFunctionFlagTapAndHoldGestureEvent |
		kTouchContextFunctionFlagPinchOutInGestureEvent |
		kTouchContextFunctionFlagRotationGestureEvent );
	if (isFunctionFlagEnable( kTouchPortFront, eventType) ||
		isFunctionFlagEnable( kTouchPortBack , eventType)) {
			ret = m_numOfGestureEvent;
	} else {
		ret = SCE_SAMPLE_UTIL_ERROR_FUNCTION_NOT_ENABLED;
	}

	return ret;
}

int ssii::TouchContextImpl::getGestureEvents(TouchGestureEvent* events, const uint32_t size) const
{
	if (events == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
	}

	if (m_touchGestureEvent == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NOT_INITIALIZED;
	}

	uint32_t eventType =  ( kTouchContextFunctionFlagTapGestureEvent |
		kTouchContextFunctionFlagDragGestureEvent |
		kTouchContextFunctionFlagTapAndHoldGestureEvent |
		kTouchContextFunctionFlagPinchOutInGestureEvent |
		kTouchContextFunctionFlagRotationGestureEvent );

	if (isFunctionFlagEnable( kTouchPortFront, eventType) ||
		isFunctionFlagEnable( kTouchPortBack , eventType)) {

			uint32_t copySize = ( m_numOfGestureEvent < size ) ? m_numOfGestureEvent : size;
			for (int i = 0 ; i < copySize ; i++) {
				events[i].port = ( m_touchGestureEvent[i].eventID & 0x0000FF00 ) ? kTouchPortBack : kTouchPortFront;
				memcpy( &events[i].event, &m_touchGestureEvent[i], sizeof(SceSystemGestureTouchEvent) );
			}
			return copySize;
	} else {
		return SCE_SAMPLE_UTIL_ERROR_FUNCTION_NOT_ENABLED;
	}
}

bool ssii::TouchContextImpl::setFunctionFlagState(const TouchPort port, const uint32_t type, const uint32_t flag)
{
	bool ret = ( (m_currentFunctionFlag[port] & type) != (flag & type) ) ? true : false;
	if (ret) {
		m_currentFunctionFlag[port] ^= type;
	}
	return ret;
}

bool ssii::TouchContextImpl::isFunctionFlagEnable(const TouchPort port, const uint32_t type) const
{
	return (m_currentFunctionFlag[port] & type) ? true : false;
}

int ssi::createTouchContext(ssi::TouchContext **outTouchContext, const ssi::TouchContextOption* option)
{
	if (outTouchContext == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssii::TouchContextImpl *t = new ssii::TouchContextImpl;
	int ret = t->initialize(option);
	if (ret != SCE_OK) {
		delete t;
		return ret;
	}
	*outTouchContext = t;
	return ret;
}

#endif // _SCE_TARGET_OS_PSP2
