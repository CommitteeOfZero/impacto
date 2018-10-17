#include "inst_movie.h"

#include "inst_macros.inc"

#include <math.h>

#include "expression.h"
#include "../game.h"
#include "../log.h"

namespace Impacto {

namespace Vm {

VmInstruction(InstPlayMovie) {
  StartInstruction;
  PopUint8(playMode);
  if (playMode == 99) {
    PopExpression(playModeEx);
    PopExpression(playView);
    PopExpression(playNo);
    PopExpression(movCancelFlag);
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction PlayMovie(playMode: %i, playModeEx: %i, "
               "playView: %i, playNo: %i, movCancelFlag: %i)\n",
               playMode, playModeEx, playView, playNo, movCancelFlag);

  } else {
    PopUint8(playView);
    PopExpression(playNo);
    PopExpression(movCancelFlag);
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction PlayMovie(playMode: %i, playView: %i, "
               "playNo: %i, movCancelFlag: %i)\n",
               playMode, playView, playNo, movCancelFlag);
  }
}
VmInstruction(InstMovieMain) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 2:  // Stop
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MovieMain(type: Stop)\n");
      break;
    case 3:  // StopWait
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MovieMain(type: StopWaitForSomething)\n");
      break;
    default:
      ImpLogSlow(LL_Warning, LC_VMStub,
                 "STUB instruction MovieMain(type: %i)\n", type);
      break;
  }
}
VmInstruction(InstLoadMovie) {
  StartInstruction;
  PopExpression(arg1);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction LoadMovie(arg1: %i)\n",
             arg1);
}
VmInstruction(InstPlayMovieMemory) {
  StartInstruction;
  PopUint8(playMode);
  if (playMode == 99) {
    PopExpression(playModeEx);
    PopExpression(playView);
    PopExpression(movCancelFlag);
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction PlayMovie(playMode: %i, playModeEx: %i, "
               "playView: %i, movCancelFlag: %i)\n",
               playMode, playModeEx, playView, movCancelFlag);

  } else {
    PopUint8(playView);
    PopExpression(movCancelFlag);
    ImpLogSlow(LL_Warning, LC_VMStub,
               "STUB instruction PlayMovie(playMode: %i, playView: %i, "
               "movCancelFlag: %i)\n",
               playMode, playView, movCancelFlag);
  }
}
VmInstruction(InstSFDpause) {
  StartInstruction;
  PopUint8(paused);
  ImpLogSlow(LL_Warning, LC_VMStub, "STUB instruction SFDpause(paused: %i)\n",
             paused);
}

}  // namespace Vm

}  // namespace Impacto