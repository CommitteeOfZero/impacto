root.TipsMenu = {
    Type = TipsMenuType.CCLCC,
    DrawType = DrawComponentType.SystemMenu,
    FadeInDuration = 0.4,
    FadeOutDuration = 0.4,
    TransitionInDuration = 0.4,
    TransitionOutDuration = 0.4,
    BackgroundSprite = "TipsBookLayer",
    TipsGuideSprite = "TipsGuide",
    TipsGuideX = 0,
    TipsGuideY = 990,
    TipsMaskSprite = "TipsMask",
    TipsNewSprite = "TipsNewSprite",
    TipsHighlightedSprite = "TipsHighlightedSprite",

    TipsEntryBounds = { X = 80, Y = 382, Width = 781, Height = 50},
    TipEntryNewOffset = { X = 0, Y = 1 },
    TipsEntryHighlightOffset = { X = 41, Y = 3 },
    TipsEntryNumberOffset = { X = 41, Y = 12 },
    TipsEntryNameOffset = {X = 94, Y = 10},
    TipsEntryNumberFontSize = 26,
    TipsEntryNameFontSize = 30,
    TipsTabBounds = {X = 0, Y = 382, Width = 890, Height = 545 },
    TipsHighlightedTabSprite = "TipsHighlightedTabSprite",
    TipsTabNameDisplay = {X = 266, Y = 285},
    TipsHighlightedTabAdder = 161,

    TipsTextTableIndex = 2,
    TipsTextEntryLockedIndex = 7,
    TipsTextSortStringIndex = 10,

    CategoryPos = { X = 1230, Y = 76},
    CategoryFontSize = 40,
    NamePos = { X = 1020, Y = 170},
    NameFontSize = 54,
    NumberPos = { X = 1875, Y = 80},
    NumberFontSize = 62,
    PronounciationPos = { X = 1020, Y = 246},
    PronounciationFontSize = 34,

    TipsEntryNameUnreadColor = 0x117733, 
    TipsMenuDarkTextColor = 0x3e3e3c,

    TipsScrollThumbSprite = "TipsScrollThumb",
    TipsScrollThumbLength = 40,
    TipsScrollYStart = 385,
    TipsScrollYEnd = 930,
    TipsScrollEntriesX = 896,
    TipsScrollDetailsX = 1783,

    TipsMask = "TipsMask",

    AdvanceFocusTimeInterval = 0.05,
    MinHoldTime = 1.0
};

root.Sprites["TipsBookLayer"] = {
    Sheet = "Tips",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
};

root.Sprites["TipsGuide"] = {
    Sheet = "Tips",
    Bounds = { X = 0, Y = 1152, Width = 1925, Height = 55 }
};

root.Sprites["TipsMask"] = {
    Sheet = "MenuChip",
    Bounds = { X = 154, Y = 140, Width = 1900, Height = 1061 },
};

-- Start of Tips Tab Name Sprites
root.Sprites["TipsHighlightedTabSprite"] = {
    Sheet = "Tips",
    Bounds = { X = 1355, Y = 1212, Width = 160, Height = 44 }
};

root.Sprites["TipsNewSprite"] = {
    Sheet = "Tips",
    Bounds = { X = 1314, Y = 1212, Width = 40, Height = 40 }
};

root.Sprites["TipsHighlightedSprite"] = {
    Sheet = "Tips",
    Bounds = { X = 517, Y = 1208, Width = 740, Height = 40 }
};

root.SpriteSheets["TipsMask"] = {
    Path = { Mount = "system", Id = 29 },
    DesignWidth = 1920,
    DesignHeight = 1080
};

root.Sprites["TipsScrollThumb"] = {
    Sheet = "Tips",
    Bounds = { X = 2016, Y = 994, Width = 11, Height = 40 }
};
