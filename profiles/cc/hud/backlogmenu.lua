root.BacklogMenu = {
    Type = BacklogMenuType.CC,
    DrawType = DrawComponentType.SystemMenu,

    BacklogBackgroundSprite = "BacklogBackground",
    BacklogBackgroundRepeatHeight = 1080,

    BacklogHeaderSprite = "BacklogHeader",
    BacklogHeaderPosition = { X = 0, Y = 0 },

    BacklogControlsSprite = "BacklogControls",
    BacklogControlsPosition = { X = 0, Y = 988 },

    EntryHighlightSprite = "EntryHighlight",
    EntryHighlightLocation = EntryHighlightLocationType.AllLinesLeftOfScreen,
    EntryHighlightOffset = { X = 0, Y = 0 },
    EntryHighlightPadding = 16.0,

    VoiceIconSprite = "VoiceIcon",
    VoiceIconOffset = { X = -7, Y = 3 },

    ScrollbarTrackSprite = "ScrollbarTrack",
    ScrollbarThumbSprite = "ScrollbarThumb",
    ScrollbarPosition = { X = 1656, Y = 40 },
    ScrollbarThumbLength = 87,

    EntriesStart = { X = 547, Y = 149 },
    RenderingBounds = { X = 0, Y = 121, Width = 1920, Height = 868 },
    EntryYPadding = 26,

    FadeInDuration = 0.5,
    FadeOutDuration = 0.5,
    FadeInDirectDuration = 0.25,
    FadeOutDirectDuration = 0.25,

    ScrollingSpeed = 900,
    PageUpDownHeight = 765,

    MenuMask = "MenuMask",
    BacklogMask = "BacklogMask",
    HoverBounds = { X = 380, Y = 145, Width = 1230, Height = 820 }
};

root.Sprites["BacklogBackground"] = {
    Sheet = "Backlog",
    Bounds = { X = 0, Y = 0, Width = 1920, Height = 1100 },
};

root.Sprites["BacklogHeader"] = {
    Sheet = "Backlog",
    Bounds = { X = 962, Y = 1210, Width = 542, Height = 542 },
};

root.SpriteSheets["BacklogMask"] = {
    Path = { Mount = "system", Id = 2 },
    DesignWidth = 1920,
    DesignHeight = 1080
};

root.Sprites["BacklogControls"] = {
    Sheet = "Backlog",
    Bounds = { X = 0, Y = 1153, Width = 1920, Height = 57 },
};

root.Sprites["MenuMask"] = {
    Sheet = "MenuChip",
    Bounds = { X = 154, Y = 140, Width = 1900, Height = 1061 },
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