root.BacklogMenu = {
    Type = BacklogMenuType.None,
    DrawType = DrawComponentType.SystemMenu,
    BacklogBackgroundSprite = "BacklogBackground",
    EntryHighlightSprite = "EntryHighlight",
    EntryHighlightPadding = 0.0,
    VoiceIconSprite = "VoiceIcon",
    ScrollbarTrackSprite = "ScrollbarTrack",
    ScrollbarThumbSprite = "ScrollbarThumb",
    ScrollbarPosition = { X = 1748, Y = 177 },
    EntriesStart = { X = 245, Y = 167 },
    RenderingBounds = { X = 87, Y = 145, Width = 1750, Height = 720 },
    EntryYPadding = 22,
    FadeInDuration = 0.2,
    FadeOutDuration = 0.2
};

root.Sprites["BacklogBackground"] = {
    Sheet = "Backlog",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 },
};

root.Sprites["VoiceIcon"] = {
    Sheet = "Backlog",
    Bounds = { X = 1281, Y = 1, Width = 30, Height = 30 },
};

root.Sprites["EntryHighlight"] = {
    Sheet = "Data",
    Bounds = { X = 65, Y = 1, Width = 94, Height = 30 }
};

root.Sprites["ScrollbarThumb"] = {
    Sheet = "Backlog",
    Bounds = { X = 1281, Y = 32, Width = 30, Height = 30 },
};

root.Sprites["ScrollbarTrack"] = {
    Sheet = "Backlog",
    Bounds = { X = 1500, Y = 0, Width = 8, Height = 567 },
};