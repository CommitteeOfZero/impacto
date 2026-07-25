root.Fonts = {
    ["Default"] = {
        Type = FontType.LanguageBarrier,
        ForegroundSheet = "FontLBForeground",
        OutlineSheet = "FontLBOutline",
        ForegroundGridSize = { X = 64, Y = 125 },
        AdvanceWidths = "games/cclcc/font-lb-italic/widths.bin",
        AdvanceWidthsEmWidth = 48,
        ForegroundOffset = { X = -13, Y = -13 },
        OutlineOffset = { X = -13, Y = -13 },
        BitmapEmWidth = 48,
        BitmapEmHeight = 48,
    }
};

root.SpriteSheets["Font"] = nil;

root.SpriteSheets["FontLBForeground"] = {
    Path = "games/cclcc/font-lb-italic/foreground.png",
    DesignWidth = 4096,
    DesignHeight = 8000
};

root.SpriteSheets["FontLBOutline"] = {
    Path = "games/cclcc/font-lb-italic/outline.png",
    DesignWidth = 4096,
    DesignHeight = 8000
};
