root.TrophyMenu = {
    DrawType = DrawComponentType.SystemMenu,
    Type = TrophyMenuType.CHLCC,
    TransitionDuration = 64 / 60,
    BackgroundColor = 0xe8641b,
    CircleSprite = "CircleTrophy",
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
    MenuTitleTextRightPos = { X = 790, Y = 256 },
    MenuTitleTextLeftPos = { X = 4, Y = 4 },
    MenuTitleTextAngle = 4.45,
    TitleFadeInDuration = 40 / 60,
    TitleFadeOutDuration = 28 / 60,
    MenuTitleText = "MenuTitleTextTrophy",
    ButtonPromptSprite = "ButtonPromptTrophy",
    ButtonPromptPosition = { X = 1112, Y = 651 },
};

root.Sprites["CircleTrophy"] = {
    Sheet = "Trophy",
    Bounds = { X = 289, Y = 665, Width = 106, Height = 106 }
}

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

root.Sprites["MenuTitleTextTrophy"] = {
    Sheet = "Trophy",
    Bounds = { X = 964, Y = 4, Width = 116, Height = 664 }
}

root.Sprites["ButtonPromptTrophy"] = {
    Sheet = "Trophy",
    Bounds = { X = 2, Y = 802, Width = 167, Height = 28 }
}