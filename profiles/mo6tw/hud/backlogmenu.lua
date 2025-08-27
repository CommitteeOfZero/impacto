root.BacklogMenu = {
    Type = BacklogMenuType.MO6TW,
    DrawType = DrawComponentType.ExtrasScenes,

    BacklogBackgroundSprite = "BacklogBackground",

    EntryHighlightSprite = "EntryHighlight",
    EntryHighlightLocation = EntryHighlightLocationType.BottomLeftOfEntry,
    EntryHighlightOffset = { X = 0, Y = 0 },
    EntryHighlightPadding = 0.0,

    VoiceIconSprite = "VoiceIcon",
    VoiceIconOffset = { X = 0, Y = 0 },

    ScrollbarTrackSprite = "ScrollbarTrack",
    ScrollbarThumbSprite = "ScrollbarThumb",
    ScrollbarPosition = { X = 1165, Y = 98 },
    ScrollbarThumbLength = 30,

    EntriesStart = { X = 163, Y = 90 },
    RenderingBounds = { X = 83, Y = 83, Width = 1117, Height = 590 },
    EntryYPadding = 22,

    HoverBounds = { X = 130, Y = 83, Width = 992, Height = 590 },

    FadeInDuration = 0.2,
    FadeOutDuration = 0.2,

    ScrollingSpeed = 600,
    PageUpDownHeight = 520
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