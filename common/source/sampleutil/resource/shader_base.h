/* SIE CONFIDENTIAL
PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
* Copyright (C) 2012 Sony Interactive Entertainment Inc.
* All Rights Reserved.
*/

#ifndef __SHADERBASE_H__
#define __SHADERBASE_H__

#ifndef __PSSL__

#include <vectormath.h>
using namespace sce::Vectormath::Simd::Aos;

#endif /* __PSSL__ */


#ifndef __cplusplus



#define Vec2		float2
#define Vec3		float3
#define Vec4		float4


#define Vector2		float2
#define Vector3		float3
#define Vector4		float4

uint FIRSTBITLOW_SLOW( uint input )
{
	for( int bit=0; bit<32; ++bit )
		if( input & (1<<bit) )
			return bit;
	return 32;
}

uint FIRSTBITHIGH_SLOW( uint input )
{
	for( int bit=31; bit>=0; --bit )
		if( input & (1<<bit) )
			return bit;
	return 32;
}

#define __CBREGISTER( i )		 : register( b ## i )

#ifdef __PSSL__
	#define Matrix4		column_major matrix
	#define unistruct	ConstantBuffer
#else /*__PSSL__*/
	#define Matrix4		matrix
	#define unistruct	cbuffer
#endif /*__PSSL__*/

#else /* __cplusplus */
#include <stddef.h>

#define __CBREGISTER( i )
#define ATTR_OFFS			offsetof

#define unistruct	struct
#endif


#endif
