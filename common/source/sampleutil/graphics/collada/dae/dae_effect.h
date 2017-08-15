/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _COLLADA_LOADER_UTIL_DAE_EFFECT_H
#define _COLLADA_LOADER_UTIL_DAE_EFFECT_H

#include "dae_data_flow.h"

#include <string>
#include <tinyxml.h>
#include <graphics/collada/collada_util.h>
#include "dae_common.h"


namespace sce
{
namespace SampleUtil
{
namespace Graphics
{

	 namespace Collada
	 {
	 namespace Dae
	 {
		namespace effect
		{

			struct Surface
			{
				std::string m_init_from;
				std::string m_format;
				void parse(TiXmlElement* elem)
				{
					m_init_from = elem->FirstChildElement("init_from")->GetText();
					if(elem->FirstChildElement("format") != NULL){
						m_format    = elem->FirstChildElement("format")->GetText();
					}else{
						m_format = "";
					}

				}
			};

			struct Sampler2D
			{
				std::string m_source;
				std::string m_wrapS;
				std::string m_wrapT;
				std::string m_minfilter;
				std::string m_magfilter;
				std::string m_mipfilter;

				void parse(TiXmlElement* elem)
				{
					m_source    = elem->FirstChildElement("source")->GetText();

					m_wrapS     = (elem->FirstChildElement("wrap_s") != NULL) ? elem->FirstChildElement("wrap_s")->GetText() : "";
					m_wrapT     = (elem->FirstChildElement("wrap_t") != NULL) ? elem->FirstChildElement("wrap_t")->GetText() : "";;
					m_minfilter = (elem->FirstChildElement("minfilter") != NULL) ? elem->FirstChildElement("minfilter")->GetText() : "";;
					m_magfilter = (elem->FirstChildElement("magfilter") != NULL) ? elem->FirstChildElement("magfilter")->GetText() : "";;
					m_mipfilter = (elem->FirstChildElement("mipfilter") != NULL) ? elem->FirstChildElement("mipfilter")->GetText() : "";;
				}
			};

			struct NewParam
			{
				std::string m_sid;
				std::string m_semantic;
				Dae::ValueType m_valueType;
				float *m_floats;
				uint32_t m_numFloats;
				Surface *m_surface;
				Sampler2D *m_sampler2D;

				NewParam(void)
				{
					m_surface = NULL;
					m_sampler2D= NULL;
				}

				void parse(TiXmlElement* elem);

				void dispose(void);

				~NewParam(void)
				{
					dispose();
				}

				void copyFrom(NewParam *newParam)
				{
					*this = *newParam;
					if(m_valueType == Dae::kValueInvalid){
						return;
					}else if(m_valueType ==  Dae::_kValueSurface){
						m_surface = new Surface;
						*m_surface = *newParam->m_surface;
					}else if(m_valueType ==  Dae::kValueSampler2d){
						m_sampler2D = new Sampler2D;
						*m_sampler2D = *newParam->m_sampler2D;
					}else{
						m_floats = new float[m_numFloats];
						memcpy(m_floats, newParam->m_floats, sizeof(float)*m_numFloats);
					}


				}

			};

			struct ColorOrTexture
			{
				bool m_isColor;
				float m_rgba[4];
				std::string m_texture;
				std::string m_texcoord;

				ColorOrTexture(void)
				{
					m_isColor = true;
					m_rgba[0] = m_rgba[1] = m_rgba[2] = m_rgba[3] = 0;
				}

				void parse(TiXmlElement* elem)
				{
					if(elem == NULL){
						m_isColor = true;
						m_rgba[0] = m_rgba[1] = m_rgba[2] = m_rgba[3] = 0;
						return;
					}
					if(elem->FirstChildElement("color") != NULL){
						const char *color_text = elem->FirstChildElement("color")->GetText();
						Internal::Util::parseFloatArray(m_rgba, 4, color_text); // TODO: range check
						m_isColor = true;
					}else{
						m_texture = elem->FirstChildElement("texture")->Attribute("texture");
						m_texcoord = elem->FirstChildElement("texture")->Attribute("texcoord");
						m_isColor = false;
					}
				}


			};

			// TODO
			static bool getTransparent(TiXmlElement* elem)
			{
				if(elem->FirstChildElement("transparent")){
					TiXmlElement* transElem = elem->FirstChildElement("transparent");
					if(transElem->Attribute("opaque") != NULL){
						std::string opaque = transElem->Attribute("opaque");
						return opaque == "A_ONE";
					}
				}
				return false;
			}

			struct Phong
			{
				ColorOrTexture m_emission;
				ColorOrTexture m_ambient;
				ColorOrTexture m_diffuse;
				ColorOrTexture m_specular;
				float m_shininess;
				bool isTransparent;
				std::vector<std::string> m_texcoords;
				void parse(TiXmlElement* elem)
				{
					m_emission.parse(elem->FirstChildElement("emission"));
					m_ambient .parse(elem->FirstChildElement("ambient"));
					m_diffuse .parse(elem->FirstChildElement("diffuse"));
					m_specular.parse(elem->FirstChildElement("specular"));

					m_shininess = 1.0f;
					TiXmlElement* selem = elem->FirstChildElement("shininess");
					if(selem != NULL){
						TiXmlElement* felem = selem->FirstChildElement("float");
						const char * s_float = felem->GetText();
						if(s_float != NULL){
							m_shininess = (float)atof(s_float);
						}
					}

					isTransparent = getTransparent(elem);

					if( !m_emission.m_isColor ){
						m_texcoords.push_back(m_emission.m_texcoord);
					}
					if( !m_ambient.m_isColor ){
						m_texcoords.push_back(m_ambient.m_texcoord);
					}
					if( !m_diffuse.m_isColor ){
						m_texcoords.push_back(m_diffuse.m_texcoord);
					}
					if( !m_specular.m_isColor ){
						m_texcoords.push_back(m_specular.m_texcoord);
					}
				}
			};

			struct Blinn
			{
				ColorOrTexture m_emission;
				ColorOrTexture m_ambient;
				ColorOrTexture m_diffuse;
				ColorOrTexture m_specular;
				float m_shininess;
				bool isTransparent;
				std::vector<std::string> m_texcoords;
				void parse(TiXmlElement* elem)
				{
					m_emission.parse(elem->FirstChildElement("emission"));
					m_ambient .parse(elem->FirstChildElement("ambient"));
					m_diffuse .parse(elem->FirstChildElement("diffuse"));
					m_specular.parse(elem->FirstChildElement("specular"));
					isTransparent = getTransparent(elem);

					if( !m_emission.m_isColor ){
						m_texcoords.push_back(m_emission.m_texcoord);
					}
					if( !m_ambient.m_isColor ){
						m_texcoords.push_back(m_ambient.m_texcoord);
					}
					if( !m_diffuse.m_isColor ){
						m_texcoords.push_back(m_diffuse.m_texcoord);
					}
					if( !m_specular.m_isColor ){
						m_texcoords.push_back(m_specular.m_texcoord);
					}

					m_shininess = 1.0f;
					TiXmlElement* selem = elem->FirstChildElement("shininess");
					if(selem != NULL){
						TiXmlElement* felem = selem->FirstChildElement("float");
						const char * s_float = felem->GetText();
						if(s_float != NULL){
							m_shininess = (float)atof(s_float);
						}
					}
				}
			};

			struct Lambert
			{
				ColorOrTexture m_emission;
				ColorOrTexture m_ambient;
				ColorOrTexture m_diffuse;
				ColorOrTexture m_specular;
				bool isTransparent;
				std::vector<std::string> m_texcoords;

				void parse(TiXmlElement* elem)
				{
					m_emission.parse(elem->FirstChildElement("emission"));
					m_ambient .parse(elem->FirstChildElement("ambient"));
					m_diffuse .parse(elem->FirstChildElement("diffuse"));
					m_specular.m_isColor = true;
					m_specular.m_rgba[0] = 0.0f;
					m_specular.m_rgba[1] = 0.0f;
					m_specular.m_rgba[2] = 0.0f;
					m_specular.m_rgba[3] = 0.0f;
					isTransparent = getTransparent(elem);

					if( !m_emission.m_isColor ){
						m_texcoords.push_back(m_emission.m_texcoord);
					}
					if( !m_ambient.m_isColor ){
						m_texcoords.push_back(m_ambient.m_texcoord);
					}
					if( !m_diffuse.m_isColor ){
						m_texcoords.push_back(m_diffuse.m_texcoord);
					}
					if( !m_specular.m_isColor ){
						m_texcoords.push_back(m_specular.m_texcoord);
					}
				}

			};

			struct Technique
			{
				std::string m_sid;
				Phong *m_phong;
				Blinn *m_blinn;
				Lambert *m_lambert;

				void parse(TiXmlElement* elem)
				{
					m_sid = elem->Attribute("sid");

					m_phong = NULL;
					if(elem->FirstChildElement("phong") != NULL){
						m_phong = new Phong;
						m_phong->parse(elem->FirstChildElement("phong"));
					}

					m_blinn = NULL;
					if(elem->FirstChildElement("blinn") != NULL){
						m_blinn = new Blinn;
						m_blinn->parse(elem->FirstChildElement("blinn"));
					}

					m_lambert = NULL;
					if(elem->FirstChildElement("lambert") != NULL){
						m_lambert = new Lambert;
						m_lambert->parse(elem->FirstChildElement("lambert"));
					}
				}

				~Technique(void)
				{
					if(m_phong != NULL){
						delete m_phong;
						m_phong = NULL;
					}
					if(m_blinn != NULL){
						delete m_blinn;
						m_blinn = NULL;
					}
					if(m_lambert != NULL){
						delete m_lambert;
						m_lambert = NULL;
					}
				}

				std::vector<std::string>* getTexcoords(void)
				{
					if(m_phong != NULL){
						return &m_phong->m_texcoords;
					}
					if(m_blinn != NULL){
						return &m_blinn->m_texcoords;
					}
					if(m_lambert != NULL){
						return &m_lambert->m_texcoords;
					}
					return NULL;
				}
			};

			struct ProfileCommon
			{
				std::vector<NewParam*> m_newParams;
				Technique* m_technique;
				void parse(TiXmlElement* elem)
				{
					for(TiXmlElement* celem=elem->FirstChildElement("newparam");
						celem != NULL;
						celem = celem->NextSiblingElement("newparam"))
					{
						NewParam *n = new NewParam;
						n->parse(celem);
						m_newParams.push_back(n);
					}

					m_technique = new Technique;
					m_technique->parse(elem->FirstChildElement("technique"));
				}
				~ProfileCommon(void)
				{
					for(unsigned int i=0; i<m_newParams.size(); i++){
						delete m_newParams.at(i);
					}
					m_newParams.clear();
					delete m_technique;

				}

				const NewParam* findNewParamBySid(std::string sid)
				{
					for(unsigned int i=0; i<m_newParams.size(); i++){
						if(m_newParams.at(i)->m_sid == sid){
							return m_newParams.at(i);
						}
					}
					return NULL;
				}
				std::vector<std::string>* getTexcoords(void)
				{
					return m_technique->getTexcoords();
				}


			};

			struct Include
			{
				std::string m_sid;
				std::string m_url;
				void parse(TiXmlElement* elem)
				{
					m_sid = elem->Attribute("sid");
					m_url = elem->Attribute("url");
				}
			};

			struct Bind
			{
				std::string m_symbol;
				DataFlow::Param *m_param;

				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("symbol")){
						m_symbol = elem->Attribute("symbol");
					}

					if( elem->FirstChildElement("param") != NULL){
						m_param = new DataFlow::Param;
						m_param->parse(loaderConfig, elem->FirstChildElement("param"));

					}else{
						m_param  =NULL;
					}

				}
				void dispose(void)
				{
					if(m_param != NULL){
						delete m_param;
						m_param = NULL;
					}
				}
				~Bind(void)
				{
					dispose();
				}

			};

			struct Name
			{
				std::string m_source;
				std::string m_name;
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("source") != NULL){
						m_source = elem->Attribute("source");
					}
					if(elem->GetText() != NULL){
						m_name = elem->GetText();
					}else{
						m_name = "";
					}
				}
			};

			struct Shader
			{
				std::string m_stage;
				std::string m_compierTarget;
				Name *m_name;
				std::vector<Bind*> m_binds;

				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("stage")){
						m_stage = elem->Attribute("stage");
					}
					if(elem->FirstChildElement("compiler_target") != NULL){
						m_compierTarget = elem->FirstChildElement("compiler_target")->GetText();
					}
					m_name = NULL;
					if(elem->FirstChildElement("name") != NULL){
						m_name = new Name;
						m_name->parse(loaderConfig, elem->FirstChildElement("name"));
					}

					for(TiXmlElement* celem=elem->FirstChildElement("bind");
						celem != NULL;
						celem = celem->NextSiblingElement("bind"))
					{
						Bind *b = new Bind;
						b->parse(loaderConfig, celem);
						m_binds.push_back(b);
					}
				}

				Bind* findBindBySymbol(std::string symbol)
				{
					for(uint32_t i=0; i<m_binds.size(); i++){
						if(m_binds.at(i)->m_symbol == symbol){
							return m_binds.at(i);
						}
					}
					return NULL;
				}

				void dispose(void)
				{
					if(m_name != NULL){
						delete m_name;
						m_name = NULL;
					}
					for(uint32_t i=0; i<m_binds.size(); i++){
						delete m_binds.at(i);
					}
					m_binds.clear();
				}

				~Shader(void)
				{
					dispose();
				}
			};

			struct BlendFunc
			{
				std::string m_src;
				std::string m_dest;
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					TiXmlElement* src=elem->FirstChildElement("src");
					if((src != NULL) && (src->Attribute("value") != NULL)){
						m_src = src->Attribute("value");
					}else{
						m_src = "ONE";
					}

					TiXmlElement* dest=elem->FirstChildElement("dest");
					if((dest != NULL) && (dest->Attribute("value") != NULL)){
						m_dest = dest->Attribute("value");
					}else{
						m_dest = "ZERO";
					}

				}
			};

			struct Pass
			{
				std::string m_sid;
				std::vector<Shader*> m_shaders;
				BlendFunc *m_blendFunc;
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("sid")){
						m_sid = elem->Attribute("sid");
					}

					{					
						TiXmlElement* celem=elem->FirstChildElement("blend_func");
						if(celem == NULL){
							m_blendFunc = NULL;
						}else{
							m_blendFunc = new BlendFunc;
							m_blendFunc->parse(loaderConfig, celem);
						}
					}

					for(TiXmlElement* celem=elem->FirstChildElement("shader");
						celem != NULL;
						celem = celem->NextSiblingElement("shader"))
					{
						Shader *s = new Shader;
						s->parse(loaderConfig, celem);
						m_shaders.push_back(s);
					}
				}

				Shader *getShaderByStage(std::string stage)
				{
					for(uint32_t i=0; i<m_shaders.size(); i++){
						if(m_shaders.at(i)->m_stage == stage){
							return m_shaders.at(i);
						}
					}
					return NULL;
				}


				void dispose(void)
				{
					if(m_blendFunc != NULL){
						delete m_blendFunc;
					}
					m_blendFunc = NULL;
					for(uint32_t i=0; i<m_shaders.size(); i++){
						delete m_shaders.at(i);
					}
					m_shaders.clear();
				}

				~Pass(void)
				{
					dispose();
				}

			};

			struct ProfileCgTechnique
			{
				std::string m_sid;
				std::vector<Pass*> m_passes;
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					if(elem->Attribute("sid")){
						m_sid = elem->Attribute("sid");
					}

					for(TiXmlElement* celem=elem->FirstChildElement("pass");
						celem != NULL;
						celem = celem->NextSiblingElement("pass"))
					{
						Pass *p = new Pass;
						p->parse(loaderConfig, celem);
						m_passes.push_back(p);
					}
				}

				void dispose(void)
				{
					for(uint32_t i=0; i<m_passes.size(); i++){
						delete m_passes.at(i);
					}
					m_passes.clear();
				}

				~ProfileCgTechnique(void)
				{
					dispose();
				}


			};

			struct ProfileCg
			{
				std::vector<Include*> m_includes;
				std::vector<NewParam*> m_newParams;
				ProfileCgTechnique* m_technique;

				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					for(TiXmlElement* celem=elem->FirstChildElement("include");
						celem != NULL;
						celem = celem->NextSiblingElement("include"))
					{
						Include *i = new Include;
						i->parse(celem);
						m_includes.push_back(i);
					}

					for(TiXmlElement* celem=elem->FirstChildElement("newparam");
						celem != NULL;
						celem = celem->NextSiblingElement("newparam"))
					{
						NewParam *n = new NewParam;
						n->parse(celem);
						m_newParams.push_back(n);
					}

					m_technique = new ProfileCgTechnique;
					m_technique->parse(loaderConfig, elem->FirstChildElement("technique"));
				}
				~ProfileCg(void)
				{
					for(unsigned int i=0; i<m_includes.size(); i++){
						delete m_includes.at(i);
					}
					for(unsigned int i=0; i<m_newParams.size(); i++){
						delete m_newParams.at(i);
					}
					m_newParams.clear();
					delete m_technique;

				}

				std::string getSourceUrl(std::string sid)
				{
					for(unsigned int i=0; i<m_includes.size(); i++){
						if(m_includes.at(i)->m_sid == sid){
							return m_includes.at(i)->m_url;
						}
					}
					return "";
				}

				NewParam* findNewParamBySid(std::string sid)
				{
					for(unsigned int i=0; i<m_newParams.size(); i++){
						if(m_newParams.at(i)->m_sid == sid){
							return m_newParams.at(i);
						}
					}

					return NULL;
				}

				Shader *getShaderByStage(std::string stage)
				{
					Pass *pass = m_technique->m_passes.at(0);
					Shader * ret = pass->getShaderByStage(stage);

					return ret;
				}

			};


			struct Effect 
			{
				std::string m_id;
				std::string m_name;
				std::vector<ProfileCommon*> m_profileCommons;
				std::vector<ProfileCg*> m_profileCgs;

				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					m_id = elem->Attribute("id");
					if(elem->Attribute("name") != NULL){
						m_name = elem->Attribute("name");
					}
					for(TiXmlElement* celem=elem->FirstChildElement("profile_COMMON");
						celem != NULL;
						celem = celem->NextSiblingElement("profile_COMMON"))
					{
						ProfileCommon *pc = new ProfileCommon;
						pc->parse(celem);
						m_profileCommons.push_back(pc);
					}

					for(TiXmlElement* celem=elem->FirstChildElement("profile_CG");
						celem != NULL;
						celem = celem->NextSiblingElement("profile_CG"))
					{
						ProfileCg *pc = new ProfileCg;
						pc->parse(loaderConfig, celem);
						m_profileCgs.push_back(pc);
					}
				}

				~Effect(void)
				{
					for(unsigned int i=0; i<m_profileCgs.size(); i++){
						delete m_profileCgs.at(i);
					}
					m_profileCgs.clear();
					for(unsigned int i=0; i<m_profileCommons.size(); i++){
						delete m_profileCommons.at(i);
					}
					m_profileCommons.clear();
				}


				std::string getSourceUrl(std::string sid)
				{
					return m_profileCgs.at(0)->getSourceUrl(sid);
				}

			};

			struct BindMaterial
			{
				Dae::Common::TechniqueCommon *m_techniqueCommon;

				BindMaterial(void)
				{
					m_techniqueCommon = NULL;
				}
				int parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					int ret;
					if(elem->FirstChildElement("technique_common") == NULL){
						fprintf(stderr, "Non-skin controller is not supported: %s\n", Internal::Util::getXmlContext(elem).c_str());
						dispose();
						return -1;
					}
					m_techniqueCommon = new Dae::Common::TechniqueCommon;
					ret = m_techniqueCommon->parse(loaderConfig, elem->FirstChildElement("technique_common"));
					return ret;
				}
				void dispose(void)
				{
					if(m_techniqueCommon != NULL){
						delete m_techniqueCommon;
						m_techniqueCommon = NULL;
					}
				}

				~BindMaterial(void)
				{
					dispose();
				}
			};


			struct LibraryEffects
			{
				std::vector<Effect*> m_effects;
				void parse(sce::SampleUtil::Graphics::Collada::ColladaLoader* loaderConfig, TiXmlElement* elem)
				{
					for(TiXmlElement* celem=elem->FirstChildElement("effect");
						celem != NULL;
						celem = celem->NextSiblingElement("effect"))
					{
						Effect *e = new Effect;
						e->parse(loaderConfig, celem);
						m_effects.push_back(e);
					}
				}

				~LibraryEffects(void)
				{
					for(unsigned int i=0; i<m_effects.size(); i++){
						delete m_effects.at(i);
					}
					m_effects.clear();
				}

			};
		}
}
}
}


}
}
#endif /* _COLLADA_LOADER_UTIL_DAE_EFFECT_H */