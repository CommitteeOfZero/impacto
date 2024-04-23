#pragma once

#include "../impacto.h"

namespace Impacto {
namespace UI {
namespace MapSystem {

class MapSystemBase {
 public:
  virtual void MapInit() = 0;
  virtual void MapSetFadein(int32_t partId, int32_t partType) = 0;
  virtual void MapSetGroup(int32_t index, int32_t mappedId1, int32_t mappedId2,
                           int32_t mappedId3) = 0;
  virtual void MapSetFadeout(int32_t partId, int32_t partType) = 0;
  virtual void MapSetDisp(int32_t partId, int32_t partType) = 0;
  virtual void MapSetHide(int32_t arg1, int32_t arg2) = 0;
  virtual bool MapPoolFadeEndChk_Wait() = 0;
  virtual void MapMoveAnimeInit(int32_t arg1, int32_t arg2, int32_t arg3) = 0;
  virtual bool MapMoveAnimeMain() = 0;
  virtual void MapGetPos(int32_t partId, int32_t partType, int32_t& getX, int32_t& getY) = 0;
  virtual void MapSetPool(int32_t index, int32_t id, int32_t type) = 0;
  virtual void MapResetPoolAll(int32_t arg1) = 0;
  virtual bool MapFadeEndChk_Wait() = 0;
  virtual void MapPoolShuffle(int32_t param_1) = 0;
  virtual void MapPoolSetDisp(int32_t arg1, int32_t arg2) = 0;
  virtual void MapPoolSetHide(int32_t arg1, int32_t arg2) = 0;
  virtual void MapPoolSetFadein(int32_t unused, int32_t poolIdx) = 0;
  virtual void MapPoolSetFadeout(int32_t unused, int32_t poolIdx) = 0;
  virtual bool MapPlayerPhotoSelect(int32_t unused) = 0;
  virtual void MapResetPool(int32_t poolIdx) = 0;
  virtual void MapSetGroupEx(int32_t index, int32_t type, int32_t mappedId) = 0;
  virtual void MapZoomInit(int32_t mapX, int32_t mapY, int32_t size) = 0;
  virtual bool MapZoomMain() = 0;
  virtual void MapZoomInit2(int32_t arg1, int32_t arg2) = 0;
  virtual bool MapZoomMain3() = 0;
  virtual bool MapZoomInit3(int32_t setMapX, int32_t setMapY, int32_t setMapSize,
                            bool halfZoom = false) = 0;
  virtual bool MapMoveAnimeInit2(int32_t setMapX, int32_t setMapY,
                                 int32_t setTransitionSize) = 0;
  virtual bool MapMoveAnimeMain2() = 0;
  virtual void MapPlayerPotalSelectInit() = 0;
  virtual bool MapPlayerPotalSelect() = 0;
  virtual void MapSystem_28() = 0;
  virtual void Update(float dt) = 0;
  virtual void RenderButtonGuide() = 0;
  virtual void Render() = 0;
};

extern MapSystemBase* MapSystemPtr;
void Init();
void MapInit();
void MapSetFadein(int32_t partId, int32_t partType);
void MapSetGroup(int32_t index, int32_t mappedId1, int32_t mappedId2, int32_t mappedId3);
void MapSetFadeout(int32_t partId, int32_t partType);
void MapSetDisp(int32_t partId, int32_t partType);
void MapSetHide(int32_t arg1, int32_t arg2);
bool MapPoolFadeEndChk_Wait();
void MapMoveAnimeInit(int32_t arg1, int32_t arg2, int32_t arg3);
bool MapMoveAnimeMain();
void MapGetPos(int32_t partId, int32_t partType, int32_t& getX, int32_t& getY);
void MapSetPool(int32_t index, int32_t id, int32_t type);
void MapResetPoolAll(int32_t arg1);
bool MapFadeEndChk_Wait();
void MapPoolShuffle(int32_t param_1);
void MapPoolSetDisp(int32_t arg1, int32_t arg2);
void MapPoolSetHide(int32_t arg1, int32_t arg2);
void MapPoolSetFadein(int32_t unused, int32_t poolIdx);
void MapPoolSetFadeout(int32_t unused, int32_t poolIdx);
bool MapPlayerPhotoSelect(int32_t unused);
void MapResetPool(int32_t poolIdx);
void MapSetGroupEx(int32_t index, int32_t type, int32_t mappedId);
void MapZoomInit(int32_t mapX, int32_t mapY, int32_t size);
bool MapZoomMain();
void MapZoomInit2(int32_t arg1, int32_t arg2);
bool MapZoomMain3();
bool MapZoomInit3(int32_t setMapX, int32_t setMapY, int32_t setMapSize, bool halfZoom = false);
bool MapMoveAnimeInit2(int32_t setMapX, int32_t setMapY, int32_t setTransitionSize);
bool MapMoveAnimeMain2();
void MapPlayerPotalSelectInit();
bool MapPlayerPotalSelect();
void MapSystem_28();
void Update(float dt);
void RenderButtonGuide();
void Render();

}  // namespace MapSystem
}  // namespace UI
}  // namespace Impacto