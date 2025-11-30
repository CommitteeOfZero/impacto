#include "nametagdisplay.h"

namespace Impacto {
namespace NametagDisplay {

using namespace Impacto::Profile::Dialogue;

static std::optional<uint32_t> NameId;
static std::optional<uint32_t> PrevNameId;

void Init() {
  if (NametagCurrentType == +NametagType::None) {
    return;
  }

  switch (NametagCurrentType) {
    case NametagType::None:
    case NametagType::Instant: {
      break;
    }
    case NametagType::FadeInPauseOut: {
      NametagAnimation.DurationIn = NameTagDuration;
      NametagAnimation.SkipOnSkipMode = true;
      break;
    }
  }
}

void Update(float dt, DialoguePageMode mode) {
  if (mode != DialoguePageMode::DPM_ADV) {
    return;
  }
  switch (NametagCurrentType) {
    case NametagType::None:
    case NametagType::Instant: {
      break;
    }
    case NametagType::FadeInPauseOut: {
      // if the text can start appearing and there's no nametag we should skip
      // the pause
      if (DialogueCanStartAppearing(mode) && !NameId.has_value() &&
          NametagAnimation.Progress < NameTagAnimProgressShowNew) {
        NametagAnimation.Progress = NameTagAnimProgressShowNew;
      }
      break;
    }
  }
  NametagAnimation.Update(dt);
}

void Reset() {
  NametagAnimation.Progress = 0;
  NametagAnimation.Stop();
  NameId.reset();
  PrevNameId.reset();
}

bool DialogueCanStartAppearing(DialoguePageMode mode) {
  if (mode != DialoguePageMode::DPM_ADV) {
    return true;
  }
  switch (NametagCurrentType) {
    case NametagType::FadeInPauseOut: {
      const bool hasCurrentName = NameId.has_value();
      return (!hasCurrentName && !IsHiding()) ||
             (hasCurrentName && IsShowing());
    }
    default: {
      // None and Instant
      return true;
    }
  }
}

std::optional<uint32_t> GetNameToDraw() {
  std::optional<uint32_t> animatedName = NameId;

  switch (NametagCurrentType) {
    case NametagType::None:  // falls through
    case NametagType::Instant: {
      break;
    }
    case NametagType::FadeInPauseOut: {
      if (NametagAnimation.Progress != 0 && NametagAnimation.Progress != 1 &&
          (NametagAnimation.Progress < NameTagAnimProgressHideOld)) {
        animatedName = PrevNameId;
      }
      break;
    }
  }

  return animatedName;
}

void StartHiding() {
  PrevNameId = NameId;
  NametagAnimation.StartIn(true);
}

void UpdateNames(std::optional<uint32_t> nameId,
                 std::optional<uint32_t> prevNameId, DialoguePageMode mode) {
  if (mode != +DialoguePageMode::DPM_ADV) {
    return;
  }

  switch (NametagCurrentType) {
    case NametagType::None:  // falls through
    case NametagType::Instant: {
      NameId = nameId;
      PrevNameId = prevNameId;
      return;
    }
    case NametagType::FadeInPauseOut: {
      if (NametagAnimation.State == AnimationState::Playing) {
        NameId = nameId;
        return;
      }
      NameId = nameId;
      PrevNameId = prevNameId;
      if (NameId == PrevNameId) {
        // if name state stayed the same, still no name or same name
        NametagAnimation.Finish();
      } else if (NameId.has_value() && !PrevNameId.has_value()) {
        // skip hiding name if there was none
        NametagAnimation.Progress = NameTagAnimProgressShowNew;
        NametagAnimation.StartIn();
      } else {
        // start with hiding prev name
        NametagAnimation.StartIn(true);
      };
    }
  }
}

float GetHidingProgress() {
  switch (NametagCurrentType) {
    case NametagType::FadeInPauseOut: {
      if (NameTagAnimProgressHideOld == 0.0f) {
        return 1.0f;
      }
      return NametagAnimation.Progress / NameTagAnimProgressHideOld;
    }
    default: {
      return 1.0f;
    }
  }
}

float GetShowingProgress() {
  switch (NametagCurrentType) {
    case NametagType::FadeInPauseOut: {
      if (NameTagAnimProgressShowNew == 1.0f) {
        return 1.0f;
      }
      return (NametagAnimation.Progress - NameTagAnimProgressShowNew) /
             (1.0f - NameTagAnimProgressShowNew);
    }
    default: {
      return 1.0f;
    }
  }
}

}  // namespace NametagDisplay
}  // namespace Impacto