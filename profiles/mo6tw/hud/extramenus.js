root.ExtraMenus = {
    ClearListMenu: {
        Type: ClearListMenuType.MO6TW,
        BackgroundSprite: "ClearListBackground",
        WindowSprite: "ClearListWindow",
        WindowPosition: { X: 85, Y: 115 },
        WindowSpritePartLeft: "ClearListSceneWindowLeft",
        WindowSpritePartRight: "ClearListSceneWindowRight",
        LabelPosition: { X: 790, Y: 32 },
        ClearListLabel: "ClearListLabel",
        EndingsLabelPosition: { X: 64, Y: 116 },
        EndingsLabel: "EndingsLabel",
        ScenesLabelPosition: { X: 128, Y: 228 },
        ScenesLabel: "ScenesLabel",
        CompletionLabelPosition: { X: 96, Y: 340 },
        CompletionLabel: "CompletionLabel",
        AlbumLabelPosition: { X: 128, Y: 452 },
        AlbumLabel: "AlbumLabel",
        PlayTimeLabelPosition: { X: 64, Y: 564 },
        PlayTimeLabel: "PlayTimeLabel",
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
        ScrollbarTrackSprite: "ClearListScrollbarTrack",
        ScrollbarThumbSprite: "ClearListScrollbarThumb",
        ScrollbarPosition: { X: 870, Y: 130 },
        ArrowsAnimationDuration: 0.3,
        FadeInDuration: 0.2,
        FadeOutDuration: 0.2
    }
};

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