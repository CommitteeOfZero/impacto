#pragma once

#include "../nv12frame.h"

namespace Impacto {
namespace OpenGL {

class GLNV12Frame : public NV12Frame {
 public:
  void Init(float width, float height) override;

  void Submit(const void* luma, int lumaStride, const void* cbcr,
              int cbcrStride) override;
  void Release() override;
};

}  // namespace OpenGL
}  // namespace Impacto