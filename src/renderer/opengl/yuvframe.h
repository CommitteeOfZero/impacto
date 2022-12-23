#pragma once

#include "../yuvframe.h"

namespace Impacto {
namespace OpenGL {

class GLYUVFrame : public YUVFrame {
 public:
  void Init(int width, int height) override;

  void Submit(void* luma, void* cb, void* cr) override;
  void Release() override;
};

}  // namespace OpenGL
}  // namespace Impacto