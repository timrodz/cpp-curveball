/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#include "audio_internal.h"

namespace ssaid = sce::SampleUtil::Audio::Internal::Data;

const uint8_t ssaid::WavFileInfo::kSubformatPcm[16]       = { 0x1,  0x0,  0x0,  0x0,  0x0,  0x0,  0x10, 0x0,  0x80, 0x0,  0x0,  0xaa, 0x0,  0x38, 0x9b, 0x71 };
const uint8_t ssaid::WavFileInfo::kSubformatAdpcm[16]     = { 0x2,  0x0,  0x0,  0x0,  0x0,  0x0,  0x10, 0x0,  0x80, 0x0,  0x0,  0xaa, 0x0,  0x38, 0x9b, 0x71 };
const uint8_t ssaid::WavFileInfo::kSubformatIeeeFloat[16] = { 0x3,  0x0,  0x0,  0x0,  0x0,  0x0,  0x10, 0x0,  0x80, 0x0,  0x0,  0xaa, 0x0,  0x38, 0x9b, 0x71 };
const uint8_t ssaid::WavFileInfo::kSubformatAtrac9[16]    = { 0xd2, 0x42, 0xe1, 0x47, 0xba, 0x36, 0x8d, 0x4d, 0x88, 0xfc, 0x61, 0x65, 0x4f, 0x8c, 0x83, 0x6c };

void ssaid::VagFileInfo::VagHeader::dump(void)
{
	printf("ID          : %c%c%c%c\n", ID[0],ID[1],ID[2],ID[3]);
	printf("version     : %d(%#x)\n", version, version);
	printf("dataSize    : %d(%#x)\n", dataSize, dataSize);
	printf("sampleRate  : %d\n",      sampleRate);
	printf("numChannels : %d\n",      numChannels);
	printf("Name        : %s\n",      name);
}


int ssaid::VagFileInfo::parse(sce::SampleUtil::Internal::Util::Reader *reader)
{
	int ret;
	reader->setEndian(sce::SampleUtil::Internal::Util::Reader::ENDIAN_BIG);
	reader->seek(0);
	
	//E Check the file size and allocate the memory
	//J ファイルサイズの確認とメモリ確保
	if(reader->getSize() <= 48){
		return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
	}
	
	ret = reader->read(m_header.ID, 4);
	if(ret != SCE_OK){ 
		return ret;
	}

	if((m_header.ID[0] != 'V')
		|| (m_header.ID[1] != 'A')
		|| (m_header.ID[2] != 'G')){
			return -1;
	}
	
	reader->skip(4); //reserved;
	
	ret = reader->readUint32(&m_header.version);
	if(ret != SCE_OK){ return ret;	}
	
	ret = reader->readUint32(&m_header.dataSize);
	if(ret != SCE_OK){ return ret;	}
	
	ret = reader->readUint32(&m_header.sampleRate);
	if(ret != SCE_OK){ return ret;	}
	
	reader->skip(10); //reserved;
	
	ret = reader->readUint8(&m_header.numChannels);
	if(ret != SCE_OK){ return ret;	}
	if(m_header.numChannels == 0){
		m_header.numChannels = 1;
	}
	
	reader->skip(1); //reserved;
	
	ret = reader->read(m_header.name, 16);
	if(ret != SCE_OK){ return ret;	}
	m_header.name[16] = 0;
	
	SCE_SAMPLE_UTIL_ASSERT(reader->tell() == 48);

	//m_header.dump();
	return SCE_OK;
}

/* ----------------------------------------------------------------------- */


void ssaid::WavFileInfo::FormatChunk::dump(void) const
{
	printf("   [Format Chunk]\n");
	printf("      formatTag    = %#x\n", formatTag);
	printf("      numChannels  = %d\n", numChannels);
	printf("      sampleRate   = %d\n", sampleRate);
	printf("      dataPerSec   = %d\n", dataPerSec);
	printf("      blockSize    = %d\n", blockSize);
	printf("      bitPerSample = %d\n", bitPerSample);
	printf("      extDataSize  = %#x\n", extDataSize);
	printf("      extData:\n");
	printf("         samplesPerBlock = %#x\n", extData.samplesPerBlock);
	printf("         channelMask     = %#x\n", extData.channelMask);
	printf("         subFormat       = {%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x }\n",
		extData.subFormat[0], extData.subFormat[1], extData.subFormat[2], extData.subFormat[3],
		extData.subFormat[4], extData.subFormat[5], extData.subFormat[6], extData.subFormat[7],
		extData.subFormat[8], extData.subFormat[9], extData.subFormat[10], extData.subFormat[11],
		extData.subFormat[12], extData.subFormat[13], extData.subFormat[14], extData.subFormat[15]
	);
	printf("         versionInfo     = %#x\n", extData.versionInfo);
	printf("         configData      = %#x\n", extData.configData);
	
}



void ssaid::WavFileInfo::FactChunk::dump(void) const
{
	printf("   [Fact Chunk]\n");
	printf("      totalSamples                = %d\n", totalSamples);
	printf("      inputAndOverlapDelaySamples = %d\n", inputAndOverlapDelaySamples);
	printf("      encoderDelaySamples         = %d\n", encoderDelaySamples);

}

void ssaid::WavFileInfo::SampleLoop::dump(void) const
{
	printf("      [SampleLoop]\n");
	printf("          identifier  = %d\n", identifier);
	printf("          type        = %d\n", type);
	printf("          start       = %d\n", start);
	printf("          end         = %d\n", end);
	printf("          fraction    = %d\n", fraction);
	printf("          playCount   = %d\n", playCount);
}


ssaid::WavFileInfo::SmplChunk::SmplChunk(void)
{
	sampleLoops = NULL;
}

ssaid::WavFileInfo::SmplChunk::~SmplChunk(void)
{
	if(sampleLoops != NULL){
		delete [] sampleLoops;
	}
	sampleLoops = NULL;
}

void ssaid::WavFileInfo::SmplChunk::dump(void) const
{
	printf("   [Smpl Chunk]\n");
	printf("      manufacturer      = %d\n", manufacturer);
	printf("      product           = %d\n", product);
	printf("      samplePeriod      = %d\n", samplePeriod);
	printf("      midiUnityNote     = %d\n", midiUnityNote);
	printf("      midiPitchFraction = %d\n", midiPitchFraction);
	printf("      smpteFormat       = %d\n", smpteFormat);
	printf("      smpteOffset       = %d\n", smpteOffset);
	printf("      numSampleLoops    = %d\n", numSampleLoops);
	printf("      samplerData       = %d\n", samplerData);
	for(uint32_t i=0; i<numSampleLoops; i++){
		sampleLoops[i].dump();
	}
}

int ssaid::WavFileInfo::checkWav(void)
{
	m_reader->seek(0);
	
	char riffMagic[4];
	int ret = m_reader->read(riffMagic, 4);
	if(ret != SCE_OK){return ret;}
	if(memcmp(riffMagic, "RIFF", 4) != 0){
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}

	ret = m_reader->readUint32(&m_riffDataSize);
	if(ret != SCE_OK){return ret;}
	
	char wavMagic[4];
	ret = m_reader->read(wavMagic, 4);
	if(ret != SCE_OK){return ret;}
	if(memcmp(wavMagic, "WAVE", 4) != 0){
		return SCE_SAMPLE_UTIL_ERROR_UNSUPPORTED_FILE_FORMAT;
	}
	
	return SCE_OK;
}

int ssaid::WavFileInfo::seekChunk(const char *chunkName, uint32_t *pChunkOffset, uint32_t *pSizeofChunk)
{
	int ret = m_reader->seek(12);
	if(ret < SCE_OK){
		return ret;
	}
	while(1){
		if(m_reader->tell() >= (m_riffDataSize + 8)){
			return SCE_SAMPLE_UTIL_ERROR_FILE_LOAD;
		}
		
		char chunkMagic[4];
		uint32_t sizeChunk;
		ret = m_reader->read(chunkMagic, 4);
		if(ret != SCE_OK){
			return ret;
		}
		ret = m_reader->readUint32(&sizeChunk);
		if(ret != SCE_OK){
			return ret;
		}
		//sce::SampleUtil::Impl::debugPrintfln("chunk %c%c%c%c, size=%d\n", chunkMagic[0],chunkMagic[1],chunkMagic[2],chunkMagic[3], sizeChunk);
		
		if(memcmp(chunkName, chunkMagic, 4) == 0){
			*pChunkOffset = m_reader->tell();
			*pSizeofChunk = sizeChunk;
			return ret;
		}
		
		ret = m_reader->skip(sizeChunk);
		if(ret != SCE_OK){
			return 	ret;
		}
	}
	//SCE_SAMPLE_UTIL_ASSERT_EQUAL(0);
}


bool ssaid::WavFileInfo::FormatChunk::isAtrac9(void) const
{

	return (formatTag == ssaid::WavFileInfo::kWaveFormatExtensible)
		&& (memcmp(extData.subFormat, kSubformatAtrac9, 16) == 0);
		
}

int ssaid::WavFileInfo::parseFormatChunk(void)
{
	int ret;
	memset(&m_formatChunk, 0, sizeof(m_formatChunk));
	ret = m_reader->readUint16(&m_formatChunk.formatTag);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint16(&m_formatChunk.numChannels);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_formatChunk.sampleRate);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_formatChunk.dataPerSec);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint16(&m_formatChunk.blockSize);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint16(&m_formatChunk.bitPerSample);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint16(&m_formatChunk.extDataSize);
	if(ret != SCE_OK){ return ret; }

	if(m_formatChunk.formatTag == kWaveFormatExtensible){
		
		ret = m_reader->readUint16(&m_formatChunk.extData.samplesPerBlock);
		if(ret != SCE_OK){ return ret; }
		ret = m_reader->readUint32(&m_formatChunk.extData.channelMask);
		if(ret != SCE_OK){ return ret; }
		ret = m_reader->read(&m_formatChunk.extData.subFormat, 16);
		if(ret != SCE_OK){ return ret; }
		m_formatChunk.extData.subFormat[16] = 0;

		if(m_formatChunk.isAtrac9()){
			ret = m_reader->readUint32(&m_formatChunk.extData.versionInfo);
			if(ret != SCE_OK){ return ret; }
			ret = m_reader->readUint32(&m_formatChunk.extData.configData);
			if(ret != SCE_OK){ return ret; }
		}
	}
	
	return SCE_OK;
}



int ssaid::WavFileInfo::parseFactChunk(void)
{
	int ret;
	memset(&m_factChunk, 0, sizeof(m_factChunk));
	ret = m_reader->readUint32(&m_factChunk.totalSamples);
	if(ret != SCE_OK){ return ret; }
	if(m_formatChunk.isAtrac9()){
		ret = m_reader->readUint32(&m_factChunk.inputAndOverlapDelaySamples);
		if(ret != SCE_OK){ return ret; }
		ret = m_reader->readUint32(&m_factChunk.encoderDelaySamples);
		if(ret != SCE_OK){ return ret; }
	}

	return SCE_OK;
}


int ssaid::WavFileInfo::parseSampleLoop(SampleLoop *loop)
{
	int ret;
	memset(loop, 0, sizeof(SampleLoop));
	ret = m_reader->readUint32(&loop->identifier);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&loop->type);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&loop->start);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&loop->end);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&loop->fraction);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&loop->playCount);
	if(ret != SCE_OK){ return ret; }
	return SCE_OK;

}
int ssaid::WavFileInfo::parseSmplChunk(void)
{
	int ret;
	memset(&m_smplChunk, 0, sizeof(m_smplChunk));
	ret = m_reader->readUint32(&m_smplChunk.manufacturer);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_smplChunk.product);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_smplChunk.samplePeriod);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_smplChunk.midiUnityNote);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_smplChunk.midiPitchFraction);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_smplChunk.smpteFormat);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_smplChunk.smpteOffset);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_smplChunk.numSampleLoops);
	if(ret != SCE_OK){ return ret; }
	ret = m_reader->readUint32(&m_smplChunk.samplerData);
	if(ret != SCE_OK){ return ret; }

	m_smplChunk.sampleLoops = new SampleLoop[m_smplChunk.numSampleLoops];
	for(uint32_t i=0; i<m_smplChunk.numSampleLoops; i++){
		ret = parseSampleLoop(&m_smplChunk.sampleLoops[i]);
		if(ret != SCE_OK){ return ret; }
	}

	return SCE_OK;
}


ssaid::WavFileInfo::WavFileInfo(void)
{
	m_reader = NULL;		
}

int ssaid::WavFileInfo::parse(sce::SampleUtil::Internal::Util::Reader *reader)
{
	reader->setEndian(sce::SampleUtil::Internal::Util::Reader::ENDIAN_LITTLE);

	int ret;
	m_reader = reader;

	ret = checkWav();
	if(ret != SCE_OK){
		return ret;
	}

	m_offsetFmtChunk = 0;
	m_sizeofFmtChunk = 0;
	m_offsetDataChunk = 0;
	m_sizeofDataChunk = 0;
	m_offsetFactChunk = 0;
	m_sizeofFactChunk = 0;
	m_offsetSmplChunk = 0;
	m_sizeofSmplChunk = 0;

	ret = seekChunk("fmt ", &m_offsetFmtChunk, &m_sizeofFmtChunk);
	if(ret != SCE_OK){
		return ret;
	}

	ret = parseFormatChunk();
	if(ret != SCE_OK){
		return ret;
	}

	ret = seekChunk("data", &m_offsetDataChunk, &m_sizeofDataChunk);
	if(ret != SCE_OK){
		return ret;
	}
	ret = seekChunk("fact", &m_offsetFactChunk, &m_sizeofFactChunk);
	if(ret == SCE_OK){
		ret = parseFactChunk();
		if(ret != SCE_OK){
			return ret;
		}
	}

	ret = seekChunk("smpl", &m_offsetSmplChunk, &m_sizeofSmplChunk);
	if(ret == SCE_OK){
		ret = parseSmplChunk();
		if(ret != SCE_OK){
			return ret;
		}
	}
	return SCE_OK;
}




const ssaid::WavFileInfo::FormatChunk* ssaid::WavFileInfo::getFormatChunk(void) const
{
	if(m_sizeofFmtChunk ==0){
		return NULL;
	}
	return &m_formatChunk;
}
const ssaid::WavFileInfo::FactChunk *ssaid::WavFileInfo::getFactChunk(void) const
{
	if(m_sizeofFactChunk == 0){
		return NULL;
	}
	return &m_factChunk;
}
const ssaid::WavFileInfo::SmplChunk *ssaid::WavFileInfo::getSmplChunk(void) const 
{
	if(m_sizeofSmplChunk == 0){
		return NULL;
	}
	return &m_smplChunk;
}

void ssaid::WavFileInfo::dump(void) const
{
	printf("[Dump Wav]\n");
	if(getFormatChunk() != NULL){
		getFormatChunk()->dump();
	}
	if(getFactChunk()  != NULL){
		getFactChunk()->dump();
	}
	if(getSmplChunk()  != NULL){
		getSmplChunk()->dump();
	}
}

uint32_t ssaid::WavFileInfo::getDataChunkOffset(void) const
{
	return m_offsetDataChunk;
}
uint32_t ssaid::WavFileInfo::getDataChunkSize(void) const
{
	return m_sizeofDataChunk;		
}

//------------------------------
ssaid::Codec ssaid::WavFileInfo::getCodec(void) const
{
	if(m_formatChunk.formatTag == kWaveFormatPcm){
		return kCodecPcm;
	}
	if(m_formatChunk.formatTag == kWaveFormatAdpcm){
		return kCodecAdpcm;
	}
	if(m_formatChunk.formatTag == kWaveFormatExtensible){
		if(memcmp(m_formatChunk.extData.subFormat, kSubformatPcm, 16) == 0){
			return kCodecPcm;
		}
		if(memcmp(m_formatChunk.extData.subFormat, kSubformatAdpcm, 16) == 0){
			return kCodecAdpcm;
		}
		if(memcmp(m_formatChunk.extData.subFormat, kSubformatAtrac9, 16) == 0){
			return kCodecAtrac9;
		}
	}
	return kCodecUnknown;
}


//------------------------------

int ssaid::AudioFile::parse(sce::SampleUtil::Internal::Util::Reader *reader)
{
	int ret;

	VagFileInfo vagFileInfo;
	ret = vagFileInfo.parse(reader);
	if(ret == SCE_OK){ // the file is vag file.
		this->m_codec = kCodecAdpcm;
		this->m_numChannels = vagFileInfo.getHeader()->numChannels;
		this->m_sampleRate = vagFileInfo.getHeader()->sampleRate;
		this->m_bitsPerSample = 16;
		this->m_blockAlign = this->m_numChannels * this->m_bitsPerSample / 8;
		this->m_averageBytesPerSec = this->m_sampleRate * this->m_blockAlign;
		this->m_dataOffset = vagFileInfo.getDataOffset();
		this->m_dataSize   = vagFileInfo.getDataSize();

		this->m_loopStartOffset = this->m_dataOffset;
		this->m_loopEndOffset = this->m_dataOffset + this->m_dataSize;

		this->m_hasExFormat = false;
		this->m_samplesPerBlock = 0;
		this->m_channelMask = 0;
		memset(this->m_subFormat, 0, 16);
		this->m_at9configData = 0;
		return SCE_OK;
	}else{

		WavFileInfo wavFileInfo;
		ret = wavFileInfo.parse(reader);
		if(ret < 0){
			return ret;
		}
		if(wavFileInfo.getCodec() == ssaid::kCodecUnknown){
			return -1;
		}

		this->m_codec              = wavFileInfo.getCodec();
		this->m_numChannels        = wavFileInfo.getNumChannels();
		this->m_sampleRate         = wavFileInfo.getSampleRate();
		this->m_bitsPerSample      = wavFileInfo.getBitPerSample();
		this->m_blockAlign         = wavFileInfo.getBlockAlign();
		this->m_averageBytesPerSec = wavFileInfo.getAverageBytesPerSec();
		this->m_dataOffset         = wavFileInfo.getDataOffset();
		this->m_dataSize           = wavFileInfo.getDataSize();

		this->m_loopStartOffset = this->m_dataOffset;
		this->m_loopEndOffset = this->m_dataOffset + this->m_dataSize;
		const WavFileInfo::SmplChunk *smplChunk = wavFileInfo.getSmplChunk();
		if((smplChunk != NULL) && (smplChunk->numSampleLoops > 0)){
			this->m_loopStartOffset = smplChunk->sampleLoops[0].start;
			this->m_loopEndOffset   = smplChunk->sampleLoops[0].end;
		}

		this->m_hasExFormat = wavFileInfo.getFormatChunk()->formatTag ==  ssaid::WavFileInfo::kWaveFormatExtensible;

		if (!this->m_hasExFormat){
			const WavFileInfo::SmplChunk *smplChunk = wavFileInfo.getSmplChunk();
			if ((smplChunk != NULL) && (smplChunk->numSampleLoops > 0)){
				this->m_loopStartOffset = smplChunk->sampleLoops[0].start;
				this->m_loopEndOffset = smplChunk->sampleLoops[0].end;
			}
		}

		if(this->m_hasExFormat){
			this->m_samplesPerBlock = wavFileInfo.getFormatChunk()->extData.samplesPerBlock;
			this->m_channelMask =   wavFileInfo.getFormatChunk()->extData.channelMask;
			memcpy(this->m_subFormat, wavFileInfo.getFormatChunk()->extData.subFormat, 16);
			this->m_at9configData = (int32_t)wavFileInfo.getFormatChunk()->extData.configData;
		}else{
			this->m_samplesPerBlock = 0;
			this->m_channelMask = 0;
			memset(this->m_subFormat, 0, 16);
			this->m_at9configData = 0;
		}

		return SCE_OK;
	}

}