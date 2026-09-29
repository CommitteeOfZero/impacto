#pragma once

#include "../spritesheet.h"
#include "../util.h"

namespace Impacto {

struct VertexBufferSprites {
  glm::vec2 Position = {0.0f, 0.0f};
  glm::vec2 UV = {0.0f, 0.0f};
  glm::vec4 Tint = glm::vec4(1.0f);
  glm::vec2 MaskUV = {0.0f, 0.0f};
};

constexpr static inline std::array<VertexBufferSprites, 4> MakeVertices(
    const CornersQuad& textureSection, const CornersQuad& maskSection,
    const CornersQuad& destination, glm::vec4 tint = glm::vec4(1.0f)) {
  return std::array<VertexBufferSprites, 4>{
      VertexBufferSprites{
          .Position = destination.BottomLeft,
          .UV = textureSection.BottomLeft,
          .Tint = tint,
          .MaskUV = maskSection.BottomLeft,
      },
      VertexBufferSprites{
          .Position = destination.TopLeft,
          .UV = textureSection.TopLeft,
          .Tint = tint,
          .MaskUV = maskSection.TopLeft,
      },
      VertexBufferSprites{
          .Position = destination.TopRight,
          .UV = textureSection.TopRight,
          .Tint = tint,
          .MaskUV = maskSection.TopRight,
      },
      VertexBufferSprites{
          .Position = destination.BottomRight,
          .UV = textureSection.BottomRight,
          .Tint = tint,
          .MaskUV = maskSection.BottomRight,
      },
  };
}

enum class TopologyMode : uint8_t { Triangles, TriangleStrips };

struct PrimitiveData {
  std::span<const VertexBufferSprites> Vertices;
  std::span<const uint16_t> Indices;
  TopologyMode Topology;

  template <typename T, typename U>
  operator std::tuple<T, U>() {
    return std::tuple<T, U>{Vertices, Indices};
  }
};

constexpr static inline std::array<VertexBufferSprites, 4> MakeVertices(
    const CornersQuad& textureSection, const CornersQuad& destination,
    glm::vec4 tint = glm::vec4(1.0f)) {
  return MakeVertices(textureSection, RectF(), destination, tint);
}

template <typename T>
concept PositionedSpriteTextureType = is_any_of_v<T, SpriteSheet, Sprite> ||
                                      std::is_base_of_v<TextureRefInterface, T>;

struct PositionedSprite {
 protected:
  static const TextureRefInterface& GetTexture(
      const TextureRefInterface& texture) {
    return texture;
  }
  static const TextureRefInterface& GetTexture(const SpriteSheet& sheet) {
    return *sheet.Texture;
  }
  static const TextureRefInterface& GetTexture(const Sprite& sprite) {
    return *sprite.Sheet.Texture;
  }

  static glm::vec2 GetTextureSize(const TextureRefInterface& texture) {
    return static_cast<glm::vec2>(texture.GetDimensions());
  }
  static glm::vec2 GetTextureSize(const SpriteSheet& sheet) {
    return sheet.GetDimensions();
  }
  static glm::vec2 GetTextureSize(const Sprite& sprite) {
    return {sprite.ScaledWidth(), sprite.ScaledHeight()};
  }

  static CornersQuad GetTextureBounds(const TextureRefInterface& texture) {
    const glm::vec2 size = texture.GetDimensions();
    return RectF(0.0f, 0.0f, size.x, size.y);
  }
  static CornersQuad GetTextureBounds(const SpriteSheet& sheet) {
    return RectF(0.0f, 0.0f, sheet.DesignWidth, sheet.DesignHeight);
  }
  static CornersQuad GetTextureBounds(const Sprite& sprite) {
    return sprite.Bounds;
  }

 public:
  const TextureRefInterface& Texture;

  using QuadVertices = std::array<VertexBufferSprites, 4>;
  std::variant<PrimitiveData, QuadVertices> Vertices;

  glm::mat4 Transformation = glm::mat4(1.0f);

  std::span<const VertexBufferSprites> GetVertices() const {
    return std::holds_alternative<PrimitiveData>(Vertices)
               ? std::get<PrimitiveData>(Vertices).Vertices
               : std::span<const VertexBufferSprites>(
                     std::get<QuadVertices>(Vertices));
  }

  std::span<const uint16_t> GetIndices() const {
    return std::holds_alternative<PrimitiveData>(Vertices)
               ? std::get<PrimitiveData>(Vertices).Indices
               : std::span<const uint16_t>(
                     std::array<uint16_t, 6>{0, 1, 2, 0, 2, 3});
  }

  TopologyMode GetTopologyMode() const {
    return std::holds_alternative<PrimitiveData>(Vertices)
               ? std::get<PrimitiveData>(Vertices).Topology
               : TopologyMode::Triangles;
  }

  PositionedSprite(const Sprite& texture, CornersQuad destination,
                   glm::mat4 transformation = glm::mat4(1.0f),
                   glm::vec4 tint = glm::vec4(1.0f))
      : Texture(GetTexture(texture)),
        Vertices(MakeVertices(texture.NormalizedBounds(), destination, tint)),
        Transformation(transformation) {}

  PositionedSprite(const PositionedSpriteTextureType auto& texture,
                   CornersQuad textureSection, CornersQuad destination,
                   glm::mat4 transformation = glm::mat4(1.0f),
                   glm::vec4 tint = glm::vec4(1.0f))
      : Texture(GetTexture(texture)),
        Vertices(MakeVertices(
            textureSection.Scale(glm::vec2(1.0f) / GetTextureSize(texture),
                                 {0.0f, 0.0f}),
            destination, tint)),
        Transformation(transformation) {}

  PositionedSprite(const PositionedSpriteTextureType auto& texture,
                   CornersQuad destination,
                   glm::mat4 transformation = glm::mat4(1.0f),
                   glm::vec4 tint = glm::vec4(1.0f))
      : PositionedSprite(texture, GetTextureBounds(texture), destination,
                         transformation, tint) {}

  PositionedSprite(const PositionedSpriteTextureType auto& texture,
                   glm::vec2 position, glm::vec4 tint = glm::vec4(1.0f))
      : PositionedSprite(
            texture,
            RectF(position.x, position.y, GetTextureSize(texture).x,
                  GetTextureSize(texture).y),
            glm::mat4(1.0f), tint) {}

  PositionedSprite(const PositionedSpriteTextureType auto& texture,
                   glm::mat4 transformation, glm::vec4 tint = glm::vec4(1.0f))
      : PositionedSprite(texture,
                         RectF(0.0f, 0.0f, GetTextureSize(texture).x,
                               GetTextureSize(texture).y),
                         transformation, tint) {}

  PositionedSprite(const PositionedSpriteTextureType auto& texture,
                   std::span<const VertexBufferSprites> vertices,
                   std::span<const uint16_t> indices, TopologyMode topology,
                   glm::mat4 transformation = glm::mat4(1.0f))
      : Texture(GetTexture(texture)),
        Vertices(PrimitiveData{
            .Vertices = vertices, .Indices = indices, .Topology = topology}),
        Transformation(transformation) {}

 protected:
  PositionedSprite(const PositionedSpriteTextureType auto& texture,
                   decltype(Vertices)&& vertices,
                   glm::mat4 transformation = glm::mat4(1.0f))
      : Texture(GetTexture(texture)),
        Vertices(std::move(vertices)),
        Transformation(transformation) {}
};

struct PositionedMaskedSprite : public PositionedSprite {
  const TextureRefInterface& MaskTexture;
  glm::mat4 MaskTransformation = glm::mat4(1.0f);

  PositionedMaskedSprite(const PositionedSpriteTextureType auto& texture,
                         CornersQuad textureSection,
                         const PositionedSpriteTextureType auto& mask,
                         CornersQuad maskSection, CornersQuad destination,
                         glm::mat4 transformation = glm::mat4(1.0f),
                         glm::mat4 maskTransformation = glm::mat4(1.0f),
                         glm::vec4 tint = glm::vec4(1.0f))
      : PositionedSprite(
            texture,
            MakeVertices(
                textureSection.Scale(glm::vec2(1.0f) / GetTextureSize(texture),
                                     {0.0f, 0.0f}),
                maskSection.Scale(glm::vec2(1.0f) / GetTextureSize(mask),
                                  {0.0f, 0.0f}),
                destination, tint),
            transformation),
        MaskTexture(GetTexture(mask)),
        MaskTransformation(maskTransformation) {}

  PositionedMaskedSprite(const PositionedSpriteTextureType auto& texture,
                         const PositionedSpriteTextureType auto& mask,
                         CornersQuad maskSection, CornersQuad destination,
                         glm::mat4 transformation = glm::mat4(1.0f),
                         glm::mat4 maskTransformation = glm::mat4(1.0f),
                         glm::vec4 tint = glm::vec4(1.0f))
      : PositionedMaskedSprite(texture, GetTextureBounds(texture), mask,
                               maskSection, destination, transformation,
                               maskTransformation, tint) {}

  PositionedMaskedSprite(const PositionedSpriteTextureType auto& texture,
                         CornersQuad textureSection,
                         const PositionedSpriteTextureType auto& mask,
                         CornersQuad destination,
                         glm::mat4 transformation = glm::mat4(1.0f),
                         glm::mat4 maskTransformation = glm::mat4(1.0f),
                         glm::vec4 tint = glm::vec4(1.0f))
      : PositionedMaskedSprite(texture, textureSection, mask,
                               GetTextureBounds(mask), destination,
                               transformation, maskTransformation, tint) {}

  PositionedMaskedSprite(const PositionedSpriteTextureType auto& texture,
                         const PositionedSpriteTextureType auto& mask,
                         CornersQuad destination,
                         glm::mat4 transformation = glm::mat4(1.0f),
                         glm::mat4 maskTransformation = glm::mat4(1.0f),
                         glm::vec4 tint = glm::vec4(1.0f))
      : PositionedMaskedSprite(texture, GetTextureBounds(texture), mask,
                               GetTextureBounds(mask), destination,
                               transformation, maskTransformation, tint) {}

  PositionedMaskedSprite(const PositionedSpriteTextureType auto& texture,
                         const PositionedSpriteTextureType auto& mask,
                         glm::vec2 position, glm::vec4 tint = glm::vec4(1.0f))
      : PositionedMaskedSprite(
            texture, mask,
            RectF(position.x, position.y, GetTextureSize(texture).x,
                  GetTextureSize(texture).y),
            glm::mat4(1.0f), glm::mat4(1.0f), tint) {}

  PositionedMaskedSprite(const PositionedSpriteTextureType auto& texture,
                         const PositionedSpriteTextureType auto& mask,
                         glm::mat4 transformation, glm::mat4 maskTransformation,
                         glm::vec4 tint = glm::vec4(1.0f))
      : PositionedMaskedSprite(texture, mask,
                               RectF(0.0f, 0.0f, GetTextureSize(texture).x,
                                     GetTextureSize(texture).y),
                               transformation, maskTransformation, tint) {}

  PositionedMaskedSprite(const PositionedSpriteTextureType auto& texture,
                         const PositionedSpriteTextureType auto& mask,
                         std::span<const VertexBufferSprites> vertices,
                         std::span<const uint16_t> indices,
                         TopologyMode topology,
                         glm::mat4 transformation = glm::mat4(1.0f),
                         glm::mat4 maskTransformation = glm::mat4(1.0f))
      : PositionedSprite(texture, vertices, indices, topology, transformation),
        MaskTexture(GetTexture(mask)),
        MaskTransformation(maskTransformation) {}
};

}  // namespace Impacto
