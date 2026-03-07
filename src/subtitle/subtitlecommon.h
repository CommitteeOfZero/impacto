#pragma once

#include <memory>
#include <array>
#include <variant>

namespace Impacto::Subtitle {
struct SubtitleEntry {
  struct BitmapData {
    int X{};
    int Y{};
    int W{};
    int H{};
    int NbColors{};
    std::array<std::unique_ptr<uint8_t[]>, 4> Data{};
    int LineSize[4]{};
  };

  std::variant<std::monostate, BitmapData, std::string> Data;
  int Flags{};
  std::chrono::milliseconds StartMs{};
  std::chrono::milliseconds Duration{};
};
}  // namespace Impacto::Subtitle