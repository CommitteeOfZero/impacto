#include "trophymenu.h"

#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../profile/scriptvars.h"
#include "../../profile/ui/trophymenu.h"
#include "../../profile/games/chlcc/trophymenu.h"
#include "../../profile/games/chlcc/commonmenu.h"

#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../data/achievementsystem.h"
#include "../../data/achievementsystemcommon.h"
#include "../../profile/data/achievementsystem.h"
#include "../../ui/widgets/chlcc/trophymenuentry.h"

#include <array>
#include <fmt/format.h>
#include <magic_enum/magic_enum_containers.hpp>

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::CHLCC::CommonMenu;

using namespace Impacto::Profile::TrophyMenu;
using namespace Impacto::Profile::CHLCC::TrophyMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::GameSpecific;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;
using namespace Impacto::UI::Widgets::CHLCC;

TrophyMenu::TrophyMenu() : CommonMenu(true) {
  TrophyCountHintLabel.Enabled = false;
  TrophyCountHintLabel.MoveTo(TrophyCountHintLabelPos);

  BronzeCountLabel.MoveTo(BronzeTrophyPos + RarityCountOffset);
  SilverCountLabel.MoveTo(SilverTrophyPos + RarityCountOffset);
  GoldCountLabel.MoveTo(GoldTrophyPos + RarityCountOffset);
  PlatinumCountLabel.MoveTo(PlatinumTrophyPos + RarityCountOffset);
}

void TrophyMenu::UpdateRarityCounts() {
  using namespace Impacto::AchievementSystem;

  magic_enum::containers::array<AchievementRarity, size_t> unlockedCounts{};
  magic_enum::containers::array<AchievementRarity, size_t> totalCounts{};

  switch (Profile::AchievementSystem::Type) {
    case AchievementDataType::Common: {
      auto* commonImpl = static_cast<AchievementSystemCommon*>(Implementation);
      const size_t count = GetAchievementCount();
      for (size_t id = 0; id < count; id++) {
        const CommonAchievement* ach =
            commonImpl->GetAchievement(static_cast<int>(id));
        if (ach == nullptr) continue;
        const AchievementRarity rarity = ach->GetRarity();
        totalCounts[rarity]++;
        if (IsAchievementUnlocked(static_cast<int>(id)))
          unlockedCounts[rarity]++;
      }
      break;
    }
    case AchievementDataType::None:
      break;
  }

  const auto setCount = [&unlockedCounts, &totalCounts](
                            Label& label, AchievementRarity rarity) {
    label.SetText(
        fmt::format("{:d}/{:d}", unlockedCounts[rarity], totalCounts[rarity]),
        label.Bounds.GetPos(), RarityCountFontSize, RendererOutlineMode::Full,
        0);
  };

  setCount(BronzeCountLabel, AchievementRarity::Bronze);
  setCount(SilverCountLabel, AchievementRarity::Silver);
  setCount(GoldCountLabel, AchievementRarity::Gold);
  setCount(PlatinumCountLabel, AchievementRarity::Platinum);
}

void TrophyMenu::Show() {
  if (State != Shown) {
    if (State != Showing) {
      MenuTransition.StartIn();
      FromSystemMenuTransition->StartIn();
    }
    State = Showing;
    if (FocusedMenu != 0) {
      LastFocusedMenu = FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    FocusedMenu = this;

    for (size_t i = 0; i < MaxTrophyPages; i++) {
      for (size_t j = 0; j < EntriesPerPage; j++) {
        const size_t index = i * EntriesPerPage + j;
        if (index >= AchievementSystem::GetAchievementCount()) break;
        TrophyMenuEntry* entry = new TrophyMenuEntry(static_cast<int>(index));
        MainItems[i].Add(entry);
      }
    }
    MainItems[CurrentPage].Show();
    UpdateRarityCounts();
    if (!TrophyCountHintLabel.Enabled) {
      TrophyCountHintLabel.Enabled = true;
      TrophyCountHintLabel.SetText(
          Vm::ScriptGetTextTableStrAddress(TrophyCountHintTextTableId,
                                           TrophyCountHintStringNum),
          TrophyCountHintLabelPos, TrophyCountFontSize,
          RendererOutlineMode::Full, 0);
    }
  }
}
void TrophyMenu::Hide() {
  if (State != Hidden) {
    if (State != Hiding) {
      MenuTransition.StartOut();
      FromSystemMenuTransition->StartOut();
    }
    State = Hiding;
    if (LastFocusedMenu != 0) {
      FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void TrophyMenu::Render() {
  if (State == Hidden) return;
  DrawSubmenu(BackgroundColor, CircleSprite, MenuTitleText, MenuTitleTextAngle,
              true);

  if (MenuTransition.Progress < 0.22f) return;

  glm::vec2 offset = MenuTransition.GetPageOffset();

  DrawButtonPrompt(ButtonPromptSprite, ButtonPromptPosition);

  TrophyCountHintLabel.Move(offset);
  TrophyCountHintLabel.Render();
  TrophyCountHintLabel.Move(-offset);

  Renderer->DrawSprite(PlatinumTrophySprite, offset + PlatinumTrophyPos);
  Renderer->DrawSprite(GoldTrophySprite, offset + GoldTrophyPos);
  Renderer->DrawSprite(SilverTrophySprite, offset + SilverTrophyPos);
  Renderer->DrawSprite(BronzeTrophySprite, offset + BronzeTrophyPos);

  for (Label* countLabel : {&BronzeCountLabel, &SilverCountLabel,
                            &GoldCountLabel, &PlatinumCountLabel}) {
    countLabel->Move(offset);
    countLabel->Render();
    countLabel->Move(-offset);
  }

  Renderer->DrawSprite(TrophyPageCtBoxSprite, offset + TrophyPageCtPos);
  Renderer->DrawSprite(PageNums[CurrentPage + 1], offset + CurrentPageNumPos);
  Renderer->DrawSprite(PageNumSeparatorSlash, offset + PageNumSeparatorPos);
  Renderer->DrawSprite(ReachablePageNums[MaxTrophyPages],
                       offset + MaxPageNumPos);

  MainItems[CurrentPage].Move(offset);
  MainItems[CurrentPage].Render();
  MainItems[CurrentPage].Move(-offset);

  Renderer->DrawSprite(TrophyEntriesBorderSprite, offset);
}

void TrophyMenu::UpdateInput(float dt) {
  if (IsFocused) {
    if (PADinputButtonWentDown & PAD1DOWN || Input::MouseWheelDeltaY < 0 ||
        PADinputButtonWentDown & PADcustom[8]) {
      if (CurrentPage < 8) {
        MainItems[CurrentPage++].Hide();
        MainItems[CurrentPage].Show();
      }
    } else if (PADinputButtonWentDown & PAD1UP || Input::MouseWheelDeltaY > 0 ||
               PADinputButtonWentDown & PADcustom[7]) {
      if (CurrentPage > 0) {
        MainItems[CurrentPage--].Hide();
        MainItems[CurrentPage].Show();
      }
    }
  }
}

void TrophyMenu::Update(float dt) {
  UpdateInput(dt);

  const int sysMenuCt = ScrWork[SW_SYSMENUCT];
  const int systemMenuCHG = ScrWork[SW_SYSTEMMENUCHG];

  if ((!GetFlag(SF_ACHIEVEMENTMENU) || sysMenuCt < 10000 ||
       (sysMenuCt == 10000 && systemMenuCHG != 0 && systemMenuCHG != 64)) &&
      State == Shown) {
    Hide();
  } else if (GetFlag(SF_ACHIEVEMENTMENU) && sysMenuCt > 0 && State == Hidden) {
    Show();
  }

  if (MenuTransition.IsOut() && !GetFlag(SF_ACHIEVEMENTMENU) &&
      systemMenuCHG == 0 && (sysMenuCt == 0 || GetFlag(SF_SYSTEMMENU)) &&
      State == Hiding) {
    State = Hidden;
    for (int i = 0; i < 9; i++) {
      MainItems[i].Clear();
    }
  } else if (MenuTransition.IsIn() && sysMenuCt == 10000 &&
             (systemMenuCHG == 0 || systemMenuCHG == 64) &&
             GetFlag(SF_ACHIEVEMENTMENU) && State == Showing) {
    State = Shown;
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    FromSystemMenuTransition->Update(dt);
    if (MenuTransition.Direction == AnimationDirection::Out &&
        MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == AnimationDirection::In ||
                TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    UpdateTitles(MenuTitleTextRightPosition, MenuTitleTextLeftPosition);
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto