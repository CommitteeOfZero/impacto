#pragma once

#include "../src/text/text.h"

namespace Impacto {

struct NameInfo {
  bool RenderWindow;
  std::optional<int> NameId;
  std::span<ProcessedTextGlyph> Name;
};

class NametagDisplay {
 public:
  static std::unique_ptr<NametagDisplay> Create();
  virtual ~NametagDisplay() = default;

  virtual void Render(const NameInfo& nameInfo, glm::vec4 tint) = 0;
  virtual void Update(float dt) = 0;
  virtual void Reset() = 0;

  virtual void Show() { Hidden = false; }
  virtual void Hide() { Hidden = true; }

 protected:
  bool Hidden = true;
};

class VoidNametagDisplay : public NametagDisplay {
 public:
  void Render(const NameInfo& nameInfo, glm::vec4 tint) override;
  void Update(float dt) override {}
  void Reset() override {}
};

class SpriteNametagDisplay : public NametagDisplay {
 public:
  void Render(const NameInfo& nameInfo, glm::vec4 tint) override;
  void Update(float dt) override {}
  void Reset() override {}
};

class ThreePieceNametagDisplay : public NametagDisplay {
 public:
  void Render(const NameInfo& nameInfo, glm::vec4 tint) override;
  void Update(float dt) override {}
  void Reset() override {}
};

}  // namespace Impacto
