#pragma once

#include "../impacto.h"
#include "../log.h"
#include "audiochannel.h"

namespace Impacto {

class Game;

namespace Audio {

class AudioSystem {
 public:
  ~AudioSystem();

  void Init(Game* game);

  void Update(float dt);

  float MasterVolume = 1.0f;
  float GroupVolumes[ACG_Count];

  AudioChannel Channels[AC_Count];

 private:
  bool IsInit = false;
  Game* GameCtx;
  ALCdevice* AlcDevice = 0;
  ALCcontext* AlcContext = 0;
};

}  // namespace Audio
}  // namespace Impacto