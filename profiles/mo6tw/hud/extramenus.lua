root.ExtraMenus = {
    ClearListMenu = {
        DrawType = DrawComponentType.PlayData,
        Type = ClearListMenuType.MO6TW,
        BackgroundSprite = "ClearListBackground",
        WindowSprite = "ClearListWindow",
        WindowPosition = { X = 85, Y = 115 },
        WindowSpritePartLeft = "ClearListSceneWindowLeft",
        WindowSpritePartRight = "ClearListSceneWindowRight",
        FontSize = 32,
        SeparatorTable = 0,
        SeparatorEntry = 13,
        LabelPosition = { X = 790, Y = 32 },
        ClearListLabel = "ClearListLabel",
        EndingsLabelPosition = { X = 64, Y = 116 },
        EndingsLabel = "EndingsLabel",
        EndingCountPosition =  { X = 560, Y = 172 },
        ScenesLabelPosition = { X = 128, Y = 228 },
        ScenesLabel = "ScenesLabel",
        SceneCountPosition =  { X = 560, Y = 284 },
        CompletionLabelPosition = { X = 96, Y = 340 },
        CompletionLabel = "CompletionLabel",
        CompletionPosition =  { X = 560, Y = 396 },
        AlbumLabelPosition = { X = 128, Y = 452 },
        AlbumLabel = "AlbumLabel",
        AlbumCountPosition = { X = 560, Y = 508 },
        PlayTimeLabelPosition = { X = 64, Y = 564 },
        PlayTimeLabel = "PlayTimeLabel",
        PlayTimeTextTable = 0,
        PlayTimeSecondsTextEntry = 9,
        PlayTimeMinutesTextEntry = 8,
        PlayTimeHoursTextEntry = 10,
        PlayTimeSecondsTextPosition =  { X = 562, Y = 620 },
        PlayTimeMinutesTextPosition =  { X = 514, Y = 620 },
        PlayTimeHoursTextPosition =  { X = 466, Y = 620 },
        PlayTimeSecondsPosition =  { X = 560, Y = 620 },
        PlayTimeMinutesPosition =  { X = 512, Y = 620 },
        PlayTimeHoursPosition =  { X = 464, Y = 620 },
        ClearListColorIndex = 0,
        ClearListTextBackground = "ClearListTextBackground",
        ClearListTextBGOffset = { X = 0, Y = 48 },
        EndingListLabel = "EndingListLabel",
        EndingCount = 14,
        EndingsListNumberInitialPosition = { X = 108, Y = 142 },
        EndingsListTextInitialPosition = { X = 156, Y = 142 },
        EndingsListTextMargin = { X = 0, Y = 36 },
        EndingsListTextFontSize = 28,
        EndingsListTextLockedTable = 0,
        EndingsListTextLockedEntry = 15,
        EndingsListTextTable = 3,
        EndingsListTextColorIndex = 0,
        SceneTitleLabel = "SceneTitleLabel",
        SceneCount = 158,
        SceneListNumberInitialPosition = { X = 108, Y = 140 },
        SceneListTextInitialPosition = { X = 156, Y = 140 },
        SceneListTextMargin = { X = 0, Y = 36 },
        SceneListFontSize = 24,
        SceneListTextTable = 4,
        SceneTitleItemsRenderingBounds = { X = 90, Y = 120, Width = 795, Height = 538 },
        SceneListColorIndex = 0,
        SceneTitleLockedTable = 0,
        SceneTitleLockedEntry = 15,
        SceneTitleItemsWidth = 838,
        ScrollbarStart = 140,
        ScrollAreaHeight = 500,
        ScrollbarTrackSprite = "ClearListScrollbarTrack",
        ScrollbarThumbSprite = "ClearListScrollbarThumb",
        ScrollbarPosition = { X = 870, Y = 130 },
        ArrowsAnimationDuration = 0.3,
        ArrowLeft = "ClearListArrowLeft",
        ArrowLeftPosition = { X = 58, Y = 337 },
        ArrowRight = "ClearListArrowRight",
        ArrowRightPosition = { X = 1190, Y = 337 },
        FadeInDuration = 0.2,
        FadeOutDuration = 0.2
    },
    MovieMenu = {
        DrawType = DrawComponentType.ExtrasMovieMode,
        Type = MovieMenuType.MO6TW,
        BackgroundSprite = "MovieMenuBackground",
        FirstOPTopPartSprite = "MovieMenuFirstOPTopPart",
        FirstOPBottomPartSprite = "MovieMenuFirstOPBottomPart",
        SecondOPTopPartSprite = "MovieMenuSecondOPTopPart",
        SecondOPBottomPartSprite = "MovieMenuSecondOPBottomPart",
        UnlockedMovieThumbnailSprites = {},
        LockedMovieThumbnailSprites = {},
        SelectionHighlightTopLeft = "MovieMenuSelectionHighlightTopLeft",
        SelectionHighlightTopRight = "MovieMenuSelectionHighlightTopRight",
        SelectionHighlightBottomLeft = "MovieMenuSelectionHighlightBottomLeft",
        SelectionHighlightBottomRight = "MovieMenuSelectionHighlightBottomRight",
        ItemCount = 12,
        ItemsPerRow = 4,
        InitialItemPosition = { X = 151, Y = 80 },
        ItemOffset = { X = 250, Y = 206 },
        HighlightAnimationDuration = 0.5,
        HighlightTopLeftOffset = { X = 8, Y = 8 },
        HighlightTopRightOffset = { X = 12, Y = 9 },
        HighlightBottomLeftOffset = { X = 8, Y = 14 },
        HighlightBottomRightOffset = { X = 13, Y = 14 },
        FadeInDuration = 0.2,
        FadeOutDuration = 0.2
    },
    ActorsVoiceMenu = {
        DrawType = DrawComponentType.ExtrasActorsVoice,
        Type = ActorsVoiceMenuType.MO6TW,
        BackgroundSprite = "ActorsVoiceBackground",
        UnlockedSprites = {},
        LockedSprites = {},
        UnlockedHighlightedSprites = {},
        LockedHighlightedSprites = {},
        InitialItemPosition = { X = 256, Y = 112 },
        ItemOffset = { X = 0, Y = 60 },
        CharacterBackgroundBufferId = 4,
        FadeInDuration = 0.2,
        FadeOutDuration = 0.2
    },
    MusicMenu = {
        DrawType = DrawComponentType.ExtrasActorsVoice,
        Type = MusicMenuType.MO6TW,
        BackgroundSprite = "MusicMenuBackground",
        ItemsWindow = "MusicMenuItemsWindow",
        ItemsWindowPosition = { X = 685, Y = 233 },
        ItemsWindowRenderingBounds = { X = 702, Y = 242, Width = 420, Height = 428 },
        MusicListMargin = { X = 0, Y = 26 },
        MusicListInitialPosition = { X = 723, Y = 252 },
        PlaybackWindow = "MusicMenuPlaybackWindow",
        PlaybackWindowPosition = { X = 217, Y = 98 },
        PlaybackModeLabels = {},
        PlaybackModeLabelPosition = { X = 886, Y = 178 },
        CurrentlyPlayingLabelPosition = { X = 337, Y = 143 },
        Thumbnails = {},
        ThumbnailPosition = { X = 221, Y = 297 },
        ItemNames = {},
        ItemNameHighlightOffset = { X = 0, Y = -4 },
        Playlist =
            {
                30, 0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 8, 9, 25, 12,
                13, 14, 15, 16, 17, 18, 19, 26, 27, 28, 29, 20,
                24, 22, 31, 32, 33, 34, 35, 37, 39, 40, 36, 41
            },
        TimerChars = {},
        TimerInitialPosition = { X = 825, Y = 141 },
        TimerMargin = { X = 15, Y = 0 },
        LockedItem = "MusicMenuLockedItem",
        ScrollbarThumb = "MusicMenuScrollbarThumb",
        ScrollbarTrack = "MusicMenuScrollbarTrack",
        ScrollbarPosition = { X = 1134, Y = 244 },
        ScrollbarStart = 252,
        FadeInDuration = 0.2,
        FadeOutDuration = 0.2
    },
    AlbumMenu = {
        DrawType = DrawComponentType.Album,
        Type = MusicMenuType.MO6TW,
        CharacterButtons = {},
        HighlightedCharacterButtons = {},
        BackgroundSprite = "AlbumMenuBackground",
        InitialButtonPosition = { X = 149, Y = 115 },
        ButtonOddX = 421,
        ButtonEvenX = 149,
        ButtonMargin = { X = 0, Y = 90 },
        HighlightAnimationDuration = 0.5,
        YunoButtonIdx = 3,
        SuzuButtonIdx = 4,
        CharacterPortraits = {},
        OthersPortraitTopPart = "AlbumMenuOthersPortraitTop",
        OthersPortraitBottomPart = "AlbumMenuOthersPortraitBottom",
        PortraitPosition = { X = 1282, Y = 80 },
        OthersPortraitPosition = { X = 1282, Y = 124 },
        ThumbnailsPerRow = 3,
        ThumbnailsPerColumn = 3,
        Thumbnails = {},
        ThumbnailOffsets = {0, 20, 37, 53, 65, 78},
        LockedThumbnail = "AlbumMenuLockedThumbnail",
        ThumbnailBorder = "AlbumMenuThumbnailBorder",
        ThumbnailHighlightTopLeft = "AlbumMenuThumbnailHighlightTopLeft",
        ThumbnailHighlightTopRight = "AlbumMenuThumbnailHighlightTopRight",
        ThumbnailHighlightBottomLeft = "AlbumMenuThumbnailHighlightBottomLeft",
        ThumbnailHighlightBottomRight = "AlbumMenuThumbnailHighlightBottomRight",
        ThumbnailGridFirstPosition = { X = 154, Y = 130 },
        ThumbnailGridMargin = { X = 262, Y = 188 },
        ThumbnailGridBounds = { X = 127, Y = 106, Width = 810, Height = 550 },
        ArrowsAnimationDuration = 0.2,
        ArrowUp = "AlbumMenuArrowUp",
        ArrowUpPosition = { X = 519, Y = 84 },
        ArrowDown = "AlbumMenuArrowDown",
        ArrowDownPosition = { X = 519, Y = 665 },
        ThumbnailButtonBorderOffset = { X = -8, Y = -9 },
        ThumbnailButtonTextFontSize = 21,
        ThumbnailButtonTextColorIndex = 0,
        ThumbnailButtonTextOffset = { X = -6, Y = -6 },
        FadeInDuration = 0.2,
        FadeOutDuration = 0.2
    }
};

-- Clear list

root.Sprites["ClearListBackground"] = {
    Sheet = "ClearList",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 }
};

root.Sprites["ClearListLabel"] = {
    Sheet = "ClearList",
    Bounds = { X = 1291, Y = 659, Width = 410, Height = 62 }
};

root.Sprites["EndingsLabel"] = {
    Sheet = "ClearList",
    Bounds = { X = 105, Y = 725, Width = 576, Height = 48 }
};

root.Sprites["ScenesLabel"] = {
    Sheet = "ClearList",
    Bounds = { X = 105, Y = 774, Width = 576, Height = 48 }
};

root.Sprites["CompletionLabel"] = {
    Sheet = "ClearList",
    Bounds = { X = 105, Y = 823, Width = 576, Height = 48 }
};

root.Sprites["AlbumLabel"] = {
    Sheet = "ClearList",
    Bounds = { X = 105, Y = 872, Width = 576, Height = 48 }
};

root.Sprites["PlayTimeLabel"] = {
    Sheet = "ClearList",
    Bounds = { X = 105, Y = 921, Width = 576, Height = 48 }
};

root.Sprites["ClearListTextBackground"] = {
    Sheet = "ClearList",
    Bounds = { X = 105, Y = 970, Width = 576, Height = 48 }
};

root.Sprites["EndingListLabel"] = {
    Sheet = "ClearList",
    Bounds = { X = 1291, Y = 723, Width = 410, Height = 62 }
};

root.Sprites["SceneTitleLabel"] = {
    Sheet = "ClearList",
    Bounds = { X = 1291, Y = 595, Width = 410, Height = 62 }
};

root.Sprites["ClearListWindow"] = {
    Sheet = "ClearList",
    Bounds = { X = 1291, Y = 1, Width = 638, Height = 554 }
};

root.Sprites["ClearListSceneWindowLeft"] = {
    Sheet = "ClearList",
    Bounds = { X = 1291, Y = 1, Width = 538, Height = 554 }
};

root.Sprites["ClearListSceneWindowRight"] = {
    Sheet = "ClearList",
    Bounds = { X = 1629, Y = 1, Width = 300, Height = 554 }
};

root.Sprites["ClearListScrollbarThumb"] = {
    Sheet = "ClearList",
    Bounds = { X = 65, Y = 725, Width = 38, Height = 46 },
};

root.Sprites["ClearListScrollbarTrack"] = {
    Sheet = "ClearList",
    Bounds = { X = 1939, Y = 17, Width = 8, Height = 514 },
};

root.Sprites["ClearListArrowLeft"] = {
    Sheet = "ClearList",
    Bounds = { X = 1, Y = 725, Width = 30, Height = 46 },
};

root.Sprites["ClearListArrowRight"] = {
    Sheet = "ClearList",
    Bounds = { X = 33, Y = 725, Width = 30, Height = 46 },
};

-- Movie menu

root.Sprites["MovieMenuBackground"] = {
    Sheet = "Movie",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 }
};

root.Sprites["MovieMenuFirstOPTopPart"] = {
    Sheet = "Movie",
    Bounds = { X = 465, Y = 913, Width = 230, Height = 95 }
};

root.Sprites["MovieMenuFirstOPBottomPart"] = {
    Sheet = "Movie",
    Bounds = { X = 697, Y = 913, Width = 230, Height = 95 }
};

root.Sprites["MovieMenuSecondOPTopPart"] = {
    Sheet = "Movie",
    Bounds = { X = 1, Y = 913, Width = 230, Height = 95 }
};

root.Sprites["MovieMenuSecondOPBottomPart"] = {
    Sheet = "Movie",
    Bounds = { X = 233, Y = 913, Width = 230, Height = 95 }
};

root.Sprites["MovieMenuSelectionHighlightTopLeft"] = {
    Sheet = "Movie",
    Bounds = { X = 1161, Y = 721, Width = 22, Height = 22 }
};

root.Sprites["MovieMenuSelectionHighlightTopRight"] = {
    Sheet = "Movie",
    Bounds = { X = 1185, Y = 721, Width = 22, Height = 22 }
};

root.Sprites["MovieMenuSelectionHighlightBottomLeft"] = {
    Sheet = "Movie",
    Bounds = { X = 1161, Y = 745, Width = 22, Height = 22 }
};

root.Sprites["MovieMenuSelectionHighlightBottomRight"] = {
    Sheet = "Movie",
    Bounds = { X = 1185, Y = 745, Width = 22, Height = 22 }
};

local firstX = 1281;
local firstY = 1;

for i = 1, 10 do
    root.Sprites["UnlockedMovieThumbnail" .. i] = {
        Sheet = "Movie",
        Bounds = {
            X = firstX,
            Y = firstY,
            Width = 230,
            Height = 190
        }
    };
    root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites[#root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites + 1] = "UnlockedMovieThumbnail" .. i;

    if i % 3 == 0 then
        firstX = 1281;
        firstY = firstY + 191;
    else
        firstX = firstX + 231;
    end
end

firstX = 1;
firstY = 721;

for i = 1, 10 do
    if i == 6 then
        firstX = 1513;
        firstY = 577;
    elseif i == 8 then
        firstX = 1281;
        firstY = 769;
    end

    root.Sprites["LockedMovieThumbnail" .. i] = {
        Sheet = "Movie",
        Bounds = {
            X = firstX,
            Y = firstY,
            Width = 230,
            Height = 190
        }
    };
    root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites[#root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites + 1] = "LockedMovieThumbnail" .. i;

    firstX = firstX + 231;
end

-- I don't fucking know, do you know?
-- I don't, fuck.
local temp = root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites[7];
root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites[7] = root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites[8];
root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites[8] = temp;

temp = root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites[7];
root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites[7] = root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites[8];
root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites[8] = temp;

-- Actors Voice menu

root.Sprites["ActorsVoiceBackground"] = {
    Sheet = "ActorsVoiceBackground",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 }
};

firstX = 1;
firstY = 1;

for i = 1, 8 do
    root.Sprites["LockedHighlightedSprites" .. i] = {
        Sheet = "ActorsVoice",
        Bounds = {
            X = firstX,
            Y = firstY,
            Width = 774,
            Height = 50
        }
    };
    root.ExtraMenus.ActorsVoiceMenu.LockedHighlightedSprites[#root.ExtraMenus.ActorsVoiceMenu.LockedHighlightedSprites + 1] = "LockedHighlightedSprites" .. i;

    root.Sprites["UnlockedHighlightedSprites" .. i] = {
        Sheet = "ActorsVoice",
        Bounds = {
            X = firstX + 776,
            Y = firstY,
            Width = 774,
            Height = 50
        }
    };
    root.ExtraMenus.ActorsVoiceMenu.UnlockedHighlightedSprites[#root.ExtraMenus.ActorsVoiceMenu.UnlockedHighlightedSprites + 1] = "UnlockedHighlightedSprites" .. i;

    root.Sprites["LockedSprites" .. i] = {
        Sheet = "ActorsVoice",
        Bounds = {
            X = firstX,
            Y = firstY + 448,
            Width = 774,
            Height = 50
        }
    };
    root.ExtraMenus.ActorsVoiceMenu.LockedSprites[#root.ExtraMenus.ActorsVoiceMenu.LockedSprites + 1] = "LockedSprites" .. i;

    root.Sprites["UnlockedSprites" .. i] = {
        Sheet = "ActorsVoice",
        Bounds = {
            X = firstX + 776,
            Y = firstY + 448,
            Width = 774,
            Height = 50
        }
    };
    root.ExtraMenus.ActorsVoiceMenu.UnlockedSprites[#root.ExtraMenus.ActorsVoiceMenu.UnlockedSprites + 1] = "UnlockedSprites" .. i;

    firstY = firstY + 56;
end

-- Music menu

root.Sprites["MusicMenuBackground"] = {
    Sheet = "Music",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 }
};

root.Sprites["MusicMenuItemsWindow"] = {
    Sheet = "Music",
    Bounds = { X = 1281, Y = 1, Width = 480, Height = 454 }
};

root.Sprites["MusicMenuPlaybackWindow"] = {
    Sheet = "Music",
    Bounds = { X = 1, Y = 721, Width = 954, Height = 114 }
};

firstX = 1;
firstY = 1;
local sheetIdx = 0;

for i = 1, 39 do
    root.Sprites["MusicMenuThumbnail" .. i] = {
        Sheet = "MusicThumbnails" .. sheetIdx,
        Bounds = {
            X = firstX,
            Y = firstY,
            Width = 400,
            Height = 400
        }
    };
    root.ExtraMenus.MusicMenu.Thumbnails[#root.ExtraMenus.MusicMenu.Thumbnails + 1] = "MusicMenuThumbnail" .. i;

    if i % 10 == 0 then
        sheetIdx = sheetIdx + 1;
        firstX = 1;
        firstY = 1;
    elseif (i % 5 == 0) then
        firstX = 1;
        firstY = firstY + 402;
    else
        firstX = firstX + 402;
    end
end

firstX = 957;
firstY = 720;

for i = 0, 3 do
    root.Sprites["MusicMenuPlaybackModeLabel" .. i] = {
        Sheet = "Music",
        Bounds = {
            X = firstX,
            Y = firstY,
            Width = 94,
            Height = 24
        }
    };
    root.ExtraMenus.MusicMenu.PlaybackModeLabels[#root.ExtraMenus.MusicMenu.PlaybackModeLabels + 1] = "MusicMenuPlaybackModeLabel" .. i;

    firstY = firstY + 26;
end

firstX = 1281;
firstY = 456;

for i = 1, 40 do
    if i ~= 20 then
        root.Sprites["MusicMenuItemName" .. i] = {
            Sheet = "Music",
            Bounds = {
                X = firstX,
                Y = firstY,
                Width = 358,
                Height = 22
            }
        };
        root.ExtraMenus.MusicMenu.ItemNames[#root.ExtraMenus.MusicMenu.ItemNames + 1] = "MusicMenuItemName" .. i;
    end

    if i % 20 == 0 then
        firstX = 1640;
        firstY = 456;
    else
        firstY = firstY + 24;
    end
end

root.Sprites["MusicMenuLockedItem"] = {
    Sheet = "Music",
    Bounds = { X = 1281, Y = 912, Width = 358, Height = 22 }
};

root.Sprites["MusicMenuScrollbarThumb"] = {
    Sheet = "Music",
    Bounds = { X = 1, Y = 837, Width = 34, Height = 46 }
};

root.Sprites["MusicMenuScrollbarTrack"] = {
    Sheet = "Music",
    Bounds = { X = 1780, Y = 0, Width = 9, Height = 427 }
};

firstX = 1053;
firstY = 721;

for i = 0, 10 do
    root.Sprites["MusicMenuTimerChar" .. i] = {
        Sheet = "Music",
        Bounds = {
            X = firstX,
            Y = firstY,
            Width = 15,
            Height = 24
        }
    };
    root.ExtraMenus.MusicMenu.TimerChars[#root.ExtraMenus.MusicMenu.TimerChars + 1] = "MusicMenuTimerChar" .. i;

    firstX = firstX + 17;
end

-- Album menu

root.Sprites["AlbumMenuBackground"] = {
    Sheet = "Album",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 }
};

local buttonX = 1;
local highlightedButtonX = 1281;
local buttonY = 761;
local highlightedButtonY = 1;
local portraitX = 1;
local portraitY = 1;
local portraitWidths = {383, 413, 484, 355, 326};

for i = 1, 6 do
    if i == 4 then
        root.Sprites["AlbumMenuCharaButton" .. i .. "Locked"] = {
            Sheet = "Album",
            Bounds = {
                X = 1281,
                Y = 529,
                Width = 710,
                Height = 86
            }
        };
        root.Sprites["AlbumMenuCharaButtonHighlighted" .. i .. "Locked"] = {
            Sheet = "Album",
            Bounds = {
                X = 1281,
                Y = 617,
                Width = 710,
                Height = 86
            }
        };
        root.ExtraMenus.AlbumMenu.CharacterButtons[#root.ExtraMenus.AlbumMenu.CharacterButtons + 1] = "AlbumMenuCharaButton" .. i .. "Locked";
        root.ExtraMenus.AlbumMenu.HighlightedCharacterButtons[#root.ExtraMenus.AlbumMenu.HighlightedCharacterButtons + 1] = "AlbumMenuCharaButtonHighlighted" .. i .. "Locked";
    elseif i == 5 then
        root.Sprites["AlbumMenuCharaButton" .. i .. "Locked"] = {
            Sheet = "Album2",
            Bounds = {
                X = 817,
                Y = 813,
                Width = 710,
                Height = 86
            }
        };
        root.Sprites["AlbumMenuCharaButtonHighlighted" .. i .. "Locked"] = {
            Sheet = "Album2",
            Bounds = {
                X = 817,
                Y = 725,
                Width = 710,
                Height = 86
            }
        };
        root.ExtraMenus.AlbumMenu.CharacterButtons[#root.ExtraMenus.AlbumMenu.CharacterButtons + 1] = "AlbumMenuCharaButton" .. i .. "Locked";
        root.ExtraMenus.AlbumMenu.HighlightedCharacterButtons[#root.ExtraMenus.AlbumMenu.HighlightedCharacterButtons + 1] = "AlbumMenuCharaButtonHighlighted" .. i .. "Locked";
    end

    root.Sprites["AlbumMenuCharaButton" .. i] = {
        Sheet = "Album",
        Bounds = {
            X = buttonX,
            Y = buttonY,
            Width = 710,
            Height = 86
        }
    };

    root.Sprites["AlbumMenuCharaButtonHighlighted" .. i] = {
        Sheet = "Album",
        Bounds = {
            X = highlightedButtonX,
            Y = highlightedButtonY,
            Width = 710,
            Height = 86
        }
    };

    root.ExtraMenus.AlbumMenu.CharacterButtons[#root.ExtraMenus.AlbumMenu.CharacterButtons + 1] = "AlbumMenuCharaButton" .. i;
    root.ExtraMenus.AlbumMenu.HighlightedCharacterButtons[#root.ExtraMenus.AlbumMenu.HighlightedCharacterButtons + 1] = "AlbumMenuCharaButtonHighlighted" .. i;

    if i ~= 6 then
        root.Sprites["AlbumMenuCharaPortrait" .. i] = {
            Sheet = "Album2",
            Bounds = {
                X = portraitX,
                Y = portraitY,
                Width = portraitWidths[i],
                Height = 640
            }
        };
        root.ExtraMenus.AlbumMenu.CharacterPortraits[#root.ExtraMenus.AlbumMenu.CharacterPortraits + 1] = "AlbumMenuCharaPortrait" .. i;
        portraitX = portraitX + portraitWidths[i];
    end

    if i % 3 == 0 then
        buttonX = 713;
        buttonY = 761;
        highlightedButtonY = highlightedButtonY + 88;
    else
        buttonY = buttonY + 88;
        highlightedButtonY = highlightedButtonY + 88;
    end
end

temp = root.ExtraMenus.AlbumMenu.CharacterPortraits[4];
root.ExtraMenus.AlbumMenu.CharacterPortraits[4] = root.ExtraMenus.AlbumMenu.CharacterPortraits[5];
root.ExtraMenus.AlbumMenu.CharacterPortraits[5] = temp;

root.Sprites["AlbumMenuOthersPortraitTop"] = {
    Sheet = "Album2",
    Bounds = { X = 409, Y = 725, Width = 406, Height = 298 }
};
root.Sprites["AlbumMenuOthersPortraitBottom"] = {
    Sheet = "Album2",
    Bounds = { X = 1, Y = 725, Width = 406, Height = 298 }
};

local column = 0;
local row = 0;
local block = 0;
local thumbX = 1;
local thumbY = 1;
local thumbnailSheet = "AlbumThumbnails0";

for i = 0, 103 do
    root.Sprites["AlbumMenuThumbnail" .. i] = {
        Sheet = thumbnailSheet,
        Bounds = {
            X = thumbX,
            Y = thumbY,
            Width = 240,
            Height = 135
        }
    };

    root.ExtraMenus.AlbumMenu.Thumbnails[#root.ExtraMenus.AlbumMenu.Thumbnails + 1] = "AlbumMenuThumbnail" .. i;

    thumbX = thumbX + 242;
    column = column + 1;
    if column == 4 then
        thumbX = (block * 968) + 1;
        thumbY = thumbY + 137;
        row = row + 1;
        column = 0;
        if row == 7 then
            row = 0;
            block = block + 1;
            if block == 2 then
                thumbnailSheet = "AlbumThumbnails1";
                block = 0;
                thumbX = 1;
                thumbY = 1;
            else
                thumbX = block * 969;
                thumbY = 1;
            end
        end
    end
end

root.Sprites["AlbumMenuThumbnailBorder"] = {
    Sheet = "Album",
    Bounds = { X = 1425, Y = 721, Width = 262, Height = 158 }
};
root.Sprites["AlbumMenuLockedThumbnail"] = {
    Sheet = "Album",
    Bounds = { X = 1698, Y = 731, Width = 240, Height = 135 }
};

root.Sprites["AlbumMenuThumbnailHighlightTopLeft"] = {
    Sheet = "Album",
    Bounds = { X = 1425, Y = 881, Width = 22, Height = 22 }
};
root.Sprites["AlbumMenuThumbnailHighlightTopRight"] = {
    Sheet = "Album",
    Bounds = { X = 1449, Y = 881, Width = 22, Height = 22 }
};
root.Sprites["AlbumMenuThumbnailHighlightBottomLeft"] = {
    Sheet = "Album",
    Bounds = { X = 1425, Y = 905, Width = 22, Height = 22 }
};
root.Sprites["AlbumMenuThumbnailHighlightBottomRight"] = {
    Sheet = "Album",
    Bounds = { X = 1449, Y = 905, Width = 22, Height = 22 }
};

root.Sprites["AlbumMenuArrowUp"] = {
    Sheet = "Album",
    Bounds = { X = 1473, Y = 881, Width = 34, Height = 22 }
};
root.Sprites["AlbumMenuArrowDown"] = {
    Sheet = "Album",
    Bounds = { X = 1473, Y = 905, Width = 34, Height = 22 }
};
