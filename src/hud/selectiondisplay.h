#pragma once

#include "../text.h"

namespace Impacto {
namespace SelectionDisplay {

enum SelectionDisplayState { Hidden, Hiding, Showing, Shown };

extern SelectionDisplayState State;

extern bool IsPlain;
extern bool ChoiceMade;
extern int ChoiceCount;
extern int CurrentChoice;
extern ProcessedTextGlyph
    Choices[15][255];  // The official implementation limit is
                       // actually 60, but... really? 60 choices?
extern int ChoiceLengths[15];
extern float ChoiceWidths[15];

void Init();
void Show();
void Hide();
void Update(float dt);
void Render();

}  // namespace SelectionDisplay
}  // namespace Impacto