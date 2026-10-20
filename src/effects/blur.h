#include "../spritesheet.h"

namespace Impacto {
namespace Effects {

class BlurEffect {
 public:
  void Render(int iterations);

 private:
  MutableTextureRef BlurSprite;
};

inline BlurEffect Blur;

}  // namespace Effects
}  // namespace Impacto