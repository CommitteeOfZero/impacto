#pragma once

#include "dialoguebox.h"
#include "../spriteanimation.h"

namespace Impacto {

enum class WaitIconType : uint8_t {
  None,
  Sprite,
  SpriteAnimation,
  FixedSpriteAnimation,
  Rotating,
  Rotating3D,
};

class WaitIcon {
 public:
  static std::unique_ptr<WaitIcon> Create(const DialoguePage* parentPage);

  virtual ~WaitIcon() = default;

  void Render(glm::vec2 afterLastGlyphPos, glm::vec4 tint);
  virtual void Update(float dt) = 0;

 protected:
  WaitIcon(const DialoguePage* parentPage) : ParentPage(parentPage) {}

  virtual void RenderImpl(glm::vec2 pos, glm::vec4 tint) = 0;

  bool IsVisible() const;

  bool IsKeyIcon() const { return ParentPage == nullptr; }
  const DialoguePage* ParentPage = nullptr;
};

class VoidWaitIcon final : public WaitIcon {
 public:
  VoidWaitIcon(const DialoguePage* parentPage) : WaitIcon(parentPage) {}

  void RenderImpl(glm::vec2 pos, glm::vec4 tint) override {}
  void Update(float dt) override {}
};

class SpriteWaitIcon : public WaitIcon {
 public:
  SpriteWaitIcon(const DialoguePage* parentPage, const Sprite& iconSprite)
      : WaitIcon(parentPage), IconSprite(iconSprite) {}

  virtual void RenderImpl(glm::vec2 pos, glm::vec4 tint) override {
    if (!IsVisible()) return;
    Renderer->DrawSprite(IconSprite.get(), pos, tint);
  }
  void Update(float dt) override {}

 protected:
  std::reference_wrapper<const Sprite> IconSprite;
};

class SpriteAnimationWaitIcon final : public WaitIcon {
 public:
  SpriteAnimationWaitIcon(const DialoguePage* parentPage,
                          const SpriteAnimationDef& animationDef)
      : WaitIcon(parentPage), IconAnimation(animationDef.Instantiate()) {
    IconAnimation.LoopMode = AnimationLoopMode::Loop;
    IconAnimation.StartIn();
  }

  void RenderImpl(glm::vec2 pos, glm::vec4 tint) override {
    if (!IsVisible()) return;
    Renderer->DrawSprite(IconAnimation.CurrentSprite(), pos, tint);
  }
  void Update(float dt) override {
    if (IsVisible()) {
      IconAnimation.Update(dt);
    } else {
      IconAnimation.StartIn(true);
    }
  }

 private:
  SpriteAnimation IconAnimation;
};

class FixedSpriteAnimationWaitIcon final : public WaitIcon {
 public:
  FixedSpriteAnimationWaitIcon(const DialoguePage* parentPage,
                               const FixedSpriteAnimationDef& animationDef)
      : WaitIcon(parentPage), IconAnimation(animationDef.Instantiate()) {
    IconAnimation.LoopMode = AnimationLoopMode::Stop;
  }

  void RenderImpl(glm::vec2 pos, glm::vec4 tint) override {
    if (!IsVisible() && IconAnimation.IsStopped()) return;
    Renderer->DrawSprite(IconAnimation.CurrentSprite(), pos, tint);
  }
  void Update(float dt) override;

 private:
  FixedSpriteAnimation IconAnimation;
};

class RotatingWaitIcon : public WaitIcon {
 public:
  RotatingWaitIcon(const DialoguePage* parentPage, float rotationDuration)
      : WaitIcon(parentPage) {
    RotationAnimation.SetDuration(rotationDuration);
    RotationAnimation.StartIn();
  }

  virtual void RenderImpl(glm::vec2 pos, glm::vec4 tint) override;
  void Update(float dt) override {
    if (IsVisible()) {
      RotationAnimation.Update(dt);
    } else {
      RotationAnimation.StartIn(true);
    }
  }

 protected:
  Animation RotationAnimation;
};

class Rotating3DWaitIcon final : public RotatingWaitIcon {
 public:
  Rotating3DWaitIcon(const DialoguePage* parentPage, float rotationDuration)
      : RotatingWaitIcon(parentPage, rotationDuration) {}

  void RenderImpl(glm::vec2 pos, glm::vec4 tint) override;
};

class ChlccWaitIcon final : public SpriteWaitIcon {
 public:
  ChlccWaitIcon(const DialoguePage* parentPage, const Sprite& iconSprite)
      : SpriteWaitIcon(parentPage, iconSprite) {}

  void RenderImpl(glm::vec2 pos, glm::vec4 tint) override {
    if (!IsVisible()) return;
    Renderer->DrawSprite(IconSprite, pos + glm::vec2(1.0f),
                         {0.0f, 0.0f, 0.0f, tint.a / 2.0f});
    Renderer->DrawSprite(IconSprite, pos, tint);
  }
};

inline std::unique_ptr<WaitIcon> KeyWaitIcon = nullptr;

}  // namespace Impacto
