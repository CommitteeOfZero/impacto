#include "scene.h"

#include "../../3d/camera.h"
#include "../../../log.h"
#include "../../../workqueue.h"
#include "renderable3d.h"

#include "../../../profile/scene3d.h"

namespace Impacto {
namespace Vulkan {

Scene3D::Scene3D(VulkanWindow* window, VkDevice device, VkRenderPass renderPass,
                 VkCommandBuffer* commandBuffers) {
  Window = window;
  Device = device;
  RenderPass = renderPass;
  CommandBuffers = commandBuffers;
}

void Scene3D::Init() {
  assert(IsInit == false);
  ImpLog(LL_Info, LC_Scene, "Initializing 3D scene system\n");
  IsInit = true;

  Profile::Scene3D::Configure();

  Renderables = new IRenderable3D*[Profile::Scene3D::MaxRenderables];
  for (int i = 0; i < Profile::Scene3D::MaxRenderables; i++) {
    Renderables[i] = new Renderable3D();
  }

  Renderable3D::Init(Window, Device, RenderPass, CommandBuffers);

  MainCamera.Init();

  // clang-format off
  float ScreenFillingTriangle[] = {
      // Position       // UV
      -1.0f, -1.0f,     0.0f, 0.0f,
      3.0f, -1.0f,      2.0f, 0.0f,
      -1.0f, 3.0f,      0.0f, 2.0f
  };
  // clang-format on
  (void)ScreenFillingTriangle;  // Currently unused
}

void Scene3D::Shutdown() {
  if (!IsInit) return;
  if (Renderables) delete[] Renderables;
}

void Scene3D::Update(float dt) {
  for (int i = 0; i < Profile::Scene3D::MaxRenderables; i++) {
    if (Renderables[i]->Status == LS_Loaded) {
      Renderables[i]->Update(dt);
    }
  }
}
void Scene3D::Render() {
  RectF viewport = Window->GetViewport();
  MainCamera.AspectRatio = viewport.Width / viewport.Height;
  MainCamera.Recalculate();

  Renderable3D::BeginFrame(this, &MainCamera);

  for (int i = 0; i < Profile::Scene3D::MaxRenderables; i++) {
    if (Renderables[i]->Status == LS_Loaded &&
        Renderables[i]->StaticModel->Type == ModelType_Background) {
      Renderables[i]->Render();
    }
  }

  for (int i = 0; i < Profile::Scene3D::MaxRenderables; i++) {
    if (Renderables[i]->Status == LS_Loaded &&
        Renderables[i]->StaticModel->Type == ModelType_Character) {
      Renderables[i]->Render();
    }
  }
}

}  // namespace Vulkan
}  // namespace Impacto