#pragma once

#include "../../ui/ui.h"
#include "../../spritesheet.h"

namespace Impacto {
namespace Profile {
namespace ExtraMenus {

inline Impacto::UI::ClearListMenuType ClearListType =
    Impacto::UI::ClearListMenuType::None;
inline Impacto::UI::AlbumMenuType AlbumType = Impacto::UI::AlbumMenuType::None;
inline Impacto::UI::MusicMenuType MusicType = Impacto::UI::MusicMenuType::None;
inline Impacto::UI::MovieMenuType MovieType = Impacto::UI::MovieMenuType::None;
inline Impacto::UI::ActorsVoiceMenuType ActorsVoiceType =
    Impacto::UI::ActorsVoiceMenuType::None;

void Configure();

}  // namespace ExtraMenus
}  // namespace Profile
}  // namespace Impacto