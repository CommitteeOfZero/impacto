#include "overlay.h"

#include "profile_internal.h"
#include "../io/physicalfilestream.h"
#include "../log.h"

namespace Impacto::Profile::Overlay {

static void LoadBackground() {
  std::string path = EnsureGetMember<std::string>("BackgroundPath");

  Io::Stream* stream = nullptr;
  IoError err = Io::PhysicalFileStream::Create(path, &stream);
  if (err != IoError_OK) {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Could not open overlay toast notification background {:s}\n", path);
    Game::Shutdown();
  }

  Texture texture;
  if (!texture.Load(stream)) {
    ImpLog(LogLevel::Error, LogChannel::TextureLoad,
           "Could not load overlay toast notification background {:s}\n", path);
    delete stream;
    Game::Shutdown();
  }
  delete stream;

  SpriteSheet sheet(static_cast<float>(texture.Width),
                    static_cast<float>(texture.Height));
  sheet.Texture = texture.Submit();
  ToastNotification::NotificationBackgroundSprite =
      Sprite(sheet, 0.0f, 0.0f, sheet.DesignWidth, sheet.DesignHeight);
}

static void ConfigureAchievementNotification() {
  using namespace ToastNotification::AchievementNotification;
  EnsurePushMemberOfType("AchievementNotification", LUA_TTABLE);

  IconSize = EnsureGetMember<float>("IconSize");
  IconOffset = EnsureGetMember<glm::vec2>("IconOffset");
  TextGap = EnsureGetMember<float>("TextGap");

  TitleFontSize = EnsureGetMember<float>("TitleFontSize");
  DescriptionFontSize = EnsureGetMember<float>("DescriptionFontSize");

  Pop();
}

static void ConfigureToastNotification() {
  using namespace ToastNotification;
  EnsurePushMemberOfType("ToastNotification", LUA_TTABLE);

  LoadBackground();
  NotificationFont.emplace(EnsureGetMember<std::string>("FontPath"),
                           "overlay toast notification font");

  Scale = EnsureGetMember<float>("Scale");

  DisplayDuration = EnsureGetMember<float>("DisplayDuration");
  FadeDuration = EnsureGetMember<float>("FadeDuration");

  TextPadding = EnsureGetMember<float>("TextPadding");
  TextSize = EnsureGetMember<float>("TextSize");
  TextLineGap = EnsureGetMember<float>("TextLineGap");

  TextColors = EnsureGetMember<DialogueColorPair>("TextColors");

  ConfigureAchievementNotification();

  Pop();
}

void Configure() {
  EnsurePushMemberOfType("Overlay", LUA_TTABLE);

  ConfigureToastNotification();

  Pop();
}

}  // namespace Impacto::Profile::Overlay
