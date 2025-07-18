#include "bgeff.h"

#include <map>
#include "../profile_internal.h"
#include "../../background2d.h"

namespace Impacto {
namespace Profile {
namespace BgEff {

const static std::map<uint8_t, ShaderProgramType> ShaderIdToShaderType{
    {0x00, ShaderProgramType::Sprite},
    {0x01, ShaderProgramType::MaskedSprite},
    {0x02, ShaderProgramType::SoftLightMaskedSprite},
    {0x03, ShaderProgramType::AdditiveMaskedSprite},
    {0x04, ShaderProgramType::ColorMaskedSprite},
    {0x05, ShaderProgramType::ScreenMaskedSprite},
    {0x06, ShaderProgramType::OverlayMaskedSprite},
    {0x07, ShaderProgramType::ColorDodgeMaskedSprite},
    {0x08, ShaderProgramType::HardLightMaskedSprite},
    {0x09, ShaderProgramType::ColorBurnMaskedSprite},
    {0x10, ShaderProgramType::LinearBurnMaskedSprite},
};

void Load() {
  EnsurePushMemberOfType("BgEffData", LUA_TTABLE);

  if (TryPushMember("BgEffShaderData")) {
    AssertIs(LUA_TTABLE);

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

      const int bgId = EnsureGetArrayElementByIndex<int>(0);
      std::array<uint8_t, 4> shaders{
          EnsureGetArrayElementByIndex<uint8_t>(1),
          EnsureGetArrayElementByIndex<uint8_t>(2),
          EnsureGetArrayElementByIndex<uint8_t>(3),
          EnsureGetArrayElementByIndex<uint8_t>(4),
      };

      for (uint8_t& shader : shaders) {
        if (!ShaderIdToShaderType.contains(shader)) {
          shader = 0x01;
          ImpLog(LogLevel::Error, LogChannel::Profile,
                 "Unmapped shader type {:02x}; defaulting to Sprite!", shader);
        }
      }

      std::array<ShaderProgramType, 4> shaderTypes{
          ShaderIdToShaderType.at(shaders[0]),
          ShaderIdToShaderType.at(shaders[1]),
          ShaderIdToShaderType.at(shaders[2]),
          ShaderIdToShaderType.at(shaders[3]),
      };

      BgEffShaders.insert(std::pair{bgId, shaderTypes});

      Pop();
    }

    Pop();
  }

  Pop();
}

}  // namespace BgEff
}  // namespace Profile
}  // namespace Impacto
