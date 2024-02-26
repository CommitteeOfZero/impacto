#pragma once
#include "../../ui/mapsystem.h"
#include "../../animation.h"
#include "../../ui/widgets/button.h"
#include <array>

namespace Impacto {
namespace UI {
namespace CCLCC {
class MapSystemCCLCC : public Impacto::UI::MapSystem::MapSystemBase {
  enum DisplayState { Hidden, Hiding, Showing, Shown };

 public:
  void MapInit() override;
  void MapSetFadein(int arg1, int arg2) override;
  void MapSetGroup(int arg1, int arg2, int arg3, int arg4) override;
  void MapSetFadeout(int arg1, int arg2) override;
  void MapSetDisp(int arg1, int arg2) override;
  void MapSetHide(int arg1, int arg2);
  bool MapFadeEndChk_Wait();
  void MapMoveAnimeInit(int arg1, int arg2, int arg3);
  void MapMoveAnimeMain();
  void MapGetPos(int arg1, int arg2, int &arg3, int &arg4) override;
  void MapSetPool(int arg1, int arg2, int arg3) override;
  void MapResetPoolAll(int arg1) override;
  bool MapPoolFadeEndChk_Wait();
  void MapPoolShuffle(int arg1) override;
  void MapPoolSetDisp(int arg1, int arg2) override;
  void MapPoolSetHide(int arg1, int arg2);
  void MapPoolSetFadein(int arg1, int arg2) override;
  void MapPoolSetFadeout(int arg1, int arg2) override;
  bool MapPlayerPhotoSelect(int arg1) override;
  void MapResetPool(int arg1) override;
  void MapSetGroupEx(int arg1, int arg2, int arg3) override;
  void MapZoomInit(int arg1, int arg2, int arg3) override;
  bool MapZoomMain() override;
  void MapZoomInit2(int arg1, int arg2);
  bool MapZoomMain3() override;
  bool MapZoomInit3(int arg1, int arg2, int arg3, bool ex = false) override;
  bool MapMoveAnimeInit2(int arg1, int arg2, int arg3) override;
  bool MapMoveAnimeMain2() override;
  void MapPlayerPotalSelectInit();
  bool MapPlayerPotalSelect();
  void MapSystem_28();

  void Update(float dt) override;
  void Render() override;
  void RenderButtonGuide() override;

  struct MapPoolStruct {
    int id;
    int type;
    Widgets::Button button;
  };

  struct MapGroupStruct {
    int a;
    int b;
    int c;
  };

  struct MapPoolDispStruct {
    union {
      int angle;
      int pinId;
    };
    Animation fadeAnim;
    DisplayState state;
    int delay;
  };

  struct MapPartsDispStruct {
    int partId;
    int type;
    Animation fadeAnim;
    DisplayState state;
    int progress;
    int delay;
    int angle;
    int dist;
  };

  struct MapPositionState {
    float x;
    float y;
    float size;
  };

  struct MapPositionTransitions {
    MapPositionState Start;
    MapPositionState Current;
    MapPositionState End;
  };

 private:
  void MapFadeMain(float dt);
  void MapSetPos(float dt);
  void MapDispPhoto(int id, int arg2);
  void MapDispArticle(int id);
  void MapDispTag(int id);
  void MapDispLine(int id, int type);
  void MapDispPin(int id);
  void MapPartsSort();

  void MapPoolDispPhoto(int poolId);
  void MapPoolDispPin(int poolId);

  void HandlePoolUpDownNav(int maxPoolRow, int poolType, bool isUp);
  void HandlePoolLeftRightNav(int maxPoolRow, int poolType, bool isLeft);

  std::array<MapSystemCCLCC::MapPoolStruct, 20> MapPool = {};
  std::array<MapSystemCCLCC::MapPoolDispStruct, 40> MapPoolDisp = {};
  std::array<MapSystemCCLCC::MapPartsDispStruct, 40> MapPartsDisp = {};
  std::array<MapSystemCCLCC::MapGroupStruct, 40> MapGroup = {};

  std::array<int, 13> MapZoomCtAcc = {};

  std::array<int, 20> MapPoolCurCt = {};
  int MapPartsMax = 0;
  int MapPoolCnt = 0;
  int selectedMapPoolIdx = 0xff;
  int hoverMapPoolIdx = 0xff;
  float MapBGWidth;
  float MapBGHeight;
  float mapPosX;
  float mapPosY;
  int MapMoveMode = 0;
  bool PhotoSelClick = false;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto