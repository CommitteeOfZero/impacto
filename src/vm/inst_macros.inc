#define StartInstruction        \
  uint8_t* _oldIp = thread->Ip; \
  thread->Ip += 2
#define ResetInstruction thread->Ip = _oldIp

#define BlockThread BlockCurrentScriptThread = true

#define PopUint8(name)        \
  uint8_t name = *thread->Ip; \
  thread->Ip++
#define PopUint16(name)                                 \
  uint16_t name = SDL_SwapLE16(*(uint16_t*)thread->Ip); \
  thread->Ip += 2
#define PopLocalLabel(name)                                             \
  uint8_t* name;                                                        \
  {                                                                     \
    PopUint16(labelNum);                                                \
    name = ScriptGetLabelAddress(ScriptBuffers[thread->ScriptBufferId], \
                                 labelNum);                             \
  }                                                                     \
  (void)0
#define PopFarLabel(name, scriptBufferId)                                  \
  uint8_t* name;                                                           \
  {                                                                        \
    PopUint16(labelNum);                                                   \
    name = ScriptGetLabelAddress(ScriptBuffers[scriptBufferId], labelNum); \
  }                                                                        \
  (void)0
#define PopExpression(name) \
  int name;                 \
  ExpressionEval(thread, &name)
#define PopString(name)                                                        \
  uint8_t* name;                                                               \
  {                                                                            \
    PopUint16(stringNum);                                                      \
    name =                                                                     \
        ScriptGetStrAddress(ScriptBuffers[thread->ScriptBufferId], stringNum); \
  }                                                                            \
  (void)0