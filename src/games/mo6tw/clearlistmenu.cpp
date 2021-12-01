#include "clearlistmenu.h"

#include "../../profile/ui/extramenus.h"
#include "../../profile/games/mo6tw/clearlistmenu.h"
#include "../../profile/dialogue.h"
#include "../../renderer2d.h"
#include "../../mem.h"
#include "../../profile/scriptvars.h"
#include "../../vm/vm.h"

namespace Impacto {
namespace UI {
namespace MO6TW {

using namespace Impacto::Profile::MO6TW::ClearListMenu;
using namespace Impacto::Profile::ScriptVars;

using namespace Impacto::UI::Widgets;

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
                   std::bind(&ClearListMenu::AdvancePage, this,
                             std::placeholders::_1, std::placeholders::_2),
                   std::bind(&ClearListMenu::GoBackPage, this,
                             std::placeholders::_1, std::placeholders::_2));
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
  if (State == Shown) MainItems->UpdateInput();
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

    if (PreviousPage && PreviousPage->MoveAnimation.IsIn()) {
      PreviousPage->Hide();
    }
  }
}

void ClearListMenu::Render() {
  if (State != Hidden) {
    glm::vec4 col(1.0f, 1.0f, 1.0f, FadeAnimation.Progress);
    Renderer2D::DrawSprite(BackgroundSprite, glm::vec2(0.0f, 0.0f), col);
    MainItems->Tint = col;
    MainItems->Render();
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

  auto separator = Vm::ScriptGetTextTableStrAddress(0, 13);
  dummy.Ip = separator;
  SeparatorWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);

  // Ending count
  sprintf(temp, "%d", EndingCount);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  EndingCountWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  MainPage->Add(new Label(
      (uint8_t*)sc3StringBuffer,
      glm::vec2((64.0f - EndingCountWidth) + 560.0f, 172.0f), 32, false, 0));

  MainPage->Add(new Label(
      separator,
      glm::vec2((64.0f - (EndingCountWidth + SeparatorWidth)) + 560.0f, 172.0f),
      32, false, 0));

  UnlockedEndingCount = new Label();
  MainPage->Add(UnlockedEndingCount);

  // Scene count
  sprintf(temp, "%d", SceneCount);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  SceneCountWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  MainPage->Add(new Label(
      (uint8_t*)sc3StringBuffer,
      glm::vec2((128.0f - SceneCountWidth) + 560.0f, 284.0f), 32, false, 0));
  MainPage->Add(new Label(
      separator,
      glm::vec2((128.0f - (SceneCountWidth + SeparatorWidth)) + 560.0f, 284.0f),
      32, false, 0));

  UnlockedSceneCount = new Label();
  MainPage->Add(UnlockedSceneCount);

  // Play time
  auto secondsText = Vm::ScriptGetTextTableStrAddress(0, 9);
  dummy.Ip = secondsText;
  SecondsTextWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  MainPage->Add(new Label(
      secondsText,
      glm::vec2(PlayTimeLabelPosition.x - SecondsTextWidth + 562.0f, 620.0f),
      32, false, 0));

  auto minutesText = Vm::ScriptGetTextTableStrAddress(0, 8);
  dummy.Ip = minutesText;
  MinutesTextWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  MainPage->Add(new Label(minutesText,
                          glm::vec2(PlayTimeLabelPosition.x - SecondsTextWidth -
                                        MinutesTextWidth + 514.0f,
                                    620.0f),
                          32, false, 0));

  auto hoursText = Vm::ScriptGetTextTableStrAddress(0, 10);
  dummy.Ip = hoursText;
  HoursTextWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  HoursText =
      new Label(hoursText,
                glm::vec2(PlayTimeLabelPosition.x - SecondsTextWidth -
                              MinutesTextWidth - HoursTextWidth + 466.0f,
                          620.0f),
                32, false, 0);
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
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  UnlockedEndingCount->Bounds.X =
      (64.0f - (EndingCountWidth + SeparatorWidth + unlockedEndingCountWidth)) +
      558.0f;
  UnlockedEndingCount->Bounds.Y = 172.0f;
  UnlockedEndingCount->SetText((uint8_t*)sc3StringBuffer, 32, false, 0);
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
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  UnlockedSceneCount->Bounds.X =
      (128.0f - (SceneCountWidth + SeparatorWidth + unlockedSceneCountWidth)) +
      558.0f;
  UnlockedSceneCount->Bounds.Y = 284.0f;
  UnlockedSceneCount->SetText((uint8_t*)sc3StringBuffer, 32, false, 0);
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
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  PlaySeconds->Bounds.X =
      PlayTimeLabelPosition.x - (SecondsTextWidth + secondsWidth) + 560.0f;
  PlaySeconds->Bounds.Y = 620.0f;
  PlaySeconds->SetText((uint8_t*)sc3StringBuffer, 32, false, 0);

  sprintf(temp, "%2d", minutes);
  TextGetSc3String(std::string(temp), sc3StringBuffer);
  dummy.Ip = (uint8_t*)sc3StringBuffer;
  float minutesWidth =
      TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
  PlayMinutes->Bounds.X = PlayTimeLabelPosition.x -
                          (SecondsTextWidth + MinutesTextWidth + minutesWidth) +
                          512.0f;
  PlayMinutes->Bounds.Y = 620.0f;
  PlayMinutes->SetText((uint8_t*)sc3StringBuffer, 32, false, 0);

  if (hours != 0) {
    HoursText->Tint.a = FadeAnimation.Progress;
    PlayHours->Tint.a = FadeAnimation.Progress;
    sprintf(temp, "%2d", hours);
    TextGetSc3String(std::string(temp), sc3StringBuffer);
    dummy.Ip = (uint8_t*)sc3StringBuffer;
    float hoursWidth =
        TextGetPlainLineWidth(&dummy, Profile::Dialogue::DialogueFont, 32);
    PlayHours->Bounds.X = PlayTimeLabelPosition.x - SecondsTextWidth -
                          MinutesTextWidth - (HoursTextWidth + hoursWidth) +
                          464.0f;
    PlayHours->Bounds.Y = 620.0f;
    PlayHours->SetText((uint8_t*)sc3StringBuffer, 32, false, 0);
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
  SceneTitlePage->Add(new Label(WindowSprite, WindowPosition));

  auto numberLabelPos = glm::vec2(108.0f, 140.0f);
  auto textLabelPos = glm::vec2(156.0f, 140.0f);
  int idx = 0;
  char temp[4];
  auto lockedText = Vm::ScriptGetTextTableStrAddress(0, 15);
  for (int i = 0; i < SceneCount; i++) {
    sprintf(temp, "%2d", i + 1);
    auto numberLabel =
        new Label(std::string(temp), numberLabelPos, 24, false, 0);
    numberLabelPos += glm::vec2(0.0f, 36.0f);

    auto lockedLabel = new Label(lockedText, textLabelPos, 24, false, 0);
    auto unlockedLabel = new Label(Vm::ScriptGetTextTableStrAddress(4, i),
                                   textLabelPos, 24, false, 0);
    textLabelPos += glm::vec2(0.0f, 36.0f);

    SceneNames[idx] = unlockedLabel;
    idx += 1;
    SceneNames[idx] = lockedLabel;
    idx += 1;

    SceneTitlePage->Add(numberLabel);
    SceneTitlePage->Add(lockedLabel);
    SceneTitlePage->Add(unlockedLabel);
  }

  MainItems->Add(SceneTitlePage);
}

void ClearListMenu::UpdateSceneList() {
  int idx = 0;
  for (int i = 0; i < SceneCount * 2; i += 2) {
    if (GetFlag(SF_SCN_CLR1 + idx)) {
      SceneNames[i]->Tint.a = 1.0f;
      SceneNames[i + 1]->Tint.a = 0.0f;
    } else {
      SceneNames[i + 1]->Tint.a = 1.0f;
      SceneNames[i]->Tint.a = 0.0f;
    }
    idx += 1;
  }
}

// TEST
void ClearListMenu::AdvancePage(Widget* currentPage, Widget* nextPage) {
  currentPage->Move(glm::vec2(750.0f, 0.0f), 0.4f);
  nextPage->MoveTo(glm::vec2(-750.0f, 0.0f));
  nextPage->Move(glm::vec2(750.0f, 0.0f), 0.4f);
  nextPage->Show();
  CurrentPage = nextPage;
  PreviousPage = currentPage;
}

// TEST
void ClearListMenu::GoBackPage(Widget* currentPage, Widget* nextPage) {
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