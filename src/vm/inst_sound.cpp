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
#include "../profile/scriptvars.h"
#include "../profile/vm.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

VmInstruction(InstBGMplay) {
  StartInstruction;
  PopUint8(loop);
  PopExpression(track);
  if (loop == 2) {
    PopExpression(unk);
  }

  if (ScrWork[SW_BGMREQNO] != track) {
    ScrWork[SW_BGMREQNO] = track;

    Io::InputStream* stream;
    Io::VfsOpen("bgm", track, &stream);
    Audio::Channels[Audio::AC_BGM0].Play(Audio::AudioStream::Create(stream),
                                         (bool)loop, 0.0f);
  }
}
VmInstruction(InstBGMstop) {
  StartInstruction;
  PopUint8(channel);
  Audio::Channels[Audio::AC_BGM0 + channel].Stop(1.0f);
  ScrWork[SW_BGMREQNO] = 0xFFFF;
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
    ScrWork[SW_SEREQNO + channel] = effect;
    Audio::Channels[Audio::AC_SE0 + channel].Volume =
        (ScrWork[SW_SEVOL + channel] / 100.0f) * 0.3f;
    Audio::Channels[Audio::AC_SE0 + channel].Play(
        Audio::AudioStream::Create(stream), (bool)loop, 0.0f);
  } else {
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction SEplay(channel: %i, type: %i)\n", channel,
               type);
  }
}
VmInstruction(InstSEplayMO6) {
  StartInstruction;
  PopUint8(channel);
  PopExpression(effect);
  PopExpression(loop);
  Io::InputStream* stream;
  Io::VfsOpen("se", effect, &stream);
  Audio::Channels[Audio::AC_SE0 + channel].Volume =
      (ScrWork[SW_SEVOL + channel] / 100.0f) * 0.3f;
  Audio::Channels[Audio::AC_SE0 + channel].Play(
      Audio::AudioStream::Create(stream), (bool)loop, 0.0f);
}
VmInstruction(InstSEstop) {
  StartInstruction;
  PopUint8(channel);
  Audio::Channels[Audio::AC_SE0 + channel].Stop(1.0f);
  ScrWork[SW_SEREQNO + channel] = 0xFFFF;
}
VmInstruction(InstSSEplay) {
  StartInstruction;
  if (Profile::Vm::GameInstructionSet == +InstructionSet::CHN) {
    PopUint8(channel);
  }
  PopExpression(sysSeId);
  Io::InputStream* stream;
  int64_t err = Io::VfsOpen("sysse", sysSeId, &stream);
  if (err == IoError_OK) {
    Audio::Channels[Audio::AC_SSE].Play(Audio::AudioStream::Create(stream),
                                        false, 0.0f);
  }
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
VmInstruction(InstVoicePlayOld) {
  StartInstruction;
  PopUint8(channel);
  PopExpression(arg1);
  Io::InputStream* stream;
  Io::VfsOpen("voice", arg1, &stream);
  Audio::Channels[Audio::AC_VOICE0 + channel].Play(
      Audio::AudioStream::Create(stream), false, 0.0f);
}
VmInstruction(InstVoiceStop) {
  StartInstruction;
  PopUint8(channel);
  Audio::Channels[Audio::AC_VOICE0 + channel].Stop(0.0f);
}
VmInstruction(InstVoiceStopNew) {
  StartInstruction;
  PopUint8(channel);
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction VoiceStopNew(channel: %i, arg1: %i)\n", channel,
             arg1);
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
VmInstruction(InstSysSeload) {
  StartInstruction;
  if (Profile::Vm::GameInstructionSet == +InstructionSet::MO8 ||
    Profile::Vm::GameInstructionSet == +InstructionSet::CHN) {
    PopUint8(arg1);
  }
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SysSeload()\n");
}

}  // namespace Vm

}  // namespace Impacto