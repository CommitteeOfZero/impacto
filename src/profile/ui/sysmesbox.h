#pragma once

#include "../../ui/ui.h"
#include "../sprites.h"

namespace Impacto {
namespace Profile {
namespace SysMesBox {

inline Impacto::UI::SysMesBoxType Type = Impacto::UI::SysMesBoxType::None;

inline float TextFontSize;
inline float TextMiddleY;
inline float TextX;
inline float TextLineHeight;
inline float TextMarginY;
inline float AnimationSpeed;
inline float FadeInDuration;
inline float FadeOutDuration;

void Configure();

}  // namespace SysMesBox
}  // namespace Profile
}  // namespace Impacto