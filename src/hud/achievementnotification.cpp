#include "achievementnotification.h"

#include "../animation.h"
#include "../data/achievementsystem.h"
#include "../text/fonts/font.h"
#include "../inputsystem.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include "../overlay.h"
#include "../profile/game.h"
#include "../profile/hud/achievementnotification.h"
#include "../renderer/renderer.h"
#include "../texture/texture.h"

#include <cassert>
#include <optional>
#include <queue>
#include <string>
#include <vector>

namespace Impacto {
namespace AchievementNotification {

static Sprite BackgroundSprite;
static std::optional<ExternalFont> NotificationFont;
static std::vector<ProcessedTextGlyph> TextGlyphs;
static Animation FadeAnimation;
static float DisplayTimer = 0.0f;
static std::optional<int> CurrentAchievementId;
static bool IsConfigured = false;
static bool TextConfigured = false;
static std::queue<int> NotificationQueue;

using namespace Profile::AchievementNotification;

static RectF GetNotificationRect() {
  const float width = BackgroundSprite.ScaledWidth() * Scale;
  const float height = BackgroundSprite.ScaledHeight() * Scale;
  return RectF(Profile::Game::DesignWidth - width,
               Profile::Game::DesignHeight - height, width, height);
}

static bool LoadBackground(std::string const& path) {
  Io::Stream* stream = nullptr;
  IoError err = Io::PhysicalFileStream::Create(path, &stream);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Could not open achievement notification background {:s}\n", path);
    return false;
  }

  Texture texture;
  if (!texture.Load(stream)) {
    ImpLog(LogLevel::Error, LogChannel::TextureLoad,
           "Could not load achievement notification background {:s}\n", path);
    delete stream;
    return false;
  }
  delete stream;

  SpriteSheet sheet(static_cast<float>(texture.Width),
                    static_cast<float>(texture.Height));
  sheet.Texture = texture.Submit();
  BackgroundSprite =
      Sprite(sheet, 0.0f, 0.0f, sheet.DesignWidth, sheet.DesignHeight);
  return true;
}

static void BuildTextLine(std::string const& text, float fontSize, float left,
                          float baselineY, float availableWidth,
                          uint32_t color) {
  float width = 0.0f;
  std::vector<ExternalFontShapedGlyph> glyphs =
      NotificationFont->ShapeLine(text, fontSize, width);
  if (glyphs.empty()) return;

  float finalFontSize = fontSize;
  if (width > availableWidth && width > 0.0f) {
    finalFontSize *= availableWidth / width;
    glyphs = NotificationFont->ShapeLine(text, finalFontSize, width);
  }

  const DialogueColorPair colors{color, OutlineColor};

  glm::vec2 pen{left, baselineY};
  for (ExternalFontShapedGlyph const& glyph : glyphs) {
    const glm::vec2 glyphPos = pen + glyph.Offset;

    TextGlyphs.push_back({
        .Colors = colors,
        .CharId = glyph.GlyphIndex,
        .Opacity = 1.0f,
        .DestRect = {glyphPos.x, glyphPos.y, glyph.Advance.x, finalFontSize},
        .Position = pen,
    });

    pen += glyph.Advance;
  }
}

static void BuildTextGlyphs(AchievementSystem::Achievement const* achievement,
                            int achievementId) {
  TextGlyphs.clear();
  assert(TextConfigured);

  std::string title = achievement != nullptr ? achievement->Name() : "";
  std::string description =
      achievement != nullptr ? achievement->Description() : "";
  if (title.empty() && description.empty()) {
    title = "Achievement " + std::to_string(achievementId);
    description = "Achievement System not implemented.";
  }

  const RectF rect = GetNotificationRect();
  const float iconRight = (IconOffset.x + IconSize) * Scale;
  const float textLeft = iconRight + TextGap * Scale;
  const float textRight = rect.Width - TextRightPadding * Scale;
  const float centerY = rect.Height / 2.0f;

  const float titleFontSize = TitleFontSize * Scale;
  const float descriptionFontSize = DescriptionFontSize * Scale;
  const float textLineGap = TextLineGap * Scale;
  const float titleBaseline =
      centerY - (descriptionFontSize + textLineGap) / 2.0f;
  const float descriptionBaseline =
      centerY + (titleFontSize + textLineGap) / 2.0f;
  const float availableWidth = textRight - textLeft;

  BuildTextLine(title, titleFontSize, textLeft, titleBaseline, availableWidth,
                TextColor);
  BuildTextLine(description, descriptionFontSize, textLeft, descriptionBaseline,
                availableWidth, TextColor);
}

static void StartNextNotification() {
  if (NotificationQueue.empty()) return;

  CurrentAchievementId = NotificationQueue.front();
  NotificationQueue.pop();

  const AchievementSystem::Achievement* achievement =
      AchievementSystem::GetAchievement(*CurrentAchievementId);
  BuildTextGlyphs(achievement, *CurrentAchievementId);

  DisplayTimer = DisplayDuration;
  IsShowing = true;
  FadeAnimation.StartIn(true);
}

void Init() {
  TextGlyphs.clear();
  NotificationFont.reset();
  TextConfigured = false;

  Configure();

  FadeAnimation.DurationIn = FadeDuration;
  FadeAnimation.DurationOut = FadeDuration;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;

  if (!LoadBackground(BackgroundPath)) return;

  if (!FontPath.empty()) {
    NotificationFont.emplace(FontPath, "achievement notification font");
    TextConfigured = NotificationFont->IsLoaded();
  } else {
    ImpLog(LogLevel::Warning, LogChannel::Profile,
           "Achievement notification font path is not configured\n");
  }

  IsConfigured = true;
}

void Update(float dt) {
  assert(IsConfigured);

  FadeAnimation.Update(dt);

  if (IsShowing && FadeAnimation.IsIn()) {
    DisplayTimer -= dt;
    if (DisplayTimer <= 0.0f) {
      IsShowing = false;
      FadeAnimation.StartOut();
    }
  }

  if (!IsShowing && FadeAnimation.IsOut()) {
    StartNextNotification();
  }

  if (!Overlay::OverlayShown && !FadeAnimation.IsOut() &&
      GetNotificationRect().ContainsPoint(Input::CurMousePos)) {
    RequestCursor(CursorType::Pointer);
    if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
      Overlay::OverlayShown = true;
      Overlay::RequestedTab = Overlay::OverlayTab::Achievements;
    }
  }
}

void Render() {
  assert(IsConfigured);
  if (FadeAnimation.IsOut()) return;

  glm::vec4 tint(1.0f);
  tint.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

  const RectF rect = GetNotificationRect();
  const glm::vec2 pos = {rect.X, rect.Y};

  Renderer->DrawSprite(BackgroundSprite, rect, tint);

  const AchievementSystem::Achievement* achievement =
      CurrentAchievementId.has_value()
          ? AchievementSystem::GetAchievement(*CurrentAchievementId)
          : nullptr;
  if (achievement != nullptr && achievement->Icon().ScaledWidth() > 0.0f &&
      achievement->Icon().ScaledHeight() > 0.0f) {
    const Sprite& icon = achievement->Icon();
    const RectF iconDest(pos.x + IconOffset.x * Scale,
                         pos.y + IconOffset.y * Scale, IconSize * Scale,
                         IconSize * Scale);
    Renderer->DrawSprite(icon, iconDest, tint);
  }

  if (!TextGlyphs.empty()) {
    NotificationFont->DrawProcessedText(TextGlyphs, tint.a, tint.a,
                                        RendererOutlineMode::BottomRight, pos);
  }
}

void Show(int achievementId) {
  assert(IsConfigured);

  NotificationQueue.push(achievementId);
  if (!IsShowing && FadeAnimation.IsOut()) {
    StartNextNotification();
  }
}

}  // namespace AchievementNotification
}  // namespace Impacto
