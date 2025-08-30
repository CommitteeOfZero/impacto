#pragma once

#include "../../../text.h"

namespace Impacto {
namespace Profile {
namespace CHLCC {
namespace TipsNotification {

inline int HeaderMessageId;
inline glm::vec2 HeaderPosition;
inline float HeaderFontSize;
inline DialogueColorPair HeaderColor;

inline glm::vec2 TextStartPosition;
inline glm::vec2 TextTargetPosition;
inline float TextFontSize;
inline size_t TipNameColorIndex;

inline float SlideTime;
inline float HoldTime;

inline RectF RenderBounds;

void Configure();

}  // namespace TipsNotification
}  // namespace CHLCC
}  // namespace Profile
}  // namespace Impacto
