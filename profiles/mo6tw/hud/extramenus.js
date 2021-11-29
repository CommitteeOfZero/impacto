root.ExtraMenus = {
    ClearListMenu: {
        Type: ClearListMenuType.MO6TW,
        BackgroundSprite: "ClearListBackground",
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