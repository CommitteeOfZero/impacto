root.Fonts = {
    ["Default"] = {
        Type = FontType.LanguageBarrier,
        ForegroundSheet = "FontLBForeground",
        OutlineSheet = "FontLBOutline",
        ForegroundGridSize = { X = 64, Y = 50 },
        AdvanceWidthsBinary = {
            Path = "games/rne/font-lb/widths.bin",
            BytesPerGlyph = 1,
            EmWidth = 48,
        },
        OutlineOffset = { X = -4, Y = -4 },
    }
};

root.SpriteSheets["Font"] = nil;

root.SpriteSheets["FontLBForeground"] = {
    Path = "games/rne/font-lb/foreground.png",
    DesignWidth = 3072,
    DesignHeight = 2400
};

root.SpriteSheets["FontLBOutline"] = {
    Path = "games/rne/font-lb/outline.png",
    DesignWidth = 3648,
    DesignHeight = 2852
};