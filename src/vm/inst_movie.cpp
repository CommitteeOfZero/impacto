#include "inst_movie.h"

#include "inst_macros.inc"

#include <math.h>

#include "expression.h"
#include "../game.h"
#include "../log.h"
#include "../mem.h"
#include "../profile/scriptvars.h"
#include "../profile/vm.h"
#include "../profile/game.h"
#include "../video/videosystem.h"
#include "interface/input.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

VmInstruction(InstPlayMovie) {
  StartInstruction;
  PopUint8(playMode);

  int playView;
  if (playMode == 99) {  // PlayMovieRecover - used when loading a save file
    // playMode of the video at save time
    playMode = static_cast<uint8_t>(ExpressionEval(thread));
    playView = ExpressionEval(thread);
  } else {
    playView = *thread->GetIp();
    thread->IpOffset++;
  }

  PopExpression(playNo);
  PopExpression(movCancelFlag);

  if (Profile::GameFeatures & GameFeature::Video) {
    Io::Stream* stream;
    auto err = Io::VfsOpen("movie", playNo, &stream);
    if (err != IoError_OK) {
      ImpLog(LogLevel::Error, LogChannel::Video,
             "Failed to open movie for playback: IO error {:s}\n", err);
      return;
    }

    const uint8_t channel = (playMode / 20) == 0 ? 0 : 1;
    Video::Players[channel]->CancelFlag = movCancelFlag;
    Video::Players[channel]->CancelWaitTime = 0;
    SetFlag(SF_MOVIEFL + channel, movCancelFlag);
    ScrWork[SW_MOVIE_PLAYNO + 20 * channel] = playNo;
    ScrWork[SW_MOVIE_PLAYMODE + 20 * channel] = playMode + 20 * channel;
    ScrWork[SW_MOVIE_PLAYVIEW + 20 * channel] = playView;
    ScrWork[SW_MOVIE_LOADNO + 20 * channel] = 0xffff;

    int flags = 0;
    if (playMode >= 8) {
      playMode -= 8;
      flags |= 4;
    }
    switch (playMode) {
      case 0:
        flags |= 0b10;
        break;
      case 1:
        flags |= 0b1;
        break;
      case 2:
        flags |= 0b1010;
        break;
      case 3:
        flags |= 0b1001;
        break;
      case 4:
        flags |= 0b100000;
        break;
      case 5:
        flags |= 0b101000;
        break;
      default:
        break;
    }

    if (Video::Players[channel]->IsPlaying) Video::Players[channel]->Stop();
    Video::Players[channel]->Play(stream, flags & 8, flags & 4);

    SetFlag(SF_MOVIE_DRAWWAIT + channel, true);
    SetFlag(SF_MOVIEPLAY + channel, true);
    SetFlag(SF_MOVIECANCEL + channel, false);
  }

  BlockThread;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction PlayMovie(playMode: {:d}, playView: {:d}, "
             "playNo: {:d}, movCancelFlag: {:d})\n",
             playMode, playView, playNo, movCancelFlag);
}

static void PlayMovieOldCommon(Sc3VmThread* thread, uint8_t instType) {
  StartInstruction;
  PopUint8(playMode);

  int playView;
  if (playMode == 99) {  // PlayMovieRecover - used when loading a save file
    // playMode of the video at save time
    playMode = static_cast<uint8_t>(ExpressionEval(thread));
    playView = ExpressionEval(thread);
  } else {
    playView = *thread->GetIp();
    thread->IpOffset++;
  }

  PopExpression(playNo);
  PopExpression(movCancelFlag);

  if (Profile::GameFeatures & GameFeature::Video) {
    Io::Stream* stream;
    auto err = Io::VfsOpen("movie", playNo, &stream);
    if (err != IoError_OK) {
      ImpLog(LogLevel::Error, LogChannel::Video,
             "Failed to open movie for playback: IO error {:s}\n", err);
      return;
    }

    const uint8_t channel = (playMode / 20) == 0 ? 0 : 1;
    Video::Players[channel]->CancelFlag = movCancelFlag;
    Video::Players[channel]->CancelWaitTime = 0;
    SetFlag(SF_MOVIEFL + channel, movCancelFlag);
    ScrWork[SW_MOVIE_PLAYNO + 20 * channel] = playNo + (1000 * (instType == 2));
    ScrWork[SW_MOVIE_PLAYMODE + 20 * channel] = playMode + 20 * channel;
    ScrWork[SW_MOVIE_PLAYVIEW + 20 * channel] = playView;
    ScrWork[SW_MOVIE_LOADNO + 20 * channel] = 0xffff;

    if (Video::Players[channel]->IsPlaying) Video::Players[channel]->Stop();
    Video::Players[channel]->Play(stream, playMode == 5, playMode == 5);

    SetFlag(SF_MOVIE_DRAWWAIT + channel, true);
    SetFlag(SF_MOVIEPLAY + channel, true);
    SetFlag(SF_MOVIECANCEL + channel, false);
  }

  BlockThread;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction PlayMovieOld(playMode: {:d}, playView: {:d}, "
             "playNo: {:d}, movCancelFlag: {:d})\n",
             playMode, playView, playNo, movCancelFlag);
}

VmInstruction(InstPlayMovieOld) { PlayMovieOldCommon(thread, 1); }
VmInstruction(InstPlayMovieOld2) { PlayMovieOldCommon(thread, 2); }

VmInstruction(InstMovie) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // Restart
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Movie(type: Restart)\n");
    } break;
    case 1: {  // Pause
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Movie(type: Pause)\n");
    } break;
  }
}
VmInstruction(InstMovieMain) {
  using namespace Video;
  StartInstruction;

  const bool videoEnabled = Profile::GameFeatures & GameFeature::Video;

  PopUint8(type);
  const uint8_t playerId = type < 20 ? 0 : 1;
  VideoPlayer& player = *Players[playerId];

  switch (type) {
    case 0: {
      if (!videoEnabled) {
        SetFlag(SF_MOVIELOADPLAYFL + playerId, false);
        SetFlag(SF_MOVIE_DRAWWAIT + playerId, false);
        ScrWork[SW_MOVIE_PLAYNO + playerId * 20] = 0xffff;
        ScrWork[SW_MOVIE_LOADNO + playerId * 20] = 0xffff;
        break;
      }

      if (GetFlag(SF_MOVIEPLAY + playerId) && player.CancelWaitTime != 0) {
        player.CancelWaitTime++;
        if (player.CancelWaitTime < 8) {
          ResetInstruction;
          BlockThread;
          return;
        }

        SetFlag(SF_MOVIEPLAY + playerId, false);
        SetFlag(SF_MOVIECANCEL + playerId, true);
        player.CancelWaitTime = 0;
        BlockThread;
      }

      if (!player.CancelFlag ||
          !Interface::GetControlState(Interface::CT_MovieCancel)) {
        if (GetFlag(SF_MOVIEPLAY + playerId)) {
          ResetInstruction;
          BlockThread;
        } else {
          player.Stop();

          SetFlag(SF_MOVIELOADPLAYFL + playerId, false);
          SetFlag(SF_MOVIE_DRAWWAIT + playerId, false);
          ScrWork[SW_MOVIE_PLAYNO + playerId * 20] = 0xffff;
          ScrWork[SW_MOVIE_LOADNO + playerId * 20] = 0xffff;
        }

      } else {
        player.CancelWaitTime++;
        player.CancelFlag = false;

        ResetInstruction;
        BlockThread;
      }
    } break;

    case 1:
    case 21: {
      thread->ScriptParam = GetFlag(SF_MOVIEPLAY + playerId);
    } break;

    case 2:
    case 22: {  // Stop
      if (videoEnabled) player.Stop();

      SetFlag(SF_MOVIEPLAY + playerId, false);
      SetFlag(SF_MOVIELOADPLAYFL + playerId, false);
      SetFlag(SF_MOVIE_DRAWWAIT + playerId, false);
      ScrWork[SW_MOVIE_PLAYNO + playerId * 20] = 0xffff;
      ScrWork[SW_MOVIE_LOADNO + playerId * 20] = 0xffff;
    } break;

    case 3:
    case 23: {  // StopWait
      if (player.CancelFlag &&
          Interface::GetControlState(Interface::CT_MovieCancel)) {
        SetFlag(SF_MOVIEPLAY + playerId, false);
        SetFlag(SF_MOVIECANCEL + playerId, true);
        BlockThread;
      }

      if (!GetFlag(SF_MOVIEPLAY + playerId)) {
        if (videoEnabled) player.Stop();

        SetFlag(SF_MOVIELOADPLAYFL + playerId, false);
        ScrWork[SW_MOVIE_PLAYNO + playerId * 20] = 0xffff;
        ScrWork[SW_MOVIE_LOADNO + playerId * 20] = 0xffff;
      }
    } break;

    case 4:
    case 24: {
      if (videoEnabled && !player.IsPlaying) {
        ResetInstruction;
        BlockThread;
      } else {
        // TODO: Set frame numbers
      }
    } break;

    case 20: {
      if (!videoEnabled) {
        SetFlag(SF_MOVIEPLAY + playerId, false);
        SetFlag(SF_MOVIECANCEL + playerId, false);
        ScrWork[SW_MOVIE_PLAYNO + playerId * 20] = 0xffff;
        ScrWork[SW_MOVIE_LOADNO + playerId * 20] = 0xffff;
        break;
      }

      if (player.CancelFlag &&
          Interface::GetControlState(Interface::CT_MovieCancel)) {
        SetFlag(SF_MOVIEPLAY + playerId, true);
        SetFlag(SF_MOVIECANCEL + playerId, true);
        BlockThread;
      }

      if (!GetFlag(SF_MOVIEPLAY + playerId)) {
        player.Stop();
        ScrWork[SW_MOVIE_PLAYNO + playerId * 20] = 0xffff;
        ScrWork[SW_MOVIE_LOADNO + playerId * 20] = 0xffff;
      } else {
        ResetInstruction;
        BlockThread;
      }
    } break;
  }
}
VmInstruction(InstLoadMovie) {
  StartInstruction;
  PopExpression(arg1);
  ScrWork[SW_MOVIE_LOADNO] = arg1 + 1000;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction LoadMovie(arg1: {:d})\n", arg1);
}
VmInstruction(InstPlayMovieMemory) {
  StartInstruction;
  PopUint8(playMode);
  if (playMode == 99) {
    PopExpression(playModeEx);
    PopExpression(playView);
    PopExpression(movCancelFlag);
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction PlayMovie(playMode: {:d}, playModeEx: {:d}, "
               "playView: {:d}, movCancelFlag: {:d})\n",
               playMode, playModeEx, playView, movCancelFlag);

  } else {
    PopUint8(playView);
    PopExpression(movCancelFlag);
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction PlayMovie(playMode: {:d}, playView: {:d}, "
               "movCancelFlag: {:d})\n",
               playMode, playView, movCancelFlag);
  }
}
VmInstruction(InstSFDpause) {
  StartInstruction;
  PopUint8(paused);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SFDpause(paused: {:d})\n", paused);
}

}  // namespace Vm

}  // namespace Impacto
