#include "introsequence.h"
#include "../../renderer/renderer.h"
#include "../../profile/game.h"
#include "../../profile/games/chlcc/titlemenu.h"
#include "../../background2d.h"

using namespace Impacto::Profile;
using namespace Impacto::Profile::CHLCC::TitleMenu;
using namespace Impacto::Audio;

namespace Impacto {
namespace CHLCC {

IntroSequence::IntroSequence() {
  Texture fallingStarsMaskTexture{};
  fallingStarsMaskTexture.LoadSolidColor(DesignWidth, DesignHeight, 0);
  SpriteSheet fallingStarsMaskSheet(DesignWidth, DesignHeight);
  fallingStarsMaskSheet.Texture = fallingStarsMaskTexture.Submit();
  fallingStarsMaskSheet.IsScreenCap = true;
  FallingStarsMask =
      Sprite(fallingStarsMaskSheet, 0, 0, DesignWidth, DesignHeight);

  // Randomize falling stars
  for (size_t i = 0; i < FallingStarSeeds.size(); i++) {
    auto& [origin, angle] = FallingStarSeeds[i];

    int random = CALCrnd(100);
    origin.x = -120 + i * 110 + (random + 10) * 10;
    origin.y = (random + 10) * -10;

    angle = CALCrnd(8192) / 8192.0f * M_PI * 2;
  }

  PanningAnimation.SetDuration(IntroPanningAnimationDuration);
  IntroAnimation.AddAnimation(&PanningAnimation);

  float starBounceTime =
      IntroAnimation.DurationIn + IntroAfterPanningWaitDuration;
  StarBounceAnimation.SetDuration(IntroStarBounceAnimationDuration);
  IntroAnimation.AddAnimation(&StarBounceAnimation, starBounceTime);

  float explodingStarAnimationTime = IntroAnimation.DurationIn;
  ExplodingStarAnimation.SetDuration(IntroExplodingStarAnimationDuration);
  IntroAnimation.AddAnimation(&ExplodingStarAnimation);

  ExplodingStarRotationAnimation.LoopMode = AnimationLoopMode::Loop;
  ExplodingStarRotationAnimation.SetDuration(
      IntroExplodingStarRotationAnimationDuration);
  IntroAnimation.AddAnimation(&ExplodingStarRotationAnimation,
                              explodingStarAnimationTime,
                              ExplodingStarAnimation.DurationIn);

  float fallingStarAnimationTime = IntroAnimation.DurationIn;
  FallingStarsAnimation.SetDuration(IntroFallingStarsAnimationDuration);
  IntroAnimation.AddAnimation(&FallingStarsAnimation);

  FallingStarsRotationAnimation.LoopMode = AnimationLoopMode::Loop;
  FallingStarsRotationAnimation.SetDuration(
      IntroFallingStarsRotationAnimationDuration);
  IntroAnimation.AddAnimation(&FallingStarsRotationAnimation,
                              fallingStarAnimationTime,
                              FallingStarsAnimation.DurationIn);

  LogoFadeAnimation.SetDuration(IntroLogoFadeAnimationDuration);
  IntroAnimation.AddAnimation(&LogoFadeAnimation);

  float lccLogoAnimationTime = IntroAnimation.DurationIn;
  LCCLogoAnimation.SetDuration(IntroLCCLogoAnimationDuration);
  IntroAnimation.AddAnimation(&LCCLogoAnimation);

  DelusionADVAnimation.SetDuration(IntroDelusionADVAnimationDuration);
  IntroAnimation.AddAnimation(&DelusionADVAnimation, lccLogoAnimationTime,
                              DelusionADVAnimation.DurationIn);

  LogoStarHighlightAnimation.SetDuration(
      IntroLogoStarHighlightAnimationDuration);
  IntroAnimation.AddAnimation(&LogoStarHighlightAnimation);

  float seiraAnimationTime = IntroAnimation.DurationIn;
  SeiraAnimation.SetDuration(IntroSeiraAnimationDuration);
  IntroAnimation.AddAnimation(&SeiraAnimation);

  DelusionADVHighlightAnimation.SetDuration(
      IntroDelusionADVHighlightAnimationDuration);
  IntroAnimation.AddAnimation(&DelusionADVHighlightAnimation,
                              seiraAnimationTime,
                              DelusionADVHighlightAnimation.DurationIn);

  LogoPopOutAnimation.SetDuration(IntroLogoPopOutAnimationDuration);
  IntroAnimation.AddAnimation(
      &LogoPopOutAnimation, seiraAnimationTime + IntroLogoPopOutAnimationDelay,
      LogoPopOutAnimation.DurationIn);

  CopyrightAnimation.SetDuration(IntroCopyrightAnimationDuration);
  IntroAnimation.AddAnimation(&CopyrightAnimation);
}

IntroSequence::~IntroSequence() {
  Renderer->FreeTexture(FallingStarsMask.Sheet.Texture);
}

void IntroSequence::Update(float dt) { IntroAnimation.Update(dt); }

void IntroSequence::Render() {
  if (FallingStarsAnimation.IsOut()) {
    DrawBackground();
  }

  if (StarBounceAnimation.State == +AnimationState::Playing) {
    if (StarBounceAnimation.Progress >= 0.357f &&
        Audio::Channels[Audio::AC_SE0]->GetState() == ACS_Paused) {
      // Should still skip ahead playback in case of desync
      Audio::Channels[Audio::AC_SE0]->Resume();
    }

    DrawBouncingStar();
  } else if (ExplodingStarAnimation.State == +AnimationState::Playing) {
    DrawExplodingStars();
  } else if (FallingStarsAnimation.State == +AnimationState::Playing) {
    // Make sure the mask is the only thing in the color buffer
    Renderer->Clear(glm::vec4(0.0f));
    Renderer->DrawSprite(FallingStarsMask, glm::vec2(0.0f));

    // Draw the stars over the mask
    // effectively obtaining the union
    DrawFallingStars();

    // Update the mask by rendering to the texture
    Renderer->CaptureScreencap(FallingStarsMask);

    // Draw the original background
    // this will be visible through the holes in the mask
    DrawBackground();

    // Draw the new background with the mask
    Renderer->DrawMaskedSprite(BackgroundSprite, FallingStarsMask,
                               RectF{0.0f, 0.0f, DesignWidth, DesignHeight},
                               255, 256);

    // Draw the stars again, not to the mask this time
    DrawFallingStars();
  } else if (FallingStarsAnimation.IsIn()) {
    if (!LogoFadeAnimation.IsOut()) {
      DrawCHLogo();
    }

    if (!LCCLogoAnimation.IsOut()) {
      DrawLCCLogo();
    }

    if (!DelusionADVAnimation.IsOut()) {
      DrawDelusionADVText();
    }

    if (!SeiraAnimation.IsOut()) {
      DrawSeira();
    }

    if (!CopyrightAnimation.IsOut()) {
      float alpha = CopyrightAnimation.Progress;
      Renderer->DrawSprite(CopyrightTextSprite, CopyrightTextPosition,
                           {1.0f, 1.0f, 1.0f, alpha});
    }
  }
}

void IntroSequence::DrawBackground() const {
  float progress = std::sin(PanningAnimation.Progress * M_PI_2);
  glm::vec2 designDimensions(DesignWidth, DesignHeight);

  Renderer->DrawQuad(RectF{0, 0, DesignWidth, DesignHeight}, glm::vec4(1.0f));
  Renderer->DrawSprite(IntroBackgroundSprite, glm::vec2(0.0f),
                       {1.0f, 1.0f, 1.0f, PanningAnimation.Progress});

  glm::vec2 zoomFactor =
      designDimensions / 16.0f + designDimensions / 16.0f * 7.0f * progress;

  Renderer->SetBlendMode(RendererBlendMode::Additive);

  for (size_t i = 0; i < IntroHighlightCount; i++) {
    constexpr float scale = 1.5f;
    const Sprite& sprite = IntroHighlightSprites[i];
    float offset = IntroHighlightPositions[i];
    glm::vec2 position = (offset + 1.0f) * zoomFactor -
                         (sprite.ScaledBounds().GetSize() / 2.0f) * scale;

    RectF dest = sprite.ScaledBounds()
                     .Scale(glm::vec2(scale), glm::vec2(0.0f))
                     .Translate(position);

    Renderer->DrawSprite(sprite, dest, glm::vec4(1.0f));
  }

  Renderer->SetBlendMode(RendererBlendMode::Normal);

  Renderer->CaptureScreencap(ShaderScreencapture.BgSprite);

  // Cross-fade from black
  Renderer->DrawQuad(RectF{0.0f, 0.0f, DesignWidth, DesignHeight},
                     {0.0f, 0.0f, 0.0f, 1.0f});

  float scale = 4 / (progress * 3 + 1);
  RectF dest = ShaderScreencapture.BgSprite.ScaledBounds().Scale(
      glm::vec2(scale), glm::vec2(0.0f));

  Renderer->DrawSprite(ShaderScreencapture.BgSprite, dest,
                       {1.0f, 1.0f, 1.0f, progress});
}

void IntroSequence::DrawBouncingStar() const {
  // These are the normalized frame timings & positions
  float x = DesignWidth / 2 - IntroBouncingStarSprite.Bounds.Width / 2 +
            (1.0f - StarBounceAnimation.Progress) * 0.61 * DesignWidth;

  float y = DesignHeight / 2 + IntroBouncingStarSprite.Bounds.Height;
  if (StarBounceAnimation.Progress < 0.357f) {
    float progress = StarBounceAnimation.Progress / 0.357f;
    y -= std::sin(progress * M_PI) * 0.664f * DesignHeight;
  } else if (StarBounceAnimation.Progress < 0.536f) {
    float progress =
        (StarBounceAnimation.Progress - 0.357f) / (0.536f - 0.357f);
    y -= std::sin(progress * M_PI) * 0.094f * DesignHeight;
  } else if (StarBounceAnimation.Progress < 0.714f) {
    float progress =
        (StarBounceAnimation.Progress - 0.536f) / (0.714f - 0.536f);
    y -= std::sin(progress * M_PI) * 0.094f * DesignHeight;
  } else {
    float progress = (StarBounceAnimation.Progress - 0.714f) / (1.0f - 0.714f);
    y -= std::sin(progress * 0.8f * M_PI) * 0.475f * DesignHeight;
  }

  Renderer->DrawSprite(StarLogoSprite, glm::vec2(x, y));
}

void IntroSequence::DrawExplodingStars() const {
  glm::vec2 origin = glm::vec2(DesignWidth, DesignHeight) / 2.0f -
                     IntroBouncingStarSprite.Bounds.GetSize() / 2.0f;

  constexpr size_t NUM_STARS = 5;
  for (size_t i = 0; i < NUM_STARS; i++) {
    float rayAngle = M_PI_2 - M_PI * 2 / NUM_STARS * i;
    glm::vec2 directionVector(std::cos(rayAngle), -std::sin(rayAngle));
    glm::vec2 displacement = directionVector * ExplodingStarAnimation.Progress *
                             IntroExplodingStarAnimationDistance;
    glm::vec2 position = origin + displacement;

    float opacity = 1 - ExplodingStarAnimation.Progress;
    float angle = M_PI * 2 * ExplodingStarRotationAnimation.Progress;
    if (i >= 3) angle = -angle;

    CornersQuad dest = IntroExplodingStarSprite.ScaledBounds()
                           .Translate(position)
                           .RotateAroundCenter(angle);

    Renderer->DrawSprite(IntroExplodingStarSprite, dest,
                         {1.0f, 1.0f, 1.0f, opacity});
  }
}

void IntroSequence::DrawFallingStars() const {
  for (auto [origin, initialAngle] : FallingStarSeeds) {
    glm::vec2 displacement = IntroFallingStarsAnimationDirection *
                             IntroFallingStarsAnimationDistance *
                             FallingStarsAnimation.Progress;

    glm::vec2 position = origin + displacement;
    float angle =
        initialAngle + M_PI * 2 * FallingStarsRotationAnimation.Progress;

    CornersQuad dest = IntroFallingStarSprite.ScaledBounds()
                           .RotateAroundCenter(angle)
                           .Translate(position);

    Renderer->DrawSprite(IntroFallingStarSprite, dest);
  }
}

void IntroSequence::DrawCHLogo() const {
  if (LogoFadeAnimation.IsIn()) {
    Renderer->DrawSprite(CHLogoSprite, CHLogoPosition);
    return;
  }

  float dy = IntroCHLogoFadeAnimationStartY - CHLogoPosition.y;

  float y = IntroCHLogoFadeAnimationStartY - dy * LogoFadeAnimation.Progress;
  float opacity = LogoFadeAnimation.Progress;
  Renderer->DrawSprite(CHLogoSprite, {CHLogoPosition.x, y},
                       {1.0f, 1.0f, 1.0f, opacity});

  y = CHLogoPosition.y + dy * LogoFadeAnimation.Progress;
  opacity = std::sin(LogoFadeAnimation.Progress * M_PI) / 2.0f;
  Renderer->DrawSprite(CHLogoSprite, {CHLogoPosition.x, y},
                       {1.0f, 1.0f, 1.0f, opacity});
}

void IntroSequence::DrawLCCLogo() const {
  if (LogoStarHighlightAnimation.IsIn()) {
    Renderer->DrawSprite(LCCLogoUnderSprite, LCCLogoUnderPosition);
  }

  glm::vec2 popOutOffset =
      (1 - LogoPopOutAnimation.Progress) * -IntroLogoPopOutOffset;

  float animationStep;
  float progress =
      std::modf(LCCLogoAnimation.Progress * LCCLogoSpriteCount, &animationStep);
  size_t animatedSprite =
      std::min((size_t)animationStep, LCCLogoSpriteCount - 1);

  for (size_t index : LCCLogoDrawOrder) {
    if (index > animationStep) continue;

    float opacity = index < animationStep ? 1.0f : progress;
    float scale = index < animationStep ? 1.0f : 2 - progress;

    const Sprite& sprite = LCCLogoSprites[index];
    glm::vec2 position = LCCLogoPositions[index] + popOutOffset -
                         sprite.Bounds.GetSize() * (scale - 1) / 2.0f;

    RectF dest = sprite.ScaledBounds();
    dest.Scale(glm::vec2(scale), glm::vec2(0.0f)).Translate(position);
    Renderer->DrawSprite(sprite, dest, {1.0f, 1.0f, 1.0f, opacity});
  }

  if (LogoStarHighlightAnimation.Progress > 0.5f) {
    Renderer->DrawSprite(StarLogoSprite, StarLogoPosition + popOutOffset);
  }

  if (LogoStarHighlightAnimation.State == +AnimationState::Playing) {
    float opacity = 1 - std::abs(1 - 2 * LogoStarHighlightAnimation.Progress);
    Renderer->DrawSprite(IntroLogoStarHighlightSprite,
                         IntroLogoStarHighlightPosition,
                         {1.0f, 1.0f, 1.0f, opacity});
  }
}

void IntroSequence::DrawDelusionADVText() const {
  if (SeiraAnimation.IsOut()) {
    float animationStep;
    float progress =
        std::modf(DelusionADVAnimation.Progress * IntroDelusionADVSpriteCount,
                  &animationStep);

    for (size_t index = 0; index < IntroDelusionADVSpriteCount; index++) {
      if (index > animationStep) continue;

      float opacity = index < animationStep ? 1.0f : progress;
      float scale = index < animationStep ? 1.0f : 2 - progress;

      const Sprite& sprite = IntroDelusionADVSprites[index];
      glm::vec2 position = IntroDelusionADVPositions[index] -
                           sprite.Bounds.GetSize() * (scale - 1) / 2.0f;

      RectF dest = sprite.ScaledBounds();
      dest.Scale(glm::vec2(scale), glm::vec2(0.0f)).Translate(position);
      Renderer->DrawSprite(sprite, dest, {1.0f, 1.0f, 1.0f, opacity});
    }
  } else if (DelusionADVHighlightAnimation.State == +AnimationState::Playing) {
    float intensity =
        1 - std::abs(1 - 2 * DelusionADVHighlightAnimation.Progress);
    glm::vec3 colorShift(intensity);
    glm::vec2 position = DelusionADVPosition - DelusionADVPopoutOffset;
    RectF dest{position.x, position.y, DelusionADVSprite.Bounds.Width,
               DelusionADVSprite.Bounds.Height};

    Renderer->DrawSprite(DelusionADVSprite, dest, glm::vec4(1.0f), colorShift);
  } else if (DelusionADVAnimation.IsIn()) {
    Renderer->DrawSprite(DelusionADVUnderSprite,
                         DelusionADVPosition - DelusionADVPopoutOffset);

    glm::vec2 offset =
        -DelusionADVPopoutOffset * (1 - LogoPopOutAnimation.Progress);
    Renderer->DrawSprite(DelusionADVSprite, DelusionADVPosition + offset);
  }
}

void IntroSequence::DrawSeira() const {
  float progress = SeiraAnimation.Progress;
  glm::vec4 tint = {1.0f, 1.0f, 1.0f, progress};

  glm::vec2 seiraPosition = SeiraPosition - (1 - progress) * SeiraPopoutOffset;
  glm::vec2 seiraUnderPosition =
      SeiraUnderPosition + (1 - progress) * SeiraPopoutOffset;

  Renderer->DrawSprite(SeiraUnderSprite, seiraUnderPosition, tint);
  Renderer->DrawSprite(SeiraSprite, seiraPosition, tint);
}

}  // namespace CHLCC
}  // namespace Impacto