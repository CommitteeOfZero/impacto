#pragma once

#include <string>

namespace Impacto {
namespace Audio {
namespace ControllerAudio {

void Init();
void Update(float dt, bool enabled, bool deviceListChanged);
void Shutdown();

std::string const& GetDevice();

}  // namespace ControllerAudio
}  // namespace Audio
}  // namespace Impacto
