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
  MapBgSprite = EnsureGetMemberSprite("MapBackground");
  int MapPartsPhotosNum = EnsureGetMemberInt("MapPartsPhotosNum");
  GetMemberSpriteArray(MapPartsPhotoSprites, MapPartsPhotosNum,
                       "MapPartsPhotoSprites");
  SelectedMapPoolTagSprite = EnsureGetMemberSprite("SelectedMapPoolTag");
  int MapPartsArticlesNum = EnsureGetMemberInt("MapPartsArticlesNum");
  GetMemberSpriteArray(MapPartsArticleSprites, MapPartsArticlesNum,
                       "MapPartsArticleSprites");
  int MapPartsPinsNum = EnsureGetMemberInt("MapPartsPinsNum");
  GetMemberSpriteArray(MapPartsPinSprites, MapPartsPinsNum,
                       "MapPartsPinSprites");
  MapButtonGuideSprite = EnsureGetMemberSprite("MapButtonGuide");
  int MapPartsTagsNum = EnsureGetMemberInt("MapPartsTagsNum");
  GetMemberSpriteArray(MapPartsTagSprites, MapPartsTagsNum,
                       "MapPartsTagSprites");
  FadeAnimationDuration = EnsureGetMemberFloat("FadeAnimationDuration");
  MapLine = EnsureGetMemberSprite("MapLine");
  MapLineRed = EnsureGetMemberSprite("MapLineRed");
}

}  // namespace MapSystem
}  // namespace CCLCC
}  // namespace Profile
}  // namespace Impacto