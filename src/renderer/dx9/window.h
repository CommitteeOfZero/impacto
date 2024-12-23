#pragma once

#include "../../impacto.h"
#include "../../util.h"
#include "../windowtemplate.h"

namespace Impacto {
class WindowInterface;
namespace DirectX9 {

class DirectX9Window : public WindowTemplate<DirectX9Window> {
  friend class Impacto::WindowInterface;

 public:
  void Init();
  void SetDimensions(int width, int height, int msaa, float renderScale);
  RectF GetViewport();
  RectF GetScaledViewport();
  void SwapRTs();
  void Update();
  void Draw();
  void Shutdown();

 private:
  void UpdateDimensions();
};

}  // namespace DirectX9
}  // namespace Impacto