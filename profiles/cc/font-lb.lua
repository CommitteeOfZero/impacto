root.Fonts = {
    ["Default"] = {
        Type = FontType.LanguageBarrier,
        ForegroundSheet = "FontLBForeground",
        OutlineSheet = "FontLBOutline",
        ForegroundGridSize = { X = 64, Y = 125 },
        AdvanceWidthsBinary = {
            Path = "games/cc/font-lb/widths.bin",
            BytesPerGlyph = 1,
            EmWidth = 48,
        },
        OutlineOffset = { X = -4, Y = -4 },
    }
};

root.SpriteSheets["Font"] = nil;

root.SpriteSheets["FontLBForeground"] = {
    Path = "games/cc/font-lb/foreground.png",
    DesignWidth = 3072,
    DesignHeight = 6000
};

root.SpriteSheets["FontLBOutline"] = {
    Path = "games/cc/font-lb/outline.png",
    DesignWidth = 3648,
    DesignHeight = 7128
};