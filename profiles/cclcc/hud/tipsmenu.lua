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
    TipEntryNewOffset = { X = -90, Y = -5 },
    TipsEntryNumberOffset = { X = -53, Y = 2 },
    TipsEntryHighlightOffset = { X = -53, Y = -6 },
    TipsEntryNameInitDisplay = {X = 174, Y = 388},
    TipsEntryNameOffset = 50,

    TipsTabBounds = {X = 0, Y = 280, Width = 890, Height = 650 },
    TipsHighlightedTabSprite = "TipsHighlightedTabSprite",
    TipsTabNameDisplay = {X = 266, Y = 285},
    TipsHighlightedTabAdder = 161,

    TipsTextTableIndex = 2,
    TipsTextEntryLockedIndex = 7,
    TipsTextSortStringIndex = 10,
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