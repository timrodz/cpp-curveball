/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */


#ifndef _COLLADA_RENDER_UTIL_UTIL_H
#define _COLLADA_RENDER_UTIL_UTIL_H

#include <string>
#include <algorithm>
#include <vector>
#include <sampleutil/graphics/collada/collada_base.h>
#include <tinyxml.h>
#include <ctype.h>





/// @brief

namespace sce
{
namespace SampleUtil
{
namespace Graphics
{

	namespace Collada
	{
	namespace Internal
	{

		namespace Util
		{

			static inline unsigned int ceil16(unsigned int a)
			{
				return (a+0xf) & ~0xf;
			}

			std::string getExtention(const std::string &path);

			void* malloc_and_load(const char* path, uint32_t *pSize);

			void tokenize(std::vector<std::string>& tokens, 
				const std::string& str,
				const std::string& delimiters = " ");

			int parseIntArray(int *array, unsigned int arrayLength, std::string arrayStr);
			int parseFloatArray(float *array, unsigned int arrayLength, std::string arrayStr);
			sce::Vectormath::Simd::Aos::Matrix4 fromFloatArray(const float *ary) ;
			sce::Vectormath::Simd::Aos::Matrix4 fromElementText(TiXmlElement* elem);
			Dae::VertexSemantic strToSemantic(std::string strSemantic);

			static inline
				std::string string_to_lower(std::string as)
			{
				std::string s = as;
				std::transform(s.begin(), s.end(), s.begin(), tolower); 
				return s;
			}

			static inline
				std::string string_to_upper(std::string as)
			{
				std::string s = as;
				std::transform(s.begin(), s.end(), s.begin(), toupper); 
				return s;
			}

			static inline bool isAbsolutePath(std::string path)
			{
				std::string::size_type seppos = path.find_last_of(":");
				return (seppos != std::string::npos) || (path.at(0) == '/') || (path.at(0) == '\\') ;

			}

			static inline std::string getDir(std::string path)
			{
				std::string::size_type seppos = path.find_last_of("/\\:");
				if(seppos == std::string::npos){
					return "";
				}else{
					return path.substr(0, seppos+1);
				}
			}



			static inline std::string getXmlContext(TiXmlElement* elem)
			{
				if(elem->Parent() != NULL){
					std::string parent = getXmlContext((TiXmlElement*)elem->Parent());
					return parent + "." + std::string(elem->Value());
				}else{
					return std::string(elem->Value());
				}
			}



			static inline std::string sourceToId(std::string source)
			{
				return source.substr(1);
			}
			static inline
				std::string urlToId(std::string url)
			{
				return url.substr(1);
			}
		}

	}
	}
}
}
}



#endif /* _COLLADA_RENDER_UTIL_UTIL_H */
