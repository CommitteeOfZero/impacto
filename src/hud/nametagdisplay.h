#pragma once

#include <enum.h>
#include <optional>

#include "dialoguebox.h"
#include "../profile/dialogue.h"

namespace Impacto {
namespace NametagDisplay {
using namespace Impacto::Profile::Dialogue;

inline Animation NametagAnimation;

void Init();
void Reset();
void Update(float dt, DialoguePageMode mode);

void UpdateNames(std::optional<uint32_t> nameId,
                 std::optional<uint32_t> prevNameId, DialoguePageMode mode);
void StartHiding();

std::optional<uint32_t> GetNameToDraw();

float GetHidingProgress();
float GetShowingProgress();

bool DialogueCanStartAppearing(DialoguePageMode mode);

inline bool IsHiding() {
  return NametagAnimation.Progress <= NameTagAnimProgressHideOld;
}

inline bool IsShowing() {
  return NametagAnimation.Progress >= NameTagAnimProgressShowNew;
}

}  // namespace NametagDisplay
}  // namespace Impacto