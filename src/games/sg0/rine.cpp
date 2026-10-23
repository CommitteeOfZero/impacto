#include "rine.h"

#include <algorithm>

#include "../../mem.h"
#include "../../vm/vm.h"
#include "../../vm/expression.h"

namespace Impacto {
namespace UI {
namespace SG0 {

namespace {
constexpr int SW_RINE_TEXT_BASE = 6655;
constexpr int SW_RINE_SCROLL_MAX = 6653;
constexpr int SF_RINE_NOT_REPLIED = 2614;
constexpr int SF_RINE_READ = 2615;
constexpr int SF_RINE_NOTIFY_A = 2741;
constexpr int SF_RINE_NOTIFY_B = 2742;
constexpr int BranchThreadGroup = 8;  // the phone's thread group
constexpr int TypingDelayFrames = 40;
constexpr int MemberListEnd = 15;

bool IsPhoneOp(const uint8_t* ip, uint8_t subType) {
  return ip[0] == 0x10 && ip[1] == 0x37 && ip[2] == subType;
}
}  // namespace

void Rine::LoadParticipants(uint32_t phoneBuffer, uint16_t label) {
  PhoneBuffer = phoneBuffer;
  const std::span<uint8_t> script = Vm::ScriptBuffers[phoneBuffer];
  uint32_t pos = Vm::ScriptGetLabelAddress(phoneBuffer, label);
  Participants.clear();
  for (int i = 0; i <= MemberListEnd && pos + 3 <= script.size();
       i++, pos += 3) {
    Participants.push_back(
        {script[pos], UnalignedRead<uint16_t>(&script[pos + 1])});
  }
}

void Rine::SelectTextList(uint16_t label) {
  const uint32_t pos = Vm::ScriptGetLabelAddress(PhoneBuffer, label);
  ScrWork[SW_RINE_TEXT_BASE] =
      UnalignedRead<uint16_t>(&Vm::ScriptBuffers[PhoneBuffer][pos]);
}

void Rine::NewThread(int owner) {
  Owner = owner;
  Members = {owner};
  Messages.clear();
  InputText = -1;
  Version++;
}

void Rine::AddMember(int participant) {
  if (participant == MemberListEnd) return;
  if (std::find(Members.begin(), Members.end(), participant) == Members.end()) {
    Members.push_back(participant);
    Version++;
  }
}

void Rine::AddMessage(int sender, int kind, int value) {
  Messages.push_back({sender, kind, value});
  ScrWork[SW_RINE_SCROLL_MAX] = 0;  // the phone UI scrolls to the newest itself
  Version++;
}

void Rine::ClearAll() {
  Members.clear();
  Messages.clear();
  InputText = -1;
  MenuPending = false;
  Version++;
}

void Rine::SetInput(int textValue) {
  InputText = textValue;
  Version++;
}

void Rine::ClearInput() {
  InputText = -1;
  Version++;
}

void Rine::SetReplyMenu(uint32_t scriptBufferId, uint16_t label) {
  MenuBuffer = scriptBufferId;
  MenuLabel = label;
  MenuPending = true;
  Version++;
}

void Rine::CancelReplyMenu() {
  MenuPending = false;
  Version++;
}

bool Rine::ReadReplyMenu(int& sender, int& kind,
                         std::vector<RineReplyOption>& options) const {
  if (!MenuPending) return false;
  Vm::Sc3VmThread reader;
  reader.ScriptBufferId = MenuBuffer;
  reader.IpOffset = Vm::ScriptGetLabelAddress(MenuBuffer, MenuLabel);
  if (!IsPhoneOp(reader.GetIp(), 0x0C)) return false;
  reader.IpOffset += 3;
  sender = Vm::ExpressionEval(&reader);
  kind = Vm::ExpressionEval(&reader);

  options.clear();
  while (IsPhoneOp(reader.GetIp(), 0x0D) && options.size() < 8) {
    reader.IpOffset += 3;
    const int value = Vm::ExpressionEval(&reader);
    const uint16_t branch = UnalignedRead<uint16_t>(reader.GetIp());
    reader.IpOffset += 2;
    options.push_back({value, branch});
  }
  return !options.empty();
}

void Rine::Reply(int optionIndex) {
  int sender = 0;
  int kind = 0;
  std::vector<RineReplyOption> options;
  if (!ReadReplyMenu(sender, kind, options) || optionIndex < 0 ||
      optionIndex >= static_cast<int>(options.size())) {
    return;
  }
  const RineReplyOption& option = options[optionIndex];
  AddMessage(sender, kind, option.Value);
  SetFlag(SF_RINE_NOT_REPLIED, false);
  MenuPending = false;

  Vm::Sc3VmThread* branch = Vm::CreateThread(BranchThreadGroup);
  branch->GroupId = BranchThreadGroup;
  branch->ScriptBufferId = MenuBuffer;
  branch->IpOffset = Vm::ScriptGetLabelAddress(MenuBuffer, option.BranchLabel);
  BranchThreadId = branch->Id;
  TypingFrames = 0;
}

bool Rine::IsBranchThread(const Vm::Sc3VmThread* thread) const {
  return thread->Id == BranchThreadId;
}

void Rine::EndBranch(Vm::Sc3VmThread* thread) {
  thread->Flags = Vm::TF_Destroy;
  BranchThreadId = UINT32_MAX;
}

bool Rine::TypingDone() {
  if (++TypingFrames < TypingDelayFrames) return false;
  TypingFrames = 0;
  return true;
}

void Rine::MarkRead() {
  SetFlag(SF_RINE_READ, true);
  SetFlag(SF_RINE_NOTIFY_A, false);
  SetFlag(SF_RINE_NOTIFY_B, false);
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
