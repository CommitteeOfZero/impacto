root.TitleMenu = {
    Type = TitleMenuType.CCLCC,
    DrawType = DrawComponentType.SystemMenu,
    PressToStartX = 823,
    PressToStartY = 749,
    PressToStartAnimDurationIn = 0.6,
    PressToStartAnimDurationOut = 0.6,
    PrimaryFadeInDuration = 0.7,
    PrimaryFadeOutDuration = 0.7,
    SecondaryFadeInDuration = 0.4,
    SecondaryFadeOutDuration = 0.4,
    SlideItemsAnimationDurationIn = 0.5,
    SlideItemsAnimationDurationOut = 0.5,
    PressToStartSprite = "TitleMenuPressToStart",
    BackgroundSprite = "TitleMenuBackground",
    MainBackgroundSprite = "MainMenuBackground",
    CopyrightTextSprite = "CopyrightText",
    CopyrightTextX = 566,
    CopyrightTextY = 955,
    SmokeX = 0,
    SmokeY = 580,
    SmokeBoundsX = 20,
    SmokeBoundsY = 1550,
    SmokeBoundsWidth = 1920,
    SmokeBoundsHeight = 500,
    SmokeAnimationBoundsXOffset = 20,
    SmokeAnimationBoundsXMax = 1919,
    SmokeOpacityNormal = 0.15,
    SmokeAnimationDurationIn = 32,
    SmokeAnimationDurationOut = 32,
    SmokeSprite = "TitleMenuSmoke",
    OverlaySprite = "TitleMenuOverlay",
    MenuSprite = "TitleMenuMenu",
    MenuX = 27,
    MenuY = 26,
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
    TitleAnimationDurationIn = 32/60,
    TitleAnimationDurationOut = 16/60,
    TitleAnimationStartFrame = 15,
    TitleAnimationFrameCount = 20,
    TitleAnimationFileId = 32,
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    ExitSprite = "ExitSprite",
    MenuEntriesNum = 5,
    ChoiceBlinkAnimationDurationIn = 1,
    HighlightAnimationDurationIn = 15/60,
    HighlightAnimationDurationOut = 15/60,
    ExtraDisabledTint = 0x703030,
};

for i = 0, 4 do
    root.Sprites["TitleMenuEntry" .. i] = {
        Sheet = "TitleChip",
        Bounds = {
            X = 33,
            Y = 288 + i * root.TitleMenu.ItemPadding,
            Width = 223,
            Height = 37
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "TitleMenuEntry" .. i;
end

for i = 0, 4 do
    root.Sprites["TitleMenuEntry" .. i] = {
        Sheet = "TitleChip",
        Bounds = {
            X = 33,
            Y = 288 + i * root.TitleMenu.ItemPadding,
            Width = 223,
            Height = 37
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

root.Sprites["TitleMenuBackground"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 },
};

root.Sprites["MainMenuBackground"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 1080, Width = 1920, Height = 1080 },
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet = "TitleChip",
    Bounds = { X = 583, Y = 75, Width = 198, Height = 169 },
};

root.Sprites["CopyrightText"] = {
    Sheet = "TitleChip",
    Bounds = { X = 582, Y = 9, Width = 785, Height = 21 },
};

root.Sprites["TitleMenuMenu"] = {
    Sheet = "TitleChip",
    Bounds = { X = 0, Y = 0, Width = 544, Height = 248 },
};

root.Sprites["TitleMenuOverlay"] = {
    Sheet = "MenuChip",
    Bounds = { X = 154, Y = 140, Width = 1900, Height = 1061 },
};

root.Sprites["TitleMenuSmoke"] = {
    Sheet = "MenuChip",
    Bounds = { X = 0, Y = 1638, Width = 2000, Height = 410 },
};

root.Sprites["ExitSprite"] = {
    Sheet = "TitleChip",
    Bounds = { X = 33, Y = 568, Width = 223, Height = 37 },
};
