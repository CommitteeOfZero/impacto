root.TitleMenu = {
    Type = TitleMenuType.CHLCC_Switch,
    DrawType = DrawComponentType.SystemMenu,
    PressToStartPos = { X = 113, Y = 897 },
    PressToStartAnimDurationIn = 0.5,
    PressToStartAnimDurationOut = 0.5,
    PressToStartSprite = "TitleMenuPressToStart",
    BackgroundSprite = "TitleMenuBackground",
    IntroBackgroundSprite = "TitleMenuIntroBackground",
    IntroHighlightSprites = {
        "IntroBrightGreenHighlight",
        "IntroSunHighlight",
        "IntroGrayHighlight",
        "IntroCrescentRainbowHighlight",
        "IntroBlueHighlight",
        "IntroWhiteHighlight",
        "IntroBrownHighlight",
        "IntroDiamondHighlight",
        "IntroDarkGreenHighlight",
        "IntroCircularRainbowHighlight"
    },

    -- Positions along the diagonal normalized between -1 and 1
    IntroHighlightPositions = {
        -1.13, -1.00, -0.49, 0.00, 0.17,
        0.30, 0.58, 0.69, 0.91, 1.12
    },
    IntroPanningAnimationDuration = 2.1,
    IntroAfterPanningWaitDuration = 0.8,
    IntroBouncingStarSprite = "StarLogo",
    IntroBouncingStarBaseYOffset = 243,
    IntroBouncingStarAnimationDuration = 3.73,
    IntroExplodingStarSprite = "IntroSmallStar",
    IntroExplodingStarAnimationDuration = 1.067,
    IntroExplodingStarAnimationRotationDuration = 0.5,
    IntroExplodingStarAnimationDistance = 441,
    IntroFallingStarSprite = "IntroBigStar",
    IntroFallingStarsAnimationDuration = 3,
    IntroFallingStarsAnimationDistance = 3819,
    IntroFallingStarsAnimationDirection = { X = -1, Y = 1 },
    IntroFallingStarsAnimationRotationDuration = 0.7,
    IntroCHLogoFadeAnimationDuration = 2,
    IntroCHLogoFadeAnimationStartY = 609,
    IntroLCCLogoAnimationDuration = 1.067,
    IntroLogoStarHighlightSprite = "LogoStarHighlight",
    IntroLogoStarHighlightPosition = { X = 698, Y = 522 },
    IntroLogoStarHighlightAnimationDuration = 0.5334,
    IntroDelusionADVAnimationDuration = 1.4,
    IntroDelusionADVSprites = {},
    IntroDelusionADVPositions = {
        { X = 120, Y = 603 },
        { X = 156, Y = 603 },
        { X = 190, Y = 603 },
        { X = 225, Y = 603 },
        { X = 258, Y = 603 },
        { X = 292, Y = 603 },
        { X = 322, Y = 603 }
    },
    IntroDelusionADVHighlightAnimationDuration = 0.5334,
    IntroSeiraAnimationDuration = 0.8,
    IntroLogoPopOutAnimationDuration = 0.2,
    IntroLogoPopOutAnimationDelay = 0.5334,
    IntroLogoPopOutOffset = { X = -9, Y = -9 },
    IntroCopyrightAnimationDuration = 0.25,
    LCCLogoSprites = {
        "LoveLogo",
        "ChuLeftLogo",
        "ChuRightLogo",
        "ExclMarkLogo"
    },
    LCCLogoPositions = {
        { X = 352, Y = 504 },
        { X = 529, Y = 504 },
        { X = 750, Y = 474 },
        { X = 921, Y = 474 }
    },
    DelusionADVSpriteCount = 7,
    DelusionADVSprites = {},
    DelusionADVUnderSprite = "DelusionADVUnder",
    DelusionADVSprite = "DelusionADV",
    DelusionADVPosition = { X = 114, Y = 591 },
    DelusionADVPopoutOffset = { X = -3, Y = -4.5 },
    SeiraUnderSprite = "SeiraUnder",
    SeiraUnderPosition = { X = 1099.5, Y = 0 },
    SeiraPopoutOffset = { X = -72, Y = -72 },
    SeiraSprite = "Seira",
    SeiraPosition = { X = 1092, Y = -70.5 },
    CHLogoSprite = "CHLogo",
    CHLogoPosition = { X = 91.5, Y = 418.5 },
    LCCLogoUnderSprite = "LCCLogoUnder",
    LCCLogoUnderPosition = { X = 361.5, Y = 490.5 },
    StarLogoSprite = "StarLogo",
    StarLogoPosition = { X = 698, Y = 522 },
    CopyrightTextSprite = "CopyrightText",
    CopyrightTextPosition = { X = 108, Y = 1012.5 },
    SpinningCircleSprite = "SpinningCircle",
    SpinningCirclePosition = { X = 915.75, Y = -428.25 },
    SpinningCircleAnimationDuration = 15,
    SpinningCircleFlashingAnimationDuration = 0.833,
    ItemHighlightSprite = "TitleMenuItemHighlight",
    ItemHighlightOffset = { X = 109, Y = 10 },
    ItemPadding = 60,
    ItemYBase = 103.5,
    ItemFadeInDuration = 0.3,
    ItemFadeOutDuration = 0.6,
    SecondaryItemFadeInDuration = 0.2,
    SecondaryItemFadeOutDuration = 0.2,
    PrimaryFadeInDuration = 16 / 60,
    PrimaryFadeOutDuration = 32 / 60,
    SecondaryFadeInDuration = 8 / 60,
    SecondaryFadeOutDuration = 8 / 60,
    ItemHyperUpLine = "TitleMenuItemHyperUpLine",
    ItemSuperUpLine = "TitleMenuItemSuperUpLine",
    ItemUpLine = "TitleMenuItemUpLine",
    ItemStraightLine = "TitleMenuItemStraightLine",
    ItemDownLine = "TitleMenuItemDownLine",
    ItemSuperDownLine = "TitleMenuItemSuperDownLine",
    ItemLoadQuickSprite = "TitleMenuItemLoadQuick",
    SecondaryItemX = 480,
    ItemLoadY = 163.5,
    ItemLoadQuickY = 124.5,
    ItemLoadSprite = "TitleMenuItemLoad",
    ItemLoadQuickHighlightedSprite = "TitleMenuItemLoadQuickHighlighted",
    ItemLoadHighlightedSprite = "TitleMenuItemLoadHighlighted",
    SecondaryItemHighlightSprite = "TitleMenuSecondaryItemHighlight",
    ItemClearListY = 106.5,
    ItemCGLibraryY = 145.5,
    ItemSoundLibraryY = 184.5,
    ItemMovieLibraryY = 223.5,
    ItemTipsY = 262.5,
    ItemTrophyY = 301.5,
    ItemConfigY = 244.5,
    ItemSystemSaveY = 283.5,
    SecondaryItemHighlightX = 429,
    SecondaryMenuPaddingY = 39,
    SecondaryMenuLoadOffsetY = 114,
    SecondaryMenuLineX = 361.5,
    SecondaryMenuLoadLineY = 139.5,
    SecondaryMenuLoadQuickLineY = 178.5,
    SecondaryMenuExtraClearY = 121.5,
    SecondaryMenuExtraCGY = 160.5,
    SecondaryMenuExtraSoundY = 199.5,
    SecondaryMenuExtraMovieY = 238.5,
    SecondaryMenuExtraTipsY = 238.5,
    SecondaryMenuExtraTrophyY = 238.5,
    SecondaryMenuSystemConfigY = 259.5,
    SecondaryMenuSystemSaveY = 298.5,
    MenuEntriesNum = 13,
    MenuEntriesSprites = {},
    MenuEntriesHighlightedSprites = {},
    ExitSprite = "ExitSprite",
    ExitHighlightSprite = "ExitHighlightSprite",
    LineNum = 6,
    LineEntriesSprites = {},
    NewButtonId = 0,
    -- 13 save system-

    LoadButtonId = 2,
    QLoadButtonId = 3,
    CGButtonId = 5,
    SoundButtonId = 6,
    MovieButtonId = 7,
    EndingListButtonId = 8,
    TipsButtonId = 9,
    SystemButtonId = 10,
    HelpButtonId = 11,

    CostumeButtonId = 14, -- ?


    ConfigButtonId = 10,
    ExtraButtonId = 2,


    PrimLoadButtonId = 1,
    TrophyButtonId = 5,
};

root.TitleMenu.IntroDelusionADVSpriteCount = #root.TitleMenu.IntroDelusionADVPositions;
for i = 1, root.TitleMenu.IntroDelusionADVSpriteCount do
    root.TitleMenu.IntroDelusionADVSprites[i] = "IntroDelusionADV" .. i
end

for i = 0, 4 do
    root.Sprites["TitleMenuEntry" .. i] = {
        Sheet = "Title",
        Bounds = {
            X = 1726,
            Y = 187 + i * 38,
            Width = 282,
            Height = 35
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "TitleMenuEntry" .. i;
end

for i = 0, 7 do
    root.Sprites["TitleMenuEntry" .. (i + 5)] = {
        Sheet = "Title",
        Bounds = {
            X = 2053,
            Y = 1026 + i * 33,
            Width = 324,
            Height = 30
        }
    };
    root.TitleMenu.MenuEntriesSprites[#root.TitleMenu.MenuEntriesSprites + 1] = "TitleMenuEntry" .. (i + 5);
end

for i = 0, 4 do
    root.Sprites["TitleMenuEntryHighlighted" .. i] = {
        Sheet = "Title",
        Bounds = {
            X = 1726,
            Y = 1 + i * 38,
            Width = 282,
            Height = 35
        }
    };
    root.TitleMenu.MenuEntriesHighlightedSprites[#root.TitleMenu.MenuEntriesHighlightedSprites + 1] =
        "TitleMenuEntryHighlighted" .. i;
end

for i = 0, 7 do
    root.Sprites["TitleMenuEntryHighlighted" .. (i + 5)] = {
        Sheet = "Title",
        Bounds = {
            X = 1726.5,
            Y = 1026 + i * 33,
            Width = 324,
            Height = 30
        }
    };
    root.TitleMenu.MenuEntriesHighlightedSprites[#root.TitleMenu.MenuEntriesHighlightedSprites + 1] =
        "TitleMenuEntryHighlighted" .. (i + 5);
end

root.Sprites["TitleMenuPressToStart"] = {
    Sheet = "Title",
    Bounds = { X = 7, Y = 1386, Width = 450, Height = 34 },
};

root.Sprites["DelusionADVUnder"] = {
    Sheet = "Title",
    Bounds = { X = 2795, Y = 1162, Width = 240, Height = 37 },
};


root.Sprites["DelusionADV"] = {
    Sheet = "Title",
    Bounds = { X = 2795, Y = 1096, Width = 240, Height = 37 },
};

root.Sprites["IntroDelusionADV1"] = {
    Sheet = "Title",
    Bounds = {  X = 2795, Y = 1026, Width = 35, Height = 29 },
};

root.Sprites["IntroDelusionADV2"] = {
    Sheet = "Title",
    Bounds = { X = 2831, Y = 1027, Width = 35, Height = 28 },
};

root.Sprites["IntroDelusionADV3"] = {
    Sheet = "Title",
    Bounds = { X = 2867, Y = 1027, Width = 34, Height = 28 },
};

root.Sprites["IntroDelusionADV4"] = {
    Sheet = "Title",
    Bounds = { X = 2903, Y = 1026, Width = 34, Height = 29 },
};

root.Sprites["IntroDelusionADV5"] = {
    Sheet = "Title",
    Bounds = { X = 2938, Y = 1027, Width = 34, Height = 28  },
};

root.Sprites["IntroDelusionADV6"] = {
    Sheet = "Title",
    Bounds = { X = 2973, Y = 1027, Width = 32, Height = 28 },
};

root.Sprites["IntroDelusionADV7"] = {
    Sheet = "Title",
    Bounds = { X = 3007, Y = 1027, Width = 33, Height = 28  },
};

root.Sprites["SeiraUnder"] = {
    Sheet = "Title",
    Bounds = { X = 836, Y = 0, Width = 890, Height = 1156  },
};

root.Sprites["Seira"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 0, Width = 832, Height = 1156 },
};

root.Sprites["CHLogo"] = {
    Sheet = "Title",
    Bounds = { X = 2, Y = 1159, Width = 873, Height = 169 },
};

root.Sprites["LCCLogoUnder"] = {
    Sheet = "Title",
    Bounds = { X = 901, Y = 1160, Width = 681, Height = 179 },
};

root.Sprites["ChuLeftLogo"] = {
    Sheet = "Title",
    Bounds = { X = 724.5, Y = 1372.5, Width = 204, Height = 162 },
};

root.Sprites["ChuRightLogo"] = {
    Sheet = "Title",
    Bounds = { X = 1039.5, Y = 1342.5, Width = 204, Height = 192 },
};

root.Sprites["LoveLogo"] = {
    Sheet = "Title",
    Bounds = { X = 511.5, Y = 1372.5, Width = 210, Height = 162 },
};

root.Sprites["StarLogo"] = {
    Sheet = "Title",
    Bounds = { X = 932, Y = 1391, Width = 98, Height = 102},
};

root.Sprites["ExclMarkLogo"] = {
    Sheet = "Title",
    Bounds = { X = 1252, Y = 1351, Width = 111, Height = 120 },
};

root.Sprites["CopyrightText"] = {
    Sheet = "Title",
    Bounds = { X = 2053, Y = 1427, Width = 484, Height = 20 },
    BaseScale = { X = 4 / 3, Y = 4 / 3 }
};

root.Sprites["SpinningCircle"] = {
    Sheet = "Title",
    Bounds = { X = 2049, Y = 2, Width = 1023, Height = 1023 },
};

root.Sprites["TitleMenuIntroBackground"] = {
    Sheet = "TitleBg1",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 },
};

root.Sprites["IntroSmallStar"] = {
    Sheet = "Title",
    Bounds = { X = 1729, Y = 875, Width = 69, Height = 68  },
};

root.Sprites["IntroBigStar"] = {
    Sheet = "Title",
    Bounds = { X = 1733, Y = 592, Width = 269, Height = 256 },
};

root.Sprites["LogoStarHighlight"] = {
    Sheet = "Title",
    Bounds = { X = 1804, Y = 1391, Width = 98, Height = 102  }
}

root.Sprites["IntroBrightGreenHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 2304, Y = 0, Width = 384, Height = 384 },
};

root.Sprites["IntroSunHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 0, Y = 0, Width = 768, Height = 768 },
};

root.Sprites["IntroGrayHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 2304, Y = 384, Width = 384, Height = 384 },
};

root.Sprites["IntroCrescentRainbowHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 0, Y = 768, Width = 768, Height = 768 },
};

root.Sprites["IntroBlueHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1920, Y = 384, Width = 384, Height = 384  },
};

root.Sprites["IntroWhiteHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1920, Y = 0, Width = 384, Height = 384 },
};

root.Sprites["IntroBrownHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1536, Y = 384, Width = 384, Height = 384 },
};

root.Sprites["IntroDiamondHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 768, Y = 768, Width = 768, Height = 768 },
};

root.Sprites["IntroDarkGreenHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1536, Y = 0, Width = 384, Height = 384  },
};

root.Sprites["IntroCircularRainbowHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 768, Y = 0, Width = 768, Height = 768 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet = "TitleBg2",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 },
};

root.Sprites["TitleMenuItemHighlight"] = {
    Sheet = "Title",
    Bounds = { X = 1372.5, Y = 1426.5, Width = 366, Height = 54 },
};

root.Sprites["TitleMenuItemHyperUpLine"] = {
    Sheet = "Title",
    Bounds = { X = 2707.5, Y = 1029, Width = 76.5, Height = 120 },
};

root.Sprites["TitleMenuItemSuperUpLine"] = {
    Sheet = "Title",
    Bounds = { X = 2707.5, Y = 1164, Width = 76.5, Height = 81 },
};

root.Sprites["TitleMenuItemUpLine"] = {
    Sheet = "Title",
    Bounds = { X = 2707.5, Y = 1267.5, Width = 76.5, Height = 42 },
};

root.Sprites["TitleMenuItemStraightLine"] = {
    Sheet = "Title",
    Bounds = { X = 2707.5, Y = 1332, Width = 76.5, Height = 3 },
};

root.Sprites["TitleMenuItemDownLine"] = {
    Sheet = "Title",
    Bounds = { X = 2707.5, Y = 1365, Width = 76.5, Height = 42 },
};

root.Sprites["TitleMenuItemSuperDownLine"] = {
    Sheet = "Title",
    Bounds = { X = 2707.5, Y = 1438.5, Width = 76.5, Height = 81 },
};

root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemHyperUpLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemSuperUpLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemUpLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemStraightLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemDownLine";
root.TitleMenu.LineEntriesSprites[#root.TitleMenu.LineEntriesSprites + 1] = "TitleMenuItemSuperDownLine";

root.Sprites["TitleMenuItemLoadQuick"] = {
    Sheet = "Title",
    Bounds = { X = 2053.5, Y = 1026, Width = 324, Height = 30 },
};

root.Sprites["TitleMenuItemLoad"] = {
    Sheet = "Title",
    Bounds = { X = 2053.5, Y = 1059, Width = 324, Height = 30 },
};

root.Sprites["TitleMenuItemLoadQuickHighlighted"] = {
    Sheet = "Title",
    Bounds = { X = 1726.5, Y = 1026, Width = 324, Height = 30 },
};

root.Sprites["TitleMenuItemLoadHighlighted"] = {
    Sheet = "Title",
    Bounds = { X = 1726.5, Y = 1059, Width = 324, Height = 30 },
};

root.Sprites["TitleMenuSecondaryItemHighlight"] = {
    Sheet = "Title",
    Bounds = { X = 1372.5, Y = 1483.5, Width = 427.5, Height = 51 },
};

root.Sprites["ExitSprite"] = {
    Sheet = "Title",
    Bounds = { X = 2055, Y = 1416, Width = 108, Height = 34.5 },
};

root.Sprites["ExitHighlightSprite"] = {
    Sheet = "Title",
    Bounds = { X = 2055, Y = 1380, Width = 108, Height = 34.5 },
};
