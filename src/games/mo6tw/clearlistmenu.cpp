#include "clearlistmenu.h"
#include <fmt/format.h>
#include <string>

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/mo6tw/clearlistmenu.h"
#include "../../profile/ui/backlogmenu.h"
#include "../../profile/dialogue.h"
#include "../../renderer/renderer.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/vm.h"
#include "../../vm/interface/input.h"
#include "../../data/savesystem.h"
#include "../../ui/widgets/mo6tw/scenelistentry.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::ClearListMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::Vm::Interface;

using namespace Impacto::UI::Widgets;

void ClearListMenu::ArrowLeftOnClick(Button* target) { MainItems->Previous(); }

void ClearListMenu::ArrowRightOnClick(Button* target) { MainItems->Next(); }

ClearListMenu::ClearListMenu() {
  FadeAnimation.Direction = AnimationDirection::In;
  FadeAnimation.LoopMode = AnimationLoopMode::Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  ArrowsAnimation.Direction = AnimationDirection::In;
  ArrowsAnimation.LoopMode = AnimationLoopMode::ReverseDirection;
  ArrowsAnimation.SetDuration(ArrowsAnimationDuration);
  ArrowsAnimation.StartIn();

  MainItems = new Carousel(
      CDIR_HORIZONTAL,
      [this](auto* curPage, auto* nextPage) {
        return OnAdvancePage(curPage, nextPage);
      },
      [this](auto* curPage, auto* nextPage) {
        return OnGoBackPage(curPage, nextPage);
      });

  Arrows = new Group(this);
  Arrows->FocusLock = false;
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  auto arrowLeft =
      new Button(0, ArrowLeft, ArrowLeft, nullSprite, ArrowLeftPosition);
  arrowLeft->OnClickHandler = [this](auto* btn) {
    return ArrowLeftOnClick(btn);
  };
  auto arrowRight =
      new Button(1, ArrowRight, ArrowRight, nullSprite, ArrowRightPosition);
  arrowRight->OnClickHandler = [this](auto* btn) {
    return ArrowRightOnClick(btn);
  };

  Arrows->Add(arrowLeft);
  Arrows->Add(arrowRight);
  Arrows->VisibilityState = Shown;
}

void ClearListMenu::Show() {
  if (!IsInit) {
    InitMainPage();
    InitSceneTitlePage();
    InitEndingListPage();
    IsInit = true;
  }

  if (State != Shown) {
    State = Showing;
    FadeAnimation.StartIn();
    MainItems->Show();
    UpdateEndingCount();
    UpdateSceneCount();
    UpdateAlbumCount();
    UpdateCompletionPercentage();
    UpdateEndingList();
    UpdateSceneList();
    if (UI::FocusedMenu != 0) {
      LastFocusedMenu = UI::FocusedMenu;
      LastFocusedMenu->IsFocused = false;
    }
    IsFocused = true;
    UI::FocusedMenu = this;
  }
}
void ClearListMenu::Hide() {
  if (State != Hidden) {
    State = Hiding;
    FadeAnimation.StartOut();
    MainItems->Hide();
    if (LastFocusedMenu != 0) {
      UI::FocusedMenu = LastFocusedMenu;
      LastFocusedMenu->IsFocused = true;
    } else {
      UI::FocusedMenu = 0;
    }
    IsFocused = false;
  }
}

void ClearListMenu::UpdateInput(float dt) {
  Menu::UpdateInput(dt);
  if (State == Shown) {
    MainItems->UpdateInput(dt);
    Arrows->UpdateInput(dt);
    if ((PADinputButtonWentDown & PAD1DOWN ||
         PADinputButtonWentDown & PAD1UP) &&
        SceneTitleItems->HasFocus) {
      auto focusedEl = CurrentlyFocusedElement;
      if (focusedEl->Bounds.Y < SceneTitleItems->RenderingBounds.Y) {
        SceneListY += SceneListTextMargin.y;
      } else if (focusedEl->Bounds.Y + focusedEl->Bounds.Height >
                 SceneTitleItems->RenderingBounds.Y +
                     SceneTitleItems->RenderingBounds.Height) {
        SceneListY -= SceneListTextMargin.y;
      }
    }
  }
}

void ClearListMenu::Update(float dt) {
  UpdateInput(dt);

  FadeAnimation.Update(dt);
  if (ScrWork[SW_PLAYDATA_ALPHA] < 256 && State == Shown) {
    Hide();
  } else if (ScrWork[SW_PLAYDATA_ALPHA] == 256 && State == Hidden) {
    Show();
  }

  if (ScrWork[SW_PLAYDATA_ALPHA] == 256 && FadeAnimation.IsIn())
    State = Shown;
  else if (FadeAnimation.IsOut()) {
    State = Hidden;
  }

  if (State != Hidden) {
    ArrowsAnimation.Update(dt);
    UpdatePlayTime();
    MainItems->Update(dt);
    Arrows->Update(dt);
    SceneTitleItems->MoveTo(glm::vec2(SceneTitleItems->Bounds.X, SceneListY));

    if (PreviousPage && PreviousPage->MoveAnimation.IsIn()) {
      PreviousPage->Hide();
    }
  }
}

void ClearListMenu::Render() {
  if (State == Hidden) return;

  glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
  Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
  MainItems->Tint = col;
  MainItems->Render();
  Arrows->Tint =
      glm::vec4(1.0f, 1.0f, 1.0f, glm::step(0.5f, ArrowsAnimation.Progress));
  Arrows->Render();
}

void ClearListMenu::InitMainPage() {
  MainPage = new Group(this);
  MainPage->FocusLock = false;

  MainPage->Add(new Label(ClearListLabel, LabelPosition));
  MainPage->Add(new Label(EndingsLabel, EndingsLabelPosition));
  MainPage->Add(new Label(ClearListTextBackground,
                          EndingsLabelPosition + ClearListTextBGOffset));
  MainPage->Add(new Label(ScenesLabel, ScenesLabelPosition));
  MainPage->Add(new Label(ClearListTextBackground,
                          ScenesLabelPosition + ClearListTextBGOffset));
  MainPage->Add(new Label(CompletionLabel, CompletionLabelPosition));
  MainPage->Add(new Label(ClearListTextBackground,
                          CompletionLabelPosition + ClearListTextBGOffset));
  MainPage->Add(new Label(AlbumLabel, AlbumLabelPosition));
  MainPage->Add(new Label(ClearListTextBackground,
                          AlbumLabelPosition + ClearListTextBGOffset));
  MainPage->Add(new Label(PlayTimeLabel, PlayTimeLabelPosition));
  MainPage->Add(new Label(ClearListTextBackground,
                          PlayTimeLabelPosition + ClearListTextBGOffset));

  Vm::Sc3VmThread dummy;
  uint16_t sc3StringBuffer[10];
  Vm::Sc3Stream sc3StrStream(sc3StringBuffer);

  auto separator =
      Vm::ScriptGetTextTableStrAddress(SeparatorTable, SeparatorEntry);
  dummy.IpOffset = separator.IpOffset;
  dummy.ScriptBufferId = separator.ScriptBufferId;
  SeparatorWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);

  // Ending count
  TextGetSc3String(fmt::to_string(EndingCount), sc3StringBuffer);
  EndingCountWidth = TextGetPlainLineWidth(
      sc3StrStream, Profile::Dialogue::DialogueFont, FontSize);
  sc3StrStream = Vm::Sc3Stream(sc3StringBuffer);
  MainPage->Add(new Label(
      sc3StrStream,
      glm::vec2(
          (EndingsLabelPosition.x - EndingCountWidth) + EndingCountPosition.x,
          EndingCountPosition.y),
      FontSize, RendererOutlineMode::None, ClearListColorIndex));

  MainPage->Add(new Label(
      separator,
      glm::vec2((EndingsLabelPosition.x - (EndingCountWidth + SeparatorWidth)) +
                    EndingCountPosition.x,
                EndingCountPosition.y),
      FontSize, RendererOutlineMode::None, ClearListColorIndex));

  UnlockedEndingCount = new Label();
  MainPage->Add(UnlockedEndingCount);

  // Scene count
  TextGetSc3String(fmt::to_string(SceneCount), sc3StringBuffer);
  SceneCountWidth = TextGetPlainLineWidth(
      sc3StrStream, Profile::Dialogue::DialogueFont, FontSize);
  sc3StrStream = Vm::Sc3Stream(sc3StringBuffer);
  MainPage->Add(new Label(sc3StrStream,
                          glm::vec2((ScenesLabelPosition.x - SceneCountWidth) +
                                        SceneCountPosition.x,
                                    SceneCountPosition.y),
                          FontSize, RendererOutlineMode::None,
                          ClearListColorIndex));
  MainPage->Add(new Label(
      separator,
      glm::vec2((ScenesLabelPosition.x - (SceneCountWidth + SeparatorWidth)) +
                    SceneCountPosition.x,
                SceneCountPosition.y),
      FontSize, RendererOutlineMode::None, ClearListColorIndex));

  UnlockedSceneCount = new Label();
  MainPage->Add(UnlockedSceneCount);

  // Completion percentage
  CompletionPercentage = new Label();
  MainPage->Add(CompletionPercentage);

  // Album count
  int totalCount = 0, unlockedCount = 0;
  SaveSystem::GetViewedEVsCount(&totalCount, &unlockedCount);
  TextGetSc3String(fmt::to_string(totalCount), sc3StringBuffer);
  sc3StrStream = Vm::Sc3Stream(sc3StringBuffer);
  AlbumCountWidth = TextGetPlainLineWidth(
      sc3StrStream, Profile::Dialogue::DialogueFont, FontSize);
  sc3StrStream = Vm::Sc3Stream(sc3StringBuffer);
  MainPage->Add(new Label(
      sc3StrStream,
      glm::vec2((AlbumLabelPosition.x - AlbumCountWidth) + AlbumCountPosition.x,
                AlbumCountPosition.y),
      FontSize, RendererOutlineMode::None, ClearListColorIndex));

  MainPage->Add(new Label(
      separator,
      glm::vec2((AlbumLabelPosition.x - (AlbumCountWidth + SeparatorWidth)) +
                    AlbumCountPosition.x,
                AlbumCountPosition.y),
      FontSize, RendererOutlineMode::None, ClearListColorIndex));

  UnlockedAlbumCount = new Label();
  MainPage->Add(UnlockedAlbumCount);

  // Play time
  auto secondsText = Vm::ScriptGetTextTableStrAddress(PlayTimeTextTable,
                                                      PlayTimeSecondsTextEntry);
  dummy.SetIp(secondsText);
  SecondsTextWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  MainPage->Add(new Label(secondsText,
                          glm::vec2(PlayTimeLabelPosition.x - SecondsTextWidth +
                                        PlayTimeSecondsTextPosition.x,
                                    PlayTimeSecondsTextPosition.y),
                          FontSize, RendererOutlineMode::None,
                          ClearListColorIndex));

  auto minutesText = Vm::ScriptGetTextTableStrAddress(PlayTimeTextTable,
                                                      PlayTimeMinutesTextEntry);
  dummy.SetIp(minutesText);
  MinutesTextWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  MainPage->Add(
      new Label(minutesText,
                glm::vec2(PlayTimeLabelPosition.x - SecondsTextWidth -
                              MinutesTextWidth + PlayTimeMinutesTextPosition.x,
                          PlayTimeMinutesTextPosition.y),
                FontSize, RendererOutlineMode::None, ClearListColorIndex));

  auto hoursText = Vm::ScriptGetTextTableStrAddress(PlayTimeTextTable,
                                                    PlayTimeHoursTextEntry);
  dummy.SetIp(hoursText);
  HoursTextWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  HoursText = new Label(
      hoursText,
      glm::vec2(PlayTimeLabelPosition.x - SecondsTextWidth - MinutesTextWidth -
                    HoursTextWidth + PlayTimeHoursTextPosition.x,
                PlayTimeHoursTextPosition.y),
      FontSize, RendererOutlineMode::None, ClearListColorIndex);
  HoursText->Tint.a = 0.0f;
  MainPage->Add(HoursText);
  PlaySeconds = new Label();
  MainPage->Add(PlaySeconds);
  PlayMinutes = new Label();
  MainPage->Add(PlayMinutes);
  PlayHours = new Label();
  PlayHours->Tint.a = 0.0f;
  MainPage->Add(PlayHours);

  UpdatePlayTime();

  MainPage->Show();

  MainItems->Add(MainPage);
}

void ClearListMenu::UpdateEndingCount() {
  uint16_t sc3StringBuffer[10];
  Vm::Sc3Stream stream(sc3StringBuffer);

  int unlockedEndingCount = 0;
  for (int i = 0; i < EndingCount; i++) {
    unlockedEndingCount += GetFlag(SF_CLR_END1 + i);
  }
  TextGetSc3String(fmt::format("{:2}", unlockedEndingCount), sc3StringBuffer);
  float unlockedEndingCountWidth =
      TextGetPlainLineWidth(stream, Profile::Dialogue::DialogueFont, FontSize);
  stream = Vm::Sc3Stream(sc3StringBuffer);
  UnlockedEndingCount->Bounds.X =
      (EndingsLabelPosition.x -
       (EndingCountWidth + SeparatorWidth + unlockedEndingCountWidth)) +
      EndingCountPosition.x;
  UnlockedEndingCount->Bounds.Y = EndingCountPosition.y;
  UnlockedEndingCount->SetText(stream, FontSize, RendererOutlineMode::None,
                               ClearListColorIndex);
}

void ClearListMenu::UpdateSceneCount() {
  uint16_t sc3StringBuffer[10];
  Vm::Sc3Stream stream(sc3StringBuffer);
  int unlockedSceneCount = 0;
  for (int i = 0; i < SceneCount; ++i) {
    unlockedSceneCount += GetFlag(SF_SCN_CLR1 + i);
  }
  TextGetSc3String(fmt::to_string(unlockedSceneCount), sc3StringBuffer);
  float unlockedSceneCountWidth =
      TextGetPlainLineWidth(stream, Profile::Dialogue::DialogueFont, FontSize);
  stream = Vm::Sc3Stream(sc3StringBuffer);
  UnlockedSceneCount->Bounds.X =
      (ScenesLabelPosition.x -
       (SceneCountWidth + SeparatorWidth + unlockedSceneCountWidth)) +
      SceneCountPosition.x;
  UnlockedSceneCount->Bounds.Y = SceneCountPosition.y;
  UnlockedSceneCount->SetText(stream, FontSize, RendererOutlineMode::None,
                              ClearListColorIndex);
}

void ClearListMenu::UpdateAlbumCount() {
  uint16_t sc3StringBuffer[10];
  Vm::Sc3Stream stream(sc3StringBuffer);

  int totalCount = 0, unlockedCount = 0;
  SaveSystem::GetViewedEVsCount(&totalCount, &unlockedCount);
  TextGetSc3String(fmt::to_string(unlockedCount), sc3StringBuffer);
  float unlockedAlbumCountWidth =
      TextGetPlainLineWidth(stream, Profile::Dialogue::DialogueFont, FontSize);
  stream = Vm::Sc3Stream(sc3StringBuffer);
  UnlockedAlbumCount->Bounds.X =
      (AlbumLabelPosition.x -
       (AlbumCountWidth + SeparatorWidth + unlockedAlbumCountWidth)) +
      AlbumCountPosition.x;
  UnlockedAlbumCount->Bounds.Y = AlbumCountPosition.y;
  UnlockedAlbumCount->SetText(stream, FontSize, RendererOutlineMode::None,
                              ClearListColorIndex);
}

void ClearListMenu::UpdateCompletionPercentage() {
  uint16_t sc3StringBuffer[10];
  Vm::Sc3Stream stream(sc3StringBuffer);

  int totalMessageCount = 0, readMessageCount = 0;

  SaveSystem::GetReadMessagesCount(&totalMessageCount, &readMessageCount);
  float readPercentage = readMessageCount / (float)totalMessageCount * 100.0f;
  TextGetSc3String(fmt::format("{:.2f}%", readPercentage), sc3StringBuffer);
  float percentageWidth =
      TextGetPlainLineWidth(stream, Profile::Dialogue::DialogueFont, FontSize);
  stream = Vm::Sc3Stream(sc3StringBuffer);

  CompletionPercentage->Bounds.X =
      CompletionLabelPosition.x - percentageWidth + CompletionPosition.x;
  CompletionPercentage->Bounds.Y = CompletionPosition.y;
  CompletionPercentage->SetText(stream, FontSize, RendererOutlineMode::None,
                                ClearListColorIndex);
}

void ClearListMenu::UpdatePlayTime() {
  auto seconds = ScrWork[SW_TOTALPLAYTIME] % 3600 % 60;
  auto minutes = ScrWork[SW_TOTALPLAYTIME] % 3600 / 60;
  auto hours = ScrWork[SW_TOTALPLAYTIME] / 3600;

  uint16_t sc3StringBuffer[10];
  Vm::Sc3Stream stream(sc3StringBuffer);

  TextGetSc3String(fmt::format("{:2d}", seconds), sc3StringBuffer);
  float secondsWidth =
      TextGetPlainLineWidth(stream, Profile::Dialogue::DialogueFont, FontSize);
  stream = Vm::Sc3Stream(sc3StringBuffer);
  PlaySeconds->Bounds.X = PlayTimeLabelPosition.x -
                          (SecondsTextWidth + secondsWidth) +
                          PlayTimeSecondsPosition.x;
  PlaySeconds->Bounds.Y = PlayTimeSecondsPosition.y;
  PlaySeconds->SetText(stream, FontSize, RendererOutlineMode::None,
                       ClearListColorIndex);
  TextGetSc3String(fmt::format("{:2d}", minutes), sc3StringBuffer);
  stream = Vm::Sc3Stream(sc3StringBuffer);
  float minutesWidth =
      TextGetPlainLineWidth(stream, Profile::Dialogue::DialogueFont, FontSize);
  stream = Vm::Sc3Stream(sc3StringBuffer);
  PlayMinutes->Bounds.X = PlayTimeLabelPosition.x -
                          (SecondsTextWidth + MinutesTextWidth + minutesWidth) +
                          PlayTimeMinutesPosition.x;
  PlayMinutes->Bounds.Y = PlayTimeMinutesPosition.y;
  PlayMinutes->SetText(stream, FontSize, RendererOutlineMode::None,
                       ClearListColorIndex);

  if (hours != 0) {
    HoursText->Tint.a = FadeAnimation.Progress;
    PlayHours->Tint.a = FadeAnimation.Progress;
    TextGetSc3String(fmt::format("{:2d}", hours), sc3StringBuffer);
    stream = Vm::Sc3Stream(sc3StringBuffer);
    float hoursWidth = TextGetPlainLineWidth(
        stream, Profile::Dialogue::DialogueFont, FontSize);
    stream = Vm::Sc3Stream(sc3StringBuffer);
    PlayHours->Bounds.X = PlayTimeLabelPosition.x - SecondsTextWidth -
                          MinutesTextWidth - (HoursTextWidth + hoursWidth) +
                          PlayTimeHoursPosition.x;
    PlayHours->Bounds.Y = PlayTimeHoursPosition.y;
    PlayHours->SetText(stream, FontSize, RendererOutlineMode::None,
                       ClearListColorIndex);
  } else {
    PlayHours->Tint.a = 0.0f;
    HoursText->Tint.a = 0.0f;
  }
}

void ClearListMenu::InitEndingListPage() {
  EndingListPage = new Group(this);
  EndingListPage->FocusLock = false;
  EndingNames = new Widget*[EndingCount * 2];

  EndingListPage->Add(new Label(EndingListLabel, LabelPosition));
  EndingListPage->Add(new Label(WindowSprite, WindowPosition));

  auto numberLabelPos = EndingsListNumberInitialPosition;
  auto textLabelPos = EndingsListTextInitialPosition;
  int idx = 0;
  auto lockedText = Vm::ScriptGetTextTableStrAddress(
      EndingsListTextLockedTable, EndingsListTextLockedEntry);
  for (int i = 0; i < EndingCount; i++) {
    auto numberLabel = new Label(
        fmt::format("{:2d}", i + 1), numberLabelPos, EndingsListTextFontSize,
        RendererOutlineMode::None, EndingsListTextColorIndex);
    numberLabelPos += EndingsListTextMargin;

    auto lockedLabel =
        new Label(lockedText, textLabelPos, EndingsListTextFontSize,
                  RendererOutlineMode::None, EndingsListTextColorIndex);
    auto unlockedLabel =
        new Label(Vm::ScriptGetTextTableStrAddress(EndingsListTextTable, i),
                  textLabelPos, EndingsListTextFontSize,
                  RendererOutlineMode::None, EndingsListTextColorIndex);
    textLabelPos += EndingsListTextMargin;

    EndingNames[idx] = unlockedLabel;
    idx += 1;
    EndingNames[idx] = lockedLabel;
    idx += 1;

    EndingListPage->Add(numberLabel);
    EndingListPage->Add(lockedLabel);
    EndingListPage->Add(unlockedLabel);
  }

  MainItems->Add(EndingListPage);
}

void ClearListMenu::UpdateEndingList() {
  int idx = 0;
  for (int i = 0; i < EndingCount * 2; i += 2) {
    if (GetFlag(SF_CLR_END1 + idx)) {
      EndingNames[i]->Tint.a = 1.0f;
      EndingNames[i + 1]->Tint.a = 0.0f;
    } else {
      EndingNames[i + 1]->Tint.a = 1.0f;
      EndingNames[i]->Tint.a = 0.0f;
    }
    idx += 1;
  }
}

void ClearListMenu::InitSceneTitlePage() {
  SceneTitlePage = new Group(this);
  SceneTitlePage->FocusLock = false;
  SceneNames = new Widget*[SceneCount * 2];

  SceneTitlePage->Add(new Label(SceneTitleLabel, LabelPosition));
  SceneTitlePage->Add(new Label(WindowSpritePartLeft, WindowPosition));
  SceneTitlePage->Add(new Label(
      WindowSpritePartRight,
      WindowPosition + glm::vec2(WindowSpritePartLeft.ScaledWidth(), 0.0f)));

  SceneTitleItems = new Group(this);
  SceneTitleItems->FocusLock = false;

  auto numberLabelPos = SceneListNumberInitialPosition;
  auto textLabelPos = SceneListTextInitialPosition;

  auto lockedText = Vm::ScriptGetTextTableStrAddress(SceneTitleLockedTable,
                                                     SceneTitleLockedEntry);
  for (int i = 0; i < SceneCount; i++) {
    auto numberLabel = new Label(fmt::format("{:2d}", i + 1), numberLabelPos,
                                 SceneListFontSize, RendererOutlineMode::None,
                                 SceneListColorIndex);
    numberLabelPos += SceneListTextMargin;

    auto lockedLabel =
        new Label(lockedText, textLabelPos, SceneListFontSize,
                  RendererOutlineMode::None, SceneListColorIndex);
    auto unlockedLabel = new Label(
        Vm::ScriptGetTextTableStrAddress(SceneListTextTable, i), textLabelPos,
        SceneListFontSize, RendererOutlineMode::None, SceneListColorIndex);
    textLabelPos += SceneListTextMargin;
    SceneTitleItems->Add(new Widgets::MO6TW::SceneListEntry(
                             i, numberLabel, lockedLabel, unlockedLabel,
                             Profile::BacklogMenu::EntryHighlight, true),
                         FDIR_DOWN);
  }

  float totalHeight =
      (SceneListFontSize * SceneCount) +
      ((SceneListTextMargin.y - SceneListFontSize) * (SceneCount - 2));
  SceneTitleItems->RenderingBounds = SceneTitleItemsRenderingBounds;
  SceneTitleItems->HoverBounds = SceneTitleItemsRenderingBounds;
  SceneTitleItems->Bounds =
      RectF(SceneListNumberInitialPosition.x, SceneListNumberInitialPosition.y,
            SceneTitleItemsWidth, totalHeight);
  SceneTitlePage->Add(SceneTitleItems);

  SceneListY = ScrollbarStart;
  auto scrollbar = new Scrollbar(
      0, ScrollbarPosition, ScrollbarStart,
      ScrollbarStart - totalHeight + ScrollAreaHeight, &SceneListY,
      SBDIR_VERTICAL, ScrollbarTrack, ScrollbarThumb);
  SceneTitlePage->Add(scrollbar);

  MainItems->Add(SceneTitlePage);
}

void ClearListMenu::UpdateSceneList() {
  for (int i = 0; i < SceneCount; i++) {
    auto entry = static_cast<Widgets::MO6TW::SceneListEntry*>(
        SceneTitleItems->Children.at(i));
    entry->IsLocked = !GetFlag(SF_SCN_CLR1 + i);
  }
}

// TEST
void ClearListMenu::OnAdvancePage(Widget* currentPage, Widget* nextPage) {
  currentPage->Move(glm::vec2(750.0f, 0.0f), 0.4f);
  nextPage->MoveTo(glm::vec2(-750.0f, 0.0f));
  nextPage->Move(glm::vec2(750.0f, 0.0f), 0.4f);
  nextPage->Show();
  CurrentPage = nextPage;
  PreviousPage = currentPage;
}

// TEST
void ClearListMenu::OnGoBackPage(Widget* currentPage, Widget* nextPage) {
  currentPage->Move(glm::vec2(-750.0f, 0.0f), 0.4f);
  nextPage->MoveTo(glm::vec2(750.0f, 0.0f));
  nextPage->Move(glm::vec2(-750.0f, 0.0f), 0.4f);
  nextPage->Show();
  CurrentPage = nextPage;
  PreviousPage = currentPage;
}

}  // namespace MO6TW
}  // namespace UI
}  // namespace Impacto