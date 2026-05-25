#include "achievementnotification.h"

#include "../animation.h"
#include "../io/physicalfilestream.h"
#include "../log.h"
#include "../profile/game.h"
#include "../profile/profile_internal.h"
#include "../renderer/renderer.h"
#include "../texture/texture.h"

#include <queue>

namespace Impacto {
namespace AchievementNotification {

static constexpr float DefaultDisplayDuration = 5.0f;
static constexpr float DefaultFadeDuration = 0.5f;
static constexpr float VisualScale = 0.75f;

static Sprite BackgroundSprite;
static Animation FadeAnimation;
static float DisplayTimer = 0.0f;
static bool IsConfigured = false;
static bool IsShowing = false;
static std::queue<uint32_t> NotificationQueue;

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

static void StartNextNotification() {
  if (NotificationQueue.empty()) return;

  NotificationQueue.pop();
  DisplayTimer = DefaultDisplayDuration;
  IsShowing = true;
  FadeAnimation.StartIn(true);
}

void Init() {
  IsConfigured = false;
  DisplayTimer = 0.0f;
  IsShowing = false;
  NotificationQueue = {};

  FadeAnimation.DurationIn = DefaultFadeDuration;
  FadeAnimation.DurationOut = DefaultFadeDuration;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;

  std::string path;
  if (!Profile::TryGetMember<std::string>("AchievementNotificationPath",
                                          path)) {
    return;
  }

  IsConfigured = LoadBackground(path);
}

void Update(float dt) {
  if (!IsConfigured) return;

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
}

void Render() {
  if (!IsConfigured || FadeAnimation.IsOut()) return;

  glm::vec4 tint(1.0f);
  tint.a = glm::smoothstep(0.0f, 1.0f, FadeAnimation.Progress);

  const float backgroundWidth = BackgroundSprite.ScaledWidth() * VisualScale;
  const float backgroundHeight = BackgroundSprite.ScaledHeight() * VisualScale;
  const glm::vec2 pos = {Profile::DesignWidth - backgroundWidth,
                         Profile::DesignHeight - backgroundHeight};

  Renderer->DrawSprite(BackgroundSprite,
                       RectF(pos.x, pos.y, backgroundWidth, backgroundHeight),
                       tint);
}

void Show(int achievementId) {
  if (!IsConfigured) return;

  NotificationQueue.push(achievementId);
  if (!IsShowing && FadeAnimation.IsOut()) {
    StartNextNotification();
  }
}

}  // namespace AchievementNotification
}  // namespace Impacto
