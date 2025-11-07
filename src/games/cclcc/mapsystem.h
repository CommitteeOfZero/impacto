#pragma once
#include "../../animation.h"
#include "../../ui/widgets/button.h"
#include <array>

namespace Impacto {
namespace UI {
namespace CCLCC {
class MapSystem {
  enum DisplayState { Hidden, Hiding, Showing, Shown };

 public:
  void MapInit();
  int MapLoad(uint8_t* data);
  int MapSave(uint8_t* data);
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
  bool MapZoomInit3(int setMapX, int setMapY, int setMapSize,
                    bool halfZoom = false);
  bool MapMoveAnimeInit2(int setMapX, int setMapY, int setTransitionSize);
  bool MapMoveAnimeMain2();
  void MapPlayerPotalSelectInit();
  bool MapPlayerPotalSelect();
  void MapSystem_28();
  void Update(float dt);
  void Render();

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
    Animation fadeAnim;
    DisplayState state;
    int delay;
    union {
      int angle;
      int pinId;
    };
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

  static MapSystem& GetInstance() {
    static MapSystem impl;
    return impl;
  }

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

  std::array<MapSystem::MapPoolStruct, 20> MapPool = {};
  std::array<MapSystem::MapPoolDispStruct, 40> MapPoolDisp = {};

  std::array<MapSystem::MapPartsDispStruct, 40> MapPartsDisp = {};
  std::array<MapSystem::MapGroupStruct, 40> MapGroup = {};
  int MapPartsMax = 0;

  std::array<MapSystem::MapPositionTransitions, 4> MapPosTransitions;
  std::array<int, 13> MapZoomCtAcc = {};
  std::array<int, 20> MapPoolCurCt = {};

  int MapZoomMode = 0;
  int MapZoomCt = 0;
  int MapZoomCtMax = 0;

  int SelectedMapPoolIdx = 0xff;
  int HoverMapPoolIdx = 0xff;
  float MapBGWidth;
  float MapBGHeight;
  float MapPosX;
  float MapPosY;
  int MapMoveMode = 0;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto