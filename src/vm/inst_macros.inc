#define StartInstruction                                     \
  uint8_t* _oldIp = thread->GetIp();                         \
  bool noExpressions = (bool)(*thread->GetIp() & 0x80);      \
  /* Sometimes needed by subsequent code, silence warning */ \
  (void)noExpressions;                                       \
  (void)_oldIp;                                              \
  thread->IpOffset += 2
#define ResetInstruction thread->SetIp(_oldIp)

#define BlockThread BlockCurrentScriptThread = true

#define PopUint8(name)                                       \
  uint8_t name = *thread->GetIp();                           \
  /* Sometimes needed by subsequent code, silence warning */ \
  (void)name;                                                \
  thread->IpOffset += 1;
#define PopUint16(name)                                                   \
  uint16_t name = SDL_SwapLE16(UnalignedRead<uint16_t>(thread->GetIp())); \
  /* Sometimes needed by subsequent code, silence warning */              \
  (void)name;                                                             \
  thread->IpOffset += 2;
#define PopUint32(name)                                                   \
  uint32_t name = SDL_SwapLE32(UnalignedRead<uint32_t>(thread->GetIp())); \
  /* Sometimes needed by subsequent code, silence warning */              \
  (void)name;                                                             \
  thread->IpOffset += 4;
#define PopLocalLabel(name)                                         \
  uint32_t name;                                                    \
  {                                                                 \
    PopUint16(labelNum);                                            \
    name = ScriptGetLabelAddress(thread->ScriptBufferId, labelNum); \
  }                                                                 \
  (void)0
#define PopFarLabel(name, scriptBufferId)                   \
  uint32_t name;                                            \
  {                                                         \
    PopUint16(labelNum);                                    \
    name = ScriptGetLabelAddress(scriptBufferId, labelNum); \
  }                                                         \
  (void)0
#define PopExpression(name) [[maybe_unused]] int name = ExpressionEval(thread)
#define PopStringId(name)                            \
  uint32_t name;                                     \
  {                                                  \
    if (Profile::Vm::StringIdSize == 2) {            \
      PopUint16(stringId_);                          \
      name = stringId_;                              \
    } else if (Profile::Vm::StringIdSize == 4) {     \
      PopUint32(stringId_);                          \
      name = stringId_;                              \
    } else {                                         \
      ImpLogSlow(LogLevel::Error, LogChannel::VM,    \
                 "Unsupported StringIdSize: {:d}\n", \
                 Profile::Vm::StringIdSize);         \
      name = std::numeric_limits<uint32_t>::max();   \
    }                                                \
  }                                                  \
  (void)0

#define PopString(name)                                            \
  uint32_t name;                                                   \
  {                                                                \
    PopUint16(stringNum);                                          \
    name = ScriptGetStrAddress(thread->ScriptBufferId, stringNum); \
  }                                                                \
  (void)0
#define PopMsbString(name)                                      \
  uint32_t name;                                                \
  {                                                             \
    PopExpression(stringNum);                                   \
    name = MsbGetStrAddress(thread->ScriptBufferId, stringNum); \
  }                                                             \
  (void)0
