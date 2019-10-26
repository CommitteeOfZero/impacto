#include "sysmesbox.h"

#include "../impacto.h"
#include "../renderer2d.h"
#include "../game.h"
#include "../mem.h"
#include "../profile/hud/sysmesbox.h"
#include "../profile/dialogue.h"

namespace Impacto {
namespace SysMesBox {

using namespace Impacto::Profile::SysMesBox;

SysMesBoxBase* Implementation = 0;

void Init() { Configure(); }

void Show() {
  if (Implementation) Implementation->Show();
}
void Hide() {
  if (Implementation) Implementation->Hide();
}
void Update(float dt) {
  if (Implementation) Implementation->Update(dt);
}
void Render() {
  if (Implementation) Implementation->Render();
}

}  // namespace SysMesBox
}  // namespace Impacto