#include "savesystem.h"

#include "io/physicalfilestream.h"
#include "mem.h"
#include "vm/vm.h"
#include "profile/savesystem.h"

namespace Impacto {
namespace SaveSystem {

using namespace Impacto::Vm;
using namespace Impacto::Profile::SaveSystem;

SaveSystemBase* Implementation = 0;

void Init() { Configure(); }

SaveError MountSaveFile() {
  if (Implementation)
    return Implementation->MountSaveFile();
  else
    return SaveOK;  // Just so we don't get stuck at loading save data in
                    // script.
}

void SaveMemory(SaveType type, int id) {
  if (Implementation) Implementation->SaveMemory(type, id);
}

void LoadMemory(SaveType type, int id) {
  if (Implementation) Implementation->LoadMemory(type, id);
}

}  // namespace SaveSystem
}  // namespace Impacto