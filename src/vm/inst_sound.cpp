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
#include "../data/savesystem.h"

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
    SaveSystem::SetBgmFlag(track, true);
    Audio::Channels[Audio::AC_BGM0]->Play("bgm", track, (bool)loop, 0.0f);
  }
}
VmInstruction(InstBGMstop) {
  StartInstruction;
  PopUint8(channel);
  Audio::Channels[Audio::AC_BGM0 + channel]->Stop(1.0f);
  ScrWork[SW_BGMREQNO] = 0xFFFF;
}
VmInstruction(InstSEplay) {
  StartInstruction;
  PopUint8(channel);
  PopUint8(type);

  if (type != 2) {
    PopExpression(effect);
    PopExpression(loop);
    if (loop) {
      ScrWork[SW_SEREQNO + channel] = effect;
    } else {
      ScrWork[SW_SEREQNO + channel] = 0xFFFF;
    }
    Audio::Channels[Audio::AC_SE0 + channel]->SetVolume(
        (ScrWork[SW_SEVOL + channel] / 100.0f) * 0.3f);

    Audio::Channels[Audio::AC_SE0 + channel]->Play("se", effect, (bool)loop,
                                                   0.0f);
    if (type == 1) {
      Audio::Channels[Audio::AC_SE0 + channel]->Pause();
    }
  } else {
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction SEplay(channel: {:d}, type: {:d})\n", channel,
               type);
  }
}
VmInstruction(InstSEplayMO6) {
  StartInstruction;
  PopUint8(channel);
  PopExpression(effect);
  PopExpression(loop);
  Audio::Channels[Audio::AC_SE0 + channel]->SetVolume(
      (ScrWork[SW_SEVOL + channel] / 100.0f) * 0.3f);
  Audio::Channels[Audio::AC_SE0 + channel]->Play("se", effect, (bool)loop,
                                                 0.0f);
}
VmInstruction(InstSEstop) {
  StartInstruction;
  PopUint8(channel);
  Audio::Channels[Audio::AC_SE0 + channel]->Stop(1.0f);
  ScrWork[SW_SEREQNO + channel] = 0xFFFF;
}
VmInstruction(InstSSEplay) {
  StartInstruction;
  if (Profile::Vm::GameInstructionSet == +InstructionSet::CHN) {
    PopUint8(channel);
  }
  PopExpression(sysSeId);
  Audio::Channels[Audio::AC_SSE]->Play("sysse", sysSeId, false, 0.0f);
}
VmInstruction(InstSSEstop) {
  StartInstruction;
  Audio::Channels[Audio::AC_SSE]->Stop(1.0f);
}
VmInstruction(InstBGMflag) {
  StartInstruction;
  PopExpression(arg1);
  SaveSystem::SetBgmFlag(arg1, true);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction BGMflag(arg1: {:d})\n", arg1);
}
VmInstruction(InstVoicePlay) {
  StartInstruction;
  PopUint8(channel);
  PopExpression(fileId);
  PopExpression(loop);
  Audio::Channels[Audio::AC_VOICE0 + channel]->Play("voice", fileId, (bool)loop,
                                                    0.0f);
}
VmInstruction(InstVoicePlayOld) {
  StartInstruction;
  PopUint8(channel);
  PopExpression(fileId);
  Audio::Channels[Audio::AC_VOICE0 + channel]->Play("voice", fileId, false,
                                                    0.0f);
}
VmInstruction(InstVoiceStop) {
  StartInstruction;
  PopUint8(channel);
  Audio::Channels[Audio::AC_VOICE0 + channel]->Stop(0.0f);
}
VmInstruction(InstVoiceStopNew) {
  StartInstruction;
  PopUint8(channel);
  PopExpression(fade);
  Audio::Channels[Audio::AC_VOICE0 + channel]->Stop((float)fade);
}
VmInstruction(InstVoicePlayWait) {
  StartInstruction;
  PopUint8(channel);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction VoicePlayWait(channel: {:d})\n", channel);
}
VmInstruction(InstBGMduelPlay) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction BGMduelPlay(type: {:d}, arg1: {:d}, arg2: {:d})\n",
          type, arg1, arg2);
    } break;
    case 1: {
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction BGMduelPlay(type: {:d})\n", type);
    } break;
    case 2: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction BGMduelPlay(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
    case 3: {
      PopExpression(arg1);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction BGMduelPlay(type: {:d}, arg1: {:d})\n", type,
                 arg1);
    } break;
  }
}
VmInstruction(InstSNDpause) {
  StartInstruction;
  PopUint8(paused);

  for (int i = 0; i < 6; i++) {
    (paused) ? Audio::Channels[Audio::AC_SE0 + i]->Pause()
             : Audio::Channels[Audio::AC_SE0 + i]->Resume();
  }

  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SNDpause(paused: {:d})\n", paused);
}
VmInstruction(InstSEplayWait) {
  StartInstruction;
  PopUint8(channel);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SEplayWait(channel: {:d})\n", channel);
}
VmInstruction(InstResetSoundAll) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction ResetSoundAll()\n");
}
VmInstruction(InstSNDloadStop) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SNDloadStop()\n");
}
VmInstruction(InstBGMstopWait) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction BGMstopWait()\n");
}
VmInstruction(InstSysVoicePlay) {
  PopUint8(arg1);
  PopExpression(arg2);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SysVoicePlay(arg1: {:d}, arg2: {:d})\n", arg1,
             arg2);
}
VmInstruction(InstSysSeload) {
  StartInstruction;
  if (Profile::Vm::GameInstructionSet == +InstructionSet::MO8 ||
      Profile::Vm::GameInstructionSet == +InstructionSet::CHN) {
    PopUint8(arg1);
  }
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SysSeload()\n");
}

}  // namespace Vm

}  // namespace Impacto