/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2013 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/

#ifndef _SAMPLE_UTILITIES_PAD_COMMON_H
#define _SAMPLE_UTILITIES_PAD_COMMON_H

#include <sampleutil/input/controller.h>
#include <sampleutil/sampleutil_error.h>
#include <sampleutil_internal.h>
#include <sampleutil/input.h>

// for some const value or definition
#include <sampleutil/input/controller.h>

/* ------------------------------------------------------------------------- */
namespace ssi  = sce::SampleUtil::Input;

/* ------------------------------------------------------------------------- */
namespace sce
{
	namespace SampleUtil
	{
		namespace Input
		{
			namespace Impl
			{
				class PadContextImpl : public PadContext, public sce::SampleUtil::Impl::ResourceImpl
				{
				public:
					PadContextImpl(void);
					~PadContextImpl(void);

					int getData(PadData *data, uint32_t length) const;
					int getPreviousUpdateData(PadData *data, uint32_t length) const;
					int getPressedButtons(Button *pressedButtons) const;
					int getReleasedButtons(Button *releasedButtons) const;
					
					/* ------------ Controller Methods ------------ */
					bool isButtonDown(uint32_t buttons, ButtonEventPattern pattern=kButtonEventPatternAll) const;
					bool isButtonUp(uint32_t buttons, ButtonEventPattern pattern=kButtonEventPatternAll) const;
					bool isButtonPressed(uint32_t buttons, ButtonEventPattern pattern=kButtonEventPatternAll) const;
					bool isButtonReleased(uint32_t buttons, ButtonEventPattern pattern=kButtonEventPatternAll) const;

					const sce::Vectormath::Simd::Aos::Vector2& getLeftStick() const;
					const sce::Vectormath::Simd::Aos::Vector2& getRightStick() const;

					void setRightAnalogStickDeadZone(float rightAnalogStickDeadZone);
					void setLeftAnalogStickDeadZone(float leftAnalogStickDeadZone);

					float getDefaultLeftAnalogStickDeadZone(void);
					float getDefaultRightAnalogStickDeadZone(void);

					/* ------------ Pad System Methods ------------ */
					System::UserId getUserId(void);
					int32_t getType(void);
					int32_t getIndex(void);

					virtual std::string getResourceType(void) const {
						return "Input::PadContext";
					}

				protected:
					float m_rightAnalogStickDeadZone;
					float m_leftAnalogStickDeadZone;
					float m_defaultRightAnalogStickDeadZone;
					float m_defaultLeftAnalogStickDeadZone;

					ssi::PadData *m_currentPadData;
					ssi::PadData *m_previousPadData;
					int m_numPadData;
					int m_numCurrentPadData;
					int m_numPreviousPadData;
					uint32_t m_pressedButtonData;
					uint32_t m_releasedButtonData;

					sce::Vectormath::Simd::Aos::Vector2 m_leftStickValue;
					sce::Vectormath::Simd::Aos::Vector2 m_rightStickValue;

					static const float m_recipMaxByteAsFloat;

					System::UserId			m_userId;
					int32_t					m_type;
					int32_t					m_index;

#if _SCE_TARGET_OS_WINDOWS || _SCE_TARGET_OS_PSP2
#if !(defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS)
				public:
					int initialize(const System::UserId userId, const int32_t type, const int32_t index, const PadContextOption* pOption=NULL);
					int finalize(void);

					int update(void);
					
				private:
					virtual void updateController(ssi::PadData *data, uint32_t numPadData) = 0;
					virtual void updateTouchDevice(ssi::PadData *data, uint32_t numPadData) = 0;
					virtual void updateMotion(ssi::PadData *data, uint32_t numPadData) = 0;

					virtual int initializeController(const PadContextOption* pOption) = 0;
					virtual int initializeTouchDevice(const PadContextOption* pOption) = 0;
					virtual int initializeMotion(const PadContextOption* pOption) = 0;

					virtual int finalizeController(void) = 0;
					virtual int finalizeTouchDevice(void) = 0;
					virtual int finalizeMotion(void) = 0;

#endif/* !(defined(_SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) && _SCE_SAMPLE_UTIL_ENABLE_PAD_LIBRARY_FOR_WINDOWS) */
#endif /*_SCE_TARGET_OS_WINDOWS || _SCE_TARGET_OS_PSP2*/

				};

				int createOrbisPadContext(sce::SampleUtil::Input::PadContext **outPadContext,
										 const System::UserId userId,
										 const int32_t type,
										 const int32_t index,
										 const PadContextOption* option);

				int createVitaPadContext(sce::SampleUtil::Input::PadContext **outPadContext,
										 const System::UserId userId,
										 const int32_t type,
										 const int32_t index,
										 const PadContextOption* option);

				int createWindowsPadContext(sce::SampleUtil::Input::PadContext **outPadContext,
										  const System::UserId userId,
										  const int32_t type,
										  const int32_t index,
										  const PadContextOption* option);
			}
		}
	}
}

#endif /*_SAMPLE_UTILITIES_PAD_COMMON_H*/

