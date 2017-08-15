/* SIE CONFIDENTIAL
* PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc. 
* All Rights Reserved.
*/


#include "stdafx.h"

#include <sampleutil/graphics.h>
#include "../graphics_internal.h"
#include "util/util.h"

#if _SCE_TARGET_OS_PSP2
namespace ssg = sce::SampleUtil::Graphics;
namespace ssgi = sce::SampleUtil::Graphics::Impl;
namespace ssiu = sce::SampleUtil::Internal::Util;

static SceGxmTextureFormat presumeTextureFormat(SceGxmColorFormat format)
{
	switch(format)
	{

	case SCE_GXM_COLOR_FORMAT_U8U8U8U8_ABGR		:// SCE_GXM_COLOR_BASE_FORMAT_U8U8U8U8 | SCE_GXM_COLOR_SWIZZLE4_ABGR,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8U8 | SCE_GXM_TEXTURE_SWIZZLE4_ABGR); }
	case SCE_GXM_COLOR_FORMAT_U8U8U8U8_ARGB		:// SCE_GXM_COLOR_BASE_FORMAT_U8U8U8U8 | SCE_GXM_COLOR_SWIZZLE4_ARGB,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8U8 | SCE_GXM_TEXTURE_SWIZZLE4_ARGB); }
	case SCE_GXM_COLOR_FORMAT_U8U8U8U8_RGBA		:// SCE_GXM_COLOR_BASE_FORMAT_U8U8U8U8 | SCE_GXM_COLOR_SWIZZLE4_RGBA,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8U8 | SCE_GXM_TEXTURE_SWIZZLE4_RGBA); }
	case SCE_GXM_COLOR_FORMAT_U8U8U8U8_BGRA		:// SCE_GXM_COLOR_BASE_FORMAT_U8U8U8U8 | SCE_GXM_COLOR_SWIZZLE4_BGRA,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8U8 | SCE_GXM_TEXTURE_SWIZZLE4_BGRA); }
	case SCE_GXM_COLOR_FORMAT_U8U8U8_BGR			:// SCE_GXM_COLOR_BASE_FORMAT_U8U8U8 | SCE_GXM_COLOR_SWIZZLE3_BGR,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8 | SCE_GXM_TEXTURE_SWIZZLE3_BGR); }
	case SCE_GXM_COLOR_FORMAT_U8U8U8_RGB			:// SCE_GXM_COLOR_BASE_FORMAT_U8U8U8 | SCE_GXM_COLOR_SWIZZLE3_RGB,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8U8 | SCE_GXM_TEXTURE_SWIZZLE3_RGB); }
	case SCE_GXM_COLOR_FORMAT_U5U6U5_BGR			:// SCE_GXM_COLOR_BASE_FORMAT_U5U6U5 | SCE_GXM_COLOR_SWIZZLE3_BGR,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U5U6U5 | SCE_GXM_TEXTURE_SWIZZLE3_BGR); }
	case SCE_GXM_COLOR_FORMAT_U5U6U5_RGB			:// SCE_GXM_COLOR_BASE_FORMAT_U5U6U5 | SCE_GXM_COLOR_SWIZZLE3_RGB,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U5U6U5 | SCE_GXM_TEXTURE_SWIZZLE3_RGB); }
	case SCE_GXM_COLOR_FORMAT_U1U5U5U5_ABGR		:// SCE_GXM_COLOR_BASE_FORMAT_U1U5U5U5 | SCE_GXM_COLOR_SWIZZLE4_ABGR,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U1U5U5U5 | SCE_GXM_TEXTURE_SWIZZLE4_ABGR); }
	case SCE_GXM_COLOR_FORMAT_U1U5U5U5_ARGB		:// SCE_GXM_COLOR_BASE_FORMAT_U1U5U5U5 | SCE_GXM_COLOR_SWIZZLE4_ARGB,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U1U5U5U5 | SCE_GXM_TEXTURE_SWIZZLE4_ARGB); }
	case SCE_GXM_COLOR_FORMAT_U5U5U5U1_RGBA		:// SCE_GXM_COLOR_BASE_FORMAT_U1U5U5U5 | SCE_GXM_COLOR_SWIZZLE4_RGBA,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U1U5U5U5 | SCE_GXM_TEXTURE_SWIZZLE4_RGBA); }
	case SCE_GXM_COLOR_FORMAT_U5U5U5U1_BGRA		:// SCE_GXM_COLOR_BASE_FORMAT_U1U5U5U5 | SCE_GXM_COLOR_SWIZZLE4_BGRA,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U1U5U5U5 | SCE_GXM_TEXTURE_SWIZZLE4_BGRA); }

	case SCE_GXM_COLOR_FORMAT_U4U4U4U4_ABGR		:// SCE_GXM_COLOR_BASE_FORMAT_U4U4U4U4 | SCE_GXM_COLOR_SWIZZLE4_ABGR,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U4U4U4U4 | SCE_GXM_TEXTURE_SWIZZLE4_ABGR); }
	case SCE_GXM_COLOR_FORMAT_U4U4U4U4_ARGB		:// SCE_GXM_COLOR_BASE_FORMAT_U4U4U4U4 | SCE_GXM_COLOR_SWIZZLE4_ARGB,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U4U4U4U4 | SCE_GXM_TEXTURE_SWIZZLE4_ARGB); }
	case SCE_GXM_COLOR_FORMAT_U4U4U4U4_RGBA		:// SCE_GXM_COLOR_BASE_FORMAT_U4U4U4U4 | SCE_GXM_COLOR_SWIZZLE4_RGBA,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U4U4U4U4 | SCE_GXM_TEXTURE_SWIZZLE4_RGBA); }
	case SCE_GXM_COLOR_FORMAT_U4U4U4U4_BGRA		:// SCE_GXM_COLOR_BASE_FORMAT_U4U4U4U4 | SCE_GXM_COLOR_SWIZZLE4_BGRA,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U4U4U4U4 | SCE_GXM_TEXTURE_SWIZZLE4_BGRA); }
		
	case SCE_GXM_COLOR_FORMAT_U8U3U3U2_ARGB		:// SCE_GXM_COLOR_BASE_FORMAT_U8U3U3U2,				
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U3U3U2); }	

	case SCE_GXM_COLOR_FORMAT_F16_R				:// SCE_GXM_COLOR_BASE_FORMAT_F16 | SCE_GXM_COLOR_SWIZZLE1_R,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F16 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	
	case SCE_GXM_COLOR_FORMAT_F16_G				:// SCE_GXM_COLOR_BASE_FORMAT_F16 | SCE_GXM_COLOR_SWIZZLE1_G,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F16 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	

	case SCE_GXM_COLOR_FORMAT_F16F16_GR			:// SCE_GXM_COLOR_BASE_FORMAT_F16F16 | SCE_GXM_COLOR_SWIZZLE2_GR,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F16F16 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	
	case SCE_GXM_COLOR_FORMAT_F16F16_RG			:// SCE_GXM_COLOR_BASE_FORMAT_F16F16 | SCE_GXM_COLOR_SWIZZLE2_RG,	
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F16F16 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	

	case SCE_GXM_COLOR_FORMAT_F32_R				:// SCE_GXM_COLOR_BASE_FORMAT_F32 | SCE_GXM_COLOR_SWIZZLE1_R,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F32 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	
	case SCE_GXM_COLOR_FORMAT_S16_R				:// SCE_GXM_COLOR_BASE_FORMAT_S16 | SCE_GXM_COLOR_SWIZZLE1_R,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S16 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	
	case SCE_GXM_COLOR_FORMAT_S16_G				:// SCE_GXM_COLOR_BASE_FORMAT_S16 | SCE_GXM_COLOR_SWIZZLE1_G,		
		{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S16 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	

	case SCE_GXM_COLOR_FORMAT_S16S16_GR			:// SCE_GXM_COLOR_BASE_FORMAT_S16S16 | SCE_GXM_COLOR_SWIZZLE2_GR,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S16S16 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	
	case SCE_GXM_COLOR_FORMAT_S16S16_RG			:// SCE_GXM_COLOR_BASE_FORMAT_S16S16 | SCE_GXM_COLOR_SWIZZLE2_RG,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S16S16 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	

	case SCE_GXM_COLOR_FORMAT_U16_R				:// SCE_GXM_COLOR_BASE_FORMAT_U16 | SCE_GXM_COLOR_SWIZZLE1_R,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U16 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	
	case SCE_GXM_COLOR_FORMAT_U16_G				:// SCE_GXM_COLOR_BASE_FORMAT_U16 | SCE_GXM_COLOR_SWIZZLE1_G,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U16 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	

	case SCE_GXM_COLOR_FORMAT_U16U16_GR			:// SCE_GXM_COLOR_BASE_FORMAT_U16U16 | SCE_GXM_COLOR_SWIZZLE2_GR,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U16U16 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	
	case SCE_GXM_COLOR_FORMAT_U16U16_RG			:// SCE_GXM_COLOR_BASE_FORMAT_U16U16 | SCE_GXM_COLOR_SWIZZLE2_RG,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U16U16 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	

	case SCE_GXM_COLOR_FORMAT_U2U10U10U10_ABGR	:// SCE_GXM_COLOR_BASE_FORMAT_U2U10U10U10	| SCE_GXM_COLOR_SWIZZLE4_ABGR,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U2U10U10U10 | SCE_GXM_TEXTURE_SWIZZLE4_ABGR); }	
	case SCE_GXM_COLOR_FORMAT_U2U10U10U10_ARGB	:// SCE_GXM_COLOR_BASE_FORMAT_U2U10U10U10	| SCE_GXM_COLOR_SWIZZLE4_ARGB,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U2U10U10U10 | SCE_GXM_TEXTURE_SWIZZLE4_ARGB); }	
	case SCE_GXM_COLOR_FORMAT_U10U10U10U2_RGBA	:// SCE_GXM_COLOR_BASE_FORMAT_U2U10U10U10	| SCE_GXM_COLOR_SWIZZLE4_RGBA,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U2U10U10U10 | SCE_GXM_TEXTURE_SWIZZLE4_RGBA); }	
	case SCE_GXM_COLOR_FORMAT_U10U10U10U2_BGRA	:// SCE_GXM_COLOR_BASE_FORMAT_U2U10U10U10	| SCE_GXM_COLOR_SWIZZLE4_BGRA,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U2U10U10U10 | SCE_GXM_TEXTURE_SWIZZLE4_BGRA); }	

	case SCE_GXM_COLOR_FORMAT_U8_R				:// SCE_GXM_COLOR_BASE_FORMAT_U8 | SCE_GXM_COLOR_SWIZZLE1_R,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	
	case SCE_GXM_COLOR_FORMAT_U8_A				:// SCE_GXM_COLOR_BASE_FORMAT_U8 | SCE_GXM_COLOR_SWIZZLE1_A,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	

	case SCE_GXM_COLOR_FORMAT_S8_R				:// SCE_GXM_COLOR_BASE_FORMAT_S8 | SCE_GXM_COLOR_SWIZZLE1_R,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	
	case SCE_GXM_COLOR_FORMAT_S8_A				:// SCE_GXM_COLOR_BASE_FORMAT_S8 | SCE_GXM_COLOR_SWIZZLE1_A,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8 | SCE_GXM_TEXTURE_SWIZZLE1_R); }	

	case SCE_GXM_COLOR_FORMAT_U6S5S5_BGR			:// SCE_GXM_COLOR_BASE_FORMAT_S5S5U6 | SCE_GXM_COLOR_SWIZZLE3_BGR,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S5S5U6 | SCE_GXM_TEXTURE_SWIZZLE3_BGR); }	
	case SCE_GXM_COLOR_FORMAT_S5S5U6_RGB			:// SCE_GXM_COLOR_BASE_FORMAT_S5S5U6 | SCE_GXM_COLOR_SWIZZLE3_RGB,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S5S5U6 | SCE_GXM_TEXTURE_SWIZZLE3_RGB); }	

	case SCE_GXM_COLOR_FORMAT_U8U8_GR			:// SCE_GXM_COLOR_BASE_FORMAT_U8U8 | SCE_GXM_COLOR_SWIZZLE2_GR,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	
	case SCE_GXM_COLOR_FORMAT_U8U8_RG			:// SCE_GXM_COLOR_BASE_FORMAT_U8U8 | SCE_GXM_COLOR_SWIZZLE2_RG,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	
	case SCE_GXM_COLOR_FORMAT_U8U8_RA			:// SCE_GXM_COLOR_BASE_FORMAT_U8U8 | SCE_GXM_COLOR_SWIZZLE2_RA,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	
	case SCE_GXM_COLOR_FORMAT_U8U8_AR			:// SCE_GXM_COLOR_BASE_FORMAT_U8U8 | SCE_GXM_COLOR_SWIZZLE2_AR,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U8U8 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	

	case SCE_GXM_COLOR_FORMAT_S8S8_GR			:// SCE_GXM_COLOR_BASE_FORMAT_S8S8 | SCE_GXM_COLOR_SWIZZLE2_GR,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8S8 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	
	case SCE_GXM_COLOR_FORMAT_S8S8_RG			:// SCE_GXM_COLOR_BASE_FORMAT_S8S8 | SCE_GXM_COLOR_SWIZZLE2_RG,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8S8 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	
	case SCE_GXM_COLOR_FORMAT_S8S8_RA			:// SCE_GXM_COLOR_BASE_FORMAT_S8S8 | SCE_GXM_COLOR_SWIZZLE2_RA,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8S8 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	
	case SCE_GXM_COLOR_FORMAT_S8S8_AR			:// SCE_GXM_COLOR_BASE_FORMAT_S8S8 | SCE_GXM_COLOR_SWIZZLE2_AR,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8S8 | SCE_GXM_TEXTURE_SWIZZLE2_GR); }	

	case SCE_GXM_COLOR_FORMAT_S8S8S8S8_ABGR		:// SCE_GXM_COLOR_BASE_FORMAT_S8S8S8S8 | SCE_GXM_COLOR_SWIZZLE4_ABGR,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8S8 | SCE_GXM_TEXTURE_SWIZZLE4_ABGR); }	
	case SCE_GXM_COLOR_FORMAT_S8S8S8S8_ARGB		:// SCE_GXM_COLOR_BASE_FORMAT_S8S8S8S8 | SCE_GXM_COLOR_SWIZZLE4_ARGB,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8S8 | SCE_GXM_TEXTURE_SWIZZLE4_ARGB); }	
	case SCE_GXM_COLOR_FORMAT_S8S8S8S8_RGBA		:// SCE_GXM_COLOR_BASE_FORMAT_S8S8S8S8 | SCE_GXM_COLOR_SWIZZLE4_RGBA,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8S8 | SCE_GXM_TEXTURE_SWIZZLE4_RGBA); }	
	case SCE_GXM_COLOR_FORMAT_S8S8S8S8_BGRA		:// SCE_GXM_COLOR_BASE_FORMAT_S8S8S8S8 | SCE_GXM_COLOR_SWIZZLE4_BGRA,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_S8S8S8S8 | SCE_GXM_TEXTURE_SWIZZLE4_BGRA); }	

	case SCE_GXM_COLOR_FORMAT_F16F16F16F16_ABGR	:// SCE_GXM_COLOR_BASE_FORMAT_F16F16F16F16 | SCE_GXM_COLOR_SWIZZLE4_ABGR,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F16F16F16F16 | SCE_GXM_TEXTURE_SWIZZLE4_ABGR); }	
	case SCE_GXM_COLOR_FORMAT_F16F16F16F16_ARGB	:// SCE_GXM_COLOR_BASE_FORMAT_F16F16F16F16 | SCE_GXM_COLOR_SWIZZLE4_ARGB,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F16F16F16F16 | SCE_GXM_TEXTURE_SWIZZLE4_ARGB); }	
	case SCE_GXM_COLOR_FORMAT_F16F16F16F16_RGBA	:// SCE_GXM_COLOR_BASE_FORMAT_F16F16F16F16 | SCE_GXM_COLOR_SWIZZLE4_RGBA,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F16F16F16F16 | SCE_GXM_TEXTURE_SWIZZLE4_RGBA); }	
	case SCE_GXM_COLOR_FORMAT_F16F16F16F16_BGRA	:// SCE_GXM_COLOR_BASE_FORMAT_F16F16F16F16 | SCE_GXM_COLOR_SWIZZLE4_BGRA,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F16F16F16F16 | SCE_GXM_TEXTURE_SWIZZLE4_BGRA); }	

	case SCE_GXM_COLOR_FORMAT_F32F32_GR			:// SCE_GXM_COLOR_BASE_FORMAT_F32F32 | SCE_GXM_COLOR_SWIZZLE2_GR,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F32F32 | SCE_GXM_TEXTURE_SWIZZLE2_GR); 	}
	case SCE_GXM_COLOR_FORMAT_F32F32_RG			:// SCE_GXM_COLOR_BASE_FORMAT_F32F32 | SCE_GXM_COLOR_SWIZZLE2_RG,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F32F32 | SCE_GXM_TEXTURE_SWIZZLE2_GR); 	}

	case SCE_GXM_COLOR_FORMAT_F10F11F11_BGR		:// SCE_GXM_COLOR_BASE_FORMAT_F11F11F10 | SCE_GXM_COLOR_SWIZZLE3_BGR,		
		//{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F10F11F11 | SCE_GXM_TEXTURE_SWIZZLE3_BGR); 	}
	case SCE_GXM_COLOR_FORMAT_F11F11F10_RGB		:// SCE_GXM_COLOR_BASE_FORMAT_F11F11F10 | SCE_GXM_COLOR_SWIZZLE3_RGB,		
		//{ return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_F10F11F11 | SCE_GXM_TEXTURE_SWIZZLE3_RGB); 	}

	case SCE_GXM_COLOR_FORMAT_SE5M9M9M9_BGR		:// SCE_GXM_COLOR_BASE_FORMAT_SE5M9M9M9 | SCE_GXM_COLOR_SWIZZLE3_BGR,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_SE5M9M9M9 | SCE_GXM_TEXTURE_SWIZZLE3_BGR); 	}
	case SCE_GXM_COLOR_FORMAT_SE5M9M9M9_RGB		:// SCE_GXM_COLOR_BASE_FORMAT_SE5M9M9M9 | SCE_GXM_COLOR_SWIZZLE3_RGB,		
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_SE5M9M9M9 | SCE_GXM_TEXTURE_SWIZZLE3_RGB); 	}

	case SCE_GXM_COLOR_FORMAT_U2F10F10F10_ABGR	:// SCE_GXM_COLOR_BASE_FORMAT_U2F10F10F10	| SCE_GXM_COLOR_SWIZZLE4_ABGR,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U2F10F10F10 | SCE_GXM_TEXTURE_SWIZZLE4_ABGR); 	}
	case SCE_GXM_COLOR_FORMAT_U2F10F10F10_ARGB	:// SCE_GXM_COLOR_BASE_FORMAT_U2F10F10F10	| SCE_GXM_COLOR_SWIZZLE4_ARGB,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U2F10F10F10 | SCE_GXM_TEXTURE_SWIZZLE4_ARGB); 	}
	case SCE_GXM_COLOR_FORMAT_F10F10F10U2_RGBA	:// SCE_GXM_COLOR_BASE_FORMAT_U2F10F10F10	| SCE_GXM_COLOR_SWIZZLE4_RGBA,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U2F10F10F10 | SCE_GXM_TEXTURE_SWIZZLE4_RGBA); 	}
	case SCE_GXM_COLOR_FORMAT_F10F10F10U2_BGRA	:// SCE_GXM_COLOR_BASE_FORMAT_U2F10F10F10	| SCE_GXM_COLOR_SWIZZLE4_BGRA,	
	  { return (SceGxmTextureFormat)(SCE_GXM_TEXTURE_BASE_FORMAT_U2F10F10F10 | SCE_GXM_TEXTURE_SWIZZLE4_BGRA); 	}

	default:
		{
			SCE_SAMPLE_UTIL_ASSERT(0);
			return  (SceGxmTextureFormat)0;
		}
	}
}

int ssgi::RenderTargetImpl::initializeInternal(void *buffer,
											   uint32_t width,
											   uint32_t height,
											   SceGxmColorSurfaceType type,
											   SceGxmColorFormat format,
											   SceGxmOutputRegisterSize outputRegisterSize,
											   
											   uint32_t renderTargetFlags,
											   uint16_t scenesPerFrame,
											   SceGxmMultisampleMode msaaMode,
											   uint32_t multisampleLocations,
											   
											   SceGxmSyncObject*  vertexSyncObject,
											   SceGxmSyncObject*  fragmentSyncObject)
{

	return SCE_OK;
}


int ssgi::RenderTargetImpl::initialize(GraphicsLoaderImpl *loader, 
							 uint32_t width, uint32_t height,
							 SceGxmColorSurfaceType type,
							 SceGxmColorFormat format,
							 SceGxmOutputRegisterSize outputRegisterSize,

							 uint32_t renderTargetFlags,
							 uint16_t scenesPerFrame,
							 SceGxmMultisampleMode msaaMode,
							 uint32_t multisampleLocations,

							 SceGxmSyncObject*  vertexSyncObject,
							 SceGxmSyncObject*  fragmentSyncObject)
{
	int ret = SCE_OK;
	m_flags           = 0;
	m_validRegion     = NULL;
	m_vertexSyncObject    = vertexSyncObject;
	m_fragmentSyncObject  = fragmentSyncObject;

	m_outputRegisterSize = outputRegisterSize;
	m_msaaMode = msaaMode;

	uint32_t stride = 0;

	switch(type)
	{
	case SCE_GXM_COLOR_SURFACE_LINEAR:
		{

			ret = m_colorTexture.initializeGxmLinear(loader, presumeTextureFormat(format), width, height, 0, 
				std::max(SCE_GXM_COLOR_SURFACE_ALIGNMENT, 256u/*display buffer alignment*/));
			stride = ssiu::ceil(width, SCE_GXM_TEXTURE_IMPLICIT_STRIDE_ALIGNMENT);
			break;
		}
	case SCE_GXM_COLOR_SURFACE_TILED:
		{
			ret = m_colorTexture.initializeGxmTiled(loader, presumeTextureFormat(format), width, height, 0, 
				std::max(SCE_GXM_COLOR_SURFACE_ALIGNMENT, 256u/*display buffer alignment*/));
			stride = ssiu::ceil(width, SCE_GXM_TILE_SIZEX);
			break;
		}
	case SCE_GXM_COLOR_SURFACE_SWIZZLED:
		{
			ret = m_colorTexture.initializeGxmSwizzled(loader, presumeTextureFormat(format), width, height, 0, 
				std::max(SCE_GXM_COLOR_SURFACE_ALIGNMENT, 256u/*display buffer alignment*/));
			stride = width; // TODO
			break;
		}
	default:
		SCE_SAMPLE_UTIL_ASSERT(0);
	}
	if(ret != SCE_OK){
		return ret;
	}

	ret = m_colorTexture.setFilter(kTextureFilterLinear, kTextureFilterLinear, kTextureMipFilterDisabled);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);

	ret = sceGxmColorSurfaceInit(&m_colorSurface,
		format,
		type,
		(msaaMode != SCE_GXM_MULTISAMPLE_NONE) ? SCE_GXM_COLOR_SURFACE_SCALE_MSAA_DOWNSCALE : SCE_GXM_COLOR_SURFACE_SCALE_NONE,
		outputRegisterSize, 
		width, 
		height,
		stride, //ceil(width, 64), //GRAPHICS_UTIL_DEFAULT_DISPLAY_STRIDE_IN_PIXELS, //
		m_colorTexture.getData());
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);



	// set up parameters
	SceGxmRenderTargetParams params;
	memset(&params, 0, sizeof(SceGxmRenderTargetParams));
	params.flags				= renderTargetFlags;
	params.width				= width;
	params.height				= height;
	params.scenesPerFrame		= scenesPerFrame;
	params.multisampleMode		= msaaMode;
	params.multisampleLocations	= multisampleLocations;
	params.driverMemBlock		= SCE_UID_INVALID_UID;

	// create the render target
	ret = sceGxmCreateRenderTarget(&params, &m_renderTarget);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	return SCE_OK;
}

int ssgi::RenderTargetImpl::finalize(void)
{
	sceGxmDestroyRenderTarget(m_renderTarget);
	m_colorTexture.finalize();
	return SCE_OK;
}



int ssgi::DepthStencilSurfaceImpl::initialize(GraphicsLoaderImpl *loader, 
									uint32_t width, uint32_t height,
									SceGxmDepthStencilFormat format,
									SceGxmDepthStencilSurfaceType type,
									SceGxmMultisampleMode multisampleMode,
									bool enableForceStore,
									bool enableForceLoad

									)
{
	SceGxmTextureFormat texf = (SceGxmTextureFormat)0;
	switch(format)
	{
	case SCE_GXM_DEPTH_STENCIL_FORMAT_DF32:
		{
			texf = SCE_GXM_TEXTURE_FORMAT_F32M_R;
			break;
		}
	case SCE_GXM_DEPTH_STENCIL_FORMAT_S8:
		{
			texf = SCE_GXM_TEXTURE_FORMAT_S8_R;
			break;
		}
	case SCE_GXM_DEPTH_STENCIL_FORMAT_S8D24	:
		{
			texf = SCE_GXM_TEXTURE_FORMAT_X8U24_SD;
			break;
		}
	case SCE_GXM_DEPTH_STENCIL_FORMAT_D16:
		{
			texf = SCE_GXM_TEXTURE_FORMAT_U16_R;
			break;
		}
	case SCE_GXM_DEPTH_STENCIL_FORMAT_DF32_S8:
	default:
		{
			printf("not supported yet\n");
			SCE_SAMPLE_UTIL_ASSERT(0);
		}

	}

    // デプスバッファを作成する
    uint32_t alignedWidth; 
	uint32_t alignedHeight;

	int ret;
	// set up the texture
	if(type == SCE_GXM_DEPTH_STENCIL_SURFACE_LINEAR){
		alignedWidth = width;
		alignedHeight = height;

		if(multisampleMode == SCE_GXM_MULTISAMPLE_4X){
			alignedWidth *= 2;
			alignedHeight *= 2;
		}else if (multisampleMode == SCE_GXM_MULTISAMPLE_2X) {
			alignedHeight *= 2;
		}

		ret = m_depthStencilTexture.initializeGxmLinear(
			loader, 
			texf,
			alignedWidth,
			alignedHeight,
			0,
			maxUint32(SCE_GXM_TEXTURE_ALIGNMENT, SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT));
		SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );
	}else{
		alignedWidth = ssiu::ceil(width, SCE_GXM_TILE_SIZEX);
		alignedHeight = ssiu::ceil(height, SCE_GXM_TILE_SIZEY);

		if(multisampleMode == SCE_GXM_MULTISAMPLE_4X){
			alignedWidth *= 2;
			alignedHeight *= 2;
		}else if (multisampleMode == SCE_GXM_MULTISAMPLE_2X) {
			alignedHeight *= 2;
		}

		ret = m_depthStencilTexture.initializeGxmTiled(
			loader,
			texf,
			alignedWidth,
			alignedHeight,
			0,
			maxUint32(SCE_GXM_TEXTURE_ALIGNMENT, SCE_GXM_DEPTHSTENCIL_SURFACE_ALIGNMENT));
		SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	}

	//uint32_t sampleCount = alignedWidth*alignedHeight;
    uint32_t depthStrideInSamples = alignedWidth;
    if (multisampleMode == SCE_GXM_MULTISAMPLE_4X) {
        // XとYのサンプルを増加する
		//sampleCount *= 4;
        depthStrideInSamples *= 2;
    } else if (multisampleMode == SCE_GXM_MULTISAMPLE_2X) {
        // Yだけのサンプルを増加する
		//sampleCount *= 2;
    }

	// set up the surface
	ret = sceGxmDepthStencilSurfaceInit(
		&m_depthStencilSurface,
		format,
		type, 
		depthStrideInSamples,
		m_depthStencilTexture.getData(),
		NULL);
	SCE_SAMPLE_UTIL_ASSERT_EQUAL( ret, SCE_OK );

	sceGxmDepthStencilSurfaceSetForceLoadMode( &m_depthStencilSurface, enableForceLoad ? SCE_GXM_DEPTH_STENCIL_FORCE_LOAD_ENABLED : SCE_GXM_DEPTH_STENCIL_FORCE_LOAD_DISABLED);
	sceGxmDepthStencilSurfaceSetForceStoreMode( &m_depthStencilSurface, enableForceStore ? SCE_GXM_DEPTH_STENCIL_FORCE_STORE_ENABLED :  SCE_GXM_DEPTH_STENCIL_FORCE_STORE_DISABLED);

	sceGxmTextureSetMagFilter(m_depthStencilTexture.getGxmTexture(), SCE_GXM_TEXTURE_FILTER_LINEAR);
	sceGxmTextureSetMinFilter(m_depthStencilTexture.getGxmTexture(), SCE_GXM_TEXTURE_FILTER_LINEAR);
	sceGxmTextureSetUAddrMode(m_depthStencilTexture.getGxmTexture(), SCE_GXM_TEXTURE_ADDR_CLAMP);
	sceGxmTextureSetVAddrMode(m_depthStencilTexture.getGxmTexture(), SCE_GXM_TEXTURE_ADDR_CLAMP);


	return ret;
}

int ssgi::DepthStencilSurfaceImpl::finalize(void)
{
	m_depthStencilTexture.finalize();
	return SCE_OK;
}



#endif /* _SCE_TARGET_OS_PSP2 */


