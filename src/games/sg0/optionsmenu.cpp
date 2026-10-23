#include "optionsmenu.h"

#include <algorithm>
#include <array>
#include <cmath>

#include "../../profile/configsystem.h"
#include "../../profile/scriptvars.h"
#include "../../renderer/renderer.h"
#include "../../ui/ui.h"
#include "../../mem.h"
#include "../../inputsystem.h"
#include "../../vm/interface/input.h"
#include "../../audio/audiosystem.h"

namespace Impacto {
namespace UI {
namespace SG0 {

using namespace Impacto::Profile::SG0::OptionsMenu;
using namespace Impacto::Profile::ScriptVars;
using namespace Impacto::Profile::ConfigSystem;
using namespace Impacto::Vm::Interface;
using Setting = Impacto::Profile::SG0::OptionsMenu::Setting;

namespace {
constexpr int SubMenuNo = 5;
constexpr int SF_OPTION_CLOSED = 1830;
constexpr float SliderStep = 0.05f;
constexpr std::array<glm::ivec2, 3> Resolutions = {
    glm::ivec2(1024, 576), glm::ivec2(1280, 720), glm::ivec2(1920, 1080)};

float ToUnit(float value, glm::vec2 bounds) {
  return std::clamp((value - bounds.x) / (bounds.y - bounds.x), 0.0f, 1.0f);
}
float FromUnit(float unit, glm::vec2 bounds) {
  return bounds.x + (bounds.y - bounds.x) * unit;
}
}  // namespace

OptionsMenu::OptionsMenu() { InputConfig = InputRate::RepeatFast; }

std::vector<const OptionsMenu::Row*> OptionsMenu::PageRows(int page) const {
  std::vector<const Row*> rows;
  for (const Row& row : Rows) {
    if (row.Page == page) rows.push_back(&row);
  }
  return rows;
}

float OptionsMenu::RowY(const Row& row, int indexOnPage) const {
  const float height = row.Page == VoicePage ? VoiceRowHeight : RowHeight;
  return RowsTop + indexOnPage * height;
}

float OptionsMenu::SliderScale() const {
  return Page == VoicePage ? VoiceSliderScale : 1.0f;
}

RectF OptionsMenu::SliderRect(float y) const {
  const float scale = SliderScale();
  const float offsetY = Page == VoicePage ? VoiceSliderOffsetY : SliderOffset.y;
  return RectF(ContentX + SliderOffset.x, y + offsetY,
               SliderFrameSprite.ScaledWidth() * scale,
               SliderFrameSprite.ScaledHeight() * scale);
}

int OptionsMenu::GetChoice(const Row& row) const {
  switch (row.Kind) {
    case Setting::FullScreen:
      return SDL_GetWindowFlags(Window->SDLWindow) & SDL_WINDOW_FULLSCREEN ? 0
                                                                           : 1;
    case Setting::Resolution: {
      int best = 0;
      for (int i = 1; i < static_cast<int>(Resolutions.size()); i++) {
        if (std::abs(Resolutions[i].x - Window->WindowWidth) <
            std::abs(Resolutions[best].x - Window->WindowWidth)) {
          best = i;
        }
      }
      return best;
    }
    case Setting::TriggerStopSkip:
      return TriggerStopSkip ? 0 : 1;
    case Setting::ShowTipsNotification:
      return ShowTipsNotification ? 0 : 1;
    case Setting::AutoQuickSave: {
      const bool scene = AutoQuickSave & +AutoQuickSaveType::OnScene;
      const bool trigger = AutoQuickSave & +AutoQuickSaveType::OnTrigger;
      return scene && trigger ? 2 : scene ? 0 : trigger ? 1 : 3;
    }
    case Setting::SkipRead:
      return SkipRead ? 0 : 1;
    case Setting::SyncVoice:
      return SyncVoice ? 0 : 1;
    case Setting::SkipVoice:
      return SkipVoice ? 0 : 1;
    default:
      return 0;
  }
}

void OptionsMenu::SetChoice(const Row& row, int choice) {
  const bool on = choice == 0;
  switch (row.Kind) {
    case Setting::FullScreen:
      SDL_SetWindowFullscreen(Window->SDLWindow, on);
      break;
    case Setting::Resolution:
      if (!(SDL_GetWindowFlags(Window->SDLWindow) & SDL_WINDOW_FULLSCREEN)) {
        SDL_SetWindowSize(Window->SDLWindow, Resolutions[choice].x,
                          Resolutions[choice].y);
      }
      break;
    case Setting::TriggerStopSkip:
      TriggerStopSkip = on;
      break;
    case Setting::ShowTipsNotification:
      ShowTipsNotification = on;
      break;
    case Setting::AutoQuickSave: {
      const std::array<uint8_t, 4> values = {
          +AutoQuickSaveType::OnScene, +AutoQuickSaveType::OnTrigger,
          +AutoQuickSaveType::OnScene | +AutoQuickSaveType::OnTrigger, 0};
      AutoQuickSave = values[choice];
    } break;
    case Setting::SkipRead:
      SkipRead = on;
      break;
    case Setting::SyncVoice:
      SyncVoice = on;
      break;
    case Setting::SkipVoice:
      SkipVoice = on;
      break;
    default:
      break;
  }
}

float OptionsMenu::GetSlider(const Row& row) const {
  switch (row.Kind) {
    case Setting::TextSpeed:
      return ToUnit(TextSpeed, TextSpeedBounds);
    case Setting::AutoSpeed:
      return ToUnit(AutoSpeed, AutoSpeedBounds);
    case Setting::VoiceVolume:
      return Audio::GroupVolumes[Audio::ACG_Voice];
    case Setting::MusicVolume:
      return Audio::GroupVolumes[Audio::ACG_BGM];
    case Setting::SEVolume:
      return Audio::GroupVolumes[Audio::ACG_SE];
    case Setting::MovieVolume:
      return Audio::GroupVolumes[Audio::ACG_Movie];
    case Setting::CharacterVoice:
      return VoiceVolume[row.Index];
    default:
      return 0.0f;
  }
}

void OptionsMenu::SetSlider(const Row& row, float value) {
  value = std::clamp(value, 0.0f, 1.0f);
  switch (row.Kind) {
    case Setting::TextSpeed:
      TextSpeed = FromUnit(value, TextSpeedBounds);
      break;
    case Setting::AutoSpeed:
      AutoSpeed = FromUnit(value, AutoSpeedBounds);
      break;
    case Setting::VoiceVolume:
      Audio::GroupVolumes[Audio::ACG_Voice] = value;
      break;
    case Setting::MusicVolume:
      Audio::GroupVolumes[Audio::ACG_BGM] = value;
      break;
    case Setting::SEVolume:
      Audio::GroupVolumes[Audio::ACG_SE] = value;
      break;
    case Setting::MovieVolume:
      Audio::GroupVolumes[Audio::ACG_Movie] = value;
      break;
    case Setting::CharacterVoice:
      VoiceVolume[row.Index] = value;
      break;
    default:
      break;
  }
}

bool OptionsMenu::IsMuted(const Row& row) const {
  return row.Kind == Setting::CharacterVoice && VoiceMuted[row.Index];
}

void OptionsMenu::Show() {
  if (State == Shown || State == Showing) return;
  State = Showing;
  Cursor = 0;
  if (UI::FocusedMenu != nullptr && UI::FocusedMenu != this) {
    LastFocusedMenu = UI::FocusedMenu;
    LastFocusedMenu->IsFocused = false;
  }
  UI::FocusedMenu = this;
}

void OptionsMenu::Hide() {
  if (State == Hidden || State == Hiding) return;
  State = Hiding;
  IsFocused = false;
  if (LastFocusedMenu != nullptr) {
    UI::FocusedMenu = LastFocusedMenu;
    LastFocusedMenu->IsFocused = true;
    LastFocusedMenu = nullptr;
  } else {
    UI::FocusedMenu = nullptr;
  }
}

void OptionsMenu::UpdateInput(float dt) {
  if (!IsFocused || GetFlag(SF_OPTION_CLOSED)) return;
  const std::vector<const Row*> rows = PageRows(Page);
  const int count = static_cast<int>(rows.size());
  const glm::vec2 mouse = Input::CurMousePos;
  const bool click = Input::MouseButtonWentDown[SDL_BUTTON_LEFT];

  // Pages: Q / E, or click a page button.
  int nextPage = Page;
  if (PADinputButtonWentDown & PAD1R1) nextPage = (Page + 1) % PageCount;
  if (PADinputButtonWentDown & PAD1L1)
    nextPage = (Page + PageCount - 1) % PageCount;
  for (int page = 0; page < PageCount; page++) {
    if (click && PageButtonBounds[page].ContainsPoint(mouse)) nextPage = page;
  }
  if (nextPage != Page) {
    Page = nextPage;
    Cursor = 0;
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
    return;
  }
  if (count == 0) return;

  int next = Cursor;
  if (PADinputButtonRepeatAccelDown & PAD1UP)
    next = (Cursor + count - 1) % count;
  if (PADinputButtonRepeatAccelDown & PAD1DOWN) next = (Cursor + 1) % count;
  for (int i = 0; i < count; i++) {
    const float y = RowY(*rows[i], i);
    const float height = Page == VoicePage ? VoiceRowHeight : RowHeight;
    const RectF bounds(ContentX - RowPadding, y - RowPadding, RowWidth, height);
    if (bounds.ContainsPoint(mouse) && Input::PrevMousePos != mouse) next = i;
  }
  if (next != Cursor) {
    Cursor = next;
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
  }

  const Row& row = *rows[Cursor];
  const float y = RowY(row, Cursor);
  const int dir = (PADinputButtonRepeatAccelDown & PAD1RIGHT)  ? 1
                  : (PADinputButtonRepeatAccelDown & PAD1LEFT) ? -1
                                                               : 0;
  if (!row.Choices.empty()) {
    const int choices = static_cast<int>(row.Choices.size());
    int choice = GetChoice(row);
    int picked = std::clamp(choice + dir, 0, choices - 1);
    if (PADinputButtonWentDown & PAD1A) picked = (choice + 1) % choices;
    for (int i = 0; i < choices && click; i++) {
      const Sprite& word = row.Choices[i];
      const RectF bounds(ContentX + word.Bounds.X - row.Label.Bounds.X,
                         y + word.Bounds.Y - row.Label.Bounds.Y,
                         word.ScaledWidth(), word.ScaledHeight());
      if (bounds.ContainsPoint(mouse)) picked = i;
    }
    if (picked != choice) {
      SetChoice(row, picked);
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
    }
  } else {
    float value = GetSlider(row) + dir * SliderStep;
    const RectF slider = SliderRect(y);
    if (Input::MouseButtonIsDown[SDL_BUTTON_LEFT] &&
        slider.ContainsPoint(mouse)) {
      value = (mouse.x - slider.X - SliderFillOffset.x * SliderScale()) /
              (SliderFillSprite.ScaledWidth() * SliderScale());
    }
    if (std::abs(value - GetSlider(row)) > 1e-4f) {
      SetSlider(row, value);
      if (dir != 0) Audio::PlayInGroup(Audio::ACG_SE, "sysse", 1, false, 0);
    }
    if (row.Kind == Setting::CharacterVoice &&
        PADinputButtonWentDown & (PAD1A | PAD1Y)) {
      VoiceMuted[row.Index] = !VoiceMuted[row.Index];
      Audio::PlayInGroup(Audio::ACG_SE, "sysse", 2, false, 0);
    }
  }

  if (PADinputButtonWentDown & PAD1B ||
      Input::MouseButtonWentDown[SDL_BUTTON_RIGHT]) {
    SetFlag(SF_OPTION_CLOSED, true);
    Audio::PlayInGroup(Audio::ACG_SE, "sysse", 3, false, 0);
  }
}

void OptionsMenu::Update(float dt) {
  const int count =
      ScrWork[SW_SYSSUBMENUNO] == SubMenuNo ? ScrWork[SW_SYSSUBMENUCT] : 0;
  if (count > LastSubMenuCount && count > 0) {
    Show();
  } else if (count < LastSubMenuCount) {
    Hide();
  }
  LastSubMenuCount = count;

  if (State == Showing && count >= CountMax) {
    State = Shown;
    IsFocused = UI::FocusedMenu == this;
  } else if (State == Hiding && count <= 0) {
    State = Hidden;
  }
  if (State == Shown) UpdateInput(dt);
}

void OptionsMenu::Render() {
  if (State == Hidden) return;
  const float alpha =
      ScrWork[SW_SYSSUBMENUNO] == SubMenuNo
          ? std::clamp(ScrWork[SW_SYSSUBMENUCT] / float(CountMax), 0.0f, 1.0f)
          : 0.0f;
  const glm::vec4 tint(1.0f, 1.0f, 1.0f, alpha);
  const glm::vec4 dim(glm::vec3(UnselectedBrightness), alpha);

  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f), tint);
  Renderer->DrawSprite(PageButtonSprites[Page], PageButtonBounds[Page].GetPos(),
                       tint);
  Renderer->DrawSprite(PageHeadingSprites[Page], HeadingPos, tint);

  const std::vector<const Row*> rows = PageRows(Page);
  for (int i = 0; i < static_cast<int>(rows.size()); i++) {
    const Row& row = *rows[i];
    const float y = RowY(row, i);
    if (i == Cursor && State == Shown) {
      const float height = Page == VoicePage ? VoiceRowHeight : RowHeight;
      Renderer->DrawQuad(
          RectF(ContentX - RowPadding, y - RowPadding, RowWidth, height),
          glm::vec4(HighlightColor, HighlightOpacity * alpha));
    }
    Renderer->DrawSprite(row.Label, glm::vec2(ContentX, y), tint);

    if (!row.Choices.empty()) {
      const int selected = GetChoice(row);
      for (int c = 0; c < static_cast<int>(row.Choices.size()); c++) {
        const Sprite& word = row.Choices[c];
        Renderer->DrawSprite(
            word,
            glm::vec2(ContentX + word.Bounds.X - row.Label.Bounds.X,
                      y + word.Bounds.Y - row.Label.Bounds.Y),
            c == selected ? tint : dim);
      }
      continue;
    }

    const float scale = SliderScale();
    const float value = GetSlider(row);
    const RectF frame = SliderRect(y);
    const glm::vec2 fillPos = frame.GetPos() + SliderFillOffset * scale;
    const glm::vec2 fillSize = glm::vec2(SliderFillSprite.ScaledWidth(),
                                         SliderFillSprite.ScaledHeight()) *
                               scale;
    Renderer->DrawSprite(SliderFrameSprite, frame, tint);
    Sprite fill = SliderFillSprite;
    fill.Bounds.Width *= value;
    Renderer->DrawSprite(
        fill, RectF(fillPos.x, fillPos.y, fillSize.x * value, fillSize.y),
        tint);
    const glm::vec2 knobSize = glm::vec2(SliderKnobSprite.ScaledWidth(),
                                         SliderKnobSprite.ScaledHeight()) *
                               scale;
    Renderer->DrawSprite(
        SliderKnobSprite,
        RectF(fillPos.x + fillSize.x * value - knobSize.x / 2.0f,
              frame.Y + SliderKnobOffsetY * scale, knobSize.x, knobSize.y),
        tint);
    if (IsMuted(row)) {
      Renderer->DrawSprite(VoiceOffSprite,
                           RectF(fillPos.x, fillPos.y, fillSize.x, fillSize.y),
                           tint);
    }
  }

  Renderer->DrawSprite(Page == VoicePage ? VoiceGuideSprite : GuideSprite,
                       Page == VoicePage ? VoiceGuidePos : GuidePos, tint);
}

}  // namespace SG0
}  // namespace UI
}  // namespace Impacto
