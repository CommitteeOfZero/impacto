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
    queued.name = def.Name;
    queued.description = def.Description;
    queued.hidden = def.Hidden;
    queued.rarity = def.Rarity;
    queued.hasTexture = false;

    Io::Stream* iconStream = nullptr;
    IoError err = Io::PhysicalFileStream::Create(def.IconPath, &iconStream);
    if (err != IoError_OK) {
      ImpLog(LogLevel::Warning, LogChannel::IO,
             "Couldn't open achievement icon {:s}\n", def.IconPath);
    } else {
      if (queued.texture.Load(iconStream)) {
        queued.hasTexture = true;
      } else {
        ImpLog(LogLevel::Warning, LogChannel::TextureLoad,
               "Couldn't load achievement icon {:s}\n", def.IconPath);
      }
      delete iconStream;
    }

    queuedAchievements.push_back(std::move(queued));
  }

  mainThreadCallback = [this, queuedAchievements =
                                  std::move(queuedAchievements)]() mutable {
    Achievements.clear();
    Achievements.reserve(queuedAchievements.size());
    for (QueuedAchievement& queued : queuedAchievements) {
      Sprite icon;
      if (queued.hasTexture) {
        SpriteSheet sheet(static_cast<float>(queued.texture.Width),
                          static_cast<float>(queued.texture.Height));
        sheet.Texture = queued.texture.Submit();
        icon = Sprite(sheet, 0.0f, 0.0f, sheet.DesignWidth, sheet.DesignHeight);
      }
      Achievements.push_back(std::make_unique<CommonAchievement>(
          std::move(queued.name), std::move(queued.description), queued.hidden,
          queued.rarity, icon));
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
