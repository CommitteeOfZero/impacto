#define StartInstruction        \
  uint8_t* _oldIp = thread->Ip; \
  thread->Ip += 2
#define ResetInstruction thread->Ip = _oldIp

#define BlockThread thread->VmContext->BlockCurrentScriptThread = true

#define PopUint8(name)        \
  uint8_t name = *thread->Ip; \
  thread->Ip++
#define PopUint16(name)                   \
  uint16_t name = SDL_SwapLE16(*(uint16_t*)thread->Ip); \
  thread->Ip += 2
#define PopLocalLabel(name)                                                                           \
  uint8_t* name;                                                                                      \
  {                                                                                                   \
    PopUint16(labelNum);                                                                              \
    name = ScriptGetLabelAddress(thread->VmContext->ScriptBuffers[thread->ScriptBufferId], labelNum); \
  }                                                                                                   \
  (void)0
#define PopFarLabel(name, scriptBufferId)                                                             \
  uint8_t* name;                                                                                      \
  {                                                                                                   \
    PopUint16(labelNum);                                                                              \
    name = ScriptGetLabelAddress(thread->VmContext->ScriptBuffers[scriptBufferId], labelNum);         \
  }                                                                                                   \
  (void)0
#define PopExpression(name) \
  uint32_t name;            \
  ExpressionEval(thread, &name)