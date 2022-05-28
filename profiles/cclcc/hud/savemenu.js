root.SaveMenu = {
    Type: SaveMenuType.CCLCC,
    DrawType: DrawComponentType.SystemMenu,
    SaveMenuBackgroundSprite: "SaveMenuBackground",
    EmptyThumbnailSprite: "EmptyThumbnail",
    EntryStartX: 153,
    EntryXPadding: 512,
    EntryStartY: 102,
    EntryYPadding: 141,
    FadeInDuration: 0.2,
    FadeOutDuration: 0.2
};

root.Sprites["SaveMenuBackground"] = {
    Sheet: "Title",
    Bounds: { X: 0, Y: 0, Width: 1920, Height: 1080 },
};

root.Sprites["EmptyThumbnail"] = {
    Sheet: "Save",
    Bounds: { X: 1755, Y: 11, Width: 234, Height: 159 }
};