/* SIE CONFIDENTIAL
 * PlayStation(R)4 Programmer Tool Runtime Library Release 04.508.001
 * Copyright (C) 2013 Sony Interactive Entertainment Inc. 
 * All Rights Reserved.
 */

#ifndef _SCE_SAMPLE_UTIL_AUDIO_INTERNAL_H
#define _SCE_SAMPLE_UTIL_AUDIO_INTERNAL_H

#include "util/util.h"
#include "sampleutil_internal.h"

namespace sce
{
	namespace SampleUtil
	{
		namespace Audio
		{
			namespace Internal
			{
				namespace Data
				{
					enum Codec
					{
						kCodecUnknown = 0x0,
						kCodecPcm     = 0x1,
						kCodecAdpcm   = 0x2,
						kCodecAtrac9  = 0x3
					};



					class VagFileInfo
					{
					public:
						struct VagHeader
						{
							static const uint32_t HEADER_SIZE_IN_FILE=48;
							char ID[4];
							uint32_t version;
							//uint8_t reserved[4];
							uint32_t dataSize;
							uint32_t sampleRate;
							//uint8_t reserved2[10];
							uint8_t numChannels;
							//uint8_t reserved;
							char    name[16+1];

							void dump(void);
						};


					private:
						VagHeader m_header;
					public:
						int parse(sce::SampleUtil::Internal::Util::Reader *reader);
						const VagHeader* getHeader(void) const
						{
							return &m_header;
						}

						uint32_t getDataOffset(void) const
						{
							return VagHeader::HEADER_SIZE_IN_FILE;
						}
						uint32_t getDataSize(void) const
						{
							return m_header.dataSize;
						}
					};
					class WavFileInfo
					{
					public:

						/* PCM */
						static const uint32_t kWaveFormatPcm             = (0x0001);
						static const uint32_t kWaveFormatAdpcm           = (0x0002);
						/* ATRAC9 */
						static const uint32_t kWaveFormatExtensible      = (0xFFFE);

						static const uint32_t kAt9FactChunkLength	       = (12);

						static const uint8_t kSubformatPcm[16];
						static const uint8_t kSubformatAdpcm[16];
						static const uint8_t kSubformatIeeeFloat[16];
						static const uint8_t kSubformatAtrac9[16];
						

						struct FormatChunk
						{
							uint16_t formatTag;
							uint16_t numChannels;
							uint32_t sampleRate;
							uint32_t dataPerSec;
							uint16_t blockSize;
							uint16_t bitPerSample;
							uint16_t extDataSize;

							struct {
								uint16_t samplesPerBlock;
								uint32_t channelMask;
								uint8_t    subFormat[16 + 1];
								uint32_t versionInfo; //atrac9 only
								uint32_t configData;  //atrac9 only
								//uint32_t reserved; 
							} extData;

							void dump(void) const;
							bool isAtrac9(void) const;
							
						};

						struct FactChunk
						{
							uint32_t totalSamples;
							uint32_t inputAndOverlapDelaySamples;
							uint32_t encoderDelaySamples;
							void dump(void) const;

						};


						struct SampleLoop 
						{
							uint32_t identifier;
							uint32_t type;
							uint32_t start;
							uint32_t end;
							uint32_t fraction;
							uint32_t playCount;
							void dump(void) const;
						} ;

						struct SmplChunk 
						{
							uint32_t manufacturer;
							uint32_t product;
							uint32_t samplePeriod;
							uint32_t midiUnityNote;
							uint32_t midiPitchFraction;
							uint32_t smpteFormat;
							uint32_t smpteOffset;
							uint32_t numSampleLoops;
							uint32_t samplerData;
							SampleLoop *sampleLoops;

							SmplChunk(void);

							~SmplChunk(void);

							void dump(void) const;
						} ;



					private:
						int checkWav(void);

						int seekChunk(const char *chunkName, uint32_t *pChunkOffset, uint32_t *pSizeofChunk);

						int parseFormatChunk(void);

						int parseFactChunk(void);

						int parseSampleLoop(SampleLoop *loop);

						int parseSmplChunk(void);

						sce::SampleUtil::Internal::Util::Reader *m_reader;
						uint32_t m_riffDataSize;

						FormatChunk m_formatChunk;
						FactChunk m_factChunk;
						SmplChunk m_smplChunk;

						uint32_t m_offsetFmtChunk;
						uint32_t m_sizeofFmtChunk;
						uint32_t m_offsetDataChunk;
						uint32_t m_sizeofDataChunk;
						uint32_t m_offsetFactChunk;
						uint32_t m_sizeofFactChunk;
						uint32_t m_offsetSmplChunk;
						uint32_t m_sizeofSmplChunk;



					public:

						WavFileInfo(void);

						int parse(sce::SampleUtil::Internal::Util::Reader *reader);

						const FormatChunk* getFormatChunk(void) const;

						const FactChunk *getFactChunk(void) const;

						const SmplChunk *getSmplChunk(void) const ;

						void dump(void) const;

						uint32_t getDataChunkOffset(void) const;

						uint32_t getDataChunkSize(void) const;
						//----------------------------------------------
						Codec getCodec(void) const;
						uint32_t getNumChannels(void) const
						{
							return m_formatChunk.numChannels;
						}
						uint32_t getSampleRate(void) const
						{
							return m_formatChunk.sampleRate;
						}
						uint32_t getBitPerSample(void) const
						{
							return m_formatChunk.bitPerSample;
						}
						uint32_t getBlockAlign(void) const
						{
							return m_formatChunk.bitPerSample / 8 * m_formatChunk.numChannels;
						}
						uint32_t getAverageBytesPerSec(void) const
						{
							return getSampleRate() * getBlockAlign();
						}
						uint32_t getDataOffset(void) const
						{
							return getDataChunkOffset();
						}
						uint32_t getDataSize(void) const
						{
							return getDataChunkSize();

						}
					};

					class AudioFile
					{
					public:
						Codec m_codec;
						uint32_t m_numChannels;
						uint32_t m_sampleRate;

						uint32_t m_averageBytesPerSec; 
						uint32_t m_blockAlign;       
						uint32_t m_bitsPerSample;
						uint32_t m_dataOffset;
						uint32_t m_dataSize;

                        uint32_t m_loopStartOffset;
                        uint32_t m_loopEndOffset;

						bool m_hasExFormat;
						uint16_t m_samplesPerBlock;
						uint32_t m_channelMask;
						uint8_t  m_subFormat[16];

                        int32_t m_at9configData;
					public:
                        AudioFile(void)
{
						m_codec = kCodecUnknown;
m_numChannels = 0;
m_sampleRate = 0;

m_averageBytesPerSec = 0;
m_blockAlign = 0;
m_bitsPerSample = 0;
m_dataOffset = 0;
m_dataSize = 0;

m_loopStartOffset = 0;
m_loopEndOffset = 0;

m_hasExFormat = false;
m_samplesPerBlock = 0;
m_channelMask = 0;
memset(m_subFormat, 0, sizeof(m_subFormat));
m_at9configData = 0;
}
						int parse(sce::SampleUtil::Internal::Util::Reader *reader);
					};
				}
			}
		}
	}
}



#endif /* _SCE_SAMPLE_UTIL_AUDIO_INTERNAL_H */


