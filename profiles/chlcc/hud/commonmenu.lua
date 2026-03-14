root.CommonMenu = {
  Type = CommonMenuType.CHLCC,
  CircleStartPosition = { X = 20, Y = 20 },
  CircleOffset = 200,

  ErinSprite = "Erin",
  ErinPosition = { X = 301, Y = 1 },
  BackgroundFilter = "BackgroundFilter",

  InitialRedBarPosition = { X = 0, Y = 538 },
  RightRedBarPosition = { X = 1059, Y = 538 },
  RedBarDivision = 1826,
  RedBarBaseX = 1059,
  RedBarSprite = "RedBar",
  RedBarLabelPosition = { X = 1067, Y = 573 },
  RedBarLabel = "RedBarLabel",

  TitleFadeInDuration = 40 / 60,
  TitleFadeOutDuration = 28 / 60,
  MenuSelectPromptDuration =  110 / 60,
  MenuSelectPromptInterval = 5 / 60,
  MenuTransitionDuration = 64 / 60,
  ShowPageAnimationStartTime = 16 / 60,
  ShowPageAnimationDuration = (48 - 16) / 60,
  ButtonPromptAnimationStartTime = 48 / 60,
  ButtonPromptAnimationDuration = (64 - 48) / 60,
  ButtonPromptStartPosition = { X = 603 + (64 - 48) * 40, Y = 651 },
  DiagonalTitlesOffsetStart = {X = 572.0 * 3, Y= -460.0 },
  DiagonalTitlesOffsetEnd = {X = -572.0 , Y= 460.0 / 3.0 }
};

root.Sprites["Erin"] = {
    Sheet = "Main",
    Bounds = { X = 641, Y = 1, Width = 978, Height = 798 }
}

root.Sprites["BackgroundFilter"] = {
    Sheet = "Main",
    Bounds = { X = 0, Y = 0, Width = 640, Height = 360 }
}

root.Sprites["RedBar"] = {
    Sheet = "Main",
    Bounds = { X = 767, Y = 913, Width = 1280, Height = 110 }
}

root.Sprites["RedBarLabel"] = {
    Sheet = "Main",
    Bounds = { X = 506, Y = 469, Width = 133, Height = 74 }
}