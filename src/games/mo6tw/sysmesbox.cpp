#include "sysmesbox.h"

#include "../../profile/ui/sysmesbox.h"
#include "../../profile/games/mo6tw/sysmesbox.h"
#include "../../profile/dialogue.h"
#include "../../profile/scriptvars.h"
#include "../../mem.h"
#include "../../renderer/renderer.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::UI::Widgets;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::MO6TW::SysMesBox;

void SysMesBox::ChoiceItemOnClick(Button* target) {
  ScrWork[SW_SYSSEL] = target->Id;
  ChoiceMade = true;
}

void SysMesBox::Show() {
  MessageItems = new Widgets::Group(this);
  ChoiceItems = new Widgets::Group(this);

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  auto onClick = [this](auto* btn) { return ChoiceItemOnClick(btn); };

  float diff = 0.0f;
  float maxWidth = FLT_MIN;
  for (int i = 0; i < MessageCount; i++) {
    if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
  }
  if (maxWidth < BoxMinimumWidth) maxWidth = BoxMinimumWidth;

  for (int i = 0; i < MessageCount; i++) {
    diff = Messages[i][0].DestRect.X - (TextX - (maxWidth / 2.0f));
    for (int j = 0; j < Messages[i].size(); j++) {
      Messages[i][j].Colors = Profile::Dialogue::ColorTable[0];
      Messages[i][j].DestRect.X -= diff;
      Messages[i][j].DestRect.Y = TextMiddleY + (i * TextLineHeight);
    }

    Label* message = new Label(Messages[i], MessageWidths[i], TextFontSize,
                               RendererOutlineMode::Full);

    MessageItems->Add(message, FDIR_DOWN);
  }

  float totalChoiceWidth = 0.0f;
  for (int i = 0; i < ChoiceCount; i++) {
    totalChoiceWidth += ChoiceWidths[i] + ChoicePadding;
  }
  if (maxWidth < totalChoiceWidth) maxWidth = totalChoiceWidth;
  if (maxWidth < MinMaxMesWidth) maxWidth = MinMaxMesWidth;

  ChoiceX = (maxWidth / 2.0f) - totalChoiceWidth + ChoiceXBase;

  float tempChoiceX = ChoiceX;

  for (int i = 0; i < ChoiceCount; i++) {
    diff = Choices[i][0].DestRect.X - tempChoiceX;
    for (int j = 0; j < Choices[i].size(); j++) {
      Choices[i][j].Colors = Profile::Dialogue::ColorTable[0];
      Choices[i][j].DestRect.X -= diff;
      Choices[i][j].DestRect.Y = ChoiceY;
    }

    Button* choice = new Button(
        i, nullSprite, nullSprite, SelectionHighlight,
        glm::vec2(Choices[i][0].DestRect.X, Choices[i][0].DestRect.Y));

    choice->SetText(Choices[i], ChoiceWidths[i],
                    Profile::Dialogue::DefaultFontSize,
                    RendererOutlineMode::Full);
    choice->OnClickHandler = onClick;

    ChoiceItems->Add(choice, FDIR_LEFT);

    tempChoiceX += ChoiceWidths[i] + ChoicePadding;
  }

  FadeAnimation.StartIn();
  MessageItems->Show();
  MessageItems->HasFocus = false;
  if (ChoiceCount != 0) ChoiceItems->Show();
  State = Showing;

  if (UI::FocusedMenu != 0) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  IsFocused = true;
  UI::FocusedMenu = this;
}

void SysMesBox::Hide() {
  FadeAnimation.StartOut();
  State = Hiding;
  if (LastFocusedMenu != 0) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
  } else {
    UI::FocusedMenu = 0;
  }
  IsFocused = false;
}

void SysMesBox::Update(float dt) {
  UpdateInput();

  FadeAnimation.Update(dt);
  if (State != Hidden) {
    if (FadeAnimation.IsIn()) State = Shown;
    if (FadeAnimation.IsOut()) State = Hidden;

    if (IsFocused) {
      MessageItems->Update(dt);
      ChoiceItems->Update(dt);
    }
  }
}

void SysMesBox::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);

    float maxWidth = FLT_MIN;
    for (int i = 0; i < MessageCount; i++) {
      if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
    }
    if (maxWidth < BoxMinimumWidth) maxWidth = BoxMinimumWidth;

    Renderer->DrawSprite(BoxPartLeft, glm::vec2(BoxX - (maxWidth / 2.0f), BoxY),
                         col);

    float remainWidth = maxWidth - BoxMiddleRemainBase;
    float currentX = BoxMiddleBaseX - (maxWidth / 2.0f);
    while (remainWidth >= BoxMiddleBaseWidth) {
      Renderer->DrawSprite(BoxPartMiddle, glm::vec2(currentX, BoxY), col);
      currentX += BoxMiddleBaseWidth;
      remainWidth -= BoxMiddleBaseWidth;
    }

    BoxPartRight.Bounds.X = BoxRightBaseX - (remainWidth + BoxRightRemainPad);
    BoxPartRight.Bounds.Width = (remainWidth + BoxRightRemainPad) - 1.0f;
    Renderer->DrawSprite(BoxPartRight, glm::vec2(currentX, BoxY), col);

    MessageItems->Tint.a = FadeAnimation.Progress;
    MessageItems->Render();
    ChoiceItems->Tint.a = FadeAnimation.Progress;
    ChoiceItems->Render();
  }
}

void SysMesBox::Init() {
  ChoiceMade = false;
  MessageCount = 0;
  ChoiceCount = 0;

  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
}

void SysMesBox::AddMessage(uint8_t* str) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  Messages[MessageCount] =
      TextLayoutPlainLine(&dummy, 255, Profile::Dialogue::DialogueFont,
                          TextFontSize, Profile::Dialogue::ColorTable[10], 1.0f,
                          glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (int i = 0; i < Messages[MessageCount].size(); i++) {
    mesLen += Messages[MessageCount][i].DestRect.Width;
  }
  MessageWidths[MessageCount] = mesLen;
  MessageCount++;
}

void SysMesBox::AddChoice(uint8_t* str) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  Choices[ChoiceCount] =
      TextLayoutPlainLine(&dummy, 255, Profile::Dialogue::DialogueFont,
                          TextFontSize, Profile::Dialogue::ColorTable[10], 1.0f,
                          glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (int i = 0; i < Choices[ChoiceCount].size(); i++) {
    mesLen += Choices[ChoiceCount][i].DestRect.Width;
  }
  ChoiceWidths[ChoiceCount] = mesLen;
  ChoiceCount++;
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto