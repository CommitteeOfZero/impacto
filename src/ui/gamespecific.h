#pragma once
namespace Impacto {
namespace UI {
namespace GameSpecific {
void Init();
void Update(float dt);
void RenderMain();
void RenderLayer(uint32_t layer);
void RenderCCButtonGuide();
void UpdateCCButtonGuide(float dt);

}  // namespace GameSpecific
}  // namespace UI
}  // namespace Impacto