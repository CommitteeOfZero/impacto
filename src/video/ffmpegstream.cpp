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
  FlushFrameQueue();
  FlushPacketQueue();
  avcodec_close(CodecContext);
  SDL_DestroyMutex(PacketLock);
  SDL_DestroyMutex(FrameLock);
  SDL_DestroyCond(DecodeCond);
}

void FFmpegStream::FlushPacketQueue() {
  SDL_LockMutex(PacketLock);
  while (PacketQueue.size() > 0) {
    auto packet = PacketQueue.front();
    if (packet.Serial != INT32_MIN) av_packet_free(&packet.Packet);
    PacketQueue.pop();
  }
  PacketQueueSerial++;
  SDL_UnlockMutex(PacketLock);
}

void FFmpegStream::FlushFrameQueue() {
  while (FrameQueue.size() > 0) {
    auto frame = FrameQueue.front();
    if (frame.Serial != INT32_MIN) av_frame_free(&frame.Frame);
    FrameQueue.pop();
  }
}

}  // namespace Video
}  // namespace Impacto