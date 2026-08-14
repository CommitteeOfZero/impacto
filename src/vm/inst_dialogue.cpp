#include "inst_dialogue.h"

#include "inst_macros.inc"

#include "expression.h"
#include "../profile/scriptvars.h"
#include "../profile/configsystem.h"
#include "../profile/dialogue.h"
#include "../mem.h"
#include "../log.h"
#include "../audio/audiostream.h"
#include "../profile/vm.h"
#include "../hud/saveicondisplay.h"
#include "../hud/tipsnotification.h"
#include "../data/savesystem.h"
#include "../data/tipssystem.h"
#include "../ui/ui.h"
#include "interface/input.h"
#include "../text/dialoguepage.h"
#include "../audio/audiosystem.h"
#include "vm.h"

namespace Impacto {

namespace Vm {

using namespace Impacto::Profile::ScriptVars;

VmInstruction(InstMesViewFlag) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // Set
      PopExpression(scriptId);
      PopExpression(lineId);
      SaveSystem::SetLineRead(static_cast<size_t>(scriptId),
                              static_cast<size_t>(lineId));
    } break;
    case 1: {  // Check
      PopExpression(scrWorkEntry);
      PopExpression(scriptId);
      PopExpression(lineId);
      ScrWork[scrWorkEntry] = SaveSystem::IsLineRead(
          static_cast<size_t>(scriptId), static_cast<size_t>(lineId));
    } break;
  }
}
VmInstruction(InstSetMesWinPri) {
  StartInstruction;
  PopExpression(arg1);
  PopExpression(arg2);
  PopExpression(unused);
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction MesViewFlag(arg1: {:d}, arg2: "
             "{:d}, unused: {:d})\n",
             arg1, arg2, unused);
}

enum class SyncType {
  Wait = 1,
  End = 2,
};

static bool InstMesSyncMain(int pageId, SyncType type) {
  DialoguePage& currentPage = DialoguePages[pageId];
  DialoguePage::State pageState = currentPage.GetState();
  using enum DialoguePage::State;
  using enum DialoguePage::AdvanceMethodType;

  if (type == SyncType::Wait) {
    if (pageState == Initial || pageState == Showing) {
      if (GetFlag(SF_MESALLSKIP)) {
        currentPage.Typewriter.Finish();
      } else if (GetFlag(SF_MESSKIP)) {
        currentPage.Typewriter.CancelRequested = true;
      }
    }
    if (pageState == Initial || pageState == Showing || pageState == Hiding)
      return true;

    if (pageState == Shown) {
      const bool autoForward =
          currentPage.AdvanceMethod == AutoForward ||
          currentPage.AdvanceMethod == AutoForwardSyncVoice;
      if (autoForward) {
        SyncAutoModeEnabled = true;
        SyncAutoTime = currentPage.AutoWaitTime;
      }
    }
    return false;
  } else if (type == SyncType::End) {
    switch (pageState) {
      case Initial:
      case Showing:
      case Hiding:
        return true;
      case Shown: {
        if (currentPage.AdvanceMethod == Skip) {
          return false;
        }

        if (!SyncAutoModeEnabled) {
          SetFlag(SF_SYSMENUDISABLE, false);
          SetFlag(currentPage.Id + SF_SHOWWAITICON, true);
        }

        if (!GetFlag(SF_UIHIDDEN) && GetFlag(SF_MESSKIP)) {
          currentPage.AutoWaitTime = 0.0f;

          if (Profile::ConfigSystem::SkipVoice) {
            Audio::Channels[Audio::AC_VOICE0]->Stop(0.0f);
          }
        }

        const Audio::AudioChannelState audioState =
            Audio::Channels[Audio::AC_VOICE0]->GetState();
        const bool audioPlaying = audioState == Audio::ACS_Playing ||
                                  audioState == Audio::ACS_FadingIn ||
                                  audioState == Audio::ACS_FadingOut;
        if (currentPage.AutoWaitTime == 0.0f &&
            (!audioPlaying || GetFlag(SF_MESSKIP))) {
          if (Profile::ConfigSystem::SkipVoice) {
            Audio::Channels[Audio::AC_VOICE0]->Stop(0.0f);
          }

          SetFlag(currentPage.Id + SF_CHAANIME, false);
          SetFlag(currentPage.Id + SF_SHOWWAITICON, false);
          SetFlag(SF_SYSMENUDISABLE, true);

          const bool nvlDontClear = currentPage.AdvanceMethod != PresentClear &&
                                    currentPage.GetMode() == DPM_NVL;
          const bool advanceWithoutHiding =
              currentPage.AdvanceMethod == Present0x18 ||
              currentPage.AdvanceMethod == AutoForward || nvlDontClear;

          if (advanceWithoutHiding) {
            if (!nvlDontClear) {
              SetFlag(SF_MESCLEAR0 + currentPage.Id, true);
              currentPage.PushBacklogEntry();
            }

            SaveSystem::SetLineRead(
                static_cast<size_t>(ScrWork[currentPage.Id * 2 + SW_SCRIPTID]),
                static_cast<size_t>(ScrWork[currentPage.Id * 2 + SW_LINEID]));

            return false;
          }

          if (GetFlag(SF_MESALLSKIP)) {
            currentPage.Typewriter.Finish(AnimationDirection::Out);
          } else {
            currentPage.Typewriter.StartOut(true);
          }
        }
        return true;
      }
      case Hidden:
        currentPage.PushBacklogEntry();
        SetFlag(SF_MESCLEAR0 + currentPage.Id, true);
        SaveSystem::SetLineRead(
            static_cast<size_t>(ScrWork[currentPage.Id * 2 + SW_SCRIPTID]),
            static_cast<size_t>(ScrWork[currentPage.Id * 2 + SW_LINEID]));
        return false;
      default:
        break;
    }
    throw std::invalid_argument(
        fmt::format("Invalid page state: {}", pageState));
  }
  throw std::invalid_argument("Invalid sync type");
};

VmInstruction(InstMesSync) {
  using enum DialoguePage::SyncStatusType;
  StartInstruction;

  PopUint8(type);

  const auto threadPageHandler = [&](std::invocable<size_t> auto&& callback) {
    if (type > 10) {
      callback(thread->DialoguePageId);
    } else if (type == 10) {
      PopExpression(pageId);
      thread->DialoguePageId = pageId;
      callback(pageId);
    } else {
      for (uint32_t pageId = 0; pageId < DialoguePages.size(); pageId++) {
        callback(pageId);
      }
    }
  };

  switch (type) {
    case 0:
    case 10: {
      SyncAutoModeEnabled = false;
      threadPageHandler([&](uint32_t pageId) {
        DialoguePages[pageId].SyncStatus = Stopped;
        if (DialoguePages[pageId].SyncEnabled) {
          DialoguePages[pageId].SyncEnabled = false;
          ++DialoguePages[pageId].SyncStatus;
          bool windowOpen = GetFlag(pageId + SF_MESWINDOW0OPENFL);
          if (!windowOpen) {
            SetFlag(pageId + SF_MESWINDOW0OPENFL, 1);
            BlockThread;
          }
          if (!DialoguePages[pageId].FadeAnimation.IsIn())
            DialoguePages[pageId].Show();
        }
      });
    } break;
    case 1:
    case 11: {
      bool fade = false;
      threadPageHandler([&](uint32_t pageId) {
        if ((DialoguePages[pageId].SyncStatus != Stopped) &&
            !DialoguePages[pageId].FadeAnimation.IsIn()) {
          fade = true;
        }
      });
      if (fade) {
        ResetInstruction;
        BlockThread;
      }
    } break;
    case 2:
    case 12: {
      threadPageHandler([&](uint32_t pageId) {
        if (DialoguePages[pageId].SyncStatus == Waiting) {
          ++DialoguePages[pageId].SyncStatus;
          DialoguePages[pageId].PlayLine();
          SetFlag(SF_CHAANIME + thread->DialoguePageId, true);
          BlockThread;
        }
      });
    } break;
    case 3:   // Wait
    case 4:   // End
    case 13:  // Wait2
    case 14:  // End2
    {
      bool fade = false;
      threadPageHandler([&](uint32_t pageId) {
        SyncType syncType =
            (type == 3) || (type == 13) ? SyncType::Wait : SyncType::End;
        if ((syncType == SyncType::Wait &&
             DialoguePages[pageId].SyncStatus == Playing) ||
            (syncType == SyncType::End &&
             DialoguePages[pageId].SyncStatus == Hiding)) {
          fade = true;
          bool waiting = InstMesSyncMain(pageId, syncType);
          if (!waiting) {
            ++DialoguePages[pageId].SyncStatus;
          }
        }
      });
      if (fade) {
        ResetInstruction;
        BlockThread;
      }
    } break;
    case 20: {
      bool fading = false;
      for (DialoguePage& page : DialoguePages) {
        if (page.SyncStatus != Stopped &&
            page.GetState() != DialoguePage::State::Shown) {
          fading = true;
        }
      }
      if (fading) {
        ResetInstruction;
        BlockThread;
      }
    } break;
  }
}
VmInstruction(InstMesSetID) {
  StartInstruction;

  int dialoguePageId = 0;

  PopUint8(type);
  uint8_t kind = type & 0x7F;
  switch (kind) {
    case 0: {  // SetSavePointPage0
      if (Profile::Vm::UseReturnIds) {
        PopUint16(savePointId);
        if (!GetFlag(SF_MESSAVEPOINT_SSP)) {
          SaveSystem::SetCheckpointId(savePointId);
        }
        ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                   "STUB instruction MesSetID(type: SetSavePoint, savePointId: "
                   "{:d})\n",
                   savePointId);
      } else {
        ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                   "STUB instruction MesSetID(type: SetSavePoint)\n");
      }
    } break;
    case 1: {  // SetSavePointForPage
      PopUint16(savePointId);
      dialoguePageId = ExpressionEval(thread);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction MesSetID(type: SetSavePoint1, "
                 "savePointId: {:d}, arg1: {:d})\n",
                 savePointId, dialoguePageId);
      if (!GetFlag(SF_MESSAVEPOINT_SSP + dialoguePageId)) {
        SaveSystem::SetCheckpointId(savePointId);
      }
    } break;
    case 2: {  // SetPage
      dialoguePageId = ExpressionEval(thread);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction MesSetID(type: SetPage, dialoguePageId: {:d})\n",
          dialoguePageId);
    } break;
  }

  thread->DialoguePageId = dialoguePageId;
}

VmInstruction(InstMesSetIDOld) {
  StartInstruction;

  int dialoguePageId = 0;

  PopUint8(type);
  if (type != 0) {
    dialoguePageId = ExpressionEval(thread);
  }
  thread->DialoguePageId = dialoguePageId;
}

VmInstruction(InstMesCls) {
  StartInstruction;

  const auto hidePage = [](DialoguePage& page) -> bool {
    const DialoguePage::State pageState = page.GetState();
    switch (pageState) {
      using enum DialoguePage::State;
      case Initial:
      case Hidden:
        return true;

      case Showing:
      case Hiding:
        return false;

      case Shown:
        if (GetFlag(SF_MESALLSKIP)) {
          page.Typewriter.Finish(AnimationDirection::Out);
          page.RenderName = false;
        } else {
          page.Typewriter.StartOut();
        }
        return false;

      default:
        assert(false && "Missing page state");
        return true;
    }
  };

  enum class ClsType : uint8_t {
    HidePage = 0,
    HideAllPages = 1,
    SetThreadPageAndReset = 2,
    ResetAllPageFlags = 3,
    ClearCurrentPageIfWindowHidden = 4,
    ClearAllPagesIfWindowHidden = 5,
    ClearPage = 6,
    ClearAllPages = 7,
    AdvancePage = 8,
  };

  PopUint8(type);
  switch (static_cast<ClsType>(type)) {
    using enum ClsType;

    case HidePage: {
      PopExpression(pageId);
      DialoguePage& page = DialoguePages[pageId];

      const bool hidden = hidePage(page);
      if (hidden) {
        SetFlag(SF_SHOWWAITICON + pageId, false);
        page.Glyphs.clear();
        page.RubyChunks.clear();
        page.CurrentLineTop = 0.0f;
        page.RenderName = false;
      } else {
        ResetInstruction;
        BlockThread;
      }
    } break;

    case HideAllPages: {
      bool allHidden = true;
      for (DialoguePage& page : DialoguePages) {
        allHidden &= hidePage(page);
      }

      if (allHidden) {
        for (DialoguePage& page : DialoguePages) {
          page.RenderName = false;
          page.Glyphs.clear();
          page.RubyChunks.clear();
          page.CurrentLineTop = 0.0f;
          SetFlag(SF_SHOWWAITICON + page.Id, false);
        }
      } else {
        ResetInstruction;
        BlockThread;
      }

    } break;

    case SetThreadPageAndReset: {
      PopExpression(pageId);
      SetFlag(SF_MESWINDOW0OPENFL + pageId, false);
      SetFlag(SF_SHOWWAITICON + pageId, false);
      DialoguePages[pageId].RenderName = false;
      thread->DialoguePageId = pageId;
    } break;

    case ResetAllPageFlags: {
      for (DialoguePage& page : DialoguePages) {
        page.RenderName = false;
        SetFlag(SF_MESWINDOW0OPENFL + page.Id, false);
        SetFlag(SF_SHOWWAITICON + page.Id, false);
      }
    } break;

    case ClearCurrentPageIfWindowHidden: {
      DialoguePage& page = DialoguePages[thread->DialoguePageId];
      if (ScrWork[SW_MESWINDOW0ALPHA + page.Id] == 0) {
        page.Clear();
      } else {
        ResetInstruction;
        BlockThread;
      }
    } break;

    case ClearAllPagesIfWindowHidden: {
      bool allHidden = true;
      for (size_t pageIdx = 0; pageIdx < DialoguePages.size(); pageIdx++) {
        allHidden &= (ScrWork[SW_MESWINDOW0ALPHA + pageIdx] == 0);
      }

      if (allHidden) {
        for (DialoguePage& page : DialoguePages) {
          page.Clear();
        }
      } else {
        ResetInstruction;
        BlockThread;
      }
    } break;

    case ClearPage: {
      PopExpression(pageId);
      SetFlag(SF_MESWINDOW0OPENFL + pageId, false);
      ScrWork[SW_MESWINDOW0ALPHA + pageId] = 0;
      DialoguePages[pageId].Clear();
    } break;

    case ClearAllPages: {
      for (DialoguePage& page : DialoguePages) {
        SetFlag(SF_MESWINDOW0OPENFL + page.Id, false);
        ScrWork[SW_MESWINDOW0ALPHA + page.Id] = 0;
        page.Clear();
      }
    } break;

    case AdvancePage: {
      PopExpression(pageId);
      DialoguePage& page = DialoguePages[pageId];

      const bool hidden = hidePage(page);
      if (!hidden) {
        ResetInstruction;
        BlockThread;
        break;
      }

      SetFlag(SF_SHOWWAITICON + pageId, false);
      SetFlag(SF_MESCLEAR0 + pageId, true);

      SaveSystem::SetLineRead(
          static_cast<size_t>(ScrWork[SW_SCRIPTID + pageId * 2]),
          static_cast<size_t>(ScrWork[SW_LINEID + pageId * 2]));

      if (!GetFlag(SF_REVADDDISABLE)) {
        page.PushBacklogEntry();
      }
    } break;

    default: {
      if (!(type & 1)) {
        PopExpression(pageId);
        ImpLogSlow(
            LogLevel::Error, LogChannel::VM,
            "Unexpected instruction type MesCls(type: {:d}, pageId: {:d})\n",
            type, pageId);
      } else {
        ImpLogSlow(LogLevel::Error, LogChannel::VM,
                   "Unexpected instruction type MesCls(type: {:d})\n", type);
      }
    } break;
  }
}
VmInstruction(InstMesVoiceWait) {
  StartInstruction;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction MesVoiceWait()\n");
}
VmInstruction(InstMes) {
  StartInstruction;

  DialoguePage& dialoguePage = DialoguePages[thread->DialoguePageId];
  uint32_t scriptId = LoadedScriptMetas[thread->ScriptBufferId].Id;

  // After loading a save we need to make sure the textbox is actually shown
  if (dialoguePage.FadeAnimation.IsOut() &&
      GetFlag(thread->DialoguePageId + SF_MESWINDOW0OPENFL)) {
    dialoguePage.FadeAnimation.StartIn(true);
  }
  SaveSystem::SetQSavedOnCurrentLine(false);

  PopUint8(type);
  bool voiced = type & 1;
  bool acted = type & (1 << 1);
  bool sync = type & (1 << 3);
  bool MSB = type & (1 << 7);

  std::optional<int> audioId;
  int animationId = 0;
  if (voiced) audioId = ExpressionEval(thread);
  PopExpression(characterId);
  if (acted) animationId = ExpressionEval(thread);

  if (characterId == 32) characterId = 0;
  int lineId;
  if (MSB) {
    lineId = ExpressionEval(thread);
  } else {
    PopUint16(lineIdTemp);
    lineId = lineIdTemp;
  }

  uint32_t line = MSB ? MsbGetStrAddress(thread->ScriptBufferId, lineId)
                      : ScriptGetStrAddress(thread->ScriptBufferId, lineId);

  if (!(ScrWork[Profile::Vm::ScrWorkMesStructSize * thread->DialoguePageId +
                SW_MESWIN0TYPE] &
        +MesWinTypeBit::DontSkipIfRead)) {
    SetFlag(SF_MESREAD, SaveSystem::IsLineRead(scriptId, lineId));
    ChkMesSkip();
  }

  ScrWork[2 * dialoguePage.Id + SW_LINEID] = lineId;
  ScrWork[2 * dialoguePage.Id + SW_SCRIPTID] = scriptId;

  if (GetFlag(SF_MESALLSKIP)) {
    Audio::Channels[Audio::AC_VOICE0]->Stop(0.0f);
  }

  if (GetFlag(SF_MESCLEAR0 + dialoguePage.Id)) {
    dialoguePage.Clear();
    SetFlag(SF_MESCLEAR0 + dialoguePage.Id, false);
  }

  uint32_t oldIp = thread->IpOffset;
  thread->IpOffset = line;

  // TODO: dirty hack, need to actually decouple AddString and UseMSBBuffers
  // field of the thread
  auto prev = thread->UseMSBBuffers;
  thread->UseMSBBuffers = MSB;
  dialoguePage.AddString(thread, audioId, acted, animationId, characterId);
  thread->UseMSBBuffers = prev;

  ResetInstruction;
  if (!GetFlag(SF_MESSAVEPOINT_SSP + thread->DialoguePageId)) {
    if (!(ScrWork[thread->DialoguePageId * Profile::Vm::ScrWorkMesStructSize +
                  SW_MESWIN0TYPE] &
          +MesWinTypeBit::DisableAutoSave) &&
        ScrWork[SW_TITLE] != 0xffff) {
      SaveSystem::SaveMemory();
      SetFlag(SF_SAVECAPTURE, 1);
      SetFlag(SF_AUTOSAVEENABLE, 1);
    }
  } else {
    SetFlag(SF_MESSAVEPOINT_SSP + thread->DialoguePageId, 0);
  }

  thread->IpOffset = oldIp;

  if (sync) {
    dialoguePage.SyncEnabled = true;
  } else {
    SetFlag(SF_CHAANIME + thread->DialoguePageId, true);
    dialoguePage.PlayLine();
  }
  SetFlag(SF_SYSTEMMENUDISABLE2, false);
}
VmInstruction(InstMesMain) {
  using enum DialoguePage::AdvanceMethodType;
  using enum DialoguePage::State;

  StartInstruction;
  PopUint8(type);
  DialoguePage& currentPage = DialoguePages[thread->DialoguePageId];
  DialoguePage::State pageState = currentPage.GetState();

  if (pageState == Initial || pageState == Showing) {
    if (GetFlag(SF_MESALLSKIP)) {
      currentPage.Typewriter.Finish(AnimationDirection::In);
    } else if (GetFlag(SF_MESSKIP)) {
      currentPage.Typewriter.CancelRequested = true;
    }
  }

  if (pageState == Initial || pageState == Showing || pageState == Hiding) {
    ResetInstruction;
    BlockThread;
    return;
  }

  if (pageState == Hidden) {
    currentPage.PushBacklogEntry();

    SetFlag(SF_MESCLEAR0 + currentPage.Id, true);

    SaveSystem::SetLineRead(
        static_cast<size_t>(ScrWork[currentPage.Id * 2 + SW_SCRIPTID]),
        static_cast<size_t>(ScrWork[currentPage.Id * 2 + SW_LINEID]));
    return;
  }

  assert(pageState == Shown);

  const bool autoForward = currentPage.AdvanceMethod == AutoForward ||
                           currentPage.AdvanceMethod == AutoForwardSyncVoice;
  if (!autoForward) {
    SetFlag(SF_SYSMENUDISABLE, false);

    if (currentPage.AdvanceMethod == Skip && type != 1) {
      currentPage.PushBacklogEntry();

      return;
    }

    SetFlag(currentPage.Id + SF_SHOWWAITICON, true);
  }

  if (!GetFlag(SF_UIHIDDEN) && GetFlag(SF_MESSKIP)) {
    currentPage.AutoWaitTime = 0.0f;

    if (Profile::ConfigSystem::SkipVoice) {
      Audio::Channels[Audio::AC_VOICE0]->Stop(0.0f);
    }
  }

  const Audio::AudioChannelState audioState =
      Audio::Channels[Audio::AC_VOICE0]->GetState();
  const bool audioPlaying = audioState == Audio::ACS_Playing ||
                            audioState == Audio::ACS_FadingIn ||
                            audioState == Audio::ACS_FadingOut;
  if (currentPage.AutoWaitTime == 0.0f &&
      (!audioPlaying || GetFlag(SF_MESSKIP))) {
    if (Profile::ConfigSystem::SkipVoice) {
      Audio::Channels[Audio::AC_VOICE0]->Stop(0.0f);
    }

    SetFlag(currentPage.Id + SF_CHAANIME, false);
    SetFlag(currentPage.Id + SF_SHOWWAITICON, false);
    SetFlag(SF_SYSMENUDISABLE, true);

    const bool nvlDontClear = currentPage.AdvanceMethod != PresentClear &&
                              type != 1 && currentPage.GetMode() == DPM_NVL;
    const bool advanceWithoutHiding =
        currentPage.AdvanceMethod == Present0x18 ||
        currentPage.AdvanceMethod == AutoForward || nvlDontClear;
    if (advanceWithoutHiding) {
      if (!nvlDontClear) {
        SetFlag(SF_MESCLEAR0 + currentPage.Id, true);
      }

      SaveSystem::SetLineRead(
          static_cast<size_t>(ScrWork[currentPage.Id * 2 + SW_SCRIPTID]),
          static_cast<size_t>(ScrWork[currentPage.Id * 2 + SW_LINEID]));

      currentPage.PushBacklogEntry();

      BlockThread;
      return;
    }

    if (GetFlag(SF_MESALLSKIP)) {
      currentPage.Typewriter.Finish(AnimationDirection::Out);
    } else {
      currentPage.Typewriter.StartOut(true);
    }
  }

  BlockThread;
  ResetInstruction;
}
VmInstruction(InstSetMesModeFormat) {
  using namespace Impacto::Profile::Dialogue;

  struct RawMesModeInfo {
    uint16_t DisplayMode;
    uint16_t WindowId;
    int16_t WindowPosX;
    int16_t WindowPosY;
    uint16_t NameDispMode;
    uint16_t MaxNameWidth;
    int16_t NamePosX;
    int16_t NamePosY;
    uint16_t NameGlyphWidth;
    uint16_t NameGlyphHeight;
    uint16_t MaxLineWidth;
    uint16_t WaitIconDispMode;
    uint16_t WaitIconPosX;
    uint16_t WaitIconPosY;
    uint16_t TextGlyphWidth;
    uint16_t TextGlyphHeight;
    uint16_t RubyGlyphWidth;
    uint16_t RubyGlyphHeight;
    uint16_t LineSpacing;
    uint16_t RubyLineSpacing;
    uint16_t RubyDispMode;
    uint16_t LinefeedSpacing;
    uint16_t NamePosFlags;
    uint16_t NameLengthL;  // Idk what this is either
  };

  StartInstruction;
  PopExpression(id);
  PopLocalLabel(modeDataAdr);

  Sc3VmThread dummy;
  dummy.IpOffset = modeDataAdr;
  dummy.ScriptBufferId = thread->ScriptBufferId;
  const RawMesModeInfo info = std::bit_cast<RawMesModeInfo>(
      *reinterpret_cast<const uint8_t(*)[sizeof(RawMesModeInfo)]>(
          dummy.GetIp()));

  const auto profileFields = ProfileTextModesInfoFields[id];
  TextModeInfo& dest = TextModesInfo[id];
  const auto setVal = [profileFields](auto& member, TextModeInfoFieldFlags flag,
                                      auto&& value) {
    // Only set the value if it is not already set through the profile
    if (!profileFields.test(flag)) {
      member = std::forward<decltype(value)>(value);
    }
  };

  const glm::vec2 designScale = {Profile::Game::DesignWidth / 1280.0f,
                                 Profile::Game::DesignHeight / 720.0f};

  using enum TextModeInfoFieldFlags;
  setVal(dest.DisplayMode, DisplayMode, info.DisplayMode);
  setVal(dest.WindowId, WindowId, info.WindowId);
  setVal(dest.WindowPos, WindowPos,
         glm::vec2(info.WindowPosX, info.WindowPosY) * designScale);
  setVal(dest.NameDispMode, NameDispMode,
         static_cast<TextModeInfo::NameDispModeType>(info.NameDispMode));
  setVal(dest.MaxNameWidth, MaxNameWidth, info.MaxNameWidth * designScale.x);
  setVal(dest.NamePos, NamePos,
         glm::vec2(info.NamePosX, info.NamePosY) * designScale);
  setVal(dest.NameGlyphSize, NameGlyphSize,
         glm::vec2(info.NameGlyphWidth, info.NameGlyphHeight) * designScale);
  setVal(dest.MaxLineWidth, MaxLineWidth, info.MaxLineWidth * designScale.x);
  setVal(
      dest.WaitIconDispMode, WaitIconDispMode,
      static_cast<TextModeInfo::WaitIconDispModeType>(info.WaitIconDispMode));
  setVal(dest.WaitIconPos, WaitIconPos,
         glm::vec2(info.WaitIconPosX, info.WaitIconPosY) * designScale);
  setVal(dest.TextGlyphSize, TextGlyphSize,
         glm::vec2(info.TextGlyphWidth, info.TextGlyphHeight) * designScale);
  setVal(dest.RubyGlyphSize, RubyGlyphSize,
         glm::vec2(info.RubyGlyphWidth, info.RubyGlyphHeight) * designScale);
  setVal(dest.LineSpacing, LineSpacing, info.LineSpacing * designScale.y);
  setVal(dest.RubyLineSpacing, RubyLineSpacing,
         info.RubyLineSpacing * designScale.y);
  setVal(dest.AlwaysAddRubySpacing, AlwaysAddRubySpacing,
         info.RubyDispMode != 0);
  setVal(dest.LinefeedSpacing, LinefeedSpacing,
         info.LinefeedSpacing * designScale.y);
  setVal(
      dest.NameAlignment, NameAlignment,
      static_cast<TextModeInfo::NameAlignmentType>(info.NamePosFlags & 0b111));
  setVal(dest.UseNameLengthL, UseNameLengthL,
         (info.NamePosFlags & 0b1000) != 0);
  setVal(dest.NameLengthL, NameLengthL, info.NameLengthL);
}  // namespace Vm
VmInstruction(InstSetNGmoji) {
  StartInstruction;
  PopString(endingPuncts);
  PopString(startingPuncts);

  StringToken::AddFlags({ScriptBuffers, thread->ScriptBufferId, startingPuncts},
                        +CharacterTypeFlags::WordStartingPunct);
  StringToken::AddFlags({ScriptBuffers, thread->ScriptBufferId, endingPuncts},
                        +CharacterTypeFlags::WordEndingPunct);
}
VmInstruction(InstMesRev) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:  // DispInit
      UI::BacklogMenuPtr->Show();
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: DispInit)\n");
      break;
    case 1:  // Main
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: Main)\n");
      break;
    case 2:  // AllCls
      UI::BacklogMenuPtr->Hide();
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: AllCls)\n");
      break;
    case 3:  // ChkLoad
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: ChkLoad)\n");
      break;
    case 4:  // SAVELoad
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: SAVELoad)\n");
      break;
    case 5:  // SoundUnk
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: SoundUnk)\n");
      break;
    case 0xA:  // DispInit
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetMesModeFormat(type: DispInit)\n");
      break;
  }
}
VmInstruction(InstMessWindow) {
  StartInstruction;

  const auto getCurrentPage = [&thread]() -> DialoguePage& {
    return DialoguePages[thread->DialoguePageId];
  };

  PopUint8(type);
  switch (type) {
    case 0: {  // Close
      DialoguePage& currentPage = getCurrentPage();

      if (!currentPage.FadeAnimation.IsOut()) {
        currentPage.Hide();
        SetFlag(currentPage.Id + SF_MESWINDOW0OPENFL, 0);
        BlockThread;
      }
    } break;

    case 1: {  // Open
      DialoguePage& currentPage = getCurrentPage();

      if (!currentPage.FadeAnimation.IsIn()) {
        currentPage.Clear();
        currentPage.Show();

        SetFlag(SF_SYSTEMMENUDISABLE2, true);
        SetFlag(thread->DialoguePageId + SF_MESWINDOW0OPENFL, 1);

        if (ScrWork[currentPage.Id + SW_MESWINDOW0ALPHA] == 0) {
          currentPage.RenderName = false;
        }

        BlockThread;
      }
    } break;

    case 2:    // OpenedWait
    case 3: {  // ClosedWait
      const bool fading = getCurrentPage().FadeAnimation.IsPlaying();
      SetFlag(SF_SYSTEMMENUDISABLE2, fading);

      if (fading) {
        ResetInstruction;
        BlockThread;
      }
    } break;

    case 4: {  // HideCurrent04
      DialoguePage& currentPage = getCurrentPage();

      SetFlag(currentPage.Id + SF_MESWINDOW0OPENFL, false);

      currentPage.Hide();
      currentPage.FadeAnimation.Finish();
      currentPage.Clear();
      currentPage.RenderName = false;
    } break;

    case 5: {  // CloseEx
      PopExpression(dialoguePageId);
      thread->DialoguePageId = dialoguePageId;
      DialoguePage& currentPage = getCurrentPage();

      if (!currentPage.FadeAnimation.IsOut()) {
        currentPage.Hide();
        currentPage.Name.clear();
        SetFlag(dialoguePageId + SF_MESWINDOW0OPENFL, 0);
        BlockThread;
      }
    } break;

    case 6: {  // OpenEx
      PopExpression(dialoguePageId);
      thread->DialoguePageId = dialoguePageId;
      DialoguePage& currentPage = getCurrentPage();

      if (!currentPage.FadeAnimation.IsIn()) {
        if (ScrWork[dialoguePageId + SW_MESWINDOW0ALPHA] == 0) {
          currentPage.RenderName = false;
        }

        currentPage.Clear();
        currentPage.Show();

        SetFlag(dialoguePageId + SF_MESWINDOW0OPENFL, true);
        BlockThread;
      }
    } break;

    case 7: {  // FastClose
      PopExpression(dialoguePageId);
      thread->DialoguePageId = dialoguePageId;

      SetFlag(dialoguePageId + SF_MESWINDOW0OPENFL, false);
      ScrWork[dialoguePageId + SW_MESWINDOW0ALPHA] = 0;
      getCurrentPage().Name.clear();
    } break;
  }
}
VmInstruction(InstSel) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // SelInit
      if (Profile::Vm::GameInstructionSet == InstructionSet::Dash ||
          Profile::Vm::GameInstructionSet == InstructionSet::CC ||
          Profile::Vm::GameInstructionSet == InstructionSet::LCCSwitch ||
          Profile::Vm::GameInstructionSet == InstructionSet::MO8) {
        PopUint16(savepointid);
        // SF_MESSAVEPOINT_SSP + dialog page's field 5 in decompile?
        // if (GetFlag(SF_MESSAVEPOINT_SSP + thread->DialoguePageId) == 0) {
        if (ScrWork[SW_TITLE] != 0xffff) {
          SaveSystem::SetCheckpointId(savepointid);
          SaveSystem::SaveMemory();
          SetFlag(SF_SAVECAPTURE, 1);
          SetFlag(SF_AUTOSAVEENABLE, 1);
          BlockThread;
        }
        // }
      }
      PopExpression(arg1);
      UI::SelectionMenuPtr->InitSelectionMenu((bool)arg1);
      // SaveIconDisplay::Show();
      break;
    }
    case 1: {
      PopUint16(selStrNum);
      auto offset = ScriptGetStrAddress(thread->ScriptBufferId, selStrNum);
      UI::SelectionMenuPtr->AddChoice(
          {.BufferId = thread->ScriptBufferId, .IpOffset = offset});
      break;
    }
    case 0x81: {
      PopMsbString(line);
      UI::SelectionMenuPtr->AddChoice(
          {.BufferId = thread->ScriptBufferId, .IpOffset = line});
    } break;
    case 2: {
      PopUint16(selStrNum);
      auto offset = ScriptGetStrAddress(thread->ScriptBufferId, selStrNum);
      UI::SelectionMenuPtr->AddChoice(
          {.BufferId = thread->ScriptBufferId, .IpOffset = offset});
      PopExpression(arg2);
      break;
    }
    case 0x82: {
      PopMsbString(line);
      PopExpression(arg2);
      UI::SelectionMenuPtr->AddChoice(
          {.BufferId = thread->ScriptBufferId, .IpOffset = line});
    } break;
  }
}
VmInstruction(InstSelect) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {
      UI::SelectionMenuPtr->Show();
      bool flag = GetFlag(SF_SAVEDISABLE);
      SetFlag(thread->DialoguePageId + SF_CHAANIME, false);
      if (ScrWork[SW_AUTOSAVERESTART] == 2) {
        thread->IpOffset += 12;
        return;
      } else {
        SaveSystem::SaveMemory();
        auto quicksaveEntries = SaveSystem::GetQuickSaveOpenSlot();
        if (!flag && quicksaveEntries.has_value()) {
          SaveIconDisplay::ShowFor(2.4f);
          SaveSystem::FlushWorkingSaveEntry(SaveSystem::SaveType::Quick,
                                            *quicksaveEntries, 2);
          SaveSystem::SaveThumbnailData();
        }
        ScrWork[SW_AUTOSAVERESTART] = 0;
        if (quicksaveEntries == -1) {
          thread->IpOffset += 12;
        }
      }
      SetFlag(SF_SYSMENUDISABLE, 0);
    } break;
    case 1: {
      if (!UI::SelectionMenuPtr->ChoiceMade) {
        ResetInstruction;
        BlockThread;
      } else if (UI::SelectionMenuPtr->State == UI::MenuState::Hiding) {
        ResetInstruction;
        BlockThread;
      } else if (UI::SelectionMenuPtr->State == UI::MenuState::Hidden) {
        BlockThread;
      } else {
        UI::SelectionMenuPtr->Hide();
        ResetInstruction;
        BlockThread;
      }
    } break;
    case 2: {
      PopExpression(arg1);
      ScrWork[arg1] = UI::SelectionMenuPtr->SelectedChoiceId;
    } break;
  }
}
VmInstruction(InstSysSel) {
  StartInstruction;
  PopUint8(type);  // TODO: Implement type 0, 1
  if (type >= 2) {
    PopString(arg1);
    (void)arg1;
  }
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SysSel(type: {:d})\n", type);
}
VmInstruction(InstSysSelect) {
  StartInstruction;
  PopUint8(type);
  switch (type & 0xF) {
    case 0: {
      PopExpression(arg1);
      PopExpression(arg2);
      ImpLogSlow(
          LogLevel::Warning, LogChannel::VMStub,
          "STUB instruction SysSelect(type: {:d}, arg1: {:d}, arg2: {:d})\n",
          type, arg1, arg2);
    } break;
    case 2:
    case 3: {
      PopExpression(destination);
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SysSelect(type: {:d}, destination: {:d})\n",
                 type, destination);
    } break;
  }
}
VmInstruction(InstSetTextTable) {
  StartInstruction;
  PopExpression(id);
  PopLocalLabel(tableDataAdr);
  TextTable[id].scriptBufferId = static_cast<uint8_t>(thread->ScriptBufferId);
  TextTable[id].labelAdr = tableDataAdr;
  ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
             "STUB instruction SetTextTable(id: {:d})\n", id);
}
VmInstruction(InstSetDic) {
  StartInstruction;
  PopUint8(type);
  PopExpression(tipId);
  switch (type) {
    case 0:    // NewTip
    case 1: {  // Check
      bool tipLocked = TipsSystem::GetTipLockedState(tipId);
      if (tipLocked) {
        TipsSystem::SetTipLockedState(tipId, false);
        TipsSystem::SetTipNewState(tipId, true);
        TipsSystem::SetTipUnreadState(tipId, true);
        TipsNotification::AddTip(tipId);
        TipsSystem::GetNewTipsIndices().push_back(static_cast<uint16_t>(tipId));
      }
      if (type == 1) {
        PopExpression(flagId);
        SetFlag(flagId, tipLocked);
      }
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetDic(type: NewTip, tipId: {:d})\n", tipId);
    } break;
    case 2:  // SetDic02
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction SetDic(type: {:d}, tipId: {:d})\n", type,
                 tipId);
      break;
  }
}
VmInstruction(InstEncyclopedia) {
  StartInstruction;
  PopExpression(tipId);
  if (TipsSystem::GetTipLockedState(tipId)) {
    TipsSystem::SetTipLockedState(tipId, false);
    TipsNotification::AddTip(tipId);
    TipsSystem::GetNewTipsIndices().push_back(static_cast<uint16_t>(tipId));
  }
}
VmInstruction(InstNameID) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0:
      if (Profile::Vm::GameInstructionSet == InstructionSet::CC ||
          Profile::Vm::GameInstructionSet == InstructionSet::LCCSwitch ||
          Profile::Vm::GameInstructionSet == InstructionSet::MO8 ||
          Profile::Vm::GameInstructionSet == InstructionSet::CHN) {
        PopLocalLabel(namePlateDataBlock);
        Sc3Stream namePlateData(
            &ScriptBuffers[thread->ScriptBufferId][namePlateDataBlock]);
        InitNamePlateData(namePlateData);
      } else if (Profile::Vm::GameInstructionSet == InstructionSet::MO6TW) {
        PopExpression(arg1);
        PopExpression(arg2);
        PopExpression(arg3);
      }
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction NameID(type: {:d})\n", type);
      break;
    case 1: {
      PopLocalLabel(namePlateDataBlock);
      (void)namePlateDataBlock;
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction NameID(type: {:d})\n", type);
    } break;
    case 2:
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction NameID(type: {:d})\n", type);
      break;
  }
}
VmInstruction(InstTips) {
  StartInstruction;
  PopUint8(type);
  switch (type) {
    case 0: {  // TipsDataInit
      PopUint16(tipsLabelNum);
      uint32_t tipsDataAdr =
          ScriptGetLabelAddress(thread->ScriptBufferId, tipsLabelNum);
      if (Profile::Vm::GameInstructionSet == InstructionSet::MO8 ||
          Profile::Vm::GameInstructionSet == InstructionSet::CHN) {
        PopLocalLabel(tipsDataAdr1);
        (void)tipsDataAdr1;
      }
      uint32_t tipsDataSize =
          ScriptGetLabelSize(thread->ScriptBufferId, tipsLabelNum);
      TipsSystem::DataInit(thread->ScriptBufferId, tipsDataAdr, tipsDataSize);
      if (Profile::Vm::GameInstructionSet == InstructionSet::CC ||
          Profile::Vm::GameInstructionSet == InstructionSet::LCCSwitch) {
        UI::TipsMenuPtr->Init();
      }
    } break;
    case 1:  // TipsInit
      TipsSystem::UpdateTipRecords();
      if ((Profile::Vm::GameInstructionSet != InstructionSet::CC ||
           Profile::Vm::GameInstructionSet == InstructionSet::LCCSwitch) &&
          UI::TipsMenuPtr) {
        UI::TipsMenuPtr->Init();
      }
      break;
    case 2:  // TipsMain
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Tips(type: TipsMain)\n");
      break;
    case 3:  // TipsEnd
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Tips(type: TipsEnd)\n");
      break;
    case 4:  // TipsSet
      TipsSystem::UpdateTipRecords();
      break;
    case 5:
      TipsSystem::UpdateTipRecords();
      if ((Profile::Vm::GameInstructionSet != InstructionSet::CC ||
           Profile::Vm::GameInstructionSet == InstructionSet::LCCSwitch) &&
          UI::TipsMenuPtr) {
        UI::TipsMenuPtr->Init();
      }
      break;
    case 10:  // Tips_ProfSetXboxEvent
      ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
                 "STUB instruction Tips(type: Tips_ProfSetXboxEvent)\n");
      break;
  }
}
VmInstruction(InstSetRevMes) {
  StartInstruction;
  PopUint8(type);

  bool voiced = type & (1 << 0);
  bool savep = (type & (1 << 1)) && voiced;
  bool expression = type & (1 << 7);

  if ((type & (1 << 1)) || expression) {
    ImpLogSlow(LogLevel::Warning, LogChannel::VMStub,
               "STUB instruction SetRevMes(type: {:d})\n", type);
  }

  std::optional<int> audioId;
  int animationId = 0;
  if (voiced) {
    audioId = ExpressionEval(thread);
    animationId = ExpressionEval(thread);
  }

  if (savep) {
    // TODO: use?
    ExpressionEval(thread);
  }

  int lineId;
  if (expression) {
    lineId = ExpressionEval(thread);
  } else {
    PopUint16(lineIdTemp);
    lineId = lineIdTemp;
  }

  uint32_t line = expression
                      ? MsbGetStrAddress(thread->ScriptBufferId, lineId)
                      : ScriptGetStrAddress(thread->ScriptBufferId, lineId);

  uint32_t scriptId = LoadedScriptMetas[thread->ScriptBufferId].Id;

  SaveSystem::SetLineRead(scriptId, lineId);
  UI::BacklogMenuPtr->AddMessage(
      {.BufferId = thread->ScriptBufferId, .IpOffset = line}, audioId,
      animationId);
}

void ChkMesSkip() {
  bool mesSkip = false;
  bool mesAllSkip = false;

  if (Profile::Vm::GameInstructionSet != InstructionSet::CHLCC &&
      ScrWork[SW_SYSMESALPHA] != 255) {
    SkipModeEnabled = false;
    AutoModeEnabled = false;
  }

  if ((ScrWork[SW_GAMESTATE] & 0b101) == 0b001 && !GetFlag(SF_UIHIDDEN)) {
    mesSkip |= Interface::GetControlState(Interface::CT_NextMessage);

    if (Interface::GetControlState(Interface::CT_ForceSkip,
                                   Interface::InputDownType::IsDown)) {
      mesSkip = true;
      mesAllSkip = true;
    };

    if (Interface::PADinputButtonWentDown & Interface::PADcustom[8]) {
      SkipModeEnabled = !SkipModeEnabled;
    }

    if (Interface::PADinputButtonWentDown & Interface::PADcustom[9]) {
      AutoModeEnabled = !AutoModeEnabled;
    }

    if (SkipModeEnabled &&
        (!Profile::ConfigSystem::SkipRead || GetFlag(SF_MESREAD))) {
      mesSkip = true;
      mesAllSkip = true;
    }
  }

  SetFlag(SF_MESSKIP, mesSkip);
  SetFlag(SF_MESALLSKIP, mesAllSkip);
}

}  // namespace Vm

}  // namespace Impacto
