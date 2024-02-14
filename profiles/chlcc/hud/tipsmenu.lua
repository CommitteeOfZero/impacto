root.TipsMenu = {
    DrawType = DrawComponentType.SystemMenu,
    Type = TipsMenuType.CHLCC,
    TransitionDuration = 64 / 60,
    BackgroundColor = 0x007efe,
    CircleSprite = "CircleTips",
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
    MenuTitleTextRightPos = { X = 790, Y = 212 },
    MenuTitleTextLeftPos = { X = 258, Y = 3 },
    MenuTitleTextAngle = 4.45,
    TitleFadeInDuration = 40 / 60,
    TitleFadeOutDuration = 28 / 60,
    MenuTitleText = "MenuTitleTextTips",
    TipsTree = "TipsTree",
    TreePosition = { X = 49, Y = 0 },
    TipsGradient = "TipsGradient",
    GradientPosition = { X = 73, Y = 0 },
    EndOfGradientColor = 0x2691ff,
    ButtonPromptSprite = "ButtonPromptTips",
    ButtonPromptPosition = { X = 685, Y = 651 },
    CurrentTipBackgroundSprite = "CurrentTipBackgroundSprite",
    CurrentTipBackgroundPosition = { X = 381, Y = 42 },
    SelectWord = {},
    SelectWordPos = {
        { X = 94, Y = 51 },
        { X = 109, Y = 51 },
        { X = 122, Y = 51 },
        { X = 134, Y = 51 },
        { X = 147, Y = 51 },
        { X = 161, Y = 51 },
        { X = 180, Y = 51 },
        { X = 199, Y = 51 },
        { X = 213, Y = 51 },
        { X = 228, Y = 51 }
    },
};

root.Sprites["CircleTips"] = {
    Sheet = "Tips",
    Bounds = { X = 1, Y = 917, Width = 106, Height = 106 }
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

root.Sprites["MenuTitleTextTips"] = {
    Sheet = "Tips",
    Bounds = { X = 1154, Y = 1, Width = 117, Height = 742 }
}

root.Sprites["TipsTree"] = {
    Sheet = "Tips",
    Bounds = { X = 848, Y = 273, Width = 300, Height = 150 }
}

--Purposefully cut the texture to create smaller Rect below
root.Sprites["TipsGradient"] = {
    Sheet = "Tips",
    Bounds = { X = 852, Y = 1, Width = 295, Height = 132 }
}

root.Sprites["ButtonPromptTips"] = {
    Sheet = "Tips",
    Bounds = { X = 3, Y = 611, Width = 595, Height = 28 }
}

root.Sprites["CurrentTipBackgroundSprite"] = {
    Sheet = "Tips",
    Bounds = { X = 1, Y = 1, Width = 846, Height = 605 }
}