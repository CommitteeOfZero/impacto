root.TitleMenu = {
    Type = TitleMenuType.SG,
    DrawType = DrawComponentType.SystemMenu,
    PressToStartX = 72,
    PressToStartY = 595,
    PressToStartAnimDurationIn = 0.5,
    PressToStartAnimDurationOut = 0.5,
    PressToStartSprite = "TitleMenuPressToStart",
    IntroBackgroundSprite = "TitleMenuIntroBackground",
    BackgroundSprite = "TitleMenuBackground",
    ExclMarkLogoSprite = "ExclMarkLogo",
    ExclMarkLogoX = 614,
    ExclMarkLogoY = 316,
    CopyrightTextSprite = "CopyrightText",
    CopyrightTextX = 72,
    CopyrightTextY = 675,
    SpinningCircleSprite = "SpinningCircle",
    SpinningCircleX = 610.5,
    SpinningCircleY = -285.5,
    SpinningCircleAnimationDuration = 15,
    ItemHighlightSprite = "TitleMenuItemHighlight",
    ItemHighlightOffsetX = 73,
    ItemHighlightOffsetY = 7,
    ItemPadding = 40,
    ItemYBase = 69,
    ItemFadeInDuration = 0.3,
    ItemFadeOutDuration = 0.6,
    SecondaryItemFadeInDuration = 0.2,
    SecondaryItemFadeOutDuration = 0.2,
    PrimaryFadeInDuration = 0.3,
    PrimaryFadeOutDuration = 0.3,
    SecondaryFadeInDuration = 0.512,
    SecondaryFadeOutDuration = 0.512,
    ItemHyperUpLine = "TitleMenuItemHyperUpLine",
    ItemSuperUpLine = "TitleMenuItemSuperUpLine",
    ItemUpLine = "TitleMenuItemUpLine",
    ItemStraightLine = "TitleMenuItemStraightLine",
    ItemDownLine = "TitleMenuItemDownLine",
    ItemSuperDownLine = "TitleMenuItemSuperDownLine",
    ItemLoadQuickSprite = "TitleMenuItemLoadQuick",
    SecondaryItemX = 320,
    ItemLoadY = 109,
    ItemLoadQuickY = 83,
    ItemLoadSprite = "TitleMenuItemLoad",
    ItemLoadQuickHighlightedSprite = "TitleMenuItemLoadQuickHighlighted",
    ItemLoadHighlightedSprite = "TitleMenuItemLoadHighlighted",
    SecondaryItemHighlightSprite = "TitleMenuSecondaryItemHighlight",
    ItemClearListY = 71,
    ItemCGLibraryY = 97,
    ItemSoundLibraryY = 123,
    ItemMovieLibraryY = 149,
    ItemTipsY = 175,
    ItemTrophyY = 201,
    ItemConfigY = 163,
    ItemSystemSaveY = 189,
    SecondaryItemHighlightX = 286,
    SecondaryMenuPaddingY = 26,
    SecondaryMenuLoadOffsetY = 76,
    SecondaryMenuLineX = 241,
    SecondaryMenuLoadLineY = 93,
    SecondaryMenuLoadQuickLineY = 119,
    SecondaryMenuExtraClearY = 81,
    SecondaryMenuExtraCGY = 107,
    SecondaryMenuExtraSoundY = 133,
    SecondaryMenuExtraMovieY = 159,
    SecondaryMenuExtraTipsY = 159,
    SecondaryMenuExtraTrophyY = 159,
    SecondaryMenuSystemConfigY = 173,
    SecondaryMenuSystemSaveY = 199,
    MenuEntriesNum = 14,
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    LineNum = 6,
    LineEntriesSprites = {}
};

for i = 0, 3 do
    root.Sprites["TitleMenuEntry" .. i] = {
        Sheet = "Title",
        Bounds = {
            X = 1151,
            Y = 101 + i * 25,
            Width = 188,
            Height = 23
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "TitleMenuEntry" .. i;
end

for i = 0, 9 do
    root.Sprites["TitleMenuEntry" .. (i + 4)] = {
        Sheet = "Title",
        Bounds = {
            X = 1369,
            Y = 684 + i * 22,
            Width = 216,
            Height = 20
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "TitleMenuEntry" .. (i + 4);
end

for i = 0, 3 do
    root.Sprites["TitleMenuEntryHighlighted" .. i] = {
        Sheet = "Title",
        Bounds = {
            X = 1151,
            Y = 1 + i * 25,
            Width = 188,
            Height = 23
        }
    };
    root.TitleMenu.MenuEntriesHighlightedSprites[#root.TitleMenu.MenuEntriesHighlightedSprites + 1] = "TitleMenuEntryHighlighted" .. i;
end

for i = 0, 9 do
    root.Sprites["TitleMenuEntryHighlighted" .. (i + 4)] = {
        Sheet = "Title",
        Bounds = {
            X = 1151,
            Y = 684 + i * 22,
            Width = 216,
            Height = 20
        }
    };
    root.TitleMenu.MenuEntriesHighlightedSprites[#root.TitleMenu.MenuEntriesHighlightedSprites + 1] = "TitleMenuEntryHighlighted" .. (i + 4);
end

root.Sprites["TitleMenuPressToStart"] = {
    Sheet = "Title",
    Bounds = { X = 1, Y = 921, Width = 313, Height = 28 },
};

root.Sprites["ExclMarkLogo"] = {
    Sheet = "Title",
    Bounds = { X = 831, Y = 895, Width = 82, Height = 128 },
};

root.Sprites["CopyrightText"] = {
    Sheet = "Title",
    Bounds = { X = 193, Y = 891, Width = 380, Height = 24 },
};

root.Sprites["SpinningCircle"] = {
    Sheet = "Title",
    Bounds = { X = 1366, Y = 1, Width = 681, Height = 681 },
};

root.Sprites["TitleMenuIntroBackground"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 },
};

root.Sprites["TitleMenuItemHighlight"] = {
    Sheet = "Title",
    Bounds = { X = 915, Y = 951, Width = 244, Height = 36 },
};

root.Sprites["TitleMenuItemHyperUpLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 686, Width = 51, Height = 80 },
};

root.Sprites["TitleMenuItemSuperUpLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 776, Width = 51, Height = 54 },
};

root.Sprites["TitleMenuItemUpLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 845, Width = 51, Height = 28 },
};

root.Sprites["TitleMenuItemStraightLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 888, Width = 51, Height = 2 },
};

root.Sprites["TitleMenuItemDownLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 910, Width = 51, Height = 28 },
};

root.Sprites["TitleMenuItemSuperDownLine"] = {
    Sheet = "Title",
    Bounds = { X = 1805, Y = 959, Width = 51, Height = 54 },
};

root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemHyperUpLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemSuperUpLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemUpLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemStraightLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemDownLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemSuperDownLine";

root.Sprites["TitleMenuItemLoadQuick"] = {
    Sheet = "Title",
    Bounds = { X = 1369, Y = 684, Width = 216, Height = 20 },
};

root.Sprites["TitleMenuItemLoad"] = {
    Sheet = "Title",
    Bounds = { X = 1369, Y = 706, Width = 216, Height = 20 },
};

root.Sprites["TitleMenuItemLoadQuickHighlighted"] = {
    Sheet = "Title",
    Bounds = { X = 1151, Y = 684, Width = 216, Height = 20 },
};

root.Sprites["TitleMenuItemLoadHighlighted"] = {
    Sheet = "Title",
    Bounds = { X = 1151, Y = 706, Width = 216, Height = 20 },
};

root.Sprites["TitleMenuSecondaryItemHighlight"] = {
    Sheet = "Title",
    Bounds = { X = 915, Y = 989, Width = 285, Height = 34 },
};

root.TitleMenu = {
    Type = TitleMenuType.SG,
    DrawType = DrawComponentType.SystemMenu,
    BackgroundSprite = "TitleMenuBackground",
    BackgroundX = 0.0,
    BackgroundY = 0.0,

    PressToStartX = 830,
    PressToStartY = 620,
    PressToStartAnimDurationIn = 0.7,
    PressToStartAnimDurationOut = 0.7,
    PressToStartAnimFastDurationIn = 0.1,
    PressToStartAnimFastDurationOut = 0.1,
    PressToStartSprite = "TitleMenuPressToStart",

    MenuX = 1555,
    MenuY = 110,
    MenuEntriesNum = 5,
    MenuEntriesSprites = { "TitleMenuEntryStart", "TitleMenuEntryLoad", "TitleMenuEntryExtra", "TitleMenuEntryConfig", "TitleMenuEntryHelp" },
    MenuEntriesHighlightedSprites = { "TitleMenuEntryStartHighlighted", "TitleMenuEntryLoadHighlighted", "TitleMenuEntryExtraHighlighted", "TitleMenuEntryConfigHighlighted", "TitleMenuEntryHelpHighlighted" },
    MenuEntriesSpacingY = 75,
    MenuStartId = 0,
    MenuLoadId = 1,
    MenuExtraId = 2,
    MenuConfigId = 3,
    MenuHelpId = 4,
};

root.Sprites["TitleMenuBackground"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 },
};

root.Sprites["TitleMenuPressToStart"] = {
    Sheet = "Title",
    Bounds = { X = 140, Y = 1080, Width = 360, Height = 50 },
};

MenuEntryWidth = 200;
MenuEntryHeight = 50;
MenuEntrySheet = "Title";
MenuEntryFirstRowX = 1920;
MenuEntrySecondRowX = 1920 + MenuEntryWidth;
MenuEntryThirdRowX = 1920 + MenuEntryWidth * 2;
MenuEntryFourthRowX = 1920 + MenuEntryWidth * 3;

root.Sprites["TitleMenuEntryStartHighlighted"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntryFirstRowX, Y = 0, Width = MenuEntryWidth, Height = MenuEntryHeight },
};

root.Sprites["TitleMenuEntryLoadHighlighted"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntryFirstRowX, Y = MenuEntryHeight * 1, Width = MenuEntryWidth, Height = MenuEntryHeight },
};

root.Sprites["TitleMenuEntryExtraHighlighted"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntryFirstRowX, Y = MenuEntryHeight * 2, Width = MenuEntryWidth, Height = MenuEntryHeight },
};

root.Sprites["TitleMenuEntryConfigHighlighted"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntryFirstRowX, Y = MenuEntryHeight * 3, Width = MenuEntryWidth, Height = MenuEntryHeight },
};

root.Sprites["TitleMenuEntryHelpHighlighted"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntryThirdRowX, Y = 0, Width = MenuEntryWidth, Height = MenuEntryHeight },
};

root.Sprites["TitleMenuEntryStart"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntrySecondRowX, Y = 0, Width = MenuEntryWidth, Height = MenuEntryHeight },
};

root.Sprites["TitleMenuEntryLoad"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntrySecondRowX, Y = MenuEntryHeight * 1, Width = MenuEntryWidth, Height = MenuEntryHeight },
};

root.Sprites["TitleMenuEntryExtra"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntrySecondRowX, Y = MenuEntryHeight * 2, Width = MenuEntryWidth, Height = MenuEntryHeight },
};

root.Sprites["TitleMenuEntryConfig"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntrySecondRowX, Y = MenuEntryHeight * 3, Width = MenuEntryWidth, Height = MenuEntryHeight },
};

root.Sprites["TitleMenuEntryHelp"] = {
    Sheet = MenuEntrySheet,
    Bounds = { X = MenuEntryFourthRowX, Y = 0, Width = MenuEntryWidth, Height = MenuEntryHeight },
};
