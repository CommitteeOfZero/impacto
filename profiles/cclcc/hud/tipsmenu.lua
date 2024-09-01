root.TipsMenu = {
    Type = TipsMenuType.CCLCC,
    DrawType = DrawComponentType.SystemMenu,
    BackgroundSprite = "TipsMenuBackground",
    FadeInDuration = 0.2,
    FadeOutDuration = 0.2,
    TipsBookLayerSprite = "TipsBookLayer",
    TipsGuideSprite = "TipsGuide",
    TipsGuideX = 0,
    TipsGuideY = 990,
    TipsMaskSprite = "TipsMask",
    TipsNewSprite = "TipsNewSprite",
    TipsHighlightedSprite = "TipsHighlightedSprite",

    TipsEntryBounds = { X = 80, Y = 382, Width = 781, Height = 50},
    TipEntryNewOffset = { X = 0, Y = 1 },
    TipsEntryHighlightOffset = { X = 41, Y = 3 },
    TipsEntryNumberOffset = { X = 41, Y = 8 },
    TipsEntryNameOffset = {X = 94, Y = 6},
    TipsTabBounds = {X = 0, Y = 382, Width = 890, Height = 545 },
    TipsHighlightedTabSprite = "TipsHighlightedTabSprite",
    TipsTabNameDisplay = {X = 266, Y = 285},
    TipsHighlightedTabAdder = 161,

    TipsTextTableIndex = 2,
    TipsTextEntryLockedIndex = 7,
    TipsTextSortStringIndex = 10,

    CategoryPos = { X = 1230, Y = 76},
    CategoryFontSize = 32,
    NamePos = { X = 1020, Y = 160},
    NameFontSize = 40,
    NumberPos = { X = 1807, Y = 84},
    NumberFontSize = 44,
    PronounciationPos = { X = 1020, Y = 246},
    PronounciationFontSize = 26,

    TipsEntryNameUnreadColor = 0x117733, 
    TipsMenuDarkTextColor = 0x3e3e3c,
};

root.Sprites["TipsMenuBackground"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 1080, Width = 1920, Height = 1080 }
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