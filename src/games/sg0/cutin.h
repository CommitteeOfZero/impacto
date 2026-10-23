#pragma once

#include <array>

namespace Impacto {
namespace SG0 {

// STEINS;GATE 0 cut-ins: a region of a background shown enlarged in another
// rectangle (SG0_01_08 zooms into the EV_C11 flashback). SetCutin(slot,
// enable, bufferMask, bufferMask) switches a slot on for the background in
// that script buffer; the macros in macrosys write the slot's ScrWork block
// at SW_CUTIN1 + 20 * slot: +1..+4 source and +5..+8 destination rectangles
// (1280x720 units), +9 alpha (0..256, faded by the macros), +10 the layer
// it is drawn on.
struct CutinSlot {
  bool Enabled = false;
  int BufferMask = 0;
};

inline std::array<CutinSlot, 4> Cutins;

void RenderCutins(int layer);

}  // namespace SG0
}  // namespace Impacto
