root.ExtraMenus = {
    ClearListMenu: {
        DrawType: DrawComponentType.PlayData,
        Type: ClearListMenuType.MO6TW,
        BackgroundSprite: "ClearListBackground",
        WindowSprite: "ClearListWindow",
        WindowPosition: { X: 85, Y: 115 },
        WindowSpritePartLeft: "ClearListSceneWindowLeft",
        WindowSpritePartRight: "ClearListSceneWindowRight",
        FontSize: 32,
        SeparatorTable: 0,
        SeparatorEntry: 13,
        LabelPosition: { X: 790, Y: 32 },
        ClearListLabel: "ClearListLabel",
        EndingsLabelPosition: { X: 64, Y: 116 },
        EndingsLabel: "EndingsLabel",
        EndingCountPosition:  { X: 560, Y: 172 },
        ScenesLabelPosition: { X: 128, Y: 228 },
        ScenesLabel: "ScenesLabel",
        SceneCountPosition:  { X: 560, Y: 284 },
        CompletionLabelPosition: { X: 96, Y: 340 },
        CompletionLabel: "CompletionLabel",
        CompletionPosition:  { X: 560, Y: 396 },
        AlbumLabelPosition: { X: 128, Y: 452 },
        AlbumLabel: "AlbumLabel",
        AlbumCountPosition: { X: 560, Y: 508 },
        PlayTimeLabelPosition: { X: 64, Y: 564 },
        PlayTimeLabel: "PlayTimeLabel",
        PlayTimeTextTable: 0,
        PlayTimeSecondsTextEntry: 9,
        PlayTimeMinutesTextEntry: 8,
        PlayTimeHoursTextEntry: 10,
        PlayTimeSecondsTextPosition:  { X: 562, Y: 620 },
        PlayTimeMinutesTextPosition:  { X: 514, Y: 620 },
        PlayTimeHoursTextPosition:  { X: 466, Y: 620 },
        PlayTimeSecondsPosition:  { X: 560, Y: 620 },
        PlayTimeMinutesPosition:  { X: 512, Y: 620 },
        PlayTimeHoursPosition:  { X: 464, Y: 620 },
        ClearListColorIndex: 0,
        ClearListTextBackground: "ClearListTextBackground",
        ClearListTextBGOffset: { X: 0, Y: 48 },
        EndingListLabel: "EndingListLabel",
        EndingCount: 14,
        EndingsListNumberInitialPosition: { X: 108, Y: 142 },
        EndingsListTextInitialPosition: { X: 156, Y: 142 },
        EndingsListTextMargin: { X: 0, Y: 36 },
        EndingsListTextFontSize: 28,
        EndingsListTextLockedTable: 0,
        EndingsListTextLockedEntry: 15,
        EndingsListTextTable: 3,
        EndingsListTextColorIndex: 0,
        SceneTitleLabel: "SceneTitleLabel",
        SceneCount: 158,
        SceneListNumberInitialPosition: { X: 108, Y: 140 },
        SceneListTextInitialPosition: { X: 156, Y: 140 },
        SceneListTextMargin: { X: 0, Y: 36 },
        SceneListFontSize: 24,
        SceneListTextTable: 4,
        SceneTitleItemsRenderingBounds: { X: 90, Y: 120, Width: 795, Height: 538 },
        SceneListColorIndex: 0,
        SceneTitleLockedTable: 0,
        SceneTitleLockedEntry: 15,
        SceneTitleItemsWidth: 838,
        ScrollbarStart: 140,
        ScrollAreaHeight: 500,
        ScrollbarTrackSprite: "ClearListScrollbarTrack",
        ScrollbarThumbSprite: "ClearListScrollbarThumb",
        ScrollbarPosition: { X: 870, Y: 130 },
        ArrowsAnimationDuration: 0.3,
        ArrowLeft: "ClearListArrowLeft",
        ArrowLeftPosition: { X: 58, Y: 337 },
        ArrowRight: "ClearListArrowRight",
        ArrowRightPosition: { X: 1190, Y: 337 },
        FadeInDuration: 0.2,
        FadeOutDuration: 0.2
    },
    MovieMenu: {
        DrawType: DrawComponentType.ExtrasMovieMode,
        Type: MovieMenuType.MO6TW,
        BackgroundSprite: "MovieMenuBackground",
        FirstOPTopPartSprite: "MovieMenuFirstOPTopPart",
        FirstOPBottomPartSprite: "MovieMenuFirstOPBottomPart",
        SecondOPTopPartSprite: "MovieMenuSecondOPTopPart",
        SecondOPBottomPartSprite: "MovieMenuSecondOPBottomPart",
        UnlockedMovieThumbnailSprites: [],
        LockedMovieThumbnailSprites: [],
        SelectionHighlightTopLeft: "MovieMenuSelectionHighlightTopLeft",
        SelectionHighlightTopRight: "MovieMenuSelectionHighlightTopRight",
        SelectionHighlightBottomLeft: "MovieMenuSelectionHighlightBottomLeft",
        SelectionHighlightBottomRight: "MovieMenuSelectionHighlightBottomRight",
        ItemCount: 12,
        ItemsPerRow: 4,
        InitialItemPosition: { X: 151, Y: 80 },
        ItemOffset: { X: 250, Y: 206 },
        HighlightAnimationDuration: 0.5,
        HighlightTopLeftOffset: { X: 8, Y: 8 },
        HighlightTopRightOffset: { X: 12, Y: 9 },
        HighlightBottomLeftOffset: { X: 8, Y: 14 },
        HighlightBottomRightOffset: { X: 13, Y: 14 },
        FadeInDuration: 0.2,
        FadeOutDuration: 0.2
    },
    ActorsVoiceMenu: {
        DrawType: DrawComponentType.ExtrasActorsVoice,
        Type: ActorsVoiceMenuType.MO6TW,
        BackgroundSprite: "ActorsVoiceBackground",
        UnlockedSprites: [],
        LockedSprites: [],
        UnlockedHighlightedSprites: [],
        LockedHighlightedSprites: [],
        InitialItemPosition: { X: 256, Y: 112 },
        ItemOffset: { X: 0, Y: 60 },
        CharacterBackgroundBufferId: 4,
        FadeInDuration: 0.2,
        FadeOutDuration: 0.2
    },
    MusicMenu: {
        DrawType: DrawComponentType.ExtrasActorsVoice,
        Type: MusicMenuType.MO6TW,
        BackgroundSprite: "MusicMenuBackground",
        FadeInDuration: 0.2,
        FadeOutDuration: 0.2
    }
};

// Clear list

root.Sprites["ClearListBackground"] = {
    Sheet: "ClearList",
    Bounds: { X: 0, Y: 0, Width: 1280, Height: 720 }
};

root.Sprites["ClearListLabel"] = {
    Sheet: "ClearList",
    Bounds: { X: 1291, Y: 659, Width: 410, Height: 62 }
};

root.Sprites["EndingsLabel"] = {
    Sheet: "ClearList",
    Bounds: { X: 105, Y: 725, Width: 576, Height: 48 }
};

root.Sprites["ScenesLabel"] = {
    Sheet: "ClearList",
    Bounds: { X: 105, Y: 774, Width: 576, Height: 48 }
};

root.Sprites["CompletionLabel"] = {
    Sheet: "ClearList",
    Bounds: { X: 105, Y: 823, Width: 576, Height: 48 }
};

root.Sprites["AlbumLabel"] = {
    Sheet: "ClearList",
    Bounds: { X: 105, Y: 872, Width: 576, Height: 48 }
};

root.Sprites["PlayTimeLabel"] = {
    Sheet: "ClearList",
    Bounds: { X: 105, Y: 921, Width: 576, Height: 48 }
};

root.Sprites["ClearListTextBackground"] = {
    Sheet: "ClearList",
    Bounds: { X: 105, Y: 970, Width: 576, Height: 48 }
};

root.Sprites["EndingListLabel"] = {
    Sheet: "ClearList",
    Bounds: { X: 1291, Y: 723, Width: 410, Height: 62 }
};

root.Sprites["SceneTitleLabel"] = {
    Sheet: "ClearList",
    Bounds: { X: 1291, Y: 595, Width: 410, Height: 62 }
};

root.Sprites["ClearListWindow"] = {
    Sheet: "ClearList",
    Bounds: { X: 1291, Y: 1, Width: 638, Height: 554 }
};

root.Sprites["ClearListSceneWindowLeft"] = {
    Sheet: "ClearList",
    Bounds: { X: 1291, Y: 1, Width: 538, Height: 554 }
};

root.Sprites["ClearListSceneWindowRight"] = {
    Sheet: "ClearList",
    Bounds: { X: 1629, Y: 1, Width: 300, Height: 554 }
};

root.Sprites["ClearListScrollbarThumb"] = {
    Sheet: "ClearList",
    Bounds: { X: 65, Y: 725, Width: 38, Height: 46 },
};

root.Sprites["ClearListScrollbarTrack"] = {
    Sheet: "ClearList",
    Bounds: { X: 1939, Y: 17, Width: 8, Height: 514 },
};

root.Sprites["ClearListArrowLeft"] = {
    Sheet: "ClearList",
    Bounds: { X: 1, Y: 725, Width: 30, Height: 46 },
};

root.Sprites["ClearListArrowRight"] = {
    Sheet: "ClearList",
    Bounds: { X: 33, Y: 725, Width: 30, Height: 46 },
};

// Movie menu

root.Sprites["MovieMenuBackground"] = {
    Sheet: "Movie",
    Bounds: { X: 0, Y: 0, Width: 1280, Height: 720 }
};

root.Sprites["MovieMenuFirstOPTopPart"] = {
    Sheet: "Movie",
    Bounds: { X: 465, Y: 913, Width: 230, Height: 95 }
};

root.Sprites["MovieMenuFirstOPBottomPart"] = {
    Sheet: "Movie",
    Bounds: { X: 697, Y: 913, Width: 230, Height: 95 }
};

root.Sprites["MovieMenuSecondOPTopPart"] = {
    Sheet: "Movie",
    Bounds: { X: 1, Y: 913, Width: 230, Height: 95 }
};

root.Sprites["MovieMenuSecondOPBottomPart"] = {
    Sheet: "Movie",
    Bounds: { X: 233, Y: 913, Width: 230, Height: 95 }
};

root.Sprites["MovieMenuSelectionHighlightTopLeft"] = {
    Sheet: "Movie",
    Bounds: { X: 1161, Y: 721, Width: 22, Height: 22 }
};

root.Sprites["MovieMenuSelectionHighlightTopRight"] = {
    Sheet: "Movie",
    Bounds: { X: 1185, Y: 721, Width: 22, Height: 22 }
};

root.Sprites["MovieMenuSelectionHighlightBottomLeft"] = {
    Sheet: "Movie",
    Bounds: { X: 1161, Y: 745, Width: 22, Height: 22 }
};

root.Sprites["MovieMenuSelectionHighlightBottomRight"] = {
    Sheet: "Movie",
    Bounds: { X: 1185, Y: 745, Width: 22, Height: 22 }
};

var firstX = 1281;
var firstY = 1;

for (var i = 1; i < 11; i++) {
    root.Sprites["UnlockedMovieThumbnail" + i] = {
        Sheet: "Movie",
        Bounds: {
            X: firstX,
            Y: firstY,
            Width: 230,
            Height: 190
        }
    };
    root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites.push("UnlockedMovieThumbnail" + i);

    if (i % 3 == 0) {
        firstX = 1281;
        firstY += 191;
    } else {
        firstX += 231;
    }
}

firstX = 1;
firstY = 721;

for (var i = 1; i < 11; i++) {
    if (i == 6) {
        firstX = 1513;
        firstY = 577;
    } else if (i == 8) {
        firstX = 1281;
        firstY = 769;
    }

    root.Sprites["LockedMovieThumbnail" + i] = {
        Sheet: "Movie",
        Bounds: {
            X: firstX,
            Y: firstY,
            Width: 230,
            Height: 190
        }
    };
    root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites.push("LockedMovieThumbnail" + i);

    firstX += 231;
}

// I don't fucking know, do you know?
// I don't, fuck.
var temp = root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites[6];
root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites[6] = root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites[7];
root.ExtraMenus.MovieMenu.UnlockedMovieThumbnailSprites[7] = temp;

temp = root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites[6];
root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites[6] = root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites[7];
root.ExtraMenus.MovieMenu.LockedMovieThumbnailSprites[7] = temp;

// Actors Voice menu

root.Sprites["ActorsVoiceBackground"] = {
    Sheet: "ActorsVoiceBackground",
    Bounds: { X: 0, Y: 0, Width: 1280, Height: 720 }
};

firstX = 1;
firstY = 1;

for (var i = 0; i < 8; i++) {
    root.Sprites["LockedHighlightedSprites" + i] = {
        Sheet: "ActorsVoice",
        Bounds: {
            X: firstX,
            Y: firstY,
            Width: 774,
            Height: 50
        }
    };
    root.ExtraMenus.ActorsVoiceMenu.LockedHighlightedSprites.push("LockedHighlightedSprites" + i);

    root.Sprites["UnlockedHighlightedSprites" + i] = {
        Sheet: "ActorsVoice",
        Bounds: {
            X: firstX + 776,
            Y: firstY,
            Width: 774,
            Height: 50
        }
    };
    root.ExtraMenus.ActorsVoiceMenu.UnlockedHighlightedSprites.push("UnlockedHighlightedSprites" + i);

    root.Sprites["LockedSprites" + i] = {
        Sheet: "ActorsVoice",
        Bounds: {
            X: firstX,
            Y: firstY + 448,
            Width: 774,
            Height: 50
        }
    };
    root.ExtraMenus.ActorsVoiceMenu.LockedSprites.push("LockedSprites" + i);

    root.Sprites["UnlockedSprites" + i] = {
        Sheet: "ActorsVoice",
        Bounds: {
            X: firstX + 776,
            Y: firstY + 448,
            Width: 774,
            Height: 50
        }
    };
    root.ExtraMenus.ActorsVoiceMenu.UnlockedSprites.push("UnlockedSprites" + i);

    firstY += 56;
}

// Music menu

root.Sprites["MusicMenuBackground"] = {
    Sheet: "Music",
    Bounds: { X: 0, Y: 0, Width: 1280, Height: 720 }
};