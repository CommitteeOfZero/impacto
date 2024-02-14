root.TitleMenu = {
    Type = TitleMenuType.CC,
    DrawType = DrawComponentType.SystemMenu,
    PressToStartX = 816,
    PressToStartY = 738,
    PressToStartAnimDurationIn = 0.7,
    PressToStartAnimDurationOut = 0.7,
    PressToStartAnimFastDurationIn = 0.1,
    PressToStartAnimFastDurationOut = 0.1,
    BackgroundX = 0,
    BackgroundY = 0,
    BackgroundBoundsX = 580,
    BackgroundBoundsYNormal = 1,
    BackgroundBoundsYTrue = 1535,
    BackgroundBoundsWidth = 1920,
    BackgroundBoundsHeight = 318,
    FenceX = 0,
    FenceY = 288,
    FenceBoundsX = 1536,
    FenceBoundsYNormal = 1,
    FenceBoundsYTrue = 865,
    FenceBoundsWidth = 1920,
    FenceBoundsHeight = 862,
    CopyrightX = 640,
    CopyrightY = 947,
    CopyrightXMoveOffset = 1536,
    SmokeX = 0,
    SmokeY = 580,
    SmokeBoundsX = 20,
    SmokeBoundsY = 1800,
    SmokeBoundsWidth = 1920,
    SmokeBoundsHeight = 500,
    SmokeAnimationBoundsXOffset = 20,
    SmokeAnimationBoundsXMax = 1919,
    SmokeOpacityNormal = 0.25,
    SmokeAnimationDurationIn = 32,
    SmokeAnimationDurationOut = 32,
    MoveLeftAnimationDurationIn = 0.7,
    MoveLeftAnimationDurationOut = 0.7,
    MenuSprite = "TitleMenuMenu",
    MenuX = 27,
    MenuY = 26,
    PressToStartSprite = "TitleMenuPressToStart",
    BackgroundSprite = "TitleMenuBackground",
    FenceSprite = "TitleMenuFence",
    CopyrightSprite = "TitleMenuCopyright",
    OverlaySprite = "TitleMenuOverlay",
    SmokeSprite = "TitleMenuSmoke",
    ItemHighlightSprite = "TitleMenuItemHighlight",
    ItemHighlightOffsetX = 174,
    ItemHighlightOffsetY = 7,
    ItemHighlightPointerSprite = "TitleMenuPointerItemHighlight",
    ItemHighlightPointerY = 89,
    ItemPadding = 56,
    ItemYBase = 335,
    SecondaryFirstItemHighlightOffsetX = 225,
    SecondarySecondItemHighlightOffsetX = 370,
    SecondaryThirdItemHighlightOffsetX = 590,
    LoadSprite = "TitleMenuLoad",
    LoadHighlightSprite = "TitleMenuLoadHighlight",
    QuickLoadSprite = "TitleMenuQLoad",
    QuickLoadHighlightSprite = "TitleMenuQLoadHighlight",
    TipsSprite = "TitleMenuTips",
    TipsHighlightSprite = "TitleMenuTipsHighlight",
    LibrarySprite = "TitleMenuLibrary",
    LibraryHighlightSprite = "TitleMenuLibraryHighlight",
    EndingListSprite = "TitleMenuEndingList",
    EndingListHighlightSprite = "TitleMenuEndingListHighlight",
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    MenuEntriesNum = 5
};

for i = 0, 4 do
    root.Sprites["TitleMenuEntry" .. i] = {
        Sheet = "TitleChip",
        Bounds = {
            X = 4,
            Y = 324 + i * root.TitleMenu.ItemPadding,
            Width = 220,
            Height = 30
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "TitleMenuEntry" .. i;
end

for i = 0, 4 do
    root.Sprites["TitleMenuEntry" .. i] = {
        Sheet = "TitleChip",
        Bounds = {
            X = 4,
            Y = 324 + i * root.TitleMenu.ItemPadding,
            Width = 220,
            Height = 30
        }
    };
    root.TitleMenu.MenuEntriesHighlightedSprites[#root.TitleMenu.MenuEntriesHighlightedSprites + 1] = "TitleMenuEntry" .. i;
end

root.Sprites["TitleMenuItemHighlight"] = {
    Sheet = "TitleChip",
    Bounds = { X = 896, Y = 64, Width = 638, Height = 50 },
};

root.Sprites["TitleMenuLoad"] = {
    Sheet = "TitleChip",
    Bounds = { X = 417, Y = 288, Width = 114, Height = 39 },
};

root.Sprites["TitleMenuLoadHighlight"] = {
    Sheet = "TitleChip",
    Bounds = { X = 414, Y = 342, Width = 116, Height = 39 },
};

root.Sprites["TitleMenuQLoad"] = {
    Sheet = "TitleChip",
    Bounds = { X = 553, Y = 288, Width = 245, Height = 39 },
};

root.Sprites["TitleMenuQLoadHighlight"] = {
    Sheet = "TitleChip",
    Bounds = { X = 550, Y = 342, Width = 249, Height = 39 },
};

root.Sprites["TitleMenuTips"] = {
    Sheet = "TitleChip",
    Bounds = { X = 416, Y = 402, Width = 95, Height = 34 },
};

root.Sprites["TitleMenuTipsHighlight"] = {
    Sheet = "TitleChip",
    Bounds = { X = 417, Y = 457, Width = 96, Height = 36 },
};

root.Sprites["TitleMenuLibrary"] = {
    Sheet = "TitleChip",
    Bounds = { X = 535, Y = 402, Width = 178, Height = 34 },
};

root.Sprites["TitleMenuLibraryHighlight"] = {
    Sheet = "TitleChip",
    Bounds = { X = 535, Y = 457, Width = 180, Height = 36 },
};

root.Sprites["TitleMenuEndingList"] = {
    Sheet = "TitleChip",
    Bounds = { X = 736, Y = 401, Width = 258, Height = 35 },
};

root.Sprites["TitleMenuEndingListHighlight"] = {
    Sheet = "TitleChip",
    Bounds = { X = 736, Y = 457, Width = 260, Height = 36 },
};

root.Sprites["TitleMenuPointerItemHighlight"] = {
    Sheet = "TitleChip",
    Bounds = { X = 805, Y = 78, Width = 56, Height = 36 },
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet = "TitleChip",
    Bounds = { X = 1508, Y = 689, Width = 304, Height = 94 },
};

root.Sprites["TitleMenuFence"] = {
    Sheet = "Title",
    Bounds = { X = 1536, Y = 1, Width = 1920, Height = 862 },
};

root.Sprites["TitleMenuCopyright"] = {
    Sheet = "TitleChip",
    Bounds = { X = 1, Y = 667, Width = 860, Height = 20 },
};

root.Sprites["TitleMenuMenu"] = {
    Sheet = "TitleChip",
    Bounds = { X = 971, Y = 705, Width = 521, Height = 236 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet = "TitleChip",
    Bounds = { X = 580, Y = 1, Width = 1920, Height = 318 },
};

root.Sprites["TitleMenuOverlay"] = {
    Sheet = "MenuChip",
    Bounds = { X = 155, Y = 142, Width = 1898, Height = 1058 },
};

root.Sprites["TitleMenuSmoke"] = {
    Sheet = "MenuChip",
    Bounds = { X = 0, Y = 1900, Width = 2000, Height = 410 },
};