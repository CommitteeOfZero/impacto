#pragma once
#include <optional>

#include "../../animation.h"
#include "../../renderer/renderer.h"
#include "../../ui/menu.h"

namespace Impacto {
namespace UI {
namespace CCLCC {
constexpr inline int GridRowCount = 10;
constexpr inline int GridColCount = 20;

constexpr inline size_t SmokeLayerCount = 2;

class CommonMenu {
 public:
  static void Init();

  static void InitSmokePos();
  static void DrawSmoke(float alpha);

  static void DrawOverlay(float alpha = 1.0f);

 protected:
  using GridVertices =
      std::array<VertexBufferSprites, (GridRowCount + 1) * (GridColCount + 1)>;

  static inline glm::vec2 BGPosition{};

  bool OpenedAsDirect;
  CommonMenu() {};
  CommonMenu(Animation& fadeAnimation);
  void OnShow(float fadeInDuration, float fadeOutDuration,
              Animation& fadeAnimation);

  static void Update(float dt);

  template <bool IsSystem>
  void DrawBgSprite(MenuState state, const Animation& fadeAnimation,
                    std::optional<Sprite> ScreenCap = std::nullopt);

 private:
  static glm::vec2 TransformImageVertex(const glm::vec2 vertex,
                                        glm::mat4 const& transformation,
                                        glm::vec2 const& origin);
  static void TransformImage(CornersQuad const& sprCorners,
                             CornersQuad const& destCorners,
                             glm::mat4 const& transformation, glm::vec2 origin,
                             glm::vec2 sheetBounds,
                             CommonMenu::GridVertices& vertices,
                             glm::vec4 tint);

  static inline GridVertices Vertices;
  static inline int PrevSysMenuCt;
  static inline int PrevSubMenuCt;

  static inline std::array<Animation, SmokeLayerCount> SmokeAnimations;

  static inline Sprite CaptureSprite;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto