root.ExtraMenus = {
    ClearListMenu = {
        DrawType = DrawComponentType.SystemMenu,
        Type = ClearListMenuType.CHLCC,
        TransitionDuration = 64 / 60,
        BackgroundColor = 0x405f86,
        CircleSprite = "Circle",
        CircleStartPosition = { X = 20, Y = 20 },
        CircleOffset = 200,
        ErinSprite = "Erin",
        ErinPosition = { X = 301, Y = 1 },
        BackgroundFilter = "BackgroundFilter",
        InitialRedBarPosition = { X = 0, Y = 538 },
        RightRedBarPosition = { X = 1059, Y = 538 },
        RedBarDivision = 1826,
        RedBarBaseX = 1059,
        RedBarSprite = "RedBar",
        RedBarLabelPosition = { X = 1067, Y = 573 },
        RedBarLabel = "RedBarLabel",
        MenuTitleTextRightPos = { X = 788, Y = 127 },
        MenuTitleTextLeftPos = { X = 1, Y = 1 },
        MenuTitleTextAngle = 4.45,
        TitleFadeInDuration = 40 / 60,
        TitleFadeOutDuration = 28 / 60,
        MenuTitleText = "MenuTitleText",
        ClearListLabel = "ClearListLabel",
        LabelPosition = { X = 800, Y = 44 },
        Digits = {},
        TimePositions = root.Language == "Japanese" and {
            { X = 988, Y = 69 }, { X = 1008, Y = 69 },
            { X = 1079, Y = 69 }, { X = 1099, Y = 69 },
            { X = 1148, Y = 69 }, { X = 1168, Y = 69 },
        } or {
            { X = 1026, Y = 69 }, { X = 1046, Y = 69 },
            { X = 1087, Y = 69 }, { X = 1107, Y = 69 },
            { X = 1157, Y = 69 }, { X = 1177, Y = 69 },
        },
        AlbumPositions = {
            { X = 1125, Y = 152 },
            { X = 1145, Y = 152 },
            { X = 1165, Y = 152 }
        },
        EndingCountPosition = { X = 1159, Y = 96 },
        TIPSCountPositions = root.Language == "Japanese" and {
            { X = 1105, Y = 124 }, { X = 1125, Y = 124 }, { X = 1145, Y = 124 },
        } or {
            { X = 1102, Y = 124 }, { X = 1122, Y = 124 }, { X = 1142, Y = 124 },
        },
        EndingList = "EndingList",
        ListPosition = { X = 0, Y = 0 },
        EndingBox = "EndingBox",
        BoxPositions = {
            { X = 341, Y = 218 }, { X = 572, Y = 146 },
            { X = 341, Y = 345 }, { X = 341, Y = 472 },
            { X = 572, Y = 400 }, { X = 572, Y = 273 },
            { X = 572, Y = 527 }, { X = 110, Y = 146 }
        },
        EndingThumbnails = {},
        ThumbnailPositions = {},
        LockedThumbnail = "LockedThumbnail",
        ButtonPromptPosition = { X = 1112, Y = 651 },
        ButtonPromptSprite = "ButtonPrompt"
    },
    MovieMenu = {
        DrawType = DrawComponentType.SystemMenu,
        Type = MovieMenuType.CHLCC,
        TransitionDuration = 64 / 60,
        BackgroundColor = 0x8e4f9f,
        CircleSprite = "CircleMovie",
        CircleStartPosition = { X = 20, Y = 20 },
        CircleOffset = 200,
        ErinSprite = "Erin",
        ErinPosition = { X = 301, Y = 1 },
        BackgroundFilter = "BackgroundFilter",
        InitialRedBarPosition = { X = 0, Y = 538 },
        RightRedBarPosition = { X = 1059, Y = 538 },
        RedBarDivision = 1826,
        RedBarBaseX = 1059,
        RedBarSprite = "RedBar",
        RedBarLabelPosition = { X = 1067, Y = 573 },
        RedBarLabel = "RedBarLabel",
        MenuTitleTextRightPos = { X = 790, Y = 75 },
        MenuTitleTextLeftPos = { X = 6, Y = 3 },
        MenuTitleTextAngle = 4.45,
        TitleFadeInDuration = 40 / 60,
        TitleFadeOutDuration = 28 / 60,
        MenuTitleText = "MovieMenuTitleText",
        MovieLabel = "MovieLabel",
        LabelPosition = { X = 800, Y = 44 },
        MovieList = "MovieList",
        ListPosition = { X = 0, Y = 0 },
        MovieBox = "MovieBox",
        BoxPositions = {
            { X = 93, Y = 127 }, { X = 93, Y = 255 },
            { X = 93, Y = 383 }, { X = 93, Y = 511 },
            { X = 341, Y = 199 }, { X = 341, Y = 327 },
            { X = 341, Y = 455 }, { X = 589, Y = 127 },
            { X = 589, Y = 255 }, { X = 589, Y = 383 },
        },
        MoviesThumbnails = {},
        ThumbnailPositions = {},
        LockedThumbnail = "MovieLockedThumbnail",
        ButtonPromptPosition = { X = 1022, Y = 651 },
        ButtonPromptSprite = "MovieButtonPrompt",
        SelectedMovieAnimation = "SelectedMovieAnimDef",
        SelectedMovieYellowDot = "SelectedMovieYellowDot",
        SelectMovieFadeDuration = 110 / 60,
        SelectMovie = {},
        SelectMoviePos = {
            { X = 94, Y = 51 },
            { X = 109, Y = 51 },
            { X = 122, Y = 51 },
            { X = 134, Y = 51 },
            { X = 147, Y = 51 },
            { X = 161, Y = 51 },
            { X = 180, Y = 51 },
            { X = 199, Y = 51 },
            { X = 213, Y = 51 },
            { X = 229, Y = 51 },
            { X = 234, Y = 51 },
        },
        MovieExtraVideosEnabled = true,
        MovieBoxExtra = "MovieBoxExtra",
        MovieThumbnailExtraOp = "MovieThumbnailExtraOp",
        MovieButtonExtraPromptPosition = { X = 877, Y = 651 },
        MovieButtonExtraPrompt = "MovieButtonExtraPrompt",
        SelectedMovieExtraAnimation = "SelectedMovieExtraAnimDef",
    },
    AlbumMenu = {
        DrawType = DrawComponentType.SystemMenu,
        Type = AlbumMenuType.CHLCC,
        TransitionDuration = 64 / 60,
        BackgroundColor = 0xf36989,
        CircleSprite = "CircleCG",
        CircleStartPosition = { X = 20, Y = 20 },
        CircleOffset = 200,
        ErinSprite = "Erin",
        ErinPosition = { X = 301, Y = 1 },
        BackgroundFilter = "BackgroundFilter",
        InitialRedBarPosition = { X = 0, Y = 538 },
        RightRedBarPosition = { X = 1059, Y = 538 },
        RedBarDivision = 1826,
        RedBarBaseX = 1059,
        RedBarSprite = "RedBar",
        RedBarLabelPosition = { X = 1067, Y = 573 },
        RedBarLabel = "RedBarLabel",
        TitleFadeInDuration = 40 / 60,
        TitleFadeOutDuration = 28 / 60,
        CGList = "CGList",
        CGListPosition = { X = 0, Y = 0 },
        PageCountLabel = "PageCountLabel",
        PageLabelPosition = { X = 1090, Y = 61 },
        CGBox = "CGBox",
        CGBoxTemplatePosition = { X = 86, Y = 142 },
        AlbumThumbnails = {},
        ThumbnailTemplatePosition = { X = 95, Y = 156 },
        VariationUnlocked = "VariationUnlocked",
        VariationLocked = "VariationLocked",
        VariationTemplateOffset = { X = 208, Y = 106 },
        LockedCG = "LockedCG",
        ThumbnailOffset = { X = 264, Y = 152 },
        ThumbnailHighlight = "ThumbnailHighlight",
        AlbumPages = 6,
        EntriesPerPage = 9,
        PageNums = {},
        CurrentPageNumPos = { X = 1133, Y = 65 },
        PageNumSeparatorSlash = "PageNumSeparatorSlash",
        PageNumSeparatorSlashPos = { X = 1161, Y = 90 },
        MaxPageNumPos = { X = 1179, Y = 90 },
        ReachablePageNums = {},
        ButtonGuide = "ButtonGuide",
        ButtonGuidePos = { X = 1019, Y = 651 },
        SelectData = {},
        SelectDataPos = {
            { X = 94, Y = 51 },
            { X = 109, Y = 51 },
            { X = 122, Y = 51 },
            { X = 134, Y = 51 },
            { X = 147, Y = 51 },
            { X = 161, Y = 51 },
            { X = 180, Y = 51 },
            { X = 199, Y = 51 },
            { X = 213, Y = 51 },
            { X = 228, Y = 51 }
        },
        AlbumMenuTitle = "AlbumMenuTitle",
        AlbumMenuTitleRightPos = { X = 787, Y = 106 },
        AlbumMenuTitleLeftPos = { X = 1, Y = 1 },
        AlbumMenuTitleAngle = 4.45,
        CgViewerButtonGuideVariation = "CgViewerButtonGuideVariation",
        CgViewerButtonGuideNoVariation = "CgViewerButtonGuideNoVariation",
        CgViewerButtonGuidePos = { X = 208, Y = 648 },
        SelectionMarkerSprite = "SelectionMarkerSprite",
        SelectionMarkerRelativePos = { X = -16, Y = 50 }
    },
    MusicMenu = {
        DrawType = DrawComponentType.SystemMenu,
        Type = MusicMenuType.CHLCC,
        TransitionDuration = 64 / 60,
        BackgroundColor = 0x339455,
        CircleSprite = "CircleSound",
        CircleStartPosition = { X = 20, Y = 20 },
        CircleOffset = 200,
        ErinSprite = "Erin",
        ErinPosition = { X = 301, Y = 1 },
        BackgroundFilter = "BackgroundFilter",
        InitialRedBarPosition = { X = 0, Y = 538 },
        RightRedBarPosition = { X = 1059, Y = 538 },
        RedBarDivision = 1826,
        RedBarBaseX = 1059,
        RedBarSprite = "RedBar",
        RedBarLabelPosition = { X = 1067, Y = 573 },
        RedBarLabel = "RedBarLabel",
        TitleFadeInDuration = 40 / 60,
        TitleFadeOutDuration = 28 / 60,
        TrackTreeSprite = "TrackTreeSprite",
        TrackTreePos = { X = 0, Y = 0 },
        TrackButtonPosTemplate = { X = 94, Y = 170 },
        TrackNameOffset = { X = 95, Y = 5 },
        ArtistOffset = { X = 411, Y = 5 },
        TrackOffset = { X = 0, Y = 28 },
        TrackHighlight = "TrackHighlight",
        TrackNumRelativePos = { X = 25, Y = 5 },
        ButtonPromptPosition = { X = 738, Y = 651 },
        ButtonPromptSprite = "MusicButtonPrompt",
        PlaymodeRepeatPos = { X = 718, Y = 146 },
        PlaymodeAllPos = { X = 771, Y = 146 },
        PlaymodeRepeat = "PlaymodeRepeat",
        PlaymodeAll = "PlaymodeAll",
        PlaymodeRepeatHighlight = "PlaymodeRepeatHighlight",
        PlaymodeAllHighlight = "PlaymodeAllHighlight",
        NowPlaying = "NowPlaying",
        NowPlayingPos = { X = 745, Y = 44 },
        NowPlayingAnimationDuration = 16 / 60,
        PlayingTrackOffset = { X = 1, Y = 25 },
        PlayingTrackArtistOffset = { X = 1, Y = 59 },
        SoundLibraryTitle = "SoundLibraryTitle",
        SoundLibraryTitleLeftPos = { X = 1, Y = 1 },
        SoundLibraryTitleRightPos = { X = 787, Y = 103 },
        SoundLibraryTitleAngle = 4.45,
        HighlightStar = "HighlightStar",
        HighlightStarRelativePos = { X = 1, Y = 1 },
        Playlist = {
            8, 14, 26, 10, 28, 12, 20, 16, 18, 24, 22, 80, 76, 32, 34,
            36, 38, 77, 41, 43, 78, 46, 49, 79, 51, 53, 56, 81, 59, 61,
            63, 65, 82, 83, 84, 0, 1, 2, 4, 6, 85, 72, 74, 75, 71
        },
        SelectSound = {},
        SelectSoundPos = {
            { X = 94, Y = 51 },
            { X = 109, Y = 51 },
            { X = 122, Y = 51 },
            { X = 134, Y = 51 },
            { X = 147, Y = 51 },
            { X = 161, Y = 51 },
            { X = 178, Y = 51 },
            { X = 192, Y = 51 },
            { X = 206, Y = 51 },
            { X = 220, Y = 51 },
            { X = 234, Y = 52 },
        },
    }
}

--ClearList

root.Sprites["Circle"] = {
    Sheet = "ClearList",
    Bounds = { X = 1, Y = 917, Width = 106, Height = 106 }
}

root.Sprites["Erin"] = {
    Sheet = "Main",
    Bounds = { X = 641, Y = 1, Width = 978, Height = 798 }
}

root.Sprites["RedBar"] = {
    Sheet = "Main",
    Bounds = { X = 767, Y = 913, Width = 1280, Height = 110 }
}

root.Sprites["RedBarLabel"] = {
    Sheet = "Main",
    Bounds = { X = 506, Y = 469, Width = 133, Height = 74 }
}

root.Sprites["MenuTitleText"] = {
    Sheet = "ClearList",
    Bounds = { X = 607, Y = 1, Width = 120, Height = 912 }
}

root.Sprites["ClearListLabel"] = {
    Sheet = "ClearList",
    Bounds = { X = 1, Y = 591, Width = 490, Height = 136 }
}

for i = 0, 9 do
    root.Sprites["Digit" .. i] = {
        Sheet = "ClearList",
        Bounds = {
            X = 211 + 22 * i,
            Y = 729,
            Width = 20,
            Height = 30
        }
    }
    root.ExtraMenus.ClearListMenu.Digits[#root.ExtraMenus.ClearListMenu.Digits + 1] = "Digit" .. i;
end

root.Sprites["BackgroundFilter"] = {
    Sheet = "Main",
    Bounds = { X = 0, Y = 0, Width = 640, Height = 360 }
}

root.Sprites["EndingList"] = {
    Sheet = "ClearList",
    Bounds = { X = 1, Y = 1, Width = 604, Height = 588 }
}

root.Sprites["EndingBox"] = {
    Sheet = "ClearList",
    Bounds = { X = 1, Y = 761, Width = 198, Height = 96 }
}

local thumbnailXOffset = 30;
local thumbnailYOffset = 3;
local bp = root.ExtraMenus.ClearListMenu.BoxPositions;

for i = 1, #bp do
    root.ExtraMenus.ClearListMenu.ThumbnailPositions[#root.ExtraMenus.ClearListMenu.ThumbnailPositions + 1] = {
        X = bp[i].X + thumbnailXOffset,
        Y = bp[i].Y + thumbnailYOffset
    };
end

root.Sprites["LockedThumbnail"] = {
    Sheet = "ClearList",
    Bounds = { X = 729, Y = 1, Width = 160, Height = 90 }
}

local firstX = 729;
local firstY = 93;
local yDelta = 92;

for i = 1, 8 do
    root.Sprites["EndingThumbnail" .. i] = {
        Sheet = "ClearList",
        Bounds = {
            X = firstX,
            Y = firstY,
            Width = 160,
            Height = 90
        }
    };
    root.ExtraMenus.ClearListMenu.EndingThumbnails[#root.ExtraMenus.ClearListMenu.EndingThumbnails + 1] = "EndingThumbnail" .. i;
    firstY = firstY + yDelta;
end

--Reorganize to be in the flag order (801 -> 808)
--Yes I know it's bad
et = root.ExtraMenus.ClearListMenu.EndingThumbnails;
temp = et[1];
et[1] = et[2];
et[2] = temp;
et[2] = et[5];
et[5] = temp;
et[5] = et[7];
et[7] = temp;
et[7] = et[8];
et[8] = temp;

root.Sprites["ButtonPrompt"] = {
    Sheet = "ClearList",
    Bounds = { X = 1, Y = 729, Width = 208, Height = 30 }
}

--MovieList
root.Sprites["CircleMovie"] = {
    Sheet = "Movie",
    Bounds = { X = 1, Y = 917, Width = 106, Height = 106 }
}

root.Sprites["MovieBox"] = {
    Sheet = "Movie",
    Bounds = { X = 728, Y = 1, Width = 198, Height = 122 }
}

root.Sprites["MovieBoxExtra"] = {
    Sheet = "Movie",
    Bounds = { X = 1091, Y = 1, Width = 198, Height = 122 }
}

root.Sprites["MovieThumbnailExtraOp"] = {
    Sheet = "Movie",
    Bounds = { X = 1291, Y = 93, Width = 160, Height = 90 }
}

root.Sprites["MovieButtonExtraPrompt"] = {
    Sheet = "Movie",
    Bounds = { X = 1, Y = 732, Width = 427, Height = 28 }
}

local thumbnailXOffset = 20;
local thumbnailYOffset = 16;
local bp = root.ExtraMenus.MovieMenu.BoxPositions;

for i = 1, #bp do
    root.ExtraMenus.MovieMenu.ThumbnailPositions[#root.ExtraMenus.MovieMenu.ThumbnailPositions + 1] = {
        X = bp[i].X + thumbnailXOffset,
        Y = bp[i].Y + thumbnailYOffset
    };
end

root.Sprites["MovieLabel"] = {
    Sheet = "Movie",
    Bounds = { X = 1, Y = 591, Width = 490, Height = 136 }
}

root.Sprites["MovieList"] = {
    Sheet = "Movie",
    Bounds = { X = 1, Y = 1, Width = 605, Height = 585 }
}

root.Sprites["MovieMenuTitleText"] = {
    Sheet = "Movie",
    Bounds = { X = 609, Y = 3, Width = 116, Height = 1005 }
}

local firstX = 928;
local firstY = 93;
local yDelta = 92;

for i = 1, 10 do
    root.Sprites["MovieThumbnail" .. i] = {
        Sheet = "Movie",
        Bounds = {
            X = firstX,
            Y = firstY,
            Width = 160,
            Height = 90
        }
    };
    root.ExtraMenus.MovieMenu.MoviesThumbnails[#root.ExtraMenus.MovieMenu.MoviesThumbnails + 1] = "MovieThumbnail" .. i;
    firstY = firstY + yDelta;
end

root.Sprites["MovieLockedThumbnail"] = {
    Sheet = "Movie",
    Bounds = { X = 928, Y = 1, Width = 160, Height = 90 }
}

root.Sprites["MovieButtonPrompt"] = {
    Sheet = "Movie",
    Bounds = { X = 1, Y = 686, Width = 282, Height = 28 }
}

root.Sprites["SelectMovie0"] = {
    Sheet = "Movie",
    Bounds = { X = 1, Y = 597, Width = 15, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie0";

root.Sprites["SelectMovie1"] = {
    Sheet = "Movie",
    Bounds = { X = 19, Y = 597, Width = 16, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie1";

root.Sprites["SelectMovie2"] = {
    Sheet = "Movie",
    Bounds = { X = 37, Y = 597, Width = 15, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie2";

root.Sprites["SelectMovie3"] = {
    Sheet = "Movie",
    Bounds = { X = 53, Y = 597, Width = 16, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie3";

root.Sprites["SelectMovie4"] = {
    Sheet = "Movie",
    Bounds = { X = 72, Y = 597, Width = 17, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie4";

root.Sprites["SelectMovie5"] = {
    Sheet = "Movie",
    Bounds = { X = 89, Y = 597, Width = 13, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie5";

root.Sprites["SelectMovie6"] = {
    Sheet = "Movie",
    Bounds = { X = 114, Y = 597, Width = 21, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie6";

root.Sprites["SelectMovie7"] = {
    Sheet = "Movie",
    Bounds = { X = 138, Y = 597, Width = 16, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie7";

root.Sprites["SelectMovie8"] = {
    Sheet = "Movie",
    Bounds = { X = 157, Y = 597, Width = 15, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie8";

root.Sprites["SelectMovie9"] = {
    Sheet = "Movie",
    Bounds = { X = 179, Y = 597, Width = 7, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie9";

root.Sprites["SelectMovie10"] = {
    Sheet = "Movie",
    Bounds = { X = 189, Y = 597, Width = 16, Height = 57 }
}
root.ExtraMenus.MovieMenu.SelectMovie[#root.ExtraMenus.MovieMenu.SelectMovie + 1] = "SelectMovie10";

root.Sprites["SelectedMovieYellowDot"] = {
    Sheet = "Movie",
    Bounds = { X = 294, Y = 694, Width = 14, Height = 14 }
}

MakeAnimation({
    Name = "SelectedMovieAnimDef",
    Sheet = "Movie",
    FirstFrameX = 728,
    FirstFrameY = 125,
    FrameWidth = 198,
    ColWidth = 198,
    FrameHeight = 124,
    RowHeight = 124,
    Frames = 7,
    Duration = 0.2,
    Rows = 7,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Down,
});

MakeAnimation({
    Name = "SelectedMovieExtraAnimDef",
    Sheet = "Movie",
    FirstFrameX = 1091,
    FirstFrameY = 125,
    FrameWidth = 198,
    ColWidth = 198,
    FrameHeight = 124,
    RowHeight = 124,
    Frames = 7,
    Duration = 0.2,
    Rows = 7,
    Columns = 1,
    PrimaryDirection = AnimationDirections.Down,
});

--SoundList
root.Sprites["CircleSound"] = {
    Sheet = "Sound",
    Bounds = { X = 1, Y = 917, Width = 106, Height = 106 }
}

root.Sprites["TrackTreeSprite"] = {
    Sheet = "Sound",
    Bounds = { X = 1, Y = 1, Width = 802, Height = 608 }
}

root.Sprites["TrackHighlight"] = {
    Sheet = "Sound",
    Bounds = { X = 1, Y = 693, Width = 676, Height = 29 }
}

root.Sprites["PlaymodeRepeat"] = {
    Sheet = "Sound",
    Bounds = { X = 257, Y = 644, Width = 51, Height = 10 }
}

root.Sprites["PlaymodeAll"] = {
    Sheet = "Sound",
    Bounds = { X = 310, Y = 644, Width = 28, Height = 10 }
}

root.Sprites["PlaymodeRepeatHighlight"] = {
    Sheet = "Sound",
    Bounds = { X = 257, Y = 632, Width = 51, Height = 10 }
}

root.Sprites["PlaymodeAllHighlight"] = {
    Sheet = "Sound",
    Bounds = { X = 310, Y = 632, Width = 28, Height = 10 }
}

root.Sprites["NowPlaying"] = {
    Sheet = "Sound",
    Bounds = { X = 1, Y = 611, Width = 656, Height = 19 }
}

root.Sprites["MusicButtonPrompt"] = {
    Sheet = "Sound",
    Bounds = { X = 1, Y = 724, Width = 552, Height = 28}
}


root.Sprites["SoundLibraryTitle"] = {
    Sheet = "Sound",
    Bounds = { X = 805, Y = 1, Width = 118, Height = 961 }
}

root.Sprites["HighlightStar"] = {
    Sheet = "Sound",
    Bounds = { X = 216, Y = 632, Width = 23, Height = 23 }
}

root.Sprites["SelectSound0"] = {
    Sheet = "Sound",
    Bounds = { X = 2, Y = 633, Width = 15, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound0";

root.Sprites["SelectSound1"] = {
    Sheet = "Sound",
    Bounds = { X = 20, Y = 633, Width = 16, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound1";

root.Sprites["SelectSound2"] = {
    Sheet = "Sound",
    Bounds = { X = 38, Y = 633, Width = 15, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound2";

root.Sprites["SelectSound3"] = {
    Sheet = "Sound",
    Bounds = { X = 54, Y = 633, Width = 16, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound3";

root.Sprites["SelectSound4"] = {
    Sheet = "Sound",
    Bounds = { X = 73, Y = 633, Width = 15, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound4";

root.Sprites["SelectSound5"] = {
    Sheet = "Sound",
    Bounds = { X = 90, Y = 633, Width = 13, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound5";

root.Sprites["SelectSound6"] = {
    Sheet = "Sound",
    Bounds = { X = 116, Y = 633, Width = 16, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound6";

root.Sprites["SelectSound7"] = {
    Sheet = "Sound",
    Bounds = { X = 136, Y = 633, Width = 16, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound7";

root.Sprites["SelectSound8"] = {
    Sheet = "Sound",
    Bounds = { X = 156, Y = 633, Width = 17, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound8";

root.Sprites["SelectSound9"] = {
    Sheet = "Sound",
    Bounds = { X = 176, Y = 633, Width = 18, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound9";

root.Sprites["SelectSound10"] = {
    Sheet = "Sound",
    Bounds = { X = 197, Y = 633, Width = 17, Height = 58 }
}
root.ExtraMenus.MusicMenu.SelectSound[#root.ExtraMenus.MusicMenu.SelectSound + 1] = "SelectSound10";

--CGList
root.Sprites["CircleCG"] = {
    Sheet = "CG",
    Bounds = { X = 1, Y = 917, Width = 106, Height = 106 }
}

root.Sprites["CGList"] = {
    Sheet = "CG",
    Bounds = { X = 1, Y = 1, Width = 638, Height = 538 }
}

root.Sprites["PageCountLabel"] = {
    Sheet = "CG",
    Bounds = { X = 1, Y = 611, Width = 204, Height = 57 }
}

root.Sprites["CGBox"] = {
    Sheet = "CG",
    Bounds = { X = 235, Y = 611, Width = 252, Height = 158 }
}

for i = 0, 6 do
    for j = 0, 7 do
        root.Sprites["AlbumThumbnail" .. (8 * i + j)] = {
            Sheet = "AlbumThumbnailSheet",
            Bounds = { X = 234 * j + 1, Y = 132 * i + 1, Width = 232, Height = 130 }
        }
        root.ExtraMenus.AlbumMenu.AlbumThumbnails[#root.ExtraMenus.AlbumMenu.AlbumThumbnails + 1] = "AlbumThumbnail" .. (8 * i + j);
    end
end

for i = 0, 6 do
    root.Sprites["AlbumThumbnail" .. (56 + i)] = {
        Sheet = "AlbumThumbnailSheet2",
        Bounds = { X = 1, Y = 132 * i + 1, Width = 232, Height = 130 }
    }
    root.ExtraMenus.AlbumMenu.AlbumThumbnails[#root.ExtraMenus.AlbumMenu.AlbumThumbnails + 1] = "AlbumThumbnail" .. (56 + i);
end

root.Sprites["VariationUnlocked"] = {
    Sheet = "CG",
    Bounds = { X = 489, Y = 611, Width = 22, Height = 22 }
}

root.Sprites["VariationLocked"] = {
    Sheet = "CG",
    Bounds = { X = 513, Y = 611, Width = 22, Height = 22 }
}

root.Sprites["LockedCG"] = {
    Sheet = "CG",
    Bounds = { X = 235, Y = 785, Width = 232, Height = 130 }
}

root.Sprites["ThumbnailHighlight"] = {
    Sheet = "CG",
    Bounds = { X = 1, Y = 785, Width = 232, Height = 130 }
}

for i = 0, 9 do
    root.Sprites["PageNum" .. i] = {
        Sheet = "Data",
        Bounds = { X = i * 36 + 371, Y = 1, Width = 34, Height = 48 }
    };
    root.ExtraMenus.AlbumMenu.PageNums[#root.ExtraMenus.AlbumMenu.PageNums + 1] = "PageNum" .. i;
end

root.Sprites["PageNumSeparatorSlash"] = {
    Sheet = "Data",
    Bounds = { X = 173, Y = 67, Width = 16, Height = 22 }
};

for i = 0, 9 do
    root.Sprites["ReachablePageNum" .. i] = {
        Sheet = "Data",
        Bounds = { X = 191 + 18 * i, Y = 67, Width = 16, Height = 22 }
    };
    root.ExtraMenus.AlbumMenu.ReachablePageNums[#root.ExtraMenus.AlbumMenu.ReachablePageNums + 1] = "ReachablePageNum" .. i;
end

root.Sprites["ButtonGuide"] = {
    Sheet = "CG",
    Bounds = { X = 1, Y = 581, Width = 380, Height = 28 }
}

root.Sprites["SelectData0"] = {
    Sheet = "CG",
    Bounds = { X = 1, Y = 670, Width = 18, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData0";

root.Sprites["SelectData1"] = {
    Sheet = "CG",
    Bounds = { X = 21, Y = 670, Width = 16, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData1";

root.Sprites["SelectData2"] = {
    Sheet = "CG",
    Bounds = { X = 39, Y = 670, Width = 15, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData2";

root.Sprites["SelectData3"] = {
    Sheet = "CG",
    Bounds = { X = 56, Y = 670, Width = 16, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData3";

root.Sprites["SelectData4"] = {
    Sheet = "CG",
    Bounds = { X = 74, Y = 670, Width = 17, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData4";

root.Sprites["SelectData5"] = {
    Sheet = "CG",
    Bounds = { X = 93, Y = 670, Width = 22, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData5";

root.Sprites["SelectData6"] = {
    Sheet = "CG",
    Bounds = { X = 117, Y = 670, Width = 22, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData6";

root.Sprites["SelectData7"] = {
    Sheet = "CG",
    Bounds = { X = 141, Y = 670, Width = 17, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData7";

root.Sprites["SelectData8"] = {
    Sheet = "CG",
    Bounds = { X = 159, Y = 670, Width = 18, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData8";

root.Sprites["SelectData9"] = {
    Sheet = "CG",
    Bounds = { X = 179, Y = 670, Width = 20, Height = 55 }
}
root.ExtraMenus.AlbumMenu.SelectData[#root.ExtraMenus.AlbumMenu.SelectData + 1] = "SelectData9";

root.Sprites["AlbumMenuTitle"] = {
    Sheet = "CG",
    Bounds = { X = 641, Y = 1, Width = 120, Height = 954 }
}

root.Sprites["CgViewerButtonGuideVariation"] = {
    Sheet = "CG",
    Bounds = { X = 757, Y = 578, Width = 1190, Height = 38 }
}

root.Sprites["CgViewerButtonGuideNoVariation"] = {
    Sheet = "CG",
    Bounds = { X = 757, Y = 536, Width = 1190, Height = 38 }
}

root.Sprites["SelectionMarkerSprite"] = {
    Sheet = "CG",
    Bounds = { X = 433, Y = 581, Width = 28, Height = 28 }
}