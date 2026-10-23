#include "phone.h"
#include "rine.h"
#include "fullwidth.h"

#include <algorithm>

#include "../../profile/games/sg0/phone.h"
#include "../../profile/dialogue.h"
#include "../../profile/vm.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/vm.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::SG0::Phone;
using namespace Impacto::Vm::Interface;

// Phone flags and work indices, from the SG0 scripts (sg0-phone.md section 4).
// They overlap CHAOS;CHILD's SW_BGEFF1_* names, so they stay local here.
namespace {
constexpr int SF_PHONE_MAY_OPEN = 1611;  // engine: no other UI is up
constexpr int SF_PHONE_SHOWN = 2600;     // phone raised (script or player)
constexpr int SF_PHONE_SESSION_ACTIVE = 2605;  // script: player session running
constexpr int SF_PHONE_CLOSED = 2603;          // engine: the player closed it
constexpr int SF_PHONE_SESSION_END = 2604;  // engine: interactive session over
constexpr int SF_AMADEUS_LOCKED = 2613;
constexpr int SF_TRIGGER_FIRED = 2620;  // engine: armed action performed
constexpr int SF_CALL_RINGING = 2740;
constexpr int SF_AMADEUS_RINGING = 2743;
constexpr int SF_REJECT_CALL_ARMED = 2744;
constexpr int SF_REJECT_AMADEUS_ARMED = 2745;
constexpr int SF_RINE_ARMED = 2748;
constexpr int SF_POWER_OFF_ARMED = 2749;
constexpr int SF_AMADEUS_CALL_ARMED = 2750;

constexpr int SW_PHONE_SLIDE = 6601;           // 0 (lowered) .. 29 (raised)
constexpr int SW_PHONE_MODE = 6602;            // current screen
constexpr int SW_PHONE_OPEN_MODE = 6604;       // screen the player opens to
constexpr int SW_PHONE_TRIGGER_KIND = 6608;    // 1 call, 3 RINE
constexpr int SW_PHONE_TRIGGER_RESULT = 6609;  // 0 answer / 1 reject / slot
constexpr int SW_PHONE_CALLER = 6613;          // contact slot of the caller
constexpr int SlideMax = 29;

// _phone UI string ids (label 2).
constexpr uint16_t StrOK = 5, StrCancel = 6, StrPowerOff = 7, StrContacts = 10,
                   StrRine = 11, StrAmadeus = 12, StrIncomingCall = 17,
                   StrAnswer = 18, StrReject = 19, StrCalling = 21, StrEnd = 22,
                   StrCallFromKurisu = 25, StrConnectToKurisu = 27;
constexpr int OwnNumberSlot = 10;
}  // namespace

void Phone::LoadTables(uint32_t scriptBufferId,
                       std::span<const uint16_t, 7> labels) {
  ScriptBuffer = scriptBufferId;
  const std::span<uint8_t> script = Vm::ScriptBuffers[scriptBufferId];

  // labels[0]: u16 UI string ids up to the next table.
  const uint32_t stringsStart =
      Vm::ScriptGetLabelAddress(scriptBufferId, labels[0]);
  const uint32_t stringsEnd =
      Vm::ScriptGetLabelAddress(scriptBufferId, labels[1]);
  UiStrings.clear();
  for (uint32_t pos = stringsStart; pos + 2 <= stringsEnd; pos += 2) {
    UiStrings.push_back(UnalignedRead<uint16_t>(&script[pos]));
  }

  // labels[1]: u16 keypad string, then {u8 chara, u16 name, u16 number,
  // u16 registered flag, u16 call-trigger flag} until chara 0xFF.
  Contacts.clear();
  uint32_t pos = Vm::ScriptGetLabelAddress(scriptBufferId, labels[1]) + 2;
  while (pos + 9 <= script.size() && script[pos] != 0xFF) {
    Contacts.push_back({UnalignedRead<uint16_t>(&script[pos + 1]),
                        UnalignedRead<uint16_t>(&script[pos + 5]),
                        UnalignedRead<uint16_t>(&script[pos + 7])});
    pos += 9;
  }
  Loaded = true;
}

std::vector<ProcessedTextGlyph> Phone::Layout(uint16_t strId, glm::vec2 pos,
                                              float fontSize) const {
  return Layout(strId, pos, fontSize, {TextColor, OutlineColor});
}

std::vector<ProcessedTextGlyph> Phone::Layout(uint16_t strId, glm::vec2 pos,
                                              float fontSize,
                                              DialogueColorPair colors) const {
  Vm::Sc3VmThread dummy;
  dummy.ScriptBufferId = ScriptBuffer;
  dummy.IpOffset = Vm::ScriptGetStrAddress(ScriptBuffer, strId);
  dummy.UseMSBBuffers = Profile::Vm::UseMsbStrings;
  return TextLayoutPlainLine(&dummy, 255, *Profile::Dialogue::DialogueFont,
                             fontSize, colors, 1.0f, pos, TextAlignment::Left);
}

void Phone::AddItem(uint16_t uiStr, Action act, int param) {
  const uint16_t strId = uiStr < UiStrings.size() ? UiStrings[uiStr] : 0;
  AddItemText(strId, act, param);
}

void Phone::AddItemText(uint16_t strId, Action act, int param) {
  const float y = ItemsTop + Items.size() * RowHeight;
  Items.push_back(
      {Layout(strId, glm::vec2(Padding.x, y), ItemFontSize), act, param});
}

Phone::Screen Phone::DisplayedScreen() const {
  if (GetFlag(SF_CALL_RINGING) || GetFlag(SF_AMADEUS_RINGING)) {
    return Screen::Call;
  }
  switch (ScrWork[SW_PHONE_MODE]) {
    case 0:
      return Screen::Off;
    case 2:
      return Screen::PowerOff;
    case 7:
      return Screen::Rine;
    case 11:
      return Screen::Contacts;
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
      return Screen::Call;
    case 17:
    case 18:
    case 19:
      return Screen::Amadeus;
    default:
      return Screen::Home;
  }
}

void Phone::Open(Screen screen) {
  Current = screen;
  CurrentMode = ScrWork[SW_PHONE_MODE];
  Title.clear();
  Subtitle.clear();
  Items.clear();
  Cursor = 0;
  if (!Loaded) return;

  const auto ui = [this](uint16_t id) {
    return id < UiStrings.size() ? UiStrings[id] : uint16_t{0};
  };
  const glm::vec2 titlePos = Padding;
  const glm::vec2 subtitlePos = Padding + glm::vec2(0.0f, RowHeight);

  switch (screen) {
    case Screen::Off:
      break;
    case Screen::Home:
      AddItem(StrContacts, Action::Contacts);
      AddItem(StrRine, Action::Rine);
      AddItem(StrAmadeus, Action::Amadeus);
      AddItem(StrPowerOff, Action::AskPowerOff);
      break;
    case Screen::PowerOff:
      Title = Layout(ui(StrPowerOff), titlePos, TitleFontSize);
      AddItem(StrOK, Action::PowerOff);
      AddItem(StrCancel, Action::Back);
      break;
    case Screen::Contacts:
      Title = Layout(ui(StrContacts), titlePos, TitleFontSize);
      for (int slot = 0; slot < static_cast<int>(Contacts.size()); slot++) {
        if (slot == OwnNumberSlot || !GetFlag(Contacts[slot].RegisteredFlag)) {
          continue;
        }
        AddItemText(Contacts[slot].NameStr, Action::CallContact, slot);
      }
      break;
    case Screen::Rine:
      RineLaidOut = UINT64_MAX;  // relaid out on the next update
      RineReplyCursor = 0;
      break;
    case Screen::Amadeus:
      Title = Layout(ui(StrAmadeus), titlePos, TitleFontSize);
      Subtitle = Layout(ui(StrConnectToKurisu), subtitlePos, ItemFontSize);
      AddItem(StrOK, Action::Connect);
      AddItem(StrCancel, Action::Back);
      break;
    case Screen::Call: {
      const int caller = ScrWork[SW_PHONE_CALLER];
      if (GetFlag(SF_AMADEUS_RINGING)) {
        Title = Layout(ui(StrCallFromKurisu), titlePos, TitleFontSize);
      } else if (caller >= 0 && caller < static_cast<int>(Contacts.size())) {
        Title = Layout(Contacts[caller].NameStr, titlePos, TitleFontSize);
      }
      const bool ringing =
          GetFlag(SF_CALL_RINGING) || GetFlag(SF_AMADEUS_RINGING);
      const int mode = ScrWork[SW_PHONE_MODE];
      const uint16_t status = ringing || mode == 13 ? StrIncomingCall
                              : mode == 16          ? StrCalling
                              : mode == 15          ? StrEnd
                                                    : 0xFFFF;
      if (status != 0xFFFF) {
        Subtitle = Layout(ui(status), subtitlePos, ItemFontSize);
      }
      if (ringing) {
        AddItem(StrAnswer, Action::Answer);
        AddItem(StrReject, Action::Reject);
        if (GetFlag(SF_POWER_OFF_ARMED))
          AddItem(StrPowerOff, Action::AskPowerOff);
      }
    } break;
  }
}

void Phone::Fire(int result, int kind) {
  ScrWork[SW_PHONE_TRIGGER_RESULT] = result;
  ScrWork[SW_PHONE_TRIGGER_KIND] = kind;
  SetFlag(SF_TRIGGER_FIRED, true);
  // End the session without SF_PHONE_CLOSED: the phone stays raised for the
  // scripted follow-up.
  SetFlag(SF_PHONE_SESSION_END, true);
  Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
}

void Phone::Close() {
  SetFlag(SF_PHONE_CLOSED, true);
  SetFlag(SF_PHONE_SESSION_END, true);
}

void Phone::Back() {
  if (Current == Screen::Home || Current == Screen::Call ||
      Current == Screen::Off) {
    Close();
  } else if (GetFlag(SF_CALL_RINGING) || GetFlag(SF_AMADEUS_RINGING)) {
    Open(Screen::Call);
  } else {
    Open(Screen::Home);
  }
}

void Phone::Activate(const Item& item) {
  const bool callRinging = GetFlag(SF_CALL_RINGING);
  const bool amadeusRinging = GetFlag(SF_AMADEUS_RINGING);
  switch (item.Act) {
    case Action::Answer:
      if (callRinging || amadeusRinging)
        Fire(0, ScrWork[SW_PHONE_TRIGGER_KIND]);
      break;
    case Action::Reject:
      // Reject only counts when the script armed it; otherwise it is the same
      // as letting the phone ring.
      if ((callRinging && GetFlag(SF_REJECT_CALL_ARMED)) ||
          (amadeusRinging && GetFlag(SF_REJECT_AMADEUS_ARMED))) {
        Fire(1, ScrWork[SW_PHONE_TRIGGER_KIND]);
      } else {
        Close();
      }
      break;
    case Action::AskPowerOff:
      Open(Screen::PowerOff);
      break;
    case Action::PowerOff:
      if (GetFlag(SF_POWER_OFF_ARMED) || ScrWork[SW_PHONE_OPEN_MODE] == 2) {
        Fire(ScrWork[SW_PHONE_TRIGGER_RESULT], ScrWork[SW_PHONE_TRIGGER_KIND]);
      } else {
        Close();
      }
      break;
    case Action::Back:
      Back();
      break;
    case Action::Contacts:
      Open(Screen::Contacts);
      break;
    case Action::Rine:
      if (GetFlag(SF_RINE_ARMED)) {
        Fire(ScrWork[SW_PHONE_TRIGGER_RESULT], 3);
      } else {
        Open(Screen::Rine);
      }
      break;
    case Action::Amadeus:
      Open(Screen::Amadeus);
      break;
    case Action::CallContact:
      if (GetFlag(Contacts[item.Param].CallTriggerFlag)) {
        Fire(item.Param, 1);
      } else {
        Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0);
      }
      break;
    case Action::Connect:
      if (GetFlag(SF_AMADEUS_CALL_ARMED) && !GetFlag(SF_AMADEUS_LOCKED)) {
        Fire(ScrWork[SW_PHONE_TRIGGER_RESULT], 2);
      } else {
        Audio::PlayInGroup(Audio::ACG_SE, "sysse", 4, false, 0);
      }
      break;
  }
}

void Phone::BeginSession() {
  // Startup and new game also run sub-type 1 as a reset; only the _system
  // phone thread's call (session flag 2605 set) opens the phone for the player.
  if (!GetFlag(SF_PHONE_SESSION_ACTIVE)) {
    SessionOpen = false;
    Hide();
    return;
  }
  SessionOpen = true;
  Open(DisplayedScreen());
  Show();
}

void Phone::EndSession() {
  SessionOpen = false;
  Hide();
}

void Phone::Show() {
  if (State == Shown) return;
  State = Shown;
  if (UI::FocusedMenu != nullptr && UI::FocusedMenu != this) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  UI::FocusedMenu = this;
  IsFocused = true;
}

void Phone::Hide() {
  if (State == Hidden) return;
  State = Hidden;
  IsFocused = false;
  if (LastFocusedMenu != nullptr) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
    LastFocusedMenu = nullptr;
  } else {
    UI::FocusedMenu = nullptr;
  }
}

void Phone::TakeInput(float dt) {
  if (!SessionOpen || !IsFocused) return;
  UpdateInput(dt);
  PADinputButtonWentDown = 0;
  PADinputButtonRepeatDown = 0;
  PADinputButtonRepeatAccelDown = 0;
  PADinputMouseWentDown = 0;
}

void Phone::UpdateInput(float dt) {
  if (GetFlag(SF_PHONE_SESSION_END)) return;
  if (Current == Screen::Rine) {
    UpdateRineInput();
    return;
  }
  const int count = static_cast<int>(Items.size());
  if (count > 0) {
    int next = Cursor;
    if (PADinputButtonWentDown & PAD1UP) next = (Cursor + count - 1) % count;
    if (PADinputButtonWentDown & PAD1DOWN) next = (Cursor + 1) % count;
    const glm::vec2 screen = BodyPos() + ScreenOffset;
    for (int i = 0; i < count; i++) {
      const RectF row(screen.x,
                      screen.y + ItemsTop + i * RowHeight - RowPadding,
                      ScreenSize.x, RowHeight);
      if (!row.ContainsPoint(Input::CurMousePos)) continue;
      if (Input::PrevMousePos != Input::CurMousePos) next = i;
      if (Input::MouseButtonWentDown[SDL_BUTTON_LEFT]) {
        Cursor = i;
        Activate(Items[i]);
        return;
      }
    }
    if (next != Cursor) {
      Cursor = next;
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
    }
    if (PADinputButtonWentDown & PAD1A) {
      Activate(Items[Cursor]);
      return;
    }
  }
  if (PADinputButtonWentDown & (PAD1B | PAD1L2 | PAD1R2) ||
      Input::MouseButtonWentDown[SDL_BUTTON_RIGHT]) {
    Back();
  }
}

void Phone::Update(float dt) {
  // Slide counter: one step per 60 Hz frame toward raised or lowered.
  SlideTimer += dt;
  while (SlideTimer >= 1.0f / 60.0f) {
    SlideTimer -= 1.0f / 60.0f;
    int& slide = ScrWork[SW_PHONE_SLIDE];
    if (GetFlag(SF_PHONE_SHOWN)) {
      if (slide < SlideMax) slide++;
    } else if (slide > 0) {
      slide--;
    }
  }

  SetFlag(SF_PHONE_MAY_OPEN,
          UI::FocusedMenu == nullptr || UI::FocusedMenu == this);

  // Follow the scripts' screen changes while the phone is up.
  if (ScrWork[SW_PHONE_SLIDE] > 0 && !SessionOpen) {
    const Screen shown = DisplayedScreen();
    if (shown != Current || ScrWork[SW_PHONE_MODE] != CurrentMode) Open(shown);
  }

  if (Current == Screen::Rine) {
    if (RineLaidOut != RineState.Version) LayoutRine();
    if (SessionOpen) RineState.MarkRead();
  }
}

glm::vec2 Phone::BodyPos() const {
  const float t = ScrWork[SW_PHONE_SLIDE] / static_cast<float>(SlideMax);
  return glm::vec2(HiddenX + (ShownPos.x - HiddenX) * t, ShownPos.y);
}

void Phone::Render() {
  if (ScrWork[SW_PHONE_SLIDE] <= 0 && !GetFlag(SF_PHONE_SHOWN)) return;

  const glm::vec2 body = BodyPos();
  const RectF bodyDest(body.x, body.y, BodySprite.ScaledWidth() * Scale,
                       BodySprite.ScaledHeight() * Scale);
  Renderer->DrawSprite(BodySprite, bodyDest);

  const glm::vec2 screen = body + ScreenOffset;
  const RectF screenRect(screen.x, screen.y, ScreenSize.x, ScreenSize.y);
  if (Current == Screen::Off) {
    Renderer->DrawQuad(screenRect, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    return;
  }
  Renderer->DrawSprite(WallpaperSprite, screenRect);
  Renderer->DrawQuad(screenRect, glm::vec4(0.0f, 0.0f, 0.0f, ScreenShadeAlpha));
  if (Current == Screen::Rine) {
    RenderRine(screen);
    return;
  }

  Font& font = *Profile::Dialogue::DialogueFont;
  font.DrawProcessedText(Title, 1.0f, 1.0f, RendererOutlineMode::Full, screen);
  font.DrawProcessedText(Subtitle, 1.0f, 1.0f, RendererOutlineMode::Full,
                         screen);
  for (int i = 0; i < static_cast<int>(Items.size()); i++) {
    if (SessionOpen && i == Cursor) {
      const RectF bar(screen.x,
                      screen.y + ItemsTop + i * RowHeight - RowPadding,
                      ScreenSize.x, RowHeight);
      Renderer->DrawSprite(SelectionSprite, bar);
    }
    font.DrawProcessedText(Items[i].Text, 1.0f, 1.0f, RendererOutlineMode::Full,
                           screen);
  }
}

// --- RINE screen ----------------------------------------------------------

namespace {
glm::vec4 Rgba(uint32_t rgb, float alpha = 1.0f) {
  return glm::vec4(((rgb >> 16) & 0xFF) / 255.0f, ((rgb >> 8) & 0xFF) / 255.0f,
                   (rgb & 0xFF) / 255.0f, alpha);
}

bool IsSpaceGlyph(const ProcessedTextGlyph& glyph) {
  return glyph.CharId == 0 || glyph.CharId == 63;  // half- and full-width space
}

// Breaks a single laid-out line at spaces so it fits maxWidth; returns the
// size of the wrapped block. Glyphs start at x 0.
glm::vec2 WrapGlyphs(std::vector<ProcessedTextGlyph>& glyphs, float maxWidth,
                     float lineHeight) {
  if (glyphs.empty()) return glm::vec2(0.0f);
  size_t lineStart = 0;
  int lines = 1;
  float width = 0.0f;
  for (size_t i = 0; i < glyphs.size(); i++) {
    if (glyphs[i].DestRect.X + glyphs[i].DestRect.Width > maxWidth &&
        i > lineStart) {
      size_t next = i;
      for (size_t j = i; j > lineStart; j--) {
        if (IsSpaceGlyph(glyphs[j])) {
          next = j + 1;
          break;
        }
      }
      if (next >= glyphs.size()) break;
      const float shift = glyphs[next].DestRect.X;
      for (size_t k = next; k < glyphs.size(); k++) {
        glyphs[k].Move(glm::vec2(-shift, lineHeight));
      }
      lineStart = next;
      i = next;
      lines++;
    }
    width = std::max(width, glyphs[i].DestRect.X + glyphs[i].DestRect.Width);
  }
  return glm::vec2(width, lines * lineHeight);
}
}  // namespace

Sprite Phone::AvatarFor(int avatar) const {
  Sprite sprite = RineAvatarSprite;
  sprite.Bounds.Y += avatar * RineAvatarStepY;
  return sprite;
}

Sprite Phone::StickerFor(int sticker) const {
  Sprite sprite = RineStickerSprite;
  sprite.Bounds.X += (sticker % RineStickerColumns) * RineStickerStep.x;
  sprite.Bounds.Y += (sticker / RineStickerColumns) * RineStickerStep.y;
  return sprite;
}

void Phone::LayoutRine() {
  RineLaidOut = RineState.Version;
  const DialogueColorPair bubbleText{RineTextColor, RineTextColor};

  RineBubbles.clear();
  float y = RineGap;
  for (const RineMessage& message : RineState.Messages) {
    RineBubble bubble;
    const bool own = message.Sender == RineState.Owner;
    glm::vec2 size;
    if (message.Kind == 1) {
      bubble.Sticker = message.Value;
      size = RineStickerSize;
    } else {
      bubble.Text = Layout(static_cast<uint16_t>(message.Value),
                           glm::vec2(0.0f), RineFontSize, bubbleText);
      size = WrapGlyphs(bubble.Text, RineBubbleMaxWidth, RineLineHeight) +
             2.0f * RineBubblePadding;
    }
    const float x = own ? ScreenSize.x - RineMargin - size.x
                        : RineMargin * 2.0f + RineAvatarSize;
    if (!own && message.Sender >= 0 &&
        message.Sender < static_cast<int>(RineState.Participants.size())) {
      bubble.Avatar = RineState.Participants[message.Sender].Avatar;
    }
    bubble.Box = RectF(x, y, size.x, size.y);
    for (ProcessedTextGlyph& glyph : bubble.Text) {
      glyph.Move(glm::vec2(x, y) + RineBubblePadding);
    }
    y += std::max(size.y, own ? 0.0f : RineAvatarSize) + RineGap;
    RineBubbles.push_back(std::move(bubble));
  }
  RineContentHeight = y;

  // Title: the first member other than the thread owner.
  uint16_t titleStr = StrRine < UiStrings.size() ? UiStrings[StrRine] : 0;
  for (int member : RineState.Members) {
    if (member != RineState.Owner && member >= 0 &&
        member < static_cast<int>(RineState.Participants.size())) {
      titleStr = RineState.Participants[member].NameStr;
      break;
    }
  }
  RineTitle = Layout(titleStr, Padding, TitleFontSize);

  RineInput.clear();
  if (RineState.InputText >= 0) {
    RineInput = Layout(static_cast<uint16_t>(RineState.InputText),
                       glm::vec2(RineMargin, 0.0f), RineFontSize, bubbleText);
    SquishText(RineInput, ScreenSize.x - 2.0f * RineMargin, RineMargin);
  }

  RineReplies.clear();
  int sender = 0;
  std::vector<RineReplyOption> options;
  if (RineState.ReadReplyMenu(sender, RineReplyKind, options)) {
    for (size_t i = 0; i < options.size(); i++) {
      RineReply reply;
      if (RineReplyKind == 1) {
        reply.Sticker = options[i].Value;
      } else {
        reply.Text =
            Layout(static_cast<uint16_t>(options[i].Value),
                   glm::vec2(RineMargin, i * RineReplyRowHeight), RineFontSize);
        SquishText(reply.Text, ScreenSize.x - 2.0f * RineMargin, RineMargin);
      }
      RineReplies.push_back(std::move(reply));
    }
  }
  RineReplyCursor =
      std::clamp(RineReplyCursor, 0,
                 std::max(0, static_cast<int>(RineReplies.size()) - 1));
}

float Phone::RineReplyAreaHeight() const {
  if (SessionOpen && !RineReplies.empty()) {
    return RineReplyKind == 1
               ? RineReplyStickerSize.y + 2.0f * RineGap
               : RineReplies.size() * RineReplyRowHeight + 2.0f * RineGap;
  }
  return RineInput.empty() ? 0.0f : RineInputHeight;
}

void Phone::UpdateRineInput() {
  const int count = static_cast<int>(RineReplies.size());
  if (count > 0 && RineState.MenuPending) {
    int next = RineReplyCursor;
    if (PADinputButtonWentDown & (PAD1UP | PAD1LEFT)) {
      next = (RineReplyCursor + count - 1) % count;
    }
    if (PADinputButtonWentDown & (PAD1DOWN | PAD1RIGHT)) {
      next = (RineReplyCursor + 1) % count;
    }
    if (next != RineReplyCursor) {
      RineReplyCursor = next;
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
    }
    if (PADinputButtonWentDown & PAD1A) {
      RineState.Reply(RineReplyCursor);
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
      return;
    }
  }
  if (PADinputButtonWentDown & (PAD1B | PAD1L2 | PAD1R2) ||
      Input::MouseButtonWentDown[SDL_BUTTON_RIGHT]) {
    Back();
  }
}

void Phone::RenderRine(glm::vec2 screen) {
  Fonts::Font& font = *Profile::Dialogue::DialogueFont;
  font.DrawProcessedText(RineTitle, 1.0f, 1.0f, RendererOutlineMode::Full,
                         screen);

  const float reserved = RineReplyAreaHeight();
  const float areaHeight = ScreenSize.y - RineTop - reserved;
  const float scroll = std::max(0.0f, RineContentHeight - areaHeight);
  const glm::vec2 origin(screen.x, screen.y + RineTop - scroll);

  Renderer->EnableScissor();
  Renderer->SetScissorRect(
      RectF(screen.x, screen.y + RineTop, ScreenSize.x, areaHeight));
  for (size_t i = 0; i < RineBubbles.size(); i++) {
    const RineBubble& bubble = RineBubbles[i];
    const RectF box = bubble.Box + origin;
    if (bubble.Avatar >= 0) {
      Renderer->DrawSprite(
          AvatarFor(bubble.Avatar),
          RectF(origin.x + RineMargin, box.Y, RineAvatarSize, RineAvatarSize));
    }
    if (bubble.Sticker >= 0) {
      Renderer->DrawSprite(StickerFor(bubble.Sticker), box);
      continue;
    }
    const bool own = RineState.Messages[i].Sender == RineState.Owner;
    Renderer->DrawQuad(box, Rgba(own ? RineOwnColor : RineOtherColor));
    DrawTextAt(bubble.Text, origin, 1.0f, RendererOutlineMode::None);
  }
  Renderer->DisableScissor();

  if (reserved <= 0.0f) return;
  const float top = screen.y + ScreenSize.y - reserved;
  Renderer->DrawQuad(RectF(screen.x, top, ScreenSize.x, reserved),
                     glm::vec4(0.0f, 0.0f, 0.0f, 0.55f));

  if (SessionOpen && !RineReplies.empty()) {
    for (int i = 0; i < static_cast<int>(RineReplies.size()); i++) {
      const RineReply& reply = RineReplies[i];
      if (RineReplyKind == 1) {
        const float x =
            screen.x + RineMargin + i * (RineReplyStickerSize.x + RineMargin);
        const RectF box(x, top + RineGap, RineReplyStickerSize.x,
                        RineReplyStickerSize.y);
        if (i == RineReplyCursor) {
          Renderer->DrawQuad(box, glm::vec4(1.0f, 0.65f, 0.25f, 0.45f));
        }
        Renderer->DrawSprite(StickerFor(reply.Sticker), box);
        continue;
      }
      const float rowY = top + RineGap + i * RineReplyRowHeight;
      if (i == RineReplyCursor) {
        Renderer->DrawSprite(SelectionSprite,
                             RectF(screen.x, rowY - RowPadding / 2.0f,
                                   ScreenSize.x, RineReplyRowHeight));
      }
      font.DrawProcessedText(reply.Text, 1.0f, 1.0f, RendererOutlineMode::Full,
                             glm::vec2(screen.x, top + RineGap));
    }
    return;
  }

  Renderer->DrawQuad(RectF(screen.x + RineMargin / 2.0f, top + 4.0f,
                           ScreenSize.x - RineMargin, reserved - 8.0f),
                     Rgba(RineOtherColor));
  font.DrawProcessedText(
      RineInput, 1.0f, 1.0f, RendererOutlineMode::None,
      glm::vec2(screen.x, top + (reserved - RineFontSize) / 2.0f));
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
