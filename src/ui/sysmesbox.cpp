#include "sysmesbox.h"

namespace Impacto {
namespace UI {

void SysMesBox::Show() {}
void SysMesBox::Hide() {}
void SysMesBox::Update(float dt) {}
void SysMesBox::Render() {}
void SysMesBox::Init() {}
void SysMesBox::AddMessage(Vm::BufferOffsetContext) {}
void SysMesBox::AddChoice(Vm::BufferOffsetContext) {}

SysMesBox* SysMesBox::Current(uint32_t id) {
  auto it = MsgBoxes.find(id);
  return it == MsgBoxes.end() ? nullptr : it->second.get();
}

SysMesBox* SysMesBox::Push(uint32_t id) {
  auto it = MsgBoxes.find(id);
  if (it != MsgBoxes.end()) {
    it->second->Init();
    return it->second.get();
  }

  std::unique_ptr<SysMesBox> box(Factory());
  box->Init();
  SysMesBox* boxPtr = box.get();
  MsgBoxes[id] = std::move(box);
  return boxPtr;
}

void SysMesBox::Pop(uint32_t id) { MsgBoxes.erase(id); }

}  // namespace UI
}  // namespace Impacto