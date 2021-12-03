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

root.Sprites["ClearListArrowLeft"] = {
    Sheet: "ClearList",
    Bounds: { X: 1, Y: 725, Width: 30, Height: 46 },
};

root.Sprites["ClearListArrowRight"] = {
    Sheet: "ClearList",
    Bounds: { X: 33, Y: 725, Width: 30, Height: 46 },
};