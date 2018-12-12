#include "atrac9audiostream.h"
#include "../log.h"
#include "../util.h"

using namespace Impacto::Io;

namespace Impacto {
namespace Audio {

struct At9ContainerInfo {
  int ChannelCount;
  int SampleRate;
  int SampleCount;
  int EncoderDelay;
  bool HasLoop;
  int LoopStart;
  int LoopEnd;
  uint8_t ConfigData[4];

  int StreamDataOffset;
};

static const uint32_t RIFFMagic = 0x52494646u;
static const uint32_t WAVEMagic = 0x57415645u;
static const uint32_t fmtMagic = 0x666d7420u;
static const uint32_t factMagic = 0x66616374u;
static const uint32_t smplMagic = 0x736d706cu;
static const uint32_t dataMagic = 0x64617461u;

static const uint16_t WaveFormatExtensible = 0xFFFE;
static const uint8_t Atrac9Guid[] = {0xD2, 0x42, 0xE1, 0x47, 0xBA, 0x36,
                                     0x8D, 0x4D, 0x88, 0xFC, 0x61, 0x65,
                                     0x4F, 0x8C, 0x83, 0x6C};

// http://www.piclist.com/techref/io/serial/midi/wave.html
static bool ParseAt9Riff(InputStream* stream, At9ContainerInfo* info) {
  if (ReadBE<uint32_t>(stream) != RIFFMagic) return false;
  uint32_t remaining = ReadLE<uint32_t>(stream) - 4;
  if (ReadBE<uint32_t>(stream) != WAVEMagic) return false;

  bool gotFmt = false;
  bool gotFact = false;

  while (remaining) {
    uint32_t chunkId = ReadBE<uint32_t>(stream);
    uint32_t chunkSize = ReadLE<uint32_t>(stream);
    remaining -= 8;
    remaining -= chunkSize;
    switch (chunkId) {
      case fmtMagic: {
        if (gotFmt) {
          ImpLog(LL_Error, LC_Audio,
                 "Encountered ATRAC9 file with more than one fmt chunk\n");
          return false;
        }
        gotFmt = true;
        if (chunkSize != 52) return false;
        uint8_t fmtData[52];
        stream->Read(fmtData, 52);
        if (SDL_SwapLE16(*(uint16_t*)fmtData) != WaveFormatExtensible)
          return false;
        if (memcmp(fmtData + 24, Atrac9Guid, sizeof(Atrac9Guid)) != 0)
          return false;

        // yep, it's an ATRAC9 file

        info->ChannelCount = SDL_SwapLE16(*(uint16_t*)(fmtData + 2));
        if (info->ChannelCount != 1 && info->ChannelCount != 2) {
          ImpLog(LL_Error, LC_Audio,
                 "Encountered ATRAC9 file with unsupported channel count %d\n",
                 info->ChannelCount);
          return false;
        }

        info->SampleRate = SDL_SwapLE32(*(uint32_t*)(fmtData + 4));
        memcpy(info->ConfigData, fmtData + 44, 4);

        break;
      }
      case factMagic: {
        if (gotFact) {
          ImpLog(LL_Error, LC_Audio,
                 "Encountered ATRAC9 file with more than one fact chunk\n");
          return false;
        }
        gotFact = true;
        if (chunkSize != 12) return false;
        uint8_t factData[12];
        stream->Read(factData, 12);

        info->SampleCount = SDL_SwapLE32(*(uint32_t*)(factData));
        info->EncoderDelay = SDL_SwapLE32(*(uint32_t*)(factData + 8));

        break;
      }
      case smplMagic: {
        if (chunkSize < 36) {
          stream->Seek(chunkSize, RW_SEEK_CUR);
          break;
        }
        uint8_t smplData[36];
        stream->Read(smplData, 36);
        int loopCount = SDL_SwapLE32(*(uint32_t*)(smplData + 28));
        info->HasLoop = loopCount > 0;
        if (info->HasLoop) {
          uint8_t loopChunk[24];
          stream->Read(loopChunk, 24);
          info->LoopStart = SDL_SwapLE32(*(uint32_t*)(loopChunk + 8));
          info->LoopEnd = SDL_SwapLE32(*(uint32_t*)(loopChunk + 12));
        }
        // skip further loop chunks
        if (loopCount > 1) stream->Seek(24 * (loopCount - 1), RW_SEEK_CUR);
        break;
      }
      case dataMagic: {
        info->StreamDataOffset = stream->Position;
        goto breakLoop;
      }
      default: {
        stream->Seek(chunkSize, RW_SEEK_CUR);
        break;
      }
    }
  }

breakLoop:
  if (!gotFmt || !gotFact) {
    ImpLog(LL_Error, LC_Audio, "Encountered incomplete ATRAC9 file\n");
    return false;
  }
  return true;
}

AudioStream* Atrac9AudioStream::Create(InputStream* stream) {
  Atrac9AudioStream* result = 0;

  At9ContainerInfo container;
  int ret;
  Atrac9CodecInfo codecinfo;

  void* At9 = Atrac9GetHandle();
  if (!At9) {
    ImpLog(LL_Error, LC_Audio, "Atrac9GetHandle failed\n");
    goto fail;
  }

  container = {0};
  if (!ParseAt9Riff(stream, &container)) {
    goto fail;
  }

  ret = Atrac9InitDecoder(At9, container.ConfigData);
  if (ret != 0) {
    ImpLog(LL_Error, LC_Audio, "Atrac9InitDecoder failed with %d\n", ret);
    goto fail;
  }

  Atrac9GetCodecInfo(At9, &codecinfo);
  if (codecinfo.channels != container.ChannelCount ||
      codecinfo.samplingRate != container.SampleRate) {
    ImpLog(LL_Error, LC_Audio, "Atrac9CodecInfo does not match container\n");
    goto fail;
  }

  result = new Atrac9AudioStream;
  result->At9 = At9;
  result->BaseStream = stream;
  result->InitWithInfo(&container, &codecinfo);

  return result;

fail:
  if (At9) Atrac9ReleaseHandle(At9);
  if (result) {
    result->At9 = 0;
    result->BaseStream = 0;
    delete result;
  }
  stream->Seek(0, RW_SEEK_SET);
  return 0;
}

void Atrac9AudioStream::InitWithInfo(At9ContainerInfo* container,
                                     Atrac9CodecInfo* codecinfo) {
  EncoderDelay = container->EncoderDelay;
  Duration = container->SampleCount + container->EncoderDelay;
  SampleRate = container->SampleRate;
  ChannelCount = container->ChannelCount;
  LoopStart = container->LoopStart;
  LoopEnd = container->HasLoop ? container->LoopEnd : Duration;
  StreamDataOffset = container->StreamDataOffset;

  BitDepth = 16;

  SamplesPerFrame = codecinfo->frameSamples;
  FramesPerSuperframe = codecinfo->framesInSuperframe;
  SamplesPerBuffer = SamplesPerFrame * FramesPerSuperframe;

  DecodedBuffer =
      (int16_t*)malloc(sizeof(int16_t) * ChannelCount * SamplesPerBuffer);
  EncodedBytesPerBuffer = codecinfo->superframeSize;
  EncodedBuffer = (uint8_t*)malloc(EncodedBytesPerBuffer);

  ImpLog(LL_Debug, LC_Audio,
         "Created ATRAC9 stream Duration=%d, SampleRate=%d, ChannelCount=%d, "
         "LoopStart=%d, LoopEnd=%d\n",
         Duration, SampleRate, ChannelCount, LoopStart, LoopEnd);

  BaseStream->Seek(StreamDataOffset, RW_SEEK_SET);
  Seek(EncoderDelay);
}

Atrac9AudioStream::~Atrac9AudioStream() {
  if (At9) Atrac9ReleaseHandle(At9);
  if (DecodedBuffer) free(DecodedBuffer);
  if (EncodedBuffer) free(EncodedBuffer);
}

// TODO load more than one superframe at once? reading in <1KB chunks is
// probably kinda slow

int Atrac9AudioStream::Read(void* buffer, int samples) {
  return ReadBuffered((int16_t*)buffer, samples);
}

bool Atrac9AudioStream::DecodeBuffer() {
  uint8_t* encoded = EncodedBuffer;
  int16_t* decoded = DecodedBuffer;
  // Atrac9Decode always decodes one frame
  for (int i = 0; i < FramesPerSuperframe; i++) {
    int bytesUsed;
    int err = Atrac9Decode(At9, encoded, decoded, &bytesUsed);
    if (err != 0) {
      ImpLog(LL_Error, LC_Audio, "Atrac9Decode error %d\n", err);
      return false;
    }
    encoded += bytesUsed;
    decoded += SamplesPerFrame * ChannelCount;
  }
  return true;
}

void Atrac9AudioStream::Seek(int samples) { return SeekBuffered(samples); }

bool Atrac9AudioStream::_registered =
    AudioStream::AddAudioStreamCreator(&Atrac9AudioStream::Create);

}  // namespace Audio
}  // namespace Impacto