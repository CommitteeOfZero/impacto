#pragma once

#include <glm/glm.hpp>

#include "../../3d/scene.h"
#include "../window.h"
#include "../renderer.h"

namespace Impacto {
namespace DirectX9 {

class Scene3D : public IScene3D {
 public:
  Scene3D(DirectX9Window* window, IDirect3DDevice9* device);

  void Init();
  void Shutdown();
  void Update(float dt);
  void Render();

 private:
  bool IsInit = false;

  DirectX9Window* Window;
  IDirect3DDevice9* Device;
};

}  // namespace DirectX9
}  // namespace Impacto