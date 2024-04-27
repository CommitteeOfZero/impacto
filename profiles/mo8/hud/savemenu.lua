root.SaveMenu = {
    Type = SaveMenuType.MO8,
    DrawType = DrawComponentType.SystemMenu,
    SaveMenuBackgroundSprite = "SaveMenuBackground",
    EmptyThumbnailSprite = "EmptyThumbnail",
    QuickLoadTextSprite = "QuickLoadText",
    LoadTextSprite = "LoadText",
    SaveTextSprite = "SaveText",
    MenuTitleTextPos = { X = 60, Y = 30 },
    NextButtonSprite = "SaveMenuNextButton",
    NextButtonHighlightedSprite = "SaveMenuNextButtonHighlighted",
    NextButtonPosition = { X = 1586, Y = 972 },
    BackButtonSprite = "SaveMenuBackButton",
    BackButtonHighlightedSprite = "SaveMenuBackButtonHighlighted",
    BackButtonPosition = { X = 30, Y = 972 },
    BackButtonPositionVoice = { X = 808, Y = 972 },
    EntryStartX = 153,
    EntryXPadding = 512,
    EntryStartY = 102,
    EntryYPadding = 141,
    FadeInDuration = 0.2,
    FadeOutDuration = 0.2
};

root.Sprites["SaveMenuBackground"] = {
    Sheet = "Save",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 }
};

root.Sprites["EmptyThumbnail"] = {
    Sheet = "Save",
    Bounds = { X = 1281, Y = 1, Width = 178, Height = 108 }
};

root.Sprites["QuickLoadText"] = {
    Sheet = "Save",
    Bounds = { X = 0, Y = 1848, Width = 438, Height = 90 }
};
root.Sprites["SaveText"] = {
    Sheet = "Save",
    Bounds = { X = 0, Y = 1652, Width = 438, Height = 90 }
};
root.Sprites["LoadText"] = {
    Sheet = "Save",
    Bounds = { X = 0, Y = 1750, Width = 438, Height = 90 }
};

root.Sprites["SaveMenuBackButton"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 722, Width = 304, Height = 80 }
};

root.Sprites["SaveMenuBackButtonHighlighted"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 898, Width = 304, Height = 80 }
};

root.Sprites["SaveMenuNextButton"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 810, Width = 304, Height = 80 }
};

root.Sprites["SaveMenuNextButtonHighlighted"] = {
    Sheet = "Data",
    Bounds = { X = 0, Y = 634, Width = 304, Height = 80 }
};
