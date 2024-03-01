#include "../profile/ui/mapsystem.h"

namespace Impacto {
namespace UI {
namespace MapSystem {

MapSystemBase* MapSystemPtr = 0;

void Init() {
  Profile::MapSystem::Configure();
  Profile::MapSystem::CreateInstance();
}
void MapInit() { return MapSystemPtr->MapInit(); }
void MapSetFadein(int partId, int partType) {
  return MapSystemPtr->MapSetFadein(partId, partType);
}
void MapSetGroup(int index, int mappedId1, int mappedId2, int mappedId3) {
  return MapSystemPtr->MapSetGroup(index, mappedId1, mappedId2, mappedId3);
}
void MapSetFadeout(int partId, int partType) {
  return MapSystemPtr->MapSetFadeout(partId, partType);
}
void MapSetDisp(int partId, int partType) {
  return MapSystemPtr->MapSetDisp(partId, partType);
}
void MapSetHide(int arg1, int arg2) {
  return MapSystemPtr->MapSetHide(arg1, arg2);
}
bool MapPoolFadeEndChk_Wait() { return MapSystemPtr->MapPoolFadeEndChk_Wait(); }
void MapMoveAnimeInit(int arg1, int arg2, int arg3) {
  return MapSystemPtr->MapMoveAnimeInit(arg1, arg2, arg3);
}
bool MapMoveAnimeMain() { return MapSystemPtr->MapMoveAnimeMain(); }
void MapGetPos(int partId, int partType, int& getX, int& getY) {
  return MapSystemPtr->MapGetPos(partId, partType, getX, getY);
}
void MapSetPool(int index, int id, int type) {
  return MapSystemPtr->MapSetPool(index, id, type);
}
void MapResetPoolAll(int arg1) { return MapSystemPtr->MapResetPoolAll(arg1); }
bool MapFadeEndChk_Wait() { return MapSystemPtr->MapFadeEndChk_Wait(); }
void MapPoolShuffle(int param_1) {
  return MapSystemPtr->MapPoolShuffle(param_1);
}
void MapPoolSetDisp(int arg1, int arg2) {
  return MapSystemPtr->MapPoolSetDisp(arg1, arg2);
}
void MapPoolSetHide(int arg1, int arg2) {
  return MapSystemPtr->MapPoolSetHide(arg1, arg2);
}
void MapPoolSetFadein(int unused, int poolIdx) {
  return MapSystemPtr->MapPoolSetFadein(unused, poolIdx);
}
void MapPoolSetFadeout(int unused, int poolIdx) {
  return MapSystemPtr->MapPoolSetFadeout(unused, poolIdx);
}
bool MapPlayerPhotoSelect(int unused) {
  return MapSystemPtr->MapPlayerPhotoSelect(unused);
}
void MapResetPool(int poolIdx) { return MapSystemPtr->MapResetPool(poolIdx); }
void MapSetGroupEx(int index, int type, int mappedId) {
  return MapSystemPtr->MapSetGroupEx(index, type, mappedId);
}
void MapZoomInit(int mapX, int mapY, int size) {
  return MapSystemPtr->MapZoomInit(mapX, mapY, size);
}
bool MapZoomMain() { return MapSystemPtr->MapZoomMain(); }
void MapZoomInit2(int arg1, int arg2) {
  return MapSystemPtr->MapZoomInit2(arg1, arg2);
}
bool MapZoomMain3() { return MapSystemPtr->MapZoomMain3(); }
bool MapZoomInit3(int setMapX, int setMapY, int setMapSize, bool halfZoom) {
  return MapSystemPtr->MapZoomInit3(setMapX, setMapY, setMapSize, halfZoom);
}
bool MapMoveAnimeInit2(int setMapX, int setMapY, int setTransitionSize) {
  return MapSystemPtr->MapMoveAnimeInit2(setMapX, setMapY, setTransitionSize);
}
bool MapMoveAnimeMain2() { return MapSystemPtr->MapMoveAnimeMain2(); }
void MapPlayerPotalSelectInit() {
  return MapSystemPtr->MapPlayerPotalSelectInit();
}
bool MapPlayerPotalSelect() { return MapSystemPtr->MapPlayerPotalSelect(); }
void MapSystem_28() { return MapSystemPtr->MapSystem_28(); }
void Update(float dt) { return MapSystemPtr->Update(dt); }
void RenderButtonGuide() { return MapSystemPtr->RenderButtonGuide(); }
void Render() { return MapSystemPtr->Render(); }

}  // namespace MapSystem
}  // namespace UI
}  // namespace Impacto