/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include "dae_scene.h"
#include "dae_transform.h"
#include <new>

using namespace sce::SampleUtil::Graphics::Collada;

int Dae::Scene::Node::parse(sce::SampleUtil::Graphics::Collada::ColladaLoader *loaderConfig, TiXmlElement* elem)
{
	m_id = elem->Attribute("id");
	if(elem->Attribute("type") != NULL){
		m_type = elem->Attribute("type");
	}else{
		m_type = "NODE";
	}
	if(elem->Attribute("name") != NULL){
		m_name = elem->Attribute("name");
	}
	if(elem->Attribute("sid") != NULL){
		m_sid = elem->Attribute("sid");
	}

	for(TiXmlElement* celem = elem->FirstChildElement();
		celem != NULL;
		celem = celem->NextSiblingElement())
	{
		std::string elemName = celem->Value();
		//printf("elemName=%s\n", elemName.c_str());
		if(elemName == "matrix"){
			Dae::Transform::Matrix *m = new Dae::Transform::Matrix;
			m->parse(loaderConfig, celem);
			m_matrices.push_back(m);
		}else if(elemName == "rotate"){
			Dae::Transform::Rotate *r = new Dae::Transform::Rotate;
			r->parse(loaderConfig, celem);
			m_matrices.push_back(r);
		}else if(elemName == "translate"){
			Dae::Transform::Translate *t = new  Dae::Transform::Translate;
			t->parse(loaderConfig, celem);
			m_matrices.push_back(t);
		}else if(elemName == "scale"){
			Dae::Transform::Scale *s = new Dae::Transform::Scale;
			s->parse(loaderConfig, celem);
			m_matrices.push_back(s);
		}else if(elemName == "skew"){
			Dae::Transform::Skew *s = new Dae::Transform::Skew;
			s->parse(loaderConfig, celem);
			m_matrices.push_back(s);
		}else if(elemName == "node"){
			Node *n = new Node;
			n->parse(loaderConfig, celem);
			m_nodes.push_back(n);
		}else if(elemName == "instance_controller"){
			Dae::Controller::InstanceController *n = new Dae::Controller::InstanceController;
			n->parse(loaderConfig, celem);
			m_instanceController.push_back(n);
		}else if(elemName == "instance_geometry"){
			Dae::Geometry::InstanceGeometry *n = new Dae::Geometry::InstanceGeometry;
			n->parse(loaderConfig, celem);
			m_instanceGeometry.push_back(n);
		}
	}
	return SCE_OK;
}

void Dae::Scene::Node::dispose(void)
{
	m_id = "";
	m_sid = "";
	m_type = "";
	m_name = "";

	for(unsigned int i=0; i<m_matrices.size();i++){
		delete m_matrices.at(i);
	}
	m_matrices.clear();

	for(unsigned int i=0; i<m_nodes.size();i++){
		delete m_nodes.at(i);
	}
	m_nodes.clear();

	for(unsigned int i=0; i<m_instanceController.size();i++){
		delete m_instanceController.at(i);
	}
	m_instanceController.clear();

	for(unsigned int i=0; i<m_instanceGeometry.size();i++){
		delete m_instanceGeometry.at(i);
	}
	m_instanceGeometry.clear();
}
