#pragma once

#include <ankerl/unordered_dense.h>
#include <map>
#include "texture/texture.h"
#include "spritesheet.h"
#include "loadable.h"
#include "renderer/renderer.h"

namespace Impacto {

enum class LinkDirection { Off, Up, Down, Left, Right, Up3, Down3 };

class Background2D;

struct LinkState {
  Background2D* LinkedBuffer;
  LinkDirection Direction;
  glm::vec2 DisplayCoords;
};

class Background2D : public Loadable<Background2D, bool, uint32_t> {
  friend class Loadable<Background2D, bool, uint32_t>;

 public:
  static void Init();
  static void InitFrameBuffers();

  Sprite BgSprite;

  bool BgEffsLoaded = false;
  constexpr static size_t MaxBgEffCount = 4;
  std::array<Texture, MaxBgEffCount> BgEffTextures;
  std::array<Sprite, MaxBgEffCount> BgEffSprites;
  std::array<ShaderProgramType, MaxBgEffCount> BgEffShaders = {
      ShaderProgramType::Sprite, ShaderProgramType::Sprite,
      ShaderProgramType::Sprite, ShaderProgramType::Sprite};

  glm::vec2 Position = {0.0f, 0.0f};
  glm::vec2 Origin = {0.0f, 0.0f};

  glm::vec2 Scale = {1.0f, 1.0f};
  glm::quat Rotation = glm::quat();

  glm::vec4 Tint = glm::vec4(1.0f);
  int MaskNumber = 0;

  int FadeCount = 0;
  int FadeRange = 0;

  bool Show = false;
  int RenderType = 0;
  std::array<int, 2> Layers;
  std::array<LinkState, 2> Links;

  inline static Background2D* LastRenderedBackground = nullptr;

  inline static std::map<int, std::array<ShaderProgramType, 4>> BgEffShaderMap;
  static std::array<ShaderProgramType, 4> GetBgEffShaders(int bgId) {
    // Unmapped means sprite
    return BgEffShaderMap.contains(bgId)
               ? BgEffShaderMap[bgId]
               : std::array<ShaderProgramType, 4>{
                     ShaderProgramType::Sprite, ShaderProgramType::Sprite,
                     ShaderProgramType::Sprite, ShaderProgramType::Sprite};
  }

  inline static std::map<int, std::array<int, 4>> BgEffTextureIdMap;

  virtual void Render(int layer);
  virtual void UpdateState(int bgId);

  void LoadSolidColor(uint32_t color, int width, int height);

 protected:
  Texture BgTexture;

  bool LoadSync(uint32_t bgId);
  void UnloadSync();
  void MainThreadOnLoad(bool result);

  void LinkBuffers(int linkCode, int currentBufferId);

  bool OnLayer(int layer) {
    return std::find(Layers.begin(), Layers.end(), layer) != Layers.end();
  }

  using BackgroundRenderProc = auto (Background2D::*)() -> void;

  void RenderRegular();
  void RenderMasked();
  void RenderMaskedInverted();
  void RenderFade();

  std::array<BackgroundRenderProc, 30> constexpr static BackgroundRenderTable =
      {
          &Background2D::RenderRegular,         // 0
          &Background2D::RenderFade,            // 1
          &Background2D::RenderRegular,         // 2
          &Background2D::RenderRegular,         // 3
          &Background2D::RenderRegular,         // 4
          &Background2D::RenderRegular,         // 5
          &Background2D::RenderRegular,         // 6
          &Background2D::RenderRegular,         // 7
          &Background2D::RenderRegular,         // 8
          &Background2D::RenderRegular,         // 9
          &Background2D::RenderRegular,         // 10
          &Background2D::RenderRegular,         // 11
          &Background2D::RenderRegular,         // 12
          &Background2D::RenderRegular,         // 13
          &Background2D::RenderRegular,         // 14
          &Background2D::RenderMasked,          // 15
          &Background2D::RenderMaskedInverted,  // 16
          &Background2D::RenderRegular,         // 17
          &Background2D::RenderRegular,         // 18
          &Background2D::RenderRegular,         // 19
          &Background2D::RenderRegular,         // 20
          &Background2D::RenderRegular,         // 21
          &Background2D::RenderRegular,         // 22
          &Background2D::RenderRegular,         // 23
          &Background2D::RenderRegular,         // 24
          &Background2D::RenderRegular,         // 25
          &Background2D::RenderRegular,         // 26
          &Background2D::RenderRegular,         // 27
          &Background2D::RenderRegular,         // 28
          &Background2D::RenderRegular,         // 29
      };
};

class Capture2D : public Background2D {
 public:
  void Render(int layer) override;
  void UpdateState(int capId) override;
};

class BackgroundEffect2D : public Background2D {
 public:
  void Render(int layer) override;
  void UpdateState(int bgId) override;

 private:
  size_t VertexCount = 4;
  std::array<glm::vec2, 4> Vertices;

  glm::vec2 StencilOffset;
};

inline std::array<Background2D, 8> Backgrounds;
inline std::array<Capture2D, 2> Screencaptures;
inline std::array<BackgroundEffect2D, MaxFramebuffers> Framebuffers;
inline Background2D ShaderScreencapture;

inline ankerl::unordered_dense::map<int, Background2D*> Backgrounds2D;

}  // namespace Impacto