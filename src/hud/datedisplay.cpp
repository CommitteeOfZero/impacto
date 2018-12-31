#include "datedisplay.h"

namespace Impacto {
namespace DateDisplay {

DateDisplay* Implementation = 0;

void Update(float dt) { Implementation->Update(dt); }
void Render() { Implementation->Render(); }

}  // namespace DateDisplay
}  // namespace Impacto