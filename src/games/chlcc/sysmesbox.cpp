#include "sysmesbox.h"

#include "../../impacto.h"
#include "../../renderer2d.h"
#include "../../game.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../profile/scriptvars.h"
#include "../../profile/ui/sysmesbox.h"
#include "../../profile/games/chlcc/sysmesbox.h"
#include "../../profile/dialogue.h"

namespace Impacto {
namespace UI {
namespace CHLCC {

using namespace Impacto::UI::Widgets;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::SysMesBox;
using namespace Impacto::Profile::CHLCC::SysMesBox;

static float boxanimcount = 0.0f;
static float boxtopy = 0.0f;
static float boxbottomy = 0.0f;
static float linelength = 0.0f;
static float boxheight = 0.0f;
static float boxprogresscount = 0.0f;
static int textstartcount = 0;

void SysMesBox::ChoiceItemOnClick(Button* target) {
  ScrWork[SW_SYSSEL] = target->Id;
  ChoiceMade = true;
}

void SysMesBox::Show() {
  MessageItems = new WidgetGroup();
  ChoiceItems = new WidgetGroup();

  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);

  auto onClick =
      std::bind(&SysMesBox::ChoiceItemOnClick, this, std::placeholders::_1);

  float diff = 0.0f;
  float maxWidth = FLT_MIN;
  for (int i = 0; i < MessageCount; i++) {
    if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
  }
  if (maxWidth < MinHighlightWidth) maxWidth = MinHighlightWidth;

  for (int i = 0; i < MessageCount; i++) {
    diff = Messages[i][0].DestRect.X - (TextX - (maxWidth / 2.0f));
    for (int j = 0; j < MessageLengths[i]; j++) {
      Messages[i][j].Colors = Profile::Dialogue::ColorTable[0];
      Messages[i][j].DestRect.X -= diff;
      Messages[i][j].DestRect.Y = TextMiddleY + (i * TextLineHeight);
    }

    Label* message = new Label(Messages[i], MessageLengths[i], MessageWidths[i],
                               TextFontSize, true);

    MessageItems->Add(message, FocusDirection::Vertical);
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
    for (int j = 0; j < ChoiceLengths[i]; j++) {
      Choices[i][j].Colors = Profile::Dialogue::ColorTable[0];
      Choices[i][j].DestRect.X -= diff;
      Choices[i][j].DestRect.Y = ChoiceY;
    }

    Button* choice = new Button(
        i, nullSprite, nullSprite, SelectionLeftPart,
        glm::vec2(Choices[i][0].DestRect.X, Choices[i][0].DestRect.Y));

    choice->SetText(Choices[i], ChoiceLengths[i], ChoiceWidths[i],
                    Profile::Dialogue::DefaultFontSize, true);
    choice->OnClickHandler = onClick;

    ChoiceItems->Add(choice, FocusDirection::Horizontal);

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

    float diff = 0.0f;
    float maxWidth = FLT_MIN;
    for (int i = 0; i < MessageCount; i++) {
      if (maxWidth < MessageWidths[i]) maxWidth = MessageWidths[i];
    }
    if (maxWidth < MinMaxMesWidth) maxWidth = MinMaxMesWidth;

    Renderer2D::DrawSprite(Box, glm::vec2(BoxX - (maxWidth / 2.0f), BoxY), col);
    MessageItems->Opacity = FadeAnimation.Progress;
    MessageItems->Render();
    ChoiceItems->Opacity = FadeAnimation.Progress;
    ChoiceItems->Render();
  }
}

void SysMesBox::Init() {
  ChoiceMade = false;
  MessageCount = 0;
  ChoiceCount = 0;

  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;

  memset(Messages, 0, (8 * 255) * sizeof(ProcessedTextGlyph));
  memset(Choices, 0, (8 * 255) * sizeof(ProcessedTextGlyph));
}

void SysMesBox::AddMessage(uint8_t* str) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  int len = TextLayoutPlainLine(&dummy, 255, Messages[MessageCount],
                                Profile::Dialogue::DialogueFont, TextFontSize,
                                Profile::Dialogue::ColorTable[10], 1.0f,
                                glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (int i = 0; i < len; i++) {
    mesLen += Messages[MessageCount][i].DestRect.Width;
  }
  MessageWidths[MessageCount] = mesLen;
  MessageLengths[MessageCount] = len;
  MessageCount++;
}

void SysMesBox::AddChoice(uint8_t* str) {
  Impacto::Vm::Sc3VmThread dummy;
  dummy.Ip = str;
  int len = TextLayoutPlainLine(&dummy, 255, Choices[ChoiceCount],
                                Profile::Dialogue::DialogueFont, TextFontSize,
                                Profile::Dialogue::ColorTable[10], 1.0f,
                                glm::vec2(TextX, 0.0f), TextAlignment::Left);
  float mesLen = 0.0f;
  for (int i = 0; i < len; i++) {
    mesLen += Choices[ChoiceCount][i].DestRect.Width;
  }
  ChoiceWidths[ChoiceCount] = mesLen;
  ChoiceLengths[ChoiceCount] = len;
  ChoiceCount++;
}
/*void sysmesbox::show() {
state = showing;

// set up so we don't have to do this each frame
float diff = 0.0f;
float maxwidth = flt_min;
for (int i = 0; i < messagecount; i++) {
  if (maxwidth < messagewidths[i]) maxwidth = messagewidths[i];
}

float totalchoicewidth = 0.0f;
for (int i = 0; i < choicecount; i++) {
  totalchoicewidth += choicewidths[i] + choicepadding;
}
if (maxwidth < totalchoicewidth) maxwidth = totalchoicewidth;
if (maxwidth < minmaxmeswidth) maxwidth = minmaxmeswidth;

choicex = (maxwidth / 2.0f) - totalchoicewidth + choicexbase;

for (int i = 0; i < messagecount; i++) {
  diff = messages[i][0].destrect.x - (textx - (maxwidth / 2.0f));
  for (int j = 0; j < messagelengths[i]; j++) {
    messages[i][j].colors = profile::dialogue::colortable[0];
    messages[i][j].destrect.x -= diff;
    messages[i][j].destrect.y = textmiddley + (i * textlineheight);
  }
}

float tempchoicex = choicex;

for (int i = 0; i < choicecount; i++) {
  diff = choices[i][0].destrect.x - tempchoicex;
  for (int j = 0; j < choicelengths[i]; j++) {
    choices[i][j].colors = profile::dialogue::colortable[0];
    choices[i][j].destrect.x -= diff;
    choices[i][j].destrect.y = choicey;
  }
  tempchoicex += choicewidths[i] + choicepadding;
}
}
void sysmesbox::hide() { state = hiding; }

void sysmesbox::update(float dt) {
fadeanimation.update(dt);

if (state == hiding) {
  boxanimcount -= animationspeed * dt;
  if (boxanimcount <= 0.0f) {
    boxanimcount = 0.0f;
    state = hidden;
  }
} else if (state == showing) {
  boxanimcount += animationspeed * dt;
  if (boxanimcount >= scrwork[sw_sysmesanimctf]) {
    boxanimcount = scrwork[sw_sysmesanimctf];
    state = shown;
  }
}

scrwork[sw_sysmesanimctcur] = std::floor(boxanimcount);

if (state == shown) {
  // nice input
  if (input::keyboardbuttonwentdown[sdl_scancode_right]) {
    if (currentchoice == 255)
      currentchoice = 1;
    else {
      currentchoice++;
      if (currentchoice > 1) currentchoice = 0;
    }
  } else if (input::keyboardbuttonwentdown[sdl_scancode_left]) {
    if (currentchoice == 255)
      currentchoice = 0;
    else {
      currentchoice--;
      if (currentchoice < 0) currentchoice = 1;
    }
  } else if (vm::interface::pad1a & vm::interface::padinputbuttonwentdown) {
    choicemade = true;
  }
}

if (state != hidden) {
  if (state == showing && fadeanimation.isout())
    fadeanimation.startin();
  else if (state == hiding && fadeanimation.isin())
    fadeanimation.startout();

  if (currentchoice != 255 && choicecount > 0) {
    highlightx = choicex;
    for (int i = 0; i < currentchoice; i++) {
      highlightx += choicewidths[i] + choicepadding;
    }
    selectedwidth = choicewidths[currentchoice];
    selectedwidthadjusted = selectedwidth + highlightrightpartspritewidth;
    if (selectedwidthadjusted < minhighlightwidth) {
      selectedwidth = minhighlightwidth;
      selectedwidthadjusted = selectedwidth;
    }
  }
}
}

void sysmesbox::render() {
if (boxopacity) {
  glm::vec4 col(1.0f, 1.0f, 1.0f, fadeanimation.progress);
  renderer2d::drawsprite(box, glm::vec2(boxx, boxy), col);

  if (currentchoice != 255 && choicecount > 0) {
    if (selectedwidth < highlightbasewidth) {
      rectf prevbounds = selectionleftpart.bounds;
      selectionleftpart.bounds =
          rectf(selectionleftpart.bounds.x, selectionleftpart.bounds.y,
                selectedwidth, selectionleftpart.bounds.height);
      renderer2d::drawsprite(selectionleftpart,
                             glm::vec2(highlightx - highlightxoffset,
                                       choicey - highlightyoffset),
                             col);
      selectionleftpart.bounds = prevbounds;
      renderer2d::drawsprite(
          selectionrightpart,
          glm::vec2(highlightx + selectedwidth - highlightxoffset,
                    choicey - highlightyoffset),
          col);
    } else {
      renderer2d::drawsprite(selectionleftpart,
                             glm::vec2(highlightx - highlightxoffset,
                                       choicey - highlightyoffset),
                             col);
      selectedwidthadjusted -= highlightbasewidth;
      highlightx += highlightxstep + 1.0f;
      while (selectedwidthadjusted > highlightbasewidth) {
        renderer2d::drawsprite(
            selectionmiddlepart,
            glm::vec2(highlightx, choicey - highlightyoffset), col);
        highlightx += highlightxstep;
        selectedwidthadjusted -= highlightxstep;
      }
      rectf prevbounds = selectionrightpart.bounds;
      selectionrightpart.bounds =
          rectf(highlightxbase - selectedwidthadjusted - 1.0f,
                selectionrightpart.bounds.y, selectedwidthadjusted,
                selectionrightpart.bounds.height);
      renderer2d::drawsprite(
          selectionrightpart,
          glm::vec2(highlightx, choicey - highlightyoffset), col);
      selectionrightpart.bounds = prevbounds;
    }
  }

  for (int i = 0; i < messagecount; i++) {
    renderer2d::drawprocessedtext(messages[i], messagelengths[i],
                                  profile::dialogue::dialoguefont, col.a,
                                  true);
  }

  for (int i = 0; i < choicecount; i++) {
    renderer2d::drawprocessedtext(choices[i], choicelengths[i],
                                  profile::dialogue::dialoguefont, col.a,
                                  true);
  }
}
}*/

}  // namespace CHLCC
}  // namespace UI
}  // namespace Impacto