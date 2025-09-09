root.TrophyMenu = {
    DrawType = DrawComponentType.SystemMenu,
    Type = TrophyMenuType.CHLCC,

    TransitionDuration = 64 / 60,

    ShowPageAnimationStartTime = 13 / 64, -- 16 / 64,
    ShowPageAnimationDuration = 43 / 64, -- (48 - 16) / 64,

    BackgroundColor = 0xe8641b,
    BackgroundFilter = "BackgroundFilter",

    CircleSprite = "CircleTrophy",
    CircleStartPosition = { X = 20, Y = 20 },
    CircleOffset = 200,

    ErinSprite = "Erin",
    ErinPosition = { X = 301, Y = 1 },

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
    ButtonPromptPosition = { X = 1113, Y = 651 },

    PlatinumTrophySprite = "PlatinumTrophy",
    PlatinumTrophyPos = { X = 585, Y = 91 },
    GoldTrophySprite = "GoldTrophy",
    GoldTrophyPos = { X =  649, Y = 91 },
    SilverTrophySprite = "SilverTrophy",
    SilverTrophyPos = { X = 713, Y = 91 },
    BronzeTrophySprite = "BronzeTrophy",
    BronzeTrophyPos = { X = 777, Y = 91 },

    DefaultTrophyIconSprite = "DefaultTrophyIcon",
    TrophyEntryCardSprite = "TrophyEntryCard",
    TrophyEntriesBorderSprite = "TrophyEntriesBorder",
    TrophyPageCtBoxSprite = "TrophyPageCtBox",
    TrophyPageCtPos = { X = 1090, Y = 60 },

    EntriesPerPage = 6,
    FirstEntryPos = { X = 0, Y = 130 },
    EntryHeight = 74,
    EntryCardOffset = { X = 91, Y = 1 },
    EntryNameOffset = { X = 218, Y = 12 },
    EntryNameFontSize = 26,
    EntryDescriptionOffset = { X = 218, Y = 42 },
    EntryDescriptionFontSize = 18,
    EntryIconOffset = { X = 112, Y = 4 },
    EntryDefaultNameTextTableId = 0,
    EntryDefaultNameStringNum = 19,

    CurrentPageNumPos = { X = 1133, Y = 65 },
    PageNumSeparatorSlashSprite = "PageNumSeparatorSlash",
    PageNumSeparatorSlashPos = { X = 1161, Y = 90 },
    MaxPageNumPos = { X = 1179, Y = 90 },
    PageNums = {},
    ReachablePageNums = {},

    TrophyCountHintTextTableId = 0,
    TrophyCountHintStringNum = 20,
    TrophyCountHintLabelPos = { X = 297, Y = 97 },
    TrophyCountFontSize = 20
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

root.Sprites["DefaultTrophyIcon"] = {
    Sheet = "Trophy",
    Bounds = { X = 1, Y = 665, Width = 64, Height = 64 }
}

root.Sprites["TrophyEntryCard"] = {
    Sheet = "Trophy",
    Bounds = { X = 1, Y = 577, Width = 830, Height = 70 }
}

root.Sprites["PlatinumTrophy"] = {
    Sheet = "Trophy",
    Bounds = { X = 33, Y = 737, Width = 30, Height = 30 }
}

root.Sprites["GoldTrophy"] = {
    Sheet = "Trophy",
    Bounds = { X = 65, Y = 737, Width = 30, Height = 30 }
}

root.Sprites["SilverTrophy"] = {
    Sheet = "Trophy",
    Bounds = { X = 97, Y = 737, Width = 30, Height = 30 }
}

root.Sprites["BronzeTrophy"] = {
    Sheet = "Trophy",
    Bounds = { X = 129, Y = 737, Width = 30, Height = 30 }
}

root.Sprites["TrophyEntriesBorder"] = {
    Sheet = "Trophy",
    Bounds = { X = 1, Y = 1, Width = 958, Height = 574 }
}

root.Sprites["TrophyPageCtBox"] = {
    Sheet = "Trophy",
    Bounds = { X = 1, Y = 833, Width =  204, Height = 57 }
}

for i = 0, 9 do
    root.Sprites["PageNum" .. i] = {
        Sheet = "Data",
        Bounds = { X = i * 36 + 371, Y = 1, Width = 34, Height = 48 }
    };
    root.TrophyMenu.PageNums[#root.TrophyMenu.PageNums + 1] = "PageNum" .. i;
end

root.Sprites["PageNumSeparatorSlash"] = {
    Sheet = "Data",
    Bounds = { X = 173, Y = 67, Width = 16, Height = 22 }
};

for i = 0, 9 do
    root.Sprites["ReachablePageNum" .. i] = {
        Sheet = "Data",
        Bounds = { X = 191 + 18 * i, Y = 67, Width = 16, Height = 22 }
    };
    root.TrophyMenu.ReachablePageNums[#root.TrophyMenu.ReachablePageNums + 1] = "ReachablePageNum" .. i;
end

