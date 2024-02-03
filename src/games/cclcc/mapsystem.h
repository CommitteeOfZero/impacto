#pragma once
#include "../../ui/mapsystem.h"

namespace Impacto {
namespace UI {
namespace CCLCC {
class MapSystemCCLCC : public Impacto::UI::MapSystem::MapSystemBase {
 public:
  enum class MapBGState { Hidden, Showing, Shown, Hiding };
  void MapInit() override;
  void MapSetFadein(int arg1, int arg2) override;
  void MapSetGroup(int arg1, int arg2, int arg3, int arg4);
  void MapSetFadeout(int arg1, int arg2);
  void MapSetDisp(int arg1, int arg2) override;
  void MapSetHide(int arg1, int arg2);
  bool MapFadeEndChk_Wait();
  void MapMoveAnimeInit(int arg1, int arg2, int arg3);
  void MapMoveAnimeMain();
  void MapGetPos(int arg1, int arg2);
  void MapSetPool(int arg1, int arg2, int arg3) override;
  void MapResetPoolAll(int arg1);
  bool MapPoolFadeEndChk_Wait();
  void MapPoolShuffle(int arg1);
  void MapPoolSetDisp(int arg1, int arg2);
  void MapPoolSetHide(int arg1, int arg2);
  void MapPoolSetFadein(int arg1, int arg2);
  void MapPoolSetFadeout(int arg1, int arg2);
  bool MapPlayerPhotoSelect(int arg1) override;
  void MapResetPool(int arg1) override;
  void MapSetGroupEx(int arg1, int arg2, int arg3);
  void MapZoomInit(int arg1, int arg2, int arg3);
  void MapZoomMain();
  void MapZoomInit2(int arg1, int arg2);
  void MapZoomMain3();
  void MapZoomInit3(int arg1, int arg2, int arg3);
  void MapMoveAnimeInit2(int arg1, int arg2, int arg3);
  void MapPlayerPotalSelectInit();
  bool MapPlayerPotalSelect();
  void MapSystem_28();

  void Update(float dt) override;
  void Render() override;

  struct MapPoolMember {
    int id;
    int type;
  };

  struct MapPoolDispStruct {
    struct {
      int shown;
      int inOrOut;
      int progress;
    } info;
    int animState;
    union {
      int angle;
      int pinId;
    };
  };

  struct MapPartsDispStruct {
    int partId;
    int type;
    int shown;
    int inOrOut;
    int progress;
    int animState;
    int angle;
    int dist;
  };
  MapBGState mapBgState = MapBGState::Hidden;

 private:
  void MapFadeMain();
  void MapSetPos();
};

}  // namespace CCLCC
}  // namespace UI
}  // namespace Impacto