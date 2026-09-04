#include "shader.h"

#include <SDL3/SDL_iostream.h>

#include "../../log.h"

namespace Impacto::Bgfx {

ShaderProgram::ShaderProgram(ShaderProgram&& other) {
  *this = std::move(other);
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) {
  if (&other == this) return *this;
  Reset(true);
  if (!other.IsInit) return *this;

  IsInit = true;
  ProgramHandle = other.ProgramHandle;
  VertexShader = other.VertexShader;
  FragmentShader = other.FragmentShader;

  other.Reset(false);

  return *this;
}

ShaderProgram::~ShaderProgram() { Reset(true); }

std::optional<ShaderProgram> ShaderProgram::Create(
    const std::string_view vertexShaderName,
    const std::string_view fragmentShaderName) {
  const std::string_view pathPrefix = []() -> std::string_view {
    switch (bgfx::getRendererType()) {
      using enum bgfx::RendererType::Enum;
      case Direct3D11:
      case Direct3D12:
        return "dx11";
      case Metal:
        return "metal";
      case OpenGLES:
        return "essl";
      case OpenGL:
        return "glsl";
      case Vulkan:
        return "spirv";

      case Noop:
      case Agc:
      case Gnm:
      case Nvn:
      case Count:
        break;
    }
    return "";
  }();
  if (pathPrefix.empty()) {
    const std::string errorMsg =
        fmt::format("Unsupported renderer type for shaders: \"{:s}\"\n",
                    magic_enum::enum_name(bgfx::getRendererType()));
    ImpLog(LogLevel::Fatal, LogChannel::Render, "{:s}\n", errorMsg);
    throw errorMsg;
  }

  const std::string vertexShaderPath = fmt::format(
      "./shaders/bgfx/{:s}/{:s}.sc.bin", pathPrefix, vertexShaderName);
  const std::optional<const bgfx::ShaderHandle> vertexShader =
      CreateShader(vertexShaderPath.c_str());
  if (!vertexShader.has_value()) return std::nullopt;

  const std::string fragmentShaderPath = fmt::format(
      "./shaders/bgfx/{:s}/{:s}.sc.bin", pathPrefix, fragmentShaderName);
  const std::optional<const bgfx::ShaderHandle> fragmentShader =
      CreateShader(fragmentShaderPath.c_str());
  if (!fragmentShader.has_value()) return std::nullopt;

  ShaderProgram program;

  program.VertexShader = *vertexShader;
  program.FragmentShader = *fragmentShader;

  program.ProgramHandle =
      bgfx::createProgram(program.VertexShader, program.FragmentShader, true);
  assert(bgfx::isValid(program.ProgramHandle));

  program.IsInit = true;

  return program;
}

std::optional<bgfx::ShaderHandle> ShaderProgram::CreateShader(
    const char* const sourcePath) {
  size_t fileSize;
  void* const source = SDL_LoadFile(sourcePath, &fileSize);
  if (source == nullptr) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "Failed to read shader source file \"{:s}\": {:s}\n", sourcePath,
           SDL_GetError());
    return std::nullopt;
  }

  bgfx::ShaderHandle shader =
      bgfx::createShader(bgfx::copy(source, static_cast<uint32_t>(fileSize)));
  assert(bgfx::isValid(shader));

  SDL_free(source);

  return shader;
}

void ShaderProgram::Reset(bool cleanUpResources) {
  if (!IsInit) return;
  IsInit = false;

  if (cleanUpResources) {
    bgfx::destroy(ProgramHandle);
  }
}

}  // namespace Impacto::Bgfx
