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
void MapSetFadein(int32_t partId, int32_t partType) {
  return MapSystemPtr->MapSetFadein(partId, partType);
}
void MapSetGroup(int32_t index, int32_t mappedId1, int32_t mappedId2, int32_t mappedId3) {
  return MapSystemPtr->MapSetGroup(index, mappedId1, mappedId2, mappedId3);
}
void MapSetFadeout(int32_t partId, int32_t partType) {
  return MapSystemPtr->MapSetFadeout(partId, partType);
}
void MapSetDisp(int32_t partId, int32_t partType) {
  return MapSystemPtr->MapSetDisp(partId, partType);
}
void MapSetHide(int32_t arg1, int32_t arg2) {
  return MapSystemPtr->MapSetHide(arg1, arg2);
}
bool MapPoolFadeEndChk_Wait() { return MapSystemPtr->MapPoolFadeEndChk_Wait(); }
void MapMoveAnimeInit(int32_t arg1, int32_t arg2, int32_t arg3) {
  return MapSystemPtr->MapMoveAnimeInit(arg1, arg2, arg3);
}
bool MapMoveAnimeMain() { return MapSystemPtr->MapMoveAnimeMain(); }
void MapGetPos(int32_t partId, int32_t partType, int32_t& getX, int32_t& getY) {
  return MapSystemPtr->MapGetPos(partId, partType, getX, getY);
}
void MapSetPool(int32_t index, int32_t id, int32_t type) {
  return MapSystemPtr->MapSetPool(index, id, type);
}
void MapResetPoolAll(int32_t arg1) { return MapSystemPtr->MapResetPoolAll(arg1); }
bool MapFadeEndChk_Wait() { return MapSystemPtr->MapFadeEndChk_Wait(); }
void MapPoolShuffle(int32_t param_1) {
  return MapSystemPtr->MapPoolShuffle(param_1);
}
void MapPoolSetDisp(int32_t arg1, int32_t arg2) {
  return MapSystemPtr->MapPoolSetDisp(arg1, arg2);
}
void MapPoolSetHide(int32_t arg1, int32_t arg2) {
  return MapSystemPtr->MapPoolSetHide(arg1, arg2);
}
void MapPoolSetFadein(int32_t unused, int32_t poolIdx) {
  return MapSystemPtr->MapPoolSetFadein(unused, poolIdx);
}
void MapPoolSetFadeout(int32_t unused, int32_t poolIdx) {
  return MapSystemPtr->MapPoolSetFadeout(unused, poolIdx);
}
bool MapPlayerPhotoSelect(int32_t unused) {
  return MapSystemPtr->MapPlayerPhotoSelect(unused);
}
void MapResetPool(int32_t poolIdx) { return MapSystemPtr->MapResetPool(poolIdx); }
void MapSetGroupEx(int32_t index, int32_t type, int32_t mappedId) {
  return MapSystemPtr->MapSetGroupEx(index, type, mappedId);
}
void MapZoomInit(int32_t mapX, int32_t mapY, int32_t size) {
  return MapSystemPtr->MapZoomInit(mapX, mapY, size);
}
bool MapZoomMain() { return MapSystemPtr->MapZoomMain(); }
void MapZoomInit2(int32_t arg1, int32_t arg2) {
  return MapSystemPtr->MapZoomInit2(arg1, arg2);
}
bool MapZoomMain3() { return MapSystemPtr->MapZoomMain3(); }
bool MapZoomInit3(int32_t setMapX, int32_t setMapY, int32_t setMapSize, bool halfZoom) {
  return MapSystemPtr->MapZoomInit3(setMapX, setMapY, setMapSize, halfZoom);
}
bool MapMoveAnimeInit2(int32_t setMapX, int32_t setMapY, int32_t setTransitionSize) {
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