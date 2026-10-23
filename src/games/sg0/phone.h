#pragma once

#include <span>
#include <vector>

#include "../../ui/menu.h"
#include "../../text/text.h"

namespace Impacto {
namespace UI {
namespace SG0 {

// STEINS;GATE 0 phone (opcode 10:37, PhoneSG0). The phone is how the player
// makes route decisions: scripts arm a trigger (incoming call, power off,
// calling a contact, opening RINE, Amadeus) and poll flag 2620 after every
// message line; the engine sets it, plus the result in ScrWork 6609 / 6608,
// when the player performs the armed action. Scripts also wait for the slide
// counter ScrWork 6601 to reach 29 (raised) or 0 (lowered), which the engine
// animates.
//
// Session handling, the triggers, a plain overlay with text menus, and the
// RINE screen (see rine.h for the message model).
class Phone : public Menu {
 public:
  void Show() override;
  void Hide() override;
  void Update(float dt) override;
  void UpdateInput(float dt) override;
  void Render() override;

  // PhoneSG0 sub-types.
  void LoadTables(uint32_t scriptBufferId, std::span<const uint16_t, 7> labels);
  void BeginSession();
  void EndSession();

  // Runs before the VM each frame: while the player has the phone open, the
  // phone takes the confirm / cancel / direction input so the message window
  // does not advance underneath it.
  void TakeInput(float dt);

 private:
  enum class Screen { Off, Home, PowerOff, Contacts, Rine, Amadeus, Call };
  enum class Action {
    Answer,
    Reject,
    AskPowerOff,
    PowerOff,
    Back,
    Contacts,
    Rine,
    Amadeus,
    CallContact,
    Connect
  };
  struct Contact {
    uint16_t NameStr;
    uint16_t RegisteredFlag;
    uint16_t CallTriggerFlag;
  };
  struct Item {
    std::vector<ProcessedTextGlyph> Text;
    Action Act;
    int Param;
  };

  Screen DisplayedScreen() const;
  void Open(Screen screen);
  void Activate(const Item& item);
  void Back();
  void Fire(int result, int kind);
  void Close();
  std::vector<ProcessedTextGlyph> Layout(uint16_t strId, glm::vec2 pos,
                                         float fontSize) const;
  std::vector<ProcessedTextGlyph> Layout(uint16_t strId, glm::vec2 pos,
                                         float fontSize,
                                         DialogueColorPair colors) const;
  void AddItem(uint16_t uiStr, Action act, int param = 0);
  void AddItemText(uint16_t strId, Action act, int param = 0);
  glm::vec2 BodyPos() const;

  // RINE screen.
  struct RineBubble {
    RectF Box;  // relative to the top of the message area
    std::vector<ProcessedTextGlyph> Text;
    int Sticker = -1;
    int Avatar = -1;
  };
  struct RineReply {
    std::vector<ProcessedTextGlyph> Text;
    int Sticker = -1;
  };
  void LayoutRine();
  void RenderRine(glm::vec2 screen);
  void UpdateRineInput();
  float RineReplyAreaHeight() const;
  Sprite AvatarFor(int avatar) const;
  Sprite StickerFor(int sticker) const;

  uint32_t ScriptBuffer = 0;
  bool Loaded = false;
  std::vector<uint16_t> UiStrings;
  std::vector<Contact> Contacts;

  bool SessionOpen = false;
  Screen Current = Screen::Home;
  int CurrentMode = -1;
  std::vector<ProcessedTextGlyph> Title;
  std::vector<ProcessedTextGlyph> Subtitle;
  std::vector<Item> Items;
  int Cursor = 0;
  float SlideTimer = 0.0f;

  uint64_t RineLaidOut = UINT64_MAX;
  std::vector<RineBubble> RineBubbles;
  float RineContentHeight = 0.0f;
  std::vector<ProcessedTextGlyph> RineTitle;
  std::vector<ProcessedTextGlyph> RineInput;
  std::vector<RineReply> RineReplies;
  int RineReplyKind = 0;
  int RineReplyCursor = 0;
};

inline Phone* PhonePtr = nullptr;

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
