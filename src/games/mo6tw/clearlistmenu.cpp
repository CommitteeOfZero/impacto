#include "clearlistmenu.h"

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
  FadeAnimation.Direction = 1;
  FadeAnimation.LoopMode = ALM_Stop;
  FadeAnimation.DurationIn = FadeInDuration;
  FadeAnimation.DurationOut = FadeOutDuration;

  ArrowsAnimation.Direction = 1;
  ArrowsAnimation.LoopMode = ALM_ReverseDirection;
  ArrowsAnimation.DurationIn = ArrowsAnimationDuration;
  ArrowsAnimation.DurationOut = ArrowsAnimationDuration;
  ArrowsAnimation.StartIn();

  MainItems =
      new Carousel(CDIR_HORIZONTAL,
                   std::bind(&ClearListMenu::OnAdvancePage, this,
                             std::placeholders::_1, std::placeholders::_2),
                   std::bind(&ClearListMenu::OnGoBackPage, this,
                             std::placeholders::_1, std::placeholders::_2));

  Arrows = new Group(this);
  Arrows->FocusLock = false;
  Sprite nullSprite = Sprite();
  nullSprite.Bounds = RectF(0.0f, 0.0f, 0.0f, 0.0f);
  auto arrowLeft =
      new Button(0, ArrowLeft, ArrowLeft, nullSprite, ArrowLeftPosition);
  arrowLeft->OnClickHandler =
      std::bind(&ClearListMenu::ArrowLeftOnClick, this, std::placeholders::_1);
  auto arrowRight =
      new Button(1, ArrowRight, ArrowRight, nullSprite, ArrowRightPosition);
  arrowRight->OnClickHandler =
      std::bind(&ClearListMenu::ArrowRightOnClick, this, std::placeholders::_1);
  Arrows->Add(arrowLeft);
  Arrows->Add(arrowRight);
  Arrows->IsShown = true;
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

void ClearListMenu::UpdateInput() {
  Menu::UpdateInput();
  if (State == Shown) {
    MainItems->UpdateInput();
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
  UpdateInput();

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
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer->DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    MainItems->Tint = col;
    MainItems->Render();
    Arrows->Tint =
        glm::vec4(1.0f, 1.0f, 1.0f, glm::step(0.5f, ArrowsAnimation.Progress));
    Arrows->Render();
  }
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
  char temp[10];
  uint16_t sc3StringBuffer[10];

  auto separator =
      Vm::ScriptGetTextTableStrAddress(SeparatorTable, SeparatorEntry);
  dummy.Ip = separator;
  SeparatorWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);

  // Ending count
  sprintf(temp, "%d", EndingCount);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  EndingCountWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  MainPage->Add(new Label(
      (uint8_t*)sc3StringBuffer,
      glm::vec2(
          (EndingsLabelPosition.x - EndingCountWidth) + EndingCountPosition.x,
          EndingCountPosition.y),
      FontSize, false, ClearListColorIndex));

  MainPage->Add(new Label(
      separator,
      glm::vec2((EndingsLabelPosition.x - (EndingCountWidth + SeparatorWidth)) +
                    EndingCountPosition.x,
                EndingCountPosition.y),
      FontSize, false, ClearListColorIndex));

  UnlockedEndingCount = new Label();
  MainPage->Add(UnlockedEndingCount);

  // Scene count
  sprintf(temp, "%d", SceneCount);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  SceneCountWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  MainPage->Add(new Label((uint8_t*)sc3StringBuffer,
                          glm::vec2((ScenesLabelPosition.x - SceneCountWidth) +
                                        SceneCountPosition.x,
                                    SceneCountPosition.y),
                          FontSize, false, ClearListColorIndex));
  MainPage->Add(new Label(
      separator,
      glm::vec2((ScenesLabelPosition.x - (SceneCountWidth + SeparatorWidth)) +
                    SceneCountPosition.x,
                SceneCountPosition.y),
      FontSize, false, ClearListColorIndex));

  UnlockedSceneCount = new Label();
  MainPage->Add(UnlockedSceneCount);

  // Completion percentage
  CompletionPercentage = new Label();
  MainPage->Add(CompletionPercentage);

  // Album count
  int totalCount = 0, unlockedCount = 0;
  SaveSystem::GetViewedEVsCount(&totalCount, &unlockedCount);
  sprintf(temp, "%d", totalCount);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  AlbumCountWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  MainPage->Add(new Label(
      (uint8_t*)sc3StringBuffer,
      glm::vec2((AlbumLabelPosition.x - AlbumCountWidth) + AlbumCountPosition.x,
                AlbumCountPosition.y),
      FontSize, false, ClearListColorIndex));

  MainPage->Add(new Label(
      separator,
      glm::vec2((AlbumLabelPosition.x - (AlbumCountWidth + SeparatorWidth)) +
                    AlbumCountPosition.x,
                AlbumCountPosition.y),
      FontSize, false, ClearListColorIndex));

  UnlockedAlbumCount = new Label();
  MainPage->Add(UnlockedAlbumCount);

  // Play time
  auto secondsText = Vm::ScriptGetTextTableStrAddress(PlayTimeTextTable,
                                                      PlayTimeSecondsTextEntry);
  dummy.Ip = secondsText;
  SecondsTextWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  MainPage->Add(new Label(secondsText,
                          glm::vec2(PlayTimeLabelPosition.x - SecondsTextWidth +
                                        PlayTimeSecondsTextPosition.x,
                                    PlayTimeSecondsTextPosition.y),
                          FontSize, false, ClearListColorIndex));

  auto minutesText = Vm::ScriptGetTextTableStrAddress(PlayTimeTextTable,
                                                      PlayTimeMinutesTextEntry);
  dummy.Ip = minutesText;
  MinutesTextWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  MainPage->Add(
      new Label(minutesText,
                glm::vec2(PlayTimeLabelPosition.x - SecondsTextWidth -
                              MinutesTextWidth + PlayTimeMinutesTextPosition.x,
                          PlayTimeMinutesTextPosition.y),
                FontSize, false, ClearListColorIndex));

  auto hoursText = Vm::ScriptGetTextTableStrAddress(PlayTimeTextTable,
                                                    PlayTimeHoursTextEntry);
  dummy.Ip = hoursText;
  HoursTextWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  HoursText = new Label(
      hoursText,
      glm::vec2(PlayTimeLabelPosition.x - SecondsTextWidth - MinutesTextWidth -
                    HoursTextWidth + PlayTimeHoursTextPosition.x,
                PlayTimeHoursTextPosition.y),
      FontSize, false, ClearListColorIndex);
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
  Vm::Sc3VmThread dummy;
  char temp[10];
  uint16_t sc3StringBuffer[10];

  int unlockedEndingCount = 0;
  for (int i = 0; i < EndingCount; i++) {
    unlockedEndingCount += GetFlag(SF_CLR_END1 + i);
  }
  sprintf(temp, "%2d", unlockedEndingCount);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  float unlockedEndingCountWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  UnlockedEndingCount->Bounds.X =
      (EndingsLabelPosition.x -
       (EndingCountWidth + SeparatorWidth + unlockedEndingCountWidth)) +
      EndingCountPosition.x;
  UnlockedEndingCount->Bounds.Y = EndingCountPosition.y;
  UnlockedEndingCount->SetText((uint8_t*)sc3StringBuffer, FontSize, false,
                               ClearListColorIndex);
}

void ClearListMenu::UpdateSceneCount() {
  Vm::Sc3VmThread dummy;
  char temp[10];
  uint16_t sc3StringBuffer[10];

  int unlockedSceneCount = 0;
  for (int i = 0; i < SceneCount; ++i) {
    unlockedSceneCount += GetFlag(SF_SCN_CLR1 + i);
  }
  sprintf(temp, "%d", unlockedSceneCount);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  float unlockedSceneCountWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  UnlockedSceneCount->Bounds.X =
      (ScenesLabelPosition.x -
       (SceneCountWidth + SeparatorWidth + unlockedSceneCountWidth)) +
      SceneCountPosition.x;
  UnlockedSceneCount->Bounds.Y = SceneCountPosition.y;
  UnlockedSceneCount->SetText((uint8_t*)sc3StringBuffer, FontSize, false,
                              ClearListColorIndex);
}

void ClearListMenu::UpdateAlbumCount() {
  Vm::Sc3VmThread dummy;
  char temp[10];
  uint16_t sc3StringBuffer[10];

  int totalCount = 0, unlockedCount = 0;
  SaveSystem::GetViewedEVsCount(&totalCount, &unlockedCount);
  sprintf(temp, "%d", unlockedCount);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  float unlockedAlbumCountWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  UnlockedAlbumCount->Bounds.X =
      (AlbumLabelPosition.x -
       (AlbumCountWidth + SeparatorWidth + unlockedAlbumCountWidth)) +
      AlbumCountPosition.x;
  UnlockedAlbumCount->Bounds.Y = AlbumCountPosition.y;
  UnlockedAlbumCount->SetText((uint8_t*)sc3StringBuffer, FontSize, false,
                              ClearListColorIndex);
}

void ClearListMenu::UpdateCompletionPercentage() {
  Vm::Sc3VmThread dummy;
  char temp[10];
  uint16_t sc3StringBuffer[10];
  int totalMessageCount = 0, readMessageCount = 0;

  SaveSystem::GetReadMessagesCount(&totalMessageCount, &readMessageCount);
  float readPercentage = readMessageCount / (float)totalMessageCount * 100.0f;
  sprintf(temp, "%.2f%%", readPercentage);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  float percentageWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  CompletionPercentage->Bounds.X =
      CompletionLabelPosition.x - percentageWidth + CompletionPosition.x;
  CompletionPercentage->Bounds.Y = CompletionPosition.y;
  CompletionPercentage->SetText((uint8_t*)sc3StringBuffer, FontSize, false,
                                ClearListColorIndex);
}

void ClearListMenu::UpdatePlayTime() {
  Vm::Sc3VmThread dummy;
  auto seconds = ScrWork[SW_TOTALPLAYTIME] % 3600 % 60;
  auto minutes = ScrWork[SW_TOTALPLAYTIME] % 3600 / 60;
  auto hours = ScrWork[SW_TOTALPLAYTIME] / 3600;

  char temp[10];
  uint16_t sc3StringBuffer[10];

  sprintf(temp, "%2d", seconds);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  float secondsWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  PlaySeconds->Bounds.X = PlayTimeLabelPosition.x -
                          (SecondsTextWidth + secondsWidth) +
                          PlayTimeSecondsPosition.x;
  PlaySeconds->Bounds.Y = PlayTimeSecondsPosition.y;
  PlaySeconds->SetText((uint8_t*)sc3StringBuffer, FontSize, false,
                       ClearListColorIndex);

  sprintf(temp, "%2d", minutes);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  float minutesWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, FontSize);
  PlayMinutes->Bounds.X = PlayTimeLabelPosition.x -
                          (SecondsTextWidth + MinutesTextWidth + minutesWidth) +
                          PlayTimeMinutesPosition.x;
  PlayMinutes->Bounds.Y = PlayTimeMinutesPosition.y;
  PlayMinutes->SetText((uint8_t*)sc3StringBuffer, FontSize, false,
                       ClearListColorIndex);

  if (hours != 0) {
    HoursText->Tint.a = FadeAnimation.Progress;
    PlayHours->Tint.a = FadeAnimation.Progress;
    sprintf(temp, "%2d", hours);
    TextGetSc3String(std::string(temp), sc3StringBuffer);
    dummy.Ip = (uint8_t*)sc3StringBuffer;
    float hoursWidth = TextGetPlainLineWidth(
        &dummy, Profile::Dialogue::DialogueFont, FontSize);
    PlayHours->Bounds.X = PlayTimeLabelPosition.x - SecondsTextWidth -
                          MinutesTextWidth - (HoursTextWidth + hoursWidth) +
                          PlayTimeHoursPosition.x;
    PlayHours->Bounds.Y = PlayTimeHoursPosition.y;
    PlayHours->SetText((uint8_t*)sc3StringBuffer, FontSize, false,
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
  char temp[4];
  auto lockedText = Vm::ScriptGetTextTableStrAddress(
      EndingsListTextLockedTable, EndingsListTextLockedEntry);
  for (int i = 0; i < EndingCount; i++) {
    sprintf(temp, "%2d", i + 1);
    auto numberLabel =
        new Label(std::string(temp), numberLabelPos, EndingsListTextFontSize,
                  false, EndingsListTextColorIndex);
    numberLabelPos += EndingsListTextMargin;

    auto lockedLabel =
        new Label(lockedText, textLabelPos, EndingsListTextFontSize, false,
                  EndingsListTextColorIndex);
    auto unlockedLabel = new Label(
        Vm::ScriptGetTextTableStrAddress(EndingsListTextTable, i), textLabelPos,
        EndingsListTextFontSize, false, EndingsListTextColorIndex);
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

  int idx = 0;
  char temp[4];
  auto lockedText = Vm::ScriptGetTextTableStrAddress(SceneTitleLockedTable,
                                                     SceneTitleLockedEntry);
  for (int i = 0; i < SceneCount; i++) {
    sprintf(temp, "%2d", i + 1);
    auto numberLabel = new Label(std::string(temp), numberLabelPos,
                                 SceneListFontSize, false, SceneListColorIndex);
    numberLabelPos += SceneListTextMargin;

    auto lockedLabel = new Label(lockedText, textLabelPos, SceneListFontSize,
                                 false, SceneListColorIndex);
    auto unlockedLabel =
        new Label(Vm::ScriptGetTextTableStrAddress(SceneListTextTable, i),
                  textLabelPos, SceneListFontSize, false, SceneListColorIndex);
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