#include "renderer.h"

#include "../profile/game.h"
#include "../log.h"

#ifndef IMPACTO_DISABLE_OPENGL
#include "opengl/renderer.h"
#endif
#ifndef IMPACTO_DISABLE_VULKAN
#include "vulkan/renderer.h"
#endif
#ifndef IMPACTO_DISABLE_DX9
#include "dx9/renderer.h"
#endif

#include <numeric>

namespace Impacto {

void InitRenderer() {
  switch (Profile::Game::ActiveRenderer) {
#ifndef IMPACTO_DISABLE_OPENGL
    case RendererType::OpenGL:
      Renderer = new OpenGL::Renderer();
      break;
#endif
#ifndef IMPACTO_DISABLE_VULKAN
    case RendererType::Vulkan:
      Renderer = new Vulkan::Renderer();
      break;
#endif
#ifndef IMPACTO_DISABLE_DX9
    case RendererType::DirectX9:
      Renderer = new DirectX9::Renderer();
      break;
#endif
    default:
      ImpLog(LogLevel::Error, LogChannel::Render,
             "Unknown or unsupported renderer selected!\n");
      exit(1);
  }

  Renderer->Init();
}

void BaseRenderer::DrawCCMessageBox(Sprite const& sprite, Sprite const& mask,
                                    glm::vec2 topLeft, glm::vec4 tint,
                                    int alpha, int fadeRange, float effectCt,
                                    glm::vec2 scale) {
  const RectF dest =
      sprite.ScaledBounds().Scale(scale, {0.0f, 0.0f}).Translate(topLeft);
  DrawCCMessageBox(sprite, mask, dest, tint, alpha, fadeRange, effectCt);
}

void BaseRenderer::DrawConvexShape(const std::span<const glm::vec2> vertices,
                                   const glm::mat4 transformation,
                                   const glm::vec4 color) {
  assert(vertices.size() >= 3);

  if (vertices.size() == 4) {
    CornersQuad quad = {vertices[0], vertices[1], vertices[2], vertices[3]};
    quad.Transform(transformation);
    DrawQuad(quad, color);

    return;
  }

  std::vector<VertexBufferSprites> vertexAttributes;
  vertexAttributes.reserve(vertices.size());
  std::transform(
      vertices.begin(), vertices.end(), std::back_inserter(vertexAttributes),
      [color, transformation](const glm::vec2 pos) {
        return VertexBufferSprites{
            .Position = glm::vec2(transformation * glm::vec4(pos, 0.0f, 1.0f)),
            .Tint = color,
        };
      });

  if (vertices.size() == 3) {
    DrawPrimitives(RectSprite.Sheet, ShaderProgramType::Sprite,
                   vertexAttributes, std::array<uint16_t, 3>{0, 1, 2});
    return;
  }

  std::vector<uint16_t> indices;
  indices.reserve(vertices.size() * 3);

  const glm::vec2 centerOfMass =
      std::reduce(vertices.begin(), vertices.end()) / (float)vertices.size();

  const uint16_t centerOfMassIndex = (uint16_t)vertices.size();
  vertexAttributes.emplace_back(VertexBufferSprites{
      .Position = centerOfMass,
      .Tint = color,
  });

  for (uint16_t i = 0; i < centerOfMassIndex; i++) {
    const uint16_t nextIndex = (i != centerOfMassIndex - 1) ? i + 1 : 0;
    indices.insert(indices.end(), {centerOfMassIndex, i, nextIndex});
  }

  DrawPrimitives(RectSprite.Sheet, ShaderProgramType::Sprite, vertexAttributes,
                 indices);
}

void BaseRenderer::DrawQuad(const CornersQuad& dest, const glm::vec4 color) {
  DrawSprite(RectSprite, dest, color);
}

void BaseRenderer::QuadSetPosition(CornersQuad quad, glm::vec2* const pos,
                                   int stride) {
  *(glm::vec2*)((uint8_t*)pos + 0 * stride) = DesignToNDC(quad.TopLeft);
  *(glm::vec2*)((uint8_t*)pos + 1 * stride) = DesignToNDC(quad.BottomLeft);
  *(glm::vec2*)((uint8_t*)pos + 2 * stride) = DesignToNDC(quad.BottomRight);
  *(glm::vec2*)((uint8_t*)pos + 3 * stride) = DesignToNDC(quad.TopRight);
}

void BaseRenderer::QuadSetUV(const CornersQuad bounds,
                             const glm::vec2 dimensions, glm::vec2* const uvs,
                             const size_t stride) {
  *(glm::vec2*)((uint8_t*)uvs + 0 * stride) = bounds.TopLeft / dimensions;
  *(glm::vec2*)((uint8_t*)uvs + 1 * stride) = bounds.BottomLeft / dimensions;
  *(glm::vec2*)((uint8_t*)uvs + 2 * stride) = bounds.BottomRight / dimensions;
  *(glm::vec2*)((uint8_t*)uvs + 3 * stride) = bounds.TopRight / dimensions;
}

}  // namespace Impacto
