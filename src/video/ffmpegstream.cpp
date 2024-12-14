#include "ffmpegstream.h"
#include <libavutil/avutil.h>
#include "ffmpegplayer.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
};

namespace Impacto {
namespace Video {

template <AVMediaType MediaType>
void FFmpegStream<MediaType>::FlushPacketQueue() {
  std::lock_guard lock{PacketLock};
  while (PacketQueue.size() > 0) {
    auto packet = PacketQueue.front();
    PacketQueue.pop();
  }
  PacketQueueSerial++;
}

template void FFmpegStream<AVMEDIA_TYPE_AUDIO>::FlushPacketQueue();
template void FFmpegStream<AVMEDIA_TYPE_VIDEO>::FlushPacketQueue();

template <AVMediaType MediaType>
void FFmpegStream<MediaType>::FlushFrameQueue() {
  while (FrameQueue.size() > 0) {
    auto frame = FrameQueue.front();
    FrameQueue.pop();
  }
}
template void FFmpegStream<AVMEDIA_TYPE_AUDIO>::FlushFrameQueue();
template void FFmpegStream<AVMEDIA_TYPE_VIDEO>::FlushFrameQueue();

}  // namespace Video
}  // namespace Impacto