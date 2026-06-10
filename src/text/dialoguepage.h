#pragma once

#include "textpage.h"
#include "typewritereffect.h"

#include "../profile/scriptvars.h"

#include "../hud/dialoguebox.h"

#include "../audio/audiostream.h"

namespace Impacto {
enum DialoguePageMode : uint8_t {
  DPM_ADV = 0,
  DPM_NVL = 1,
  DPM_REV = 2,
  DPM_TIPS = 3
};

struct DialoguePage : public TextPage {
  static void Init();

  DialoguePage(int id) : Id(id) {}

  int Id;
  int CurrentVoiceCharacterId = 0;
  int AnimationId = 0;
  int CharacterId = 0;
  std::optional<int> AudioId;

  TypewriterEffect Typewriter;
  Animation FadeAnimation;
  Animation TextFadeAnimation;

  std::vector<ProcessedTextGlyph> Name;
  bool RenderName = false;

  enum class AdvanceMethodType : uint8_t {
    Skip,
    Present,
    PresentClear,
    Present0x18,
    AutoForwardSyncVoice,
    AutoForward,
  };
  AdvanceMethodType AdvanceMethod = AdvanceMethodType::Skip;

  float AutoWaitTime = 0.0f;

  enum class State { Initial, Showing, Hiding, Shown, Hidden };
  State GetState() const;

  DialoguePageMode GetMode() const {
    using namespace Profile::ScriptVars;
    const uint8_t mode = static_cast<uint8_t>(ScrWork[SW_MESMODE0 + 10 * Id]);
    return *magic_enum::enum_cast<DialoguePageMode>(mode);
  }

  const TextModeInfo& GetTextModeInfo() const {
    return TextModesInfo[GetMode()];
  }

  bool TextIsFullyOpaque();
  void Clear() override;
  void AddString(Vm::Sc3VmThread* ctx,
                 std::optional<int> voiceId = std::nullopt, bool acted = true,
                 int animId = 0, int charId = 0);
  void Update(float dt);

  void Move(glm::vec2 relativePos) override;

  void Render(float alpha, RendererOutlineMode outlineMode =
                               RendererOutlineMode::Full) override;
  void Hide();
  void Show();

  void PushBacklogEntry();

 protected:
  RectF SetBounds() override;

 private:
  std::unique_ptr<DialogueBox> DialogueBoxInst = DialogueBox::Create(*this);

  std::optional<Vm::BufferOffsetContext> CurrentStringAddress;
};

inline std::vector<DialoguePage> DialoguePages;
inline bool SkipModeEnabled = false;
inline bool AutoModeEnabled = false;

}  // namespace Impacto
