#include "savemenu.h"

#include "../../profile/ui/savemenu.h"
#include "../../profile/games/chlcc/savemenu.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../inputsystem.h"
#include "../../ui/widgets/button.h"
#include "../../ui/widgets/chlcc/saveentrybutton.h"
#include "../../data/savesystem.h"
#include "../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::Profile::SaveMenu;
using namespace Impacto::Profile::CHLCC::SaveMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets::CHLCC;

Widget* EntryGrid[EntriesPerPage];

void SaveMenu::MenuButtonOnClick(Widgets::Button* target) {
  if ((SaveSystem::GetSaveSatus(SaveSystem::SaveType::SaveFull, target->Id) !=
       0) ||
      ScrWork[SW_SAVEMENUMODE] == 1) {
    ScrWork[SW_SAVEFILENO] = target->Id;
    ChoiceMade = true;
  }
}

SaveMenu::SaveMenu() {
  MenuTransition.Direction = 1;
  MenuTransition.LoopMode = ALM_Stop;
  MenuTransition.DurationIn = FadeInDuration;
  MenuTransition.DurationOut = FadeOutDuration;

  TitleFade.Direction = 1.0f;
  TitleFade.LoopMode = ALM_Stop;
  TitleFade.DurationIn = TitleFadeInDuration;
  TitleFade.DurationOut = TitleFadeOutDuration;

  RedBarSprite = InitialRedBarSprite;
  RedBarPosition = InitialRedBarPosition;
}

void SaveMenu::Show() {
  if (State != Shown) {
    MainItems = new Widgets::Group(this);
    MainItems->WrapFocus = false;

    auto onClick =
        std::bind(&SaveMenu::MenuButtonOnClick, this, std::placeholders::_1);

    Sprite entrySprite;
    Sprite nullSprite = Sprite();
    nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

    switch (ScrWork[SW_SAVEMENUMODE]) {
      case 0:
        EntryType = SaveSystem::SaveQuick;
        CircleSprite = QuickLoadCircle;
        MenuTitleTextSprite = QuickLoadTextSprite;
        entrySprite = QuickLoadEntrySprite;
        break;
      case 1:
        EntryType = SaveSystem::SaveFull;
        CircleSprite = SaveCircle;
        MenuTitleTextSprite = SaveTextSprite;
        entrySprite = SaveEntrySprite;
        break;
      case 2:
        EntryType = SaveSystem::SaveFull;
        CircleSprite = LoadCircle;
        MenuTitleTextSprite = LoadTextSprite;
        entrySprite = LoadEntrySprite;
        break;
    }
    int id = 0;
    for (int i = 0; i < EntriesPerPage; i++) {
      SaveEntryButton* saveEntryButton =
          new SaveEntryButton(id, entrySprite, EntryHighlightedSprite,
                              entrySprite, EntryPositions[i]);

      saveEntryButton->OnClickHandler = onClick;
      saveEntryButton->AddEntryNumberHintText(
          Vm::ScriptGetTextTableStrAddress(0, 6), 18, true);
      if (SaveSystem::GetSaveSatus(SaveSystem::SaveType::SaveFull, id) != 0) {
        saveEntryButton->EntryActive = true;
        saveEntryButton->AddSceneTitleText(
            Vm::ScriptGetTextTableStrAddress(
                1,
                SaveSystem::GetSaveTitle(SaveSystem::SaveType::SaveFull, id)),
            24, true);
        saveEntryButton->AddPlayTimeHintText(
            Vm::ScriptGetTextTableStrAddress(0, 2), 18, true);
        uint32_t time = SaveSystem::GetSavePlayTime(EntryType, i);
        uint32_t hours = time / 3600;
        uint32_t minutes = (time % 3600) / 60;
        uint32_t seconds = (time % 3600) % 60;
        char temp[10];
        sprintf(temp, "%3d:%02d:%02d", hours, minutes, seconds);
        saveEntryButton->AddPlayTimeText(std::string(temp), 18, true);
        saveEntryButton->AddSaveDateHintText(
            Vm::ScriptGetTextTableStrAddress(0, 3), 18, true);
        tm date = SaveSystem::GetSaveDate(EntryType, i);
        char tempdate[20];
        sprintf(tempdate, "%4d/%2d/%2d %2d:%2d:%2d", date.tm_year, date.tm_mon,
                date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
        saveEntryButton->AddSaveDateText(std::string(tempdate), 18, true);
      } else {
        saveEntryButton->AddSceneTitleText(
            Vm::ScriptGetTextTableStrAddress(0, 1), 24, true);
      }
      saveEntryButton->AddThumbnail(EmptyThumbnailSprite, ThumbnailRelativePos);
      id++;
      MainItems->Add(saveEntryButton);
      EntryGrid[i] = saveEntryButton;
    }

    // Yes I know this is bad...
    for (int i = 0; i < EntriesPerPage - 2; i++) {
      if (i % 3 == 0) {
        EntryGrid[i]->SetFocus(EntryGrid[(i % 2) + 4], FDIR_RIGHT);
        EntryGrid[(i % 2) + 4]->SetFocus(EntryGrid[i], FDIR_LEFT);
      }
      EntryGrid[i]->SetFocus(EntryGrid[(i + 1) % 4], FDIR_DOWN);
      EntryGrid[(i + 1) % 4]->SetFocus(EntryGrid[i], FDIR_UP);
    }
    EntryGrid[4]->SetFocus(EntryGrid[5], FDIR_DOWN);
    EntryGrid[4]->SetFocus(EntryGrid[5], FDIR_UP);
    EntryGrid[5]->SetFocus(EntryGrid[4], FDIR_UP);
    EntryGrid[5]->SetFocus(EntryGrid[4], FDIR_DOWN);

    State = Showing;
    MenuTransition.StartIn();
    MainItems->Show();
    CurrentlyFocusedElement = EntryGrid[0];
    EntryGrid[0]->HasFocus = true;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void SaveMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    SaveEntryButton::FocusedAlphaFadeReset();
    MenuTransition.StartOut();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void SaveMenu::Update(float dt) {
  UpdateInput();
  if (ScrWork[SW_SYSMENUCT] < 10000 && State == Shown) {
    Hide();
  } else if (GetFlag(SF_SAVEMENU) && ScrWork[SW_SYSMENUCT] > 0 &&
             State == Hidden) {
    Show();
  }

  if (ScrWork[SW_SYSMENUCT] == 0 && State == Hiding) {
    State = Hidden;
    MainItems->Hide();
  } else if (ScrWork[SW_SYSMENUCT] == 10000 && State == Showing) {
    State = Shown;
    SaveEntryButton::FocusedAlphaFadeStart();
  }

  if (State != Hidden) {
    MenuTransition.Update(dt);
    if (MenuTransition.Direction == -1.0f && MenuTransition.Progress <= 0.72f) {
      TitleFade.StartOut();
    } else if (MenuTransition.IsIn() &&
               (TitleFade.Direction == 1.0f || TitleFade.IsOut())) {
      TitleFade.StartIn();
    }
    TitleFade.Update(dt);
    MainItems->Update(dt);
    SaveEntryButton::UpdateFocusedAlphaFade(dt);
  }
}

void SaveMenu::Render() {
  if (State != Hidden) {
    glm::vec3 tint(1.0f);
    glm::vec4 col(1.0f, 1.0f, 1.0f, 1.0f);
    DrawCircles();
    DrawErin();
    float alpha =
        MenuTransition.Progress < 0.5f ? MenuTransition.Progress * 2.0f : 1.0f;
    Renderer2D::DrawSprite(BackgroundFilter, RectF(0.0f, 0.0f, 1280.0f, 720.0f),
                           glm::vec4(tint, alpha));
    DrawRedBar();
    DrawTitles();
    int yOffset = 0;
    if (MenuTransition.Progress > 0.22f) {
      if (MenuTransition.Progress < 0.72f) {
        // Approximated function from the original, another mess
        yOffset = glm::mix(
            -720.0f, 0.0f,
            1.00397f * std::sin(3.97161f - 3.26438f * MenuTransition.Progress) -
                0.00295643f);
      }
      MainItems->MoveTo(glm::vec2(0, yOffset));
      MainItems->Tint = col;
      MainItems->Render();
      Renderer2D::DrawSprite(SaveListSprite,
                             SaveListPosition + glm::vec2(0, yOffset));
    }
  }
}

inline void SaveMenu::DrawCircles() {
  float y = CircleStartPosition.y;
  int resetCounter = 0;
  // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
  // duration is totalframes/60
  float progress = MenuTransition.Progress * FadeInDuration * 60.0f;
  for (int line = 0; line < 4; line++) {
    int counter = resetCounter;
    float x = CircleStartPosition.x;
    for (int col = 0; col < 7; col++) {
      if (counter + 1 <= (progress)) {
        float scale = ((progress) - (counter + 1.0f)) * 16.0f;
        scale = scale <= 320.0f ? scale : 320.0f;
        Renderer2D::DrawSprite(
            CircleSprite, RectF(x + (CircleSprite.Bounds.Width - scale) / 2.0f,
                                y + (CircleSprite.Bounds.Height - scale) / 2.0f,
                                scale, scale));
        x += CircleOffset;
      }
      counter += 2;
    }
    y += CircleOffset;
    resetCounter += 2;
  }
};

inline void SaveMenu::DrawErin() {
  float y = ErinPosition.y;
  if (MenuTransition.Progress < 0.78f) {
    y = 801.0f;
    if (MenuTransition.Progress > 0.22f) {
      // Approximation from the original function, which was a bigger mess
      y = glm::mix(
          -19.0f, 721.0f,
          0.998938f -
              0.998267f * sin(3.97835f - 3.27549f * MenuTransition.Progress));
    }
  }
  Renderer2D::DrawSprite(ErinSprite, glm::vec2(ErinPosition.x, y));
}

inline void SaveMenu::DrawRedBar() {
  if (MenuTransition.IsIn()) {
    Renderer2D::DrawSprite(InitialRedBarSprite, InitialRedBarPosition);
  } else if (MenuTransition.Progress > 0.70f) {
    // Give the whole range that mimics ScrWork[SW_SYSMENUCT] given that the
    // duration is totalframes/60
    float progress = MenuTransition.Progress * FadeInDuration * 60.0f;
    float pixelPerAdvanceLeft = RedBarBaseX * (progress - 47.0f) / 17.0f;
    RedBarSprite.Bounds.X = RedBarDivision - pixelPerAdvanceLeft;
    RedBarSprite.Bounds.Width = pixelPerAdvanceLeft;
    RedBarPosition.x = RedBarBaseX - pixelPerAdvanceLeft;
    Renderer2D::DrawSprite(RedBarSprite, RedBarPosition);
    float pixelPerAdvanceRight = 13.0f * (progress - 47.0f);
    RedBarSprite.Bounds.X = RedBarDivision;
    RedBarSprite.Bounds.Width = pixelPerAdvanceRight;
    RedBarPosition = RightRedBarPosition;
    Renderer2D::DrawSprite(RedBarSprite, RedBarPosition);
  }
}

inline void SaveMenu::DrawTitles() {
  if (MenuTransition.Progress > 0.34f) {
    float labelX = RedBarLabelPosition.x;
    float labelY = RedBarLabelPosition.y;
    float rightTitleX = MenuTitleTextRightPos.x;
    float rightTitleY = MenuTitleTextRightPos.y -
                        (MenuTitleTextSprite.Bounds.Height + 2.0f) / 2.0f;
    float leftTitleY = glm::mix(
        1.0f, 721.0f,
        1.01011f * sin(1.62223f * TitleFade.Progress + 3.152f) + 1.01012f);
    if (MenuTransition.Progress < 0.72f) {
      labelX -= 572.0f * (MenuTransition.Progress * 4.0f - 3.0f);
      rightTitleX -= 572.0f * (MenuTransition.Progress * 4.0f - 3.0f);
      labelY += 460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f;
      rightTitleY += 460.0f * (MenuTransition.Progress * 4.0f - 3.0f) / 3.0f;
    }
    Renderer2D::DrawSprite(RedBarLabel, glm::vec2(labelX, labelY));
    Renderer2D::DrawSprite(MenuTitleTextSprite,
                           glm::vec2(rightTitleX, rightTitleY), glm::vec4(1.0f),
                           glm::vec2(1.0f), MenuTitleTextAngle);
    Renderer2D::DrawSprite(MenuTitleTextSprite,
                           glm::vec2(MenuTitleTextLeftPos.x, leftTitleY));
  }
}

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto