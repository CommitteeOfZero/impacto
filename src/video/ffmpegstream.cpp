#include "ffmpegstream.h"

namespace Impacto {
namespace Video {

FFmpegStream::FFmpegStream(AVStream* avStream, AVCodecContext* codecCtx) {
  stream = avStream;
  CodecContext = codecCtx;
  PacketLock = SDL_CreateMutex();
  FrameLock = SDL_CreateMutex();
  DecodeCond = SDL_CreateCond();
}

FFmpegStream::~FFmpegStream() {
  while (FrameQueue.size() > 0) {
    auto frame = FrameQueue.front();
    av_frame_free(&frame.Frame);
    FrameQueue.pop();
  }
  while (PacketQueue.size() > 0) {
    auto packet = PacketQueue.front();
    av_packet_free(&packet.Packet);
    PacketQueue.pop();
  }
  avcodec_close(CodecContext);
  SDL_DestroyMutex(PacketLock);
  SDL_DestroyMutex(FrameLock);
  SDL_DestroyCond(DecodeCond);
}

}  // namespace Video
}  // namespace Impacto