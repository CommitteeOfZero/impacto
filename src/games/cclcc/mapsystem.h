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
  void MapSetFadein(int partId, int partType) override;
  void MapSetGroup(int index, int mappedId1, int mappedId2,
                   int mappedId3) override;
  void MapSetFadeout(int partId, int partType) override;
  void MapSetDisp(int partId, int partType) override;
  void MapSetHide(int arg1, int arg2) override;
  bool MapPoolFadeEndChk_Wait() override;
  void MapMoveAnimeInit(int arg1, int arg2, int arg3) override;
  bool MapMoveAnimeMain() override;
  void MapGetPos(int partId, int partType, int& getX, int& getY) override;
  void MapSetPool(int index, int id, int type) override;
  void MapResetPoolAll(int arg1) override;
  bool MapFadeEndChk_Wait() override;
  void MapPoolShuffle(int param_1) override;
  void MapPoolSetDisp(int arg1, int arg2) override;
  void MapPoolSetHide(int arg1, int arg2) override;
  void MapPoolSetFadein(int unused, int poolIdx) override;
  void MapPoolSetFadeout(int unused, int poolIdx) override;
  bool MapPlayerPhotoSelect(int unused) override;
  void MapResetPool(int poolIdx) override;
  void MapSetGroupEx(int index, int type, int mappedId) override;
  void MapZoomInit(int mapX, int mapY, int size) override;
  bool MapZoomMain() override;
  void MapZoomInit2(int arg1, int arg2) override;
  bool MapZoomMain3() override;
  bool MapZoomInit3(int setMapX, int setMapY, int setMapSize,
                    bool halfZoom = false) override;
  bool MapMoveAnimeInit2(int setMapX, int setMapY,
                         int setTransitionSize) override;
  bool MapMoveAnimeMain2() override;
  void MapPlayerPotalSelectInit() override;
  bool MapPlayerPotalSelect() override;
  void MapSystem_28() override;
  void Update(float dt) override;
  void RenderButtonGuide() override;
  void Render() override;

  struct MapPoolStruct {
    int id;
    int type;
    Widgets::Button button;
  };

  struct MapGroupStruct {
    int groupId1;
    int groupId2;
    int groupId3;
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
  enum MapPosEnum { MapZoom, MapZoom3, MapMove2, MapPosTransitionsMax };
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

  void MapSetFadein_PhotoArticle(int arg1, int arg2);
  void MapSetFadein_Line(int arg1, int arg2);

  std::array<MapSystemCCLCC::MapPoolStruct, 20> MapPool = {};
  std::array<MapSystemCCLCC::MapPoolDispStruct, 40> MapPoolDisp = {};
  int MapPoolCnt = 0;

  std::array<MapSystemCCLCC::MapPartsDispStruct, 40> MapPartsDisp = {};
  std::array<MapSystemCCLCC::MapGroupStruct, 40> MapGroup = {};
  int MapPartsMax = 0;

  std::array<MapSystemCCLCC::MapPositionTransitions, 4> MapPosTransitions;
  std::array<int, 13> MapZoomCtAcc = {};
  std::array<int, 20> MapPoolCurCt = {};

  int MapZoomMode = 0;
  int MapZoomCt = 0;
  int MapZoomCtMax = 0;

  int selectedMapPoolIdx = 0xff;
  int hoverMapPoolIdx = 0xff;
  float MapBGWidth;
  float MapBGHeight;
  float MapPosX;
  float MapPosY;
  int MapMoveMode = 0;
  bool PhotoSelClick = false;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto