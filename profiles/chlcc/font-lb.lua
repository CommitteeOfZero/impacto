root.Fonts = {
    ["Default"] = {
        Type = FontType.LanguageBarrier,
        ForegroundSheet = "FontLBForeground",
        OutlineSheet = "FontLBOutline",
        ForegroundGridSize = { X = 64, Y = 39 },
        AdvanceWidths = "games/chlcc/font-lb/widths.bin",
        AdvanceWidthsEmWidth = 60,
        ForegroundOffset = { X = -7, Y = 0 },
        OutlineOffset = { X = -6, Y = 1 },
    }
};

root.SpriteSheets["Font"] = nil;

root.SpriteSheets["FontLBForeground"] = {
    Path = "games/chlcc/font-lb/foreground.png",
    DesignWidth = 4096,
    DesignHeight = 2496
};

root.SpriteSheets["FontLBOutline"] = {
    Path = "games/chlcc/font-lb/outline.png",
    DesignWidth = 4096,
    DesignHeight = 2496
};