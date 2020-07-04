//#include "titlemenu.h"
//
//#include "../../profile/ui/titlemenu.h"
//#include "../../profile/games/dash/titlemenu.h"
//#include "../../renderer2d.h"
//#include "../../texture/texture.h"
//#include "../../spritesheet.h"
//#include "../../audio/audiosystem.h"
//#include "../../audio/audiostream.h"
//#include "../../audio/audiochannel.h"
//#include "../../io/vfs.h"
//#include "../../mem.h"
//#include "../../profile/scriptvars.h"
//#include "../../background2d.h"
//
// namespace Impacto {
// namespace Dash {
//
// using namespace Impacto::Profile::TitleMenu;
// using namespace Impacto::Profile::Dash::TitleMenu;
// using namespace Impacto::Profile::ScriptVars;
//
// void TitleMenu::Show() {
//  if (State != Shown) {
//    State = Showing;
//  }
//}
// void TitleMenu::Hide() {
//  if (State != Hidden) {
//    State = Hiding;
//  }
//}
// void TitleMenu::Update(float dt) {
//  PressToStartAnimation.Update(dt);
//
//  if (State == Showing) {
//    if (PressToStartAnimation.State == AS_Stopped)
//      PressToStartAnimation.StartIn();
//  }
//}
// void TitleMenu::Render() {
//  if (State != Hidden && GetFlag(SF_TITLEMODE)) {
//    glm::vec4 col = glm::vec4(1.0f);
//    col.a = glm::smoothstep(0.0f, 1.0f, PressToStartAnimation.Progress);
//    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f));
//    Renderer2D::DrawSprite(PressToStartSprite,
//                           glm::vec2(PressToStartX, PressToStartY), col);
//  }
//}
//
//}  // namespace Dash
//}  // namespace Impacto