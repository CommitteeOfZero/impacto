#pragma once

namespace Impacto {
namespace UI {
namespace MapSystem {

class MapSystemBase {
 public:
  virtual void MapInit() = 0;
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;
  virtual void MapSetFadein(int arg1, int arg2) = 0;
  virtual void MapSetPool(int index, int id, int type) = 0;
  virtual void MapSetDisp(int id, int type) = 0;
  virtual void MapResetPool(int index) = 0;
  virtual void MapResetPoolAll(int index) = 0;
  virtual void MapPoolSetDisp(int arg1, int arg2) = 0;
  virtual void MapPoolSetFadein(int arg1, int arg2) = 0;
  virtual void MapPoolSetFadeout(int arg1, int arg2) = 0;
  virtual bool MapPoolFadeEndChk_Wait() = 0;
  virtual bool MapPlayerPhotoSelect(int arg1) = 0;
  virtual void MapPoolShuffle(int arg1) = 0;
};

extern MapSystemBase* MapSystemPtr;

void Init();
void MapInit();
void MapSetFadeIn(int arg1, int arg2);
void MapSetPool(int index, int id, int type);
void MapSetDisp(int id, int type);
void MapResetPool(int index);
void MapResetPoolAll(int index);
void MapPoolSetDisp(int arg1, int arg2);
void MapPoolSetFadein(int arg1, int arg2);
void MapPoolSetFadeout(int arg1, int arg2);
bool MapPoolFadeEndChk_Wait();
bool MapPlayerPhotoSelect(int arg1);
void MapPoolShuffle(int arg1);
void Render();
void Update(float dt);

}  // namespace MapSystem
}  // namespace UI
}  // namespace Impacto