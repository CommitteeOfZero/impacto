root.Fonts = {
    ["Default"] = {
        Type = FontType.EdgeDetectedSingleSheet,
        Sheet = "Font",
        GridSize = { X = 64, Y = 117 },
        AdvanceWidthsBinary = {
            Path = { Mount = "system", Id = 10 },
            BytesPerGlyph = 2,
            EmWidth = 32,
        },
    },

    ["Default2"] = {
        Type = FontType.EdgeDetectedSingleSheet,
        Sheet = "Font2",
        GridSize = { X = 64, Y = 117 },
        AdvanceWidthsBinary = {
            Path = { Mount = "system", Id = 12 },
            BytesPerGlyph = 2,
            EmWidth = 32,
        },
    },

    ["LanguageBarrier"] = {
        Type = FontType.LanguageBarrier,
        ForegroundSheet = "FontLBForeground",
        OutlineSheet = "FontLBOutline",
        ForegroundGridSize = { X = 64, Y = 125 },
        AdvanceWidthsBinary = {
            Path = "games/cclcc/font-lb/widths.bin",
            BytesPerGlyph = 1,
            EmWidth = 48,
        },
        ForegroundOffset = { X = -13, Y = -13 },
        OutlineOffset = { X = -13, Y = -13 },
        BitmapEmWidth = 48,
        BitmapEmHeight = 48,
        ForegroundOpacityCurve = OpacityCurve.Smoothstep,
        OutlineOpacityCurve = OpacityCurve.Smoothstep,
    }
};

root.SpriteSheets["FontLBForeground"] = {
    Path = "games/cclcc/font-lb/foreground.png",
    DesignWidth = 4096,
    DesignHeight = 8000
};

root.SpriteSheets["FontLBOutline"] = {
    Path = "games/cclcc/font-lb/outline.png",
    DesignWidth = 4096,
    DesignHeight = 8000
};
