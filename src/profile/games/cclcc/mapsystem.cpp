#include "mapsystem.h"
#include "../../profile_internal.h"

#include "../../../game.h"
#include "../../../ui/ui.h"
#include "../../../games/cclcc/mapsystem.h"

namespace Impacto {
namespace Profile {
namespace CCLCC {
namespace MapSystem {

void Configure() {
  if (!TryPushMember("MapSystem")) return;
  AssertIs(LUA_TTABLE);
  MapBgSprite = EnsureGetMember<Sprite>("MapBackground");
  int MapPartsPhotosNum = EnsureGetMember<int>("MapPartsPhotosNum");
  GetMemberArray<Sprite>(MapPartsPhotoSprites, MapPartsPhotosNum,
                         "MapPartsPhotoSprites");
  SelectedMapPoolTagSprite = EnsureGetMember<Sprite>("SelectedMapPoolTag");
  int MapPartsArticlesNum = EnsureGetMember<int>("MapPartsArticlesNum");
  GetMemberArray<Sprite>(MapPartsArticleSprites, MapPartsArticlesNum,
                         "MapPartsArticleSprites");
  int MapPartsPinsNum = EnsureGetMember<int>("MapPartsPinsNum");
  GetMemberArray<Sprite>(MapPartsPinSprites, MapPartsPinsNum,
                         "MapPartsPinSprites");
  int MapPartsTagsNum = EnsureGetMember<int>("MapPartsTagsNum");
  GetMemberArray<Sprite>(MapPartsTagSprites, MapPartsTagsNum,
                         "MapPartsTagSprites");
  FadeAnimationDuration = EnsureGetMember<float>("FadeAnimationDuration");
  MapLine = EnsureGetMember<Sprite>("MapLine");
  MapLineRed = EnsureGetMember<Sprite>("MapLineRed");

  Pop();
}

}  // namespace MapSystem
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto