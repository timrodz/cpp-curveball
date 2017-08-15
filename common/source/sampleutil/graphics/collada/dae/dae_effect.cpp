/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include "dae_effect.h"



using namespace sce::SampleUtil::Graphics::Collada::Dae::effect;



static float *getFValues(uint32_t numFloat, const char *text)
{
	float *ret = new float[numFloat];
	sce::SampleUtil::Graphics::Collada::Internal::Util::parseFloatArray(ret, numFloat, text);
	return ret;
}

void NewParam::parse(TiXmlElement* elem)
{
	m_sid = elem->Attribute("sid");
	m_semantic = "";
	if(elem->FirstChildElement("semantic")){
		m_semantic = elem->FirstChildElement("semantic")->GetText();
	}

	m_valueType = Dae::kValueInvalid;
	m_numFloats=0; 

	if(elem->FirstChildElement("surface")){
		m_valueType = Dae::_kValueSurface;
		m_surface = new Surface;
		m_surface->parse(elem->FirstChildElement("surface"));
		return;
	}else if(elem->FirstChildElement("sampler2D")){
		m_valueType = Dae::kValueSampler2d;
		m_sampler2D = new Sampler2D;
		m_sampler2D->parse(elem->FirstChildElement("sampler2D"));
		return;
	}


	if(elem->FirstChildElement("float"))   { m_valueType = Dae::kValueFloat;    m_numFloats=1; m_floats = getFValues(1,   elem->FirstChildElement("float")->GetText());    return; }
	if(elem->FirstChildElement("float2"))  { m_valueType = Dae::kValueFloat2;   m_numFloats=2; m_floats = getFValues(2,   elem->FirstChildElement("float2")->GetText());   return; }
	if(elem->FirstChildElement("float3"))  { m_valueType = Dae::kValueFloat3;   m_numFloats=3; m_floats = getFValues(3,   elem->FirstChildElement("float3")->GetText());   return; }
	if(elem->FirstChildElement("float4"))  { m_valueType = Dae::kValueFloat4;   m_numFloats=4; m_floats = getFValues(4,   elem->FirstChildElement("float4")->GetText());   return; }

	if(elem->FirstChildElement("float1x1")){ m_valueType = Dae::kValueFloat1x1; m_numFloats=1*1; m_floats = getFValues(1*1, elem->FirstChildElement("float1x1")->GetText()); return; }
	if(elem->FirstChildElement("float1x2")){ m_valueType = Dae::kValueFloat1x2; m_numFloats=1*2; m_floats = getFValues(1*2, elem->FirstChildElement("float1x2")->GetText()); return; }
	if(elem->FirstChildElement("float1x3")){ m_valueType = Dae::kValueFloat1x3; m_numFloats=1*3; m_floats = getFValues(1*3, elem->FirstChildElement("float1x3")->GetText()); return; }
	if(elem->FirstChildElement("float1x4")){ m_valueType = Dae::kValueFloat1x4; m_numFloats=1*4; m_floats = getFValues(1*4, elem->FirstChildElement("float1x4")->GetText()); return; }

	if(elem->FirstChildElement("float2x1")){ m_valueType = Dae::kValueFloat2x1; m_numFloats=2*1; m_floats = getFValues(2*1, elem->FirstChildElement("float2x1")->GetText()); return; }
	if(elem->FirstChildElement("float2x2")){ m_valueType = Dae::kValueFloat2x2; m_numFloats=2*2; m_floats = getFValues(2*2, elem->FirstChildElement("float2x2")->GetText()); return; }
	if(elem->FirstChildElement("float2x3")){ m_valueType = Dae::kValueFloat2x3; m_numFloats=2*3; m_floats = getFValues(2*3, elem->FirstChildElement("float2x3")->GetText()); return; }
	if(elem->FirstChildElement("float2x4")){ m_valueType = Dae::kValueFloat2x4; m_numFloats=2*4; m_floats = getFValues(2*4, elem->FirstChildElement("float2x4")->GetText()); return; }

	if(elem->FirstChildElement("float3x1")){ m_valueType = Dae::kValueFloat3x1; m_numFloats=3*1; m_floats = getFValues(3*1, elem->FirstChildElement("float3x1")->GetText()); return; }
	if(elem->FirstChildElement("float3x2")){ m_valueType = Dae::kValueFloat3x2; m_numFloats=3*2; m_floats = getFValues(3*2, elem->FirstChildElement("float3x2")->GetText()); return; }
	if(elem->FirstChildElement("float3x3")){ m_valueType = Dae::kValueFloat3x3; m_numFloats=3*3; m_floats = getFValues(3*3, elem->FirstChildElement("float3x3")->GetText()); return; }
	if(elem->FirstChildElement("float3x4")){ m_valueType = Dae::kValueFloat3x4; m_numFloats=3*4; m_floats = getFValues(3*4, elem->FirstChildElement("float3x4")->GetText()); return; }

	if(elem->FirstChildElement("float4x1")){ m_valueType = Dae::kValueFloat4x1; m_numFloats=4*1; m_floats = getFValues(4*1, elem->FirstChildElement("float4x1")->GetText()); return; }
	if(elem->FirstChildElement("float4x2")){ m_valueType = Dae::kValueFloat4x2; m_numFloats=4*2; m_floats = getFValues(4*2, elem->FirstChildElement("float4x2")->GetText()); return; }
	if(elem->FirstChildElement("float4x3")){ m_valueType = Dae::kValueFloat4x3; m_numFloats=4*3; m_floats = getFValues(4*3, elem->FirstChildElement("float4x3")->GetText()); return; }
	if(elem->FirstChildElement("float4x4")){ m_valueType = Dae::kValueFloat4x4; m_numFloats=4*4; m_floats = getFValues(4*4, elem->FirstChildElement("float4x4")->GetText()); return; }

}

void NewParam::dispose(void)
{
	if(m_valueType == Dae::kValueInvalid){
		return;
	}else if(m_valueType == Dae::_kValueSurface){
		delete m_surface;
	}else if(m_valueType == Dae::kValueSampler2d){
		delete m_sampler2D;
	}else{
		delete [] m_floats;
	}

}