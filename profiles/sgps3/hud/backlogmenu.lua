root.BacklogMenu = {
    Type = BacklogMenuType.None,
    DrawType = DrawComponentType.ExtrasScenes,
    BacklogBackgroundSprite = "BacklogBackground",
    EntryHighlightSprite = "EntryHighlight",
    EntryHighlightPadding = 0.0,
    VoiceIconSprite = "VoiceIcon",
    ScrollbarTrackSprite = "ScrollbarTrack",
    ScrollbarThumbSprite = "ScrollbarThumb",
    ScrollbarPosition = { X = 1165, Y = 98 },
    EntriesStart = { X = 163, Y = 85 },
    RenderingBounds = { X = 87, Y = 83, Width = 1055, Height = 590 },
    EntryYPadding = 22,
    FadeInDuration = 0.2,
    FadeOutDuration = 0.2
};

root.Sprites["BacklogBackground"] = {
    Sheet = "Backlog",
    Bounds = { X = 0, Y = 0, Width = 1280, Height = 720 },
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