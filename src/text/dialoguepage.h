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
  int AnimationId = 0;
  int NextAnimationId = 0;
  int CharacterId = -1;

  TypewriterEffect Typewriter;
  Animation FadeAnimation;
  Animation TextFadeAnimation;

  std::vector<ProcessedTextGlyph> Name;
  bool RenderName = false;

  Audio::AudioStream* Voice;

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
    const int mode = ScrWork[SW_MESMODE0 + 10 * Id];
    assert(mode < 4);
    return static_cast<DialoguePageMode>(mode);
  }

  bool TextIsFullyOpaque();
  void Clear() override;
  void AddString(Vm::Sc3VmThread* ctx, Audio::AudioStream* voice = 0,
                 bool acted = true, int animId = 0, int charId = -1,
                 bool shouldUpdateCharId = false);
  void Update(float dt);

  void Move(glm::vec2 relativePos) override;

  void Render(float alpha, RendererOutlineMode outlineMode =
                               RendererOutlineMode::Full) override;
  void Hide();
  void Show();

 private:
  void FinishLine(Vm::Sc3VmThread* ctx, size_t nextLineStart,
                  const RectF& boxBounds, TextAlignment alignment);

  std::unique_ptr<DialogueBox> DialogueBoxInst = DialogueBox::Create();
};

inline std::vector<DialoguePage> DialoguePages;
inline bool SkipModeEnabled = false;
inline bool AutoModeEnabled = false;

}  // namespace Impacto
