#pragma once

#include "impacto.h"
#include <glm/glm.hpp>
#include "util.h"

namespace Impacto {

int constexpr ScrWorkSize = 8000;
int constexpr FlagWorkSize = 1000;

inline std::array<int, ScrWorkSize> ScrWork;
inline std::array<uint8_t, FlagWorkSize> FlagWork;

inline float ScrRealToFloat(int scrReal) { return (float)scrReal / 1000.0f; }
inline int FloatToScrReal(float f) { return (int)(f * 1000.0f); }

inline void ScrWorkSetFloat(int id, float f) {
  ScrWork[id] = FloatToScrReal(f);
}
inline float ScrWorkGetFloat(int id) { return ScrRealToFloat(ScrWork[id]); }
inline void ScrWorkSetVec3(int idX, int idY, int idZ, glm::vec3 vec) {
  ScrWorkSetFloat(idX, vec.x);
  ScrWorkSetFloat(idY, vec.y);
  ScrWorkSetFloat(idZ, vec.z);
}
inline glm::vec3 ScrWorkGetVec3(int idX, int idY, int idZ) {
  return glm::vec3(ScrWorkGetFloat(idX), ScrWorkGetFloat(idY),
                   ScrWorkGetFloat(idZ));
}
inline void ScrWorkSetAngle(int id, float radians) {
  ScrWorkSetFloat(id, RadToDeg(NormalizeRad(radians)));
}
inline float ScrWorkGetAngle(int id) {
  return DegToRad(NormalizeDeg(ScrWorkGetFloat(id)));
}
inline void ScrWorkSetAngleVec2(int idX, int idY, glm::vec2 vecRadians) {
  ScrWorkSetAngle(idX, vecRadians.x);
  ScrWorkSetAngle(idY, vecRadians.y);
}
inline glm::vec2 ScrWorkGetAngleVec2(int idX, int idY) {
  return glm::vec2(ScrWorkGetAngle(idX), ScrWorkGetAngle(idY));
}
inline void ScrWorkSetAngleVec3(int idX, int idY, int idZ,
                                glm::vec3 vecRadians) {
  ScrWorkSetAngle(idX, vecRadians.x);
  ScrWorkSetAngle(idY, vecRadians.y);
  ScrWorkSetAngle(idZ, vecRadians.z);
}
inline glm::vec3 ScrWorkGetAngleVec3(int idX, int idY, int idZ) {
  return glm::vec3(ScrWorkGetAngle(idX), ScrWorkGetAngle(idY),
                   ScrWorkGetAngle(idZ));
}
inline glm::vec4 ScrWorkGetColor(int id) { return RgbIntToFloat(ScrWork[id]); }

void SetFlag(uint32_t flagId, uint32_t value);
bool GetFlag(uint32_t flagId);

}  // namespace Impacto