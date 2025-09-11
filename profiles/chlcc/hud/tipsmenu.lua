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
    TreePosition = { X = 52, Y = 0 },
    TipsGradient = "TipsGradient",
    GradientPosition = { X = 73, Y = 0 },
    EndOfGradientColor = 0x2691ff,
    ButtonPromptSprite = "ButtonPromptTips",
    ButtonPromptPosition = { X = 685, Y = 651 },
    CurrentTipBackgroundSprite = "CurrentTipBackgroundSprite",
    CurrentTipBackgroundPosition = { X = 381, Y = 42 },
    SelectWordSprite = "SelectWord",
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
    CategoryString = "【 】",
    TipsStringTable = 2,
    CategoryStringIndex = 5, 
    SortStringIndex = 10, 
    LockedTipsIndex = 7, 
    NumberLabelStrIndex = 8, 
    PageSeparatorIndex = 9, 
    NewLabelStrIndex = 11,
    UnreadLabelStrIndex = 12,
    TipListEntryBounds = { X = 74, Y = 130, Width = 286, Height = 24 },
    TipListEntryTextOffsetX = 20;
    TipListEntryFontSize = 20,
    TipListYPadding = 24,
    TipsListBounds = { X = 74, Y = 130, Width = 286, Height = 500 },
    TipsListRenderBounds = { X = 30, Y = 130, Width = 380, Height = 500 },
    NumberLabelPosition = { X = 396, Y = 54 },
    NumberLabelFontSize = 22,
    NumberBounds = { X = 456, Y = 54, Width = 0, Height = 0 },
    NumberFontSize = 22,
    CurrentPageBounds = { X = 1130, Y = 645, Width = 0, Height = 0 },
    TotalPagesBounds = { X = 1184, Y = 645, Width = 0, Height = 0 },
    DefaultColorIndex = 0,
    UnreadColorIndex = 70,
    PageSeparatorPosition = { X = 1154, Y = 645 },
    PageSeparatorFontSize = 32,
    PronounciationFontSize = 20,
    NameFontSize = 32,
    TipListEntryNameXOffset = 50,
    NameInitialBounds = { X = 1210, Y = 77, Width = 0, Height = 0 },
    PronounciationInitialBounds = { X = 1210, Y = 107, Width = 0, Height = 0 },
    TipsListEntryDotOffset = { X = -17, Y = -4},
    TipsListNewDotOffset= { X = -40, Y = -4},

    TipsEntryHighlightBarSprite = "TipsEntryHighlightBar",
    TipsEntryHighlightDotSprite = "TipsEntryHighlightDot",
    TipsEntryNewDotSprite = "TipsEntryNewDot",
    TipsLeftLineSprite = "TipsLeftLine",
    TipsLeftLineEndSprite = "TipsLeftLineEnd",
    TipsLeftLineHoleSprite = "TipsLeftLineHole",
    TipsLeftLineHoleEndSprite = "TipsLeftLineHoleEnd",
    TipsListBgBarSprite = "TipsListBgBar",
    TipsListBgBarHoleSprite = "TipsListBgBarHole",
    TipsScrollThumbSprite = "TipsScrollThumb",
    TipsScrollTrackSprite = "TipsScrollTrack",

}

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
    Bounds = { X = 851, Y = 273, Width = 182, Height = 131 }
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

root.Sprites["SelectWord"] = {
    Sheet = "Tips",
    Bounds = { X = 1, Y = 643, Width = 200, Height = 57 }
}

root.Sprites["TipsEntryHighlightBar"] = {
    Sheet = "Tips",
    Bounds = { X = 851, Y = 510, Width = 296, Height = 24 }
}
root.Sprites["TipsEntryHighlightDot"] = {
    Sheet = "Tips",
    Bounds = { X = 636, Y = 609, Width = 32, Height = 32 }
}
root.Sprites["TipsEntryNewDot"] = {
    Sheet = "Tips",
    Bounds = { X = 602, Y = 609, Width = 32, Height = 32 }
}
root.Sprites["TipsLeftLine"] = {
    Sheet = "Tips",
    Bounds = { X = 851, Y = 406, Width = 32, Height = 24 }
}
root.Sprites["TipsLeftLineHole"] = {
    Sheet = "Tips",
    Bounds = { X = 851, Y = 432, Width = 32, Height = 24 }
}
root.Sprites["TipsLeftLineHoleEnd"] = {
    Sheet = "Tips",
    Bounds = { X = 851, Y = 458, Width = 32, Height = 24 }
}
root.Sprites["TipsLeftLineEnd"] = {
    Sheet = "Tips",
    Bounds = { X = 851, Y = 484, Width = 316, Height = 8 }
}
root.Sprites["TipsListBgBar"] = {
    Sheet = "Tips",
    Bounds = { X = 851, Y = 134, Width = 286, Height = 24 }
}
root.Sprites["TipsListBgBarHole"] = {
    Sheet = "Tips",
    Bounds = { X = 851, Y = 160, Width = 286, Height = 24 }
}
root.Sprites["TipsScrollThumb"] = {
    Sheet = "Tips",
    Bounds = { X = 1274, Y = 511, Width = 16, Height = 61 }
}
root.Sprites["TipsScrollTrack"] = {
    Sheet = "Tips",
    Bounds = { X = 1280, Y = 0, Width = 3, Height = 500 }
}