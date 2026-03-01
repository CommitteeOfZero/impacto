local languageSuffix = root.Language
if root.Language == "Japanese" then languageSuffix = "" end

root.TitleMenu = {
    Type = TitleMenuType.CHLCC,
    DrawType = DrawComponentType.SystemMenu,
    PressToStartX = 72,
    PressToStartY = 595,
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
    IntroBouncingStarAnimationDuration = 3.73;
    IntroExplodingStarSprite = "IntroSmallStar",
    IntroExplodingStarAnimationDuration = 1.067,
    IntroExplodingStarAnimationRotationDuration = 0.5,
    IntroExplodingStarAnimationDistance = 294,
    IntroFallingStarSprite = "IntroBigStar",
    IntroFallingStarsAnimationDuration = 3,
    IntroFallingStarsAnimationDistance = 2546,
    IntroFallingStarsAnimationDirection = { X = -1, Y = 1 },
    IntroFallingStarsAnimationRotationDuration = 0.7,
    IntroCHLogoFadeAnimationDuration = 2,
    IntroCHLogoFadeAnimationStartY = 406,
    IntroLCCLogoAnimationDuration = 1.067,
    IntroLogoStarHighlightSprite = "LogoStarHighlight";
    IntroLogoStarHighlightPosition = { X = 471, Y = 342 };
    IntroLogoStarHighlightAnimationDuration = 0.5334,
    IntroDelusionADVAnimationDuration = 1.4;
    IntroDelusionADVSprites = {},
    IntroDelusionADVPositions = root.Language == "Japanese" and {
        { X = 80,  Y = 402 },
        { X = 104, Y = 402 },
        { X = 127, Y = 402 },
        { X = 150, Y = 402 },
        { X = 172, Y = 402 },
        { X = 195, Y = 402 },
        { X = 215, Y = 402 }
    } or {
        { X = 78,  Y = 400 },
        { X = 78,  Y = 418 },
        { X = 174, Y = 400 },
        { X = 194, Y = 400 },
        { X = 211, Y = 400 }
    },
    IntroDelusionADVHighlightAnimationDuration = 0.5334,
    IntroSeiraAnimationDuration = 0.8,
    IntroLogoPopOutAnimationDuration = 0.2,
    IntroLogoPopOutAnimationDelay = 0.5334,
    IntroLogoPopOutOffset = { X = -6, Y = -6 },
    IntroCopyrightAnimationDuration = 0.25,
    LCCLogoSprites = {
        "LoveLogo",
        "ChuLeftLogo",
        "ChuRightLogo",
        "ExclMarkLogo"
    };
    LCCLogoPositions = {
        root.Language == "Japanese"
            and { X = 235, Y = 336 }
            or { X = 231, Y = 333 },
        { X = 353, Y = 336 },
        { X = 500, Y = 316 },
        { X = 614, Y = 316 }
    },
    DelusionADVSpriteCount = 7,
    DelusionADVSprites = {},
    DelusionADVUnderSprite = "DelusionADVUnder" .. languageSuffix,
    DelusionADVSprite = "DelusionADV" .. languageSuffix,
    DelusionADVPosition = root.Language == "Japanese"
        and { X = 76, Y = 394 }
        or { X = 76, Y = 397 },
    DelusionADVPopoutOffset = { X = -2, Y = -3 },
    SeiraUnderSprite = "SeiraUnder",
    SeiraUnderPosition = { X = 733, Y = 0 },
    SeiraPopoutOffset = { X = -48, Y = -48 };
    SeiraSprite = "Seira",
    SeiraPosition = { X = 728, Y = -47 },
    CHLogoSprite = "CHLogo",
    CHLogoPosition = { X = 61, Y = 279 },
    LCCLogoUnderSprite = "LCCLogoUnder",
    LCCLogoUnderPosition = root.Language == "Japanese"
        and { X = 241, Y = 327 }
        or { X = 242, Y = 328 },
    StarLogoSprite = "StarLogo",
    StarLogoPosition = { X = 465, Y = 316 },
    CopyrightTextSprite = "CopyrightText",
    CopyrightTextPosition = { X = 72, Y = 675 },
    SpinningCircleSprite = "SpinningCircle",
    SpinningCirclePosition = { X = 610.5, Y = -285.5 },
    SpinningCircleAnimationDuration = 15,
    SpinningCircleFlashingAnimationDuration = 0.833;
    ItemHighlightSprite = "TitleMenuItemHighlight",
    ItemHighlightOffset = { X = 73, Y = 7 },
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
    ExitSprite = "ExitSprite",
    ExitHighlightSprite = "ExitHighlightSprite",
    LineNum = 6,
    LineEntriesSprites = {}
};

root.TitleMenu.IntroDelusionADVSpriteCount = #root.TitleMenu.IntroDelusionADVPositions;
for i = 1, root.TitleMenu.IntroDelusionADVSpriteCount do
    root.TitleMenu.IntroDelusionADVSprites[i] = "IntroDelusionADV" .. i .. languageSuffix
end

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

root.Sprites["DelusionADVUnder"] = {
    Sheet = "Title",
    Bounds = { X = 1863, Y = 772, Width = 163, Height = 27 },
};

root.Sprites["DelusionADVUnderEnglish"] = {
    Sheet = "Title",
    Bounds = { X = 1862, Y = 785, Width = 154, Height = 33 },
};

root.Sprites["DelusionADV"] = {
    Sheet = "Title",
    Bounds = { X = 1863, Y = 728, Width = 163, Height = 27 },
};

root.Sprites["DelusionADVEnglish"] = {
    Sheet = "Title",
    Bounds = { X = 1862, Y = 734, Width = 154, Height = 33 },
};

root.Sprites["IntroDelusionADV1"] = {
    Sheet = "Title",
    Bounds = { X = 1863, Y = 684, Width = 23, Height = 20 },
};

root.Sprites["IntroDelusionADV2"] = {
    Sheet = "Title",
    Bounds = { X = 1888, Y = 684, Width = 22, Height = 20 },
};

root.Sprites["IntroDelusionADV3"] = {
    Sheet = "Title",
    Bounds = { X = 1912, Y = 684, Width = 22, Height = 20 },
};

root.Sprites["IntroDelusionADV4"] = {
    Sheet = "Title",
    Bounds = { X = 1936, Y = 684, Width = 21, Height = 20 },
};

root.Sprites["IntroDelusionADV5"] = {
    Sheet = "Title",
    Bounds = { X = 1959, Y = 684, Width = 22, Height = 20 },
};

root.Sprites["IntroDelusionADV6"] = {
    Sheet = "Title",
    Bounds = { X = 1983, Y = 684, Width = 20, Height = 20 },
};

root.Sprites["IntroDelusionADV7"] = {
    Sheet = "Title",
    Bounds = { X = 2005, Y = 684, Width = 21, Height = 20 },
};

root.Sprites["IntroDelusionADV1English"] = {
    Sheet = "Title",
    Bounds = { X = 1862, Y = 683, Width = 90, Height = 18 },
};

root.Sprites["IntroDelusionADV2English"] = {
    Sheet = "Title",
    Bounds = { X = 1862, Y = 701, Width = 90, Height = 15 },
};

root.Sprites["IntroDelusionADV3English"] = {
    Sheet = "Title",
    Bounds = { X = 1958, Y = 683, Width = 19, Height = 33 },
};

root.Sprites["IntroDelusionADV4English"] = {
    Sheet = "Title",
    Bounds = { X = 1978, Y = 683, Width = 17, Height = 33 },
};

root.Sprites["IntroDelusionADV5English"] = {
    Sheet = "Title",
    Bounds = { X = 1996, Y = 683, Width = 19, Height = 33 },
};

root.Sprites["SeiraUnder"] = {
    Sheet = "Title",
    Bounds = { X = 555, Y = 1, Width = 594, Height = 768 },
};

root.Sprites["Seira"] = {
    Sheet = "Title",
    Bounds = { X = 1, Y = 1, Width = 552, Height = 768 },
};

root.Sprites["CHLogo"] = {
    Sheet = "Title",
    Bounds = { X = 1, Y = 771, Width = 592, Height = 115 },
};

root.Sprites["LCCLogoUnder"] = {
    Sheet = "Title",
    Bounds = { X = 597, Y = 771, Width = 462, Height = 122 },
};

root.Sprites["LCCLogoUnderEnglish"] = {
    Sheet = "Title",
    Bounds = { X = 591, Y = 771, Width = 468, Height = 122 },
};

root.Sprites["ChuLeftLogo"] = {
    Sheet = "Title",
    Bounds = { X = 483, Y = 915, Width = 136, Height = 108 },
};

root.Sprites["ChuRightLogo"] = {
    Sheet = "Title",
    Bounds = { X = 693, Y = 895, Width = 136, Height = 128 },
};

root.Sprites["LoveLogo"] = {
    Sheet = "Title",
    Bounds = { X = 341, Y = 915, Width = 140, Height = 108 },
};

root.Sprites["StarLogo"] = {
    Sheet = "Title",
    Bounds = { X = 621, Y = 895, Width = 70, Height = 128 },
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
    Sheet = "TitleBg1",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 },
};

root.Sprites["IntroSmallStar"] = {
    Sheet = "Title",
    Bounds = { X = 1153, Y = 534, Width = 45, Height = 44 },
};

root.Sprites["IntroBigStar"] = {
    Sheet = "Title",
    Bounds = { X = 1156, Y = 345, Width = 178, Height = 170 },
};

root.Sprites["LogoStarHighlight"] = {
    Sheet = "Title",
    Bounds = { X = 1202, Y = 915, Width = 70, Height = 108 }
}

root.Sprites["IntroBrightGreenHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1536, Y = 0, Width = 256, Height = 256 },
};

root.Sprites["IntroSunHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 0, Y = 0, Width = 512, Height = 512 },
};

root.Sprites["IntroGrayHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1536, Y = 256, Width = 256, Height = 256 },
};

root.Sprites["IntroCrescentRainbowHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 0, Y = 512, Width = 512, Height = 512 },
};

root.Sprites["IntroBlueHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1280, Y = 256, Width = 256, Height = 256 },
};

root.Sprites["IntroWhiteHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1280, Y = 0, Width = 256, Height = 256 },
};

root.Sprites["IntroBrownHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1024, Y = 256, Width = 256, Height = 256 },
};

root.Sprites["IntroDiamondHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 512, Y = 512, Width = 512, Height = 512 },
};

root.Sprites["IntroDarkGreenHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 1024, Y = 0, Width = 256, Height = 256 },
};

root.Sprites["IntroCircularRainbowHighlight"] = {
    Sheet = "Highlights",
    Bounds = { X = 512, Y = 0, Width = 512, Height = 512 },
};

root.Sprites["TitleMenuBackground"] = {
    Sheet = "TitleBg2",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 },
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

root.Sprites["ExitSprite"] = {
    Sheet = "Title",
    Bounds = { X = 1370, Y = 944, Width = 72, Height = 23 },
};

root.Sprites["ExitHighlightSprite"] = {
    Sheet = "Title",
    Bounds = { X = 1370, Y = 920, Width = 72, Height = 23 },
};