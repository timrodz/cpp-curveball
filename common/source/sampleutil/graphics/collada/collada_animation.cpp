/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2012 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "stdafx.h"

#include <sampleutil/graphics/collada/collada_animation.h>
#include <sampleutil/graphics/collada.h>
#include "graphics/collada/collada_internal.h"
#include <graphics/collada/collada_util.h>
#include <graphics/collada/dae/dae.h>
#include <float.h>
#include <set>

using namespace sce::SampleUtil::Graphics;
using namespace sce::SampleUtil::Graphics::Collada::Internal::Util;

namespace ssg = sce::SampleUtil::Graphics;
namespace ssgc = sce::SampleUtil::Graphics::Collada;
namespace ssgci = sce::SampleUtil::Graphics::Collada::Internal;
namespace vm = sce::Vectormath::Simd::Aos;

int Collada::Internal::TransformAnimationTarget::initialize(std::string targetString, bool maybeTagetIsMatrix)
{
	m_attribute = "";
	m_originalString = targetString;
	//printf("targetString=%s\n", targetString.c_str());
	unsigned int current = 0;
	{
		int slash = targetString.find_first_of("/", current);
		m_targetNodeId = targetString.substr(current, slash);
		current = slash+ strlen("/");
	}
	{
		int sidstart = current;
		int sidend = targetString.find_first_of("(.", sidstart);
		m_targetNodeSid = targetString.substr(sidstart, sidend-sidstart);
		current = sidend;
	}
	if((current < targetString.size()) && (targetString.at(current) == '.')){
		int attrstart = current+1;
		int attrend =  targetString.find_first_of("(", attrstart);
		m_attribute = targetString.substr(attrstart, attrend  - attrstart);
		current= attrend;
	}

	if((current < targetString.size()) && (targetString.at(current) == '(')){
		m_type = TYPE_MATRIX_ELEM;

		int colstart = current + strlen("(");
		int colend = targetString.find_first_of(")", colstart);
		std::string colstr = targetString.substr(colstart, colend-colstart);
		m_col = atoi(colstr.c_str());
		int rowstart = colend + strlen(")(");
		int rowend = targetString.find_first_of(")", rowstart);
		std::string rowstr = targetString.substr(rowstart, rowend-rowstart);
		m_row = atoi(rowstr.c_str());
		//printf("m_targetNode=%s, m_col=%d, m_row=%d\n", m_targetNode.c_str(), m_col, m_row);
	}else{
		if(m_attribute == "ANGLE"){
			m_type = TYPE_ANGLE;
		}else if(m_attribute == "X"){
			m_type = TYPE_VECTOR_X;
		}else if(m_attribute == "Y"){
			m_type = TYPE_VECTOR_Y;
		}else if(m_attribute == "Z"){
			m_type = TYPE_VECTOR_Z;
		}else if(m_attribute == ""){
			if(maybeTagetIsMatrix) {
				m_type = TYPE_MATRIX;
			}else{
				//printf(__FILE__":%d: VECTOR!? targetString=%s\n", __LINE__, targetString.c_str());
				m_type = TYPE_VECTOR;
			}
		}else{
			printf(__FILE__":%d: unsupported targetString=%s\n", __LINE__, targetString.c_str());
			m_type = TYPE_UNSUPPORTED;
		}
	}
	//print();
	return 0;
}

Collada::Internal::TransformAnimationChannel::TransformAnimationChannel(void)
{
	m_times = NULL;
	m_values = NULL;
	m_indexMap = NULL;
}

void Collada::Internal::TransformAnimationChannel::initialize(float *times, unsigned int numData, 
										   float *values, unsigned int numValues,
										   std::string targetString, bool maybeTargetIsMatrix)
{
	m_numData = numData;
	m_times = new float[m_numData];
	for(unsigned int i=0; i<m_numData; i++){
		m_times[i] = times[i];
	}
	m_values = new float[numValues];
	for(unsigned int i=0; i<numValues; i++){
		m_values[i] = values[i];
	}

	m_minPeriod = FLT_MAX;
	for(unsigned int i=0; i<m_numData-1; i++){
		float period = m_times[i+1] - times[i];
		m_minPeriod = std::min(period, m_minPeriod);
	}
	if(m_minPeriod <= 0.0f){
		m_minPeriod = (float)(1.0 / 60.0);
	}
	m_beginTime = m_times[0];
	m_endTime   = m_times[m_numData-1];

	m_numIndexMap = (unsigned int)( (m_endTime - m_beginTime) / m_minPeriod);
	m_indexMap = new int[m_numIndexMap];
	for(unsigned int i=0; i<m_numIndexMap; i++){
		m_indexMap[i] = getIndex(m_beginTime + m_minPeriod * i, 0).first;
	}

	m_target.initialize(targetString, maybeTargetIsMatrix);
}


std::pair<int,int> Collada::Internal::TransformAnimationChannel::getIndex(float time) const
{
	if(m_numIndexMap <= 0){
		return std::pair<int,int>(0,0);
	}

	int offset = (int)((time - m_beginTime) / m_minPeriod);
	offset = std::min(std::max(0, offset), (int)m_numIndexMap -1);
	int index = m_indexMap[ offset ];

	return getIndex(time, index);
}
std::pair<int,int> Collada::Internal::TransformAnimationChannel::getIndex(float time, int index) const
{
	do{
		if(time < m_times[index]){
			if(index == 0){
				return std::pair<int,int>(0,0);
			}else{
				index--;
			}
		}else if((index+1) >= (int)m_numData){
			return std::pair<int,int>(index,index);
		}else if(m_times[index+1] <= time){
			if(index+1 >= ((int)m_numData-1)){
				return std::pair<int,int>(m_numData-1,m_numData-1);
			}else{
				index++;
			}
		}else{
			return std::pair<int,int>(index,index+1);
		}

	}while(1);
/*
				if(time < m_times[0]){
					return std::pair<int,int>(0,0);
				}
				if(time > m_times[m_numData-1]){
					return std::pair<int,int>(m_numData-1,m_numData-1);
				}
				unsigned int index;
				for(index=1; index<m_numData; index++){
					if(m_times[index] > time){
						return std::pair<int,int>(index-1, index);
					}
				}
				//SCE_DBG_ASSERT(0);
				return std::pair<int,int>(0,0);
*/
}


void Collada::Internal::TransformAnimationChannel::finalize(void)
{
	if(m_indexMap != NULL){
		delete [] m_indexMap;
		m_indexMap = NULL;
	}

	if(m_times != NULL){
		delete [] m_times;
	}
	m_times = NULL;
	if(m_values != NULL){
		delete [] m_values;
	}
	m_values = NULL;
}
Collada::Internal::TransformAnimationChannel::~TransformAnimationChannel(void)
{
	finalize();
}

std::pair<float,float> Collada::Internal::TransformAnimationChannel::getTimeRange(void) const
{
	return std::pair<float,float>(m_beginTime, m_endTime);
}

float Collada::Internal::TransformAnimationChannel::getFloatValue(float time) const
{
	std::pair<int,int> range = getIndex(time);
	if(range.first == range.second){
		return m_values[range.first];
	}
	SCE_SAMPLE_UTIL_ASSERT(m_times[range.first] <= time);
	SCE_SAMPLE_UTIL_ASSERT(time <= m_times[range.second]);
	float time_range = m_times[range.second] - m_times[range.first];
	float diff = time - m_times[range.first];
	float s = diff / time_range;
	float t = 1 - s;
	return t * m_values[range.first] + s * m_values[range.second];

}
Vector3 Collada::Internal::TransformAnimationChannel::getVectorValue(float time) const
{
	std::pair<int,int> indices = getIndex(time);
	// TODO:should interpolate
	return Vector3(m_values[indices.first*3+0], m_values[indices.first*3+1], m_values[indices.first*3+2]);
	//return Vector3(m_values[0], m_values[1], m_values[2]);
}
Matrix4 Collada::Internal::TransformAnimationChannel::getMatrixValue(float time) const
{
	std::pair<int,int> indices = getIndex(time);
	// TODO:should interpolate
	float *matrix_p = &m_values[indices.first*16];
	return Internal::Util::fromFloatArray(matrix_p);
}

Collada::Internal::TransformAnimation::TransformAnimation(Dae::Animation::Animation *animation)
{
	m_numChannels = animation->m_channels.size();
	m_channels = new TransformAnimationChannel[m_numChannels];

	for(unsigned int i=0; i<m_numChannels; i++){ 
		Dae::Animation::Channel *channel = animation->m_channels.at(i);
		std::string source = channel->m_source;
		std::string targetStr = channel->m_target;

		Dae::Animation::Sampler *sampler = animation->findSampler(sourceToId(source));
		SCE_SAMPLE_UTIL_ASSERT(sampler != NULL);

		Dae::DataFlow::Input *inputInput = sampler->findInput(Dae::kSemanticInput);
		SCE_SAMPLE_UTIL_ASSERT(inputInput != NULL);
		Dae::DataFlow::Source *inputSource = animation->findSource(sourceToId(inputInput->m_source));
		SCE_SAMPLE_UTIL_ASSERT(inputSource != NULL);


		bool maybeTagetIsMatrix = false; // TODO
		Dae::DataFlow::Input *outputInput = sampler->findInput(Dae::kSemanticOutput);
		SCE_SAMPLE_UTIL_ASSERT(outputInput != NULL);
		Dae::DataFlow::Source *outputSource = animation->findSource(sourceToId(outputInput->m_source));
		SCE_SAMPLE_UTIL_ASSERT(outputSource != NULL);

		if(outputSource->m_techniqueCommon != NULL){
			if(outputSource->m_techniqueCommon->m_accessor->params.size() > 0){
				if(outputSource->m_techniqueCommon->m_accessor->params.at(0)->m_type == "float4x4"){
					maybeTagetIsMatrix = true;
					//printf(__FILE__":%d: maybe target is matrix\n", __LINE__);
				}
			}
		}

		m_channels[i].initialize(inputSource->m_floatArray->m_data, 
			                     inputSource->m_floatArray->m_count,
								 outputSource->m_floatArray->m_data, 
								 outputSource->m_floatArray->m_count,
								 targetStr, maybeTagetIsMatrix);

	}
}

std::pair<float,float> Collada::Internal::TransformAnimation::getTimeRange(void)
{
	std::pair<float,float> ret(FLT_MAX, FLT_MIN);
	for(unsigned int i=0; i<m_numChannels; i++){
		std::pair<float,float> range = m_channels[i].getTimeRange();
		ret = std::pair<float,float>(std::min(ret.first, range.first),
			                          std::max(ret.second, range.second));
	}
	return ret;
}

static ssgc::NodeTransform matrixToTransform(sce::Vectormath::Simd::Aos::Matrix4_arg m)
{
	ssgc::NodeTransform ret;
	vm::Matrix3 m3 = m.getUpper3x3();

	vm::Matrix3 scaleSqM = m3 * transpose(m3);
	vm::Vector3 scaleSq((float)scaleSqM[0][0], (float)scaleSqM[1][1], (float)scaleSqM[2][2]);
	ret.scale = sqrtPerElem(scaleSq);

	vm::Vector3 invScale = divPerElem(vm::Vector3(1,1,1), ret.scale);
	vm::Matrix3 rot = vm::Matrix3::scale(invScale) * m3;

	ret.rotation = vm::Quat(rot);
	ret.translation = m.getTranslation();

	return ret;
}

static sce::Vectormath::Simd::Aos::Matrix4 transformToMatrix(const ssgc::NodeTransform &transform)
{
	vm::Matrix3 sr = vm::Matrix3::scale(transform.scale) * vm::Matrix3::rotation(transform.rotation);
	return vm::Matrix4(sr , transform.translation);
}


ssgc::NodeAnimation::NodeAnimation(void)
{
	m_transforms = NULL;
}
ssgc::NodeAnimation::~NodeAnimation(void)
{
	finalize();
}
std::string ssgc::NodeAnimation::getNodeId(void) const
{
	return m_nodeId;
}

sce::Vectormath::Simd::Aos::Matrix4  ssgc::NodeAnimation::getMatrix(uint32_t frameIndex) const
{
	if(frameIndex >= m_numFrames){
		frameIndex = (m_numFrames == 0) ? 0 : (m_numFrames -1);
	}
	return transformToMatrix(m_transforms[frameIndex] );
}

uint32_t ssgc::NodeAnimation::getNumFrames(void) const
{
	return m_numFrames;
}

ssgc::NodeTransform ssgc::NodeAnimation::getNodeTransform(uint32_t frameIndex) const
{
	if(frameIndex >= m_numFrames){
		if(m_numFrames == 0){
			frameIndex = 0;
		}else{
			frameIndex = frameIndex % m_numFrames;
		}
	}
	return m_transforms[frameIndex];
}

int ssgc::NodeAnimation::initialize(std::string nodeId, uint32_t numFrames)
{
	m_nodeId = nodeId;
	m_numFrames = numFrames;
	m_transforms = new NodeTransform[numFrames];

	return SCE_OK;
}
int ssgc::NodeAnimation::finalize(void)
{
	if(m_transforms != NULL){
		delete [] m_transforms;
		m_transforms = NULL;
	}

	return SCE_OK;
}


ssgc::Animation::Animation(void)
{
	m_nodeAnimations = NULL;
	m_numNodeAnimations = 0;
	m_framePeriod  = 0.0f;
	m_numFrames = 0;
}
ssgc::Animation::~Animation(void)
{
	finalize();
}
int ssgc::Animation::initialize(ssgc::Internal::LibraryTransformAnimations *libraryAnimations, 
								VisualScene *visualScene, 
								float framePeriod)
{
	m_visualScene = visualScene;
	m_framePeriod = framePeriod;

	std::set<std::string> nodeNames;
	for(uint32_t i=0; i<libraryAnimations->m_numAnimations; i++){
		Internal::TransformAnimation *anim = libraryAnimations->m_animations[i];
		for(uint32_t c=0; c<anim->getNumChannels(); c++){
			const Internal::TransformAnimationChannel *channel = anim->getChannel(c);
			std::string nodeId = channel->getTarget().getTargetNodeId();
			nodeNames.insert(nodeId);
		}
	}

	std::vector<Internal::NodeInternal *> nodes;

	for(std::set<std::string>::iterator i=nodeNames.begin(); i != nodeNames.end(); i++){
		std::string nodeName = *i;
		//printf("animnode=%s\n", nodeName.c_str());
		Internal::NodeInternal *node = (Internal::NodeInternal *)visualScene->findNodeById(nodeName);
		if(node != NULL){
			//printf("found\n");
			nodes.push_back(node);
		}else{
			//printf("not found\n");
		}
	}

	std::pair<float,float> timeRange = libraryAnimations->getTimeRange();
	float startTime = timeRange.first;
	float endTime   = timeRange.second;

	m_startTime = startTime;
	m_endTime = endTime;

	float fNumFrames = (framePeriod == 0.0f) ? 0.0f : ((endTime - startTime) / framePeriod);
	uint32_t numFrames = (fNumFrames < 0.0f) ? 0 : (uint32_t)fNumFrames;
	if((startTime + numFrames*framePeriod) < endTime){
		numFrames++;
	}
	m_numFrames = numFrames;

	m_numNodeAnimations = nodes.size();
	m_nodeAnimations = new NodeAnimation[m_numNodeAnimations];
	for(uint32_t i=0; i<m_numNodeAnimations; i++){
		//printf("anim[%d] = %s\n", i, nodes.at(i)->getId().c_str());
		m_nodeAnimations[i].initialize(nodes.at(i)->getId(), numFrames);
	}

	Internal::TransFormAnimationPlayer transformAnimationPlayer;
	int ret = transformAnimationPlayer.initialize(libraryAnimations, (Internal::NodeInternal *)visualScene->getRootNode());
	SCE_SAMPLE_UTIL_ASSERT_EQUAL(ret, SCE_OK);
	(void)ret;

	for(uint32_t i=0; i<numFrames; i++){
		float time = startTime + i*framePeriod;

		transformAnimationPlayer.setTime(time);

		for(uint32_t j=0; j<m_numNodeAnimations; j++){
			sce::Vectormath::Simd::Aos::Matrix4 t = nodes.at(j)->getTransformMatrix();
			m_nodeAnimations[j].m_transforms[i] = matrixToTransform(t);
		}

	}
	transformAnimationPlayer.finalize();

	return SCE_OK;
}
				

int ssgc::Animation::finalize(void)
{

	if(m_nodeAnimations != NULL){
		delete[] m_nodeAnimations;
		m_nodeAnimations = NULL;
	}
	return SCE_OK;
}


float ssgc::Animation::getStartTime(void) const
{
	return m_startTime;
}


float ssgc::Animation::getEndTime(void) const
{
	return m_endTime;
}

uint32_t ssgc::Animation::getNumFrames(void) const
{
	return m_numFrames;
}




ssgci::AnimationPlayerImpl::AnimationPlayerImpl(void)
{
	m_nodeAnimationInstance = NULL;
	m_numNodeAnimationInstance = 0;
	m_animation = NULL;
	m_instanceVisualScene = NULL;

}

ssgci::AnimationPlayerImpl::~AnimationPlayerImpl(void)
{
	finalize();
}




int ssgci::AnimationPlayerImpl::initialize(ssgc::InstanceVisualScene *instanceVisualScene, const ssgc::Animation *animation)
{
	m_instanceVisualScene = instanceVisualScene;

	m_animation = animation;
	m_nodeAnimationInstance = new NodeAnimationInstance[animation->m_numNodeAnimations];
	m_numNodeAnimationInstance = animation->m_numNodeAnimations;
	for(uint32_t i=0; i<m_numNodeAnimationInstance; i++){
		m_nodeAnimationInstance[i].m_node = m_instanceVisualScene->getRootNode()->findNodeById(animation->m_nodeAnimations[i].getNodeId() );
		m_nodeAnimationInstance[i].m_nodeAnimation = &animation->m_nodeAnimations[i];
	}

	return SCE_OK;
}




int ssgci::AnimationPlayerImpl::finalize(void)
{
	if(m_nodeAnimationInstance != NULL){
		delete [] m_nodeAnimationInstance;
		m_nodeAnimationInstance = NULL;
	}
	m_numNodeAnimationInstance = 0;
	return SCE_OK;
}


void ssgci::AnimationPlayerImpl::setTime(float time) const
{
	if(time < m_animation->m_startTime){
		time = m_animation->m_startTime;
	}
	if(time > m_animation->m_endTime){
		time = m_animation->m_endTime;
	}
	float fFrameIndex = (m_animation->m_framePeriod == 0.0f) ? 0.0f : ((time - m_animation->m_startTime) / m_animation->m_framePeriod);
	uint32_t frameIndex = (fFrameIndex < 0.0f) ? 0 : (uint32_t)fFrameIndex;
	//printf("frameIndex=%d\n", frameIndex);

	for(uint32_t i=0; i<m_numNodeAnimationInstance; i++){
		Node *node = m_nodeAnimationInstance[i].m_node;
		if(node != NULL){
			sce::Vectormath::Simd::Aos::Matrix4 m = m_nodeAnimationInstance[i].m_nodeAnimation->getMatrix(frameIndex);
			node->setTransformMatrix(m);
		}
	}
	m_instanceVisualScene->getRootNode()->updateWorldMatrix();
	m_instanceVisualScene->getRootNode()->updateSkinMatrices();
}


void ssgci::AnimationPlayerImpl::setFrame(uint32_t frame) const
{
	uint32_t numFrames = getAnimation()->getNumFrames();
	uint32_t frameIndex = frame;
	if(numFrames <=frameIndex){
		frameIndex = (numFrames >0) ? (numFrames - 1) : 0;
	}

	for(uint32_t i=0; i<m_numNodeAnimationInstance; i++){
		Node *node = m_nodeAnimationInstance[i].m_node;
		if(node != NULL){
			sce::Vectormath::Simd::Aos::Matrix4 m = m_nodeAnimationInstance[i].m_nodeAnimation->getMatrix(frameIndex);
			node->setTransformMatrix(m);
		}
	}
	m_instanceVisualScene->getRootNode()->updateWorldMatrix();
	m_instanceVisualScene->getRootNode()->updateSkinMatrices();
}


const ssgc::Animation* ssgci::AnimationPlayerImpl::getAnimation(void) const
{
	return m_animation;
}


int ssgc::createAnimationPlayer(AnimationPlayer **outAnimationPlayer, 
								InstanceVisualScene *instanceVisualScene, 
								const Animation *animation)
{
	if(outAnimationPlayer == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(instanceVisualScene == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}
	if(animation == NULL){
		return SCE_SAMPLE_UTIL_ERROR_NULL_POINTER;
	}

	ssgci::AnimationPlayerImpl* impl = new ssgci::AnimationPlayerImpl();
	int ret = impl->initialize(instanceVisualScene, animation);
	if(ret != SCE_OK){
		delete impl;
		return ret;
	}
	*outAnimationPlayer = impl;
	return SCE_OK;
}
