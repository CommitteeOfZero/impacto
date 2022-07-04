root.SaveMenu = {
    Type: SaveMenuType.CHLCC,
    DrawType: DrawComponentType.SystemMenu,
    SaveCircle: "PinkCircle",
    LoadCircle: "LightBlueCircle",
    QuickLoadCircle: "GreenCircle",
    CircleStartPosition: {X: 20, Y: 20},
    CircleOffset: 200,
    ErinSprite: "Erin",
    ErinPosition: {X: 301, Y: 1},
    BackgroundFilter: "BackgroundFilter",
    InitialRedBarPosition: {X: 0, Y: 538},
    RightRedBarPosition: {X: 1059, Y: 538},
    RedBarDivision: 1826,
    RedBarBaseX: 1059,
    RedBarSprite: "RedBar",
    RedBarLabelPosition: {X: 1067, Y: 573},
    RedBarLabel: "RedBarLabel",
    QuickLoadTextSprite: "QuickLoadText",
    SaveTextSprite: "SaveText",
    LoadTextSprite: "LoadText",
    MenuTitleTextAngle: 4.45,
    MenuTitleTextLeftPos: {X: 1, Y: 1},
    MenuTitleTextRightPos: {X: 782, Y: 584},
    TitleFadeInDuration: 40 / 60,
    TitleFadeOutDuration: 28 / 60,
    SaveListPosition: {X: 0, Y: 0},
    SaveListSprite: "SaveList",
    EntryPositions: [
        {X: 91, Y: 127}, {X: 91, Y: 254},
        {X: 91, Y: 381}, {X: 91, Y: 508},
        {X: 661, Y: 127}, {X: 661, Y: 508}
    ],
    SaveMenuBackgroundSprite: "SaveMenuBackground",
    EmptyThumbnailSprite: "EmptyThumbnail",
    EntryStartX: 153,
    EntryXPadding: 512,
    EntryStartY: 102,
    EntryYPadding: 141,
    QuickLoadEntrySprite: "QuickLoadEntry",
    SaveEntrySprite: "SaveEntry",
    LoadEntrySprite: "LoadEntry",
    EntryHighlightedSprite: "EntryHighlighted",
    ThumbnailRelativePos: {X: 21, Y: 12},
    FadeInDuration: 64 / 60,
    FadeOutDuration: 64 / 60
};

root.Sprites["PinkCircle"] = {
    Sheet: "Save",
    Bounds: {X: 1, Y: 917, Width: 106, Height: 106}
};

root.Sprites["LightBlueCircle"] = {
    Sheet: "Save",
    Bounds: {X: 109, Y: 917, Width: 106, Height: 106}
};

root.Sprites["GreenCircle"] = {
    Sheet: "Save",
    Bounds: {X: 217, Y: 917, Width: 106, Height: 106}
};

root.Sprites["Erin"] = {
    Sheet: "ClearList",
    Bounds: {X: 641, Y: 1, Width: 978, Height: 798}
};

root.Sprites["RedBar"] = {
    Sheet: "ClearList",
    Bounds: {X: 767, Y: 913, Width: 1280, Height: 110}
};

root.Sprites["BackgroundFilter"] = {
    Sheet: "ClearList",
    Bounds: {X: 0, Y: 0, Width: 640, Height: 360}
};

root.Sprites["RedBarLabel"] = {
    Sheet: "ClearList",
    Bounds: {X: 506, Y: 469, Width: 133, Height: 74}
};

root.Sprites["SaveMenuBackground"] = {
    Sheet: "Save",
    Bounds: {X: 0, Y: 0, Width: 1280, Height: 720}
};

root.Sprites["EmptyThumbnail"] = {
    Sheet: "Save",
    Bounds: {X: 1, Y: 727, Width: 160, Height: 90}
};

root.Sprites["QuickLoadText"] = {
    Sheet: "Save",
    Bounds: {X: 945, Y: 1, Width: 130, Height: 974}
};

root.Sprites["SaveText"] = {
    Sheet: "Save",
    Bounds: {X: 681, Y: 1, Width: 130, Height: 420}
};

root.Sprites["LoadText"] = {
    Sheet: "Save",
    Bounds: {X: 813, Y: 1, Width: 130, Height: 420}
};

root.Sprites["SaveList"] = {
    Sheet: "Save",
    Bounds: {X: 1, Y: 1, Width: 678, Height: 578}
}

root.Sprites["QuickLoadEntry"] = {
    Sheet: "Save",
    Bounds: {X: 1077, Y: 233, Width: 532, Height: 116}
};
root.Sprites["SaveEntry"] = {
    Sheet: "Save",
    Bounds: {X: 1077, Y: 1, Width: 532, Height: 116}
};
root.Sprites["LoadEntry"] = {
    Sheet: "Save",
    Bounds: {X: 1077, Y: 117, Width: 532, Height: 116}
};

root.Sprites["EntryHighlighted"] = {
    Sheet: "Save",
    Bounds: {X: 1077, Y: 349, Width: 532, Height: 116}
};