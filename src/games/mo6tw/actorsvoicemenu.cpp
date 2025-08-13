#include "actorsvoicemenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/mo6tw/actorsvoicemenu.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../data/savesystem.h"
#include "../../background2d.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::ActorsVoiceMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

void ActorsVoiceMenu::VoiceButtonOnClick(Button* target) {
  if (!target->IsLocked) {
    ScrWork[SW_ACTORVOICE_CUR] = target->Id;
  }
}

ActorsVoiceMenu::ActorsVoiceMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  MainItems = new Group(this);

  auto onClick = [this](auto* btn) { return VoiceButtonOnClick(btn); };

  auto pos = InitialItemPosition;

  for (int i = 0; i < ActorsVoiceCount; i++) {
    auto button = new Widgets::MO6TW::ActorsVoiceButton(
        i, UnlockedSprites[i], LockedSprites[i], UnlockedHighlightedSprites[i],
        LockedHighlightedSprites[i], pos);
    button->OnClickHandler = onClick;

    MainItems->Add(button, FDIR_DOWN);
    pos += ItemOffset;
  }
}

void ActorsVoiceMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();
    UpdateActorsVoiceEntries();
    ScrWork[SW_ACTORVOICE_CUR] = 255;
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void ActorsVoiceMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void ActorsVoiceMenu::UpdateInput(float dt) {
  Menu::UpdateInput(dt);
  if (State == Shown) {
    if (ScrWork[SW_ACTORVOICE_CUR] == 255) {
      MainItems->UpdateInput(dt);
    }
  }
}

void ActorsVoiceMenu::Update(float dt) {
  UpdateInput(dt);

  FadeAnimation.Update(dt);
  if (ScrWork[SW_ACTORVOICE_ALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_ACTORVOICE_ALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_ACTORVOICE_ALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut()) {
    State = Hidden;
  }

  if (State != Hidden) {
    MainItems->Update(dt);
  }
}

void ActorsVoiceMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    MainItems->Tint = col;
    MainItems->Render();
    if (ScrWork[SW_ACTORVOICE_CUR] != 255) {
      Renderer->DrawSprite(Backgrounds2D[CharacterBackgroundBufferId]->BgSprite,
                           glm::vec2(0.0f, 0.0f), col);
    }
  }
}

void ActorsVoiceMenu::UpdateActorsVoiceEntries() {
  for (auto el : MainItems->Children) {
    auto actorsVoiceButton = static_cast<Button*>(el);
    actorsVoiceButton->IsLocked =
        !GetFlag(SF_ACTORSVOICE_UNLOCK1 + actorsVoiceButton->Id);
  }
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto