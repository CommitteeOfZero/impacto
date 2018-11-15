#include "inst_sound.h"

#include "inst_macros.inc"

#include <math.h>

#include "expression.h"
#include "../game.h"
#include "../mem.h"
#include "../log.h"
#include "../audio/audiosystem.h"
#include "../audio/audiostream.h"
#include "../audio/audiochannel.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstBGMplay) {
  StartInstruction;
  PopUint8(loop);
  PopExpression(track);
  if (loop == 2) {
    PopExpression(unk);
  } else {
  }

  Io::InputStream* stream;
  Io::VfsOpen("bgm", track, &stream);
  Audio::Channels[Audio::AC_BGM0].Volume = 0.15f;
  Audio::Channels[Audio::AC_BGM0].Play(Audio::AudioStream::Create(stream),
                                       (bool)loop, 0.0f);
}
VmInstruction(InstBGMstop) {
  StartInstruction;
  PopUint8(channel);
  Audio::Channels[Audio::AC_BGM0 + channel].Stop(1.0f);
}
VmInstruction(InstSEplay) {
  StartInstruction;
  PopUint8(channel);
  PopUint8(type);
  if (type != 2) {
    PopExpression(effect);
    PopExpression(loop);
    Io::InputStream* stream;
    Io::VfsOpen("se", effect, &stream);
    Audio::Channels[Audio::AC_SE0 + channel].Volume =
        (ScrWork[4315 + channel] / 100.0f) - 0.3f;
    Audio::Channels[Audio::AC_SE0 + channel].Play(
        Audio::AudioStream::Create(stream), (bool)loop, 0.0f);
  } else {
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction SEplay(channel: %i, type: %i)\n", channel,
               type);
  }
}
VmInstruction(InstSEstop) {
  StartInstruction;
  PopUint8(channel);
  Audio::Channels[Audio::AC_SE0 + channel].Stop(1.0f);
}
VmInstruction(InstSSEplay) {
  StartInstruction;
  PopExpression(sysSeId);
  Io::InputStream* stream;
  Io::VfsOpen("sysse", sysSeId, &stream);
  Audio::Channels[Audio::AC_SSE].Play(Audio::AudioStream::Create(stream), false,
                                      0.0f);
}
VmInstruction(InstSSEstop) {
  StartInstruction;
  Audio::Channels[Audio::AC_SSE].Stop(1.0f);
}
VmInstruction(InstBGMflag) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction BGMflag(arg1: %i)\n",
             arg1);
}
VmInstruction(InstVoicePlay) {
  StartInstruction;
  PopUint8(channel);
  PopExpression(arg1);
  PopExpression(arg2);
  Io::InputStream* stream;
  Io::VfsOpen("voice", arg1, &stream);
  Audio::Channels[Audio::AC_VOICE0 + channel].Play(
      Audio::AudioStream::Create(stream), (bool)arg2, 0.0f);
}
VmInstruction(InstVoiceStop) {
  StartInstruction;
  PopUint8(channel);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction VoiceStop(channel: %i)\n",
             channel);
}
VmInstruction(InstVoicePlayWait) {
  StartInstruction;
  PopUint8(channel);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction VoicePlayWait(channel: %i)\n", channel);
}
VmInstruction(InstBGMduelPlay) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction BGMduelPlay(type: %i, arg1: %i, arg2: %i)\n",
                 type, arg1, arg2);
    } break;
    case 1: {
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction BGMduelPlay(type: %i)\n", type);
    } break;
    case 2: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction BGMduelPlay(type: %i, arg1: %i)\n", type,
                 arg1);
    } break;
    case 3: {
      PopExpression(arg1);
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction BGMduelPlay(type: %i, arg1: %i)\n", type,
                 arg1);
    } break;
  }
}
VmInstruction(InstSNDpause) {
  StartInstruction;
  PopUint8(paused);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SNDpause(paused: %i)\n",
             paused);
}
VmInstruction(InstSEplayWait) {
  StartInstruction;
  PopUint8(channel);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SEplayWait(channel: %i)\n", channel);
}
VmInstruction(InstResetSoundAll) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction ResetSoundAll()\n");
}
VmInstruction(InstSNDloadStop) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SNDloadStop()\n");
}
VmInstruction(InstBGMstopWait) {
  StartInstruction;
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction BGMstopWait()\n");
}
VmInstruction(InstSysVoicePlay) {
  PopUint8(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction SysVoicePlay(arg1: %i, arg2: %i)\n", arg1, arg2);
}

}  // namespace Vm

}  // namespace Impacto