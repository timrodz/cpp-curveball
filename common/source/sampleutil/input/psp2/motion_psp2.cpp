/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#include "stdafx.h"

#include <sampleutil/input/motion.h>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil_internal.h>

#include <scebase_common.h>
#if _SCE_TARGET_OS_PSP2

#include <sampleutil/sampleutil_common.h>

#pragma comment(lib, "SceMotion_stub")

namespace sce
{
	namespace SampleUtil
	{
		namespace Input
		{
			namespace Impl
			{
				class MotionContextImpl : public MotionContext, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					MotionContextImpl(void){}
					virtual ~MotionContextImpl(void)
					{
						finalize();
					}

					//This executes the initialization processing of the MotionContext class.
					int initialize(void);
					//This executes termination processing of the TouchContext class.
					int finalize(void);
					//This updates the MotionContext class.
					int update(void);

					//Make enable/disable settings of each MotionContext functionality.
					int enableFunctions(uint32_t functionFlag);

					//Get the current motion context function flag setting value.
					uint32_t getEnabledFunctions(void) const;

					//Get the current SceMotionState.
					int getMotionState(SceMotionState* motionState) const;
					//Get the current MotionContextData.
					int getMotionContextData(MotionContextData* motionContextData) const;

					//Resets the device orientation.
					int resetReferenceOrientation(void);

					//Rotate around the yaw axis.
					int rotateReferenceOrientationYaw(float radians);

					//Sets the angle threshold.
					int setUserInterfaceOrientationThreshold(float radians);

					//Sets tilt correction.
					void enableAccelerometerTiltCorrection(bool enable);

					//Sets deadbanding of gyro.
					void enableGyroDeadband(bool enable);

					//Sets the gyro bias correction.
					void enableGyroBiasCorrection(bool enable);

					std::string getResourceType(void) const
					{
						return "Input::MotionContext";
					}

				private:
					uint32_t m_currentMotionFunctionFlag;
					MotionContextData* m_motionContextData;
					SceMotionState* m_motionState;
					float m_radiansThreshold;
				};
			}
		}
	}
}


namespace ss = sce::SampleUtil;
namespace ssi = sce::SampleUtil::Input;
namespace ssii = sce::SampleUtil::Input::Impl;
using namespace sce::Vectormath::Simd::Aos;

int ssii::MotionContextImpl::initialize()
{
	int ret = SCE_OK;

	m_currentMotionFunctionFlag = 0;
	m_radiansThreshold = sceMotionGetAngleThreshold();

	m_motionContextData = (MotionContextData*)malloc(sizeof(MotionContextData));
	m_motionState = (SceMotionState*)malloc(sizeof(SceMotionState));
	return ret;
}

int ssii::MotionContextImpl::finalize()
{
	int ret = SCE_OK;

	if (m_currentMotionFunctionFlag == 0) {
		return ret;
	}

	if (m_currentMotionFunctionFlag & kMotionContextFunctionTypeMagnetometer) {
		ret = sceMotionMagnetometerOff();
		SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionMagnetometerOff() failed with: 0x%x\n", ret);
	}

	if (m_currentMotionFunctionFlag & kMotionContextFunctionTypeAccelerometerAndGyro) {
		ret = sceMotionStopSampling();
		SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionStopSampling() failed with: 0x%x %d %x\n", ret,ret,SCE_OK);
	}

	m_currentMotionFunctionFlag = 0;

	free(m_motionContextData);
	free(m_motionState);

	return ret;
}

int ssii::MotionContextImpl::update()
{
	int ret = SCE_OK;

	if (m_currentMotionFunctionFlag & kMotionContextFunctionTypeAccelerometerAndGyro) {
		SceMotionState motionState;
		ret = sceMotionGetState(&motionState);
		if (ret != SCE_OK) {
			return ret;
		}

		m_motionState = &motionState;

		m_motionContextData->deviceTimestamp = motionState.timestamp;
		m_motionContextData->systemTimestamp = motionState.hostTimestamp;
		m_motionContextData->relativeOrientation.setW( motionState.deviceQuat.w );
		m_motionContextData->relativeOrientation.setX( motionState.deviceQuat.x );
		m_motionContextData->relativeOrientation.setY( motionState.deviceQuat.y );
		m_motionContextData->relativeOrientation.setZ( motionState.deviceQuat.z );
		m_motionContextData->linearAcceleration.setX( motionState.acceleration.x );
		m_motionContextData->linearAcceleration.setY( motionState.acceleration.y );
		m_motionContextData->linearAcceleration.setZ( motionState.acceleration.z );
		m_motionContextData->angularVelocity.setX( motionState.angularVelocity.x );
		m_motionContextData->angularVelocity.setY( motionState.angularVelocity.y );
		m_motionContextData->angularVelocity.setZ( motionState.angularVelocity.z );
		m_motionContextData->userInterfaceOrientation.setX( motionState.basicOrientation.x );
		m_motionContextData->userInterfaceOrientation.setY( motionState.basicOrientation.y );
		m_motionContextData->userInterfaceOrientation.setZ( motionState.basicOrientation.z );

		if (m_currentMotionFunctionFlag & kMotionContextFunctionTypeMagnetometer) {
			for (int i=0; i<4; i++) {
				for (int j=0; j<4; j++) {
					m_motionContextData->northEastDownOrientation.setElem( i, j, motionState.nedMatrix.f[i][j] );
				}
			}
			m_motionContextData->magneticFieldStability = motionState.magnFieldStability;
		} else {
			memset(&m_motionState->nedMatrix, 0, sizeof(SceUMatrix4));
			m_motionState->magnFieldStability = 0;

			m_motionContextData->northEastDownOrientation = Matrix4::zero();
			m_motionContextData->magneticFieldStability = 0;
		}
	} else {
		memset(m_motionState, 0, sizeof(SceMotionState));
		memset(m_motionContextData, 0, sizeof(MotionContextData));
	}

	m_radiansThreshold = sceMotionGetAngleThreshold();

	return ret;
}

int ssii::MotionContextImpl::enableFunctions(uint32_t functionFlag)
{
	int ret;

	if (functionFlag == m_currentMotionFunctionFlag) {
		return SCE_OK;
	} else if (functionFlag == kMotionContextFunctionTypeMagnetometer) {
		return SCE_SAMPLE_UTIL_ERROR_NOT_ACTIVE;
	} else {
		uint32_t diff = functionFlag ^ m_currentMotionFunctionFlag;
		switch (diff) {
		case kMotionContextFunctionTypeAccelerometerAndGyro|kMotionContextFunctionTypeMagnetometer:
			{
				if (functionFlag & diff) {
					ret = sceMotionStartSampling();
					SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionStartSampling() failed with: 0x%x %d %x\n", ret, ret, SCE_OK);
					ret = sceMotionMagnetometerOn();
					SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionMagnetometerOn() failed with: 0x%x\n", ret);
					break;
				} else {
					ret = sceMotionMagnetometerOff();
					SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionMagnetometerOff() failed with: 0x%x\n", ret);
					ret = sceMotionStopSampling();
					SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionStopSampling() failed with: 0x%x %d %x\n", ret, ret, SCE_OK);
					break;
				}
			}
		case kMotionContextFunctionTypeMagnetometer:
			{
				if (functionFlag & diff) {
					ret = sceMotionMagnetometerOn();
					SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionMagnetometerOn() failed with: 0x%x\n", ret);
					break;
				} else {
					ret = sceMotionMagnetometerOff();
					SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionMagnetometerOff() failed with: 0x%x\n", ret);
					break;
				}
			}
		case kMotionContextFunctionTypeAccelerometerAndGyro:
			{
				if (functionFlag & diff) {
					ret = sceMotionStartSampling();
					SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionStartSampling() failed with: 0x%x %d %x\n", ret, ret, SCE_OK);
					break;
				} else {
					ret = sceMotionStopSampling();
					SCE_SAMPLE_UTIL_ASSERT_MSG(ret == SCE_OK, "sceMotionStopSampling() failed with: 0x%x %d %x\n", ret, ret, SCE_OK);
					break;
				}
			}
		default:
			return SCE_SAMPLE_UTIL_ERROR_INVALID_PARAM;
		}
		m_currentMotionFunctionFlag = functionFlag;
		return ret;
	}
}

uint32_t ssii::MotionContextImpl::getEnabledFunctions() const
{
	return m_currentMotionFunctionFlag;
}

int ssii::MotionContextImpl::getMotionState(SceMotionState* motionState) const
{
	if (motionState == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	memcpy(motionState,m_motionState,sizeof(SceMotionState));

	return SCE_OK;
}

int ssii::MotionContextImpl::getMotionContextData(MotionContextData* motionContextData) const
{
	if (motionContextData == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	memcpy(motionContextData,m_motionContextData,sizeof(MotionContextData));

	return SCE_OK;
}

int ssii::MotionContextImpl::resetReferenceOrientation()
{
	return sceMotionReset();
}

int ssii::MotionContextImpl::rotateReferenceOrientationYaw(float radians)
{
	return sceMotionRotateYaw(radians);
}

int ssii::MotionContextImpl::setUserInterfaceOrientationThreshold(float radians)
{
	return sceMotionSetAngleThreshold(radians);
}

void ssii::MotionContextImpl::enableAccelerometerTiltCorrection(bool enable)
{
	sceMotionSetTiltCorrection(enable);
}

void ssii::MotionContextImpl::enableGyroDeadband(bool enable)
{
	sceMotionSetDeadband(enable);
}

void ssii::MotionContextImpl::enableGyroBiasCorrection(bool enable)
{
	sceMotionSetGyroBiasCorrection(enable);
}


int ssi::createMotionContext(ssi::MotionContext **outMotionContext)
{
	if (outMotionContext == NULL) {
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	ssii::MotionContextImpl *t = new ssii::MotionContextImpl;
	int ret = t->initialize();
	if (ret != SCE_OK) {
		delete t;
		return ret;
	}
	*outMotionContext = t;
	return ret;
}

#endif /* _SCE_TARGET_OS_PSP2 */
