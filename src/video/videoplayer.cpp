#include "videoplayer.h"
#include "../log.h"

namespace Impacto {
namespace Video {

VideoPlayer* VideoPlayer::Create(Io::Stream* stream) {
  for (auto f : Registry) {
    VideoPlayer* result = f(stream);
    if (result) return result;
  }
  ImpLog(LogLevel::Error, LogChannel::Video, "No video player found\n");
  return 0;
}

bool VideoPlayer::AddVideoPlayerCreator(VideoPlayerCreator c) {
  Registry.push_back(c);
  return true;
}

std::vector<VideoPlayer::VideoPlayerCreator> VideoPlayer::Registry;

}  // namespace Video
}  // namespace Impacto