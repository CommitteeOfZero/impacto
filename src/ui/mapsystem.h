#pragma once

namespace Impacto {
namespace UI {
namespace MapSystem {

class MapSystemBase {
 public:
  virtual void MapInit() = 0;
  virtual void Update(float dt) = 0;
  virtual void Render() = 0;
  virtual void RenderButtonGuide() = 0;
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
  virtual void MapZoomInit(int arg1, int arg2, int arg3) = 0;
  virtual bool MapZoomMain() = 0;
  virtual bool MapZoomInit3(int arg1, int arg2, int arg3, bool ex = false) = 0;
  virtual bool MapZoomMain3() = 0;
  virtual bool MapMoveAnimeInit2(int arg1, int arg2, int arg3) = 0;
  virtual bool MapMoveAnimeMain2() = 0;
  virtual void MapGetPos(int arg1, int arg2, int &arg3, int &arg4) = 0;
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
void MapZoomInit(int arg1, int arg2, int arg3);
bool MapZoomMain();
bool MapZoomInit3(int arg1, int arg2, int arg3);
bool MapZoomMain3();
bool MapMoveAnimeInit2(int arg1, int arg2, int arg3);
bool MapMoveAnimeMain2();
void MapGetPos(int arg1, int arg2, int &arg3, int &arg4);
void Render();
void RenderButtonGuide();
void Update(float dt);

}  // namespace MapSystem
}  // namespace UI
}  // namespace Impacto