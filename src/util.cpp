#include "util.h"

#include "profile/game.h"
#include <memory>

namespace Impacto {

glm::mat2 Rotate2D(float angle) {
  glm::mat2 result;
  float cosa = cosf(angle);
  float sina = sinf(angle);
  result[0][0] = cosa;
  result[0][1] = sina;
  result[1][0] = -sina;
  result[1][1] = cosa;
  return result;
}

glm::vec2 DesignToNDC(glm::vec2 xy) {
  glm::vec2 result;
  result.x = (xy.x / (Profile::DesignWidth * 0.5f)) - 1.0f;
  result.y = 1.0f - (xy.y / (Profile::DesignHeight * 0.5f));
  return result;
}

glm::vec2 DesignToNDCNonFlipped(glm::vec2 xy) {
  glm::vec2 result;
  result.x = (xy.x / (Profile::DesignWidth * 0.5f)) - 1.0f;
  result.y = (xy.y / (Profile::DesignHeight * 0.5f)) - 1.0f;
  return result;
}

RectF DesignToNDC(RectF const& rect) {
  RectF result;
  glm::vec2 xy = DesignToNDC(glm::vec2(rect.X, rect.Y));
  result.X = xy.x;
  result.Y = xy.y;
  result.Width = rect.Width / (Profile::DesignWidth * 0.5f);
  result.Height = rect.Height / (Profile::DesignHeight * 0.5f);
  return result;
}

uint32_t GetHashCode(uint8_t* data, int length) {
  uint32_t hash = 2166136261;
  for (int i = 0; i < length; i++) {
    hash = (hash ^ data[i]) * 16777619;
  }

  return hash;
}

std::string DumpMat4(glm::mat4* matrix, std::string_view columnSeparator,
                     std::string_view rowSeparator) {
  return fmt::sprintf(
      "%s%.03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f%s%s"
      "%.03f%s%.03f%s%.03f%s%.03f%s%s%.03f%s%.03f%s%.03f%s%.03f",
      columnSeparator, (*matrix)[0][0], columnSeparator, (*matrix)[1][0],
      columnSeparator, (*matrix)[2][0], columnSeparator, (*matrix)[3][0],
      rowSeparator, columnSeparator, (*matrix)[0][1], columnSeparator,
      (*matrix)[1][1], columnSeparator, (*matrix)[2][1], columnSeparator,
      (*matrix)[3][1], rowSeparator, columnSeparator, (*matrix)[0][2],
      columnSeparator, (*matrix)[1][2], columnSeparator, (*matrix)[2][2],
      columnSeparator, (*matrix)[3][2], rowSeparator, columnSeparator,
      (*matrix)[0][3], columnSeparator, (*matrix)[1][3], columnSeparator,
      (*matrix)[2][3], columnSeparator, (*matrix)[3][3]);
}

int ResizeImage(Rect const& srcRect, Rect const& dstRect,
                std::span<uint8_t> src, std::span<uint8_t> dst) {
  using SurfacePtr = std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)>;
  if (srcRect.Width == dstRect.Width && srcRect.Height == dstRect.Height) {
    assert(dst.size() >= src.size());
    memcpy(dst.data(), src.data(), src.size());
    return 0;
  }
  const uint32_t sdlFormat = SDL_PIXELFORMAT_RGBA32;
  const int bytesPerPixel = 4;

  assert(src.size() >= srcRect.Width * srcRect.Height * bytesPerPixel);
  SurfacePtr srcSurface(
      SDL_CreateRGBSurfaceWithFormatFrom(
          src.data(), srcRect.Width, srcRect.Height, bytesPerPixel * 8,
          srcRect.Width * bytesPerPixel, sdlFormat),
      SDL_FreeSurface);
  SDL_SetSurfaceBlendMode(srcSurface.get(), SDL_BLENDMODE_NONE);
  if (!srcSurface) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "SDL_CreateRGBSurfaceWithFormatFrom failed: {:s}\n", SDL_GetError());
    return -1;
  }
  assert(dst.size() >= dstRect.Width * dstRect.Height * 4);
  SurfacePtr dstSurface(
      SDL_CreateRGBSurfaceWithFormatFrom(
          dst.data(), dstRect.Width, dstRect.Height, bytesPerPixel * 8,
          dstRect.Width * bytesPerPixel, sdlFormat),
      SDL_FreeSurface);
  if (!dstSurface) {
    ImpLog(LogLevel::Error, LogChannel::Render,
           "SDL_CreateRGBSurfaceWithFormat failed: {:s}\n", SDL_GetError());
    return -1;
  }
  SDL_SetSurfaceBlendMode(dstSurface.get(), SDL_BLENDMODE_NONE);

  SDL_Rect srcRectSDL = {srcRect.X, srcRect.Y, srcRect.Width, srcRect.Height};
  SDL_Rect dstRectSDL = {dstRect.X, dstRect.Y, dstRect.Width, dstRect.Height};
  if (SDL_BlitScaled(srcSurface.get(), &srcRectSDL, dstSurface.get(),
                     &dstRectSDL) != 0) {
    ImpLog(LogLevel::Error, LogChannel::Render, "SDL_BlitScaled failed: {:s}\n",
           SDL_GetError());
    return -1;
  }
  return 0;
}

RectF RectF::Coalesce(const RectF& first, const RectF& second) {
  RectF rect;

  rect.X = std::min(first.X, second.X);
  rect.Y = std::min(first.Y, second.Y);
  rect.Width =
      std::max(first.X + first.Width, second.X + second.Width) - rect.X;
  rect.Height =
      std::max(first.Y + first.Height, second.Y + second.Height) - rect.Y;

  return rect;
}

}  // namespace Impacto