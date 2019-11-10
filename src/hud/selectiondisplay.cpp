#include "selectiondisplay.h"

#include "../impacto.h"
#include "../renderer2d.h"
#include "../inputsystem.h"
#include "../profile/dialogue.h"
#include "../vm/interface/input.h"

namespace Impacto {
namespace SelectionDisplay {

// using namespace Impacto::Profile::SelectionDisplay;

SelectionDisplayState State;

bool IsPlain;
int ChoiceCount;
bool ChoiceMade = false;
int CurrentChoice = -1;
ProcessedTextGlyph Choices[15][255];  // The official implementation limit is
                                      // actually 60, but... really? 60 choices?
int ChoiceLengths[15];
float ChoiceWidths[15];

void Init() {
  // Configure();
}

void Show() { State = Shown; }
void Hide() {
  State = Hidden;
  ChoiceMade = false;
}

void Update(float dt) {
  if (State == Shown) {
    if (Input::KeyboardButtonWentDown[SDL_SCANCODE_DOWN]) {
      CurrentChoice++;
      if (CurrentChoice > ChoiceCount - 1) CurrentChoice = 0;
    } else if (Input::KeyboardButtonWentDown[SDL_SCANCODE_UP]) {
      CurrentChoice--;
      if (CurrentChoice < 0) CurrentChoice = ChoiceCount - 1;
    } else if (Vm::Interface::PAD1A & Vm::Interface::PADinputWentDown) {
      ChoiceMade = true;
    }
  }
}

void Render() {
  if (State == Shown) {
    for (int i = 0; i < ChoiceCount; i++) {
      bool outline = false;
      if (i == CurrentChoice) outline = true;
      Renderer2D::DrawProcessedText(Choices[i], ChoiceLengths[i],
                                    Profile::Dialogue::DialogueFont, 1.0f,
                                    outline);
    }
  }
}

}  // namespace SelectionDisplay
}  // namespace Impacto