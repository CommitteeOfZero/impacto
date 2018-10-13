#include "inst_sound.h"

#include "inst_macros.inc"

#include <math.h>

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstBGMplay) {
  StartInstruction;
  PopUint8(loop);
  PopExpression(track);
  if (loop == 2) {
    PopExpression(unk);
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction BGMplay(loop: %i, track: %i, unk: %i)\n", loop,
               track, unk);
  } else {
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction BGMplay(loop: %i, track: %i)\n", loop, track);
  }
}
VmInstruction(InstBGMstop) {
  StartInstruction;
  PopUint8(channel);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction BGMstop(channel: %i)\n",
             channel);
}
VmInstruction(InstSEplay) {
  StartInstruction;
  PopUint8(channel);
  PopUint8(type);
  if (type != 2) {
    PopExpression(effect);
    PopExpression(loop);
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction SEplay(channel: %i, type: %i, effect: %i, "
               "loop: %i)\n",
               channel, type, effect, loop);
  } else {
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction SEplay(channel: %i, type: %i)\n", channel,
               type);
  }
}
VmInstruction(InstSEstop) {
  StartInstruction;
  PopUint8(channel);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SEstop(channel: %i)\n",
             channel);
}
VmInstruction(InstSSEplay) {
  StartInstruction;
  PopExpression(sysSeId);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SSEplay(sysSeId: %i)\n",
             sysSeId);
}
VmInstruction(InstSSEstop) {}
VmInstruction(InstBGMflag) {}
VmInstruction(InstVoicePlay) {
  StartInstruction;
  PopUint8(channel);
  PopExpression(arg1);
  PopExpression(arg2);
  ImpLogSlow(LL_Warning, LC_VMStub,
             "STUB instruction VoicePlay(channel: %i, arg1: %i, arg2: %i)\n",
             channel, arg1, arg2);
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
VmInstruction(InstBGMduelPlay) {}
VmInstruction(InstSNDpause) {}
VmInstruction(InstSEplayWait) {}
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
VmInstruction(InstSysVoicePlay) {}

}  // namespace Vm

}  // namespace Impacto