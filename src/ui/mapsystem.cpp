#include "../profile/ui/mapsystem.h"

namespace Impacto {
namespace UI {
namespace MapSystem {

MapSystemBase* MapSystemPtr = 0;

void Init() {
  Profile::MapSystem::Configure();
  Profile::MapSystem::CreateInstance();
}

void Render() {
  if (MapSystemPtr) MapSystemPtr->Render();
}
void MapInit() {
  if (MapSystemPtr) MapSystemPtr->MapInit();
}
void MapSetFadeIn(int arg1, int arg2) {
  if (MapSystemPtr) MapSystemPtr->MapSetFadein(arg1, arg2);
}

void MapSetPool(int index, int id, int type) {
  if (MapSystemPtr) MapSystemPtr->MapSetPool(index, id, type);
}

void MapSetDisp(int arg1, int arg2) {
  if (MapSystemPtr) MapSystemPtr->MapSetDisp(arg1, arg2);
}

void MapResetPool(int index) {
  if (MapSystemPtr) MapSystemPtr->MapResetPool(index);
}

void MapResetPoolAll(int index) {
  if (MapSystemPtr) MapSystemPtr->MapResetPoolAll(index);
}

void MapPoolSetDisp(int arg1, int arg2) {
  if (MapSystemPtr) MapSystemPtr->MapPoolSetDisp(arg1, arg2);
}

void MapPoolSetFadein(int arg1, int arg2) {
  if (MapSystemPtr) MapSystemPtr->MapPoolSetFadein(arg1, arg2);
}

void MapPoolSetFadeout(int arg1, int arg2) {
  if (MapSystemPtr) MapSystemPtr->MapPoolSetFadeout(arg1, arg2);
}

bool MapPoolFadeEndChk_Wait() {
  if (MapSystemPtr) return MapSystemPtr->MapPoolFadeEndChk_Wait();
}

bool MapPlayerPhotoSelect(int arg1) {
  if (MapSystemPtr) return MapSystemPtr->MapPlayerPhotoSelect(arg1);
}

void MapPoolShuffle(int arg1) {
  if (MapSystemPtr) return MapSystemPtr->MapPoolShuffle(arg1);
}

void Update(float dt) {
  if (MapSystemPtr) MapSystemPtr->Update(dt);
}

}  // namespace MapSystem
}  // namespace UI
}  // namespace Impacto