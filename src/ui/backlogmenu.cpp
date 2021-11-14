#include "backlogmenu.h"

#include "ui.h"
#include "../renderer2d.h"
#include "../mem.h"
#include "../profile/scriptvars.h"
#include "../profile/dialogue.h"
#include "../profile/ui/backlogmenu.h"
#include "../inputsystem.h"
#include "../io/vfs.h"

namespace Impacto {
namespace UI {

using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::BacklogMenu;

using namespace Impacto::UI::Widgets;

void BacklogMenu::MenuButtonOnClick(Widgets::BacklogEntry* target) {
  if (target->AudioId != -1) {
    Io::InputStream* stream;
    Io::VfsOpen("voice", target->AudioId, &stream);
    Audio::Channels[Audio::AC_REV].Play(Audio::AudioStream::Create(stream),
                                        false, 0.0f);
  }
}

BacklogMenu::BacklogMenu() {
  MainItems = new WidgetGroup();
  // TEMP //
  MainItems->RenderingBounds = RectF(0.0f, 75.0f, 1280.0f, 615.0f);
  //////////

  CurrentEntryPos = EntriesStart;

  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
}

void BacklogMenu::Show() {
  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void BacklogMenu::Hide() {
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

void BacklogMenu::Update(float dt) {
  FadeAnimation.Update(dt);

  if (FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut())
    State = Hidden;

  if (State == Shown && IsFocused) {
    // TEMP //
    if (Input::KeyboardButtonIsDown[SDL_SCANCODE_K]) {
      MainItems->Move(glm::vec2(0.0f, 10.0f));
      CurrentEntryPos.y += 10.0f;
    }
    if (Input::KeyboardButtonIsDown[SDL_SCANCODE_I]) {
      MainItems->Move(glm::vec2(0.0f, -10.0f));
      CurrentEntryPos.y += -10.0f;
    }
    if (Input::KeyboardButtonIsDown[SDL_SCANCODE_L]) {
      MainItems->Move(glm::vec2(10.0f, 0.0f));
      CurrentEntryPos.x += 10.0f;
    }
    if (Input::KeyboardButtonIsDown[SDL_SCANCODE_J]) {
      MainItems->Move(glm::vec2(-10.0f, 0.0f));
      CurrentEntryPos.x += -10.0f;
    }
    //////////
    MainItems->Update(dt);
  }
}

void BacklogMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    MainItems->Opacity = col.a;
    Renderer2D::DrawSprite(BacklogBackground, glm::vec2(0.0f));
    MainItems->Render();
  }
}

void BacklogMenu::AddMessage(uint8_t* str, int audioId) {
  auto onClick =
      std::bind(&BacklogMenu::MenuButtonOnClick, this, std::placeholders::_1);

  auto backlogEntry = new BacklogEntry(
      CurrentId, str, audioId, glm::vec2(CurrentEntryPos.x, CurrentEntryPos.y));
  CurrentId += 1;
  CurrentEntryPos.y += backlogEntry->TextHeight + EntryYPadding;
  backlogEntry->OnClickHandler = onClick;
  MainItems->Add(backlogEntry, FocusDirection::Vertical);
  // MainItems->Move(glm::vec2(-(backlogEntry->TextHeight + EntryYPadding),
  // 0.0f));
}

}  // namespace UI
}  // namespace Impacto