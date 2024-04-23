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
  void MapSetFadein(int32_t partId, int32_t partType) override;
  void MapSetGroup(int32_t index, int32_t mappedId1, int32_t mappedId2,
                   int32_t mappedId3) override;
  void MapSetFadeout(int32_t partId, int32_t partType) override;
  void MapSetDisp(int32_t partId, int32_t partType) override;
  void MapSetHide(int32_t arg1, int32_t arg2) override;
  bool MapPoolFadeEndChk_Wait() override;
  void MapMoveAnimeInit(int32_t arg1, int32_t arg2, int32_t arg3) override;
  bool MapMoveAnimeMain() override;
  void MapGetPos(int32_t partId, int32_t partType, int32_t& getX, int32_t& getY) override;
  void MapSetPool(int32_t index, int32_t id, int32_t type) override;
  void MapResetPoolAll(int32_t arg1) override;
  bool MapFadeEndChk_Wait() override;
  void MapPoolShuffle(int32_t param_1) override;
  void MapPoolSetDisp(int32_t arg1, int32_t arg2) override;
  void MapPoolSetHide(int32_t arg1, int32_t arg2) override;
  void MapPoolSetFadein(int32_t unused, int32_t poolIdx) override;
  void MapPoolSetFadeout(int32_t unused, int32_t poolIdx) override;
  bool MapPlayerPhotoSelect(int32_t unused) override;
  void MapResetPool(int32_t poolIdx) override;
  void MapSetGroupEx(int32_t index, int32_t type, int32_t mappedId) override;
  void MapZoomInit(int32_t mapX, int32_t mapY, int32_t size) override;
  bool MapZoomMain() override;
  void MapZoomInit2(int32_t arg1, int32_t arg2) override;
  bool MapZoomMain3() override;
  bool MapZoomInit3(int32_t setMapX, int32_t setMapY, int32_t setMapSize,
                    bool halfZoom = false) override;
  bool MapMoveAnimeInit2(int32_t setMapX, int32_t setMapY,
                         int32_t setTransitionSize) override;
  bool MapMoveAnimeMain2() override;
  void MapPlayerPotalSelectInit() override;
  bool MapPlayerPotalSelect() override;
  void MapSystem_28() override;
  void Update(float dt) override;
  void RenderButtonGuide() override;
  void Render() override;

  struct MapPoolStruct {
    int32_t id;
    int32_t type;
    Widgets::Button button;
  };

  struct MapGroupStruct {
    int32_t groupId1;
    int32_t groupId2;
    int32_t groupId3;
  };

  struct MapPoolDispStruct {
    union {
      int32_t angle;
      int32_t pinId;
    };
    Animation fadeAnim;
    DisplayState state;
    int32_t delay;
  };

  struct MapPartsDispStruct {
    int32_t partId;
    int32_t type;
    Animation fadeAnim;
    DisplayState state;
    int32_t delay;
    int32_t angle;
    int32_t dist;
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
  void MapDispPhoto(int32_t id, int32_t arg2);
  void MapDispArticle(int32_t id);
  void MapDispTag(int32_t id);
  void MapDispLine(int32_t id, int32_t type);
  void MapDispPin(int32_t id);
  void MapPartsSort();

  void MapPoolDispPhoto(int32_t poolId);
  void MapPoolDispPin(int32_t poolId);

  void HandlePoolUpDownNav(int32_t maxPoolRow, int32_t poolType, bool isUp);
  void HandlePoolLeftRightNav(int32_t maxPoolRow, int32_t poolType, bool isLeft);

  void MapSetFadein_PhotoArticle(int32_t arg1, int32_t arg2);
  void MapSetFadein_Line(int32_t arg1, int32_t arg2);

  std::array<MapSystemCCLCC::MapPoolStruct, 20> MapPool = {};
  std::array<MapSystemCCLCC::MapPoolDispStruct, 40> MapPoolDisp = {};

  std::array<MapSystemCCLCC::MapPartsDispStruct, 40> MapPartsDisp = {};
  std::array<MapSystemCCLCC::MapGroupStruct, 40> MapGroup = {};
  int32_t MapPartsMax = 0;

  std::array<MapSystemCCLCC::MapPositionTransitions, 4> MapPosTransitions;
  std::array<int, 13> MapZoomCtAcc = {};
  std::array<int, 20> MapPoolCurCt = {};

  int32_t MapZoomMode = 0;
  int32_t MapZoomCt = 0;
  int32_t MapZoomCtMax = 0;

  int32_t selectedMapPoolIdx = 0xff;
  int32_t hoverMapPoolIdx = 0xff;
  float MapBGWidth;
  float MapBGHeight;
  float MapPosX;
  float MapPosY;
  int32_t MapMoveMode = 0;
  bool PhotoSelClick = false;
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto