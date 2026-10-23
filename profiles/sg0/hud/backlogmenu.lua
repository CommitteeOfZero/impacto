local sheet = "Backlog";

local function sprite(name, x, y, w, h)
    root.Sprites[name] = { Sheet = sheet, Bounds = { X = x, Y = y, Width = w, Height = h } };
    return name;
end

root.BacklogMenu = {
    Type = BacklogMenuType.SG0,
    DrawType = DrawComponentType.SystemMenu,

    BacklogBackgroundSprite = sprite("BacklogPanel", 160, 0, 1668, 1080),
    PanelPos = { X = 126, Y = 0 },
    ShadeOpacity = 0.6,

    EntryHighlightLocation = EntryHighlightLocationType.None,
    EntryHighlightSprite = "BacklogPanel",
    EntryHighlightOffset = { X = 0, Y = 0 },
    EntryHighlightPadding = 8.0,
    HighlightBounds = { X = 176, Y = 0, Width = 1568, Height = 0 },
    HighlightColor = 0xFFA040,
    HighlightOpacity = 0.16,

    VoiceIconSprite = sprite("BacklogVoiceIcon", 1928, 116, 60, 48),
    VoiceIconOffset = { X = -14, Y = 2 },

    ScrollbarTrackSprite = sprite("BacklogScrollbarTrack", 1924, 1084, 28, 1084),
    ScrollbarThumbSprite = sprite("BacklogScrollbarThumb", 1932, 8, 28, 92),
    ScrollbarPosition = { X = 1808, Y = -2 },
    ScrollbarThumbLength = 92,

    -- Offset by text mode 9's window position (243 px).
    EntriesStart = { X = 57, Y = 60 },
    RenderingBounds = { X = 126, Y = 40, Width = 1668, Height = 1000 },
    HoverBounds = { X = 230, Y = 40, Width = 1500, Height = 1000 },
    EntryYPadding = 24,

    FadeInDuration = 16 / 60,
    FadeOutDuration = 16 / 60,
    ScrollingSpeed = 900,
    PageUpDownHeight = 900
};
