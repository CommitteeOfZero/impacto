#include "sprites.h"
#include "profile_internal.h"
#include "../io/assetpath.h"
#include "../log.h"
#include "../renderer/renderer.h"
#include "../texture/texture.h"
#include <algorithm>
#include <future>

namespace Impacto {
namespace Profile {

static Texture LoadTexture(Io::Stream* stream, std::string name) {
  Texture texture{};
  if (!texture.Load(stream)) {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Spritesheet {:s} texture could not be imported, using fallback\n",
           name);
    texture.LoadCheckerboard();
  }

  delete stream;

  return texture;
}

// SG0 Steam splits its font atlas into two textures (font_a, font_b).
// Stack them back into one sheet.
static Texture LoadStackedTexture(Io::Stream* firstStream,
                                  Io::Stream* continuationStream,
                                  std::string name, int firstHeight,
                                  int height) {
  Texture first = LoadTexture(firstStream, name);
  Texture continuation = LoadTexture(continuationStream, name + " (continued)");

  if (firstHeight <= 0 || firstHeight > first.Height)
    firstHeight = first.Height;
  const size_t firstPixels = static_cast<size_t>(first.Width) * first.Height;
  if (first.Width != continuation.Width ||
      first.Format != continuation.Format || firstPixels == 0 ||
      height <= firstHeight || height > firstHeight + continuation.Height) {
    ImpLog(LogLevel::Error, LogChannel::Profile,
           "Spritesheet {:s} pages cannot be stacked to height {:d}, using "
           "the first page only\n",
           name, height);
    return first;
  }

  const size_t bytesPerPixel = first.Buffer.size() / firstPixels;
  const size_t rowBytes = static_cast<size_t>(first.Width) * bytesPerPixel;
  Texture stacked{};
  stacked.Init(first.Format, first.Width, height);
  std::copy_n(first.Buffer.begin(), firstHeight * rowBytes,
              stacked.Buffer.begin());
  std::copy_n(continuation.Buffer.begin(), (height - firstHeight) * rowBytes,
              stacked.Buffer.begin() + firstHeight * rowBytes);
  return stacked;
}

void LoadSpritesheets() {
  EnsurePushMemberOfType("SpriteSheets", LUA_TTABLE);

  std::vector<std::tuple<std::string, std::future<Texture>>> futures;

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    std::string name(EnsureGetKey<std::string>());

    SpriteSheet& sheet = SpriteSheets[name];
    sheet.DesignWidth = EnsureGetMember<float>("DesignWidth");
    sheet.DesignHeight = EnsureGetMember<float>("DesignHeight");
    sheet.ScriptHandled = TryGetMember<bool>("ScriptHandled") == true;
    sheet.Path = EnsureGetMember<Io::AssetPath>("Path");

    if (sheet.ScriptHandled) {
      sheet.Texture = Renderer->MapSpriteSheet(sheet);
    } else {
      Io::Stream* stream;
      IoError err = sheet.Path.Open(&stream);
      if (err != IoError_OK) {
        ImpLog(LogLevel::Fatal, LogChannel::Profile,
               "Could not open spritesheet {:s}\n", name);
        Window->Shutdown();
      }

      if (auto continuationPath =
              TryGetMember<Io::AssetPath>("ContinuationPath")) {
        Io::Stream* continuationStream;
        if (continuationPath->Open(&continuationStream) != IoError_OK) {
          ImpLog(LogLevel::Fatal, LogChannel::Profile,
                 "Could not open spritesheet {:s} continuation page\n", name);
          Window->Shutdown();
        }
        futures.emplace_back(std::tuple(
            name, std::async(std::launch::async, LoadStackedTexture, stream,
                             continuationStream, name,
                             TryGetMember<int>("FirstPageHeight").value_or(0),
                             static_cast<int>(sheet.DesignHeight))));
      } else {
        futures.emplace_back(std::tuple(
            name, std::async(std::launch::async, LoadTexture, stream, name)));
      }
    }
    Pop();
  }

  for (auto& [name, future] : futures) {
    SpriteSheet& sheet = SpriteSheets[name];
    sheet.Texture = future.get().Submit();
  }

  Pop();

  EnsurePushMemberOfType("Sprites", LUA_TTABLE);

  PushInitialIndex();
  while (PushNextTableElement() != 0) {
    std::string name(EnsureGetKey<std::string>());

    Sprite& sprite = Sprites[name];
    sprite.Sheet = EnsureGetMember<SpriteSheet>("Sheet");
    sprite.Bounds = EnsureGetMember<RectF>("Bounds");
    if (!TryGetMember<glm::vec2>("BaseScale", sprite.BaseScale))
      sprite.BaseScale = glm::vec2(1.0f);

    Pop();
  }

  Pop();
}

}  // namespace Profile
}  // namespace Impacto