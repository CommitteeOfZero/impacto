root.BacklogMenu = {
    Type = BacklogMenuType.CC,
    DrawType = DrawComponentType.SystemMenu,
    BacklogBackgroundSprite = "BacklogBackground",
    BacklogBackgroundRepeatHeight = 1080,
    BacklogHeaderSprite = "BacklogHeader",
    BacklogHeaderPosition = { X = 0, Y = 0 },
    BacklogAlphaMask = "BacklogAlphaMask",
    BacklogControlsSprite = "BacklogControls",
    BacklogControlsPosition = { X = 0, Y = 988 },
    EntryHighlightSprite = "EntryHighlight",
    EntryHighlightLocation = EntryHighlightLocationType.TopLineLeftOfScreen,
    EntryHighlightOffset = { X = 0, Y = 0 },
    VoiceIconSprite = "VoiceIcon",
    VoiceIconOffset = { X = -16, Y = -4 },
    ScrollbarTrackSprite = "ScrollbarTrack",
    ScrollbarThumbSprite = "ScrollbarThumb",
    ScrollbarPosition = { X = 1656, Y = 40 },
    ScrollbarThumbLength = 87,
    EntriesStart = { X = 547, Y = 149 },
    RenderingBounds = { X = 194, Y = 121, Width = 1531, Height = 868 },
    EntryYPadding = 26,
    FadeInDuration = 0.2,
    FadeOutDuration = 0.2
};

root.Sprites["BacklogBackground"] = {
    Sheet = "Backlog",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1100 },
};

root.Sprites["BacklogHeader"] = {
    Sheet = "Backlog",
    Bounds = { X = 962, Y = 1210, Width = 542, Height = 542 },
};

root.Sprites["BacklogAlphaMask"] = {
    Sheet = "BacklogMask",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1080 },
};

root.Sprites["BacklogControls"] = {
    Sheet = "Backlog",
    Bounds = { X = 0, Y = 1153, Width = 1920, Height = 57 },
};

root.Sprites["VoiceIcon"] = {
    Sheet = "Backlog",
    Bounds = { X = 1519, Y = 1216, Width = 43, Height = 43 },
};

root.Sprites["EntryHighlight"] = {
    Sheet = "Backlog",
    Bounds = { X = 0, Y = 1101, Width = 1920, Height = 52 }
};

root.Sprites["ScrollbarThumb"] = {
    Sheet = "Backlog",
    Bounds = { X = 2025, Y = 1113, Width = 23, Height = 87 },
};

root.Sprites["ScrollbarTrack"] = {
    Sheet = "Backlog",
    Bounds = { X = 2030, Y = 41, Width = 11, Height = 915 },
};