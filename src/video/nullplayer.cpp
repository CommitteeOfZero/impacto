#include "nullplayer.h"

namespace Impacto {
namespace Video {

void NullPlayer::Init() {}

void NullPlayer::Play(Io::InputStream* stream, bool loop, bool alpha) {}

void NullPlayer::Stop() {}

void NullPlayer::Seek(int64_t pos) {}

void NullPlayer::Update(float dt) {}

void NullPlayer::Render(float videoAlpha) {}

}  // namespace Video
}  // namespace Impacto