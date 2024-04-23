#include "dummyaudiostream.h"

using namespace Impacto::Io;

namespace Impacto {
namespace Audio {

AudioStream* DummyAudioStream::Create(InputStream* stream) {
  DummyAudioStream* result = 0;

  result = new DummyAudioStream;
  result->BaseStream = stream;
  return result;
}

DummyAudioStream::~DummyAudioStream() {}

int DummyAudioStream::Read(void* buffer, int samples) { return 0; }

void DummyAudioStream::Seek(int samples) {}

bool DummyAudioStream::_registered =
    AudioStream::AddAudioStreamCreator(&DummyAudioStream::Create);

}  // namespace Audio
}  // namespace Impacto