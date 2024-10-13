root.ExtraMenus = {
    ClearListMenu = {
        DrawType = DrawComponentType.SystemMenu,
        Type = ClearListMenuType.CCLCC,
        BackgroundSprite = "MenuBackground",
        FadeInDuration = 0.2,
        FadeOutDuration = 0.2,
        ClearListBookLayerSprite = "ClearListBookLayer",
        ClearListGuideSprite = "ClearListGuide",
        ClearListGuideX = 0,
        ClearListGuideY = 990,
        LibraryMaskSprite = "LibraryMask",
    },
    LibraryMenu = {
        DrawType = DrawComponentType.SystemMenu,
        Type = LibraryMenuType.CCLCC,
        BackgroundSprite = "ClearListBackground",
        FadeInDuration = 0.2,
        FadeOutDuration = 0.2,
        LibraryBackgroundSprite = "LibraryBackground",
        LibraryBackgroundPosition = { X = 291, Y = 0 },
        LibraryIndexSprite = "LibraryIndex",
        LibraryIndexPosition = { X = 0, Y = 31 },
        LibraryButtonGuideSprite = "LibraryButtonGuide",
        LibraryButtonGuidePosition = { X = 0, Y = 988 },
        LibraryMaskSprite = "LibraryMask",
    }
}

-- Common
root.Sprites["LibraryMask"] = {
    Sheet = "MenuChip",
    Bounds = { X = 154, Y = 140, Width = 1900, Height = 1061 },
};

-- ClearList
root.Sprites["ClearListBackground"] = {
    Sheet = "Title",
    Bounds = { X = 0, Y = 1080, Width = 1920, Height = 1080 }
};

root.Sprites["ClearListBookLayer"] = {
    Sheet = "ClearList",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
};

root.Sprites["ClearListGuide"] = {
    Sheet = "ClearList",
    Bounds = { X = 0, Y = 2265, Width = 1925, Height = 55 }
};

root.Sprites["ClearListMask"] = {
    Sheet = "MenuChip",
    Bounds = { X = 154, Y = 140, Width = 1900, Height = 1061 },
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
    Bounds = { X = 0, Y = 0, Width = 252, Height = 300 },
};

root.Sprites["HitSongsHover"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 256, Y = 0, Width = 252, Height = 300 },
};

root.Sprites["LoveMovieHover"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 512, Y = 0, Width = 252, Height = 300 },
};

root.Sprites["SnapPhotoSelecting"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 0, Y = 365, Width = 350, Height = 300 },
};

root.Sprites["HitSongsHover"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 352, Y = 365, Width = 350, Height = 300 },
};

root.Sprites["LoveMovieHover"] = {
    Sheet = "LibraryMenu",
    Bounds = { X = 704, Y = 365, Width = 350, Height = 300 },
};

