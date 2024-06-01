#pragma once
#include <cstdint>
namespace Impacto {
namespace UI {
namespace MapSystem {

class MapSystemBase {
 public:
  virtual void MapInit() = 0;
  virtual void MapLoad(uint8_t* data, int& dataSize) = 0;
  virtual void MapSetFadein(int partId, int partType) = 0;
  virtual void MapSetGroup(int index, int mappedId1, int mappedId2,
                           int mappedId3) = 0;
  virtual void MapSetFadeout(int partId, int partType) = 0;
  virtual void MapSetDisp(int partId, int partType) = 0;
  virtual void MapSetHide(int arg1, int arg2) = 0;
  virtual bool MapPoolFadeEndChk_Wait() = 0;
  virtual void MapMoveAnimeInit(int arg1, int arg2, int arg3) = 0;
  virtual bool MapMoveAnimeMain() = 0;
  virtual void MapGetPos(int partId, int partType, int& getX, int& getY) = 0;
  virtual void MapSetPool(int index, int id, int type) = 0;
  virtual void MapResetPoolAll(int arg1) = 0;
  virtual bool MapFadeEndChk_Wait() = 0;
  virtual void MapPoolShuffle(int param_1) = 0;
  virtual void MapPoolSetDisp(int arg1, int arg2) = 0;
  virtual void MapPoolSetHide(int arg1, int arg2) = 0;
  virtual void MapPoolSetFadein(int unused, int poolIdx) = 0;
  virtual void MapPoolSetFadeout(int unused, int poolIdx) = 0;
  virtual bool MapPlayerPhotoSelect(int unused) = 0;
  virtual void MapResetPool(int poolIdx) = 0;
  virtual void MapSetGroupEx(int index, int type, int mappedId) = 0;
  virtual void MapZoomInit(int mapX, int mapY, int size) = 0;
  virtual bool MapZoomMain() = 0;
  virtual void MapZoomInit2(int arg1, int arg2) = 0;
  virtual bool MapZoomMain3() = 0;
  virtual bool MapZoomInit3(int setMapX, int setMapY, int setMapSize,
                            bool halfZoom = false) = 0;
  virtual bool MapMoveAnimeInit2(int setMapX, int setMapY,
                                 int setTransitionSize) = 0;
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
void MapLoad(uint8_t* data, int& dataSize);
void MapInit();
void MapSetFadein(int partId, int partType);
void MapSetGroup(int index, int mappedId1, int mappedId2, int mappedId3);
void MapSetFadeout(int partId, int partType);
void MapSetDisp(int partId, int partType);
void MapSetHide(int arg1, int arg2);
bool MapPoolFadeEndChk_Wait();
void MapMoveAnimeInit(int arg1, int arg2, int arg3);
bool MapMoveAnimeMain();
void MapGetPos(int partId, int partType, int& getX, int& getY);
void MapSetPool(int index, int id, int type);
void MapResetPoolAll(int arg1);
bool MapFadeEndChk_Wait();
void MapPoolShuffle(int param_1);
void MapPoolSetDisp(int arg1, int arg2);
void MapPoolSetHide(int arg1, int arg2);
void MapPoolSetFadein(int unused, int poolIdx);
void MapPoolSetFadeout(int unused, int poolIdx);
bool MapPlayerPhotoSelect(int unused);
void MapResetPool(int poolIdx);
void MapSetGroupEx(int index, int type, int mappedId);
void MapZoomInit(int mapX, int mapY, int size);
bool MapZoomMain();
void MapZoomInit2(int arg1, int arg2);
bool MapZoomMain3();
bool MapZoomInit3(int setMapX, int setMapY, int setMapSize, bool halfZoom = false);
bool MapMoveAnimeInit2(int setMapX, int setMapY, int setTransitionSize);
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