root.ExtraMenus = {
    ClearListMenu = {
        DrawType = DrawComponentType.SystemMenu,
        Type = ClearListMenuType.CCLCC,
        FadeInDuration = 0.4,
        FadeOutDuration = 0.4,
        ClearListBookLayerSprite = "ClearListBookLayer",
        ClearListGuideSprite = "ClearListGuide",
        ClearListGuidePosition = {X = 0, Y = 989 },
        ClearListMaskSprite = "ClearListMask",
        EndingSprites = {
            "ItouBadEnd", "NonoNormalEnd", "NonoGoodEnd", "HinaeNormalEnd", "HinaeGoodEnd", "YuiYutoNormalEnd",
            "YuiYutoGoodEnd", "HanaNormalEnd", "SerikaNormalEnd", "SerikaTrueEnd"
        },
        EndingSpriteOffsetY = 1080.0,
        MenuOffsetY = 26.0,
    },
    LibraryMenu = {
        DrawType = DrawComponentType.SystemMenu,
        Type = LibraryMenuType.CCLCC,
        FadeInDuration = 0.4,
        FadeOutDuration = 0.4,
        LibraryBackgroundSprite = "LibraryBackground",
        LibraryBackgroundPosition = { X = 291, Y = 0 },
        LibraryIndexSprite = "LibraryIndex",
        LibraryIndexPosition = { X = 0, Y = 31 },
        LibraryButtonGuideSprite = "LibraryButtonGuide",
        LibraryButtonGuidePosition = { X = 0, Y = 989 },
        LibraryMaskSprite = "LibraryMask",
        
        SnapPhotoSpriteHover = "SnapPhotoHover",
        SnapPhotoSpriteSelect = "SnapPhotoSelect",
        HitSongsSpriteHover = "HitSongsHover",
        HitSongsSpriteSelect = "HitSongsSelect",
        LoveMovieSpriteHover = "LoveMovieHover",
        LoveMovieSpriteSelect = "LoveMovieSelect",

        SnapPhotoPos = {X=0, Y=80},
        HitSongsPos = {X=0, Y=382},
        LoveMoviePos = {X=0, Y=684},
    }
}

-- Common
root.Sprites["LibraryMask"] = {
    Sheet = "MenuChip",
    Bounds = { X = 154, Y = 140, Width = 1900, Height = 1061 },
};

-- ClearList
root.Sprites["ClearListBookLayer"] = {
    Sheet = "ClearList",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
};

root.Sprites["ClearListGuide"] = {
    Sheet = "ClearList",
    Bounds = { X = 0, Y = 2265, Width = 1925, Height = 58 }
};

root.Sprites["ClearListMask"] = {
    Sheet = "ClearList",
    Bounds = { X = 2048, Y = 0, Width = 1920, Height = 1080 },
};

root.Sprites["ItouBadEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 1022, Y = 1660, Width = 262, Height = 276 }
};

root.Sprites["NonoNormalEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 129, Y = 1901, Width = 225, Height = 183 }
};

root.Sprites["NonoGoodEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 312, Y = 1182, Width = 176, Height = 422 }
};

root.Sprites["HinaeNormalEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 358, Y = 1713, Width = 225, Height = 183 }
};

root.Sprites["HinaeGoodEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 677, Y = 1182, Width = 176, Height = 422 }
};

root.Sprites["YuiYutoNormalEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 358, Y = 1901, Width = 225, Height = 183 }
};

root.Sprites["YuiYutoGoodEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 129, Y = 1182, Width = 176, Height = 422 }
};

root.Sprites["HanaNormalEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 494, Y = 1182, Width = 176, Height = 422 }
};

root.Sprites["SerikaNormalEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 129, Y = 1713, Width = 225, Height = 183 }
};

root.Sprites["SerikaTrueEnd"] = {
    Sheet = "ClearList",
    Bounds = { X = 1026, Y = 1182, Width = 629, Height = 425 }
};

-- LibraryMenu
root.Sprites["LibraryBackground"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 0, Y = 700, Width = 1629, Height = 1080 },
};

root.Sprites["LibraryIndex"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 1658, Y = 0, Width = 381, Height = 1049 },
};

root.Sprites["LibraryButtonGuide"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 0, Y = 1780, Width = 1926, Height = 57 },
};

root.Sprites["AlbumButtonGuide"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 0, Y = 1878, Width = 1926, Height = 57 },
};

root.Sprites["SnapPhotoHover"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 0, Y = 0, Width = 256, Height = 300 },
};

root.Sprites["HitSongsHover"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 256, Y = 0, Width = 256, Height = 300 },
};

root.Sprites["LoveMovieHover"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 512, Y = 0, Width = 256, Height = 300 },
};

root.Sprites["SnapPhotoSelect"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 0, Y = 360, Width = 352, Height = 300 },
};

root.Sprites["HitSongsSelect"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 352, Y = 360, Width = 352, Height = 300 },
};

root.Sprites["LoveMovieSelect"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 704, Y = 360, Width = 352, Height = 300 },
};