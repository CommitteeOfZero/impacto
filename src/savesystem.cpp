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

void SaveMemory() {
  if (Implementation) Implementation->SaveMemory();
}

void LoadMemory(SaveType type, int id) {
  if (Implementation) Implementation->LoadMemory(type, id);
}

void FlushWorkingSaveEntry(SaveType type, int id) {
  if (Implementation) Implementation->FlushWorkingSaveEntry(type, id);
}

void WriteSaveFile() {
  if (Implementation) Implementation->WriteSaveFile();
}

}  // namespace SaveSystem
}  // namespace Impacto