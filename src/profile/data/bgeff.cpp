#include "bgeff.h"

#include <map>
#include "../profile_internal.h"
#include "../../background2d.h"

namespace Impacto {
namespace Profile {
namespace BgEff {

void Load() {
  using magic_enum::enum_cast;
  EnsurePushMemberOfType("BgEffData", LUA_TTABLE);

  {
    EnsurePushMemberOfType("BgEffShaderData", LUA_TTABLE);

    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      AssertIs(LUA_TTABLE);

      auto arraySize = lua_rawlen(LuaState, -1);
      if (arraySize != 5) {
        ImpLog(LogLevel::Fatal, LogChannel::Profile,
               "Expected five entries in shader definition defined; got {:d}\n",
               arraySize);
        Window->Shutdown();
      }

      Background2D::BgEffShaderMap.emplace(
          EnsureGetArrayElementByIndex<int>(0),
          std::array<ShaderProgramType, 4>{
              enum_cast<ShaderProgramType>(EnsureGetArrayElementByIndex<int>(1))
                  .value(),
              enum_cast<ShaderProgramType>(EnsureGetArrayElementByIndex<int>(2))
                  .value(),
              enum_cast<ShaderProgramType>(EnsureGetArrayElementByIndex<int>(3))
                  .value(),
              enum_cast<ShaderProgramType>(EnsureGetArrayElementByIndex<int>(4))
                  .value(),
          });
      Pop();
    }

    Pop();
  }

  {
    EnsurePushMemberOfType("BgEffTextureData", LUA_TTABLE);

    PushInitialIndex();
    while (PushNextTableElement() != 0) {
      AssertIs(LUA_TTABLE);

      auto arraySize = lua_rawlen(LuaState, -1);
      if (arraySize != 5) {
        ImpLog(
            LogLevel::Fatal, LogChannel::Profile,
            "Expected five entries in texture id mapping defined; got {:d}\n",
            arraySize);
        Window->Shutdown();
      }

      Background2D::BgEffTextureIdMap.emplace(
          EnsureGetArrayElementByIndex<int>(0),
          std::array<int, 4>{
              EnsureGetArrayElementByIndex<int>(1),
              EnsureGetArrayElementByIndex<int>(2),
              EnsureGetArrayElementByIndex<int>(3),
              EnsureGetArrayElementByIndex<int>(4),
          });

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace BgEff
}  // namespace Profile
}  // namespace Impacto
