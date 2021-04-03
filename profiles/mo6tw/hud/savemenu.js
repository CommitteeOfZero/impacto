root.SaveMenu = {
    Type: SaveMenuType.MO6TW,
    SaveMenuBackgroundSprite: "SaveMenuBackground",
	EmptyThumbnailSprite: "EmptyThumbnail",
    QuickLoadTextSprite: "QuickLoadText",
    LoadTextSprite: "LoadText",
    SaveTextSprite: "SaveText",
    MenuTitleTextPos: { X: 62.0, Y: 33.0 },
    EntryStartX: 153,
    EntryXPadding: 512,
    EntryStartY: 102,
    EntryYPadding: 141,
    QuickLoadEntrySprite: "QuickLoadEntry",
    QuickLoadEntryHighlightedSprite: "QuickLoadEntryHighlighted",
    SaveEntrySprite: "SaveEntry",
    SaveEntryHighlightedSprite: "SaveEntryHighlighted",
    LoadEntrySprite: "LoadEntry",
    LoadEntryHighlightedSprite: "LoadEntryHighlighted",
    FadeInDuration: 0.2,
    FadeOutDuration: 0.2
};

root.Sprites["SaveMenuBackground"] = {
    Sheet: "Save",
    Bounds: { X: 0, Y: 0, Width: 1280, Height: 720 }
};

root.Sprites["EmptyThumbnail"] = {
    Sheet: "Save",
    Bounds: { X: 1281, Y: 1, Width: 178, Height: 108 }
};

root.Sprites["QuickLoadText"] = {
    Sheet: "Save",
    Bounds: { X: 1281, Y: 111, Width: 262, Height: 42 }
};
root.Sprites["SaveText"] = {
    Sheet: "Save",
    Bounds: { X: 1281, Y: 155, Width: 262, Height: 42 }
};
root.Sprites["LoadText"] = {
    Sheet: "Save",
    Bounds: { X: 1281, Y: 199, Width: 262, Height: 42 }
};

root.Sprites["QuickLoadEntry"] = {
    Sheet: "Save",
    Bounds: { X: 1, Y: 721, Width: 508, Height: 128 }
};
root.Sprites["SaveEntry"] = {
    Sheet: "Save",
    Bounds: { X: 511, Y: 721, Width: 508, Height: 128 }
};
root.Sprites["LoadEntry"] = {
    Sheet: "Save",
    Bounds: { X: 1021, Y: 721, Width: 508, Height: 128 }
};
root.Sprites["QuickLoadEntryHighlighted"] = {
    Sheet: "Save",
    Bounds: { X: 1, Y: 851, Width: 508, Height: 128 }
};
root.Sprites["SaveEntryHighlighted"] = {
    Sheet: "Save",
    Bounds: { X: 511, Y: 851, Width: 508, Height: 128 }
};
root.Sprites["LoadEntryHighlighted"] = {
    Sheet: "Save",
    Bounds: { X: 1021, Y: 851, Width: 508, Height: 128 }
};