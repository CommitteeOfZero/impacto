#pragma once

#include <bgfx/bgfx.h>

#include <cassert>
#include <string_view>
#include <optional>

namespace Impacto::Bgfx {

class ShaderProgram {
 public:
  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&&);
  ~ShaderProgram();

  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&&);

  [[nodiscard]] static std::optional<ShaderProgram> Create(
      const std::string_view vertexShaderName,
      const std::string_view fragmentShaderName);

  operator bgfx::ProgramHandle() { return GetProgramHandle(); }

  [[nodiscard]] bgfx::ProgramHandle GetProgramHandle() {
    assert(IsInit && bgfx::isValid(ProgramHandle));
    return ProgramHandle;
  }

 private:
  ShaderProgram() = default;

  bool IsInit = false;

  bgfx::ProgramHandle ProgramHandle;

  bgfx::ShaderHandle VertexShader;
  bgfx::ShaderHandle FragmentShader;

  void Reset(bool cleanUpResources);

  [[nodiscard]] static std::optional<bgfx::ShaderHandle> CreateShader(
      const char* sourcePath);
};

}  // namespace Impacto::Bgfx
