#include "achievementsystemcommon.h"

#include "../io/physicalfilestream.h"
#include "../profile/data/achievementsystem.h"
#include "../log.h"

using namespace Impacto::Profile::AchievementSystem;

namespace Impacto {
namespace AchievementSystem {

size_t AchievementSystemCommon::GetAchievementCount() const {
  return Profile::AchievementSystem::Achievements.size();
}

AchievementError AchievementSystemCommon::MountAchievementFile(
    std::function<void()>& mainThreadCallback) {
  std::vector<QueuedAchievement> queuedAchievements;
  queuedAchievements.reserve(Profile::AchievementSystem::Achievements.size());

  for (auto const& def : Profile::AchievementSystem::Achievements) {
    QueuedAchievement queued;
    queued.Name = def.Name;
    queued.Description = def.Description;
    queued.Hidden = def.Hidden;
    queued.Rarity = def.Rarity;

    Io::Stream* iconStream = nullptr;
    IoError err = Io::PhysicalFileStream::Create(def.IconPath, &iconStream);
    if (err != IoError_OK) {
      ImpLog(LogLevel::Warning, LogChannel::IO,
             "Couldn't open achievement icon {:s}\n", def.IconPath);
    } else {
      Texture texture;
      if (texture.Load(iconStream)) {
        queued.IconTexture = std::move(texture);
      } else {
        ImpLog(LogLevel::Warning, LogChannel::TextureLoad,
               "Couldn't load achievement icon {:s}\n", def.IconPath);
      }
      delete iconStream;
    }

    queuedAchievements.push_back(std::move(queued));
  }

  std::optional<Texture> lockedTexture;
  std::string const& lockedIconPath =
      Profile::AchievementSystem::LockedIconPath;
  if (!lockedIconPath.empty()) {
    Io::Stream* lockedIconStream = nullptr;
    IoError err =
        Io::PhysicalFileStream::Create(lockedIconPath, &lockedIconStream);
    if (err != IoError_OK) {
      ImpLog(LogLevel::Warning, LogChannel::IO,
             "Couldn't open locked achievement icon {:s}\n", lockedIconPath);
    } else {
      Texture texture;
      if (texture.Load(lockedIconStream)) {
        lockedTexture = std::move(texture);
      } else {
        ImpLog(LogLevel::Warning, LogChannel::TextureLoad,
               "Couldn't load locked achievement icon {:s}\n", lockedIconPath);
      }
      delete lockedIconStream;
    }
  }

  mainThreadCallback = [this,
                        queuedAchievements = std::move(queuedAchievements),
                        lockedTexture = std::move(lockedTexture)]() mutable {
    Sprite lockedIcon;
    if (lockedTexture) {
      SpriteSheet sheet(static_cast<float>(lockedTexture->Width),
                        static_cast<float>(lockedTexture->Height));
      sheet.Texture = lockedTexture->Submit();
      lockedIcon =
          Sprite(sheet, 0.0f, 0.0f, sheet.DesignWidth, sheet.DesignHeight);
    }

    Achievements.clear();
    Achievements.reserve(queuedAchievements.size());
    for (QueuedAchievement& queued : queuedAchievements) {
      Sprite icon;
      if (queued.IconTexture) {
        SpriteSheet sheet(static_cast<float>(queued.IconTexture->Width),
                          static_cast<float>(queued.IconTexture->Height));
        sheet.Texture = queued.IconTexture->Submit();
        icon = Sprite(sheet, 0.0f, 0.0f, sheet.DesignWidth, sheet.DesignHeight);
      }
      Achievements.push_back(std::make_unique<CommonAchievement>(
          std::move(queued.Name), std::move(queued.Description), queued.Hidden,
          queued.Rarity, icon, lockedIcon));
    }
  };

  return AchievementError::OK;
}

const CommonAchievement* AchievementSystemCommon::GetAchievement(int id) {
  if (id < 0 || id >= std::ssize(Achievements)) return nullptr;
  return Achievements[id].get();
}

}  // namespace AchievementSystem
}  // namespace Impacto
